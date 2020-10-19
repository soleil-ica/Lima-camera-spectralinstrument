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

// SYSTEM
#include <sstream>
#include <iostream>
#include <string>
#include <math.h>
#include <algorithm>

// PROJECT
#include "SpectralCamera.h"
#include "CameraControl.h"

using namespace lima;
using namespace lima::Spectral;
using namespace lima::SpectralDetector_ns;

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
// used to give acess to the Camera instance like a singleton.
// The use of a real singleton is not possible due to architectural problems
// (the cameras'management in Lima).
//-----------------------------------------------------------------------------
Camera * Camera::g_singleton = NULL;

/****************************************************************************************************
 * \fn Camera(const std::string & connection_addtress, unsigned long connection_port, unsigned long image_packet_pixels_nb, unsigned long image_packet_delay_micro_sec)
 * \brief  constructor
 * \param  connection_address           server name or IP address of the SI Image SGL II software
 * \param  connection_port              TCP/IP port of the SI Image SGL II software
 * \param  image_packet_pixels_nb       number of pixels sent into a image part TCP/IP packet
 * \param  image_packet_delay_micro_sec delay between the sending of two image part TCP/IP packets (in micro-seconds)
 * \return none
 ****************************************************************************************************/
Camera::Camera(const std::string & connection_address          ,
               unsigned long       connection_port             ,
               unsigned long       image_packet_pixels_nb      ,
               unsigned long       image_packet_delay_micro_sec)
{
    DEB_CONSTRUCTOR();

    const int connection_timeout_sec = 2; // 2 seconds - TODO - property or move the constant in the class

    m_connection_address           = connection_address          ;
    m_connection_port              = connection_port             ;
    m_image_packet_pixels_nb       = image_packet_pixels_nb      ;
    m_image_packet_delay_micro_sec = image_packet_delay_micro_sec;

    DEB_TRACE() << "Starting Spectral camera...";
    g_singleton = this;

    // creating the camera control instance
    CameraControl::create();

    // starting the tcp/ip connection
    CameraControl::getInstance()->SetConnectionTimeout(connection_timeout_sec);
    CameraControl::getInstance()->connect(m_connection_address, m_connection_port);

    if(0)
    {
        THROW_HW_ERROR(Error) << "Unable to initialize the camera (Check if it is switched on or if an other software is currently using it).";
    }

    DEB_TRACE() << "Starting done.";
}

/****************************************************************************************************
 * \fn ~Camera()
 * \brief  destructor
 * \param  none
 * \return none
 ****************************************************************************************************/
Camera::~Camera()
{
    DEB_DESTRUCTOR();

    stopAcq();
               
    // Closing camera
    DEB_TRACE() << "Shutdown Spectral camera...";

    // releasing the camera control instance (also, disconnecting the tcp/ip connection).
    CameraControl::release();

    g_singleton = NULL;

    DEB_TRACE() << "Shutdown done.";
}

/****************************************************************************************************
 * \fn void execStopAcq()
 * \brief  execute a StopAcq command
 * \param  none
 * \return none
 ****************************************************************************************************/
void Camera::execStopAcq()
{
    DEB_MEMBER_FUNCT();
    DEB_TRACE() << "executing StopAcq command...";

    if((getStatus() != Camera::Exposure) && (getStatus() != Camera::Readout))
        DEB_WARNING() << "Execute a stop acq command but an acquisition is not running!";
}

//=============================================================================
/****************************************************************************************************
 * \fn HwEventCtrlObj* getEventCtrlObj()
 * \brief  Return the event control object
 * \param  none
 * \return event control object
 ****************************************************************************************************/
HwEventCtrlObj* Camera::getEventCtrlObj()
{
    return &m_event_ctrl_obj;
}

/****************************************************************************************************
 * \fn HwBufferCtrlObj* getBufferCtrlObj()
 * \brief  return the internal buffer manager
 * \param  none
 * \return internal buffer manager
 ****************************************************************************************************/
HwBufferCtrlObj* Camera::getBufferCtrlObj()
{
    DEB_MEMBER_FUNCT();
    return &m_buffer_ctrl_obj;
}

//=============================================================================
/****************************************************************************************************
 * \fn Camera * getInstance()
 * \brief  access to the singleton
 * \param  none
 * \return singleton
 ****************************************************************************************************/
Camera * Camera::getInstance()
{
    return Camera::g_singleton;
}

/****************************************************************************************************
 * \fn const Camera * getConstInstance()
 * \brief  access to the singleton (const version)
 * \param  none
 * \return singleton (const version)
 ****************************************************************************************************/
const Camera * Camera::getConstInstance()
{
    return Camera::g_singleton;
}
