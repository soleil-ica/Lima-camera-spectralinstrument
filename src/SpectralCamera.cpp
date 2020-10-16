///###########################################################################
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
//############################################################################

#include <sstream>
#include <iostream>
#include <string>
#include <math.h>
#include <algorithm>
#include "SpectralCamera.h"

using namespace lima;
using namespace lima::Spectral;

//-----------------------------------------------------------------------------
const double Camera::g_pixel_size_x = 75e-6; // pixel size is ? micron
const double Camera::g_pixel_size_y = 75e-6; // pixel size is ? micron

// we split the camera source code into several functionnalities blocks 
#include "SpectralCameraInterface.hpp"
#include "SpectralCameraBin.hpp"
#include "SpectralCameraRoi.hpp"
#include "SpectralCameraSync.hpp"
#include "SpectralCameraDetInfo.hpp"

//-----------------------------------------------------------------------------
///  Ctor
//-----------------------------------------------------------------------------
Camera::Camera(const std::string & connection_addtress         , // server name or IP address of the SI Image SGL II software
               unsigned long       connection_port             , // TCP/IP port of the SI Image SGL II software
               unsigned long       image_packet_pixels_nb      , // number of pixels sent into a image part TCP/IP packet
               unsigned long       image_packet_delay_micro_sec) // delay between the sending of two image part TCP/IP packets (in micro-seconds)
{
    DEB_CONSTRUCTOR();

    DEB_TRACE() << "Starting Spectral camera...";

    if(0)
    {
        THROW_HW_ERROR(Error) << "Unable to initialize the camera (Check if it is switched on or if an other software is currently using it).";
    }

    DEB_TRACE() << "Starting done.";
}

//-----------------------------------------------------------------------------
///  Dtor
//-----------------------------------------------------------------------------
Camera::~Camera()
{
    DEB_DESTRUCTOR();

    stopAcq();
               
    // Close camera
    DEB_TRACE() << "Shutdown Spectral camera...";

    DEB_TRACE() << "Shutdown done.";
}

//---------------------------------------------------------------------------------------
//! Camera::execStopAcq()
//---------------------------------------------------------------------------------------
void Camera::execStopAcq()
{
    DEB_MEMBER_FUNCT();
    DEB_TRACE() << "executing StopAcq command...";

    if((getStatus() != Camera::Exposure) && (getStatus() != Camera::Readout))
        DEB_WARNING() << "Execute a stop acq command but an acquisition is not running!";
}

//=============================================================================
//-----------------------------------------------------
// Return the event control object
//-----------------------------------------------------
HwEventCtrlObj* Camera::getEventCtrlObj()
{
    return &m_event_ctrl_obj;
}

//-----------------------------------------------------------------------------
/// return the internal buffer manager
/*!
@ return buffer control object
*/
//-----------------------------------------------------------------------------
HwBufferCtrlObj* Camera::getBufferCtrlObj()
{
    DEB_MEMBER_FUNCT();
    return &m_buffer_ctrl_obj;
}
