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
 * \file   NetPackets.h
 * \brief  header file of network packets classes.
 * \author Cédric Castel - SOLEIL (MEDIANE SYSTEME - IT consultant) 
 * \date   Created on October 20, 2020
 ****************************************************************************************************/

#ifndef SPECTRALNETPACKETS_H
#define SPECTRALNETPACKETS_H

// SYSTEM
#include <cstdlib>
#include <cstring>
#include <string>
#include <stdint.h>
#include <vector>

// LIMA
#include "lima/SizeUtils.h"

// PROJECT
#include "SpectralCompatibility.h"

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
 *  \class NetGenericHeader
 *  \brief This class is a generic header packet class
 */
class NetGenericHeader
{
    friend class CameraControl   ;
    friend class NetPacketsGroups;

public:
    // constructor
    NetGenericHeader();

    // check if this is a command packet
    bool isCommandPacket() const;

    // check if this is a acknowledge packet
    bool isAcknowledgePacket() const;

    // check if this is a data packet
    bool isDataPacket() const;

    // check if this is an image packet
    bool isImagePacket() const;

    //-----------------------
    // not recursive methods
    //-----------------------
    // get the specific packet size
    virtual std::size_t size() const;

    // read the values stored into a memory block and fill them into the class members
    virtual bool read(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size);

    // write the class members values into a memory block
    virtual bool write(uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) const;

    // log the class content
    virtual void log() const;

    //-----------------------
    // recursive methods
    //-----------------------
    // get the total packet size
    virtual std::size_t totalSize() const;

    // totally read the values stored into a memory block and fill them into the class members
    virtual bool totalRead(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size);

    // totally write the class members values into a memory block
    virtual bool totalWrite(uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) const;

    // totally log the classes content (recursive)
    virtual void totalLog() const;

    //-----------------------
    // template methods
    //-----------------------
    // read template pattern (only a specialized version can be used)
    template <typename T>
    void readData(const uint8_t * & in_out_memory_data, T & out_class_data);

    // write template pattern (only a specialized version can be used)
    template <typename T>
    void writeData(uint8_t * & in_out_memory_data, T in_class_data) const;

protected:
    // packet name used during logging
    std::string m_packet_name;

    uint32_t m_packet_lenght    ; // total number of bytes in packet
    uint8_t  m_packet_identifier; 
    uint8_t  m_camera_identifier; // 0 for server commands, Camera number (1..max)

    static const uint8_t g_server_command_identifier        ; // to set Camera identifier
    static const uint8_t g_packet_identifier_for_command    ; // to set Packet identifier
    static const uint8_t g_packet_identifier_for_acknowledge; // to set Packet identifier
    static const uint8_t g_packet_identifier_for_data       ; // to set Packet identifier
    static const uint8_t g_packet_identifier_for_image      ; // to set Packet identifier
};

//------------------------------------------------------------
// specialized template methods for 8 bits unsigned integers
//------------------------------------------------------------
// fill the class data with the read value of the memory data. Also, jump to the next value.
template <> void NetGenericHeader::readData<uint8_t>(const uint8_t * & in_out_memory_data, uint8_t & out_class_data);

// fill the memory data with the class data value. Also, jump to the next value.
template <> void NetGenericHeader::writeData<uint8_t>(uint8_t * & in_out_memory_data, uint8_t in_class_data) const;

//------------------------------------------------------------
// specialized template methods for 16 bits signed integers
//------------------------------------------------------------
// fill the class data with the read value of the memory data. Also, jump to the next value.
template <> void NetGenericHeader::readData<int16_t>(const uint8_t * & in_out_memory_data, int16_t & out_class_data);

// fill the memory data with the class data value. Also, jump to the next value.
template <> void NetGenericHeader::writeData<int16_t>(uint8_t * & in_out_memory_data, int16_t in_class_data) const;

//------------------------------------------------------------
// specialized template methods for 16 bits unsigned integers
//------------------------------------------------------------
// fill the class data with the read value of the memory data. Also, jump to the next value.
template <> void NetGenericHeader::readData<uint16_t>(const uint8_t * & in_out_memory_data, uint16_t & out_class_data);

// fill the memory data with the class data value. Also, jump to the next value.
template <> void NetGenericHeader::writeData<uint16_t>(uint8_t * & in_out_memory_data, uint16_t in_class_data) const;

//------------------------------------------------------------
// specialized template methods for 32 bits signed integers
//------------------------------------------------------------
// fill the class data with the read value of the memory data. Also, jump to the next value.
template <> void NetGenericHeader::readData<int32_t>(const uint8_t * & in_out_memory_data, int32_t & out_class_data);

