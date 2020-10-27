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
 * \file   NetPacketsGroups.cpp
 * \brief  implementation file of network packets container class
 *         It is used during the data reception to sort the different data types.
 * \author Cédric Castel - SOLEIL (MEDIANE SYSTEME - IT consultant) 
 * \date   Created on October 23, 2020
 ****************************************************************************************************/

// PROJECT
#include "NetPacketsGroups.h"
#include "NetPackets.h"

// SYSTEM
#include <netinet/in.h>

// LIMA
#include "lima/Exceptions.h"
#include "lima/Debug.h"

using namespace lima;
using namespace lima::Spectral;

#define NET_PACKETS_GROUPS_DEBUG

//===================================================================================================
// Class NetPacketsGroups
//===================================================================================================
/****************************************************************************************************
 * \fn NetPacketsGroups()
 * \brief  constructor
 * \param  none
 * \return none
 ****************************************************************************************************/
NetPacketsGroups::NetPacketsGroups()
{
    // we should create all the needed lists which are fixed during runtime
    createGroup("acknowledge list", static_cast<NetPacketsGroupId>(NetGenericHeader::g_packet_identifier_for_acknowledge));
    createGroup("image list"      , static_cast<NetPacketsGroupId>(NetGenericHeader::g_packet_identifier_for_image));
    createGroup("get status list" , NetGenericAnswer::g_data_type_get_status           );
    createGroup("get parameters"  , NetGenericAnswer::g_data_type_get_camera_parameters);
    createGroup("get settings"    , NetGenericAnswer::g_data_type_get_settings         );
}

/****************************************************************************************************
 * \fn ~NetPacketsGroups()
 * \brief  destructor
 * \param  none
 * \return none
 ****************************************************************************************************/
NetPacketsGroups::~NetPacketsGroups()
{
    // releasing the container groups
    NetPacketsMap::iterator it;

    for (it = m_container.begin(); it != m_container.end(); ++it) 
    {
        ProtectedList<NetGenericHeader> * group = it->second;

    #ifdef NET_PACKETS_GROUPS_DEBUG
        std::cout << "NetPacketsGroups::~NetPacketsGroups - removing Group " << group->getName() << std::endl;
    #endif
    }
}

/*******************************************************************
 * \fn ProtectedList<NetGenericHeader> * searchGroup(NetPacketsGroupId in_group_id)
 * \brief search a group
 * \param[in] in_group_id identifier of the group
 * \return    the group or NULL if it does not exist.
 *******************************************************************/
ProtectedList<NetGenericHeader> * NetPacketsGroups::searchGroup(NetPacketsGroupId in_group_id)
{
    ProtectedList<NetGenericHeader> * group = NULL;

    // check if the group already exists in the container 
    NetPacketsMap::iterator search = m_container.find(in_group_id);

    // group was already created
    if(search != m_container.end()) 
    {
        group = search->second;
    }

    return group;
}

/*******************************************************************
 * \fn void createGroup(const std::string & in_name, NetPacketsGroupId in_group_id)
 * \brief add a new group 
 * \param[in] in_request_id identifier of the request
 * \param{in] in_name instance name (used for logs)
 * \param[in] in_group_request_id identifier of the group.
 * \return    none
 *******************************************************************/
void NetPacketsGroups::createGroup(const std::string & in_name, NetPacketsGroupId in_group_id)
{
    ProtectedList<NetGenericHeader> * group = NULL;

    // check if the group already exists in the container 
    group = searchGroup(in_group_id);

    if(group != NULL)
    {
    #ifdef NET_PACKETS_GROUPS_DEBUG
        std::cout << "NetPacketsGroups::createGroup - Error: Group " << (int)in_group_id << " already exists!" << std::endl;
    #endif
        return;
    }

    group = new ProtectedList<NetGenericHeader>(in_name);

// new request group
#ifdef NET_PACKETS_GROUPS_DEBUG
    std::cout << "NetPacketsGroups::createGroup - Creating new request group: "
              << in_name << " (" << (int)in_group_id << ")" << std::endl;
#endif
    // inserting the group in the map container.
    std::pair<NetPacketsMap::iterator, bool> insert_result = m_container.insert(std::make_pair(in_group_id, group));

    // this should never happen.
    if(!insert_result.second)
    {
    #ifdef NET_PACKETS_GROUPS_DEBUG
        std::cout << "NetPacketsGroups::createGroup - Problem! Group (" << (int)in_group_id << ") should not be in the requests container!" << std::endl;
    #endif
    }
}



//###########################################################################