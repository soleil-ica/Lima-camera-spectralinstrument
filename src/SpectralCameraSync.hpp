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
bool Camera::checkTrigMode(TrigMode trig_mode) ///< [in] trigger mode to check
{
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(trig_mode);
    return true;
}

//-----------------------------------------------------------------------------
/// Set the new trigger mode
//-----------------------------------------------------------------------------
void Camera::setTrigMode(TrigMode mode) ///< [in] trigger mode to set
{
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(mode);

//    m_trig_mode = mode;    
}

//-----------------------------------------------------------------------------
/// Get the current trigger mode
//-----------------------------------------------------------------------------
void Camera::getTrigMode(TrigMode& mode) ///< [out] current trigger mode
{
    DEB_MEMBER_FUNCT();
//    NetAnswerGetSettings::AcquisitionType acquisition_type = CameraControl::getConstInstance()->getAcquisitionType();
    mode = TrigMode::IntTrig;
    DEB_RETURN() << DEB_VAR1(mode);
}

//-----------------------------------------------------------------------------
/// Set the new exposure time
//-----------------------------------------------------------------------------
void Camera::setExpTime(double exp_time) ///< [in] exposure time to set
{
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(exp_time);

/*    m_exp_time = exp_time;

    double temp_exp_time;
    getExpTime(temp_exp_time);
    manage_trace( deb, "Changed Exposure time", DCAMERR_NONE, NULL, "exp:%lf >> real:%lf", m_exp_time, temp_exp_time);*/
}

//-----------------------------------------------------------------------------
/// Get the current exposure time
//-----------------------------------------------------------------------------
void Camera::getExpTime(double& exp_time) ///< [out] current exposure time
{
    DEB_MEMBER_FUNCT();
    exp_time = static_cast<double>(CameraControl::getConstInstance()->getExposureTimeMsec());
}

//-----------------------------------------------------------------------------
/// Set the new latency time between images
//-----------------------------------------------------------------------------
void Camera::setLatTime(double lat_time) ///< [in] latency time
{
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(lat_time);

/*    if (lat_time != 0.0)
    {
        THROW_HW_ERROR(Error) << "Latency is not supported";
    }*/
}

//-----------------------------------------------------------------------------
/// Get the current latency time
//-----------------------------------------------------------------------------
void Camera::getLatTime(double& lat_time) ///< [out] current latency time
{
    DEB_MEMBER_FUNCT();
//    lat_time = 0.0;
    DEB_RETURN() << DEB_VAR1(lat_time);
}

//-----------------------------------------------------------------------------
/// Get the exposure time range
//-----------------------------------------------------------------------------
void Camera::getExposureTimeRange(double& min_expo,    ///< [out] minimum exposure time
                                  double& max_expo) ///< [out] maximum exposure time
                                  const
{
    DEB_MEMBER_FUNCT();

    min_expo = 0.0;
    max_expo = 100000.0;

    DEB_RETURN() << DEB_VAR2(min_expo, max_expo);
}

//-----------------------------------------------------------------------------
///  Get the latency time range
//-----------------------------------------------------------------------------
void Camera::getLatTimeRange(double& min_lat, ///< [out] minimum latency
                             double& max_lat) ///< [out] maximum latency
                             const
{   
    DEB_MEMBER_FUNCT();

    min_lat = 0.0;
    max_lat = 100000.0;

    DEB_RETURN() << DEB_VAR2(min_lat, max_lat);
}

//-----------------------------------------------------------------------------
/// Set the number of frames to be taken
//-----------------------------------------------------------------------------
void Camera::setNbFrames(int nb_frames) ///< [in] number of frames to take
{
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(nb_frames);
    
//    m_nb_frames = nb_frames;
}

//-----------------------------------------------------------------------------
/// Get the number of frames to be taken
//-----------------------------------------------------------------------------
void Camera::getNbFrames(int& nb_frames) ///< [out] current number of frames to take
{
    DEB_MEMBER_FUNCT();
//    nb_frames = m_nb_frames;
    nb_frames = 0;
    DEB_RETURN() << DEB_VAR1(nb_frames);
}

//-----------------------------------------------------------------------------
/// Get the current acquired frames
//-----------------------------------------------------------------------------
void Camera::getNbHwAcquiredFrames(int &nb_acq_frames)
{ 
    DEB_MEMBER_FUNCT();    
//    nb_acq_frames = m_image_number;
}