// fill the memory data with the class data value. Also, jump to the next value.
template <> void NetGenericHeader::writeData<int32_t>(uint8_t * & in_out_memory_data, int32_t in_class_data) const;

//------------------------------------------------------------
// specialized template methods for 32 bits unsigned integers
//------------------------------------------------------------
// fill the class data with the read value of the memory data. Also, jump to the next value.
template <> void NetGenericHeader::readData<uint32_t>(const uint8_t * & in_out_memory_data, uint32_t & out_class_data);

// fill the memory data with the class data value. Also, jump to the next value.
template <> void NetGenericHeader::writeData<uint32_t>(uint8_t * & in_out_memory_data, uint32_t in_class_data) const;

//------------------------------------------------------------
// specialized template methods for double
//------------------------------------------------------------
// fill the class data with the read value of the memory data. Also, jump to the next value.
template <> void NetGenericHeader::readData<double>(const uint8_t * & in_out_memory_data, double & out_class_data);

// fill the memory data with the class data value. Also, jump to the next value.
template <> void NetGenericHeader::writeData<double>(uint8_t * & in_out_memory_data, double in_class_data) const;

/*
 *  \class NetCommandHeader
 *  \brief This class is a header command packet class
 */
class NetCommandHeader : public NetGenericHeader
{
    friend class CameraControl;
    friend class NetPacketsGroups;

public:
    // constructor
    NetCommandHeader();

    // init the packet lenght member
    void initPacketLenght();

    // init the camera identifier member
    void initCameraIdentifier(uint8_t in_camera_identifier);

    // init the specific data lenght
    void initSpecificDataLenght();

    //-----------------------
    // not recursive methods
    //-----------------------
    // get the specific packet size
    virtual std::size_t size() const;

    // read the values stored into a memory block and fill them into the class members
    virtual bool read(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size);

    // write the class members values into a memory block
    virtual bool write(uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) const;

    // log the class content
    virtual void log() const;

    //-----------------------
    // recursive methods
    //-----------------------
    // get the total packet size
    virtual std::size_t totalSize() const;

    // totally read the values stored into a memory block and fill them into the class members
    virtual bool totalRead(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size);

    // totally write the class members values into a memory block
    virtual bool totalWrite(uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) const;

    // totally log the classes content (recursive)
    virtual void totalLog() const;

protected:
    uint16_t m_function_number     ; // function to be executed (1000 .. 1999)
    uint16_t m_specific_data_lenght; // length of parameter block following (0, if none)
    bool     m_is_server_command   ; // some commands are server related, others to a camera (used to set the m_camera_identifier member)

    static const uint16_t g_function_number_get_status           ; // to set Function number
    static const uint16_t g_function_number_get_camera_parameters; // to set Function number
    static const uint16_t g_function_number_get_settings         ; // to set Function number

    static const uint16_t g_function_number_set_acquisition_mode ; // to set Function number
    static const uint16_t g_function_number_set_exposure_time    ; // to set Function number
    static const uint16_t g_function_number_set_format_parameters; // to set Function number
    static const uint16_t g_function_number_set_acquisition_type ; // to set Function number

    static const uint16_t g_function_number_acquire                   ; // to set Function number
    static const uint16_t g_function_number_terminate_acquisition     ; // to set Function number
    static const uint16_t g_function_number_retrieve_image            ; // to set Function number
    static const uint16_t g_function_number_terminate_image_retrieve  ; // to set Function number
    static const uint16_t g_function_number_inquire_acquisition_status; // to set Function number
    static const uint16_t g_function_number_configure_packets         ; // to set Function number
};

/*
 *  \class NetCommandGetStatus
 *  \brief This class is a Get Status command packet class (there is no specific data for this command)
 */
class NetCommandGetStatus : public NetCommandHeader
{
    friend class CameraControl;    

public:
    // constructor
    NetCommandGetStatus();
};

/*
 *  \class NetCommandGetCameraParameters
 *  \brief This class is a Get Camera Parameters command packet class (there is no specific data for this command)
 */
class NetCommandGetCameraParameters : public NetCommandHeader
{
    friend class CameraControl;    

public:
    // constructor
    NetCommandGetCameraParameters();
};

/*
 *  \class NetCommandGetSettings
 *  \brief This class is a Get Settings command packet class (there is no specific data for this command)
 */
class NetCommandGetSettings : public NetCommandHeader
{
    friend class CameraControl;    

public:
    // constructor
    NetCommandGetSettings();
};

