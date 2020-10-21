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
 * \file   NetPacket.cpp
 * \brief  implementation file of network packets classes.
 * \author Cédric Castel - SOLEIL (MEDIANE SYSTEME - IT consultant) 
 * \date   Created on October 20, 2020
 ****************************************************************************************************/

// PROJECT
#include "NetPackets.h"

// SYSTEM
#include <netinet/in.h>

// LIMA
#include "lima/Exceptions.h"
#include "lima/Debug.h"

using namespace lima;
using namespace lima::SpectralDetector_ns;

//===================================================================================================
static uint64_t htonll(uint64_t in_value);
static double   htond (double   in_value);
static uint64_t ntohll(uint64_t in_value);
static double   ntohd (double   in_value);

//===================================================================================================
#define INT16_TO_NETWORK(s)  (int16_t)(htons((uint16_t)(s)))
#define INT16_TO_HOST(s)     (int16_t)(ntohs((uint16_t)(s)))
   
#define UINT16_TO_NETWORK(s) htons(s)
#define UINT16_TO_HOST(s)    ntohs(s)

#define INT32_TO_NETWORK(s)  (int32_t)(htonl((uint32_t)(s)))
#define INT32_TO_HOST(s)     (int32_t)(ntohl((uint32_t)(s)))

#define UINT32_TO_NETWORK(s) htonl(s)
#define UINT32_TO_HOST(s)    ntohl(s)

#define UINT64_TO_NETWORK(s) htonll(s)
#define UINT64_TO_HOST(s)    ntohll(s)

#define DOUBLE_TO_NETWORK(s) htond(s)
#define DOUBLE_TO_HOST(s)    ntohd(s)

//===================================================================================================
/****************************************************************************************************
 * \fn uint64_t htonll(uint64_t in_value)
 * \brief  conversion of an unsigned 64 bits data (from host order to network order)
 * \param  unsigned 64 bits data source (in host order)
 * \return unsigned 64 bits data in network order
 ****************************************************************************************************/
uint64_t htonll(uint64_t in_value)
{
    // The answer is 42
    static const int num = 42;

    // Check the endianness
    if (*(reinterpret_cast<const char*>(&num)) == num)
    {
        const uint32_t high_part = htonl(static_cast<uint32_t>(in_value >> 32));
        const uint32_t low_part  = htonl(static_cast<uint32_t>(in_value & 0xFFFFFFFFLL));
        in_value = (static_cast<uint64_t>(low_part) << 32) | static_cast<uint64_t>(high_part);
    }

    return in_value;
}

/****************************************************************************************************
 * \fn uint64_t ntohll(uint64_t in_value)
 * \brief  conversion of an unsigned 64 bits data (from network order to host order)
 * \param  unsigned 64 bits data source (in network order)
 * \return unsigned 64 bits data in host order
 ****************************************************************************************************/
uint64_t ntohll(uint64_t in_value)
{
    // The answer is 42
    static const int num = 42;

    // Check the endianness
    if (*(reinterpret_cast<const char*>(&num)) == num)
    {
        const uint32_t high_part = ntohl(static_cast<uint32_t>(in_value >> 32));
        const uint32_t low_part  = ntohl(static_cast<uint32_t>(in_value & 0xFFFFFFFFLL));
        in_value = (static_cast<uint64_t>(low_part) << 32) | static_cast<uint64_t>(high_part);
    }

    return in_value;
}

/****************************************************************************************************
 * \fn double htond(double in_value)
 * \brief  conversion of a double data (from host order to network order)
 * \param  in_value double data source (in host order)
 * \return double data in network order
 ****************************************************************************************************/
double htond(double in_value)
{
    uint64_t temp = htonll(*(reinterpret_cast<uint64_t *>(&in_value)));
    return *(reinterpret_cast<double *>(&temp));
}

/****************************************************************************************************
 * \fn double ntohd(double in_value)
 * \brief  conversion of a double data (from network order to host order)
 * \param  in_value double data source (in network order)
 * \return double data in host order
 ****************************************************************************************************/
double ntohd(double in_value)
{
    uint64_t temp = ntohll(*(reinterpret_cast<uint64_t *>(&in_value)));
    return *(reinterpret_cast<double *>(&temp));
}

