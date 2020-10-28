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
// Class NetImage
//===================================================================================================
/****************************************************************************************************
 * \fn NetImage()
 * \brief  constructor
 * \param  none
 * \return none
 ****************************************************************************************************/
NetImage::NetImage()
{
    m_packet_name          = "Image";
    m_error_code           = 0; // 0 = no error
    m_image_identifier     = 0; // a number unique to this image
    m_image_type           = 0; // 0=U16, 1=I16, 3=I32, 4=SGL
    m_serial_lenght        = 0; // number of columns in the image
    m_parallel_lenght      = 0; // number of rows in the image
    m_total_nb_packets     = 0; // total number of packets in this image
    m_current_packets_nb   = 0; // number (0..N) of currently transmitted package
    m_offset               = 0; // packet’s offset into the linear image array
    m_specific_data_lenght = 0; // length of Image structure that is following in bytes
}

/****************************************************************************************************
 * \fn size()
 * \brief  get the specific packet size
 * \param  none
 * \return specific packet size
 ****************************************************************************************************/
std::size_t NetImage::size() const
{
    return sizeof(m_error_code          ) + 
           sizeof(m_image_identifier    ) + 
           sizeof(m_image_type          ) +
           sizeof(m_serial_lenght       ) +
           sizeof(m_parallel_lenght     ) +
           sizeof(m_total_nb_packets    ) +
           sizeof(m_current_packets_nb  ) +
           sizeof(m_offset              ) +
           sizeof(m_specific_data_lenght);
}

/****************************************************************************************************
 * \fn totalSize()
 * \brief  get the total packet size
 * \param  none
 * \return total packet size
 ****************************************************************************************************/
std::size_t NetImage::totalSize() const
{
    return NetGenericHeader::totalSize() + NetImage::size();
}

/****************************************************************************************************
 * \fn bool read(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size)
 * \brief  read the values stored into a memory block and fill them into the class members
 * \param  in_out_memory_data start of the memory block to be read (moves to the next data block)
 * \param  in_out_memory_size size of the rest of memory block (the size of the data block will be removed)
 * \return true if success else false in case of error
 ****************************************************************************************************/
bool NetImage::read(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size)
{
    if(in_out_memory_size < NetImage::size())
        return false;

    readData(in_out_memory_data, m_error_code          );
    readData(in_out_memory_data, m_image_identifier    );
    readData(in_out_memory_data, m_image_type          );
    readData(in_out_memory_data, m_serial_lenght       );
    readData(in_out_memory_data, m_parallel_lenght     );
    readData(in_out_memory_data, m_total_nb_packets    );
    readData(in_out_memory_data, m_current_packets_nb  );
    readData(in_out_memory_data, m_offset              );
    readData(in_out_memory_data, m_specific_data_lenght);

    in_out_memory_size -= NetImage::size();

    return true;
}

/****************************************************************************************************
 * \fn bool write(uint8_t * out_memory_data, std::size_t & in_out_memory_size)
 * \brief  write the class members values into a memory block
 * \param  in_out_memory_data start of the memory block to be filled (moves to the next data block)
 * \param  in_out_memory_size size of the rest of the memory block (the size of the data block will be removed)
 * \return true if success else false in case of error
 ****************************************************************************************************/
bool NetImage::write(uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) const
{
    if(in_out_memory_size < NetImage::size())
        return false;

    writeData(in_out_memory_data, m_error_code          );
    writeData(in_out_memory_data, m_image_identifier    );
    writeData(in_out_memory_data, m_image_type          );
    writeData(in_out_memory_data, m_serial_lenght       );
    writeData(in_out_memory_data, m_parallel_lenght     );
    writeData(in_out_memory_data, m_total_nb_packets    );
    writeData(in_out_memory_data, m_current_packets_nb  );
    writeData(in_out_memory_data, m_offset              );
    writeData(in_out_memory_data, m_specific_data_lenght);

    in_out_memory_size -= NetImage::size();

    return true;
}

/****************************************************************************************************
 * \fn bool read(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size)
 * \brief  totally read the values stored into a memory block and fill them into the class members
 * \param  in_out_memory_data start of the memory block to be read (moves to the next data block)
 * \param  in_out_memory_size size of the rest of memory block (the size of the data block will be removed)
 * \return true if success else false in case of error
 ****************************************************************************************************/
bool NetImage::totalRead(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size)
{
    if(!NetGenericHeader::totalRead(in_out_memory_data, in_out_memory_size))
        return false;

    return NetImage::read(in_out_memory_data, in_out_memory_size);
}

/****************************************************************************************************
 * \fn bool write(uint8_t * out_memory_data, std::size_t & in_out_memory_size)
 * \brief  totally write the class members values into a memory block
 * \param  in_out_memory_data start of the memory block to be filled (moves to the next data block)
 * \param  in_out_memory_size size of the rest of the memory block (the size of the data block will be removed)
 * \return true if success else false in case of error
 ****************************************************************************************************/
bool NetImage::totalWrite(uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) const
{
    if(!NetGenericHeader::totalWrite(in_out_memory_data, in_out_memory_size))
        return false;

    return NetImage::write(in_out_memory_data, in_out_memory_size);
}

/****************************************************************************************************
 * \fn void log() const
 * \brief  log the class content
 * \param  none
 * \return none
 ****************************************************************************************************/
void NetImage::log() const
{
    std::cout << "-- NetImage content --" << std::endl;
    std::cout << "m_error_code: " << m_error_code << std::endl;
    std::cout << "m_error_code: " << m_image_identifier << std::endl;
    std::cout << "m_error_code: " << m_image_type << std::endl;
    std::cout << "m_error_code: " << m_serial_lenght << std::endl;
    std::cout << "m_error_code: " << m_parallel_lenght << std::endl;
    std::cout << "m_error_code: " << m_total_nb_packets << std::endl;
    std::cout << "m_error_code: " << m_current_packets_nb << std::endl;
    std::cout << "m_error_code: " << m_offset << std::endl;
    std::cout << "m_error_code: " << m_specific_data_lenght << std::endl;
}

/****************************************************************************************************
 * \fn void totalLog() const
 * \brief  totally log the classes content (recursive)
 * \param  none
 * \return none
 ****************************************************************************************************/
void NetImage::totalLog() const
{
    NetGenericHeader::totalLog();
    NetImage::log();
}

//###########################################################################