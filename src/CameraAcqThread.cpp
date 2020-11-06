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
 * \file   CameraAcqThread.cpp
 * \brief  implementation file of the receive data thread class.
 * \author Cédric Castel - SOLEIL (MEDIANE SYSTEME - IT consultant) 
 * \date   Created on October 28, 2020
 ****************************************************************************************************/

// PROJECT
#include "CameraAcqThread.h"
#include "SpectralCamera.h"
#include "CameraControl.h"

// SYSTEM
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>  
#include <sys/time.h>
#include <sstream>

// LIMA
#include "lima/HwEventCtrlObj.h"

using namespace lima;
using namespace lima::Spectral;

//------------------------------------------------------------------
// internal timer management
//------------------------------------------------------------------
class lima::Spectral::InternalTimer
{
public:
    // init the start time
    void init()
    {
        gettimeofday(&m_start_time, NULL);
    }

    // get the elapsed time in milli-seconds since the start time
    long getElapsedTimeMsec() const
    {
        struct timeval end_time;
        gettimeofday(&end_time, NULL);

        long seconds    = (end_time.tv_sec  - m_start_time.tv_sec ); // seconds
        long useconds   = (end_time.tv_usec - m_start_time.tv_usec); // milliseconds
        long milli_time = ((seconds) * 1000 + (useconds/1000.0)   );

        return milli_time;
    }

private:
    struct timeval m_start_time;
};

//------------------------------------------------------------------
// singleton management
//------------------------------------------------------------------
CameraAcqThread * CameraAcqThread::g_singleton = NULL;

/************************************************************************
 * \brief constructor
 ************************************************************************/
CameraAcqThread::CameraAcqThread()
{
    DEB_MEMBER_FUNCT();
    DEB_TRACE() << "Creation of the CameraAcqThread thread...";
    m_force_stop = false;
    m_running_state = RunningState::Exposure;
}

/************************************************************************
 * \brief destructor
 ************************************************************************/
CameraAcqThread::~CameraAcqThread()
{
    DEB_MEMBER_FUNCT();
    DEB_TRACE() << "The CameraAcqThread thread was terminated.";
}

/************************************************************************
 * \brief starts the thread
 ************************************************************************/
void CameraAcqThread::start()
{
    DEB_MEMBER_FUNCT();
    DEB_TRACE() << "Starting the CameraAcqThread thread...";
    CmdThread::start();
    waitStatus(CameraAcqThread::Idle);
}

/************************************************************************
 * \brief inits the thread
 ************************************************************************/
void CameraAcqThread::init()
{
    DEB_MEMBER_FUNCT();
    DEB_TRACE() << "Initing the CameraAcqThread thread...";
    setStatus(CameraAcqThread::Idle);
}

/************************************************************************
 * \brief aborts the thread
 ************************************************************************/
void CameraAcqThread::abort()
{
	DEB_MEMBER_FUNCT();
    CmdThread::abort();
}

/************************************************************************
 * \brief command execution
 * \param cmd command indentifier
 ************************************************************************/
void CameraAcqThread::execCmd(int cmd)
{
    DEB_MEMBER_FUNCT();
    DEB_TRACE() << "Executing a command by the CameraAcqThread thread...";
    int status = getStatus();

    try
    {
        switch (cmd)
        {
            case CameraAcqThread::StartAcq:
                if (status == CameraAcqThread::Idle)
                    execStartAcq();
                break;

            default:
                break;
        }
    }
    catch (...)
    {
    }
}

/************************************************************************
 * \brief execute the stop acquisition command
 ************************************************************************/
void CameraAcqThread::execStopAcq()
{
    DEB_MEMBER_FUNCT();

    if(getStatus() == CameraAcqThread::Running)
    {
    	DEB_TRACE() << "stopping the acquisition...";

        m_force_stop = true;

        // Waiting for thread to finish or to be in error
        waitNotStatus(CameraAcqThread::Running);
    }
}

/************************************************************************
 * \brief execute the StartAcq command
 ************************************************************************/