/*
 *  \class NetCommandSetAcquisitionMode
 *  \brief This class is a Set Acquisition Mode packet class
 */
class NetCommandSetAcquisitionMode : public NetCommandHeader
{
    friend class CameraControl;    

public:
    // constructor
    NetCommandSetAcquisitionMode();

    //-----------------------
    // not recursive methods
    //-----------------------
    // get the specific packet size
    virtual std::size_t size() const;

    // read the values stored into a memory block and fill them into the class members
    virtual bool read(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size);

    // write the class members values into a memory block
    virtual bool write(uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) const;

    // log the class content
    virtual void log() const;

    //-----------------------
    // recursive methods
    //-----------------------
    // get the total packet size
    virtual std::size_t totalSize() const;

    // totally read the values stored into a memory block and fill them into the class members
    virtual bool totalRead(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size);

    // totally write the class members values into a memory block
    virtual bool totalWrite(uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) const;

    // totally log the classes content (recursive)
    virtual void totalLog() const;

protected:
    uint8_t m_acquisition_mode; // SI Image SGL II Acquisition Mode (in 8 bits this time!)
};

/*
 *  \class NetCommandSetExposureTime
 *  \brief This class is a Set Exposure Time packet class
 */
class NetCommandSetExposureTime : public NetCommandHeader
{
    friend class CameraControl;    

public:
    // constructor
    NetCommandSetExposureTime();

    //-----------------------
    // not recursive methods
    //-----------------------
    // get the specific packet size
    virtual std::size_t size() const;

    // read the values stored into a memory block and fill them into the class members
    virtual bool read(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size);

    // write the class members values into a memory block
    virtual bool write(uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) const;

    // log the class content
    virtual void log() const;

    //-----------------------
    // recursive methods
    //-----------------------
    // get the total packet size
    virtual std::size_t totalSize() const;

    // totally read the values stored into a memory block and fill them into the class members
    virtual bool totalRead(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size);

    // totally write the class members values into a memory block
    virtual bool totalWrite(uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) const;

    // totally log the classes content (recursive)
    virtual void totalLog() const;

protected:
    double m_exposure_time_sec; // exposure time (in seconds this time!)
};

/*
 *  \class NetCommandSetFormatParameters
 *  \brief This class is a Set Format Parameters packet class
 */
class NetCommandSetFormatParameters : public NetCommandHeader
{
    friend class CameraControl;    

public:
    // constructor
    NetCommandSetFormatParameters();

    //-----------------------
    // not recursive methods
    //-----------------------
    // get the specific packet size
    virtual std::size_t size() const;

    // read the values stored into a memory block and fill them into the class members
    virtual bool read(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size);

    // write the class members values into a memory block
    virtual bool write(uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) const;

    // log the class content
    virtual void log() const;

    //-----------------------
    // recursive methods
    //-----------------------
    // get the total packet size
    virtual std::size_t totalSize() const;

    // totally read the values stored into a memory block and fill them into the class members
    virtual bool totalRead(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size);

    // totally write the class members values into a memory block
    virtual bool totalWrite(uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) const;

    // totally log the classes content (recursive)
    virtual void totalLog() const;

protected:
    int32_t  m_serial_origin       ; // CCD Format Serial Origin
    int32_t  m_serial_length       ; // CCD Format Serial Length
    int32_t  m_serial_binning      ; // CCD Format Serial Binning
    int32_t  m_parallel_origin     ; // CCD Format Parallel Origin
    int32_t  m_parallel_length     ; // CCD Format Parallel Length
    int32_t  m_parallel_binning    ; // CCD Format Parallel Binning
};

/*
 *  \class NetCommandSetAcquisitionType
 *  \brief This class is a Set Acquisition Type packet class
 */
class NetCommandSetAcquisitionType : public NetCommandHeader
{
    friend class CameraControl;    

public:
    // constructor
    NetCommandSetAcquisitionType();

    //-----------------------
    // not recursive methods
    //-----------------------
    // get the specific packet size
    virtual std::size_t size() const;

    // read the values stored into a memory block and fill them into the class members
    virtual bool read(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size);

    // write the class members values into a memory block
    virtual bool write(uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) const;

    // log the class content
    virtual void log() const;

    //-----------------------
    // recursive methods
    //-----------------------
    // get the total packet size
    virtual std::size_t totalSize() const;

    // totally read the values stored into a memory block and fill them into the class members
    virtual bool totalRead(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size);