//===================================================================================================
// Class NetGenericHeader
//===================================================================================================
uint8_t  NetGenericHeader::g_server_command_identifier         = 0   ;
uint8_t  NetGenericHeader::g_packet_identifier_for_command     = 128 ;
uint8_t  NetGenericHeader::g_packet_identifier_for_acknowledge = 129 ;
uint8_t  NetGenericHeader::g_packet_identifier_for_data        = 131 ;
uint8_t  NetGenericHeader::g_packet_identifier_for_image       = 132 ;

//------------------------------------------------------------
// specialized template methods for 8 bits unsigned integers
//------------------------------------------------------------
/****************************************************************************************************
 * \fn template <> void readData<uint8_t>(const uint8_t * & in_out_memory_data, uint8_t & out_class_data)
 * \brief  fill the class data with the read value of the memory data. Also, jump to the next value.
 * \param  in_out_memory_data memory data to be read. This pointer will jump to the next value.
 * \param  out_class_data class data to be filled 
 * \return none
 ****************************************************************************************************/
template <> void NetGenericHeader::readData<uint8_t>(const uint8_t * & in_out_memory_data, uint8_t & out_class_data)
{
    out_class_data = *in_out_memory_data;
    in_out_memory_data++;
}

/****************************************************************************************************
 * \fn template <> void writeData<uint8_t>(uint8_t * & in_out_memory_data, uint8_t in_class_data)
 * \brief  fill the memory data with the class data value. Also, jump to the next value.
 * \param  in_out_memory_data memory data to be filled. This pointer will jump to the next value.
 * \param  in_class_data class data to be read
 * \return none
 ****************************************************************************************************/
template <> void NetGenericHeader::writeData<uint8_t>(uint8_t * & in_out_memory_data, uint8_t in_class_data) const
{
    *in_out_memory_data = in_class_data;
    in_out_memory_data++;
}

//------------------------------------------------------------
// specialized template methods for 16 bits signed integers
//------------------------------------------------------------
/****************************************************************************************************
 * \fn template <> void readData<int16_t>(const uint8_t * & in_out_memory_data, int16_t & out_class_data)
 * \brief  fill the class data with the read value of the memory data. Also, jump to the next value.
 * \param  in_out_memory_data memory data to be read. This pointer will jump to the next value.
 * \param  out_class_data class data to be filled 
 * \return none
 ****************************************************************************************************/
template <> void NetGenericHeader::readData<int16_t>(const uint8_t * & in_out_memory_data, int16_t & out_class_data)
{
    out_class_data = INT16_TO_HOST(*(reinterpret_cast<const int16_t * >(in_out_memory_data)));
    in_out_memory_data += sizeof(int16_t);
}

/****************************************************************************************************
 * \fn template <> void writeData<int16_t>(uint8_t * & in_out_memory_data, int16_t in_class_data)
 * \brief  fill the memory data with the class data value. Also, jump to the next value.
 * \param  in_out_memory_data memory data to be filled. This pointer will jump to the next value.
 * \param  in_class_data class data to be read
 * \return none
 ****************************************************************************************************/
template <> void NetGenericHeader::writeData<int16_t>(uint8_t * & in_out_memory_data, int16_t in_class_data) const
{
    *(reinterpret_cast<int16_t * >(in_out_memory_data)) = INT16_TO_NETWORK(in_class_data);
    in_out_memory_data += sizeof(int16_t);
}

//------------------------------------------------------------
// specialized template methods for 16 bits unsigned integers
//------------------------------------------------------------
/****************************************************************************************************
 * \fn template <> void readData<uint16_t>(const uint8_t * & in_out_memory_data, uint16_t & out_class_data)
 * \brief  fill the class data with the read value of the memory data. Also, jump to the next value.
 * \param  in_out_memory_data memory data to be read. This pointer will jump to the next value.
 * \param  out_class_data class data to be filled 
 * \return none
 ****************************************************************************************************/
template <> void NetGenericHeader::readData<uint16_t>(const uint8_t * & in_out_memory_data, uint16_t & out_class_data)
{
    out_class_data = UINT16_TO_HOST(*(reinterpret_cast<const uint16_t * >(in_out_memory_data)));
    in_out_memory_data += sizeof(uint16_t);
}

/****************************************************************************************************
 * \fn template <> void writeData<uint16_t>(uint8_t * & in_out_memory_data, uint16_t in_class_data)
 * \brief  fill the memory data with the class data value. Also, jump to the next value.
 * \param  in_out_memory_data memory data to be filled. This pointer will jump to the next value.
 * \param  in_class_data class data to be read
 * \return none
 ****************************************************************************************************/
