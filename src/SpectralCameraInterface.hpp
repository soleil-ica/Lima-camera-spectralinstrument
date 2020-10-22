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
/// Get the camera status
//-----------------------------------------------------------------------------
Camera::Status Camera::getStatus() ///< [out] current camera status
{
    DEB_MEMBER_FUNCT();

    Camera::Status result = Camera::Status::Fault;

    CameraControl::getInstance()->updateStatus();
    
    CameraControl::DetectorStatus detector_status = CameraControl::getConstInstance()->getLatestStatus();

    if(detector_status == CameraControl::DetectorStatus::Ready)
        result = Camera::Status::Ready;
    else
    if(detector_status == CameraControl::DetectorStatus::Exposure)
        result = Camera::Status::Exposure;
    else
    if(detector_status == CameraControl::DetectorStatus::Readout)
        result = Camera::Status::Readout;
    else
    if(detector_status == CameraControl::DetectorStatus::Latency)
        result = Camera::Status::Latency;
    else
    if(detector_status == CameraControl::DetectorStatus::Fault)
        result = Camera::Status::Fault;

    return result;
}

//-----------------------------------------------------------------------------
/// reset the camera, no hw reset available on Spectral camera
//-----------------------------------------------------------------------------
void Camera::reset()
{
    DEB_MEMBER_FUNCT();
    return;
}

//-----------------------------------------------------------------------------
/// CAPTURE
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/// Set detector for single image acquisition
//-----------------------------------------------------------------------------
void Camera::prepareAcq()
{
    DEB_MEMBER_FUNCT();
}

//-----------------------------------------------------------------------------
///  start the acquistion
//-----------------------------------------------------------------------------
void Camera::startAcq()
{
    DEB_MEMBER_FUNCT();
}

//-----------------------------------------------------------------------------
/// stop the acquisition
//-----------------------------------------------------------------------------
void Camera::stopAcq()
{
    DEB_MEMBER_FUNCT();
    execStopAcq();
}
