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
    m_packet_name = "Image";
}

/****************************************************************************************************
 * \fn size()
 * \brief  get the specific packet size
 * \param  none
 * \return specific packet size
 ****************************************************************************************************/
std::size_t NetImage::size() const
{
    return (m_image.size() * sizeof(uint16_t));
}

/****************************************************************************************************
 * \fn totalSize()
 * \brief  get the total packet size
 * \param  none
 * \return total packet size
 ****************************************************************************************************/
std::size_t NetImage::totalSize() const
{
    return NetImageHeader::totalSize() + NetImage::size();
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
    m_image.resize(in_out_memory_size / sizeof(uint16_t));
    memcpy((char*)m_image.data(), reinterpret_cast<const char *>(in_out_memory_data), in_out_memory_size);

    uint16_t    * ptr = m_image.data();
    std::size_t   nb  = m_image.size();

    while(nb--)
    {
        *ptr++ = UINT16_TO_HOST(*ptr);
    }

    in_out_memory_data += NetImage::size();

    if(in_out_memory_size == NetImage::size())
    {
        in_out_memory_size = 0;
    }
    else
    {
        in_out_memory_size -= NetImage::size();
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
bool NetImage::write(uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) const
{
    if(in_out_memory_size != NetImage::size())
        return false;

    memcpy(reinterpret_cast<char *>(in_out_memory_data), m_image.data(), in_out_memory_size);  

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
    if(!NetImageHeader::totalRead(in_out_memory_data, in_out_memory_size))
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
    if(!NetImageHeader::totalWrite(in_out_memory_data, in_out_memory_size))
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
    std::cout << "nb pixels in m_image: " << m_image.size() << std::endl;
}

/****************************************************************************************************
 * \fn void totalLog() const
 * \brief  totally log the classes content (recursive)
 * \param  none
 * \return none
 ****************************************************************************************************/
void NetImage::totalLog() const
{
    NetImageHeader::totalLog();
    NetImage::log();
}

/****************************************************************************************************
 * \fn bool copy() const
 * \brief  copy the image part into a destination buffer
 * \param  in_out_buffer destination copy buffer 
 * \param  in_buffer_dim destination buffer data
 * \return true if the copy was a success, else false
 ****************************************************************************************************/
bool NetImage::copy(void * in_out_buffer, lima::FrameDim & in_buffer_dim) const
{
    // check the image type
    if((static_cast<NetCommandRetrieveImage::TransfertType>(m_image_type) != NetCommandRetrieveImage::TransfertType::TransfertU16) ||
       (in_buffer_dim.getDepth() != 2)) // 16 bits only at the moment
    {
        std::cout << "NetImage::copy - error for image type: " << m_image_type << std::endl;
        return false;
    }

    // compute the position in the destination buffer
    uint16_t * dest = static_cast<uint16_t *>(in_out_buffer) + m_offset;

    // copy the image part
    memcpy(reinterpret_cast<char *>(dest), 
           reinterpret_cast<const char *>(m_image.data()),
           m_image.size() * sizeof(uint16_t)); 

    return true;
}

//###########################################################################