void CameraAcqThread::execStartAcq()
{
    DEB_MEMBER_FUNCT();
    DEB_TRACE() << "executing StartAcq command...";

    m_force_stop    = false;
    m_running_state = RunningState::Exposure;

    // disable the state update process
    Camera::getInstance()->setUpdateAuthorizeFlag(false);

    // the thread is running a new acquisition (it frees the Camera::startAcq method)
    setStatus(CameraAcqThread::Running);

    try
    {
        // Main acquisition loop
        // m_force_stop can be set to true by the execStopAcq call to abort data acquisition
        // m_force_stop can be set to true also with an error hardware camera status
        // This loop can also stops when all the images were received.
        while(!m_force_stop)
        {
            DEB_TRACE() << "wait for image: " << Camera::getConstInstance()->getNbFramesAcquired();

            // Manage the image acquisition process
            if(!imageAcquisition())
            {
                m_force_stop = true;
            }
            else
            {
                InternalTimer latency_timer;
                latency_timer.init();

                // Manage the reception of one image
                if(!imageReception())
                {
                    m_force_stop = true;
                }
                else
                // Manage the latency wait before the next image
                if(!imageLatency(latency_timer))
                {
                    m_force_stop = true;
                }
            }

            if((Camera::getConstInstance()->allFramesAcquired()) || (m_force_stop))
            {
                break;
            }
        }
    }
    catch (lima::Exception & e)
    {
        setStatus(CameraAcqThread::Error);
        std::string error_text = "exception (" + e.getErrMsg() + ") occurred during the real time acquisition!";
        manageError(error_text);
    }
    catch (...)
    {
        setStatus(CameraAcqThread::Error);
        std::string error_text = "Unknown exception occurred during the real time acquisition!";
        manageError(error_text);
    }

    // change the thread status only if the thread is not in error
    if(getStatus() == CameraAcqThread::Running)
    {
        setStatus(CameraAcqThread::Idle);
    }

    // authorize the state update process
    Camera::getInstance()->setUpdateAuthorizeFlag(true);

    DEB_TRACE() << "StartAcq command ended.";
}

/************************************************************************
 * \fn bool imageAcquisition()
 * \brief Manage the acquisition of one image
 * \param  none
 * \return true if succeed, false in case of error
 ************************************************************************/
bool CameraAcqThread::imageAcquisition()
{
    DEB_MEMBER_FUNCT();

    DEB_TRACE() << "imageAcquisition for image: " << Camera::getConstInstance()->getNbFramesAcquired();

    uint32_t exposure_time_msec = CameraControl::getConstInstance()->getExposureTimeMsec();

    // delay in milli-seconds between two tries to check if the acquisition is finished
    int delay_to_check_acq_end_msec = CameraControl::getConstInstance()->getDelayToCheckAcqEndMsec();

    // delay in milli-seconds between two sends of inquire status commands
    int inquire_acq_status_delay_msec = CameraControl::getConstInstance()->getInquireAcqStatusDelayMsec();

    // there is different timeout, depending on the exposure time value
    // configure the wait timeout in seconds for the acquire command execution (tcp/ip packets reception)
    CameraControl::getInstance()->computeTimeoutForAcquireCommand();

    // starting a new exposure
    m_running_state = RunningState::Exposure;

    // Start a new acquisition by sending a command to the hardware (always asynchronous)
    if(!CameraControl::getInstance()->acquire(false)) 
    {
        setStatus(CameraAcqThread::Error);
        std::string error_text = "Error occurred during real time acquisition!";
        manageError(error_text);
        return false;
    }

    // start a timer used for acquisition status sending 
    InternalTimer timer;
    timer.init();

    // wait for the acquisition end (exposure + readout)
    bool error_occurred = false;

    // m_force_stop can be set to true by the execStopAcq call to abort data acquisition
    // m_force_stop can be set to true also with an error hardware camera status
    bool waiting_an_acquisition_status = false;

    for(;;)
    {
        // Check if the acquisition is finished (command done received ?)
        if(CameraControl::getInstance()->checkEndOfAcquisition(error_occurred))
        {
            if(error_occurred)
            {
                setStatus(CameraAcqThread::Error);
                std::string error_text = "Error occurred during real time acquisition!";
                manageError(error_text);
            }
            break;
        }

        // Stop the acquisition by sending a command to the hardware ?
        if(m_force_stop)
        {
            if(!CameraControl::getInstance()->terminateAcquisition())
            {
                setStatus(CameraAcqThread::Error);
                std::string error_text = "Error occurred during the stop of real time acquisition!";
                manageError(error_text);
                error_occurred = true;
            }
            break;
        }

        // Inquire the acquisition status by sending a command to the hardware
        if(!waiting_an_acquisition_status)
        {
            if(timer.getElapsedTimeMsec() >= inquire_acq_status_delay_msec)
            {
                if(CameraControl::getInstance()->inquireAcquisitionStatus())
                {
                    waiting_an_acquisition_status = true;
                }
            }
        }

        // reception of an acquisition status
        if(waiting_an_acquisition_status)
        {
            NetGenericHeader * packet;

            if(CameraControl::getInstance()->getAcquisitionStatusPacket(packet))
            {
                NetAnswerAcquisitionStatus * acq_status_packet = dynamic_cast<NetAnswerAcquisitionStatus *>(packet);

                if(acq_status_packet->m_exposure_done == 100)
                {
                    m_running_state = RunningState::Readout;
                }

                delete acq_status_packet;
                waiting_an_acquisition_status = false;
                timer.init();
            }
        }

        // wait a few mseconds
        usleep(delay_to_check_acq_end_msec);
    }

    return (!error_occurred);
}

