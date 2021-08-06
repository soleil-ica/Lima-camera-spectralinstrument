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
// Class NetCommandHeader
//===================================================================================================
const uint16_t NetCommandHeader::g_function_number_get_status            = 1011;
const uint16_t NetCommandHeader::g_function_number_get_camera_parameters = 1048;
const uint16_t NetCommandHeader::g_function_number_get_settings          = 1041;

const uint16_t NetCommandHeader::g_function_number_set_acquisition_mode  = 1034;
const uint16_t NetCommandHeader::g_function_number_set_exposure_time     = 1035;
const uint16_t NetCommandHeader::g_function_number_set_format_parameters = 1043;
const uint16_t NetCommandHeader::g_function_number_set_acquisition_type  = 1036;

const uint16_t NetCommandHeader::g_function_number_acquire                    = 1037;
const uint16_t NetCommandHeader::g_function_number_terminate_acquisition      = 1018;
const uint16_t NetCommandHeader::g_function_number_retrieve_image             = 1019;
const uint16_t NetCommandHeader::g_function_number_terminate_image_retrieve   = 1020;
const uint16_t NetCommandHeader::g_function_number_inquire_acquisition_status = 1017;
const uint16_t NetCommandHeader::g_function_number_configure_packets          = 1022;
const uint16_t NetCommandHeader::g_function_number_set_cooling_value          = 1046;
const uint16_t NetCommandHeader::g_function_number_set_single_parameter       = 1044;

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
    m_packet_identifier    = NetGenericHeader::g_packet_identifier_for_command; 
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

/****************************************************************************************************
 * \fn void log() const
 * \brief  log the class content
 * \param  none
 * \return none
 ****************************************************************************************************/
void NetCommandHeader::log() const
{
    std::cout << "-- NetCommandHeader content --" << std::endl;
    std::cout << "m_function_number: " << (int)m_function_number << std::endl;
    std::cout << "m_specific_data_lenght: " << (int)m_specific_data_lenght << std::endl;
    std::cout << "m_camera_identifier: " << (int)m_camera_identifier << std::endl;
}

/****************************************************************************************************
 * \fn void totalLog() const
 * \brief  totally log the classes content (recursive)
 * \param  none
 * \return none
 ****************************************************************************************************/
void NetCommandHeader::totalLog() const
{
    NetGenericHeader::totalLog();
    NetCommandHeader::log();
}

//###########################################################################