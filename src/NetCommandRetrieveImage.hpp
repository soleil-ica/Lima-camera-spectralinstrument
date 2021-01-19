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
// Class NetCommandRetrieveImage
//===================================================================================================
/****************************************************************************************************
 * \fn NetCommandRetrieveImage()
 * \brief  constructor
 * \param  none
 * \return none
 ****************************************************************************************************/
NetCommandRetrieveImage::NetCommandRetrieveImage()
{
    m_function_number   = NetCommandHeader::g_function_number_retrieve_image; // function to be executed (1000 .. 1999)
    m_packet_name       = "Command RetrieveImage";
    m_is_server_command = true; // some commands are server related, others to a camera
    m_transfert_type    = static_cast<uint16_t>(NetCommandRetrieveImage::TransfertType::TransfertU16);
}

/****************************************************************************************************
 * \fn size()
 * \brief  get the specific packet size
 * \param  none
 * \return specific packet size
 ****************************************************************************************************/
std::size_t NetCommandRetrieveImage::size() const
{
    return sizeof(m_transfert_type);
}

/****************************************************************************************************
 * \fn totalSize()
 * \brief  get the total packet size
 * \param  none
 * \return total packet size
 ****************************************************************************************************/
std::size_t NetCommandRetrieveImage::totalSize() const
{
    return NetCommandHeader::totalSize() + NetCommandRetrieveImage::size();
}

/****************************************************************************************************
 * \fn bool read(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size)
 * \brief  read the values stored into a memory block and fill them into the class members
 * \param  in_out_memory_data start of the memory block to be read (moves to the next data block)
 * \param  in_out_memory_size size of the rest of memory block (the size of the data block will be removed)
 * \return true if success else false in case of error
 ****************************************************************************************************/
bool NetCommandRetrieveImage::read(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size)
{
    if(in_out_memory_size != NetCommandRetrieveImage::size())
        return false;

    readData(in_out_memory_data, m_transfert_type);

    in_out_memory_size -= NetCommandRetrieveImage::size();

    return true;
}

/****************************************************************************************************
 * \fn bool write(uint8_t * out_memory_data, std::size_t in_memory_size)
 * \brief  write the class members values into a memory block
 * \param  in_out_memory_data start of the memory block to be filled (moves to the next data block)
 * \param  in_out_memory_size size of the rest of the memory block (the size of the data block will be removed)
 * \return true if success else false in case of error
 ****************************************************************************************************/
bool NetCommandRetrieveImage::write(uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) const
{
    if(in_out_memory_size < NetCommandRetrieveImage::size())
        return false;

    writeData(in_out_memory_data, m_transfert_type);

    in_out_memory_size -= NetCommandRetrieveImage::size();

    return true;
}

/****************************************************************************************************
 * \fn bool read(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size)
 * \brief  totally read the values stored into a memory block and fill them into the class members
 * \param  in_out_memory_data start of the memory block to be read (moves to the next data block)
 * \param  in_out_memory_size size of the rest of memory block (the size of the data block will be removed)
 * \return true if success else false in case of error
 ****************************************************************************************************/
bool NetCommandRetrieveImage::totalRead(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size)
{
    if(!NetCommandHeader::totalRead(in_out_memory_data, in_out_memory_size))
        return false;

    return NetCommandRetrieveImage::read(in_out_memory_data, in_out_memory_size);
}

/****************************************************************************************************
 * \fn bool write(uint8_t * out_memory_data, std::size_t in_out_memory_size) const
 * \brief  totally write the class members values into a memory block
 * \param  in_out_memory_data start of the memory block to be filled (moves to the next data block)
 * \param  in_out_memory_size size of the rest of the memory block (the size of the data block will be removed)
 * \return true if success else false in case of error
 ****************************************************************************************************/
bool NetCommandRetrieveImage::totalWrite(uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) const
{
    if(!NetCommandHeader::totalWrite(in_out_memory_data, in_out_memory_size))
        return false;

    return NetCommandRetrieveImage::write(in_out_memory_data, in_out_memory_size);
}

/****************************************************************************************************
 * \fn void log() const
 * \brief  log the class content
 * \param  none
 * \return none
 ****************************************************************************************************/
void NetCommandRetrieveImage::log() const
{
    std::cout << "-- NetCommandRetrieveImage content --" << std::endl;
    std::cout << "m_transfert_type: " << (int)m_transfert_type << std::endl;
}

/****************************************************************************************************
 * \fn void totalLog() const
 * \brief  totally log the classes content (recursive)
 * \param  none
 * \return none
 ****************************************************************************************************/
void NetCommandRetrieveImage::totalLog() const
{
    NetCommandHeader::totalLog();
    NetCommandRetrieveImage::log();
}

//###########################################################################