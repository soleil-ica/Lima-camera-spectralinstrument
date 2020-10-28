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

    m_force_stop = false;

    // get the data acquisition delay in msec
    int data_update_delay_msec = Camera::getConstInstance()->getDataUpdateDelayMsec();

    // the thread is running a new acquisition (it frees the Camera::startAcq method)
    setStatus(CameraAcqThread::Running);

    // Main acquisition loop
    // m_force_stop can be set to true by the execStopAcq call to abort data acquisition
    // m_force_stop can be set to true also with an error hardware camera status
    // This loop can also stops when all the images were received.
    while(!m_force_stop)
    {
        Camera::getInstance()->updateData();

        // wait a few mseconds
        usleep(data_update_delay_msec * 1000);
    }

/*
setStatus(CameraAcqThread::Error);
std::string error_text = "Could not stop real time acquisition!";
manageError(error_text);
return;*/

    // change the thread status only if the thread is not in error
    if(getStatus() == CameraAcqThread::Running)
    {
        setStatus(CameraAcqThread::Idle);
    }
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
 * \fn CameraAcqThread * getInstance()
 * \brief  access to the singleton
 * \param  none
 * \return singleton
 ****************************************************************************************************/
CameraAcqThread * CameraAcqThread::getInstance()
{
    return CameraAcqThread::g_singleton;
}

/****************************************************************************************************
 * \fn const CameraAcqThread * getConstInstance()
 * \brief  access to the singleton (const version)
 * \param  none
 * \return singleton (const version)
 ****************************************************************************************************/
const CameraAcqThread * CameraAcqThread::getConstInstance()
{
    return CameraAcqThread::g_singleton;
}

//========================================================================================
