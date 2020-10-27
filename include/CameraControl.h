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
#include "ProtectedList.h"
#include "NetPacketsGroups.h"

// LIMA 
#include "lima/Debug.h"

/*
 *  \namespace lima
 */
namespace lima
{
/*
 *  \namespace Spectral
 */
namespace Spectral 
{
/*
 *  \class CameraControl
 *  \brief This class is used to communicate with the detector software
 */
class CameraControl : public CameraSingleton<CameraControl>
{
    DEB_CLASS_NAMESPC(DebModCamera, "CameraControl", "Spectral");

    // we need to gain access to the destructor for the management of our singleton
    friend class CameraSingleton<CameraControl>;

    public:
        // detector status values
        typedef enum DetectorStatus
        {
            Ready      , // ready to start acquisition
            Exposure   , // running an exposure
            Readout    , // running a readout
            Latency    , // running a latency
            Fault      , // acquisition stopped externally or unexpected error 

        } DetectorStatus;

    public:
        // configure the connection timeout in seconds
        void setConnectionTimeout(int in_connection_timeout_sec);

        // configure the reception timeout in seconds
        void setReceptionTimeout(int in_reception_timeout_sec);

        // configure the camera identifier
        void setCameraIdentifier(int in_camera_identifier);

        // get the latest hardware status
        CameraControl::DetectorStatus getLatestStatus() const;

        // get the detector model
        const std::string & getModel() const;

        // get the detector serial number
        const std::string & getSerialNumber() const;

        // get the detector maximum width
        std::size_t getWidthMax() const;

        // get the detector maximum height
        std::size_t getHeightMax() const;

        // get the detector pixel depth
        std::size_t getPixelDepth() const;

        // get the exposure time in milli-seconds
        uint32_t getExposureTimeMsec() const;

        // get the number of frames to acquire
        uint32_t getNbImagesToAcquire() const;

        // get the acquisition type
        NetAnswerGetSettings::AcquisitionType getAcquisitionType() const;

        // get the CCD Format Serial Origin
        std::size_t getSerialOrigin() const;

        // get the CCD Format Serial Length
        std::size_t getSerialLength() const;

        // get the CCD Format Serial Binning
        std::size_t getSerialBinning() const;

        // get the CCD Format Parallel Origin
        std::size_t getParallelOrigin() const;

        // get the CCD Format Parallel Length
        std::size_t getParallelLength() const;

        // get the CCD Format Parallel Binning
        std::size_t getParallelBinning() const;

        // connect the instance to the detector software (tcp/ip)
        void connect(const std::string & in_hostname, int in_port);

        // disconnect the instance of the detector software
        void disconnect(void);

        // Receive a SI Image SGL II packet
        bool receivePacket(NetGenericHeader * & out_packet, int32_t & out_error);

        // Add a new packet to the packets container (the instance will be freed by the container or a consumer)
        void addPacket(NetGenericHeader * in_packet);

        // Wait for a new ack packet to be received
        bool waitAcknowledgePacket(NetGenericHeader * & out_packet);

        // Wait for a new image packet to be received
        bool waitImagePacket(NetGenericHeader * & out_packet);

        // Wait for a new data packet to be received
        bool waitDataPacket(uint16_t in_data_type, NetGenericHeader * & out_packet);

       /**************************************************************************************************
        * COMMANDS MANAGEMENT
        **************************************************************************************************/
        // Update the current status by sending a command to the hardware
        bool updateStatus();

        // Init some static data (model, serial number, max width, max lenght, pixel depths)
        bool initCameraParameters();

        // Update the settings by sending a command to the hardware
        bool updateSettings();

       /***************************************************************************************************
        * SINGLETON MANAGEMENT
        ***************************************************************************************************/
        // Create the singleton instance
        static void create();

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

       /**************************************************************************************************
        * COMMANDS MANAGEMENT
        **************************************************************************************************/
        // Send a command to the detector and wait for the acknowledge
        bool sendCommandWithAck(NetCommandHeader * in_out_command, int32_t & out_error);

        // Send a command to the detector and does not wait an acknowledge (only special commands)
        bool sendCommandWithoutAck(NetCommandHeader * in_out_command, int32_t & out_error);

    private:
        // constructor
        explicit CameraControl();

        // destructor (needs to be virtual)
        virtual ~CameraControl();

        // execute a not blocking connect
        bool notBlockingConnect(struct sockaddr_in & in_out_sa, int sock, int timeout);

        // search and find a line which has the given key
        static bool findLineWithKey(const std::string & in_lines ,
                                    const std::string & in_key   ,
                                    std::string       & out_line );

        // Search and find a line which has the two given keys
        static bool findLineWithTwoKey(const std::string & in_lines     ,
                                       const std::string & in_first_key ,
                                       const std::string & in_second_key,
                                       const std::string & in_delimiter ,
                                       std::string       & out_line     );

        // Cut a substring with a position and delimiter
        static bool getSubString(const std::string & in_string     ,
                                 std::size_t         in_pos        ,
                                 const std::string & in_delimiter  ,
                                 std::string       & out_sub_string);

        // Convert a string to an integer
        static bool convertStringToInt(const std::string & in_string, int & out_value);

        // Wait for a new packet to be received
        bool waitPacket(NetPacketsGroupId in_group_id, NetGenericHeader * & out_packet);

        // Send a command to the detector
        bool sendCommand(NetCommandHeader * in_out_command, int32_t & out_error);

        // creates an autolock mutex for sendCommand access
        lima::AutoMutex sendCommandLock() const;

    private:
        // socket for commands and answers
        int m_sock;

        // address of remote server
        struct sockaddr_in m_server_name; 
        
        // true if connected
        bool m_is_connected; 

        // connection timeout in seconds
        int m_connection_timeout_sec;

        // reception timeout in seconds
        int m_reception_timeout_sec;

        // camera identifier (starts at 1)
        int m_camera_identifier;

        // latest detector status (periodically updated)
        DetectorStatus m_latest_status;

        // detector model
        std::string m_model;

        // detector serial number
        std::string m_serial_number;

        // maximum width
        std::size_t m_width_max;

        // maximum height
        std::size_t m_height_max;

        // pixel depth in bits
        std::size_t m_pixel_depth;

        // exposure time in milli-seconds
        uint32_t m_exposure_time_msec; 

        // Number of Frames to Acquire
        uint32_t m_nb_images_to_acquire;

        // SI Image SGL II Acquisition Type
        NetAnswerGetSettings::AcquisitionType m_acquisition_type;

        // CCD Format Serial Origin
        std::size_t  m_serial_origin; 

        // CCD Format Serial Length
        std::size_t  m_serial_length; 

        // CCD Format Serial Binning
        std::size_t  m_serial_binning; 

        // CCD Format Parallel Origin
        std::size_t  m_parallel_origin; 

        // CCD Format Parallel Length
        std::size_t  m_parallel_length; 

        // CCD Format Parallel Binning
        std::size_t  m_parallel_binning; 

        // packets container
        NetPacketsGroups m_packets_container;

        // condition variable used to protect the sendCommand
        mutable lima::Cond m_send_command_cond;
};

} // namespace Spectral
} // namespace lima

#endif // SPECTRALCAMERACONTROL_H
