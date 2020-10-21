//###########################################################################
// This file is part of LImA, a Library for Image Acquisition
//
// Copyright (C) : 2009-2020
// European Synchrotron Radiation Facility
// BP 220, Grenoble 38043
// FRANCE
//
// This is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This software is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//###########################################################################
/****************************************************************************************************
 * \file   CameraControl.cpp
 * \brief  implementation file of detector communication class.
 * \author Cédric Castel - SOLEIL (MEDIANE SYSTEME - IT consultant) 
 * \date   Created on October 19, 2020
 ****************************************************************************************************/

// PROJECT
#include "CameraControl.h"

// SYSTEM
#include <netinet/tcp.h>
#include <netdb.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <errno.h>  
#include <sys/time.h>

// LIMA
#include "lima/Exceptions.h"
#include "lima/Debug.h"

using namespace lima;
using namespace lima::SpectralDetector_ns;

#define SPECTRAL_CAMERA_CONTROL_ACTIVATE_TRAME_TRACE

/****************************************************************************************************
 * \fn CameraControl()
 * \brief  constructor
 * \param  none
 * \return none
 ****************************************************************************************************/
CameraControl::CameraControl()
{
    DEB_CONSTRUCTOR();

    m_connection_timeout_sec = 0;
    m_is_connected = false;

	// Ignore the sigpipe we get we try to send quit to
	// dead server in disconnect, just use error codes
	struct sigaction pipe_act;

	sigemptyset(&pipe_act.sa_mask);

    pipe_act.sa_flags   = 0      ;
	pipe_act.sa_handler = SIG_IGN;

    sigaction(SIGPIPE, &pipe_act, 0);

	m_sock = -1;
    memset(&m_server_name, 0, sizeof(struct sockaddr_in));
}

/****************************************************************************************************
 * \fn ~CameraControl()
 * \brief  destructor (needs to be virtual)
 * \param  none
 * \return none
 ****************************************************************************************************/
CameraControl::~CameraControl() 
{
    DEB_DESTRUCTOR();

    disconnect();
}

/****************************************************************************************************
 * \fn void SetConnectionTimeout(int in_connection_timeout_sec)
 * \brief  configure the connection timeout in seconds
 * \param  in_connection_timeout_sec new connection timeout in seconds
 * \return none
 ****************************************************************************************************/
void CameraControl::SetConnectionTimeout(int in_connection_timeout_sec)
{
    m_connection_timeout_sec = in_connection_timeout_sec;
}

/****************************************************************************************************
 * \fn void SetCameraIdentifier(int in_camera_identifier)
 * \brief  configure the camera identifier
 * \param  in_camera_identifier camera identifier
 * \return none
 ****************************************************************************************************/
void CameraControl::SetCameraIdentifier(int in_camera_identifier)
{
    m_camera_identifier = in_camera_identifier;
}

/****************************************************************************************************
 * \fn bool not_blocking_connect(struct sockaddr_in & in_out_sa, int in_sock, int in_timeout)
 * \brief  execute a not blocking connect
 * \param  in_out_sa connection host
 * \param  in_sock socket to connect 
 * \param  in_timeout connection delay in seconds
 * \return return false on a system call error, true on success
 ****************************************************************************************************/
