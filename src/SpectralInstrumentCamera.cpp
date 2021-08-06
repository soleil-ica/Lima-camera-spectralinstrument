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
#include "SpectralInstrumentCamera.h"
#include "CameraControl.h"
#include "CameraUpdateDataThread.h"
#include "CameraAcqThread.h"

using namespace lima;
using namespace lima::Spectral;

//-----------------------------------------------------------------------------
const double Camera::g_pixel_size_x = 75e-6; // pixel size is ? micron
const double Camera::g_pixel_size_y = 75e-6; // pixel size is ? micron

// we split the camera source code into several functionnalities blocks 
#include "SpectralInstrumentCameraInterface.hpp"
#include "SpectralInstrumentCameraBin.hpp"
#include "SpectralInstrumentCameraRoi.hpp"
#include "SpectralInstrumentCameraSync.hpp"
#include "SpectralInstrumentCameraDetInfo.hpp"

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

    // TODO? - property or move the constant in the class
    const int connection_timeout_sec        = 2   ; // connection timeout in seconds
    const int reception_timeout_sec         = 2   ; // reception timeout in seconds
    const int wait_packet_timeout_sec       = 3   ; // wait packet timeout in seconds
    const int camera_identifier             = 1   ; // camera identifier (starts at 1)
    const int data_update_delay_msec        = 1000; // delay between the data update (status, exposure time, etc...) in msec
    const int maximum_readout_time_sec      = 20  ; // maximum readout time in seconds
    const int delay_to_check_acq_end_msec   = 1   ; // delay in milli-seconds between two tries to check if the acquisition is finished
    const int inquire_acq_status_delay_msec = 20  ; // delay in milli-seconds between two sends of inquire status commands

    m_connection_address           = connection_address          ;
    m_connection_port              = connection_port             ;
    m_image_packet_pixels_nb       = image_packet_pixels_nb      ;
    m_image_packet_delay_micro_sec = image_packet_delay_micro_sec;
    m_nb_frames_to_acquire         = 0                           ;
    m_nb_frames_acquired           = 0                           ;
    m_latency_time_msec            = 0                           ;
    m_trigger_mode                 = lima::TrigMode::IntTrig     ;
    m_update_authorize_flag        = true                        ;

    setDataUpdateDelayMsec(data_update_delay_msec);

    DEB_TRACE() << "Starting Spectral camera...";

    // give access to the camera instance as a singleton
    g_singleton = this;

    // creating the camera control instance
    CameraControlInit init_parameters;

    init_parameters.setCameraIdentifier         (camera_identifier            );
    init_parameters.setConnectionTimeoutSec     (connection_timeout_sec       );
    init_parameters.setReceptionTimeoutSec      (reception_timeout_sec        );
    init_parameters.setWaitPacketTimeoutSec     (wait_packet_timeout_sec      );
    init_parameters.setMaximumReadoutTimeSec    (maximum_readout_time_sec     );
    init_parameters.setDelayToCheckAcqEndMsec   (delay_to_check_acq_end_msec  );
    init_parameters.setInquireAcqStatusDelayMsec(inquire_acq_status_delay_msec);

    CameraControl::create(init_parameters);

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

    // configure the packets sendings
    if(!CameraControl::getInstance()->configurePackets(m_image_packet_pixels_nb, m_image_packet_delay_micro_sec))
    {
        THROW_HW_ERROR(Error) << "Unable to configurate the camera (Check if it is switched on or if an other software is currently using it).";
    }

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
 * \fn HwEventCtrlObj * getEventCtrlObj()
 * \brief  Return the event control object
 * \param  none
 * \return event control object
 ****************************************************************************************************/
HwEventCtrlObj * Camera::getEventCtrlObj()
{
    return &m_event_ctrl_obj;
}

/****************************************************************************************************
 * \fn HwBufferCtrlObj * getBufferCtrlObj()
 * \brief  return the internal buffer manager
 * \param  none
 * \return internal buffer manager
 ****************************************************************************************************/
HwBufferCtrlObj * Camera::getBufferCtrlObj()
{
    DEB_MEMBER_FUNCT();
    return &m_buffer_ctrl_obj;
}

/****************************************************************************************************
 * \fnStdBufferCbMgr & getStdBufferCbMgr()
 * \brief  return the standard buffer manager
 * \param  none
 * \return standard buffer manager
 ****************************************************************************************************/