template <> void NetGenericHeader::writeData<uint16_t>(uint8_t * & in_out_memory_data, uint16_t in_class_data) const
{
    *(reinterpret_cast<uint16_t * >(in_out_memory_data)) = UINT16_TO_NETWORK(in_class_data);
    in_out_memory_data += sizeof(uint16_t);
}

//------------------------------------------------------------
// specialized template methods for 32 bits signed integers
//------------------------------------------------------------
/****************************************************************************************************
 * \fn template <> void readData<int32_t>(const uint8_t * & in_out_memory_data, int32_t & out_class_data)
 * \brief  fill the class data with the read value of the memory data. Also, jump to the next value.
 * \param  in_out_memory_data memory data to be read. This pointer will jump to the next value.
 * \param  out_class_data class data to be filled 
 * \return none
 ****************************************************************************************************/
template <> void NetGenericHeader::readData<int32_t>(const uint8_t * & in_out_memory_data, int32_t & out_class_data)
{
    out_class_data = INT32_TO_HOST(*(reinterpret_cast<const int32_t * >(in_out_memory_data)));
    in_out_memory_data += sizeof(int32_t);
}

/****************************************************************************************************
 * \fn template <> void writeData<int32_t>(uint8_t * & in_out_memory_data, int32_t in_class_data)
 * \brief  fill the memory data with the class data value. Also, jump to the next value.
 * \param  in_out_memory_data memory data to be filled. This pointer will jump to the next value.
 * \param  in_class_data class data to be read
 * \return none
 ****************************************************************************************************/
template <> void NetGenericHeader::writeData<int32_t>(uint8_t * & in_out_memory_data, int32_t in_class_data) const
{
    *(reinterpret_cast<int32_t * >(in_out_memory_data)) = INT32_TO_NETWORK(in_class_data);
    in_out_memory_data += sizeof(int32_t);
}

//------------------------------------------------------------
// specialized template methods for 32 bits unsigned integers
//------------------------------------------------------------
/****************************************************************************************************
 * \fn template <> void readData<uint32_t>(const uint8_t * & in_out_memory_data, uint32_t & out_class_data)
 * \brief  fill the class data with the read value of the memory data. Also, jump to the next value.
 * \param  in_out_memory_data memory data to be read. This pointer will jump to the next value.
 * \param  out_class_data class data to be filled 
 * \return none
 ****************************************************************************************************/
template <> void NetGenericHeader::readData<uint32_t>(const uint8_t * & in_out_memory_data, uint32_t & out_class_data)
{
    out_class_data = UINT32_TO_HOST(*(reinterpret_cast<const uint32_t * >(in_out_memory_data)));
    in_out_memory_data += sizeof(uint32_t);
}

/****************************************************************************************************
 * \fn template <> void writeData<uint32_t>(uint8_t * & out_memory_data, uint32_t in_class_data)
 * \brief  fill the memory data with the class data value. Also, jump to the next value.
 * \param  in_out_memory_data memory data to be filled. This pointer will jump to the next value.
 * \param  in_class_data class data to be read
 * \return none
 ****************************************************************************************************/
template <> void NetGenericHeader::writeData<uint32_t>(uint8_t * & in_out_memory_data, uint32_t in_class_data) const
{
    *(reinterpret_cast<uint32_t * >(in_out_memory_data)) = UINT32_TO_NETWORK(in_class_data);
    in_out_memory_data += sizeof(uint32_t);
}

//------------------------------------------------------------
// specialized template methods for double
//------------------------------------------------------------
/****************************************************************************************************
 * \fn template <> void readData<double>(const uint8_t * & in_out_memory_data, double & out_class_data)
 * \brief  fill the class data with the read value of the memory data. Also, jump to the next value.
 * \param  in_out_memory_data memory data to be read. This pointer will jump to the next value.
 * \param  out_class_data class data to be filled 
 * \return none
 ****************************************************************************************************/
template <> void NetGenericHeader::readData<double>(const uint8_t * & in_out_memory_data, double & out_class_data)
{
    out_class_data = DOUBLE_TO_HOST(*(reinterpret_cast<const double * >(in_out_memory_data)));
    in_out_memory_data += sizeof(double);
}

/****************************************************************************************************
 * \fn template <> void writeData<double>(uint8_t * & out_memory_data, double in_class_data)
 * \brief  fill the memory data with the class data value. Also, jump to the next value.
 * \param  out_memory_data memory data to be filled. This pointer will jump to the next value.
 * \param  in_class_data class data to be read
 * \return none
 ****************************************************************************************************/
