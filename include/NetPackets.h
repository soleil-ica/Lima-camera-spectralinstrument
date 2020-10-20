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

#ifndef SPECTRALNETPACKET_H
#define SPECTRALNETPACKET_H

// SYSTEM
#include <cstdlib>
#include <cstring>
#include <string>
#include <stdint.h>

// PROJECT
#include "SpectralCompatibility.h"

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
 *  \class NetPacket
 *  \brief This class is a network packet abstract class used to communicate with the detector software
 */
class NetPacket
{
public:
    //-----------------------
    // not recursive methods
    //-----------------------
    // get the specific packet size
    virtual std::size_t size() const = 0;

    // read the values stored into a memory block and fill them into the class members
    virtual bool read(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) = 0;

    // write the class members values into a memory block
    virtual bool write(uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) const = 0;

    //-----------------------
    // recursive methods
    //-----------------------
    // get the total packet size
    virtual std::size_t totalSize() const = 0;

    // totally read the values stored into a memory block and fill them into the class members
    virtual bool totalRead(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) = 0;

    // totally write the class members values into a memory block
    virtual bool totalWrite(uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) const = 0;

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
    static uint16_t g_command_get_status_fct_nb;
    static uint16_t g_answer_get_status_data_nb;
};

//------------------------------------------------------------
// specialized template methods for 8 bits unsigned integers
//------------------------------------------------------------
// fill the class data with the read value of the memory data. Also, jump to the next value.
template <> void NetPacket::readData<uint8_t>(const uint8_t * & in_out_memory_data, uint8_t & out_class_data);

// fill the memory data with the class data value. Also, jump to the next value.
template <> void NetPacket::writeData<uint8_t>(uint8_t * & in_out_memory_data, uint8_t in_class_data) const;

//------------------------------------------------------------
// specialized template methods for 16 bits signed integers
//------------------------------------------------------------
// fill the class data with the read value of the memory data. Also, jump to the next value.
template <> void NetPacket::readData<int16_t>(const uint8_t * & in_out_memory_data, int16_t & out_class_data);

// fill the memory data with the class data value. Also, jump to the next value.
template <> void NetPacket::writeData<int16_t>(uint8_t * & in_out_memory_data, int16_t in_class_data) const;

//------------------------------------------------------------
// specialized template methods for 16 bits unsigned integers
//------------------------------------------------------------
// fill the class data with the read value of the memory data. Also, jump to the next value.
template <> void NetPacket::readData<uint16_t>(const uint8_t * & in_out_memory_data, uint16_t & out_class_data);

// fill the memory data with the class data value. Also, jump to the next value.
template <> void NetPacket::writeData<uint16_t>(uint8_t * & in_out_memory_data, uint16_t in_class_data) const;

//------------------------------------------------------------
// specialized template methods for 32 bits signed integers
//------------------------------------------------------------
// fill the class data with the read value of the memory data. Also, jump to the next value.
template <> void NetPacket::readData<int32_t>(const uint8_t * & in_out_memory_data, int32_t & out_class_data);

// fill the memory data with the class data value. Also, jump to the next value.
template <> void NetPacket::writeData<int32_t>(uint8_t * & in_out_memory_data, int32_t in_class_data) const;

//------------------------------------------------------------
// specialized template methods for 32 bits unsigned integers
//------------------------------------------------------------
// fill the class data with the read value of the memory data. Also, jump to the next value.
template <> void NetPacket::readData<uint32_t>(const uint8_t * & in_out_memory_data, uint32_t & out_class_data);

// fill the memory data with the class data value. Also, jump to the next value.
template <> void NetPacket::writeData<uint32_t>(uint8_t * & in_out_memory_data, uint32_t in_class_data) const;

//------------------------------------------------------------
// specialized template methods for double
//------------------------------------------------------------
// fill the class data with the read value of the memory data. Also, jump to the next value.
template <> void NetPacket::readData<double>(const uint8_t * & in_out_memory_data, double & out_class_data);

// fill the memory data with the class data value. Also, jump to the next value.
template <> void NetPacket::writeData<double>(uint8_t * & in_out_memory_data, double in_class_data) const;

/*
 *  \class NetGenericHeader
 *  \brief This class is a generic header packet class
 */
class NetGenericHeader : public NetPacket
{
    friend class CameraControl;    

public:
    // constructor
    NetGenericHeader();

    //-----------------------
    // not recursive methods
    //-----------------------
    // get the specific packet size
    virtual std::size_t size() const;

    // read the values stored into a memory block and fill them into the class members
    virtual bool read(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size);

