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

    Camera::Status result;

    int thread_status = CameraAcqThread::readStatus();

    // error during the acquisition management ?
    // the device becomes in error state.
    if(thread_status == CameraAcqThread::Error)
    {
        result = Camera::Status::Fault;
    }
    else
    // the device is in acquisition.
    if(thread_status == CameraAcqThread::Running)
    {
        // get the current running status in detail
        CameraAcqThread::RunningState running_state = CameraAcqThread::getRunningState();

        if(running_state == CameraAcqThread::RunningState::Exposure)
        {
            result = Camera::Status::Exposure;
        }
        else
        if(running_state == CameraAcqThread::RunningState::Readout)
        {
            result = Camera::Status::Readout;
        }
        else
        if((running_state == CameraAcqThread::RunningState::Retrieve) ||
           (running_state == CameraAcqThread::RunningState::Latency ))
        {
            result = Camera::Status::Latency;
        }
    }
    else
    // the device is not in acquisition or in error, so we can read the hardware camera status
    {
        CameraControl::DetectorStatus detector_status = CameraControl::getConstInstance()->getLatestStatus();

        switch (detector_status)
        {
            case CameraControl::DetectorStatus::Ready   : result = Camera::Status::Ready   ; break;
            case CameraControl::DetectorStatus::Exposure: result = Camera::Status::Exposure; break;
            case CameraControl::DetectorStatus::Readout : result = Camera::Status::Readout ; break;
            case CameraControl::DetectorStatus::Latency : result = Camera::Status::Latency ; break;
            case CameraControl::DetectorStatus::Fault   : result = Camera::Status::Fault   ; break;

            default: result = Camera::Status::Fault; break;
        }
    }

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

    //================================================================================================
    // before a new acquisition, some data need to be updated
    //================================================================================================
    // Forcing the acquisition mode to single image mode by sending a command to the hardware
    CameraControl::getInstance()->setAcquisitionMode(NetAnswerGetSettings::AcquisitionMode::SingleImage);

    // Change the acquisition type by sending a command to the hardware
    NetAnswerGetSettings::AcquisitionType acquisition_type;

    switch (m_trigger_mode)
    {       
        case lima::TrigMode::IntTrig:
            acquisition_type = NetAnswerGetSettings::AcquisitionType::Light;
            break;

        case lima::TrigMode::ExtTrigSingle:
        case lima::TrigMode::ExtTrigMult  :
            acquisition_type = NetAnswerGetSettings::AcquisitionType::Triggered;
            break;

        default:
            THROW_HW_ERROR(ErrorType::Error) << "startAcq - Incoherent selected trigger mode: " << m_trigger_mode << "!";
            break;
    }

    CameraControl::getInstance()->setAcquisitionType(acquisition_type);

    // reinit the number of frames
    setNbFramesAcquired(0);

    //================================================================================================
    // starting the acquisition thread
    //================================================================================================
    CameraAcqThread::startAcq();
}

//-----------------------------------------------------------------------------
/// stop the acquisition
//-----------------------------------------------------------------------------
void Camera::stopAcq()
{
    DEB_MEMBER_FUNCT();

    //================================================================================================
    // stopping the acquisition thread
    //================================================================================================
    CameraAcqThread::stopAcq();
}