template <> void NetGenericHeader::writeData<double>(uint8_t * & out_memory_data, double in_class_data) const
{
    *(reinterpret_cast<double * >(out_memory_data)) = DOUBLE_TO_NETWORK(in_class_data);
    out_memory_data += sizeof(double);
}

/****************************************************************************************************
 * \fn NetGenericHeader()
 * \brief  constructor
 * \param  none
 * \return none
 ****************************************************************************************************/
NetGenericHeader::NetGenericHeader()
{
    m_packet_lenght     = 0; // total number of bytes in packet
    m_packet_identifier = 0; 
    m_camera_identifier = 0; // 0 for server commands, Camera number (1..max)
    m_packet_name       = "NetGenericHeader";
}

/****************************************************************************************************
 * \fn isCommandPacket()
 * \brief  check if this is a command packet
 * \param  none
 * \return true if this is a command packet
 ****************************************************************************************************/
bool NetGenericHeader::isCommandPacket() const
{
    return (m_packet_identifier == NetGenericHeader::g_packet_identifier_for_command);
}

/****************************************************************************************************
 * \fn isCommandPacket()
 * \brief  check if this is a acknowledge packet
 * \param  none
 * \return true if this is a acknowledge packet
 ****************************************************************************************************/
bool NetGenericHeader::isAcknowledgePacket() const
{
    return (m_packet_identifier == NetGenericHeader::g_packet_identifier_for_acknowledge);
}

/****************************************************************************************************
 * \fn isDataPacket()
 * \brief  check if this is a data packet
 * \param  none
 * \return true if this is a data packet
 ****************************************************************************************************/
bool NetGenericHeader::isDataPacket() const
{
    return (m_packet_identifier == NetGenericHeader::g_packet_identifier_for_data);
}

/****************************************************************************************************
 * \fn isImagePacket()
 * \brief  check if this is an image packet
 * \param  none
 * \return true if this is an image packet
 ****************************************************************************************************/
bool NetGenericHeader::isImagePacket() const
{
    return (m_packet_identifier == NetGenericHeader::g_packet_identifier_for_image);
}

/****************************************************************************************************
 * \fn size()
 * \brief  get the specific packet size
 * \param  none
 * \return specific packet size
 ****************************************************************************************************/
std::size_t NetGenericHeader::size() const
{
    return sizeof(m_packet_lenght    ) + 
           sizeof(m_packet_identifier) + 
           sizeof(m_camera_identifier);
}

/****************************************************************************************************
 * \fn totalSize()
 * \brief  get the total packet size
 * \param  none
 * \return total packet size
 ****************************************************************************************************/
std::size_t NetGenericHeader::totalSize() const
{
    return NetGenericHeader::size();
}

/****************************************************************************************************
 * \fn bool read(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size)
 * \brief  read the values stored into a memory block and fill them into the class members
 * \param  in_out_memory_data start of the memory block to be read (moves to the next data block)
 * \param  in_out_memory_size size of the rest of memory block (the size of the data block will be removed)
 * \return true if success else false in case of error
 ****************************************************************************************************/
bool NetGenericHeader::read(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size)
{
    if(in_out_memory_size < NetGenericHeader::size())
        return false;

    readData(in_out_memory_data, m_packet_lenght    );
    readData(in_out_memory_data, m_packet_identifier);
    readData(in_out_memory_data, m_camera_identifier);

    in_out_memory_size -= NetGenericHeader::size();

    return true;
}

/****************************************************************************************************
 * \fn bool write(uint8_t * out_memory_data, std::size_t & in_out_memory_size)
 * \brief  write the class members values into a memory block
 * \param  in_out_memory_data start of the memory block to be filled (moves to the next data block)
 * \param  in_out_memory_size size of the rest of the memory block (the size of the data block will be removed)
 * \return true if success else false in case of error
 ****************************************************************************************************/
bool NetGenericHeader::write(uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) const
{
    if(in_out_memory_size < NetGenericHeader::size())
        return false;

    writeData(in_out_memory_data, m_packet_lenght    );
    writeData(in_out_memory_data, m_packet_identifier);
    writeData(in_out_memory_data, m_camera_identifier);

    in_out_memory_size -= NetGenericHeader::size();

    return true;
}

/****************************************************************************************************
 * \fn bool read(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size)
 * \brief  totally read the values stored into a memory block and fill them into the class members
 * \param  in_out_memory_data start of the memory block to be read (moves to the next data block)
 * \param  in_out_memory_size size of the rest of memory block (the size of the data block will be removed)
 * \return true if success else false in case of error
 ****************************************************************************************************/
