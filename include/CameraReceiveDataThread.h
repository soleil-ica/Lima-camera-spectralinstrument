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
 * \file   CameraReceiveDataThread.h
 * \brief  header file of the receive data thread class.
 * \author Cédric Castel - SOLEIL (MEDIANE SYSTEME - IT consultant) 
 * \date   Created on October 22, 2020
 ****************************************************************************************************/

#ifndef SPECTRALCAMERARECEIVEDATATHREAD_H_
#define SPECTRALCAMERARECEIVEDATATHREAD_H_

// PROJECT
#include "SpectralCompatibility.h"

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
 *  \namespace Spectral
 */
namespace Spectral 
{
/*
 *  \class CameraReceiveDataThread
 *  \brief This class is used to receive the tcp/ip data packets from the detector software
 */
class CameraReceiveDataThread : public CmdThread
{
    DEB_CLASS_NAMESPC(DebModCamera, "CameraReceiveDataThread", "Spectral");

public:
	// Status
    enum
	{ 
		Idle    = MaxThreadStatus, // ready to manage data reception
        Running                  , // data reception is running 
        Error                    , // unexpected error
	};

    // Cmd
    enum
    { 
        StartReception = MaxThreadCmd, // command used to start the data reception
    };

    // constructor
    CameraReceiveDataThread();

    // destructor
    virtual ~CameraReceiveDataThread();

    // starts the thread
    virtual void start();

    // aborts the thread
    virtual void abort();

    // Create the thread
    static void create();

    // Release the thread
    static void release();

    // Starts the data reception
    static void startReception();

    // Stops the data reception
    static void stopReception();

protected:
    // Manage an incomming error
    void manageError(std::string & in_error_text);

    // Stops the data reception and abort or restart the thread 
    static void applyStopReception(bool in_restart, bool in_always_abort);

protected:
    // inits the thread
    virtual void init();

    // command execution
    virtual void execCmd(int cmd);

private:
    // execute the StartReception command
    void execStartReception();

    // execute a stop of the reception
    void execStopReception();

private :
    volatile bool m_force_stop;

    //------------------------------------------------------------------
    // singleton management
    //------------------------------------------------------------------
    static CameraReceiveDataThread * g_singleton;
};

} // namespace Spectral
} // namespace lima

#endif // SPECTRALCAMERARECEIVEDATATHREAD_H_

/*************************************************************************/