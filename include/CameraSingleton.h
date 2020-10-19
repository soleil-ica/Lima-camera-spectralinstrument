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
 * \file   CameraSingleton.h
 * \brief  header file of CameraSingleton template class.
 * \author C�dric Castel - SOLEIL (MEDIANE SYSTEME - IT consultant) 
 * \date   Created on October 19, 2020
 ****************************************************************************************************/

#ifndef SPECTRALSINGLETON_H_
#define SPECTRALSINGLETON_H_

// SYSTEM
#include <cstddef>
#include <stdint.h>
#include <cstdlib>

// LIMA
#include "lima/AutoObj.h"

/*
 *  \namespace lima
 */
namespace lima
{
/*
 *  \namespace SpectralDetector_ns
 */
namespace SpectralDetector_ns
{
/*
 *  \class CameraSingleton
 *  \brief This class is used to manage a singleton.
 */
    template< class Elem>
    class CameraSingleton
    {
    public:
        // release the singleton
        static void release();

        // return the singleton
        static lima::AutoPtr<Elem> getInstance();

        // return the singleton (const version)
        static lima::AutoPtr<const Elem> getConstInstance();

    protected:
        // constructor
        explicit CameraSingleton();

        // destructor (needs to be virtual)
        virtual ~CameraSingleton();

    private:
        // release the singleton (execute a specific code of a derived class)
        virtual void specificRelease();

    protected:
        // used to store the singleton
        static lima::AutoPtr<Elem> g_singleton;
    };

    // include the implementation file of the CameraSingleton class to separate interface and implementation
    #include "CameraSingleton.hpp"

} // namespace SpectralDetector_ns
} // namespace lima

#endif //// SPECTRALSINGLETON_H_