bool NetGenericHeader::totalRead(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size)
{
    return NetGenericHeader::read(in_out_memory_data, in_out_memory_size);
}

/****************************************************************************************************
 * \fn bool write(uint8_t * out_memory_data, std::size_t in_memory_size)
 * \brief  totally write the class members values into a memory block
 * \param  in_out_memory_data start of the memory block to be filled (moves to the next data block)
 * \param  in_out_memory_size size of the rest of the memory block (the size of the data block will be removed)
 * \return true if success else false in case of error
 ****************************************************************************************************/
bool NetGenericHeader::totalWrite(uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) const
{
    return NetGenericHeader::write(in_out_memory_data, in_out_memory_size);
}

//===================================================================================================
// Class NetCommandHeader
//===================================================================================================
uint16_t NetCommandHeader::g_function_number_get_status = 1011;

/****************************************************************************************************
 * \fn NetCommandHeader()
 * \brief  constructor
 * \param  none
 * \return none
 ****************************************************************************************************/
NetCommandHeader::NetCommandHeader()
{
    m_function_number      = 0   ; // function to be executed (1000 .. 1999)
    m_specific_data_lenght = 0   ; // length of parameter block following (0, if none)
    m_is_server_command    = true; // some commands are server related, others to a camera
    m_packet_name          = "NetCommandHeader";
}

/****************************************************************************************************
 * \fn void initPacketLenght()
 * \brief  init the packet lenght member
 * \param  none
 * \return none
 ****************************************************************************************************/
void NetCommandHeader::initPacketLenght()
{
    m_packet_lenght = totalSize(); // total number of bytes in packet
}

/****************************************************************************************************
 * \fn void initCameraIdentifier(uint8_t in_camera_identifier)
 * \brief  init the camera identifier member
 * \param  in_camera_identifier camera identifier, will only be used for a camera command
 * \return none
 ****************************************************************************************************/
void NetCommandHeader::initCameraIdentifier(uint8_t in_camera_identifier)
{
    m_camera_identifier = (m_is_server_command) ? g_server_command_identifier : in_camera_identifier;
}

/****************************************************************************************************
 * \fn void initSpecificDataLenght()
 * \brief  init the specific data lenght
 * \param  none
 * \return none
 ****************************************************************************************************/
void NetCommandHeader::initSpecificDataLenght()
{
    m_specific_data_lenght = totalSize() - NetCommandHeader::totalSize();
}

/****************************************************************************************************
 * \fn size()
 * \brief  get the specific packet size
 * \param  none
 * \return specific packet size
 ****************************************************************************************************/
std::size_t NetCommandHeader::size() const
{
    return sizeof(m_function_number     ) + 
           sizeof(m_specific_data_lenght);
}

/****************************************************************************************************
 * \fn totalSize()
 * \brief  get the total packet size
 * \param  none
 * \return total packet size
 ****************************************************************************************************/
std::size_t NetCommandHeader::totalSize() const
{
    return NetGenericHeader::totalSize() + NetCommandHeader::size();
}

/****************************************************************************************************
 * \fn bool read(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size)
 * \brief  read the values stored into a memory block and fill them into the class members
 * \param  in_out_memory_data start of the memory block to be read (moves to the next data block)
 * \param  in_out_memory_size size of the rest of memory block (the size of the data block will be removed)
 * \return true if success else false in case of error
 ****************************************************************************************************/
bool NetCommandHeader::read(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size)
{
    if(in_out_memory_size < NetCommandHeader::size())
        return false;

    readData(in_out_memory_data, m_function_number     );
    readData(in_out_memory_data, m_specific_data_lenght);

    in_out_memory_size -= NetCommandHeader::size();

    return true;
}

/****************************************************************************************************
 * \fn bool write(uint8_t * out_memory_data, std::size_t in_memory_size)
 * \brief  write the class members values into a memory block
 * \param  in_out_memory_data start of the memory block to be filled (moves to the next data block)
 * \param  in_out_memory_size size of the rest of the memory block (the size of the data block will be removed)
 * \return true if success else false in case of error
 ****************************************************************************************************/
bool NetCommandHeader::write(uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) const
{
    if(in_out_memory_size < NetCommandHeader::size())
        return false;

    writeData(in_out_memory_data, m_function_number     );
    writeData(in_out_memory_data, m_specific_data_lenght);

    in_out_memory_size -= NetCommandHeader::size();

    return true;
}