    // totally write the class members values into a memory block
    virtual bool totalWrite(uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) const;

    // totally log the classes content (recursive)
    virtual void totalLog() const;

protected:
    uint8_t m_acquisition_type; // SI Image SGL II Acquisition type (in 8 bits this time!)
};

/*
 *  \class NetCommandAcquire
 *  \brief This class is a Acquire command packet class (there is no specific data for this command)
 */
class NetCommandAcquire : public NetCommandHeader
{
    friend class CameraControl;    

public:
    // constructor
    NetCommandAcquire();
};

/*
 *  \class NetCommandTerminateAcquisition
 *  \brief This class is a Terminate Acquisition command packet class (there is no specific data for this command)
 */
class NetCommandTerminateAcquisition : public NetCommandHeader
{
    friend class CameraControl;    

public:
    // constructor
    NetCommandTerminateAcquisition();
};

/*
 *  \class NetCommandTerminateImageRetrieve
 *  \brief This class is a Terminate Image Retrieve command packet class (there is no specific data for this command)
 */
class NetCommandTerminateImageRetrieve : public NetCommandHeader
{
    friend class CameraControl;    

public:
    // constructor
    NetCommandTerminateImageRetrieve();

protected:
};

/*
 *  \class NetCommandRetrieveImage
 *  \brief This class is a Retrieve Image packet class
 */
class NetCommandRetrieveImage : public NetCommandHeader
{
    friend class CameraControl;    
    friend class NetImage;

    // image transfert type values
    typedef enum TransfertType
    {
        TransfertU16 = 0,
        TransfertI16 = 1,
        TransfertI32 = 3,
        TransfertSGL = 4,

    } TransfertType;

public:
    // constructor
    NetCommandRetrieveImage();

    //-----------------------
    // not recursive methods
    //-----------------------
    // get the specific packet size
    virtual std::size_t size() const;

    // read the values stored into a memory block and fill them into the class members
    virtual bool read(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size);

    // write the class members values into a memory block
    virtual bool write(uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) const;

    // log the class content
    virtual void log() const;

    //-----------------------
    // recursive methods
    //-----------------------
    // get the total packet size
    virtual std::size_t totalSize() const;

    // totally read the values stored into a memory block and fill them into the class members
    virtual bool totalRead(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size);

    // totally write the class members values into a memory block
    virtual bool totalWrite(uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) const;

    // totally log the classes content (recursive)
    virtual void totalLog() const;

protected:
    uint16_t m_transfert_type; // SI Image SGL II image transfert type
};

/*
 *  \class NetCommandInquireAcquisitionStatus
 *  \brief This class is a Inquire Acquisition Status command packet class (there is no specific data for this command)
 */
class NetCommandInquireAcquisitionStatus : public NetCommandHeader
{
    friend class CameraControl;    

public:
    // constructor
    NetCommandInquireAcquisitionStatus();
};

/*
 *  \class NetCommandConfigurePackets
 *  \brief This class is a Configure Packets command packet class
 */
class NetCommandConfigurePackets : public NetCommandHeader
{
    friend class CameraControl;    

public:
    // constructor
    NetCommandConfigurePackets();

    //-----------------------
    // not recursive methods
    //-----------------------
    // get the specific packet size
    virtual std::size_t size() const;

    // read the values stored into a memory block and fill them into the class members
    virtual bool read(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size);

    // write the class members values into a memory block
    virtual bool write(uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) const;

    // log the class content
    virtual void log() const;

    //-----------------------
    // recursive methods
    //-----------------------
    // get the total packet size
    virtual std::size_t totalSize() const;

    // totally read the values stored into a memory block and fill them into the class members
    virtual bool totalRead(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size);

    // totally write the class members values into a memory block
    virtual bool totalWrite(uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) const;

    // totally log the classes content (recursive)
    virtual void totalLog() const;

protected:
    uint16_t m_pixels_per_packet; // pixels per packet
    uint16_t m_packet_delay_usec; // packet sending loop delay in microseconds
};

/*
 *  \class NetAcknowledge
 *  \brief This class is an acknowledge packet class
 */
class NetAcknowledge : public NetGenericHeader
{
    friend class CameraControl   ;
    friend class NetPacketsGroups;

public:
    // constructor
    NetAcknowledge();

    // check if the command was accepted
    bool wasAccepted() const;

    //-----------------------
    // not recursive methods
    //-----------------------
    // get the specific packet size
    virtual std::size_t size() const;

    // read the values stored into a memory block and fill them into the class members
    virtual bool read(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size);

