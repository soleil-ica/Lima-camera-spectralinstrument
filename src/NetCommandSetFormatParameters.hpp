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
// Class NetCommandSetFormatParameters
//===================================================================================================
/****************************************************************************************************
 * \fn NetCommandSetFormatParameters()
 * \brief  constructor
 * \param  none
 * \return none
 ****************************************************************************************************/
NetCommandSetFormatParameters::NetCommandSetFormatParameters()
{
    m_function_number   = NetCommandHeader::g_function_number_set_format_parameters; // function to be executed (1000 .. 1999)
    m_packet_name       = "Command SetFormatParameters";
    m_is_server_command = false; // some commands are server related, others to a camera

    m_serial_origin        = 0; // CCD Format Serial Origin
    m_serial_length        = 0; // CCD Format Serial Length
    m_serial_binning       = 0; // CCD Format Serial Binning
    m_parallel_origin      = 0; // CCD Format Parallel Origin
    m_parallel_length      = 0; // CCD Format Parallel Length
    m_parallel_binning     = 0; // CCD Format Parallel Binning
}

/****************************************************************************************************
 * \fn size()
 * \brief  get the specific packet size
 * \param  none
 * \return specific packet size
 ****************************************************************************************************/
std::size_t NetCommandSetFormatParameters::size() const
{
    return sizeof(m_serial_origin       ) + 
           sizeof(m_serial_length       ) +
           sizeof(m_serial_binning      ) + 
           sizeof(m_parallel_origin     ) +
           sizeof(m_parallel_length     ) +
           sizeof(m_parallel_binning    );
}

/****************************************************************************************************
 * \fn totalSize()
 * \brief  get the total packet size
 * \param  none
 * \return total packet size
 ****************************************************************************************************/
std::size_t NetCommandSetFormatParameters::totalSize() const
{
    return NetCommandHeader::totalSize() + NetCommandSetFormatParameters::size();
}

/****************************************************************************************************
 * \fn bool read(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size)
 * \brief  read the values stored into a memory block and fill them into the class members
 * \param  in_out_memory_data start of the memory block to be read (moves to the next data block)
 * \param  in_out_memory_size size of the rest of memory block (the size of the data block will be removed)
 * \return true if success else false in case of error
 ****************************************************************************************************/
bool NetCommandSetFormatParameters::read(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size)
{
    if(in_out_memory_size != NetCommandSetFormatParameters::size())
        return false;

    readData(in_out_memory_data, m_serial_origin       );
    readData(in_out_memory_data, m_serial_length       );
    readData(in_out_memory_data, m_serial_binning      );
    readData(in_out_memory_data, m_parallel_origin     );
    readData(in_out_memory_data, m_parallel_length     );
    readData(in_out_memory_data, m_parallel_binning    );

    in_out_memory_size -= NetCommandSetFormatParameters::size();

    return true;
}

/****************************************************************************************************
 * \fn bool write(uint8_t * out_memory_data, std::size_t in_memory_size)
 * \brief  write the class members values into a memory block
 * \param  in_out_memory_data start of the memory block to be filled (moves to the next data block)
 * \param  in_out_memory_size size of the rest of the memory block (the size of the data block will be removed)
 * \return true if success else false in case of error
 ****************************************************************************************************/
bool NetCommandSetFormatParameters::write(uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) const
{
    if(in_out_memory_size < NetCommandSetFormatParameters::size())
        return false;

    writeData(in_out_memory_data, m_serial_origin       );
    writeData(in_out_memory_data, m_serial_length       );
    writeData(in_out_memory_data, m_serial_binning      );
    writeData(in_out_memory_data, m_parallel_origin     );
    writeData(in_out_memory_data, m_parallel_length     );
    writeData(in_out_memory_data, m_parallel_binning    );

    in_out_memory_size -= NetCommandSetFormatParameters::size();

    return true;
}

/****************************************************************************************************
 * \fn bool read(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size)
 * \brief  totally read the values stored into a memory block and fill them into the class members
 * \param  in_out_memory_data start of the memory block to be read (moves to the next data block)
 * \param  in_out_memory_size size of the rest of memory block (the size of the data block will be removed)
 * \return true if success else false in case of error
 ****************************************************************************************************/
bool NetCommandSetFormatParameters::totalRead(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size)
{
    if(!NetCommandHeader::totalRead(in_out_memory_data, in_out_memory_size))
        return false;

    return NetCommandSetFormatParameters::read(in_out_memory_data, in_out_memory_size);
}

/****************************************************************************************************
 * \fn bool write(uint8_t * out_memory_data, std::size_t in_out_memory_size) const
 * \brief  totally write the class members values into a memory block
 * \param  in_out_memory_data start of the memory block to be filled (moves to the next data block)
 * \param  in_out_memory_size size of the rest of the memory block (the size of the data block will be removed)
 * \return true if success else false in case of error
 ****************************************************************************************************/
bool NetCommandSetFormatParameters::totalWrite(uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) const
{
    if(!NetCommandHeader::totalWrite(in_out_memory_data, in_out_memory_size))
        return false;

    return NetCommandSetFormatParameters::write(in_out_memory_data, in_out_memory_size);
}

/****************************************************************************************************
 * \fn void log() const
 * \brief  log the class content
 * \param  none
 * \return none
 ****************************************************************************************************/
void NetCommandSetFormatParameters::log() const
{
    std::cout << "-- NetCommandSetFormatParameters content --" << std::endl;
    std::cout << "m_serial_origin: " << m_serial_origin << std::endl;
    std::cout << "m_serial_length: " << m_serial_length << std::endl;
    std::cout << "m_serial_binning: " << m_serial_binning << std::endl;
    std::cout << "m_parallel_origin: " << m_parallel_origin << std::endl;
    std::cout << "m_parallel_length: " << m_parallel_length << std::endl;
    std::cout << "m_parallel_binning: " << m_parallel_binning << std::endl;
}

/****************************************************************************************************
 * \fn void totalLog() const
 * \brief  totally log the classes content (recursive)
 * \param  none
 * \return none
 ****************************************************************************************************/
void NetCommandSetFormatParameters::totalLog() const
{
    NetCommandHeader::totalLog();
    NetCommandSetFormatParameters::log();
}

//###########################################################################