/****************************************************************************************************
 * \fn bool read(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size)
 * \brief  totally read the values stored into a memory block and fill them into the class members
 * \param  in_out_memory_data start of the memory block to be read (moves to the next data block)
 * \param  in_out_memory_size size of the rest of memory block (the size of the data block will be removed)
 * \return true if success else false in case of error
 ****************************************************************************************************/
bool NetCommandHeader::totalRead(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size)
{
    if(!NetGenericHeader::totalRead(in_out_memory_data, in_out_memory_size))
        return false;

    return NetCommandHeader::read(in_out_memory_data, in_out_memory_size);
}

/****************************************************************************************************
 * \fn bool write(uint8_t * out_memory_data, std::size_t in_out_memory_size) const
 * \brief  totally write the class members values into a memory block
 * \param  in_out_memory_data start of the memory block to be filled (moves to the next data block)
 * \param  in_out_memory_size size of the rest of the memory block (the size of the data block will be removed)
 * \return true if success else false in case of error
 ****************************************************************************************************/
bool NetCommandHeader::totalWrite(uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) const
{
    if(!NetGenericHeader::totalWrite(in_out_memory_data, in_out_memory_size))
        return false;

    return NetCommandHeader::write(in_out_memory_data, in_out_memory_size);
}

//===================================================================================================
// Class NetCommandGetStatus
//===================================================================================================
/****************************************************************************************************
 * \fn NetCommandGetStatus()
 * \brief  constructor
 * \param  none
 * \return none
 ****************************************************************************************************/
NetCommandGetStatus::NetCommandGetStatus()
{
    m_function_number   = NetCommandHeader::g_function_number_get_status; // function to be executed (1000 .. 1999)
    m_packet_name       = "Command GetStatus";
    m_is_server_command = false; // some commands are server related, others to a camera
}

//===================================================================================================
// Class NetAcknowledge
//===================================================================================================
/****************************************************************************************************
 * \fn NetAcknowledge()
 * \brief  constructor
 * \param  none
 * \return none
 ****************************************************************************************************/
NetAcknowledge::NetAcknowledge()
{
    m_accepted_flag = 0; // true (!0) or false (0)
    m_packet_name   = "Answer Acknowledge";
}

/****************************************************************************************************
 * \fn size()
 * \brief  get the specific packet size
 * \param  none
 * \return specific packet size
 ****************************************************************************************************/
std::size_t NetAcknowledge::size() const
{
    return sizeof(m_accepted_flag);
}

/****************************************************************************************************
 * \fn totalSize()
 * \brief  get the total packet size
 * \param  none
 * \return total packet size
 ****************************************************************************************************/
std::size_t NetAcknowledge::totalSize() const
{
    return NetGenericHeader::totalSize() + NetAcknowledge::size();
}

/****************************************************************************************************
 * \fn bool read(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size)
 * \brief  read the values stored into a memory block and fill them into the class members
 * \param  in_out_memory_data start of the memory block to be read (moves to the next data block)
 * \param  in_out_memory_size size of the rest of memory block (the size of the data block will be removed)
 * \return true if success else false in case of error
 ****************************************************************************************************/
bool NetAcknowledge::read(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size)
{
    if(in_out_memory_size < NetAcknowledge::size())
        return false;

    readData(in_out_memory_data, m_accepted_flag);

    in_out_memory_size -= NetAcknowledge::size();

    return true;
}

/****************************************************************************************************
 * \fn bool write(uint8_t * out_memory_data, std::size_t & in_out_memory_size)
 * \brief  write the class members values into a memory block
 * \param  in_out_memory_data start of the memory block to be filled (moves to the next data block)
 * \param  in_out_memory_size size of the rest of the memory block (the size of the data block will be removed)
 * \return true if success else false in case of error
 ****************************************************************************************************/
bool NetAcknowledge::write(uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) const
{
    if(in_out_memory_size < NetAcknowledge::size())
        return false;

    writeData(in_out_memory_data, m_accepted_flag);

    in_out_memory_size -= NetAcknowledge::size();

    return true;
}

/****************************************************************************************************
 * \fn bool read(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size)
 * \brief  totally read the values stored into a memory block and fill them into the class members
 * \param  in_out_memory_data start of the memory block to be read (moves to the next data block)
 * \param  in_out_memory_size size of the rest of memory block (the size of the data block will be removed)
 * \return true if success else false in case of error
 ****************************************************************************************************/