    // write the class members values into a memory block
    virtual bool write(uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) const;

    // log the class content
    virtual void log() const;

    //-----------------------
    // recursive methods
    //-----------------------
    // get the total packet size
    virtual std::size_t totalSize() const;

    // totally read the values stored into a memory block and fill them into the class members
    virtual bool totalRead(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size);

    // totally write the class members values into a memory block
    virtual bool totalWrite(uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) const;

    // totally log the classes content (recursive)
    virtual void totalLog() const;

protected:
    uint16_t m_accepted_flag; // true (!0) or false (0)
};

/*
 *  \class NetGenericAnswer
 *  \brief This class is an generic answer packet class
 */
class NetGenericAnswer : public NetGenericHeader
{
    friend class CameraControl   ;
    friend class NetPacketsGroups;

public:
    // constructor
    NetGenericAnswer();

    // check if this is a command done packet
    bool isCommandDonePacket() const;

    // check if this is a get status packet
    bool isGetStatusPacket() const;

    // check if this is a get camera parameters
    bool isGetCameraParameters() const;

    // check if this is a get settings
    bool isGetSettings() const;

    // check if this is an acquisition status
    bool isAcquisitionStatus() const;

    // check if there is an error
    bool hasError() const;

    //-----------------------
    // not recursive methods
    //-----------------------
    // get the specific packet size
    virtual std::size_t size() const;

    // read the values stored into a memory block and fill them into the class members
    virtual bool read(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size);

    // write the class members values into a memory block
    virtual bool write(uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) const;

    // log the class content
    virtual void log() const;

    //-----------------------
    // recursive methods
    //-----------------------
    // get the total packet size
    virtual std::size_t totalSize() const;

    // totally read the values stored into a memory block and fill them into the class members
    virtual bool totalRead(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size);

    // totally write the class members values into a memory block
    virtual bool totalWrite(uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) const;

    // totally log the classes content (recursive)
    virtual void totalLog() const;

protected:
    int32_t  m_error_code          ; // 0 = no error
    uint16_t m_data_type           ; // 2000 .. 2999
    int32_t  m_specific_data_lenght; // 0 = no data

    static const uint16_t g_data_type_get_status           ; // to check the Data type
    static const uint16_t g_data_type_get_camera_parameters; // to check the Data type
    static const uint16_t g_data_type_get_settings         ; // to check the Data type
    static const uint16_t g_data_type_command_done         ; // to check the Data type
    static const uint16_t g_data_type_acquisition_status   ; // to check the Data type
};

/*
 *  \class NetAnswerGenericString
 *  \brief This class is a generic string answer packet class
 */
class NetAnswerGenericString : public NetGenericAnswer
{
    friend class CameraControl;    

public:
    // constructor
    NetAnswerGenericString();

    //-----------------------
    // not recursive methods
    //-----------------------
    // get the specific packet size
    virtual std::size_t size() const;

    // read the values stored into a memory block and fill them into the class members
    virtual bool read(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size);

    // write the class members values into a memory block
    virtual bool write(uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) const;

    // log the class content
    virtual void log() const;

    //-----------------------
    // recursive methods
    //-----------------------
    // get the total packet size
    virtual std::size_t totalSize() const;

    // totally read the values stored into a memory block and fill them into the class members
    virtual bool totalRead(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size);

    // totally write the class members values into a memory block
    virtual bool totalWrite(uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) const;

    // totally log the classes content (recursive)
    virtual void totalLog() const;

protected:
    std::string m_value;
};

/*
 *  \class NetAnswerGetStatus
 *  \brief This class is a Get Status answer packet class
 */
class NetAnswerGetStatus : public NetAnswerGenericString
{
    friend class CameraControl;    

public:
    // hardware status values
    typedef enum HardwareStatus
    {
        CameraConnected        = 1  ,
        AcquisitionInProgress  = 2  ,
        WaitForTrigger         = 4  ,
        TriggerReceived        = 8  ,
        ServerSimulatorData    = 16 ,
        InterfaceSimulatorData = 32 ,
        ConfigurationLoaded    = 64 ,
        ConfigurationError     = 128,

    } HardwareStatus;

    // constructor
    NetAnswerGetStatus();

    //-----------------------
    // not recursive methods
    //-----------------------
    // log the class content
    virtual void log() const;

    //-----------------------
    // recursive methods
    //-----------------------
    // totally log the classes content (recursive)
    virtual void totalLog() const;

protected:
    // status name of the key to read in the complete status string returned by the detector
    static const std::string g_server_flags_status_name;

