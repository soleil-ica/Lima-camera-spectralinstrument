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
 * \file   ProtectedList.h
 * \brief  header file of a multithreads protected container (template class).
 * \author C�dric Castel - SOLEIL (MEDIANE SYSTEME - IT consultant) 
 * \date   Created on October 23, 2020
 ****************************************************************************************************/

#ifndef SPECTRALPROTECTEDLIST_H
#define SPECTRALPROTECTEDLIST_H

// SYSTEM
#include <cstddef>
#include <stdint.h>
#include <queue>
#include <set>
#include <string>
#include <cstdlib>
#include <sstream> // std::stringstream
#include <cstdlib>
#include <cstring>
#include <iostream>

// PROJECT
#include "SpectralInstrumentCompatibility.h"
#include "NetPackets.h"

// LIMA 
#include "lima/ThreadUtils.h"

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
 *  \class ProtectedList
 *  \brief This class is used to manage a FIFO of elements.
 *         It is created empty.
 *         A mutex protects the multi-threads access to this container (take and put methods).
 *         A condition variable can be used to synchronized producers and customers.
 *         The condition is signaled when the list is no more empty.
 */
template< class Elem>
class ProtectedList
{
public:
    // constructor
    ProtectedList(std::string in_name);

    // destructor - we do not allow this class to be inherited, so this method is not virtual.
    ~ProtectedList();

    // get the list name
    const std::string & getName() const;

    // set the timeout delay in seconds
    void setDelayBeforeTimeoutSec(double in_delay_before_timeout_sec);

    // gets the current number of elements in the container (multi-threads protected).
    std::size_t size() const;

    // tells if the container is empty (multi-threads protected).
    bool empty() const;

    // put an element in the container (multi-threads protected).
    void put(Elem * in_element);

    // takes an element from the container to treat it (multi-threads protected).
    Elem * take();

    // Gives write access to the first element of the container (multi-threads protected).
    const Elem * front() const;

    // Gives write access to the first element of the container (multi-threads protected).
    Elem * front();

    // waits till the container is no more empty.
    bool waiting_while_empty() const;

private:
    // creates an autolock mutex for methods access
    lima::AutoMutex accessLock() const;

    // creates an autolock mutex for waiting till the container is not empty 
    lima::AutoMutex notEmptyLock() const;

private:
  /** name of the instance (used for logs).
    */
    std::string m_name;

  /** delay in seconds before a timeout.
    */
    double m_delay_before_timeout_sec;

    //------------------------------------------------------------------
    // protection stuff
    //------------------------------------------------------------------
  /** condition variable used to synchronized producers and customers.
    * mutable keyword is used to allow const methods even if they use this class member.
    */
    mutable lima::Cond m_access_cond;

  /** condition variable used to synchronized producers and customers.
    * The condition is signaled when the list is no more empty.
    */
	mutable lima::Cond m_condition_is_not_empty;

  /** FIFO used to store the elements.
    */
    std::queue<Elem *> m_elements;
};

// include the implementation file of the ProtectedList class to separate interface and implementation
#include "ProtectedList.hpp"

} // namespace Spectral
} // namespace lima

#endif //// SPECTRALPROTECTEDLIST_H
