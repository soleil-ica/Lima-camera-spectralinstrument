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
 * \file   CameraControlInit.h
 * \brief  header file of camera control init parameters structure.
 * \author C�dric Castel - SOLEIL (MEDIANE SYSTEME - IT consultant) 
 * \date   Created on November 02, 2020
 ****************************************************************************************************/

#ifndef SPECTRALINFORMATIQUECAMERACONTROLINIT_H
#define SPECTRALINFORMATIQUECAMERACONTROLINIT_H

// SYSTEM
#include <cstdlib>
#include <cstring>
#include <stdint.h>

// PROJECT
#include "SpectralInstrumentCompatibility.h"

/*
 *  \namespace lima
 */
namespace lima
{
/*
 *  \namespace SpectralInstrument
 */
namespace SpectralInstrument 
{
    // pre-defines the CameraControl class
    class CameraControl;

   /*******************************************************************
    * \struct CameraControlInit
    * \brief This structure is used to communicate the init parameters to the camera control class
    *******************************************************************/

    typedef struct CameraControlInit
    {
        friend class CameraControl;

    public:
        // configure the camera identifier
        void setCameraIdentifier(int in_camera_identifier);

        // configure the connection timeout in seconds
        void setConnectionTimeoutSec(int in_connection_timeout_sec);

        // configure the reception timeout in seconds
        void setReceptionTimeoutSec(int in_reception_timeout_sec);

        // configure the wait packet timeout in seconds
        void setWaitPacketTimeoutSec(int in_wait_packet_timeout_sec);

        // configure the maximum readout time in seconds
        void setMaximumReadoutTimeSec(int in_maximum_readout_time_sec);

        // configure the delay in milli-seconds between two tries to check if the acquisition is finished
        void setDelayToCheckAcqEndMsec(int in_delay_to_check_acq_end_msec);

        // configure the delay in milli-seconds between two sends of inquire status commands
        void setInquireAcqStatusDelayMsec(int in_inquire_acq_status_delay_msec);

    private:
        // camera identifier (starts at 1)
        int m_camera_identifier;

        // connection timeout in seconds
        int m_connection_timeout_sec;

        // reception timeout in seconds
        int m_reception_timeout_sec;

        // wait packet timeout in seconds
        int m_wait_packet_timeout_sec;

        // maximum readout time in seconds
        int m_maximum_readout_time_sec;

        // delay in milli-seconds between two tries to check if the acquisition is finished
        int m_delay_to_check_acq_end_msec;

        // delay in milli-seconds between two sends of inquire status commands
        int m_inquire_acq_status_delay_msec;

    } CameraControlInit;

} // namespace SpectralInstrument
} // namespace lima

#endif // SPECTRALINFORMATIQUECAMERACONTROLINIT_H