    // delimiter used in a key status (name,value,unity)
    static const std::string g_server_flags_delimiter;

    // position of a value in a key status (name,value,unity)
    static const std::size_t g_server_flags_value_position;
};

/*
 *  \class NetAnswerGetCameraParameters
 *  \brief This class is a Get Camera Parameters answer packet class
 */
class NetAnswerGetCameraParameters : public NetAnswerGenericString
{
    friend class CameraControl;    

public:
    // constructor
    NetAnswerGetCameraParameters();

    //-----------------------
    // not recursive methods
    //-----------------------
    // log the class content
    virtual void log() const;

    //-----------------------
    // recursive methods
    //-----------------------
    // totally log the classes content (recursive)
    virtual void totalLog() const;

protected:
    // factory group name for keys to read
    static const std::string g_server_flags_group_factory_name;

    // miscellaneous group name for keys to read
    static const std::string g_server_flags_group_miscellaneous_name;

    // keys names
    static const std::string g_server_flags_instrument_model_name         ;
    static const std::string g_server_flags_instrument_serial_number_name ;
    static const std::string g_server_flags_instrument_serial_size_name   ;
    static const std::string g_server_flags_instrument_parallel_size_name ;
    static const std::string g_server_flags_instrument_bits_per_pixel_name;

    // delimiter used in a key (group,name,value)
    static const std::string g_server_flags_delimiter;

    // position of a value in a key (group,name,value)
    static const std::size_t g_server_flags_value_position;
};

/*
 *  \class NetAnswerGetSettings
 *  \brief This class is a get settings packet class
 */
class NetAnswerGetSettings : public NetGenericAnswer
{
    friend class CameraControl;    

public:
    // hardware acquisition type values
    typedef enum AcquisitionType
    {
        Light     = 0,
        Dark      = 1,
        Triggered = 2,

    } AcquisitionType;

    // hardware acquisition mode values
    typedef enum AcquisitionMode
    {
        SingleImage    = 0,
        Average        = 1,
        MultipleImages = 2,
        MultipleFrames = 3,
        Focus          = 4,

    } AcquisitionMode;

    // constructor
    NetAnswerGetSettings();

    //-----------------------
    // not recursive methods
    //-----------------------
    // get the specific packet size
    virtual std::size_t size() const;

    // read the values stored into a memory block and fill them into the class members
    virtual bool read(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size);

    // write the class members values into a memory block
    virtual bool write(uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) const;

    // log the class content
    virtual void log() const;

    //-----------------------
    // recursive methods
    //-----------------------
    // get the total packet size
    virtual std::size_t totalSize() const;

    // totally read the values stored into a memory block and fill them into the class members
    virtual bool totalRead(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size);

    // totally write the class members values into a memory block
    virtual bool totalWrite(uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) const;

    // totally log the classes content (recursive)
    virtual void totalLog() const;

protected:
    uint32_t m_exposure_time_msec  ; // exposure time in milli-seconds
    uint8_t  m_readout_modes_nb    ; // Number of Readout Modes defined for the camera
    uint8_t  m_readout_mode        ; // Current Readout Mode
    uint32_t m_nb_images_to_average; // Number of Images to Acquire and Average
    uint32_t m_nb_images_to_acquire; // Number of Frames to Acquire
    uint16_t m_acquisition_mode    ; // SI Image SGL II Acquisition Mode
    uint16_t m_acquisition_type    ; // SI Image SGL II Acquisition Type
    int32_t  m_serial_origin       ; // CCD Format Serial Origin
    int32_t  m_serial_length       ; // CCD Format Serial Length
    int32_t  m_serial_binning      ; // CCD Format Serial Binning
    int32_t  m_parallel_origin     ; // CCD Format Parallel Origin
    int32_t  m_parallel_length     ; // CCD Format Parallel Length
    int32_t  m_parallel_binning    ; // CCD Format Parallel Binning
};

/*
 *  \class NetAnswerCommandDone
 *  \brief This class is a Command Done packet class
 */
class NetAnswerCommandDone : public NetGenericAnswer
{
    friend class CameraControl;    

public:
    // constructor
    NetAnswerCommandDone();

    //-----------------------
    // not recursive methods
    //-----------------------
    // get the specific packet size
    virtual std::size_t size() const;

    // read the values stored into a memory block and fill them into the class members
    virtual bool read(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size);

    // write the class members values into a memory block
    virtual bool write(uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) const;

    // log the class content
    virtual void log() const;

    //-----------------------
    // recursive methods
    //-----------------------
    // get the total packet size
    virtual std::size_t totalSize() const;

