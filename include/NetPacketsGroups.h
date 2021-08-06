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
 * \file   NetPacketsGroups.h
 * \brief  header file of network packets container class
 *         It is used during the data reception to sort the different data types.
 * \author C�dric Castel - SOLEIL (MEDIANE SYSTEME - IT consultant) 
 * \date   Created on October 23, 2020
 ****************************************************************************************************/

#ifndef SPECTRALNETPACKETSMAP_H
#define SPECTRALNETPACKETSMAP_H

// SYSTEM
#include <cstdlib>
#include <cstring>
#include <string>
#include <stdint.h>
#include <map>

// PROJECT
#include "SpectralInstrumentCompatibility.h"
#include "ProtectedList.h"

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
/***********************************************************************
 * AsyncRequestGroup containers types
 ***********************************************************************/
/*
 *  \typedef NetPacketsGroupId
 *  \brief defines the type of group identifier
 */
typedef uint16_t NetPacketsGroupId;

/*
 *  \typedef NetPacketsMap
 *  \brief type of container which contains a map of NetPackets lists
 *         The list identifier is the m_packet_identifier for 
 *         acknowledge or image packets and the m_data_type for 
 *         data packets.
 */
typedef std::map<NetPacketsGroupId, ProtectedList<NetGenericHeader> *> NetPacketsMap;
    
/*
 *  \class NetPacketsGroups
 *  \brief This class is a network packets container class
 */
class NetPacketsGroups
{
    friend class CameraControl;

public:
    // constructor
    NetPacketsGroups();

    // destructor
    ~NetPacketsGroups();

    // search a group
    ProtectedList<NetGenericHeader> * searchGroup(NetPacketsGroupId in_group_id);

    // set the timeout delay in seconds for all the groups
    void setDelayBeforeTimeoutSec(int in_wait_packet_timeout_sec);

    // set the timeout delay in seconds for a specific group
    void setDelayBeforeTimeoutSec(NetPacketsGroupId in_group_id, int in_wait_packet_timeout_sec);

private:
    // add a new group 
    void createGroup(const std::string & in_name, NetPacketsGroupId in_group_id);

private:
    // container of packets'lists
    NetPacketsMap m_container;
};

} // namespace Spectral
} // namespace lima

#endif // SPECTRALNETPACKETSMAP_H
