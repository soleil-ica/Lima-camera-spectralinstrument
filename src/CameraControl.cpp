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