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
 * \file   CameraAcqThread.h
 * \brief  header file of the receive data thread class.
 * \author C�dric Castel - SOLEIL (MEDIANE SYSTEME - IT consultant) 
 * \date   Created on October 28, 2020
 ****************************************************************************************************/

#ifndef SPECTRALINSTRUMENTCAMERAACQTHREAD_H_
#define SPECTRALINSTRUMENTCAMERAACQTHREAD_H_

// PROJECT
#include "SpectralInstrumentCompatibility.h"

// LIMA 
#include "lima/Exceptions.h"
#include "lima/Debug.h"
#include "lima/Constants.h"
#include "lima/ThreadUtils.h"
#include "lima/Timestamp.h"

/*************************************************************************/
/*
 *  \namespace lima
 */
namespace lima
{
/*
 *  \namespace SpectralInstrument
 */
namespace SpectralInstrument 
{
    // predefined the internal timer used in the thread
    class InternalTimer;
    
/*
 *  \class CameraAcqThread
 *  \brief This class is used to manage an acquisition
 */
class CameraAcqThread : public CmdThread
{
    DEB_CLASS_NAMESPC(DebModCamera, "CameraAcqThread", "SpectralInstrument");

public:
	// Status
    enum
	{ 
		Idle    = MaxThreadStatus, // ready to manage data acquisition
        Running                  , // data acquisition is running 
        Error                    , // unexpected error
	};

    // status values
    typedef enum RunningState
    {
        Exposure   , // running an exposure
        Readout    , // running a readout
        Retrieve   , // retrieve the image
        Latency    , // running a latency

    } RunningState;

    // Cmd
    enum
    { 
        StartAcq = MaxThreadCmd, // command used to start the data acquisition
    };

    // constructor
    CameraAcqThread();

    // destructor
    virtual ~CameraAcqThread();

    // starts the thread
    virtual void start();

    // aborts the thread
    virtual void abort();

    // Create the thread
    static void create();

    // Release the thread
    static void release();

    // Starts the data acquisition
    static void startAcq();

    // Stops the data acquisition
    static void stopAcq();

    // get the current status
    static int readStatus();

    // get the current running status in detail
    static CameraAcqThread::RunningState getRunningState();

protected:
    // Manage an incomming error
    void manageError(std::string & in_error_text);

    // Stops the data acquisition and abort or restart the thread 
    static void applyStopAcq(bool in_restart, bool in_always_abort);

    // inits the thread
    virtual void init();

    // command execution
    virtual void execCmd(int cmd);

private:
    // execute the StartAcq command
    void execStartAcq();

    // execute a stop of the acquisition
    void execStopAcq();

    // Manage the acquisition of one image
    bool imageAcquisition();

    // Manage the reception of one image
    bool imageReception();

    // Manage the latency wait before the next image
    bool imageLatency(const InternalTimer & in_start_timer);

private :
    // allow to force a stop of the thread
    volatile bool m_force_stop;

    // running state in detail
    volatile RunningState m_running_state;

    //------------------------------------------------------------------
    // singleton management
    //------------------------------------------------------------------
    static CameraAcqThread * g_singleton;
};

} // namespace SpectralInstrument
} // namespace lima

#endif // SPECTRALCAMERAACQTHREAD_H_

/*************************************************************************/