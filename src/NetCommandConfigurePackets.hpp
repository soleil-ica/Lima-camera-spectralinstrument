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
// Class NetCommandConfigurePackets
//===================================================================================================
/****************************************************************************************************
 * \fn NetCommandConfigurePackets()
 * \brief  constructor
 * \param  none
 * \return none
 ****************************************************************************************************/
NetCommandConfigurePackets::NetCommandConfigurePackets()
{
    m_function_number   = NetCommandHeader::g_function_number_configure_packets; // function to be executed (1000 .. 1999)
    m_packet_name       = "Command ConfigurePackets";
    m_is_server_command = false; // some commands are server related, others to a camera
    m_pixels_per_packet = 512  ; // pixels per packet
    m_packet_delay_usec = 300  ; // packet sending loop delay in microseconds
}

/****************************************************************************************************
 * \fn size()
 * \brief  get the specific packet size
 * \param  none
 * \return specific packet size
 ****************************************************************************************************/
std::size_t NetCommandConfigurePackets::size() const
{
    return sizeof(m_pixels_per_packet) + 
           sizeof(m_packet_delay_usec);
}

/****************************************************************************************************
 * \fn totalSize()
 * \brief  get the total packet size
 * \param  none
 * \return total packet size
 ****************************************************************************************************/
std::size_t NetCommandConfigurePackets::totalSize() const
{
    return NetCommandHeader::totalSize() + NetCommandConfigurePackets::size();
}

/****************************************************************************************************
 * \fn bool read(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size)
 * \brief  read the values stored into a memory block and fill them into the class members
 * \param  in_out_memory_data start of the memory block to be read (moves to the next data block)
 * \param  in_out_memory_size size of the rest of memory block (the size of the data block will be removed)
 * \return true if success else false in case of error
 ****************************************************************************************************/
bool NetCommandConfigurePackets::read(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size)
{
    if(in_out_memory_size != NetCommandConfigurePackets::size())
        return false;

    readData(in_out_memory_data, m_pixels_per_packet);
    readData(in_out_memory_data, m_packet_delay_usec);

    in_out_memory_size -= NetCommandConfigurePackets::size();

    return true;
}

/****************************************************************************************************
 * \fn bool write(uint8_t * out_memory_data, std::size_t in_memory_size)
 * \brief  write the class members values into a memory block
 * \param  in_out_memory_data start of the memory block to be filled (moves to the next data block)
 * \param  in_out_memory_size size of the rest of the memory block (the size of the data block will be removed)
 * \return true if success else false in case of error
 ****************************************************************************************************/
bool NetCommandConfigurePackets::write(uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) const
{
    if(in_out_memory_size < NetCommandConfigurePackets::size())
        return false;

    writeData(in_out_memory_data, m_pixels_per_packet);
    writeData(in_out_memory_data, m_packet_delay_usec);

    in_out_memory_size -= NetCommandConfigurePackets::size();

    return true;
}

/****************************************************************************************************
 * \fn bool read(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size)
 * \brief  totally read the values stored into a memory block and fill them into the class members
 * \param  in_out_memory_data start of the memory block to be read (moves to the next data block)
 * \param  in_out_memory_size size of the rest of memory block (the size of the data block will be removed)
 * \return true if success else false in case of error
 ****************************************************************************************************/
bool NetCommandConfigurePackets::totalRead(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size)
{
    if(!NetCommandHeader::totalRead(in_out_memory_data, in_out_memory_size))
        return false;

    return NetCommandConfigurePackets::read(in_out_memory_data, in_out_memory_size);
}

/****************************************************************************************************
 * \fn bool write(uint8_t * out_memory_data, std::size_t in_out_memory_size) const
 * \brief  totally write the class members values into a memory block
 * \param  in_out_memory_data start of the memory block to be filled (moves to the next data block)
 * \param  in_out_memory_size size of the rest of the memory block (the size of the data block will be removed)
 * \return true if success else false in case of error
 ****************************************************************************************************/
bool NetCommandConfigurePackets::totalWrite(uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) const
{
    if(!NetCommandHeader::totalWrite(in_out_memory_data, in_out_memory_size))
        return false;

    return NetCommandConfigurePackets::write(in_out_memory_data, in_out_memory_size);
}

/****************************************************************************************************
 * \fn void log() const
 * \brief  log the class content
 * \param  none
 * \return none
 ****************************************************************************************************/
void NetCommandConfigurePackets::log() const
{
    std::cout << "-- NetCommandConfigurePackets content --" << std::endl;
    std::cout << "m_pixels_per_packet: " << (int)m_pixels_per_packet << std::endl;
    std::cout << "m_packet_delay_usec: " << (int)m_packet_delay_usec << std::endl;
}

/****************************************************************************************************
 * \fn void totalLog() const
 * \brief  totally log the classes content (recursive)
 * \param  none
 * \return none
 ****************************************************************************************************/
void NetCommandConfigurePackets::totalLog() const
{
    NetCommandHeader::totalLog();
    NetCommandConfigurePackets::log();
}

//###########################################################################