/************************************************************************
 * \fn bool imageReception()
 * \brief Manage the reception of one image
 * \param  none
 * \return true if succeed, false in case of error
 ************************************************************************/
bool CameraAcqThread::imageReception()
{
    DEB_MEMBER_FUNCT();

    DEB_TRACE() << "imageReception for image: " << Camera::getConstInstance()->getNbFramesAcquired();

    // reading the Lima frame size in bytes
	StdBufferCbMgr & buffer_mgr     = Camera::getInstance()->getStdBufferCbMgr();
    lima::FrameDim   frame_dim      = buffer_mgr.getFrameDim();
    void           * image_ptr      = buffer_mgr.getFrameBufferPtr(Camera::getConstInstance()->getNbFramesAcquired());
    bool             result         = true ;
    bool             finished       = false;
    int32_t          packets_nb     = 0; // number of received packets

    int              frame_mem_size = frame_dim.getMemSize  ();
    Size             frame_size     = frame_dim.getSize ();
    int              frame_depth    = frame_dim.getDepth();

    // start the latency
    m_running_state = RunningState::Retrieve;

    // Start a new image reception by sending a command to the hardware
    if(!CameraControl::getInstance()->retrieveImage()) 
    {
        setStatus(CameraAcqThread::Error);
        std::string error_text = "Error occurred during real time acquisition (start of image reception)!";
        manageError(error_text);
        return false;
    }

    for(;;)
    {
        NetGenericHeader * packet = NULL;

        // wait for a new image part
        if(!CameraControl::getInstance()->waitImagePacket(packet))
        {
            // a timeout occurred...
            setStatus(CameraAcqThread::Error);
            std::string error_text = "Error occurred during real time acquisition (during the image reception)!";
            manageError(error_text);
            result = false;
            break;
        }

        // treatment of all the received image packets
        while(packet != NULL)
        {
            NetImage * image = dynamic_cast<NetImage *>(packet);

            // check error
            if(image->hasError())
            {
                delete packet;
                packet = NULL;
                CameraControl::getInstance()->terminateImageRetrieve();

                // an error occurred...
                setStatus(CameraAcqThread::Error);
                std::string error_text = "Error occurred during real time acquisition (during an image part reception)!";
                manageError(error_text);
                result = false;
                break;
            }

            // copy the image part into the Lima image buffer
            if(!image->copy(image_ptr, frame_dim))
            {
                // an error occurred...
                setStatus(CameraAcqThread::Error);
                std::string error_text = "Error occurred during real time acquisition (during an image part copy)!";
                manageError(error_text);
                result = false;
                break;
            }

            // one more image part treated
            packets_nb++;

            // check if this is the last packet
            if((image->m_current_packets_nb + 1) == image->m_total_nb_packets)
            {
                // check if all the packets were received
                if(packets_nb != image->m_total_nb_packets)
                {
                    delete packet;
                    packet = NULL;
                    CameraControl::getInstance()->terminateImageRetrieve();

                    // an error occurred...
                    setStatus(CameraAcqThread::Error);
                    std::string error_text = "Error occurred during real time acquisition (lost image parts during the image reception)!";
                    manageError(error_text);
                    result = false;
                    break;
                }
                else
                {
	    	        // pushing the image buffer through Lima 
		            HwFrameInfoType frame_info;
					frame_info.frame_timestamp = Timestamp::now();
		            frame_info.acq_frame_nb    = Camera::getConstInstance()->getNbFramesAcquired();
                    DEB_TRACE() << "imageReception for image (frame_info.acq_frame_nb) : " << (int)frame_info.acq_frame_nb;
    		        buffer_mgr.newFrameReady(frame_info);

                    // increment the number of acquired frames
                    Camera::getInstance()->incrementNbFramesAcquired();
                    finished = true;
                }
                break;
            }

            delete packet;
            packet = NULL;

            if(!CameraControl::getInstance()->getImagePacket(packet))
            {
                break;
            }
        }

        if((!result)||(finished))
        {
            break;
        }

        // Stop the image retrieve process by sending a command to the hardware ?
        if(m_force_stop)
        {
            if(!CameraControl::getInstance()->terminateImageRetrieve())
            {
                setStatus(CameraAcqThread::Error);
                std::string error_text = "Error occurred during the stop of real time acquisition  (during the image reception)!";
                manageError(error_text);
                result = false;
            }
            break;
        }
    }

    return result;
}