    // totally read the values stored into a memory block and fill them into the class members
    virtual bool totalRead(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size);

    // totally write the class members values into a memory block
    virtual bool totalWrite(uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) const;

    // totally log the classes content (recursive)
    virtual void totalLog() const;

protected:
    uint16_t m_function_number; // function which was executed (1000 .. 1999)
};

/*
 *  \class NetAnswerSetAcquisitionMode
 *  \brief This class is a Command Done packet class for set acquisition mode answer
 */
class NetAnswerSetAcquisitionMode : public NetAnswerCommandDone
{
    friend class CameraControl;    

public:
    // constructor
    NetAnswerSetAcquisitionMode();

    //-----------------------
    // not recursive methods
    //-----------------------
    // log the class content
    virtual void log() const;

    //-----------------------
    // recursive methods
    //-----------------------
    // totally log the classes content (recursive)
    virtual void totalLog() const;
};

/*
 *  \class NetAnswerSetExposureTime
 *  \brief This class is a Command Done packet class for set exposure time answer
 */
class NetAnswerSetExposureTime : public NetAnswerCommandDone
{
    friend class CameraControl;    

public:
    // constructor
    NetAnswerSetExposureTime();

    //-----------------------
    // not recursive methods
    //-----------------------
    // log the class content
    virtual void log() const;

    //-----------------------
    // recursive methods
    //-----------------------
    // totally log the classes content (recursive)
    virtual void totalLog() const;
};

/*
 *  \class NetAnswerSetFormatParameters
 *  \brief This class is a Command Done packet class for set format parameters answer
 */
class NetAnswerSetFormatParameters : public NetAnswerCommandDone
{
    friend class CameraControl;    

public:
    // constructor
    NetAnswerSetFormatParameters();

    //-----------------------
    // not recursive methods
    //-----------------------
    // log the class content
    virtual void log() const;

    //-----------------------
    // recursive methods
    //-----------------------
    // totally log the classes content (recursive)
    virtual void totalLog() const;
};

/*
 *  \class NetAnswerSetAcquisitionType
 *  \brief This class is a Command Done packet class for set acquisition type answer
 */
class NetAnswerSetAcquisitionType : public NetAnswerCommandDone
{
    friend class CameraControl;    

public:
    // constructor
    NetAnswerSetAcquisitionType();

    //-----------------------
    // not recursive methods
    //-----------------------
    // log the class content
    virtual void log() const;

    //-----------------------
    // recursive methods
    //-----------------------
    // totally log the classes content (recursive)
    virtual void totalLog() const;
};

/*
 *  \class NetAnswerAcquire
 *  \brief This class is a Command Done packet class for acquire answer
 */
class NetAnswerAcquire : public NetAnswerCommandDone
{
    friend class CameraControl;    

public:
    // constructor
    NetAnswerAcquire();

    //-----------------------
    // not recursive methods
    //-----------------------
    // log the class content
    virtual void log() const;

    //-----------------------
    // recursive methods
    //-----------------------
    // totally log the classes content (recursive)
    virtual void totalLog() const;
};

/*
 *  \class NetAnswerTerminateAcquisition
 *  \brief This class is a Command Done packet class for terminate acquisition answer
 */
class NetAnswerTerminateAcquisition : public NetAnswerCommandDone
{
    friend class CameraControl;    

public:
    // constructor
    NetAnswerTerminateAcquisition();

    //-----------------------
    // not recursive methods
    //-----------------------
    // log the class content
    virtual void log() const;

    //-----------------------
    // recursive methods
    //-----------------------
    // totally log the classes content (recursive)
    virtual void totalLog() const;
};

/*
 *  \class NetAnswerTerminateImageRetrieve
 *  \brief This class is a Command Done packet class for terminate image retrieve
 */
class NetAnswerTerminateImageRetrieve : public NetAnswerCommandDone
{
    friend class CameraControl;    

public:
    // constructor
    NetAnswerTerminateImageRetrieve();

    //-----------------------
    // not recursive methods
    //-----------------------
    // log the class content
    virtual void log() const;

    //-----------------------
    // recursive methods
    //-----------------------
    // totally log the classes content (recursive)
    virtual void totalLog() const;
};

/*
 *  \class NetAnswerAcquisitionStatus
 *  \brief This class is an acquisition status packet class (answer for InquireAcquisitionStatus command)
 */
class NetAnswerAcquisitionStatus : public NetGenericAnswer
{
    friend class CameraControl;
    friend class CameraAcqThread;

public:
    // constructor
    NetAnswerAcquisitionStatus();

