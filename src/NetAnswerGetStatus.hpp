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
// Class NetAnswerGetStatus
//===================================================================================================
const std::string NetAnswerGetStatus::g_server_flags_status_name    = "Server Flags";
const std::string NetAnswerGetStatus::g_server_flags_delimiter      = ",";
const std::size_t NetAnswerGetStatus::g_server_flags_value_position = 1; // starts at 0

/****************************************************************************************************
 * \fn NetAnswerGetStatus()
 * \brief  constructor
 * \param  none
 * \return none
 ****************************************************************************************************/
NetAnswerGetStatus::NetAnswerGetStatus()
{
    m_packet_name = "Answer GetStatus";
}

/****************************************************************************************************
 * \fn void log() const
 * \brief  log the class content
 * \param  none
 * \return none
 ****************************************************************************************************/
void NetAnswerGetStatus::log() const
{
    std::cout << "-- NetAnswerGetStatus content --" << std::endl;
}

/****************************************************************************************************
 * \fn void totalLog() const
 * \brief  totally log the classes content (recursive)
 * \param  none
 * \return none
 ****************************************************************************************************/
void NetAnswerGetStatus::totalLog() const
{
    NetAnswerGenericString::totalLog();
    NetAnswerGetStatus::log();
}

//###########################################################################