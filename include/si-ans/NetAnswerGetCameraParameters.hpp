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
// Class NetAnswerGetCameraParameters
//===================================================================================================
const std::string NetAnswerGetCameraParameters::g_server_flags_group_factory_name       = "Factory"      ;
const std::string NetAnswerGetCameraParameters::g_server_flags_group_miscellaneous_name = "Miscellaneous";
const std::string NetAnswerGetCameraParameters::g_server_flags_group_control_name = "Control";

const std::string NetAnswerGetCameraParameters::g_server_flags_instrument_model_name          = "Instrument Model";
const std::string NetAnswerGetCameraParameters::g_server_flags_instrument_serial_number_name  = "Instrument SN"   ;
const std::string NetAnswerGetCameraParameters::g_server_flags_instrument_serial_size_name    = "Serial Size"     ;
const std::string NetAnswerGetCameraParameters::g_server_flags_instrument_parallel_size_name  = "Parallel Size"   ;
const std::string NetAnswerGetCameraParameters::g_server_flags_instrument_bits_per_pixel_name = "Bits Per Pixel"  ;

const std::string NetAnswerGetCameraParameters::g_server_flags_control_dsi_sample_time_name = "DSI Sample Time";
const std::string NetAnswerGetCameraParameters::g_camera_enum_readout_1MHz_value = "1MHz";
const std::string NetAnswerGetCameraParameters::g_camera_enum_readout_690KHz_value = "690KHz";

const std::string NetAnswerGetCameraParameters::g_server_flags_delimiter      = ",";
const std::size_t NetAnswerGetCameraParameters::g_server_flags_value_position = 2  ; // starts at 0

/****************************************************************************************************
 * \fn NetAnswerGetCameraParameters()
 * \brief  constructor
 * \param  none
 * \return none
 ****************************************************************************************************/
NetAnswerGetCameraParameters::NetAnswerGetCameraParameters()
{
    m_packet_name = "Answer GetCameraParameters";
}

/****************************************************************************************************
 * \fn void log() const
 * \brief  log the class content
 * \param  none
 * \return none
 ****************************************************************************************************/
void NetAnswerGetCameraParameters::log() const
{
    std::cout << "-- NetAnswerGetCameraParameters content --" << std::endl;
}

/****************************************************************************************************
 * \fn void totalLog() const
 * \brief  totally log the classes content (recursive)
 * \param  none
 * \return none
 ****************************************************************************************************/
void NetAnswerGetCameraParameters::totalLog() const
{
    NetAnswerGenericString::totalLog();
    NetAnswerGetCameraParameters::log();
}
//###########################################################################