bool CameraControl::not_blocking_connect(struct sockaddr_in & in_out_sa, int in_sock, int in_timeout)
{   
    DEB_MEMBER_FUNCT();

    int            flags = 0;
    int            error = 0;
    int            ret   = 0;
    fd_set         rset  ;
    fd_set         wset  ;
    socklen_t      len   = sizeof(error);
    struct timeval ts    ;
    
    ts.tv_sec  = in_timeout;
    ts.tv_usec = 0;
    
    // clear out descriptor sets for select
    // add socket to the descriptor sets
    FD_ZERO(&rset);
    FD_SET(in_sock, &rset);
    wset = rset;    //structure assignment ok
    
    // set socket nonblocking flag
    if( (flags = fcntl(in_sock, F_GETFL, 0)) < 0)
    {
        DEB_ERROR() << "set socket nonblocking flag failed!";
        return false;
    }
    
    if(fcntl(in_sock, F_SETFL, flags | O_NONBLOCK) < 0)
    {
        DEB_ERROR() << "set socket nonblocking flag failed!";
        return false;
    }
    
    // initiate non-blocking connect
    // the connect call is on the global namespace (::) to avoid a compile error.
    if((ret = ::connect(in_sock, (struct sockaddr *)&in_out_sa, sizeof(struct sockaddr_in))) < 0)
    {
        DEB_ERROR() << "connect failed!";
        if (errno != EINPROGRESS)
            return false;
    }
    else
    // then connect succeeded right away
    if(ret == 0)    
    {
        goto done;
    }
    
    // we are waiting for connect to complete now
    if((ret = select(in_sock + 1, &rset, &wset, NULL, (in_timeout) ? &ts : NULL)) < 0)
    {
        DEB_ERROR() << "select failed!";
        return false;
    }
    else
    // we had a timeout
    if(ret == 0)
    {   
        DEB_ERROR() << "timeout during non-blocking connection!";
        errno = ETIMEDOUT;
        return false;
    }

    // we had a positivite return so a descriptor is ready
    if (FD_ISSET(in_sock, &rset) || FD_ISSET(in_sock, &wset))
    {
        if(getsockopt(in_sock, SOL_SOCKET, SO_ERROR, &error, &len) < 0)
        {
            DEB_ERROR() << "getsockopt failed!";
            return false;
        }
    }
    else
    {
        DEB_ERROR() << "FD_ISSET failed!";
        return false;
    }

    // check if we had a socket error
    if(error)
    {  
        DEB_ERROR() << "socket error occured: " << error << ": " << strerror(error);
        errno = error;
        return false;
    }

done:
    // put socket back in blocking mode
    if(fcntl(in_sock, F_SETFL, flags) < 0)
    {
        DEB_ERROR() << "set socket blocking flag failed!";
        return false;
    }
    else
    {
        return true;
    }
}

/****************************************************************************************************
 * \fn void connect(const std::string & in_hostname, int in_port) 
 * \brief  connect the instance to the detector software (tcp/ip)
 * \param  in_hostname name of detector software server or ip address
 * \param  in_port detector software tcp/ip port 
 * \return none
 ****************************************************************************************************/
void CameraControl::connect(const std::string & in_hostname, int in_port) 
{
    DEB_MEMBER_FUNCT();

    DEB_TRACE() << "Trying a Server connection to: " << in_hostname << ":" << in_port;

    struct hostent  * hostPtr ;
	struct protoent * protocol;

    if (m_is_connected) 
    {
        std::ostringstream MsgErr;
        MsgErr << "Already connected";
        DEB_ERROR() << MsgErr;
        THROW_HW_ERROR(Error) << MsgErr;
	}

    // creating the socket
    m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (m_sock < 0)
    {
        std::ostringstream MsgErr;
        MsgErr << "Could not create the socket";
        DEB_ERROR() << MsgErr;
        THROW_HW_ERROR(Error) << MsgErr;
    }

    // geting host address
    hostPtr = gethostbyname(in_hostname.c_str());
    
    if (hostPtr == NULL)
    {
		endhostent();
		close(m_sock);

        std::ostringstream MsgErr;
        MsgErr << "Can't get gethostbyname : " << in_hostname;
        DEB_ERROR() << MsgErr;
        THROW_HW_ERROR(Error) << MsgErr;
    }

    // filling the sockaddr structure
    m_server_name.sin_family = AF_INET;
    m_server_name.sin_port   = htons(in_port);
    memcpy(&m_server_name.sin_addr.s_addr, hostPtr->h_addr, hostPtr->h_length);
	endhostent();

    // connect try
    if(!not_blocking_connect(m_server_name, m_sock, m_connection_timeout_sec))
    {
		close(m_sock);

        std::ostringstream MsgErr;
        MsgErr << "Connection to server refused. Is the server running?";
        DEB_ERROR() << MsgErr;
        THROW_HW_ERROR(Error) << MsgErr;
    }

    // setting socket options
    protocol = getprotobyname("tcp");

    if (protocol == 0) 
    {
        std::ostringstream MsgErr;
        MsgErr << "Can't get protocol TCP";
        DEB_ERROR() << MsgErr;
        THROW_HW_ERROR(Error) << MsgErr;
    } 
    else 
    {
	    int opt = 1;
	
        if (setsockopt(m_sock, protocol->p_proto, TCP_NODELAY, (char *) &opt, 4) < 0) 
        {
            endprotoent();

            std::ostringstream MsgErr;
            MsgErr << "Can't set socket options";
            DEB_ERROR() << MsgErr;
            THROW_HW_ERROR(Error) << MsgErr;
	    }
    }

    endprotoent();

    DEB_TRACE() << "Connected to server: " << in_hostname << ":" << in_port;

    m_is_connected = true;
}

