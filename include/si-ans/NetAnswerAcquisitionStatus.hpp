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
// Class NetAnswerAcquisitionStatus
//===================================================================================================
/****************************************************************************************************
 * \fn NetAnswerAcquisitionStatus()
 * \brief  constructor
 * \param  none
 * \return none
 ****************************************************************************************************/
NetAnswerAcquisitionStatus::NetAnswerAcquisitionStatus()
{
    m_packet_name = "Answer AcquisitionStatus";

    m_exposure_done    = 0; // % of the exposure time that has elapsed
    m_readout_done     = 0; // % of the readout that is complete
    m_readout_position = 0; // Relative position of readout pointer
    m_current_image    = 0; // Current image being acquired
}

/****************************************************************************************************
 * \fn size()
 * \brief  get the specific packet size
 * \param  none
 * \return specific packet size
 ****************************************************************************************************/
std::size_t NetAnswerAcquisitionStatus::size() const
{
    return sizeof(m_exposure_done   ) + 
           sizeof(m_readout_done    ) + 
           sizeof(m_readout_position) +
           sizeof(m_current_image   );
}

/****************************************************************************************************
 * \fn totalSize()
 * \brief  get the total packet size
 * \param  none
 * \return total packet size
 ****************************************************************************************************/
std::size_t NetAnswerAcquisitionStatus::totalSize() const
{
    return NetGenericAnswer::totalSize() + NetAnswerAcquisitionStatus::size();
}

/****************************************************************************************************
 * \fn bool read(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size)
 * \brief  read the values stored into a memory block and fill them into the class members
 * \param  in_out_memory_data start of the memory block to be read (moves to the next data block)
 * \param  in_out_memory_size size of the rest of memory block (the size of the data block will be removed)
 * \return true if success else false in case of error
 ****************************************************************************************************/
bool NetAnswerAcquisitionStatus::read(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size)
{
    if(in_out_memory_size != NetAnswerAcquisitionStatus::size())
        return false;

    readData(in_out_memory_data, m_exposure_done   );
    readData(in_out_memory_data, m_readout_done    );
    readData(in_out_memory_data, m_readout_position);
    readData(in_out_memory_data, m_current_image   );

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
bool NetAnswerAcquisitionStatus::write(uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) const
{
    if(in_out_memory_size != NetAnswerAcquisitionStatus::size())
        return false;

    writeData(in_out_memory_data, m_exposure_done   );
    writeData(in_out_memory_data, m_readout_done    );
    writeData(in_out_memory_data, m_readout_position);
    writeData(in_out_memory_data, m_current_image   );

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
bool NetAnswerAcquisitionStatus::totalRead(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size)
{
    if(!NetGenericAnswer::totalRead(in_out_memory_data, in_out_memory_size))
        return false;

    return NetAnswerAcquisitionStatus::read(in_out_memory_data, in_out_memory_size);
}

/****************************************************************************************************
 * \fn bool write(uint8_t * out_memory_data, std::size_t & in_out_memory_size)
 * \brief  totally write the class members values into a memory block
 * \param  in_out_memory_data start of the memory block to be filled (moves to the next data block)
 * \param  in_out_memory_size size of the rest of the memory block (the size of the data block will be removed)
 * \return true if success else false in case of error
 ****************************************************************************************************/
bool NetAnswerAcquisitionStatus::totalWrite(uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) const
{
    if(!NetGenericAnswer::totalWrite(in_out_memory_data, in_out_memory_size))
        return false;

    return NetAnswerAcquisitionStatus::write(in_out_memory_data, in_out_memory_size);
}

/****************************************************************************************************
 * \fn void log() const
 * \brief  log the class content
 * \param  none
 * \return none
 ****************************************************************************************************/
void NetAnswerAcquisitionStatus::log() const
{
    std::cout << "-- NetAnswerAcquisitionStatus content --" << std::endl;
    std::cout << "m_exposure_done: " << m_exposure_done << std::endl;
    std::cout << "m_readout_done: " << m_readout_done << std::endl;
    std::cout << "m_readout_position: " << m_readout_position << std::endl;
    std::cout << "m_current_image: " << m_current_image << std::endl;
}

/****************************************************************************************************
 * \fn void totalLog() const
 * \brief  totally log the classes content (recursive)
 * \param  none
 * \return none
 ****************************************************************************************************/
void NetAnswerAcquisitionStatus::totalLog() const
{
    NetGenericAnswer::totalLog();
    NetAnswerAcquisitionStatus::log();
}

//###########################################################################