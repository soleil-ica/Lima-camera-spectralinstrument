
//###########################################################################
// This file is part of LImA, a Library for Image Acquisition
//
// Copyright (C) : 2009-2012
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
#include "SpectralInstrumentSyncCtrlObj.h"

using namespace lima;
using namespace lima::Spectral;
using namespace std;

//-----------------------------------------------------
// @brief Ctor
//-----------------------------------------------------
SyncCtrlObj::SyncCtrlObj(Camera& cam)
    : HwSyncCtrlObj(), m_cam(cam)
{
    DEB_CONSTRUCTOR();
}

//-----------------------------------------------------
// @brief Dtor
//-----------------------------------------------------
SyncCtrlObj::~SyncCtrlObj()
{
    DEB_DESTRUCTOR();
}

//-----------------------------------------------------
// @brief return True if the mode is supported
//-----------------------------------------------------
bool SyncCtrlObj::checkTrigMode(TrigMode trig_mode)
{   
    return m_cam.checkTrigMode(trig_mode);   
 }

//-----------------------------------------------------
// @brief set the trigger mode
//-----------------------------------------------------
void SyncCtrlObj::setTrigMode(TrigMode trig_mode)
{
    DEB_MEMBER_FUNCT();    
    if (!checkTrigMode(trig_mode))
        THROW_HW_ERROR(InvalidValue) << "Invalid " << DEB_VAR1(trig_mode);
    m_cam.setTrigMode(trig_mode);    
}

//-----------------------------------------------------
// @brief
//-----------------------------------------------------
void SyncCtrlObj::getTrigMode(TrigMode& trig_mode)
{
    m_cam.getTrigMode(trig_mode);
}

//-----------------------------------------------------
// @brief
//-----------------------------------------------------
void SyncCtrlObj::setExpTime(double exp_time)
{
    // lima exposure time is in seconds (but shows in ms)
    // camera exposure time is in milli-seconds
    m_cam.setExpTime(static_cast<uint32_t>(exp_time * 1000.0));
}

//-----------------------------------------------------
// @brief
//-----------------------------------------------------
void SyncCtrlObj::getExpTime(double& exp_time)
{
    DEB_MEMBER_FUNCT();    

    // lima exposure time is in seconds (but shows in ms)
    // camera exposure time is in milli-seconds
    uint32_t temp;
    m_cam.getExpTime(temp);
    exp_time = static_cast<double>(temp) / 1000.0;
}

//-----------------------------------------------------
// @brief
//-----------------------------------------------------
void SyncCtrlObj::setLatTime(double lat_time)
{
    // lima latency time is in seconds (but shows in ms)
    // simulated camera latency time is in milli-seconds
    m_cam.setLatTime(static_cast<uint32_t>(lat_time * 1000.0));
}

//-----------------------------------------------------
// @brief
//-----------------------------------------------------
void SyncCtrlObj::getLatTime(double& lat_time)
{
    // lima latency time is in seconds (but shows in ms)
    // simulated camera latency time is in milli-seconds
    uint32_t temp;
    m_cam.getLatTime(temp);
    lat_time = static_cast<double>(temp) / 1000.0;
}

//-----------------------------------------------------
// @brief
//-----------------------------------------------------
void SyncCtrlObj::setNbHwFrames(int nb_frames)
{
    DEB_MEMBER_FUNCT();
    m_cam.setNbFrames(nb_frames);
}

//-----------------------------------------------------
// @brief
//-----------------------------------------------------
void SyncCtrlObj::getNbHwFrames(int& nb_frames)
{
    DEB_MEMBER_FUNCT();    
    m_cam.getNbFrames(nb_frames);
}

//--- @brief--------------------------------------------------
//
//-----------------------------------------------------
void SyncCtrlObj::getValidRanges(ValidRangesType& valid_ranges)
{
    DEB_MEMBER_FUNCT();
    uint32_t min_time;
    uint32_t max_time;

    m_cam.getExposureTimeRange(min_time, max_time);
    valid_ranges.min_exp_time = static_cast<double>(min_time) / 1000.0;
    valid_ranges.max_exp_time = static_cast<double>(max_time) / 1000.0;

    m_cam.getLatTimeRange(min_time, max_time);
    valid_ranges.min_lat_time = static_cast<double>(min_time) / 1000.0;
    valid_ranges.max_lat_time = static_cast<double>(max_time) / 1000.0;
}


//-----------------------------------------------------
// @brief
//-----------------------------------------------------
void SyncCtrlObj::setCooling(bool cooling)
{
    m_cam.setCooling(static_cast<uint8_t>(cooling));
}

//-----------------------------------------------------
// @brief
//-----------------------------------------------------
void SyncCtrlObj::getCooling(bool& cooling)
{
    DEB_MEMBER_FUNCT();    

    uint8_t temp;
    m_cam.getCooling(temp);
    cooling = static_cast<bool>(temp);
}

//-----------------------------------------------------
// @brief
//-----------------------------------------------------
void SyncCtrlObj::getCCDTemperatureFromCamera(float& in_out_value)
{
    m_cam.getCCDTemperatureFromCamera(in_out_value);
}

//-----------------------------------------------------
// @brief
//-----------------------------------------------------
void SyncCtrlObj::getReadoutSpeedFromCamera(ushort& in_out_value)
{
    m_cam.getReadoutSpeedFromCamera(in_out_value);
}

//-----------------------------------------------------
// @brief
//-----------------------------------------------------
void SyncCtrlObj::setReadoutSpeed(ushort readout_speed)
{
    m_cam.setReadoutSpeed(readout_speed);
}


