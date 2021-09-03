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
/****************************************************************************************************
 * \file   CameraControlInit.cpp
 * \brief  implementation file of camera control init parameters structure.
 * \author C�dric Castel - SOLEIL (MEDIANE SYSTEME - IT consultant) 
 * \date   Created on November 02, 2020
 ****************************************************************************************************/

// PROJECT
#include "CameraControlInit.h"

using namespace lima;
using namespace lima::SpectralInstrument;

//------------------------------------------------------------------
// CameraControlInit structure
//------------------------------------------------------------------
/****************************************************************************************************
 * \fn void setCameraIdentifier(int in_camera_identifier)
 * \brief  configure the camera identifier
 * \param  in_camera_identifier camera identifier
 * \return none
 ****************************************************************************************************/
void CameraControlInit::setCameraIdentifier(int in_camera_identifier)
{
    m_camera_identifier = in_camera_identifier;
}

/****************************************************************************************************
 * \fn void setConnectionTimeoutSec(int in_connection_timeout_sec)
 * \brief  configure the connection timeout in seconds
 * \param  in_connection_timeout_sec new connection timeout in seconds
 * \return none
 ****************************************************************************************************/
void CameraControlInit::setConnectionTimeoutSec(int in_connection_timeout_sec)
{
    m_connection_timeout_sec = in_connection_timeout_sec;
}

/****************************************************************************************************
 * \fn void setReceptionTimeoutSec(int in_reception_timeout_sec)
 * \brief  configure the reception timeout in seconds
 * \param  in_reception_timeout_sec new reception timeout in seconds
 * \return none
 ****************************************************************************************************/
void CameraControlInit::setReceptionTimeoutSec(int in_reception_timeout_sec)
{
    m_reception_timeout_sec = in_reception_timeout_sec;
}

/****************************************************************************************************
 * \fn void setWaitPacketTimeoutSec(int in_wait_packet_timeout_sec)
 * \brief  configure the wait packet timeout in seconds
 * \param  in_wait_packet_timeout_sec wait packet timeout in seconds
 * \return none
 ****************************************************************************************************/
void CameraControlInit::setWaitPacketTimeoutSec(int in_wait_packet_timeout_sec)
{
    m_wait_packet_timeout_sec = in_wait_packet_timeout_sec;
}

/****************************************************************************************************
 * \fn void setWaitPacketTimeoutSec(int in_wait_packet_timeout_sec)
 * \brief  configure the maximum readout time in seconds
 * \param  in_maximum_readout_time_sec maximum readout time in seconds
 * \return none
 ****************************************************************************************************/
void CameraControlInit::setMaximumReadoutTimeSec(int in_maximum_readout_time_sec)
{
    m_maximum_readout_time_sec = in_maximum_readout_time_sec;
}

/****************************************************************************************************
 * \fn void setDelayToCheckAcqEndMsec(int in_delay_to_check_acq_end_msec)
 * \brief  configure the delay in milli-seconds between two tries to check if the acquisition is finished
 * \param  in_delay_to_check_acq_end_msec delay in milli-seconds between two tries to check if the acquisition is finished
 * \return none
 ****************************************************************************************************/
void CameraControlInit::setDelayToCheckAcqEndMsec(int in_delay_to_check_acq_end_msec)
{
    m_delay_to_check_acq_end_msec = in_delay_to_check_acq_end_msec;
}

/****************************************************************************************************
 * \fn void setInquireAcqStatusDelayMsec(int in_inquire_acq_status_delay_msec)
 * \brief  configure the delay in milli-seconds between two sends of inquire status commands
 * \param  in_delay_to_check_acq_end_msec delay in milli-seconds between two sends of inquire status commands
 * \return none
 ****************************************************************************************************/
void CameraControlInit::setInquireAcqStatusDelayMsec(int in_inquire_acq_status_delay_msec)
{
    m_inquire_acq_status_delay_msec = in_inquire_acq_status_delay_msec;
}
