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
 * \file   CameraUpdateDataThread.cpp
 * \brief  implementation file of the receive data thread class.
 * \author Cédric Castel - SOLEIL (MEDIANE SYSTEME - IT consultant) 
 * \date   Created on October 26, 2020
 ****************************************************************************************************/

// PROJECT
#include "CameraUpdateDataThread.h"
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
// singleton management
//------------------------------------------------------------------
CameraUpdateDataThread * CameraUpdateDataThread::g_singleton = NULL;

/************************************************************************
 * \brief constructor
 ************************************************************************/
CameraUpdateDataThread::CameraUpdateDataThread()
{
    DEB_MEMBER_FUNCT();
    DEB_TRACE() << "Creation of the CameraUpdateDataThread thread...";
    m_force_stop = false;
}

/************************************************************************
 * \brief destructor
 ************************************************************************/
CameraUpdateDataThread::~CameraUpdateDataThread()
{
    DEB_MEMBER_FUNCT();
    DEB_TRACE() << "The CameraUpdateDataThread thread was terminated.";
}

/************************************************************************
 * \brief starts the thread
 ************************************************************************/
void CameraUpdateDataThread::start()
{
    DEB_MEMBER_FUNCT();
    DEB_TRACE() << "Starting the CameraUpdateDataThread thread...";
    CmdThread::start();
    waitStatus(CameraUpdateDataThread::Idle);
}

/************************************************************************
 * \brief inits the thread
 ************************************************************************/
void CameraUpdateDataThread::init()
{
    DEB_MEMBER_FUNCT();
    DEB_TRACE() << "Initing the CameraUpdateDataThread thread...";
    setStatus(CameraUpdateDataThread::Idle);
}

/************************************************************************
 * \brief aborts the thread
 ************************************************************************/
void CameraUpdateDataThread::abort()
{
	DEB_MEMBER_FUNCT();
    CmdThread::abort();
}

/************************************************************************
 * \brief command execution
 * \param cmd command indentifier
 ************************************************************************/
void CameraUpdateDataThread::execCmd(int cmd)
{
    DEB_MEMBER_FUNCT();
    DEB_TRACE() << "Executing a command by the CameraUpdateDataThread thread...";
    int status = getStatus();

    try
    {
        switch (cmd)
        {
            case CameraUpdateDataThread::StartUpdate:
                if (status == CameraUpdateDataThread::Idle)
                    execStartUpdate();
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
 * \brief execute the stop update command
 ************************************************************************/
void CameraUpdateDataThread::execStopUpdate()
{
    DEB_MEMBER_FUNCT();

    if(getStatus() == CameraUpdateDataThread::Running)
    {
    	DEB_TRACE() << "stopping the update...";

        m_force_stop = true;

        // Waiting for thread to finish or to be in error
        waitNotStatus(CameraUpdateDataThread::Running);
    }
}

/************************************************************************
 * \brief execute the StartUpdate command
 ************************************************************************/
void CameraUpdateDataThread::execStartUpdate()
{
    DEB_MEMBER_FUNCT();
    DEB_TRACE() << "executing StartUpdate command...";

    m_force_stop = false;

    // get the data update delay in msec
    int data_update_delay_msec = Camera::getConstInstance()->getDataUpdateDelayMsec();

    // the thread is running a new update (it frees the Camera::startUpdate method)
    setStatus(CameraUpdateDataThread::Running);

    // Main data update loop
    // m_force_stop can be set to true by the execStopUpdate call to abort data update
    // m_force_stop can be set to true also with an error hardware camera status
    while(!m_force_stop)
    {
        Camera::getInstance()->updateData();

        // wait a few mseconds
        usleep(data_update_delay_msec * 1000);
    }

/*
setStatus(CameraUpdateDataThread::Error);
std::string error_text = "Could not stop real time acquisition!";
manageError(error_text);
return;*/

    // change the thread status only if the thread is not in error
    if(getStatus() == CameraUpdateDataThread::Running)
    {
        setStatus(CameraUpdateDataThread::Idle);
    }
}

/************************************************************************
 * \brief Manage an incomming error
 * \param in_error_text text which describes the error
 ************************************************************************/
void CameraUpdateDataThread::manageError(std::string & in_error_text)
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
void CameraUpdateDataThread::create()
{
    // creating the camera thread
    CameraUpdateDataThread::g_singleton = new CameraUpdateDataThread();

    // starting the thread
    CameraUpdateDataThread::g_singleton->start();
}

/************************************************************************
 * \brief Release the thread
 ************************************************************************/
void CameraUpdateDataThread::release()
{
    if(CameraUpdateDataThread::g_singleton != NULL)
    {
        // stopping and aborting the thread
        applyStopUpdate(false, true);

        // releasing the thread
        delete CameraUpdateDataThread::g_singleton;
        CameraUpdateDataThread::g_singleton = NULL;
    }
}

/*******************************************************************
 * \brief Starts the data update
 *******************************************************************/
void CameraUpdateDataThread::startUpdate()
{
    CameraUpdateDataThread::stopUpdate();

    CameraUpdateDataThread::g_singleton->sendCmd(CameraUpdateDataThread::StartUpdate);
    CameraUpdateDataThread::g_singleton->waitNotStatus(CameraUpdateDataThread::Idle);
}

/*******************************************************************
 * \brief Stops the data update
 *******************************************************************/
void CameraUpdateDataThread::stopUpdate()
{
    if(CameraUpdateDataThread::g_singleton != NULL)
    {
        // stopping the thread and restarting the thread in case of error
        applyStopUpdate(true, false);
    }
}

/*******************************************************************
 * \brief Stops the data update and abort or restart the thread 
 *        if it is in error. Can also abort the thread when we exit
 *        the program.
 *******************************************************************/
void CameraUpdateDataThread::applyStopUpdate(bool in_restart, bool in_always_abort)
{
    CameraUpdateDataThread::g_singleton->execStopUpdate();

    // thread in error
    if(CameraUpdateDataThread::g_singleton->getStatus() == CameraUpdateDataThread::Error)
    {
        // aborting the thread
        CameraUpdateDataThread::g_singleton->abort();

        if(in_restart)
        {
            // releasing the thread
            delete CameraUpdateDataThread::g_singleton;

            CameraUpdateDataThread::create();
        }
    }
    else
    // we are going to exit the program, so we are forcing an abort
    if(in_always_abort)
    {
        // aborting the thread
        CameraUpdateDataThread::g_singleton->abort();
    }
}

//========================================================================================
