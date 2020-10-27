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
// Class NetAnswerGetSettings
//===================================================================================================
/****************************************************************************************************
 * \fn NetAnswerGetSettings()
 * \brief  constructor
 * \param  none
 * \return none
 ****************************************************************************************************/
NetAnswerGetSettings::NetAnswerGetSettings()
{
    m_packet_name = "Answer GetSettings";

    m_exposure_time_msec   = 0; // exposure time in milli-seconds
    m_readout_modes_nb     = 0; // Number of Readout Modes defined for the camera
    m_readout_mode         = 0; // Current Readout Mode
    m_nb_images_to_average = 0; // Number of Images to Acquire and Average
    m_nb_images_to_acquire = 0; // Number of Frames to Acquire
    m_acquisition_mode     = 0; // SI Image SGL II Acquisition Mode
    m_acquisition_type     = 0; // SI Image SGL II Acquisition Type
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
std::size_t NetAnswerGetSettings::size() const
{
    return sizeof(m_exposure_time_msec  ) + 
           sizeof(m_readout_modes_nb    ) + 
           sizeof(m_readout_mode        ) +
           sizeof(m_nb_images_to_average) + 
           sizeof(m_nb_images_to_acquire) +
           sizeof(m_acquisition_mode    ) + 
           sizeof(m_acquisition_type    ) +
           sizeof(m_serial_origin       ) + 
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
std::size_t NetAnswerGetSettings::totalSize() const
{
    return NetGenericAnswer::totalSize() + NetAnswerGetSettings::size();
}

/****************************************************************************************************
 * \fn bool read(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size)
 * \brief  read the values stored into a memory block and fill them into the class members
 * \param  in_out_memory_data start of the memory block to be read (moves to the next data block)
 * \param  in_out_memory_size size of the rest of memory block (the size of the data block will be removed)
 * \return true if success else false in case of error
 ****************************************************************************************************/
bool NetAnswerGetSettings::read(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size)
{
    if(in_out_memory_size != NetAnswerGetSettings::size())
        return false;

    readData(in_out_memory_data, m_exposure_time_msec  );
    readData(in_out_memory_data, m_readout_modes_nb    );
    readData(in_out_memory_data, m_readout_mode        );
    readData(in_out_memory_data, m_nb_images_to_average);
    readData(in_out_memory_data, m_nb_images_to_acquire);
    readData(in_out_memory_data, m_acquisition_mode    );
    readData(in_out_memory_data, m_acquisition_type    );
    readData(in_out_memory_data, m_serial_origin       );
    readData(in_out_memory_data, m_serial_length       );
    readData(in_out_memory_data, m_serial_binning      );
    readData(in_out_memory_data, m_parallel_origin     );
    readData(in_out_memory_data, m_parallel_length     );
    readData(in_out_memory_data, m_parallel_binning    );

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
bool NetAnswerGetSettings::write(uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) const
{
    if(in_out_memory_size != NetAnswerGetSettings::size())
        return false;

    writeData(in_out_memory_data, m_exposure_time_msec  );
    writeData(in_out_memory_data, m_readout_modes_nb    );
    writeData(in_out_memory_data, m_readout_mode        );
    writeData(in_out_memory_data, m_nb_images_to_average);
    writeData(in_out_memory_data, m_nb_images_to_acquire);
    writeData(in_out_memory_data, m_acquisition_mode    );
    writeData(in_out_memory_data, m_acquisition_type    );
    writeData(in_out_memory_data, m_serial_origin       );
    writeData(in_out_memory_data, m_serial_length       );
    writeData(in_out_memory_data, m_serial_binning      );
    writeData(in_out_memory_data, m_parallel_origin     );
    writeData(in_out_memory_data, m_parallel_length     );
    writeData(in_out_memory_data, m_parallel_binning    );

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
bool NetAnswerGetSettings::totalRead(const uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size)
{
    if(!NetGenericAnswer::totalRead(in_out_memory_data, in_out_memory_size))
        return false;

    return NetAnswerGetSettings::read(in_out_memory_data, in_out_memory_size);
}

/****************************************************************************************************
 * \fn bool write(uint8_t * out_memory_data, std::size_t & in_out_memory_size)
 * \brief  totally write the class members values into a memory block
 * \param  in_out_memory_data start of the memory block to be filled (moves to the next data block)
 * \param  in_out_memory_size size of the rest of the memory block (the size of the data block will be removed)
 * \return true if success else false in case of error
 ****************************************************************************************************/
bool NetAnswerGetSettings::totalWrite(uint8_t * & in_out_memory_data, std::size_t & in_out_memory_size) const
{
    if(!NetGenericAnswer::totalWrite(in_out_memory_data, in_out_memory_size))
        return false;

    return NetAnswerGetSettings::write(in_out_memory_data, in_out_memory_size);
}

/****************************************************************************************************
 * \fn void log() const
 * \brief  log the class content
 * \param  none
 * \return none
 ****************************************************************************************************/
void NetAnswerGetSettings::log() const
{
    std::cout << "-- NetAnswerGetSettings content --" << std::endl;
    std::cout << "m_exposure_time_msec: " << m_exposure_time_msec << std::endl;
    std::cout << "m_readout_modes_nb: " << m_readout_modes_nb << std::endl;
    std::cout << "m_readout_mode: " << m_readout_mode << std::endl;
    std::cout << "m_nb_images_to_average: " << m_nb_images_to_average << std::endl;
    std::cout << "m_nb_images_to_acquire: " << m_nb_images_to_acquire << std::endl;
    std::cout << "m_acquisition_mode: " << m_acquisition_mode << std::endl;
    std::cout << "m_acquisition_type: " << m_acquisition_type << std::endl;
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
void NetAnswerGetSettings::totalLog() const
{
    NetGenericAnswer::totalLog();
    NetAnswerGetSettings::log();
}

//###########################################################################