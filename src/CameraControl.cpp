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
 * \author C�dric Castel - SOLEIL (MEDIANE SYSTEME - IT consultant) 
 * \date   Created on October 19, 2020
 ****************************************************************************************************/

// PROJECT
#include "CameraControl.h"
#include "CameraReceiveDataThread.h"

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
#include <sstream>

// LIMA
#include "lima/Exceptions.h"
#include "lima/Debug.h"

using namespace lima;
using namespace lima::Spectral;

// #define SPECTRAL_CAMERA_CONTROL_ACTIVATE_NETWORK_TRACE
// #define SPECTRAL_CAMERA_CONTROL_ACTIVATE_PACKET_TRACE
// #define SPECTRAL_CAMERA_CONTROL_ACTIVATE_LIGHT_PACKET_TRACE

//------------------------------------------------------------------
// CameraControl class
//------------------------------------------------------------------
/****************************************************************************************************
 * \fn CameraControl(const CameraControlInit & in_init_parameters)
 * \brief  constructor
 * \param  in_init_parameters init parameters (timeout, delays, ...)
 * \return none
 ****************************************************************************************************/
CameraControl::CameraControl(const CameraControlInit & in_init_parameters)
{
    DEB_CONSTRUCTOR();

    // binary copy of init parameters (works because there is no pointers)
    m_init_parameters = in_init_parameters;

    // changing all the groups'timeout delays...
    m_packets_container.setDelayBeforeTimeoutSec(m_init_parameters.m_wait_packet_timeout_sec);

    // default values
    m_is_connected  = false;
    m_latest_status = DetectorStatus::Ready;
    
    m_model         = "Unknown Model"        ;
    m_serial_number = "Unknown Serial Number";

    m_width_max   = 0;
    m_height_max  = 0;
    m_pixel_depth = 0;

    m_exposure_time_msec   = 0; 
    m_nb_images_to_acquire = 0;
    m_serial_origin        = 0; 
    m_serial_length        = 0; 
    m_serial_binning       = 0; 
    m_parallel_origin      = 0; 
    m_parallel_length      = 0; 
    m_parallel_binning     = 0; 

    m_acquisition_type = NetAnswerGetSettings::AcquisitionType::Light      ;
    m_acquisition_mode = NetAnswerGetSettings::AcquisitionMode::SingleImage;

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
 * \fn void computeTimeoutForAcquireCommand()
 * \brief  configure the wait timeout in seconds for the acquire command execution
 * \param  none
 * \return none
 ****************************************************************************************************/
void CameraControl::computeTimeoutForAcquireCommand()
{
    int wait_packet_timeout_sec = m_init_parameters.m_maximum_readout_time_sec + static_cast<int>(m_exposure_time_msec / 1000);
    m_packets_container.setDelayBeforeTimeoutSec(NetCommandHeader::g_function_number_acquire, wait_packet_timeout_sec);
}

/****************************************************************************************************
 * \fn int getDelayToCheckAcqEndMsec() const
 * \brief  Get the delay in milli-seconds between two tries to check if the acquisition is finished
 * \param  none
 * \return delay in milli-seconds between two tries to check if the acquisition is finished
 ****************************************************************************************************/
int CameraControl::getDelayToCheckAcqEndMsec() const
{
    return m_init_parameters.m_delay_to_check_acq_end_msec;
}

/****************************************************************************************************
 * \fn int getInquireAcqStatusDelayMsec() const
 * \brief  Get the delay in milli-seconds between two sends of inquire status commands
 * \param  none
 * \return delay in milli-seconds between two sends of inquire status commands
 ****************************************************************************************************/
int CameraControl::getInquireAcqStatusDelayMsec() const
{
    return m_init_parameters.m_inquire_acq_status_delay_msec;
}

/****************************************************************************************************
 * \fn void DetectorStatus getLatestStatus() const
 * \brief  get the latest hardware status
 * \param  none
 * \return latest status
 ****************************************************************************************************/
CameraControl::DetectorStatus CameraControl::getLatestStatus() const
{
    return m_latest_status;
}

/****************************************************************************************************
 * \fn const std::string & getModel() const
 * \brief  get the detector model
 * \param  none
 * \return detector model
 ****************************************************************************************************/
const std::string & CameraControl::getModel() const
{
    return m_model;
}

/****************************************************************************************************
 * \fn const std::string & getSerialNumber() const
 * \brief  get the detector serial number
 * \param  none
 * \return detector serial number
 ****************************************************************************************************/
const std::string & CameraControl::getSerialNumber() const
{
    return m_serial_number;
}

/****************************************************************************************************
 * \fn std::size_t getWidthMax() const
 * \brief  get the detector maximum width
 * \param  none
 * \return detector maximum width
 ****************************************************************************************************/
std::size_t CameraControl::getWidthMax() const
{
    return m_width_max;
}

/****************************************************************************************************
 * \fn std::size_t getHeightMax() const
 * \brief  get the detector maximum height
 * \param  none
 * \return detector maximum height
 ****************************************************************************************************/
std::size_t CameraControl::getHeightMax() const
{
    return m_height_max;
}

/****************************************************************************************************
 * \fn std::size_t getPixelDepth() const
 * \brief  get the detector pixel depth
 * \param  none
 * \return detector pixel depth
 ****************************************************************************************************/
std::size_t CameraControl::getPixelDepth() const
{
    return m_pixel_depth;
}

/****************************************************************************************************
 * \fn uint32_t getExposureTimeMsec() const
 * \brief  get the exposure time in milli-seconds
 * \param  none
 * \return exposure time in milli-seconds
 ****************************************************************************************************/
uint32_t CameraControl::getExposureTimeMsec() const
{
    return m_exposure_time_msec;
}

/****************************************************************************************************
 * \fn uint32_t getNbImagesToAcquire() const
 * \brief  get the number of frames to acquire
 * \param  none
 * \return number of frames to acquire
 ****************************************************************************************************/
uint32_t CameraControl::getNbImagesToAcquire() const
{
    return m_nb_images_to_acquire;
}

/****************************************************************************************************
 * \fn NetAnswerGetSettings::AcquisitionType getAcquisitionType() const
 * \brief  get the acquisition type
 * \param  none
 * \return acquisition type
 ****************************************************************************************************/
NetAnswerGetSettings::AcquisitionType CameraControl::getAcquisitionType() const
{
    return m_acquisition_type;
}

/****************************************************************************************************
 * \fn NetAnswerGetSettings::AcquisitionType getAcquisitionMode() const
 * \brief  get the acquisition mode
 * \param  none
 * \return acquisition mode
 ****************************************************************************************************/
NetAnswerGetSettings::AcquisitionMode CameraControl::getAcquisitionMode() const
{
    return m_acquisition_mode;
}

/****************************************************************************************************
 * \fn std::size_t getSerialOrigin() const
 * \brief  get the CCD Format Serial Origin
 * \param  none
 * \return CCD Format Serial Origin
 ****************************************************************************************************/
std::size_t CameraControl::getSerialOrigin() const
{
    return m_serial_origin;
}

/****************************************************************************************************
 * \fn std::size_t getSerialLength() const
 * \brief  get the CCD Format Serial Length
 * \param  none
 * \return CCD Format Serial Length
 ****************************************************************************************************/
std::size_t CameraControl::getSerialLength() const
{
    return m_serial_length;
}

/****************************************************************************************************
 * \fn std::size_t getSerialBinning() const
 * \brief  get the CCD Format Serial Binning
 * \param  none
 * \return CCD Format Serial Binning
 ****************************************************************************************************/
std::size_t CameraControl::getSerialBinning() const
{
    return m_serial_binning;
}

/****************************************************************************************************
 * \fn std::size_t getParallelOrigin() const
 * \brief  get the CCD Format Parallel Origin
 * \param  none
 * \return CCD Format Parallel Origin
 ****************************************************************************************************/
std::size_t CameraControl::getParallelOrigin() const
{
    return m_parallel_origin;
}

/****************************************************************************************************
 * \fn std::size_t getParallelLength() const
 * \brief  get the CCD Format Parallel Length
 * \param  none
 * \return CCD Format Parallel Length
 ****************************************************************************************************/
std::size_t CameraControl::getParallelLength() const
{
    return m_parallel_length;
}

/****************************************************************************************************
 * \fn std::size_t getParallelBinning() const
 * \brief  get the CCD Format Parallel Binning
 * \param  none
 * \return CCD Format Parallel Binning
 ****************************************************************************************************/
std::size_t CameraControl::getParallelBinning() const
{
    return m_parallel_binning;
}

/****************************************************************************************************
 * \fn std::string getCCDTemperatureFromCamera() const
 * \brief  get the CCD current temperature
 * \param  none
 * \return CCD current temperature
 ****************************************************************************************************/
float CameraControl::getCCDTemperatureFromCamera() const
{
    return m_ccd_temperature;
}


ushort CameraControl::getReadoutSpeedFromCamera() const
{
    return m_readout_speed_value;
}

/****************************************************************************************************
 * \fn bool notBlockingConnect(struct sockaddr_in & in_out_sa, int in_sock, int in_timeout)
 * \brief  execute a not blocking connect
 * \param  in_out_sa connection host
 * \param  in_sock socket to connect 
 * \param  in_timeout connection delay in seconds
 * \return return false on a system call error, true on success
 ****************************************************************************************************/
bool CameraControl::notBlockingConnect(struct sockaddr_in & in_out_sa, int in_sock, int in_timeout)
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
        if (errno != EINPROGRESS)
        {
            DEB_ERROR() << "connect failed!";
            return false;
        }
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
        DEB_ERROR() << "socket error occurred: " << error << ": " << strerror(error);
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
    if(!notBlockingConnect(m_server_name, m_sock, m_init_parameters.m_connection_timeout_sec))
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

    int opt = 1;

    if (setsockopt(m_sock, protocol->p_proto, TCP_NODELAY, (void *) &opt, sizeof(opt)) < 0) 
    {
        endprotoent();

        std::ostringstream MsgErr;
        MsgErr << "Can't set socket options";
        DEB_ERROR() << MsgErr;
        THROW_HW_ERROR(Error) << MsgErr;
    }

    endprotoent();

    // set the select timeout
    struct timeval time_out;
    time_out.tv_sec  = m_init_parameters.m_reception_timeout_sec;
    time_out.tv_usec = 0;
    
    if(setsockopt(m_sock, SOL_SOCKET, SO_RCVTIMEO, &time_out, sizeof(time_out)) < 0)
    {
        std::ostringstream MsgErr;
        MsgErr << "Can't set timeout socket options";
        DEB_ERROR() << MsgErr;
        THROW_HW_ERROR(Error) << MsgErr;
    }

    DEB_TRACE() << "Connected to server: " << in_hostname << ":" << in_port;

    m_is_connected = true;

    // creating the data reception thread
    CameraReceiveDataThread::create();
    
    // starting the data reception
    CameraReceiveDataThread::startReception();
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

        // Stopping the data reception
        CameraReceiveDataThread::stopReception();

        // Releasing the data reception thread
        CameraReceiveDataThread::release();

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

#ifdef SPECTRAL_CAMERA_CONTROL_ACTIVATE_NETWORK_TRACE
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
 * \fn lima::AutoMutex sendCommandLock() const
 * \brief  creates an autolock mutex for sendCommand access
 * \param  none
 * \return auto mutex
 ****************************************************************************************************/
lima::AutoMutex CameraControl::sendCommandLock() const
{
    return lima::AutoMutex(m_send_command_cond.mutex());
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
    in_out_command->initCameraIdentifier(m_init_parameters.m_camera_identifier);
    in_out_command->initSpecificDataLenght();

#ifdef SPECTRAL_CAMERA_CONTROL_ACTIVATE_PACKET_TRACE
    in_out_command->totalLog();
#endif

    // fill a packet buffer
    std::vector<uint8_t> net_buffer;
    net_buffer.resize(in_out_command->totalSize(), 0);

    uint8_t *   memory_data = net_buffer.data();
    std::size_t memory_size = net_buffer.size();

    if(!in_out_command->totalWrite(memory_data, memory_size))
        return false;

#ifdef SPECTRAL_CAMERA_CONTROL_ACTIVATE_NETWORK_TRACE
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
            if(errno == EAGAIN)
            {      
                DEB_WARNING() << "CameraControl::receive(): TIMEOUT occurred!";
            }
            else
            {
                DEB_ERROR() << "CameraControl::receive(): Could not receive answer.";
            }

            out_error = errno;
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
        DEB_ERROR() << "CameraControl::receiveSpecificSubPacket - Error during the buffer copy into the sub packet!";
        return false;
    }

    return true;
}