bool NetAcknowledge::totalRead(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size)
{
    if(!NetGenericHeader::totalRead(in_out_memory_data, in_out_memory_size))
        return false;

    return NetAcknowledge::read(in_out_memory_data, in_out_memory_size);
}

/****************************************************************************************************
 * \fn bool write(uint8_t * out_memory_data, std::size_t & in_out_memory_size)
 * \brief  totally write the class members values into a memory block
 * \param  in_out_memory_data start of the memory block to be filled (moves to the next data block)
 * \param  in_out_memory_size size of the rest of the memory block (the size of the data block will be removed)
 * \return true if success else false in case of error
 ****************************************************************************************************/
bool NetAcknowledge::totalWrite(uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) const
{
    if(!NetGenericHeader::totalWrite(in_out_memory_data, in_out_memory_size))
        return false;

    return NetAcknowledge::write(in_out_memory_data, in_out_memory_size);
}

//===================================================================================================
// Class NetGenericAnswer
//===================================================================================================
uint16_t NetGenericAnswer::g_data_type_get_status = 2012;

/****************************************************************************************************
 * \fn NetGenericAnswer()
 * \brief  constructor
 * \param  none
 * \return none
 ****************************************************************************************************/
NetGenericAnswer::NetGenericAnswer()
{
    m_error_code           = 0; // 0 = no error
    m_data_type            = 0; // 2000 .. 2999
    m_specific_data_lenght = 0; // 0 = no data
    m_packet_name          = "NetGenericAnswer";
}

/****************************************************************************************************
 * \fn size()
 * \brief  get the specific packet size
 * \param  none
 * \return specific packet size
 ****************************************************************************************************/
std::size_t NetGenericAnswer::size() const
{
    return sizeof(m_error_code          ) + 
           sizeof(m_data_type           ) + 
           sizeof(m_specific_data_lenght);
}

/****************************************************************************************************
 * \fn totalSize()
 * \brief  get the total packet size
 * \param  none
 * \return total packet size
 ****************************************************************************************************/
std::size_t NetGenericAnswer::totalSize() const
{
    return NetGenericHeader::totalSize() + NetGenericAnswer::size();
}

/****************************************************************************************************
 * \fn bool read(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size)
 * \brief  read the values stored into a memory block and fill them into the class members
 * \param  in_out_memory_data start of the memory block to be read (moves to the next data block)
 * \param  in_out_memory_size size of the rest of memory block (the size of the data block will be removed)
 * \return true if success else false in case of error
 ****************************************************************************************************/
bool NetGenericAnswer::read(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size)
{
    if(in_out_memory_size < NetGenericAnswer::size())
        return false;

    readData(in_out_memory_data, m_error_code          );
    readData(in_out_memory_data, m_data_type           );
    readData(in_out_memory_data, m_specific_data_lenght);

    in_out_memory_size -= NetGenericAnswer::size();

    return true;
}

/****************************************************************************************************
 * \fn bool write(uint8_t * out_memory_data, std::size_t & in_out_memory_size)
 * \brief  write the class members values into a memory block
 * \param  in_out_memory_data start of the memory block to be filled (moves to the next data block)
 * \param  in_out_memory_size size of the rest of the memory block (the size of the data block will be removed)
 * \return true if success else false in case of error
 ****************************************************************************************************/
bool NetGenericAnswer::write(uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) const
{
    if(in_out_memory_size < NetGenericAnswer::size())
        return false;

    writeData(in_out_memory_data, m_error_code          );
    writeData(in_out_memory_data, m_data_type           );
    writeData(in_out_memory_data, m_specific_data_lenght);

    in_out_memory_size -= NetGenericAnswer::size();

    return true;
}

/****************************************************************************************************
 * \fn bool read(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size)
 * \brief  totally read the values stored into a memory block and fill them into the class members
 * \param  in_out_memory_data start of the memory block to be read (moves to the next data block)
 * \param  in_out_memory_size size of the rest of memory block (the size of the data block will be removed)
 * \return true if success else false in case of error
 ****************************************************************************************************/
bool NetGenericAnswer::totalRead(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size)
{
    if(!NetGenericHeader::totalRead(in_out_memory_data, in_out_memory_size))
        return false;

    return NetGenericAnswer::read(in_out_memory_data, in_out_memory_size);
}