    //-----------------------
    // not recursive methods
    //-----------------------
    // get the specific packet size
    virtual std::size_t size() const;

    // read the values stored into a memory block and fill them into the class members
    virtual bool read(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size);

    // write the class members values into a memory block
    virtual bool write(uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) const;

    // log the class content
    virtual void log() const;

    //-----------------------
    // recursive methods
    //-----------------------
    // get the total packet size
    virtual std::size_t totalSize() const;

    // totally read the values stored into a memory block and fill them into the class members
    virtual bool totalRead(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size);

    // totally write the class members values into a memory block
    virtual bool totalWrite(uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) const;

    // totally log the classes content (recursive)
    virtual void totalLog() const;

protected:
    uint16_t m_exposure_done   ; // % of the exposure time that has elapsed
    uint16_t m_readout_done    ; // % of the readout that is complete
    uint32_t m_readout_position; // Relative position of readout pointer
    int32_t  m_current_image   ; // Current image being acquired
};

/*
 *  \class NetAnswerConfigurePackets
 *  \brief This class is a Command Done packet class for configure packets answer
 */
class NetAnswerConfigurePackets : public NetAnswerCommandDone
{
    friend class CameraControl;

public:
    // constructor
    NetAnswerConfigurePackets();

    //-----------------------
    // not recursive methods
    //-----------------------
    // log the class content
    virtual void log() const;

    //-----------------------
    // recursive methods
    //-----------------------
    // totally log the classes content (recursive)
    virtual void totalLog() const;
};

/*
 *  \class NetImageHeader
 *  \brief This class is an image header packet class
 */
class NetImageHeader : public NetGenericHeader
{
    friend class CameraControl   ;
    friend class CameraAcqThread ;
    friend class NetPacketsGroups;

public:
    // constructor
    NetImageHeader();

    //-----------------------
    // not recursive methods
    //-----------------------
    // get the specific packet size
    virtual std::size_t size() const;

    // read the values stored into a memory block and fill them into the class members
    virtual bool read(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size);

    // write the class members values into a memory block
    virtual bool write(uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) const;

    // log the class content
    virtual void log() const;

    // check if there is an error
    bool hasError() const;

    //-----------------------
    // recursive methods
    //-----------------------
    // get the total packet size
    virtual std::size_t totalSize() const;

    // totally read the values stored into a memory block and fill them into the class members
    virtual bool totalRead(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size);

    // totally write the class members values into a memory block
    virtual bool totalWrite(uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) const;

    // totally log the classes content (recursive)
    virtual void totalLog() const;

protected:
    int32_t  m_error_code          ; // 0 = no error
    uint16_t m_image_identifier    ; // a number unique to this image
    uint16_t m_image_type          ; // 0=U16, 1=I16, 3=I32, 4=SGL
    uint16_t m_serial_lenght       ; // number of columns in the image
    uint16_t m_parallel_lenght     ; // number of rows in the image
    int32_t  m_total_nb_packets    ; // total number of packets in this image
    int32_t  m_current_packets_nb  ; // number (0..N) of currently transmitted package
    int32_t  m_offset              ; // packet’s offset into the linear image array
    uint32_t m_specific_data_lenght; // length of Image structure that is following in bytes
};

/*
 *  \class NetImage
 *  \brief This class is an image packet class
 */
class NetImage : public NetImageHeader
{
    friend class CameraControl   ;
    friend class NetPacketsGroups;

public:
    // constructor
    NetImage();

    //-----------------------
    // not recursive methods
    //-----------------------
    // get the specific packet size
    virtual std::size_t size() const;

    // read the values stored into a memory block and fill them into the class members
    virtual bool read(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size);

    // write the class members values into a memory block
    virtual bool write(uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) const;

    // log the class content
    virtual void log() const;

    // copy the image part into a destination buffer
    bool copy(void * in_out_buffer, lima::FrameDim & in_buffer_dim) const;

    //-----------------------
    // recursive methods
    //-----------------------
    // get the total packet size
    virtual std::size_t totalSize() const;

    // totally read the values stored into a memory block and fill them into the class members
    virtual bool totalRead(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size);

    // totally write the class members values into a memory block
    virtual bool totalWrite(uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) const;

    // totally log the classes content (recursive)
    virtual void totalLog() const;

protected:
    std::vector<uint16_t> m_image; // 16 bits image part
};

} // namespace Spectral
} // namespace lima

#endif // SPECTRALNETPACKETS_H