/****************************************************************************************************
 * \fn bool receiveImageSubPacket(const NetGenericHeader & in_packet, const NetGenericAnswer & in_answer_packet, NetGenericHeader * out_packet, std::vector<uint8_t> & in_out_net_buffer, int32_t & out_error)
 * \brief  Receive an image part sub packet with a specific lenght
 * \param  in_packet              already received sub packet
 * \param  in_image_header_packet already received image header packet
 * \param  out_packet             image to receive 
 * \param  in_out_net_buffer      packet buffer (will grow during the process because data will be concatenated)
 * \param  out_error              error code
 * \return true if succeed, false in case of error
 ****************************************************************************************************/
bool CameraControl::receiveImageSubPacket(const NetGenericHeader & in_packet             ,
                                          const NetImageHeader   & in_image_header_packet,
                                          NetImage               * out_packet            ,
                                          std::vector<uint8_t>   & in_out_net_buffer     ,
                                          int32_t                & out_error             )
{
    DEB_MEMBER_FUNCT();

    const std::size_t previous_size = in_out_net_buffer.size();

    // increasing the buffer size
    in_out_net_buffer.resize(previous_size + in_image_header_packet.m_specific_data_lenght, 0);

    if(!receive(in_out_net_buffer.data() + previous_size, in_image_header_packet.m_specific_data_lenght, out_error))
        return false;

    // filling the buffer data into the header members
    const uint8_t * memory_data = in_out_net_buffer.data() + previous_size;
    std::size_t     memory_size = in_image_header_packet.m_specific_data_lenght;

    if(!out_packet->read(memory_data, memory_size))
    {
        DEB_ERROR() << "CameraControl::receiveImageSubPacket - Error during the buffer copy into the sub packet!";
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
        DEB_ERROR() << "CameraControl::FillFullPacket - Error during the buffer copy into the "
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
    out_error  = 0   ;

    // at start, we do not know the kind of packet.
    // we can only receive the generic header to determine the packet type.
    NetGenericHeader header;
    std::vector<uint8_t> net_buffer;

    if(!receiveSubPacket(&header, net_buffer, out_error))
        return false;
#ifdef SPECTRAL_CAMERA_CONTROL_ACTIVATE_PACKET_TRACE
    header.log();
#endif

    // checking the camera identifier
    if((header.m_camera_identifier != NetCommandHeader::g_server_command_identifier) &&
       (header.m_camera_identifier != m_init_parameters.m_camera_identifier))
    {
        DEB_ERROR() << "CameraControl::receivePacket - Incorrect camera indentifier found into the packet header!";
        return false;
    }

    // checking the packet identifier to determine the next data block to read
    //===========================================================================================
    // check if this is a command packet
    if(header.isCommandPacket())
    {
        DEB_ERROR() << "CameraControl::receivePacket - A command packet can not be received!";
        return false;
    }
    else
    //===========================================================================================
    // check if this is a acknowledge packet
    if(header.isAcknowledgePacket())
    {
        NetAcknowledge ack;

        if(!receiveGenericSubPacket(header, &ack,  net_buffer, true, out_error))
            return false;

    #ifdef SPECTRAL_CAMERA_CONTROL_ACTIVATE_PACKET_TRACE
        ack.log();
    #endif

        // it's ok, we can "return" the packet
        out_packet = new NetAcknowledge();
    }
    else
    //===========================================================================================
    // check if this is a data packet
    if(header.isDataPacket())
    {
        NetGenericAnswer answer;

        if(!receiveGenericSubPacket(header, &answer,  net_buffer, false, out_error))
            return false;

    #ifdef SPECTRAL_CAMERA_CONTROL_ACTIVATE_PACKET_TRACE
        answer.log();
    #endif

        // checking the error code
    #ifdef SPECTRAL_CAMERA_CONTROL_ACTIVATE_NETWORK_TRACE
        if(answer.hasError())
        {
            DEB_TRACE() << "received an error into the " << out_packet->m_packet_name << " packet.";
        }
    #endif

        // checking the data type to determine the final class to use
        // status
        if(answer.isGetStatusPacket())
        {
            NetAnswerGetStatus get_status;

            if(!receiveSpecificSubPacket(header, answer, &get_status,  net_buffer, out_error))
                return false;
            //header.log();
            //answer.log();
            // it's ok, we can "return" the packet
            out_packet = new NetAnswerGetStatus();
        }
        else
        // camera parameters
        if(answer.isGetCameraParameters())
        {
            NetAnswerGetCameraParameters get_params;

            if(!receiveSpecificSubPacket(header, answer, &get_params,  net_buffer, out_error))
                return false;
            //header.log();
            //answer.log();
        #ifdef SPECTRAL_CAMERA_CONTROL_ACTIVATE_PACKET_TRACE
            get_params.log();
        #endif

            // it's ok, we can "return" the packet
            out_packet = new NetAnswerGetCameraParameters();
            //out_packet->log();
        }
        else
        // settings
        if(answer.isGetSettings())
        {
            NetAnswerGetSettings get_settings;

            if(!receiveSpecificSubPacket(header, answer, &get_settings,  net_buffer, out_error))
                return false;
            //header.log();
            //answer.log();
        #ifdef SPECTRAL_CAMERA_CONTROL_ACTIVATE_PACKET_TRACE
            get_settings.log();
        #endif

            // it's ok, we can "return" the packet
            out_packet = new NetAnswerGetSettings();
            //out_packet->log();
        }
        else
        // acuisition status
        if(answer.isAcquisitionStatus())
        {
            NetAnswerAcquisitionStatus acquisition_status;

            if(!receiveSpecificSubPacket(header, answer, &acquisition_status,  net_buffer, out_error))
                return false;

        #ifdef SPECTRAL_CAMERA_CONTROL_ACTIVATE_PACKET_TRACE
            acquisition_status.log();
        #endif

            // it's ok, we can "return" the packet
            out_packet = new NetAnswerAcquisitionStatus();
        }
        else
        //===========================================================================================
        // command done
        if(answer.isCommandDonePacket())
        {
            NetAnswerCommandDone command_done;

            if(!receiveSpecificSubPacket(header, answer, &command_done,  net_buffer, out_error))
                return false;

        #ifdef SPECTRAL_CAMERA_CONTROL_ACTIVATE_PACKET_TRACE
            command_done.log();
        #endif

            // check the function type
            // set acquisition mode
            if(command_done.m_function_number == NetCommandHeader::g_function_number_set_acquisition_mode)
            {
                // it's ok, we can "return" the packet
                out_packet = new NetAnswerSetAcquisitionMode();
            }
            else
            // set exposure time
            if(command_done.m_function_number == NetCommandHeader::g_function_number_set_exposure_time)
            {
                // it's ok, we can "return" the packet
                out_packet = new NetAnswerSetExposureTime();
            }
            else
            // set format parameters
            if(command_done.m_function_number == NetCommandHeader::g_function_number_set_format_parameters)
            {
                // it's ok, we can "return" the packet
                out_packet = new NetAnswerSetFormatParameters();
            }
            else
            // set acquisition type
            if(command_done.m_function_number == NetCommandHeader::g_function_number_set_acquisition_type)
            {
                // it's ok, we can "return" the packet
                out_packet = new NetAnswerSetAcquisitionType();
            }
            else
            // acquire
            if(command_done.m_function_number == NetCommandHeader::g_function_number_acquire)
            {
                // it's ok, we can "return" the packet
                out_packet = new NetAnswerAcquire();
            }
            else
            // configure packets settigs
            if(command_done.m_function_number == NetCommandHeader::g_function_number_configure_packets)
            {
                // it's ok, we can "return" the packet
                out_packet = new NetAnswerConfigurePackets();
            }
            else
            // software bug during an acquisition, we can receive incorrect function number for end of acquisition command done packet!... 
            if((command_done.m_function_number == NetCommandHeader::g_function_number_get_status                ) ||
               (command_done.m_function_number == NetCommandHeader::g_function_number_get_settings              ) ||
               (command_done.m_function_number == NetCommandHeader::g_function_number_get_camera_parameters     ) ||
               (command_done.m_function_number == NetCommandHeader::g_function_number_inquire_acquisition_status))
            {
            #ifdef SPECTRAL_CAMERA_CONTROL_ACTIVATE_LIGHT_PACKET_TRACE
                DEB_WARNING() << "received an incorrect command done packet for a function: " << command_done.m_function_number;
            #endif

                // it's ok, we can "return" the packet
                out_packet = new NetAnswerAcquire();
            }
            else
            // terminate acquisition
            if(command_done.m_function_number == NetCommandHeader::g_function_number_terminate_acquisition)
            {
                // it's ok, we can "return" the packet
                out_packet = new NetAnswerTerminateAcquisition();
            }
            else
            // terminate image retrieve
            if(command_done.m_function_number == NetCommandHeader::g_function_number_terminate_image_retrieve)
            {
                // it's ok, we can "return" the packet
                out_packet = new NetAnswerTerminateImageRetrieve();
            }
            else
            // set cooling value
            if(command_done.m_function_number == NetCommandHeader::g_function_number_set_cooling_value)
            {
                // it's ok, we can "return" the packet
                out_packet = new NetAnswerSetCoolingValue();
            }
            else
            if(command_done.m_function_number == NetCommandHeader::g_function_number_set_single_parameter)
            {
                // it's ok, we can "return" the packet
                out_packet = new NetAnswerSetReadoutSpeedValue();
            }
            else
            {
                DEB_ERROR() << "CameraControl::receivePacket - Unknown command done function type: " << command_done.m_function_number << "!";
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
    //===========================================================================================
    // check if this is an image packet
    if(header.isImagePacket())
    {
        NetImageHeader image_header;

        if(!receiveGenericSubPacket(header, &image_header,  net_buffer, false, out_error))
            return false;

    #ifdef SPECTRAL_CAMERA_CONTROL_ACTIVATE_PACKET_TRACE
        image_header.NetImageHeader::log();
    #endif

        // checking the error code
    #ifdef SPECTRAL_CAMERA_CONTROL_ACTIVATE_NETWORK_TRACE
        if(image_header.hasError())
        {
            DEB_TRACE() << "received an error into the " << out_packet->m_packet_name << " packet.";
        }
    #endif

        NetImage image;

        if(!receiveImageSubPacket(header, image_header, &image,  net_buffer, out_error))
            return false;

        // it's ok, we can "return" the packet
        out_packet = new NetImage();
    }
    else
    {
        DEB_ERROR() << "CameraControl::receivePacket - Unknown packet type!";
        return false;
    }
    
    // it's ok, we can fill the packet
#ifdef SPECTRAL_CAMERA_CONTROL_ACTIVATE_NETWORK_TRACE
    if(out_packet == NULL)
    {
        DEB_TRACE() << "CameraControl::receivePacket - out_packet should not be NULL!";
    }
#endif

    if(!FillFullPacket(out_packet, net_buffer, out_error))
    {
        delete out_packet;
        out_packet = NULL;
        return false;
    }

#ifdef SPECTRAL_CAMERA_CONTROL_ACTIVATE_NETWORK_TRACE
    DEB_TRACE() << "received " << out_packet->m_packet_name << " packet.";
#endif
    return true;
}

/****************************************************************************************************
 * \fn void addPacket(NetGenericHeader * in_packet)
 * \brief  Add a new packet to the packets container (the instance will be freed by the container or a consumer)
 * \param  in_packet new packet
 * \return none
 ****************************************************************************************************/
void CameraControl::addPacket(NetGenericHeader * in_packet)
{
    DEB_MEMBER_FUNCT();

    NetPacketsGroupId group_id;

    // check if this is an acknowledge or an image packet 
    if(in_packet->isAcknowledgePacket() || in_packet->isImagePacket())
    {
        group_id = static_cast<NetPacketsGroupId>(in_packet->m_packet_identifier);

    #ifdef SPECTRAL_CAMERA_CONTROL_ACTIVATE_LIGHT_PACKET_TRACE
        DEB_TRACE() << "> receive Acknowledge packet " << group_id;
    #endif
    }
    // check if this is a data packet
    else
    if(in_packet->isDataPacket())
    {
        NetGenericAnswer * generic_answer = dynamic_cast<NetGenericAnswer *>(in_packet);

        // check if this is a command done packet
        if(generic_answer->isCommandDonePacket())
        {
            NetAnswerCommandDone * command_done = dynamic_cast<NetAnswerCommandDone *>(generic_answer);

            // software bug during an acquisition, we can receive incorrect function number for end of acquisition command done packet!... 
            if((command_done->m_function_number == NetCommandHeader::g_function_number_get_status                ) ||
               (command_done->m_function_number == NetCommandHeader::g_function_number_get_settings              ) ||
               (command_done->m_function_number == NetCommandHeader::g_function_number_get_camera_parameters     ) ||
               (command_done->m_function_number == NetCommandHeader::g_function_number_inquire_acquisition_status))
            {
                command_done->m_function_number = NetCommandHeader::g_function_number_acquire;
            }

            group_id = command_done->m_function_number;

        #ifdef SPECTRAL_CAMERA_CONTROL_ACTIVATE_LIGHT_PACKET_TRACE
            DEB_TRACE() << "> receive CommandDone packet " << group_id;
        #endif
        }
        else
        {
            group_id = generic_answer->m_data_type;

        #ifdef SPECTRAL_CAMERA_CONTROL_ACTIVATE_LIGHT_PACKET_TRACE
            DEB_TRACE() << "> receive data packet..." << group_id;
        #endif
        }
    }
    else
    {
        DEB_ERROR() << "CameraControl::addPacket - Incoherent packet type!";
        return;
    }

    // search the group
    ProtectedList<NetGenericHeader> * group = m_packets_container.searchGroup(group_id);

    if(group == NULL)
    {
        DEB_ERROR() << "CameraControl::addPacket - The group " << (int)group_id << " is not managed!";
        return;
    }

    group->put(in_packet);
}

/****************************************************************************************************
 * \fn bool waitPacket(NetPacketsGroupId in_group_id, NetGenericHeader * & out_packet)
 * \brief  Wait for a new packet to be received
 * \param  in_group_id type of packet
 * \param  out_packet  received packet
 * \return true if succeed, false in case of error (timeout)
 ****************************************************************************************************/
bool CameraControl::waitPacket(NetPacketsGroupId in_group_id, NetGenericHeader * & out_packet)
{
    DEB_MEMBER_FUNCT();

    out_packet = NULL;

    // search the group
    ProtectedList<NetGenericHeader> * group = m_packets_container.searchGroup(in_group_id);

    if(group == NULL)
    {
        DEB_ERROR() << "CameraControl::waitPacket - The group " << (int)in_group_id << " is not managed!";
        return false;
    }

    if(group->waiting_while_empty())
    {
        if(!group->empty())
        {
            out_packet = group->take();
        }
        else
        {
            DEB_ERROR() << "CameraControl::waitPacket - Incorrect behaviour for the group " << (int)in_group_id;
        }
    }

    return (out_packet != NULL);
}

/****************************************************************************************************
 * \fn bool waitAcknowledgePacket(NetGenericHeader * & out_packet)
 * \brief  Wait for a new ack packet to be received
 * \param  out_packet  received packet
 * \return true if succeed, false in case of error
 ****************************************************************************************************/
bool CameraControl::waitAcknowledgePacket(NetGenericHeader * & out_packet)
{
    return waitPacket(static_cast<NetPacketsGroupId>(NetGenericHeader::g_packet_identifier_for_acknowledge), out_packet);
}

/****************************************************************************************************
 * \fn bool waitImagePacket(NetGenericHeader * & out_packet)
 * \brief  Wait for a new image packet to be received
 * \param  out_packet  received packet
 * \return true if succeed, false in case of error
 ****************************************************************************************************/
bool CameraControl::waitImagePacket(NetGenericHeader * & out_packet)
{
    return waitPacket(static_cast<NetPacketsGroupId>(NetGenericHeader::g_packet_identifier_for_image), out_packet);
}

/****************************************************************************************************
 * \fn bool waitDataPacket(uint16_t in_data_type, NetGenericHeader * & out_packet)
 * \brief  Wait for a new data packet to be received
 * \param  in_data_type data type
 * \param  out_packet   received packet
 * \return true if succeed, false in case of error
 ****************************************************************************************************/
bool CameraControl::waitDataPacket(uint16_t in_data_type, NetGenericHeader * & out_packet)
{
    return waitPacket(in_data_type, out_packet);
}

/****************************************************************************************************
 * \fn bool waitCommandDonePacket(uint16_t in_function_number, NetGenericHeader * & out_packet)
 * \brief  Wait for a new command done packet to be received
 * \param  in_function_number function number relative to the command done
 * \param  out_packet         received packet
 * \return true if succeed, false in case of error
 ****************************************************************************************************/
bool CameraControl::waitCommandDonePacket(uint16_t in_function_number, NetGenericHeader * & out_packet)
{
    return waitDataPacket(in_function_number, out_packet);
}

/****************************************************************************************************
 * \fn bool getPacket(NetPacketsGroupId in_group_id, NetGenericHeader * & out_packet)
 * \brief  get a new packet if there is one received
 * \param  in_group_id type of packet
 * \param  out_packet  received packet
 * \return true if succeed, false in case of error
 ****************************************************************************************************/
bool CameraControl::getPacket(NetPacketsGroupId in_group_id, NetGenericHeader * & out_packet)
{
    DEB_MEMBER_FUNCT();

    out_packet = NULL;

    // search the group
    ProtectedList<NetGenericHeader> * group = m_packets_container.searchGroup(in_group_id);

    if(group == NULL)
    {
        DEB_ERROR() << "CameraControl::getPacket - The group " << (int)in_group_id << " is not managed!";
        return false;
    }
    else
    if(!group->empty())
    {
        out_packet = group->take();
    }
    return (out_packet != NULL);
}

/****************************************************************************************************
 * \fn bool getCommandDonePacket(uint16_t in_function_number, NetGenericHeader * & out_packet)
 * \brief  get a new command done packet if there is one received
 * \param  in_function_number function number relative to the command done
 * \param  out_packet         received packet
 * \return true if succeed, false in case of error
 ****************************************************************************************************/
bool CameraControl::getCommandDonePacket(uint16_t in_function_number, NetGenericHeader * & out_packet)
{
    return getPacket(in_function_number, out_packet);
}

/****************************************************************************************************
 * \fn bool getImagePacket(NetGenericHeader * & out_packet)
 * \brief  get a new image packet if there is one received
 * \param  out_packet  received packet
 * \return true if succeed, false in case of error
 ****************************************************************************************************/
bool CameraControl::getImagePacket(NetGenericHeader * & out_packet)
{
    return getPacket(static_cast<NetPacketsGroupId>(NetGenericHeader::g_packet_identifier_for_image), out_packet);
}

/****************************************************************************************************
 * \fn bool getAcknowledgePacket(NetGenericHeader * & out_packet)
 * \brief  get a new ack packet if there is one received
 * \param  out_packet  received packet
 * \return true if succeed, false in case of error
 ****************************************************************************************************/
bool CameraControl::getAcknowledgePacket(NetGenericHeader * & out_packet)
{
    return getPacket(static_cast<NetPacketsGroupId>(NetGenericHeader::g_packet_identifier_for_acknowledge), out_packet);
}

/****************************************************************************************************
 * \fn bool getDataPacket(uint16_t in_data_type, NetGenericHeader * & out_packet)
 * \brief  get a new data packet if there is one received
 * \param  in_data_type data type
 * \param  out_packet   received packet
 * \return true if succeed, false in case of error
 ****************************************************************************************************/
bool CameraControl::getDataPacket(uint16_t in_data_type, NetGenericHeader * & out_packet)
{
    return getPacket(in_data_type, out_packet);
}

/****************************************************************************************************
 * \fn bool getAcquisitionStatusPacket(NetGenericHeader * & out_packet)
 * \brief  get a new acquisition status packet if there is one received
 * \param  out_packet   received packet
 * \return true if succeed, false in case of error
 ****************************************************************************************************/
bool CameraControl::getAcquisitionStatusPacket(NetGenericHeader * & out_packet)
{
    return getDataPacket(NetGenericAnswer::g_data_type_acquisition_status, out_packet);
}

/****************************************************************************************************
 * \fn void flushAcknowledgePackets()
 * \brief  flush old acknowledge packets
 * \param  none
 * \return none
 ****************************************************************************************************/
void CameraControl::flushAcknowledgePackets()
{
    DEB_MEMBER_FUNCT();

    NetGenericHeader * old_packet;

    while(getAcknowledgePacket(old_packet))
    {
    #ifdef SPECTRAL_CAMERA_CONTROL_ACTIVATE_NETWORK_TRACE
        DEB_WARNING() << "Flushing old acknowledge packet!";
    #endif

        delete old_packet;
    }
}

/****************************************************************************************************
 * \fn void flushAcquisitionStatusPackets()
 * \brief  flush old acquisition status packets
 * \param  none
 * \return none
 ****************************************************************************************************/
void CameraControl::flushAcquisitionStatusPackets()
{
    DEB_MEMBER_FUNCT();

    NetGenericHeader * old_packet;

    while(getAcquisitionStatusPacket(old_packet))
    {
    #ifdef SPECTRAL_CAMERA_CONTROL_ACTIVATE_NETWORK_TRACE
        DEB_WARNING() << "Flushing old acquisition status packet!";
    #endif

        delete old_packet;
    }
}

/****************************************************************************************************
 * \fn void flushImagePackets()
 * \brief  flush old image packets
 * \param  none
 * \return none
 ****************************************************************************************************/
void CameraControl::flushImagePackets()
{
    DEB_MEMBER_FUNCT();

    NetGenericHeader * old_packet;

    while(getImagePacket(old_packet))
    {
    #ifdef SPECTRAL_CAMERA_CONTROL_ACTIVATE_NETWORK_TRACE
        DEB_WARNING() << "Flushing old image packet!";
    #endif

        delete old_packet;
    }
}

/**************************************************************************************************
 * COMMANDS MANAGEMENT
 **************************************************************************************************/
/****************************************************************************************************
 * \fn bool sendCommandWithoutAck(NetCommandHeader * in_out_command, int32_t & out_error)
 * \brief  Send a command to the detector and does not wait an acknowledge (only special commands)
 * \param  in_out_command command to send
 * \param  out_error      error code
 * \return true if succeed, false in case of error
 ****************************************************************************************************/
bool CameraControl::sendCommandWithoutAck(NetCommandHeader * in_out_command, int32_t & out_error)
{
    DEB_MEMBER_FUNCT();

    // protecting the multi-threads access
    lima::AutoMutex send_command_mutex = sendCommandLock(); 

    // Send a command to the detector
    return sendCommand(in_out_command, out_error);
}

/****************************************************************************************************
 * \fn bool sendCommandWithAck(NetCommandHeader * in_out_command, int32_t & out_error)
 * \brief  Send a command to the detector and wait for the acknowledge
 * \param  in_out_command command to send
 * \param  out_error      error code
 * \return true if succeed, false in case of error
 ****************************************************************************************************/
bool CameraControl::sendCommandWithAck(NetCommandHeader * in_out_command, int32_t & out_error)
{
    DEB_MEMBER_FUNCT();

    // protecting the multi-threads access
    lima::AutoMutex send_command_mutex = sendCommandLock(); 

    bool               result       = false;
    NetGenericHeader * first_packet = NULL ;
    NetAcknowledge   * ack_packet   = NULL ;

    // first flush old acknowledge packet (should not occur!)
    flushAcknowledgePackets();

    // Send a command to the detector
    if(!sendCommand(in_out_command, out_error))
        goto done;

    // wait for an acknowledge
    if(!waitAcknowledgePacket(first_packet))
        goto done;

    ack_packet = dynamic_cast<NetAcknowledge *>(first_packet);

    // check if the command was accepted
    if (!(ack_packet->wasAccepted()))
    {
    #ifdef SPECTRAL_CAMERA_CONTROL_ACTIVATE_NETWORK_TRACE
        DEB_TRACE() << "Command " << in_out_command->m_packet_name << " was not accepted!";
    #endif
        goto done;
    }
    else
    {
        result = true;
    }

done:
    if(first_packet != NULL)
        delete first_packet ;

    return result;
}

/****************************************************************************************************
 * \fn bool findLineWithKey(const std::string & in_lines, const std::string & in_key, std::string & out_line)
 * \brief  Search and find a line which has the given key
 * \param  in_lines source string which contains the lines
 * \param  in_key    key to search in the lines
 * \param  out_line  found line
 * \return true if succeed, false in case of error
 ****************************************************************************************************/
bool CameraControl::findLineWithKey(const std::string & in_lines ,
                                    const std::string & in_key   ,
                                    std::string       & out_line )
{
    std::string line;
    bool result = false;

    std::istringstream iss(in_lines);

    while (std::getline(iss, line))
    {
        // search the status key
        if (line.find(in_key) != std::string::npos) 
        {
            out_line = line;
            result   = true;
            break;
        }
    }

    return result;
}

/****************************************************************************************************
 * \fn bool findLineWithTwoKey(const std::string & in_lines, const std::string & in_first_key, const std::string & in_second_key, const std::string & in_delimiter, std::string & out_line)
 * \brief  Search and find a line which has the two given keys
 * \param  in_lines source string which contains the lines
 * \param  in_first_key  first key to search in the lines
 * \param  in_second_key second key to search in the lines
 * \param  in_pos        substring delimiter
 * \param  out_line      found line
 * \return true if succeed, false in case of error
 ****************************************************************************************************/
bool CameraControl::findLineWithTwoKey(const std::string & in_lines     ,
                                       const std::string & in_first_key ,
                                       const std::string & in_second_key,
                                       const std::string & in_delimiter ,
                                       std::string       & out_line     )
{
    std::string key = in_first_key + in_delimiter + in_second_key;
    return findLineWithKey(in_lines, key, out_line);
}

/****************************************************************************************************
 * \fn bool getSubString(const std::string & in_string, std::size_t in_pos, const std::string & in_delimiter, std::string & out_sub_string)
 * \brief  Cut a substring with a position and delimiter
 * \param  in_string source string
 * \param  in_pos substring position (starts at 0)
 * \param  in_pos substring delimiter
 * \param  out_sub_string found substring
 * \return true if succeed, false in case of error
 ****************************************************************************************************/
bool CameraControl::getSubString(const std::string & in_string     ,
                                 std::size_t         in_pos        ,
                                 const std::string & in_delimiter  ,
                                 std::string       & out_sub_string)
{
    std::size_t last = 0;
    std::size_t next = 0;
    std::size_t pos  = 0;
    
    while ((next = in_string.find(in_delimiter, last)) != std::string::npos)
    {   
        // found the value ?
        if(pos == in_pos)
        {
            out_sub_string = in_string.substr(last, next - last);
            return true;
        }
        last = next + 1;
        pos++;
    } 

    // found the value in the last position ?
    if(pos == in_pos)
    {
        out_sub_string = in_string.substr(last);
        return true;
    }

    return false;
}

/****************************************************************************************************
 * \fn bool bool convertStringToInt(const std::string & in_string, int & out_value)
 * \brief  Convert a string to an integer
 * \param  in_string source string
 * \param  out_value converted integer value
 * \return true if succeed, false in case of error
 ****************************************************************************************************/
bool CameraControl::convertStringToInt(const std::string & in_string, int & out_value)
{
    bool        result;
    std::string sub_string;

    std::istringstream ss(in_string);
    ss >> out_value;

    if (ss.fail() || (ss.rdbuf()->in_avail() > 0))
    {
        ss.clear();
        result = false;
    }
    else
    {
        result = true;
    }

    return result;
}

/****************************************************************************************************
 * \fn bool CameraControl::updateStatus()
 * \brief  Update the current status by sending a command to the hardware
 * \param  none
 * \return true if succeed, false in case of error
 ****************************************************************************************************/
bool CameraControl::updateStatus()
{
    DEB_MEMBER_FUNCT();

    std::string          status        ;
    std::string         values         ;
    int                  status_value  ;
    int                hks_status_value;
    std::string          line          ;
    std::string          sub_string    ;
    DetectorStatus       new_status    ;
    int32_t              error         = 0    ;
    bool                 result        = false;
    NetAnswerGetStatus * status_packet = NULL ;
    NetGenericHeader   * second_packet = NULL ;
    NetCommandHeader   * command       = new NetCommandGetStatus();

    // send the command and treat the acknowledge
    if(!sendCommandWithAck(command, error))
        goto done;

    // wait for the status
    if(!waitDataPacket(NetGenericAnswer::g_data_type_get_status, second_packet))
        goto done;

    // we need to manage the status data 
    status_packet = dynamic_cast<NetAnswerGetStatus *>(second_packet);

    if(!status_packet->hasError())
    {
        status = status_packet->m_value;

        if(!findLineWithKey(status, NetAnswerGetStatus::g_server_flags_status_name, line))
            goto done;

        if(!getSubString(line, NetAnswerGetStatus::g_server_flags_value_position, 
                         NetAnswerGetStatus::g_server_flags_delimiter, sub_string))
            goto done;

        if(!convertStringToInt(sub_string, status_value))
            goto done;

        // conversion of the hardware status to a detector status
        new_status = DetectorStatus::Fault;

        if(status_value & NetAnswerGetStatus::HardwareStatus::CameraConnected)
        {
            if(!(status_value & NetAnswerGetStatus::HardwareStatus::ConfigurationError))
            {
                if(status_value & NetAnswerGetStatus::HardwareStatus::AcquisitionInProgress)
                {
                    new_status = DetectorStatus::Exposure;
                }
                else
                {
                    new_status = DetectorStatus::Ready;
                }
            }
        }


        if(!findLineWithKey(status, NetAnswerGetStatus::g_server_flags_hks_name, line))
            goto done;


        if(!getSubString(line, NetAnswerGetStatus::g_server_flags_value_position, 
                         NetAnswerGetStatus::g_server_flags_delimiter, sub_string))
            goto done;

        if(!convertStringToInt(sub_string, hks_status_value))
            goto done;


        if(status_value & NetAnswerGetStatus::HardwareStatus::CameraConnected)
        {
            if(!(status_value & NetAnswerGetStatus::HardwareStatus::ConfigurationError))
            {
                if(hks_status_value & NetAnswerGetStatus::HKSFlags::TECEnabled)
                {
                    m_cooling_value = true;
                }
                else
                {
                    m_cooling_value = false;
                }
            }
        }

        if(!findLineWithKey(status, NetAnswerGetStatus::g_server_flags_ccd_temperature_name, line))
            goto done;


        if(!getSubString(line, NetAnswerGetStatus::g_server_flags_value_position, 
                         NetAnswerGetStatus::g_server_flags_delimiter, sub_string))
            goto done;

        if(status_value & NetAnswerGetStatus::HardwareStatus::CameraConnected)
        {
            if(!(status_value & NetAnswerGetStatus::HardwareStatus::ConfigurationError))
            {
                m_ccd_temperature = std::stof(sub_string);
                
            }
        }

        m_latest_status = new_status;
        result          = true      ;


    }

done:
    if(second_packet != NULL) delete second_packet;
    if(command       != NULL) delete command      ;

    return result;
}

/****************************************************************************************************
 * \fn bool CameraControl::initCameraParameters()
 * \brief  Init some static data (model, serial number, max width, max lenght, pixel depths)
 * \param  none
 * \return true if succeed, false in case of error
 ****************************************************************************************************/
bool CameraControl::initCameraParameters()
{
    DEB_MEMBER_FUNCT();

    NetAnswerGetCameraParameters * params_packet = NULL;

    int32_t            error         = 0    ;
    bool               result        = false;
    NetGenericHeader * second_packet = NULL ;
    NetCommandHeader * command       = new NetCommandGetCameraParameters();

    std::string        values    ;
    int                value     ;
    std::string        line      ;
    std::string        sub_string;

    // send the command and treat the acknowledge
    if(!sendCommandWithAck(command, error))
        goto done;

    // wait for the status
    if(!waitDataPacket(NetGenericAnswer::g_data_type_get_camera_parameters, second_packet))
        goto done;

    // we need to manage the data 
    params_packet = dynamic_cast<NetAnswerGetCameraParameters *>(second_packet);

    if(!params_packet->hasError())
    {
        values = params_packet->m_value;

        // get the model
        if(!findLineWithTwoKey(values, NetAnswerGetCameraParameters::g_server_flags_group_factory_name,
                                       NetAnswerGetCameraParameters::g_server_flags_instrument_model_name,
                                       NetAnswerGetCameraParameters::g_server_flags_delimiter,
                                       line))
            goto done;


        if(!getSubString(line, NetAnswerGetCameraParameters::g_server_flags_value_position, 
                         NetAnswerGetCameraParameters::g_server_flags_delimiter, m_model))
            goto done;

        // get the serial number
        if(!findLineWithTwoKey(values, NetAnswerGetCameraParameters::g_server_flags_group_factory_name,
                                       NetAnswerGetCameraParameters::g_server_flags_instrument_serial_number_name,
                                       NetAnswerGetCameraParameters::g_server_flags_delimiter,
                                       line))
            goto done;

        if(!getSubString(line, NetAnswerGetCameraParameters::g_server_flags_value_position,
                         NetAnswerGetCameraParameters::g_server_flags_delimiter, m_serial_number))
            goto done;

        // get the serial size
        if(!findLineWithTwoKey(values, NetAnswerGetCameraParameters::g_server_flags_group_factory_name,
                                       NetAnswerGetCameraParameters::g_server_flags_instrument_serial_size_name,
                                       NetAnswerGetCameraParameters::g_server_flags_delimiter,
                                       line))
            goto done;

        if(!getSubString(line, NetAnswerGetCameraParameters::g_server_flags_value_position,
                         NetAnswerGetCameraParameters::g_server_flags_delimiter, sub_string))
            goto done;

        if(!convertStringToInt(sub_string, value))
            goto done;

        m_width_max = value;

        // get the parallel size
        if(!findLineWithTwoKey(values, NetAnswerGetCameraParameters::g_server_flags_group_factory_name,
                                       NetAnswerGetCameraParameters::g_server_flags_instrument_parallel_size_name,
                                       NetAnswerGetCameraParameters::g_server_flags_delimiter,
                                       line))
            goto done;

        if(!getSubString(line, NetAnswerGetCameraParameters::g_server_flags_value_position,
                         NetAnswerGetCameraParameters::g_server_flags_delimiter, sub_string))
            goto done;

        if(!convertStringToInt(sub_string, value))
            goto done;

        m_height_max = value;

        // get the parallel size
        if(!findLineWithTwoKey(values, NetAnswerGetCameraParameters::g_server_flags_group_miscellaneous_name,
                                       NetAnswerGetCameraParameters::g_server_flags_instrument_bits_per_pixel_name,
                                       NetAnswerGetCameraParameters::g_server_flags_delimiter,
                                       line))
            goto done;

        if(!getSubString(line, NetAnswerGetCameraParameters::g_server_flags_value_position,
                         NetAnswerGetCameraParameters::g_server_flags_delimiter, sub_string))
            goto done;

        if(!convertStringToInt(sub_string, value))
            goto done;

        m_pixel_depth = value;

        result = true;
    }

done:
    if(second_packet != NULL) delete second_packet;
    if(command       != NULL) delete command      ;

    return result;
}

/****************************************************************************************************
 * \fn bool CameraControl::updateSettings()
 * \brief  Update the settings by sending a command to the hardware
 * \param  none
 * \return true if succeed, false in case of error
 ****************************************************************************************************/
bool CameraControl::updateSettings()
{
    DEB_MEMBER_FUNCT();

    int32_t                error           = 0    ;
    bool                   result          = false;
    NetAnswerGetSettings * settings_packet = NULL ;
    NetGenericHeader     * second_packet   = NULL ;
    NetCommandHeader     * command         = new NetCommandGetSettings();

    NetAnswerGetCameraParameters * params_packet = NULL;

    NetGenericHeader * cam_param_second_packet = NULL ;
    NetCommandHeader * cam_param_command       = new NetCommandGetCameraParameters();

    std::string        values    ;
    std::string        line      ;
    std::string        sub_string;
    int readout_speed = 0;

    // send the command and treat the acknowledge
    if(!sendCommandWithAck(cam_param_command, error))
        goto done;

    // wait for the status
    if(!waitDataPacket(NetGenericAnswer::g_data_type_get_camera_parameters, cam_param_second_packet))
        goto done;

    // we need to manage the data 
    params_packet = dynamic_cast<NetAnswerGetCameraParameters *>(cam_param_second_packet);


    // send the command and treat the acknowledge
    if(!sendCommandWithAck(command, error))
        goto done;

    // wait for the status
    if(!waitDataPacket(NetGenericAnswer::g_data_type_get_settings, second_packet))
        goto done;

    // we need to manage the settings data 
    settings_packet = dynamic_cast<NetAnswerGetSettings *>(second_packet);


    if(!settings_packet->hasError())
    {
        m_exposure_time_msec   = settings_packet->m_exposure_time_msec; 
        m_nb_images_to_acquire = settings_packet->m_nb_images_to_acquire;
        m_serial_origin        = settings_packet->m_serial_origin; 
        m_serial_length        = settings_packet->m_serial_length; 
        m_serial_binning       = settings_packet->m_serial_binning; 
        m_parallel_origin      = settings_packet->m_parallel_origin; 
        m_parallel_length      = settings_packet->m_parallel_length; 
        m_parallel_binning     = settings_packet->m_parallel_binning;
        m_acquisition_type     = static_cast<NetAnswerGetSettings::AcquisitionType>(settings_packet->m_acquisition_type);
        m_acquisition_mode     = static_cast<NetAnswerGetSettings::AcquisitionMode>(settings_packet->m_acquisition_mode);
        result                 = true;

    }

    if(!params_packet->hasError())
    {
        values = params_packet->m_value;

        if(!findLineWithTwoKey(values, NetAnswerGetCameraParameters::g_server_flags_group_control_name,
                                       NetAnswerGetCameraParameters::g_server_flags_control_dsi_sample_time_name,
                                       NetAnswerGetCameraParameters::g_server_flags_delimiter,
                                       line))
            goto done;

        if(!getSubString(line, NetAnswerGetCameraParameters::g_server_flags_value_position,
                         NetAnswerGetCameraParameters::g_server_flags_delimiter, sub_string))
            goto done;

        if(!convertStringToInt(sub_string, readout_speed) )
            goto done;
        m_readout_speed_value = static_cast<ushort>(readout_speed);
    }

done:
    if(second_packet != NULL) delete second_packet;
    if(command       != NULL) delete command      ;

    return result;
}

/****************************************************************************************************
 * \fn bool setExposureTimeMsec(uint32_t in_exposure_time_msec)
 * \brief  change the exposure time by sending a command to the hardware
 * \param  in_exposure_time_msec  exposure time in milli-seconds
 * \return true if succeed, false in case of error
 ****************************************************************************************************/
bool CameraControl::setExposureTimeMsec(uint32_t in_exposure_time_msec)
{
    DEB_MEMBER_FUNCT();

    int32_t                error           = 0    ;
    bool                   result          = false;
    NetGenericHeader     * second_packet   = NULL ;
    NetCommandHeader     * command         = new NetCommandSetExposureTime();
    
    NetAnswerSetExposureTime  * answer_packet         = NULL ;
    NetCommandSetExposureTime * command_exposure_time = dynamic_cast<NetCommandSetExposureTime *>(command);

    // exposure time (in seconds this time!)
    command_exposure_time->m_exposure_time_sec = static_cast<double>(in_exposure_time_msec) / 1000.0; 

    // send the command and treat the acknowledge
    if(!sendCommandWithAck(command, error))
        goto done;

    // wait for the command done
    if(!waitCommandDonePacket(NetCommandHeader::g_function_number_set_exposure_time, second_packet))
        goto done;

    // we need to manage the data 
    answer_packet = dynamic_cast<NetAnswerSetExposureTime *>(second_packet);

    if(!answer_packet->hasError())
    {
        m_exposure_time_msec = in_exposure_time_msec;
        result = true;
    }

done:
    if(second_packet != NULL) delete second_packet;
    if(command       != NULL) delete command      ;

    return result;
}

/****************************************************************************************************
 * \fn bool setAcquisitionMode(NetAnswerGetSettings::AcquisitionMode in_acquisition_mode)
 * \brief  Change the acquisition mode by sending a command to the hardware
 * \param  in_acquisition_mode new acquisition mode
 * \return true if succeed, false in case of error
 ****************************************************************************************************/
bool CameraControl::setAcquisitionMode(NetAnswerGetSettings::AcquisitionMode in_acquisition_mode)
{
    DEB_MEMBER_FUNCT();

    int32_t                error           = 0    ;
    bool                   result          = false;
    NetGenericHeader     * second_packet   = NULL ;
    NetCommandHeader     * command         = new NetCommandSetAcquisitionMode();
    
    NetAnswerSetAcquisitionMode  * answer_packet            = NULL ;
    NetCommandSetAcquisitionMode * command_acquisition_mode = dynamic_cast<NetCommandSetAcquisitionMode *>(command);

    command_acquisition_mode->m_acquisition_mode = static_cast<uint8_t>(in_acquisition_mode);

    // send the command and treat the acknowledge
    if(!sendCommandWithAck(command, error))
        goto done;

    // wait for the command done
    if(!waitCommandDonePacket(NetCommandHeader::g_function_number_set_acquisition_mode, second_packet))
        goto done;

    // we need to manage the data 
    answer_packet = dynamic_cast<NetAnswerSetAcquisitionMode *>(second_packet);

    if(!answer_packet->hasError())
    {
        m_acquisition_mode = in_acquisition_mode;
        result = true;
    }

done:
    if(second_packet != NULL) delete second_packet;
    if(command       != NULL) delete command      ;

    return result;
}

/****************************************************************************************************
 * \fn bool setFormatParameters(std::size_t in_serial_origin, std::size_t in_serial_length, std::size_t in_serial_binning, std::size_t in_parallel_origin, std::size_t in_parallel_length, std::size_t in_parallel_binning)
 * \brief  Change the format parameters by sending a command to the hardware
 * \param  in_serial_origin CCD Format Serial Origin
 * \param  in_serial_length CCD Format Serial Length
 * \param  in_serial_binning CCD Format Serial Binning
 * \param  in_parallel_origin CCD Format Parallel Origin
 * \param  in_parallel_length  CCD Format Parallel Length 
 * \param  in_parallel_binning CCD Format Parallel Binning 
 * \return true if succeed, false in case of error
 ****************************************************************************************************/
bool CameraControl::setFormatParameters(std::size_t in_serial_origin   ,
                                        std::size_t in_serial_length   , 
                                        std::size_t in_serial_binning  ,
                                        std::size_t in_parallel_origin , 
                                        std::size_t in_parallel_length ,
                                        std::size_t in_parallel_binning)
{
    DEB_MEMBER_FUNCT();

    int32_t                error           = 0    ;
    bool                   result          = false;
    NetGenericHeader     * second_packet   = NULL ;
    NetCommandHeader     * command         = new NetCommandSetFormatParameters();
    
    NetAnswerSetFormatParameters  * answer_packet            = NULL ;
    NetCommandSetFormatParameters * command_acquisition_mode = dynamic_cast<NetCommandSetFormatParameters *>(command);

    command_acquisition_mode->m_serial_origin    = static_cast<int32_t>(in_serial_origin   ); // CCD Format Serial Origin
    command_acquisition_mode->m_serial_length    = static_cast<int32_t>(in_serial_length   ); // CCD Format Serial Length
    command_acquisition_mode->m_serial_binning   = static_cast<int32_t>(in_serial_binning  ); // CCD Format Serial Binning
    command_acquisition_mode->m_parallel_origin  = static_cast<int32_t>(in_parallel_origin ); // CCD Format Parallel Origin
    command_acquisition_mode->m_parallel_length  = static_cast<int32_t>(in_parallel_length ); // CCD Format Parallel Length
    command_acquisition_mode->m_parallel_binning = static_cast<int32_t>(in_parallel_binning); // CCD Format Parallel Binning

    // send the command and treat the acknowledge
    if(!sendCommandWithAck(command, error))
        goto done;

    // wait for the command done
    if(!waitCommandDonePacket(NetCommandHeader::g_function_number_set_format_parameters, second_packet))
        goto done;

    // we need to manage the data 
    answer_packet = dynamic_cast<NetAnswerSetFormatParameters *>(second_packet);

    if(!answer_packet->hasError())
    {
        m_serial_origin    = in_serial_origin   ;
        m_serial_length    = in_serial_length   ;
        m_serial_binning   = in_serial_binning  ;
        m_parallel_origin  = in_parallel_origin ;
        m_parallel_length  = in_parallel_length ;
        m_parallel_binning = in_parallel_binning;

        result = true;
    }

done:
    if(second_packet != NULL) delete second_packet;
    if(command       != NULL) delete command      ;

    return result;
}

/****************************************************************************************************
 * \fn bool setBinning(std::size_t in_serial_binning, std::size_t in_parallel_binning)
 * \brief  Change the binning by sending a command to the hardware
 * \param  in_serial_binning CCD Format Serial Binning
 * \param  in_parallel_binning CCD Format Parallel Binning 
 * \return true if succeed, false in case of error
 ****************************************************************************************************/
bool CameraControl::setBinning(std::size_t in_serial_binning, std::size_t in_parallel_binning)
{
    return setFormatParameters(m_serial_origin    ,
                               m_serial_length    , 
                               in_serial_binning  ,
                               m_parallel_origin  , 
                               m_parallel_length  ,
                               in_parallel_binning);
}

/****************************************************************************************************
 * \fn bool setRoi()
 * \brief  Change the roi by sending a command to the hardware
 * \param  in_serial_origin   CCD Format Serial Origin
 * \param  in_parallel_origin CCD Format Parallel Origin
 * \param  in_serial_length   CCD Format Serial Length
 * \param  in_parallel_length CCD Format Parallel Length 
 * \return true if succeed, false in case of error
 ****************************************************************************************************/
bool CameraControl::setRoi(std::size_t in_serial_origin  ,
                           std::size_t in_parallel_origin, 
                           std::size_t in_serial_length  ,
                           std::size_t in_parallel_length)
{
    return setFormatParameters(in_serial_origin  ,
                               in_serial_length  , 
                               m_serial_binning  ,
                               in_parallel_origin, 
                               in_parallel_length,
                               m_parallel_binning);
}

/****************************************************************************************************
 * \fn bool setAcquisitionType(NetAnswerGetSettings::AcquisitionType in_acquisition_type)
 * \brief  Change the acquisition type by sending a command to the hardware
 * \param  in_acquisition_type new acquisition type
 * \return true if succeed, false in case of error
 ****************************************************************************************************/
bool CameraControl::setAcquisitionType(NetAnswerGetSettings::AcquisitionType in_acquisition_type)
{
    DEB_MEMBER_FUNCT();

    int32_t            error           = 0    ;
    bool               result          = false;
    NetGenericHeader * second_packet   = NULL ;
    NetCommandHeader * command         = new NetCommandSetAcquisitionType();
    
    NetAnswerSetAcquisitionType  * answer_packet            = NULL ;
    NetCommandSetAcquisitionType * command_acquisition_type = dynamic_cast<NetCommandSetAcquisitionType *>(command);

    command_acquisition_type->m_acquisition_type = static_cast<uint8_t>(in_acquisition_type);

    // send the command and treat the acknowledge
    if(!sendCommandWithAck(command, error))
        goto done;

    // wait for the command done
    if(!waitCommandDonePacket(NetCommandHeader::g_function_number_set_acquisition_type, second_packet))
        goto done;

    // we need to manage the data 
    answer_packet = dynamic_cast<NetAnswerSetAcquisitionType *>(second_packet);

    if(!answer_packet->hasError())
    {
        m_acquisition_type = in_acquisition_type;
        result = true;
    }

done:
    if(second_packet != NULL) delete second_packet;
    if(command       != NULL) delete command      ;

    return result;
}

/****************************************************************************************************
 * \fn bool acquire(bool in_sync)
 * \brief  Start a new acquisition by sending a command to the hardware
 * \param  in_sync synchronous acquisition flag (if true will wait the command done reception)
 * \return true if succeed, false in case of error
 ****************************************************************************************************/
bool CameraControl::acquire(bool in_sync)
{
    DEB_MEMBER_FUNCT();

    int32_t            error         = 0    ;
    bool               result        = false;
    NetGenericHeader * second_packet = NULL ;
    NetAnswerAcquire * answer_packet = NULL ;
    NetCommandHeader * command       = new NetCommandAcquire();

    // first flush old InquireAcquisitionStatus packets (should not occur!)
    flushAcquisitionStatusPackets();

    // send the command and treat the acknowledge
    if(!sendCommandWithAck(command, error))
        goto done;

    if(in_sync)
    {
        // wait for the command done
        if(!waitCommandDonePacket(NetCommandHeader::g_function_number_acquire, second_packet))
            goto done;

        // we need to manage the data 
        answer_packet = dynamic_cast<NetAnswerAcquire *>(second_packet);

        if(!answer_packet->hasError())
        {
            result = true;
        }
    }
    else
    {
        result = true;
    }

done:
    if(second_packet != NULL) delete second_packet;
    if(command       != NULL) delete command      ;

    return result;
}

/****************************************************************************************************
 * \fn bool checkEndOfAcquisition()
 * \brief  Check if the acquisition is finished (command done received ?)
 * \param  out_error_occurred if the acquisition is finished, give an error state
 * \return true if the acquisition is finished, false if it is not
 ****************************************************************************************************/
bool CameraControl::checkEndOfAcquisition(bool & out_error_occurred)
{
    DEB_MEMBER_FUNCT();

    NetGenericHeader * packet = NULL;
    bool result = false;
    
    out_error_occurred = false;

    // check the command done reception
    if(getCommandDonePacket(NetCommandHeader::g_function_number_acquire, packet))
    {
        // we need to manage the data 
        NetAnswerAcquire * answer_packet = dynamic_cast<NetAnswerAcquire *>(packet);

        out_error_occurred = answer_packet->hasError();
        result = true;
    }

    if(packet != NULL) 
        delete packet;

    return result;
}

/****************************************************************************************************
 * \fn bool terminateAcquisition()
 * \brief  Stop the acquisition by sending a command to the hardware
 * \param  none
 * \return true if succeed, false in case of error
 ****************************************************************************************************/
bool CameraControl::terminateAcquisition()
{
    DEB_MEMBER_FUNCT();

    int32_t error  = 0    ;
    bool    result = false;

    NetGenericHeader              * first_packet  = NULL ;
    NetAnswerTerminateAcquisition * answer_packet = NULL ;
    NetCommandHeader              * command       = new NetCommandTerminateAcquisition();

    // send the command without no acknowledge
    if(!sendCommandWithoutAck(command, error))
        goto done;

    // wait for the command done
    if(!waitCommandDonePacket(NetCommandHeader::g_function_number_terminate_acquisition, first_packet))
        goto done;

    // we need to manage the settings data 
    answer_packet = dynamic_cast<NetAnswerTerminateAcquisition *>(first_packet);

    if(!answer_packet->hasError())
    {
        result = true;
    }

done:
    if(first_packet != NULL) delete first_packet;
    if(command      != NULL) delete command      ;

    return result;
}

/****************************************************************************************************
 * \fn bool terminateImageRetrieve()
 * \brief  Stop the image retrieve process by sending a command to the hardware
 * \param  none
 * \return true if succeed, false in case of error
 ****************************************************************************************************/
bool CameraControl::terminateImageRetrieve()
{
    DEB_MEMBER_FUNCT();

    int32_t error  = 0    ;
    bool    result = false;

    NetGenericHeader                * first_packet  = NULL ;
    NetAnswerTerminateImageRetrieve * answer_packet = NULL ;
    NetCommandHeader                * command       = new NetCommandTerminateImageRetrieve();

    // send the command without no acknowledge
    if(!sendCommandWithoutAck(command, error))
        goto done;

    // wait for the command done
    if(!waitCommandDonePacket(NetCommandHeader::g_function_number_terminate_image_retrieve, first_packet))
        goto done;

    // we need to manage the settings data 
    answer_packet = dynamic_cast<NetAnswerTerminateImageRetrieve *>(first_packet);

    if(!answer_packet->hasError())
    {
        result = true;
    }

done:
    if(first_packet != NULL) delete first_packet;
    if(command      != NULL) delete command      ;

    return result;
}

/****************************************************************************************************
 * \fn bool retrieveImage()
 * \brief  start the reception of the current image by sending a command to the hardware
 * \param  none
 * \return true if succeed, false in case of error
 ****************************************************************************************************/
bool CameraControl::retrieveImage()
{
    DEB_MEMBER_FUNCT();

    int32_t            error   = 0    ;
    bool               result  = false;
    NetCommandHeader * command = new NetCommandRetrieveImage();

    // first flush old image packets (should not occur!)
    flushImagePackets();

    // send the command and treat the acknowledge
    result = sendCommandWithAck(command, error);
    delete command;

    return result;
}

/****************************************************************************************************
 * \fn bool inquireAcquisitionStatus()
 * \brief  Inquire the acquisition status by sending a command to the hardware
 * \param  none
 * \return true if succeed, false in case of error
 ****************************************************************************************************/
bool CameraControl::inquireAcquisitionStatus()
{
    DEB_MEMBER_FUNCT();

    int32_t            error   = 0    ;
    bool               result  = false;
    NetCommandHeader * command = new NetCommandInquireAcquisitionStatus();

    // first flush old acquisition status packets (should not occur!)
    flushAcquisitionStatusPackets();

    // send the command without no acknowledge
    result = sendCommandWithoutAck(command, error);
    delete command;

    return result;
}

/****************************************************************************************************
 * \fn bool configurePackets(uint16_t in_pixels_per_packet, uint16_t in_packet_delay_usec)
 * \brief  Change the packets settings by sending a command to the hardware
 * \param  in_pixels_per_packet pixels per packet
 * \param  in_packet_delay_usec packet sending loop delay in microseconds
 * \return true if succeed, false in case of error
 ****************************************************************************************************/
bool CameraControl::configurePackets(uint16_t in_pixels_per_packet,
                                     uint16_t in_packet_delay_usec)
{
    DEB_MEMBER_FUNCT();

    int32_t            error           = 0    ;
    bool               result          = false;
    NetGenericHeader * second_packet   = NULL ;
    NetCommandHeader * command         = new NetCommandConfigurePackets();
    
    NetAnswerConfigurePackets  * answer_packet            = NULL ;
    NetCommandConfigurePackets * command_acquisition_type = dynamic_cast<NetCommandConfigurePackets *>(command);

    command_acquisition_type->m_pixels_per_packet = in_pixels_per_packet;
    command_acquisition_type->m_packet_delay_usec = in_packet_delay_usec;

    // send the command and treat the acknowledge
    if(!sendCommandWithAck(command, error))
        goto done;

    // wait for the command done
    if(!waitCommandDonePacket(NetCommandHeader::g_function_number_configure_packets, second_packet))
        goto done;

    // we need to manage the data 
    answer_packet = dynamic_cast<NetAnswerConfigurePackets *>(second_packet);

    if(!answer_packet->hasError())
    {
        result = true;
    }

done:
    if(second_packet != NULL) delete second_packet;
    if(command       != NULL) delete command      ;

    return result;
}


/****************************************************************************************************
 * \fn bool setCoolingValue(uint32_t in_cooling_value)
 * \brief  change the cooling value by sending a command to the hardware
 * \param  in_cooling_value  cooling value
 * \return true if succeed, false in case of error
 ****************************************************************************************************/
bool CameraControl::setCoolingValue(uint8_t in_cooling_value)
{
    DEB_MEMBER_FUNCT();

    int32_t                error           = 0    ;
    bool                   result          = false;
    NetGenericHeader     * second_packet   = NULL ;
    NetCommandHeader     * command         = new NetCommandSetCoolingValue();
    
    NetAnswerSetCoolingValue  * answer_packet         = NULL ;
    NetCommandSetCoolingValue * command_cooling_value = dynamic_cast<NetCommandSetCoolingValue *>(command);

    command_cooling_value->m_cooling_value = static_cast<bool>(in_cooling_value); 

    // send the command and treat the acknowledge
    if(!sendCommandWithAck(command, error))
        goto done;

    // wait for the command done
    if(!waitCommandDonePacket(NetCommandHeader::g_function_number_set_cooling_value, second_packet))
        goto done;

    // we need to manage the data 
    answer_packet = dynamic_cast<NetAnswerSetCoolingValue *>(second_packet);

    if(!answer_packet->hasError())
    {
        m_cooling_value = in_cooling_value;
        result = true;
    }

done:
    if(second_packet != NULL) delete second_packet;
    if(command       != NULL) delete command      ;

    return result;
}


/****************************************************************************************************
 * \fn bool setReadoutSpeedValue(uint32_t& readout_speed_value)
 * \brief  change the readout speed value by sending a command to the hardware
 * \param  readout_speed_value  readout speed value
 * \return true if succeed, false in case of error
 ****************************************************************************************************/
bool CameraControl::setReadoutSpeedValue(uint32_t readout_speed_value)
{
    DEB_MEMBER_FUNCT();

    int32_t                error           = 0    ;
    bool                   result          = false;
    NetGenericHeader     * second_packet   = NULL ;
    NetCommandHeader     * command         = new NetCommandSetReadoutSpeedValue();
    
    NetAnswerSetReadoutSpeedValue  * answer_packet         = NULL ;
    NetCommandSetReadoutSpeedValue * command_readout_speed_value = dynamic_cast<NetCommandSetReadoutSpeedValue *>(command);

    command_readout_speed_value->m_readout_speed_value = static_cast<ushort>(readout_speed_value); 

    // send the command and treat the acknowledge
    if(!sendCommandWithAck(command, error))
        goto done;

    // wait for the command done
    if(!waitCommandDonePacket(NetCommandHeader::g_function_number_set_single_parameter, second_packet))
        goto done;

    // we need to manage the data 
    answer_packet = dynamic_cast<NetAnswerSetReadoutSpeedValue *>(second_packet);

    if(!answer_packet->hasError())
    {
        result = true;
    }

done:
    if(second_packet != NULL) delete second_packet;
    if(command       != NULL) delete command      ;

    return result;
}

/****************************************************************************************************
 * \fn uint8_t getCoolingValue() const
 * \brief  get the Cooling value
 * \param  none
 * \return Cooling value
 ****************************************************************************************************/
uint8_t CameraControl::getCoolingValue() const
{
    return  static_cast<uint8_t>(m_cooling_value);
}

/**************************************************************************************************
 * SINGLETON MANAGEMENT
 **************************************************************************************************/
/****************************************************************************************************
 * \fn void create(const CameraControlInit & in_init_parameters)
 * \brief  Create the singleton instance
 * \param  in_init_parameters init parameters (timeout, delays, ...)
 * \return none
 ****************************************************************************************************/
void CameraControl::create(const CameraControlInit & in_init_parameters)
{
    init(new CameraControl(in_init_parameters));
}

//###########################################################################