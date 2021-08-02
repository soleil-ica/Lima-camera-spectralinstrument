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

//-----------------------------------------------------------------------------
/// Checks trigger mode
/*!
@return true if the given trigger mode is supported
*/
//-----------------------------------------------------------------------------
bool Camera::checkTrigMode(TrigMode trig_mode) const ///< [in] trigger mode to check
{
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(trig_mode);

    bool valid_mode;    

    switch (trig_mode)
    {       
        case lima::TrigMode::IntTrig      :
        case lima::TrigMode::ExtTrigSingle:
        case lima::TrigMode::ExtTrigMult  :
            valid_mode = true;
            break;

        default:
            valid_mode = false;
            break;
    }

    return valid_mode;
}

//-----------------------------------------------------------------------------
/// Set the new trigger mode
//-----------------------------------------------------------------------------
void Camera::setTrigMode(TrigMode mode) ///< [in] trigger mode to set
{
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(mode);

    if(checkTrigMode(mode))
    {
        m_trigger_mode = mode;
    }
    else
    {
        THROW_HW_ERROR(ErrorType::Error) << "Cannot change the Trigger Mode of the camera, this mode is not managed:" << mode;
    }
}

//-----------------------------------------------------------------------------
/// Get the current trigger mode
//-----------------------------------------------------------------------------
void Camera::getTrigMode(TrigMode& mode) const ///< [out] current trigger mode
{
    DEB_MEMBER_FUNCT();
    mode = m_trigger_mode;
    DEB_RETURN() << DEB_VAR1(mode);
}

//-----------------------------------------------------------------------------
/// Set the new exposure time (ms)
//-----------------------------------------------------------------------------
void Camera::setExpTime(uint32_t exp_time_ms) ///< [in] exposure time to set
{
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(exp_time_ms);
    CameraControl::getInstance()->setExposureTimeMsec(exp_time_ms);
}

//-----------------------------------------------------------------------------
/// Get the current exposure time (ms)
//-----------------------------------------------------------------------------
void Camera::getExpTime(uint32_t & exp_time_ms) const ///< [out] current exposure time
{
    DEB_MEMBER_FUNCT();
    exp_time_ms = CameraControl::getConstInstance()->getExposureTimeMsec();
    DEB_RETURN() << DEB_VAR1(exp_time_ms);
}

//-----------------------------------------------------------------------------
/// Set the new latency time between images
//-----------------------------------------------------------------------------
void Camera::setLatTime(uint32_t lat_time_ms) ///< [in] latency time
{
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(lat_time_ms);
    m_latency_time_msec = lat_time_ms; // latency time in milli-seconds
}

//-----------------------------------------------------------------------------
/// Get the current latency time
//-----------------------------------------------------------------------------
void Camera::getLatTime(uint32_t & lat_time_ms) const ///< [out] current latency time
{
    DEB_MEMBER_FUNCT();
    lat_time_ms = m_latency_time_msec;
    DEB_RETURN() << DEB_VAR1(lat_time_ms);
}

//-----------------------------------------------------------------------------
/// Get the exposure time range (ms)
//-----------------------------------------------------------------------------
void Camera::getExposureTimeRange(uint32_t & min_expo_ms,       ///< [out] minimum exposure time
                                  uint32_t & max_expo_ms) const ///< [out] maximum exposure time
{
    DEB_MEMBER_FUNCT();

    min_expo_ms = 0;
    max_expo_ms = 0xFFFFFFFFu;

    DEB_RETURN() << DEB_VAR2(min_expo_ms, max_expo_ms);
}

//-----------------------------------------------------------------------------
///  Get the latency time range (ms)
//-----------------------------------------------------------------------------
void Camera::getLatTimeRange(uint32_t & min_lat_ms,       ///< [out] minimum latency
                             uint32_t & max_lat_ms) const ///< [out] maximum latency
{   
    DEB_MEMBER_FUNCT();

    min_lat_ms = 0;
    max_lat_ms = 0xFFFFFFFFu;

    DEB_RETURN() << DEB_VAR2(min_lat_ms, max_lat_ms);
}

//-----------------------------------------------------------------------------
/// Set the number of frames to be taken
//-----------------------------------------------------------------------------
void Camera::setNbFrames(int nb_frames) ///< [in] number of frames to take
{
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(nb_frames);
    m_nb_frames_to_acquire = static_cast<std::size_t>(nb_frames);
}

//-----------------------------------------------------------------------------
/// Get the number of frames to be taken
//-----------------------------------------------------------------------------
void Camera::getNbFrames(int & nb_frames) const ///< [out] current number of frames to take
{
    DEB_MEMBER_FUNCT();
    nb_frames = static_cast<int>(m_nb_frames_to_acquire);
    DEB_RETURN() << DEB_VAR1(nb_frames);
}

//-----------------------------------------------------------------------------
/// Get the current acquired frames
//-----------------------------------------------------------------------------
void Camera::getNbHwAcquiredFrames(int & nb_acq_frames) const
{ 
    DEB_MEMBER_FUNCT();    
    nb_acq_frames = static_cast<int>(getNbFramesAcquired());
}


//-----------------------------------------------------------------------------
/// Set the new Cooling value
//-----------------------------------------------------------------------------
void Camera::setCooling(uint8_t cooling_value) ///< [in] cooling value to set
{
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(cooling_value);
    CameraControl::getInstance()->setCoolingValue(cooling_value);
}

//-----------------------------------------------------------------------------
/// Get the current cooling value
//-----------------------------------------------------------------------------
void Camera::getCooling(uint8_t& cooling_value) const ///< [out] current cooling value
{
    DEB_MEMBER_FUNCT();
    cooling_value = CameraControl::getConstInstance()->getCoolingValue();
    DEB_RETURN() << DEB_VAR1(cooling_value);
}



void Camera::getCCDTemperatureFromCamera(std::string& in_out_value)
{
    DEB_MEMBER_FUNCT();
    in_out_value = CameraControl::getConstInstance()->getCCDTemperatureFromCamera();
    DEB_RETURN() << DEB_VAR1(in_out_value);
}