/************************************************************************
 * \fn bool imageLatency()
 * \brief Manage the latency wait before the next image
 * \param  in_start_timer timer which was initialized just after the readout 
 * \return true if succeed, false in case of error
 ************************************************************************/
bool CameraAcqThread::imageLatency(const InternalTimer & in_start_timer)
{
    DEB_MEMBER_FUNCT();

    // get the current latency time
    uint32_t latency_time_msec;
    Camera::getConstInstance()->getLatTime(latency_time_msec);

    // retrieve the image
    m_running_state = RunningState::Latency;

    long elapsed_time_msec = in_start_timer.getElapsedTimeMsec();
    DEB_TRACE() << "elapsed_time_msec: " << (int)(elapsed_time_msec);

    if(elapsed_time_msec < latency_time_msec)
    {
        DEB_TRACE() << "imageLatency: " << (int)(latency_time_msec - elapsed_time_msec);

        // wait a few mseconds
        usleep(latency_time_msec - elapsed_time_msec);
    }

    return true;
}

/************************************************************************
 * \brief Manage an incomming error
 * \param in_error_text text which describes the error
 ************************************************************************/
void CameraAcqThread::manageError(std::string & in_error_text)
{
    Event *my_event = new Event(Hardware, Event::Info, Event::Camera, Event::Default, in_error_text);
    Spectral::Camera::getInstance()->getEventCtrlObj()->reportEvent(my_event);
}

//------------------------------------------------------------------
// singleton management
//------------------------------------------------------------------
/************************************************************************
 * \brief Create the thread
 ************************************************************************/
void CameraAcqThread::create()
{
    // creating the camera thread
    CameraAcqThread::g_singleton = new CameraAcqThread();

    // starting the acquisition thread
    CameraAcqThread::g_singleton->start();
}

/************************************************************************
 * \brief Release the thread
 ************************************************************************/
void CameraAcqThread::release()
{
    if(CameraAcqThread::g_singleton != NULL)
    {
        // stopping the acquisition and aborting the thread
        applyStopAcq(false, true);

        // releasing the thread
        delete CameraAcqThread::g_singleton;
        CameraAcqThread::g_singleton = NULL;
    }
}

/*******************************************************************
 * \brief Starts the data acquisition
 *******************************************************************/
void CameraAcqThread::startAcq()
{
    CameraAcqThread::stopAcq();

    CameraAcqThread::g_singleton->sendCmd(CameraAcqThread::StartAcq);
    CameraAcqThread::g_singleton->waitNotStatus(CameraAcqThread::Idle);
}

/*******************************************************************
 * \brief Stops the data acquisition
 *******************************************************************/
void CameraAcqThread::stopAcq()
{
    if(CameraAcqThread::g_singleton != NULL)
    {
        // stopping the thread and restarting the thread in case of error
        applyStopAcq(true, false);
    }
}

/*******************************************************************
 * \brief Stops the data acquisition and abort or restart the thread 
 *        if it is in error. Can also abort the thread when we exit
 *        the program.
 *******************************************************************/
void CameraAcqThread::applyStopAcq(bool in_restart, bool in_always_abort)
{
    CameraAcqThread::g_singleton->execStopAcq();

    // thread in error
    if(CameraAcqThread::g_singleton->getStatus() == CameraAcqThread::Error)
    {
        // aborting the thread
        CameraAcqThread::g_singleton->abort();

        if(in_restart)
        {
            // releasing the thread
            delete CameraAcqThread::g_singleton;

            CameraAcqThread::create();
        }
    }
    else
    // we are going to exit the program, so we are forcing an abort
    if(in_always_abort)
    {
        // aborting the thread
        CameraAcqThread::g_singleton->abort();
    }
}

/****************************************************************************************************
 * \fn int readStatus() const
 * \brief  get the current status
 * \param  none
 * \return current status
 ****************************************************************************************************/
int CameraAcqThread::readStatus()
{
    return CameraAcqThread::g_singleton->getStatus();
}

/****************************************************************************************************
 * \fn CameraAcqThread::RunningState getRunningState()
 * \brief  get the current running status in detail
 * \param  none
 * \return running status in detail
 ****************************************************************************************************/
CameraAcqThread::RunningState CameraAcqThread::getRunningState()
{
    return CameraAcqThread::g_singleton->m_running_state;
}

//========================================================================================