StdBufferCbMgr & Camera::getStdBufferCbMgr()
{
    return m_buffer_ctrl_obj.getBuffer();
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
 * \fn lima::AutoMutex updateAuthorizeFlagLock() const
 * \brief  creates an autolock mutex for the update authorize flag access
 * \param  none
 * \return auto mutex
 ****************************************************************************************************/
lima::AutoMutex Camera::updateAuthorizeFlagLock() const
{
    return lima::AutoMutex(m_update_authorize_cond.mutex());
}

/****************************************************************************************************
 * \fn void Camera::setUpdateAuthorizeFlag(bool in_value)
 * \brief  authorize or disable the state update process
 * \param  none
 * \return auto mutex
 ****************************************************************************************************/
void Camera::setUpdateAuthorizeFlag(bool in_value)
{
    // protecting the multi-threads access
    lima::AutoMutex send_command_mutex = updateAuthorizeFlagLock(); 
    m_update_authorize_flag = in_value;
}

/****************************************************************************************************
 * \fn bool updateData()
 * \brief  do an update of several detector data (status, exposure time, etc...)
 * \param  none
 * \return true if succeed, false in case of error
 ****************************************************************************************************/
bool Camera::updateData()
{
    DEB_MEMBER_FUNCT();

    // protecting the multi-threads access
    lima::AutoMutex send_command_mutex = updateAuthorizeFlagLock(); 

    if(m_update_authorize_flag)
    {
        if(!CameraControl::getInstance()->updateStatus())
            return false;

        if(!CameraControl::getInstance()->updateSettings())
            return false;
    }

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

/****************************************************************************************************
 * \fn void setNbFramesAcquired(std::size_t in_nb_frames_acquired)
 * \brief  set the number of acquired frames
 * \param  in_nb_frames_acquired new number of acquired frames
 * \return none
 ****************************************************************************************************/
void Camera::setNbFramesAcquired(std::size_t in_nb_frames_acquired)
{
    m_nb_frames_acquired = in_nb_frames_acquired;
}

/****************************************************************************************************
 * \fn std::size_t getNbFramesAcquired() const
 * \brief  get the number of acquired frames
 * \param  none
 * \return current number of acquired frames
 ****************************************************************************************************/
std::size_t Camera::getNbFramesAcquired() const
{
    return m_nb_frames_acquired;
}

/****************************************************************************************************
 * \fn void incrementNbFramesAcquired()
 * \brief  increment the number of acquired frames
 * \param  none
 * \return none
 ****************************************************************************************************/
void Camera::incrementNbFramesAcquired()
{
    m_nb_frames_acquired++;
}

/****************************************************************************************************
 * \fn bool allFramesAcquired() const
 * \brief  check if all the frames were acquired
 * \param  none
 * \return true is the acquisition is completed else false
 ****************************************************************************************************/
bool Camera::allFramesAcquired() const
{
    return (m_nb_frames_to_acquire == m_nb_frames_acquired);
}


/****************************************************************************************************
 * \fn bool getCoolingValue() const
 * \brief  get current Cooling value (On, Off) 
 * \param  none
 * \return true is Cooling = On else false
 ****************************************************************************************************/
bool Camera::getCoolingValue()
{
    uint8_t tmp;
    getCooling(tmp);
    m_cooling_value = static_cast<bool>(tmp);
    return m_cooling_value;
}

/****************************************************************************************************
 * \fn void setCoolingValue() const
 * \brief  set current Cooling value (On, Off) 
 * \param  in_cooling_value new Cooling value
 * \return true is Cooling = On else false
 ****************************************************************************************************/
void Camera::setCoolingValue(bool in_cooling_value)
{
    m_cooling_value = in_cooling_value;
    setCooling(static_cast<uint8_t>(m_cooling_value) );
}


/****************************************************************************************************
 * \fn float& getCCDTemperature()
 * \brief  get current camera temperature
 * \param  none
 * \return camera temperature
 ****************************************************************************************************/
float& Camera::getCCDTemperature()
{
    getCCDTemperatureFromCamera(m_ccd_temperature_value);
    return m_ccd_temperature_value;
}

/****************************************************************************************************
 * \fn ushort& getReadoutSpeed()
 * \brief  get current DSI Sample Time/Readout Speed value
 * \param  none
 * \return DSI Sample Time/Readout Speed value
 ****************************************************************************************************/
ushort& Camera::getReadoutSpeed()
{
    getReadoutSpeedFromCamera(m_readout_speed_value_sc);
    return m_readout_speed_value_sc;
}

/****************************************************************************************************
 * \fn void setReadoutSpeedValue(ushort readout_speed_value)
 * \brief  set current DSI Sample Time/Readout Speed value
 * \param  readout_speed_value new readout speed
 * \return none
 ****************************************************************************************************/
void Camera::setReadoutSpeedValue(ushort readout_speed_value)
{
    m_readout_speed_value_sc = readout_speed_value;
    setReadoutSpeed(static_cast<uint32_t>(m_readout_speed_value_sc));
}