/****************************************************************************************************
 * \fn void disconnect(void)
 * \brief  disconnect the instance of the detector software
 * \param  in_hostname name of detector software server or ip address
 * \param  in_port detector software tcp/ip port 
 * \return none
 ****************************************************************************************************/
void CameraControl::disconnect(void) 
{
    DEB_MEMBER_FUNCT();

    if (m_is_connected) 
    {
        DEB_TRACE() << "Disconnection to server";

        shutdown(m_sock, 2); // 2 for stopping both reception and transmission
		close   (m_sock   );

		m_is_connected = false;
	}
}

/****************************************************************************************************
 * \fn bool send(const std::string & in_command_name, const std::vector<uint8_t> & in_net_buffer, int32_t & out_error)
 * \brief  send a tcp/ip packet
 * \param  in_command_name command name used for logging
 * \param  in_net_buffer   packet buffer
 * \param  out_error       error code
 * \return true if succeed, false in case of error
 ****************************************************************************************************/
bool CameraControl::send(const std::string          & in_command_name,
                         const std::vector<uint8_t> & in_net_buffer  ,
                         int32_t                    & out_error      )
{
    DEB_MEMBER_FUNCT();

    int n;

    // no error by default
    out_error = 0;

    n = ::send(m_sock, in_net_buffer.data(), in_net_buffer.size(), 0);

#ifdef SPECTRAL_CAMERA_CONTROL_ACTIVATE_TRAME_TRACE
    DEB_TRACE() << "CameraControl::send(): " << n << " bytes sent: " << in_command_name;
#endif

    if(n < 0)
    {
        DEB_ERROR() << "CameraControl::send(): write to socket error";
        out_error = 1;
    }

    return (n >= 0);
}

/****************************************************************************************************
 * \fn bool sendCommand(NetCommandHeader * in_out_command, int32_t & out_error)
 * \brief  Send a command to the detector
 * \param  in_out_command command to send
 * \param  out_error      error code
 * \return true if succeed, false in case of error
 ****************************************************************************************************/
bool CameraControl::sendCommand(NetCommandHeader * in_out_command, int32_t & out_error)
{
    DEB_MEMBER_FUNCT();

    // init command members
    in_out_command->initPacketLenght();
    in_out_command->initCameraIdentifier(m_camera_identifier);
    in_out_command->initSpecificDataLenght();

    // fill a packet buffer
    std::vector<uint8_t> net_buffer;
    net_buffer.resize(in_out_command->totalSize(), 0);

    uint8_t *   memory_data = net_buffer.data();
    std::size_t memory_size = net_buffer.size();

    if(!in_out_command->totalWrite(memory_data, memory_size))
        return false;

#ifdef SPECTRAL_CAMERA_CONTROL_ACTIVATE_TRAME_TRACE
    DEB_TRACE() << "Sending command (" << in_out_command->m_function_number << "): " << in_out_command->m_packet_name;
#endif

    return send(in_out_command->m_packet_name, net_buffer, out_error);
}

/****************************************************************************************************
 * \fn bool receive(uint8_t * out_buffer, const int in_buffer_lenght, int32_t & out_error)
 * \brief  Receive a tcp/ip packet
 * \param  out_buffer       receive buffer
 * \param  in_buffer_lenght receive buffer max size
 * \param  out_error        error code
 * \return true if succeed, false in case of error
 ****************************************************************************************************/
