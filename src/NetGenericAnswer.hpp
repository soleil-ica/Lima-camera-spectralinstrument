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
// Class NetGenericAnswer
//===================================================================================================
const uint16_t NetGenericAnswer::g_data_type_get_status            = 2012;
const uint16_t NetGenericAnswer::g_data_type_get_camera_parameters = 2010;
const uint16_t NetGenericAnswer::g_data_type_get_settings          = 2008;
const uint16_t NetGenericAnswer::g_data_type_command_done          = 2007;
const uint16_t NetGenericAnswer::g_data_type_acquisition_status    = 2004;

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
 * \fn bool isCommandDonePacket()
 * \brief  check if this is a command done packet
 * \param  none
 * \return true if this is a command done packet
 ****************************************************************************************************/
bool NetGenericAnswer::isCommandDonePacket() const
{
    return (m_data_type == NetGenericAnswer::g_data_type_command_done);
}

/****************************************************************************************************
 * \fn bool isGetStatusPacket()
 * \brief  check if this is a get status packet
 * \param  none
 * \return true if this is a get status packet
 ****************************************************************************************************/
bool NetGenericAnswer::isGetStatusPacket() const
{
    return (m_data_type == NetGenericAnswer::g_data_type_get_status);
}

/****************************************************************************************************
 * \fn bool isGetCameraParameters()
 * \brief  check if this is a get camera parameters
 * \param  none
 * \return true if this is a get camera parameters
 ****************************************************************************************************/
bool NetGenericAnswer::isGetCameraParameters() const
{
    return (m_data_type == NetGenericAnswer::g_data_type_get_camera_parameters);
}

/****************************************************************************************************
 * \fn bool isGetSettings()
 * \brief  check if this is a get settings
 * \param  none
 * \return true if this is a get settings
 ****************************************************************************************************/
bool NetGenericAnswer::isGetSettings() const
{
    return (m_data_type == NetGenericAnswer::g_data_type_get_settings);
}

/****************************************************************************************************
 * \fn bool isAcquisitionStatus()
 * \brief  check if this is an acquisition status
 * \param  none
 * \return true if this is an acquisition status
 ****************************************************************************************************/
bool NetGenericAnswer::isAcquisitionStatus() const
{
    return (m_data_type == NetGenericAnswer::g_data_type_acquisition_status);
}

/****************************************************************************************************
 * \fn bool hasError()
 * \brief  check if there is an error
 * \param  none
 * \return true if there is an error
 ****************************************************************************************************/
bool NetGenericAnswer::hasError() const
{
    return (m_error_code != 0);
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

/****************************************************************************************************
 * \fn void log() const
 * \brief  log the class content
 * \param  none
 * \return none
 ****************************************************************************************************/
void NetGenericAnswer::log() const
{
    std::cout << "-- NetGenericAnswer content --" << std::endl;
    std::cout << "m_error_code: " << (int)m_error_code << std::endl;
    std::cout << "m_data_type: " << (int)m_data_type << std::endl;
    std::cout << "m_specific_data_lenght: " << (int)m_specific_data_lenght << std::endl;
}

/****************************************************************************************************
 * \fn void totalLog() const
 * \brief  totally log the classes content (recursive)
 * \param  none
 * \return none
 ****************************************************************************************************/
void NetGenericAnswer::totalLog() const
{
    NetGenericHeader::totalLog();
    NetGenericAnswer::log();
}

//###########################################################################