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
 * \file   ProtectedList.hpp
 * \brief  implementation file of a multithreads protected container (template class).
 *         Should not be included, use only ProtectedList.h as include file.
 * \author Cédric Castel - SOLEIL (MEDIANE SYSTEME - IT consultant) 
 * \date   Created on October 23, 2020
 ****************************************************************************************************/
//#define PROTECTED_LIST_ACTIVATE_DEBUG

/****************************************************************************************************
 * \fn template <class Elem> ProtectedList<Elem>::ProtectedList(std::string in_name)
 * \brief  constructor
 * \param  in_name instance name (used for logs)
 * \return none
 ****************************************************************************************************/
template <class Elem>
ProtectedList<Elem>::ProtectedList(std::string in_name)
{
    m_name                     = in_name; // storing the name of the instance (for stats)
    m_delay_before_timeout_sec = 0.0    ;
}

/****************************************************************************************************
 * \fn template <class Elem> ProtectedList<Elem>::~ProtectedList()
 * \brief  destructor
 * \param  none
 * \return none
 ****************************************************************************************************/
template <class Elem>
ProtectedList<Elem>::~ProtectedList()
{
    while(!empty())
    {
        delete take();
    }
}

/****************************************************************************************************
 * \fn template <class Elem> const std::string ProtectedList<Elem>::getName() const
 * \brief  get the list name
 * \param  none
 * \return list name
 ****************************************************************************************************/
template <class Elem>
const std::string & ProtectedList<Elem>::getName() const
{
    return m_name;
}

/****************************************************************************************************
 * \fn template <class Elem> void ProtectedList<Elem>::setDelayBeforeTimeoutSec(double in_delay_before_timeout_sec)
 * \brief  set the timeout delay in seconds
 * \param  in_delay_before_timeout_sec timeout delay in seconds
 * \return none
 ****************************************************************************************************/
template <class Elem>
void ProtectedList<Elem>::setDelayBeforeTimeoutSec(double in_delay_before_timeout_sec)
{
    m_delay_before_timeout_sec = in_delay_before_timeout_sec;
}

/****************************************************************************************************
 * \fn template <class Elem> lima::AutoMutex ProtectedList<Elem>::accessLock() const
 * \brief  creates an autolock mutex for methods access
 * \param  none
 * \return auto mutex
 ****************************************************************************************************/
template <class Elem>
lima::AutoMutex ProtectedList<Elem>::accessLock() const
{
    return lima::AutoMutex(m_access_cond.mutex());
}

/****************************************************************************************************
 * \fn template <class Elem> lima::AutoMutex ProtectedList<Elem>::notEmptyLock() const
 * \brief  creates an autolock mutex for waiting till the container is not empty 
 * \param  none
 * \return auto mutex
 ****************************************************************************************************/
template <class Elem>
lima::AutoMutex ProtectedList<Elem>::notEmptyLock() const
{
    return lima::AutoMutex(m_condition_is_not_empty.mutex());
}

/****************************************************************************************************
 * \fn template <class Elem> std::size_t ProtectedList<Elem>::size() const
 * \brief gets the current number of elements in the container (multi-threads protected).
 * \param  none
 * \return current number of elements in the container
 ****************************************************************************************************/
template <class Elem>
std::size_t ProtectedList<Elem>::size() const
{
    // protecting the multi-threads access
    lima::AutoMutex access_mutex = accessLock(); 

    return m_elements.size();
}

/****************************************************************************************************
 * \fn template <class Elem> bool ProtectedList<Elem>::empty() const
 * \brief tells if the container is empty (multi-threads protected).
 * \param  none
 * \return true if the container is empty, else false
 ****************************************************************************************************/
template <class Elem>
bool ProtectedList<Elem>::empty() const
{
    return (size() == 0);
}

/****************************************************************************************************
 * \fn template <class Elem> void ProtectedList<Elem>::put(Elem * in_element)
 * \brief put an element in the container (multi-threads protected).
 * \param in_element : element to be treated which will be put at the end of the container.
 * \return none
 ****************************************************************************************************/