bool CameraControl::receive(uint8_t * out_buffer, const int in_buffer_lenght, int32_t & out_error)
{
    DEB_MEMBER_FUNCT();

    // no error by default
    out_error = 0;

    int current_answer_lenght = 0;

    // first, receiving the header
    // read, until the expected answer length could be read
    // the answer could be fragmented into several packets
    for(;;) 
    {
        int read_max = in_buffer_lenght - current_answer_lenght; // remaining size to read
        int n        = recv(m_sock, out_buffer + current_answer_lenght, read_max, 0);

        // Server returned error code ?
        if (n <= 0)
        {
            DEB_ERROR() << "CameraControl::receive(): Could not receive answer.";
            out_error = 1;
            return false;
        }

        current_answer_lenght += n;
        
        // complete data was read, so we can leave the loop
        if (current_answer_lenght == in_buffer_lenght)
        {
            break;
        }
    }

    return true;
}

/****************************************************************************************************
 * \fn bool receiveSubPacket(NetGenericHeader * out_packet, std::vector<uint8_t> & in_out_net_buffer, int32_t & out_error)
 * \brief  Receive a SI Image SGL II sub packet (a part of the complete packet)
 * \param  out_packet        sub packet to receive
 * \param  in_out_net_buffer packet buffer (will grow during the process because data will be concatenated)
 * \param  out_error         error code
 * \return true if succeed, false in case of error
 ****************************************************************************************************/
bool CameraControl::receiveSubPacket(NetGenericHeader * out_packet, std::vector<uint8_t> & in_out_net_buffer, int32_t & out_error)
{
    DEB_MEMBER_FUNCT();

    const std::size_t previous_size = in_out_net_buffer.size();

    // increasing the buffer size
    in_out_net_buffer.resize(out_packet->totalSize(), 0);

    if(!receive(in_out_net_buffer.data() + previous_size, out_packet->size(), out_error))
        return false;

    // filling the buffer data into the header members
    const uint8_t * memory_data = in_out_net_buffer.data() + previous_size;
    std::size_t     memory_size = out_packet->size();

    if(!out_packet->read(memory_data, memory_size))
    {
        DEB_ERROR() << "CameraControl::receiveSubPacket - Error during the buffer copy into the sub packet!";
        return false;
    }
}

/****************************************************************************************************
 * \fn bool receiveGenericSubPacket(const NetGenericHeader & in_packet, NetGenericHeader * out_packet, std::vector<uint8_t> & in_out_net_buffer, bool in_final_packet, int32_t & out_error)
 * \brief  Receive a SI Image SGL II sub packet (a part of the complete packet but not the first one)
 * \param  in_packet         already received sub packet
 * \param  out_packet        generic sub packet to receive 
 * \param  in_out_net_buffer packet buffer (will grow during the process because data will be concatenated)
 * \param  in_final_packet   is the packet to receive the last one ?
 * \param  out_error         error code
 * \return true if succeed, false in case of error
 ****************************************************************************************************/
bool CameraControl::receiveGenericSubPacket(const NetGenericHeader     & in_packet        ,
                                            NetGenericHeader           * out_packet       , 
                                            std::vector<uint8_t>       & in_out_net_buffer,
                                            bool                         in_final_packet  ,       
                                            int32_t                    & out_error        )
{
    DEB_MEMBER_FUNCT();

    // checking the packet lenght
    if(((in_final_packet ) && (in_packet.m_packet_lenght != out_packet->totalSize())) || 
       ((!in_final_packet) && (in_packet.m_packet_lenght  < out_packet->totalSize())))
    {
        DEB_ERROR() << "CameraControl::receiveGenericSubPacket - Incoherent packet lenght for "
                    << out_packet->m_packet_name << " packet.";
        return false;
    }

    return receiveSubPacket(out_packet, in_out_net_buffer, out_error);
}

/****************************************************************************************************
 * \fn bool receiveSpecificSubPacket(const NetGenericHeader & in_packet, const NetGenericAnswer & in_answer_packet, NetGenericHeader * out_packet, std::vector<uint8_t> & in_out_net_buffer, int32_t & out_error)
 * \brief  Receive a SI Image SGL II sub packet with a specific lenght
 * \param  in_packet         already received sub packet
 * \param  in_answer_packet  already received answer sub packet
 * \param  out_packet        generic sub packet to receive 
 * \param  in_out_net_buffer packet buffer (will grow during the process because data will be concatenated)
 * \param  out_error         error code
 * \return true if succeed, false in case of error
 ****************************************************************************************************/
