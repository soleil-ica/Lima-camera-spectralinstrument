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
#include "CameraUpdateDataThread.h"
#include "CameraAcqThread.h"

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

    const int connection_timeout_sec  = 2   ; // 2 seconds - TODO - property or move the constant in the class
    const int reception_timeout_sec   = 2   ; // 2 seconds - TODO - property or move the constant in the class
    const int wait_packet_timeout_sec = 3   ; // 3 seconds - TODO - property or move the constant in the class
    const int camera_identifier       = 1   ; // TODO? - property or move the constant in the class
    const int data_update_delay_msec  = 1000; // TODO? - property or move the constant in the class

    m_connection_address           = connection_address          ;
    m_connection_port              = connection_port             ;
    m_image_packet_pixels_nb       = image_packet_pixels_nb      ;
    m_image_packet_delay_micro_sec = image_packet_delay_micro_sec;
    m_nb_frames_to_acquire         = 0                           ;
    m_nb_frames_acquired           = 0                           ;
    m_latency_time_msec            = 0                           ;
    m_trigger_mode                 = lima::TrigMode::IntTrig     ;

    setDataUpdateDelayMsec(data_update_delay_msec);

    DEB_TRACE() << "Starting Spectral camera...";

    // give access to the camera instance as a singleton
    g_singleton = this;

    // creating the camera control instance
    CameraControl::create(camera_identifier      , 
                          connection_timeout_sec , 
                          reception_timeout_sec  , 
                          wait_packet_timeout_sec);

    // starting the tcp/ip connection
    CameraControl::getInstance()->connect(m_connection_address, m_connection_port);

    // init some data (status, exposure time, etc...)
    if(!CameraControl::getInstance()->initCameraParameters())
    {
        THROW_HW_ERROR(Error) << "Unable to initialize the camera (Check if it is switched on or if an other software is currently using it).";
    }

    // force an update of some data (status, exposure time, etc...)
    if(!updateData())
    {
        THROW_HW_ERROR(Error) << "Unable to initialize the camera (Check if it is switched on or if an other software is currently using it).";
    }
/*
// change the exposure time by sending a command to the hardware
CameraControl::getInstance()->setExposureTimeMsec(50);

// Change the acquisition mode by sending a command to the hardware
CameraControl::getInstance()->setAcquisitionMode(NetAnswerGetSettings::AcquisitionMode::SingleImage);

// Change the format parameters by sending a command to the hardware
CameraControl::getInstance()->setBinning(1, 1);

// Change the roi by sending a command to the hardware
CameraControl::getInstance()->setRoi(5, 10, 200, 400);

// Change the acquisition type by sending a command to the hardware
CameraControl::getInstance()->setAcquisitionType(NetAnswerGetSettings::AcquisitionType::Triggered);
*/

    // creating the data update thread
    CameraUpdateDataThread::create();
    
    // starting the data update
    CameraUpdateDataThread::startUpdate();

    // creating the acquisition thread
    CameraAcqThread::create();

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

    // Stopping the acquisition (if needed)
    CameraAcqThread::stopAcq();

    // Releasing the acquisition thread
    CameraAcqThread::release();

    // Stopping the data update
    CameraUpdateDataThread::stopUpdate();

    // Releasing the data update thread
    CameraUpdateDataThread::release();

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

    // Stopping the acquisition
    CameraAcqThread::stopAcq();
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

//=============================================================================
/****************************************************************************************************
 * \fn bool updateData()
 * \brief  do an update of several detector data (status, exposure time, etc...)
 * \param  none
 * \return true if succeed, false in case of error
 ****************************************************************************************************/
bool Camera::updateData()
{
    if(!CameraControl::getInstance()->updateStatus())
        return false;

    if(!CameraControl::getInstance()->updateSettings())
        return false;

    return true;
}

/****************************************************************************************************
 * \fn void setDataUpdateDelayMsec(int in_data_update_delay_msec)
 * \brief  configure the data update delay in msec
 * \param  in_data_update_delay_msec new data update delay in msec
 * \return none
 ****************************************************************************************************/
void Camera::setDataUpdateDelayMsec(int in_data_update_delay_msec)
{
    m_data_update_delay_msec = in_data_update_delay_msec;
}

/****************************************************************************************************
 * \fn int getDataUpdateDelayMsec() const
 * \brief  get the data update delay in msec
 * \param  none
 * \return data update delay in msec
 ****************************************************************************************************/
int Camera::getDataUpdateDelayMsec() const
{
    return m_data_update_delay_msec;
}
