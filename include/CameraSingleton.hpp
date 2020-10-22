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
 * \file   CameraSingleton.hpp
 * \brief  implementation file of CameraSingleton template class
 *         Should not be included, use only CameraSingleton.h as include file.
 * \author Cédric Castel - SOLEIL (MEDIANE SYSTEME - IT consultant) 
 * \date   Created on October 19, 2020
 ****************************************************************************************************/

/****************************************************************************************************
 * CLASS STATIC DATA
 ****************************************************************************************************/
/* used to store the singleton
 */
template <class Elem>
Elem * CameraSingleton<Elem>::g_singleton = NULL;

/****************************************************************************************************
 * CLASS METHODS
 ****************************************************************************************************/
/****************************************************************************************************
 * \fn template <class Elem> CameraSingleton<Elem>::CameraSingleton
 * \brief  constructor
 * \param  none
 * \return none
 ****************************************************************************************************/
template <class Elem>
CameraSingleton<Elem>::CameraSingleton()
{
}

/****************************************************************************************************
 * \fn template <class Elem> CameraSingleton<Elem>::~CameraSingleton()
 * \brief  destructor
 * \param  none
 * \return none
 ****************************************************************************************************/
template <class Elem>
CameraSingleton<Elem>::~CameraSingleton()
{
}

/****************************************************************************************************
 * \fn template <class Elem> void CameraSingleton<Elem>::init(Elem * in_new)
 * \brief  init the singleton
 * \param  in_new new singleton instance
 * \return none
 ****************************************************************************************************/
template <class Elem>
void CameraSingleton<Elem>::init(Elem * in_new)
{
    if(g_singleton != in_new)
    {
        if(g_singleton != NULL)
            release();
        
        g_singleton = in_new;
    }
}

/****************************************************************************************************
 * \fn template <class Elem> void CameraSingleton<Elem>::specificRelease()
 * \brief  release the singleton (execute a specific code of a derived class)
 * \param  none
 * \return none
 ****************************************************************************************************/
template <class Elem>
void CameraSingleton<Elem>::specificRelease()
{
}

/****************************************************************************************************
 * \fn template <class Elem> void CameraSingleton<Elem>::release
 * \brief  release the singleton
 * \param  none
 * \return none
 ****************************************************************************************************/
template <class Elem>
void CameraSingleton<Elem>::release()
{
    if(CameraSingleton::g_singleton != NULL)
    {
        CameraSingleton::g_singleton->specificRelease();
        delete CameraSingleton::g_singleton;
        CameraSingleton::g_singleton = NULL;
    }
}

/****************************************************************************************************
 * \fn template <class Elem> Elem * CameraSingleton<Elem>::getInstance
 * \brief  access to the singleton
 * \param  none
 * \return singleton
 ****************************************************************************************************/
template <class Elem>
Elem * CameraSingleton<Elem>::getInstance()
{
    return CameraSingleton::g_singleton;
}

/****************************************************************************************************
 * \fn template <class Elem> const Elem * CameraSingleton<Elem>::getConstInstance
 * \brief  access to the singleton (const version)
 * \param  none
 * \return singleton (const version)
 ****************************************************************************************************/
template <class Elem>
const Elem * CameraSingleton<Elem>::getConstInstance()
{
    return CameraSingleton::g_singleton;
}