bool CameraControl::receiveSpecificSubPacket(const NetGenericHeader & in_packet        ,
                                             const NetGenericAnswer & in_answer_packet ,
                                             NetGenericHeader       * out_packet       , 
                                             std::vector<uint8_t>   & in_out_net_buffer,
                                             int32_t                & out_error        )
{
    DEB_MEMBER_FUNCT();

    const std::size_t previous_size = in_out_net_buffer.size();

    // increasing the buffer size
    in_out_net_buffer.resize(previous_size + in_answer_packet.m_specific_data_lenght, 0);

    if(!receive(in_out_net_buffer.data() + previous_size, in_answer_packet.m_specific_data_lenght, out_error))
        return false;

    // filling the buffer data into the header members
    const uint8_t * memory_data = in_out_net_buffer.data() + previous_size;
    std::size_t     memory_size = in_answer_packet.m_specific_data_lenght;

    if(!out_packet->read(memory_data, memory_size))
    {
        DEB_ERROR() << "CameraControl::receiveSubPacket - Error during the buffer copy into the sub packet!";
        return false;
    }

    return true;
}

/****************************************************************************************************
 * \fn bool FillFullPacket(NetGenericHeader * out_packet, const std::vector<uint8_t> & in_net_buffer, int32_t & out_error)
 * \brief  Fill the final packet with the data stored in the buffer
 * \param  out_packet        generic sub packet to be filled
 * \param  in_net_buffer     packet buffer with the data to copy
 * \param  out_error         error code
 * \return true if succeed, false in case of error
 ****************************************************************************************************/
bool CameraControl::FillFullPacket(NetGenericHeader           * out_packet   , 
                                   const std::vector<uint8_t> & in_net_buffer,
                                   int32_t                    & out_error    )
{
    DEB_MEMBER_FUNCT();

    bool result = true;

    // filling the buffer data into the class members
    const uint8_t * memory_data = in_net_buffer.data();
    std::size_t     memory_size = in_net_buffer.size();

    if(!out_packet->totalRead(memory_data, memory_size))
    {
        DEB_ERROR() << "CameraControl::receivePacket - Error during the buffer copy into the "
                    << out_packet->m_packet_name << " packet!";
        out_error = 1;
        result = false;
    }
    
    return result;
}

/****************************************************************************************************
 * \fn bool receivePacket(NetGenericHeader * & out_packet, int32_t & out_error)
 * \brief  Receive a SI Image SGL II packet
 * \param  out_packet receive packet
 * \param  out_error  error code
 * \return true if succeed, false in case of error
 ****************************************************************************************************/
