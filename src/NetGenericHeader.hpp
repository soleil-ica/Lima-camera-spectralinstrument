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

//===================================================================================================
// Class NetGenericHeader
//===================================================================================================
const uint8_t  NetGenericHeader::g_server_command_identifier         = 0   ;
const uint8_t  NetGenericHeader::g_packet_identifier_for_command     = 128 ;
const uint8_t  NetGenericHeader::g_packet_identifier_for_acknowledge = 129 ;
const uint8_t  NetGenericHeader::g_packet_identifier_for_data        = 131 ;
const uint8_t  NetGenericHeader::g_packet_identifier_for_image       = 132 ;

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
 * \fn bool isCommandPacket()
 * \brief  check if this is a command packet
 * \param  none
 * \return true if this is a command packet
 ****************************************************************************************************/
bool NetGenericHeader::isCommandPacket() const
{
    return (m_packet_identifier == NetGenericHeader::g_packet_identifier_for_command);
}

/****************************************************************************************************
 * \fn bool isAcknowledgePacket()
 * \brief  check if this is a acknowledge packet
 * \param  none
 * \return true if this is a acknowledge packet
 ****************************************************************************************************/
bool NetGenericHeader::isAcknowledgePacket() const
{
    return (m_packet_identifier == NetGenericHeader::g_packet_identifier_for_acknowledge);
}

/****************************************************************************************************
 * \fn bool isDataPacket()
 * \brief  check if this is a data packet
 * \param  none
 * \return true if this is a data packet
 ****************************************************************************************************/
bool NetGenericHeader::isDataPacket() const
{
    return (m_packet_identifier == NetGenericHeader::g_packet_identifier_for_data);
}

/****************************************************************************************************
 * \fn bool isImagePacket()
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

/****************************************************************************************************
 * \fn void log() const
 * \brief  log the class content
 * \param  none
 * \return none
 ****************************************************************************************************/
void NetGenericHeader::log() const
{
    std::cout << "-- NetGenericHeader content --" << std::endl;
    std::cout << "m_packet_name: " << m_packet_name << std::endl;
    std::cout << "m_packet_lenght: " << (int)m_packet_lenght << std::endl;
    std::cout << "m_packet_identifier: " << (int)m_packet_identifier << std::endl;
    std::cout << "m_camera_identifier: " << (int)m_camera_identifier << std::endl;
}

/****************************************************************************************************
 * \fn void totalLog() const
 * \brief  totally log the classes content (recursive)
 * \param  none
 * \return none
 ****************************************************************************************************/
void NetGenericHeader::totalLog() const
{
    NetGenericHeader::log();
}

//###########################################################################