/****************************************************************************************************
 * \fn bool write(uint8_t * out_memory_data, std::size_t & in_out_memory_size)
 * \brief  totally write the class members values into a memory block
 * \param  in_out_memory_data start of the memory block to be filled (moves to the next data block)
 * \param  in_out_memory_size size of the rest of the memory block (the size of the data block will be removed)
 * \return true if success else false in case of error
 ****************************************************************************************************/
bool NetGenericAnswer::totalWrite(uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) const
{
    if(!NetGenericHeader::totalWrite(in_out_memory_data, in_out_memory_size))
        return false;

    return NetGenericAnswer::write(in_out_memory_data, in_out_memory_size);
}

//===================================================================================================
// Class NetAnswerGetStatus
//===================================================================================================
/****************************************************************************************************
 * \fn NetAnswerGetStatus()
 * \brief  constructor
 * \param  none
 * \return none
 ****************************************************************************************************/
NetAnswerGetStatus::NetAnswerGetStatus()
{
    m_packet_name = "Answer GetStatus";
}

/****************************************************************************************************
 * \fn size()
 * \brief  get the specific packet size
 * \param  none
 * \return specific packet size
 ****************************************************************************************************/
std::size_t NetAnswerGetStatus::size() const
{
    return (m_status.size() + 1); // do not forget the 0 terminal character;
}

/****************************************************************************************************
 * \fn totalSize()
 * \brief  get the total packet size
 * \param  none
 * \return total packet size
 ****************************************************************************************************/
std::size_t NetAnswerGetStatus::totalSize() const
{
    return NetGenericAnswer::totalSize() + NetAnswerGetStatus::size();
}

/****************************************************************************************************
 * \fn bool read(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size)
 * \brief  read the values stored into a memory block and fill them into the class members
 * \param  in_out_memory_data start of the memory block to be read (moves to the next data block)
 * \param  in_out_memory_size size of the rest of memory block (the size of the data block will be removed)
 * \return true if success else false in case of error
 ****************************************************************************************************/
bool NetAnswerGetStatus::read(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size)
{
    m_status = reinterpret_cast<const char *>(in_out_memory_data);
    in_out_memory_data += NetAnswerGetStatus::size();

    if(in_out_memory_size == NetAnswerGetStatus::size())
    {
        in_out_memory_size = 0;
    }
    else
    {
        in_out_memory_size -= NetAnswerGetStatus::size();
    }

    return (!in_out_memory_size); // should have a zero value (no more data to be read)
}

/****************************************************************************************************
 * \fn bool write(uint8_t * out_memory_data, std::size_t & in_out_memory_size)
 * \brief  write the class members values into a memory block
 * \param  in_out_memory_data start of the memory block to be filled (moves to the next data block)
 * \param  in_out_memory_size size of the rest of the memory block (the size of the data block will be removed)
 * \return true if success else false in case of error
 ****************************************************************************************************/
bool NetAnswerGetStatus::write(uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) const
{
    if(in_out_memory_size != NetAnswerGetStatus::size())
        return false;

    memcpy(reinterpret_cast<char *>(in_out_memory_data), m_status.c_str(), in_out_memory_size);  

    in_out_memory_size -= NetAnswerGetStatus::size();

    return true;
}

/****************************************************************************************************
 * \fn bool read(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size)
 * \brief  totally read the values stored into a memory block and fill them into the class members
 * \param  in_out_memory_data start of the memory block to be read (moves to the next data block)
 * \param  in_out_memory_size size of the rest of memory block (the size of the data block will be removed)
 * \return true if success else false in case of error
 ****************************************************************************************************/
bool NetAnswerGetStatus::totalRead(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size)
{
    if(!NetGenericAnswer::totalRead(in_out_memory_data, in_out_memory_size))
        return false;

    return NetAnswerGetStatus::read(in_out_memory_data, in_out_memory_size);
}

/****************************************************************************************************
 * \fn bool write(uint8_t * out_memory_data, std::size_t & in_out_memory_size)
 * \brief  totally write the class members values into a memory block
 * \param  in_out_memory_data start of the memory block to be filled (moves to the next data block)
 * \param  in_out_memory_size size of the rest of the memory block (the size of the data block will be removed)
 * \return true if success else false in case of error
 ****************************************************************************************************/
bool NetAnswerGetStatus::totalWrite(uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) const
{
    if(!NetGenericAnswer::totalWrite(in_out_memory_data, in_out_memory_size))
        return false;

    return NetAnswerGetStatus::write(in_out_memory_data, in_out_memory_size);
}

//###########################################################################