bool CameraControl::receivePacket(NetGenericHeader * & out_packet, int32_t & out_error)
{
    DEB_MEMBER_FUNCT();

    out_packet = NULL;

    // at start, we do not know the kind of packet.
    // we can only receive the generic header to determine the packet type.
    NetGenericHeader header;
    std::vector<uint8_t> net_buffer;

    if(!receiveSubPacket(&header, net_buffer, out_error))
        return false;

    // checking the camera identifier
    if((header.m_camera_identifier != NetCommandHeader::g_server_command_identifier) &&
       (header.m_camera_identifier != m_camera_identifier))
    {
        DEB_ERROR() << "CameraControl::receivePacket - Incorrect camera indentifier found into the packet header!";
        return false;
    }

    // checking the packet identifier to determine the next data block to read
    // check if this is a command packet
    if(header.isCommandPacket())
    {
        DEB_ERROR() << "CameraControl::receivePacket - A command packet can not be received!";
        return false;
    }
    else
    // check if this is a acknowledge packet
    if(header.isAcknowledgePacket())
    {
        NetAcknowledge ack;

        if(!receiveGenericSubPacket(header, &ack,  net_buffer, true, out_error))
            return false;

        // it's ok, we can "return" the packet
        out_packet = new NetAcknowledge();

        if(!FillFullPacket(out_packet, net_buffer, out_error))
        {
            delete out_packet;
            out_packet = NULL;
            return false;
        }
    }
    else
    // check if this is a data packet
    if(header.isDataPacket())
    {
        NetGenericAnswer answer;

        if(!receiveGenericSubPacket(header, &answer,  net_buffer, false, out_error))
            return false;

        // checking the error code
        if(answer.m_error_code)
        {
        #ifdef SPECTRAL_CAMERA_CONTROL_ACTIVATE_TRAME_TRACE
            DEB_TRACE() << "received an error into the " << out_packet->m_packet_name << " packet.";
        #endif

            out_error = answer.m_error_code;
            return false;
        }

        // checking the data type to determine the final class to use
        if(answer.m_data_type == NetGenericAnswer::g_data_type_get_status)
        {
            NetAnswerGetStatus get_status;

            if(!receiveSpecificSubPacket(header, answer, &get_status,  net_buffer, out_error))
                return false;

            // it's ok, we can "return" the packet
            out_packet = new NetAnswerGetStatus();

            if(!FillFullPacket(out_packet, net_buffer, out_error))
            {
                delete out_packet;
                out_packet = NULL;
                return false;
            }
        }
        else
        {
            DEB_ERROR() << "CameraControl::receivePacket - Unknown data type!";
            return false;
        }
    }
    else
    // check if this is an image packet
    if(header.isImagePacket())
    {
        DEB_ERROR() << "CameraControl::receivePacket - Not managed packet type!";
        return false;
    }
    else
    {
        DEB_ERROR() << "CameraControl::receivePacket - Unknown packet type!";
        return false;
    }
    
    #ifdef SPECTRAL_CAMERA_CONTROL_ACTIVATE_TRAME_TRACE
        DEB_TRACE() << "received " << out_packet->m_packet_name << " packet.";
    #endif

    return true;
}

/**************************************************************************************************
 * COMMANDS MANAGEMENT
 **************************************************************************************************/
/****************************************************************************************************
 * \fn bool CameraControl::getStatus()
 * \brief  Read the current status
 * \param  none
 * \return true if succeed, false in case of error
 ****************************************************************************************************/
bool CameraControl::getStatus()
{
    DEB_MEMBER_FUNCT();

    int32_t error  = 0    ;
    bool    result = false;
    NetGenericHeader * first_packet  = NULL;
    NetGenericHeader * second_packet = NULL;
    NetCommandHeader * command       = new NetCommandGetStatus();
    NetGenericAnswer * answer_packet = NULL;
    NetAcknowledge   * ack_packet    = NULL;

    // Send a command to the detector
    if(!sendCommand(command, error))
        goto done;

    // wait for an acknowledge
    if(!receivePacket(first_packet, error))
        goto done;

    // check if this is a acknowledge packet
    if(!first_packet->isAcknowledgePacket())
        goto done;

    ack_packet = dynamic_cast<NetAcknowledge *>(first_packet);

    // check if the command was accepted
    if(!(ack_packet->m_accepted_flag))
    {
    #ifdef SPECTRAL_CAMERA_CONTROL_ACTIVATE_TRAME_TRACE
        DEB_TRACE() << "Command " << command->m_packet_name << " was not accepted!";
    #endif
        goto done;
    }

    // wait for the status
    if(!receivePacket(second_packet, error))
        goto done;

    // check if this is a data packet
    if(!second_packet->isDataPacket())
        goto done;

    answer_packet = dynamic_cast<NetGenericAnswer *>(second_packet);

    if(answer_packet->m_data_type == NetGenericAnswer::g_data_type_get_status)
    {
        NetAnswerGetStatus * status_packet = dynamic_cast<NetAnswerGetStatus *>(answer_packet);
        DEB_TRACE() << "!!!!!!! received status:" << status_packet->m_status;
    }

    result = true;

done:
//    if(!result)
    {
        if(first_packet  == NULL) delete first_packet ;
        if(second_packet == NULL) delete second_packet;
        if(command       == NULL) delete command      ;
    }

    return result;
}

/**************************************************************************************************
 * SINGLETON MANAGEMENT
 **************************************************************************************************/
/****************************************************************************************************
 * \fn void create()
 * \brief  Create the singleton instance
 * \param  none
 * \return none
 ****************************************************************************************************/
void CameraControl::create()
{
    CameraControl::g_singleton = new CameraControl();
}

//###########################################################################