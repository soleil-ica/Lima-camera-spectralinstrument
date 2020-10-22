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
 * \file   CameraReceiveDataThread.cpp
 * \brief  implementation file of the receive data thread class.
 * \author Cédric Castel - SOLEIL (MEDIANE SYSTEME - IT consultant) 
 * \date   Created on October 22, 2020
 ****************************************************************************************************/

// PROJECT
#include "CameraReceiveDataThread.h"
#include "SpectralCamera.h"

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
CameraReceiveDataThread * CameraReceiveDataThread::m_singleton = NULL;

/************************************************************************
 * \brief constructor
 ************************************************************************/
CameraReceiveDataThread::CameraReceiveDataThread()
{
    DEB_MEMBER_FUNCT();
    DEB_TRACE() << "Creation of the CameraReceiveDataThread thread...";
    m_force_stop = false;
}

/************************************************************************
 * \brief destructor
 ************************************************************************/
CameraReceiveDataThread::~CameraReceiveDataThread()
{
    DEB_MEMBER_FUNCT();
    DEB_TRACE() << "The CameraReceiveDataThread thread was terminated.";
}

/************************************************************************
 * \brief starts the thread
 ************************************************************************/
void CameraReceiveDataThread::start()
{
    DEB_MEMBER_FUNCT();
    DEB_TRACE() << "Starting the CameraReceiveDataThread thread...";
    CmdThread::start();
    waitStatus(CameraReceiveDataThread::Idle);
}

/************************************************************************
 * \brief inits the thread
 ************************************************************************/
void CameraReceiveDataThread::init()
{
    DEB_MEMBER_FUNCT();
    DEB_TRACE() << "Initing the CameraReceiveDataThread thread...";
    setStatus(CameraReceiveDataThread::Idle);
}

/************************************************************************
 * \brief aborts the thread
 ************************************************************************/
void CameraReceiveDataThread::abort()
{
	DEB_MEMBER_FUNCT();
    CmdThread::abort();
}

/************************************************************************
 * \brief command execution
 * \param cmd command indentifier
 ************************************************************************/
void CameraReceiveDataThread::execCmd(int cmd)
{
    DEB_MEMBER_FUNCT();
    DEB_TRACE() << "Executing a command by the CameraReceiveDataThread thread...";
    int status = getStatus();

    try
    {
        switch (cmd)
        {
            case CameraReceiveDataThread::StartReception:
                if (status == CameraReceiveDataThread::Idle)
                    execStartReception();
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
 * \brief execute the stop reception command
 ************************************************************************/
void CameraReceiveDataThread::execStopReception()
{
    DEB_MEMBER_FUNCT();

    if(getStatus() == CameraReceiveDataThread::Running)
    {
    	DEB_TRACE() << "stopping the reception...";

        m_force_stop = true;

        // Waiting for thread to finish or to be in error
        waitNotStatus(CameraReceiveDataThread::Running);
    }
}

/************************************************************************
 * \brief execute the StartReception command
 ************************************************************************/
void CameraReceiveDataThread::execStartReception()
{
    DEB_MEMBER_FUNCT();
    DEB_TRACE() << "executing StartReception command...";

    m_force_stop = false;

    // the thread is running a new reception (it frees the Camera::startReception method)
    setStatus(CameraReceiveDataThread::Running);

    // Main acquisition loop
    // m_force_stop can be set to true by the execStopAcq call to abort an acquisition
    // m_force_stop can be set to true also with an error hardware camera status
    // the loop can also end if all the frames wre acquired
    while(!m_force_stop)
    {
        lima::Sleep(1); // sleep the thread in seconds
    }

/*
setStatus(CameraReceiveDataThread::Error);
std::string error_text = "Could not stop real time acquisition!";
manageError(error_text);
return;*/

    // change the thread status only if the thread is not in error
    if(getStatus() == CameraReceiveDataThread::Running)
    {
        setStatus(CameraReceiveDataThread::Idle);
    }
}

/************************************************************************
 * \brief Manage an incomming error
 * \param in_error_text text which describes the error
 ************************************************************************/
void CameraReceiveDataThread::manageError(std::string & in_error_text)
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
void CameraReceiveDataThread::create()
{
    // creating the camera thread
    CameraReceiveDataThread::m_singleton = new CameraReceiveDataThread();

    // starting the acquisition thread
    CameraReceiveDataThread::m_singleton->start();
}

/************************************************************************
 * \brief Release the thread
 ************************************************************************/
void CameraReceiveDataThread::release()
{
    if(CameraReceiveDataThread::m_singleton != NULL)
    {
        // stopping the acquisition and aborting the thread
        applyStopReception(false, true);

        // releasing the thread
        delete CameraReceiveDataThread::m_singleton;
        CameraReceiveDataThread::m_singleton = NULL;
    }
}

/*******************************************************************
 * \brief Starts the data reception
 *******************************************************************/
void CameraReceiveDataThread::startReception()
{
    CameraReceiveDataThread::stopReception();

    CameraReceiveDataThread::m_singleton->sendCmd(CameraReceiveDataThread::MaxThreadCmd);
    CameraReceiveDataThread::m_singleton->waitNotStatus(CameraReceiveDataThread::Idle);
}

/*******************************************************************
 * \brief Stops the data reception
 *******************************************************************/
void CameraReceiveDataThread::stopReception()
{
    if(CameraReceiveDataThread::m_singleton != NULL)
    {
        // stopping the thread and restarting the thread in case of error
        applyStopReception(true, false);
    }
}

/*******************************************************************
 * \brief Stops the data reception and abort or restart the thread 
 *        if it is in error. Can also abort the thread when we exit
 *        the program.
 *******************************************************************/
void CameraReceiveDataThread::applyStopReception(bool in_restart, bool in_always_abort)
{
    CameraReceiveDataThread::m_singleton->execStopReception();

    // thread in error
    if(CameraReceiveDataThread::m_singleton->getStatus() == CameraReceiveDataThread::Error)
    {
        // aborting the thread
        CameraReceiveDataThread::m_singleton->abort();

        if(in_restart)
        {
            // releasing the thread
            delete CameraReceiveDataThread::m_singleton;

            CameraReceiveDataThread::create();
        }
    }
    else
    // we are going to exit the program, so we are forcing an abort
    if(in_always_abort)
    {
        // aborting the thread
        CameraReceiveDataThread::m_singleton->abort();
    }
}

//========================================================================================