    // write the class members values into a memory block
    virtual bool write(uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) const;

    //-----------------------
    // recursive methods
    //-----------------------
    // get the total packet size
    virtual std::size_t totalSize() const;

    // totally read the values stored into a memory block and fill them into the class members
    virtual bool totalRead(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size);

    // totally write the class members values into a memory block
    virtual bool totalWrite(uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) const;

protected:
    uint32_t m_packet_lenght    ; // total number of bytes in packet
    uint8_t  m_packet_identifier; 
    uint8_t  m_camera_identifier; // 0 for server commands, Camera number (1..max)
};

/*
 *  \class NetCommandHeader
 *  \brief This class is a header command packet class
 */
class NetCommandHeader : public NetGenericHeader
{
    friend class CameraControl;    

public:
    // constructor
    NetCommandHeader();

    //-----------------------
    // not recursive methods
    //-----------------------
    // get the specific packet size
    virtual std::size_t size() const;

    // read the values stored into a memory block and fill them into the class members
    virtual bool read(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size);

    // write the class members values into a memory block
    virtual bool write(uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) const;

    //-----------------------
    // recursive methods
    //-----------------------
    // get the total packet size
    virtual std::size_t totalSize() const;

    // totally read the values stored into a memory block and fill them into the class members
    virtual bool totalRead(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size);

    // totally write the class members values into a memory block
    virtual bool totalWrite(uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) const;

protected:
    uint16_t m_function_number     ; // function to be executed (1000 .. 1999)
    uint16_t m_specific_data_lenght; // length of parameter block following (0, if none)
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

protected:
};

/*
 *  \class NetAcknowledge
 *  \brief This class is an acknowledge packet class
 */
class NetAcknowledge : public NetGenericHeader
{
    friend class CameraControl;    

public:
    // constructor
    NetAcknowledge();

    //-----------------------
    // not recursive methods
    //-----------------------
    // get the specific packet size
    virtual std::size_t size() const;

    // read the values stored into a memory block and fill them into the class members
    virtual bool read(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size);

    // write the class members values into a memory block
    virtual bool write(uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) const;

    //-----------------------
    // recursive methods
    //-----------------------
    // get the total packet size
    virtual std::size_t totalSize() const;

    // totally read the values stored into a memory block and fill them into the class members
    virtual bool totalRead(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size);

    // totally write the class members values into a memory block
    virtual bool totalWrite(uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) const;

protected:
    uint16_t m_accepted_flag; // true (!0) or false (0)
};

/*
 *  \class NetGenericAnswer
 *  \brief This class is an generic answer packet class
 */
class NetGenericAnswer : public NetGenericHeader
{
    friend class CameraControl;    

public:
    // constructor
    NetGenericAnswer();

    //-----------------------
    // not recursive methods
    //-----------------------
    // get the specific packet size
    virtual std::size_t size() const;

    // read the values stored into a memory block and fill them into the class members
    virtual bool read(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size);

    // write the class members values into a memory block
    virtual bool write(uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) const;

    //-----------------------
    // recursive methods
    //-----------------------
    // get the total packet size
    virtual std::size_t totalSize() const;

    // totally read the values stored into a memory block and fill them into the class members
    virtual bool totalRead(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size);

    // totally write the class members values into a memory block
    virtual bool totalWrite(uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) const;

protected:
    int32_t  m_error_code          ; // 0 = no error
    uint16_t m_data_type           ; // 2000 .. 2999
    int32_t  m_specific_data_lenght; // 0 = no data
};

/*
 *  \class NetAnswerGetStatus
 *  \brief This class is a Get Status answer packet class
 */
class NetAnswerGetStatus : public NetGenericAnswer
{
    friend class CameraControl;    

    // constructor
    NetAnswerGetStatus();

    //-----------------------
    // not recursive methods
    //-----------------------
    // get the specific packet size
    virtual std::size_t size() const;

    // read the values stored into a memory block and fill them into the class members
    virtual bool read(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size);

    // write the class members values into a memory block
    virtual bool write(uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) const;

    //-----------------------
    // recursive methods
    //-----------------------
    // get the total packet size
    virtual std::size_t totalSize() const;

    // totally read the values stored into a memory block and fill them into the class members
    virtual bool totalRead(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size);

    // totally write the class members values into a memory block
    virtual bool totalWrite(uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) const;

protected:
    std::string m_status;
};

} // namespace SpectralDetector_ns
} // namespace lima

#endif // SPECTRALNETPACKET_H