template <class Elem>
void ProtectedList<Elem>::put(Elem * in_element)
{
    bool was_empty;

    // protecting the multi-threads access
    {
        lima::AutoMutex access_mutex = accessLock(); 

        was_empty = m_elements.empty();

        // putting the element at the end of the container
        m_elements.push(in_element);
    }

    // unblocking threads if no more empty
    if(was_empty)
    {
        // protecting the multi-threads access
        lima::AutoMutex not_empty_mutex = notEmptyLock(); 

#ifdef  PROTECTED_LIST_ACTIVATE_DEBUG
        std::cout << "put: container was empty, need to signal others threads..." << std::endl;
#endif
        m_condition_is_not_empty.broadcast();

#ifdef  PROTECTED_LIST_ACTIVATE_DEBUG
        std::cout << "put: container was empty, others threads were woken up." << std::endl;
#endif
    }
}

/****************************************************************************************************
 * \fn template <class Elem> Elem * in_element ProtectedList<Elem>::take()
 * \brief takes an element from the container to treat it (multi-threads protected).
 * \param  none
 * \return the first element of the container
 ****************************************************************************************************/
template <class Elem>
Elem * ProtectedList<Elem>::take()
{
    Elem * element = NULL;

    // protecting the multi-threads access
    lima::AutoMutex access_mutex = accessLock(); 

    // checking if the container is empty
    if(m_elements.empty())
    {
        std::stringstream temp_stream;
        temp_stream << "ProtectedList::take - cannot take an element of container named " << m_name << " because it is empty!";
        std::cout << temp_stream.str() << std::endl;
    }
    else
    // the container is not empty
    {
        // getting the first element of the container
        element = m_elements.front();
        m_elements.pop();
    }

    return element;
}

/****************************************************************************************************
 * \fn template <class Elem> const Elem * ProtectedList<Elem>::front() const
 * \brief Gives const access to the first element of the container (multi-threads protected).
 * \param  none
 * \return access to the first element of the container
 ****************************************************************************************************/
template <class Elem>
const Elem * ProtectedList<Elem>::front() const
{
    const Elem * element = NULL;

    // protecting the multi-threads access
    lima::AutoMutex access_mutex = accessLock(); 

    // checking if the container is empty
    if(m_elements.empty())
    {
        std::stringstream temp_stream;
        temp_stream << "ProtectedList::front - cannot access an element of container named " << m_name << " because it is empty!";
        std::cout << temp_stream.str() << std::endl;
    }
    else
    {
        // getting access to the first element of the container
        element = m_elements.front();
    }

    return element;
}

/****************************************************************************************************
 * \fn template <class Elem> Elem * ProtectedList<Elem>::front()
 * \brief Gives write access to the first element of the container (multi-threads protected).
 * \param  none
 * \return access to the first element of the container
 ****************************************************************************************************/
template <class Elem>
Elem * ProtectedList<Elem>::front()
{
    Elem * element = NULL;

    // protecting the multi-threads access
    lima::AutoMutex access_mutex = accessLock(); 

    // checking if the container is empty
    if(m_elements.empty())
    {
        std::stringstream temp_stream;
        temp_stream << "ProtectedList::front - cannot access an element of container named " << m_name << " because it is empty!";
        std::cout << temp_stream.str() << std::endl;
    }
    else
    {
        // getting access to the first element of the container
        element = m_elements.front();
    }

    return element;
}

/****************************************************************************************************
 * \fn template <class Elem> bool ProtectedList<Elem>::waiting_while_empty() const
 * \brief waits till the container is no more empty.
 * \param  none
 * \return true if we were waken up, else false if a timeout occured
 ****************************************************************************************************/
template <class Elem>
bool ProtectedList<Elem>::waiting_while_empty() const
{
    // protecting the multi-threads access
    lima::AutoMutex not_empty_mutex = notEmptyLock(); 

    bool result = true;

    if(m_elements.empty())
    {
#ifdef  PROTECTED_LIST_ACTIVATE_DEBUG
        std::cout << "waiting_while_empty: container is empty, need to wait..." << std::endl;
#endif
        result = m_condition_is_not_empty.wait(m_delay_before_timeout_sec);

#ifdef  PROTECTED_LIST_ACTIVATE_DEBUG
        if(!result)
        {
            std::cout << "waiting_while_empty: timeout!" << std::endl;
        }
        else
        {
            std::cout << "waiting_while_empty: was waken up." << std::endl;
        }
 #endif
    }

    return result;
}