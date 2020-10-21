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
 * \file   CameraControl.h
 * \brief  header file of detector communication class.
 * \author Cédric Castel - SOLEIL (MEDIANE SYSTEME - IT consultant) 
 * \date   Created on October 19, 2020
 ****************************************************************************************************/

#ifndef SPECTRALCAMERACONTROL_H
#define SPECTRALCAMERACONTROL_H

// SYSTEM
#include <cstdlib>
#include <cstring>
#include <stdint.h>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>
#include <sstream>
#include <unistd.h>
#include <stdexcept>
#include <sys/time.h>
#include <vector>
#include <cmath>
#include <map>
#include <string>
#include <stdarg.h>
#include <iomanip>
#include <algorithm>
#include <netinet/in.h>

// PROJECT
#include "SpectralCompatibility.h"
#include "CameraSingleton.h"
#include "NetPackets.h"

// LIMA 
#include "lima/Debug.h"
#include "lima/AutoObj.h"

/*
 *  \namespace lima
 */
namespace lima
{
/*
 *  \namespace SpectralDetector_ns
 */
namespace SpectralDetector_ns 
{
/*
 *  \class CameraControl
 *  \brief This class is used to communicate with the detector software
 */
class CameraControl : public CameraSingleton<CameraControl>
{
    DEB_CLASS_NAMESPC(DebModCamera, "CameraControl", "Spectral");

    // we need to gain access to the destructor for yat::SharedPtr management of our singleton
    friend class lima::AutoPtr<CameraControl>;

    public:
        // configure the connection timeout in seconds
        void SetConnectionTimeout(int in_connection_timeout_sec);

        // configure the camera identifier
        void SetCameraIdentifier(int in_camera_identifier);

        // connect the instance to the detector software (tcp/ip)
        void connect(const std::string & in_hostname, int in_port);

        // disconnect the instance of the detector software
        void disconnect(void);

        // Send a command to the detector
        bool sendCommand(NetCommandHeader * in_out_command, int32_t & out_error);

        // Receive a SI Image SGL II packet
        bool receivePacket(NetGenericHeader * & out_packet, int32_t & out_error);

       /**************************************************************************************************
        * COMMANDS MANAGEMENT
        **************************************************************************************************/
        // Read the current status
        bool getStatus();

       /***************************************************************************************************
        * SINGLETON MANAGEMENT
        ***************************************************************************************************/
        // Create the singleton instance
        static void create();

    private:
        // constructor
        explicit CameraControl();

        // destructor (needs to be virtual)
        virtual ~CameraControl();

        // execute a not blocking connect
        bool not_blocking_connect(struct sockaddr_in & in_out_sa, int sock, int timeout);

    protected:
        // send a tcp/ip packet
        bool send(const std::string          & in_command_name,
                  const std::vector<uint8_t> & in_net_buffer  ,
                  int32_t                    & out_error      );

        // Receive a tcp/ip packet
        bool receive(uint8_t * out_buffer, const int in_buffer_lenght, int32_t & out_error);

        // Receive a SI Image SGL II sub packet (a part of the complete packet)
        bool receiveSubPacket(NetGenericHeader * out_packet, std::vector<uint8_t> & in_out_net_buffer, int32_t & out_error);

        // Receive a SI Image SGL II sub packet (a part of the complete packet but not the first one)
        bool receiveGenericSubPacket(const NetGenericHeader     & in_packet        ,
                                     NetGenericHeader           * out_packet       , 
                                     std::vector<uint8_t>       & in_out_net_buffer,
                                     bool                         in_final_packet  ,       
                                     int32_t                    & out_error        );

        // Receive a SI Image SGL II sub packet with a specific lenght
        bool receiveSpecificSubPacket(const NetGenericHeader & in_packet        ,
                                      const NetGenericAnswer & in_answer_packet ,
                                      NetGenericHeader       * out_packet       , 
                                      std::vector<uint8_t>   & in_out_net_buffer,
                                      int32_t                & out_error        );

        // Fill the final packet with the data stored in the buffer
        bool FillFullPacket(NetGenericHeader           * out_packet   , 
                            const std::vector<uint8_t> & in_net_buffer,
                            int32_t                    & out_error    );

    private:
        // socket for commands and answers
        int m_sock;

        // address of remote server
        struct sockaddr_in m_server_name; 
        
        // true if connected
        bool m_is_connected; 

        // connection timeout in seconds
        int m_connection_timeout_sec;

        // camera identifier (starts at 1)
        int m_camera_identifier;
};

} // namespace SpectralDetector_ns
} // namespace lima

#endif // SPECTRALCAMERACONTROL_H
