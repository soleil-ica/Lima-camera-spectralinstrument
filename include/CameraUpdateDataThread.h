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
 * \file   CameraUpdateDataThread.h
 * \brief  header file of the update data thread class.
 * \author C�dric Castel - SOLEIL (MEDIANE SYSTEME - IT consultant) 
 * \date   Created on October 26, 2020
 ****************************************************************************************************/

#ifndef SPECTRALINSTRUMENTCAMERAUPDATEDATATHREAD_H_
#define SPECTRALINSTRUMENTCAMERAUPDATEDATATHREAD_H_

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
/*
 *  \class CameraUpdateDataThread
 *  \brief This class is used to periodically update some data like status, exposure time, etc...
 */
class CameraUpdateDataThread : public CmdThread
{
    DEB_CLASS_NAMESPC(DebModCamera, "CameraUpdateDataThread", "SpectralInstrument");

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
        StartUpdate = MaxThreadCmd, // command used to start the data update
    };

    // constructor
    CameraUpdateDataThread();

    // destructor
    virtual ~CameraUpdateDataThread();

    // starts the thread
    virtual void start();

    // aborts the thread
    virtual void abort();

    // Create the thread
    static void create();

    // Release the thread
    static void release();

    // Starts the data update
    static void startUpdate();

    // Stops the data update
    static void stopUpdate();

protected:
    // Manage an incomming error
    void manageError(std::string & in_error_text);

    // Stops the data update and abort or restart the thread 
    static void applyStopUpdate(bool in_restart, bool in_always_abort);

protected:
    // inits the thread
    virtual void init();

    // command execution
    virtual void execCmd(int cmd);

private:
    // execute the StartUpdate command
    void execStartUpdate();

    // execute a stop of the update
    void execStopUpdate();

private :
    volatile bool m_force_stop;

    //------------------------------------------------------------------
    // singleton management
    //------------------------------------------------------------------
    static CameraUpdateDataThread * g_singleton;
};

} // namespace SpectralInstrument
} // namespace lima

#endif // SPECTRALCAMERAUPDATEDATATHREAD_H_

/*************************************************************************/