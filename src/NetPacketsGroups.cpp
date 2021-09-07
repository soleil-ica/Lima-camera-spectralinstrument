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
 * \author C�dric Castel - SOLEIL (MEDIANE SYSTEME - IT consultant) 
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
using namespace lima::SpectralInstrument;

//#define NET_PACKETS_GROUPS_DEBUG

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
    createGroup("image list"      , static_cast<NetPacketsGroupId>(NetGenericHeader::g_packet_identifier_for_image      ));

    // get answers groups
    createGroup("get status list"        , NetGenericAnswer::g_data_type_get_status           );
    createGroup("get parameters list"    , NetGenericAnswer::g_data_type_get_camera_parameters);
    createGroup("get settings list"      , NetGenericAnswer::g_data_type_get_settings         );
    createGroup("acquisition status list", NetGenericAnswer::g_data_type_acquisition_status   );
    

    // command done groups
    createGroup("set acquisition mode list"    , NetCommandHeader::g_function_number_set_acquisition_mode    );
    createGroup("set exposure time list"       , NetCommandHeader::g_function_number_set_exposure_time       );
    createGroup("set format parameters list"   , NetCommandHeader::g_function_number_set_format_parameters   );
    createGroup("set acquisition type list"    , NetCommandHeader::g_function_number_set_acquisition_type    );
    createGroup("acquire list"                 , NetCommandHeader::g_function_number_acquire                 );
    createGroup("terminate acquisition list"   , NetCommandHeader::g_function_number_terminate_acquisition   );
    createGroup("terminate image retrieve list", NetCommandHeader::g_function_number_terminate_image_retrieve);
    createGroup("configure packets list"       , NetCommandHeader::g_function_number_configure_packets       );
    createGroup("set ON/OFF cooling value"     , NetCommandHeader::g_function_number_set_cooling_value );
    createGroup("set single parameter"         , NetCommandHeader::g_function_number_set_single_parameter);
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

/****************************************************************************************************
 * \fn void NetPacketsGroups::setDelayBeforeTimeoutSec(double in_delay_before_timeout_sec)
 * \brief  set the timeout delay in seconds for all the groups
 * \param  in_wait_packet_timeout_sec timeout delay in seconds
 * \return none
 ****************************************************************************************************/
void NetPacketsGroups::setDelayBeforeTimeoutSec(int in_wait_packet_timeout_sec)
{
    NetPacketsMap::iterator it;

    for (it = m_container.begin(); it != m_container.end(); ++it) 
    {
        ProtectedList<NetGenericHeader> * group = it->second;

        group->setDelayBeforeTimeoutSec(static_cast<double>(in_wait_packet_timeout_sec));

    #ifdef NET_PACKETS_GROUPS_DEBUG
        std::cout << "NetPacketsGroups::setDelayBeforeTimeoutSec: " << in_wait_packet_timeout_sec 
                  << " for group: " << group->getName() << std::endl;
    #endif
    }
}

/****************************************************************************************************
 * \fn void NetPacketsGroups::setDelayBeforeTimeoutSec(NetPacketsGroupId in_group_id, int in_wait_packet_timeout_sec)
 * \brief  set the timeout delay in seconds for a specific group
 * \param[in] in_group_id identifier of the group
 * \param[in] in_wait_packet_timeout_sec timeout delay in seconds
 * \return none
 ****************************************************************************************************/
void NetPacketsGroups::setDelayBeforeTimeoutSec(NetPacketsGroupId in_group_id, int in_wait_packet_timeout_sec)
{
    ProtectedList<NetGenericHeader> * group = NULL;

    // check if the group exists in the container 
    group = searchGroup(in_group_id);

    if(group == NULL)
    {
    #ifdef NET_PACKETS_GROUPS_DEBUG
        std::cout << "NetPacketsGroups::setDelayBeforeTimeoutSec - Error: Group " << (int)in_group_id << " does not exists!" << std::endl;
    #endif
    }
    else
    {
        group->setDelayBeforeTimeoutSec(static_cast<double>(in_wait_packet_timeout_sec));

    #ifdef NET_PACKETS_GROUPS_DEBUG
        std::cout << "NetPacketsGroups::setDelayBeforeTimeoutSec: " << in_wait_packet_timeout_sec 
                  << " for group: " << group->getName() << std::endl;
    #endif
    }
}

//###########################################################################