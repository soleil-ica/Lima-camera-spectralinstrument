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
 * \file   NetPacket.cpp
 * \brief  implementation file of network packets classes.
 * \author C�dric Castel - SOLEIL (MEDIANE SYSTEME - IT consultant) 
 * \date   Created on October 20, 2020
 ****************************************************************************************************/

// PROJECT
#include "NetPackets.h"

// SYSTEM
#include <netinet/in.h>

// LIMA
#include "lima/Exceptions.h"
#include "lima/Debug.h"

using namespace lima;
using namespace lima::SpectralInstrument;

//===================================================================================================
static uint64_t htonll(uint64_t in_value);
static double   htond (double   in_value);
static uint64_t ntohll(uint64_t in_value);
static double   ntohd (double   in_value);

//===================================================================================================
#define INT16_TO_NETWORK(s)  (int16_t)(htons((uint16_t)(s)))
#define INT16_TO_HOST(s)     (int16_t)(ntohs((uint16_t)(s)))
   
#define UINT16_TO_NETWORK(s) htons(s)
#define UINT16_TO_HOST(s)    ntohs(s)

#define INT32_TO_NETWORK(s)  (int32_t)(htonl((uint32_t)(s)))
#define INT32_TO_HOST(s)     (int32_t)(ntohl((uint32_t)(s)))

#define UINT32_TO_NETWORK(s) htonl(s)
#define UINT32_TO_HOST(s)    ntohl(s)

#define UINT64_TO_NETWORK(s) htonll(s)
#define UINT64_TO_HOST(s)    ntohll(s)

#define DOUBLE_TO_NETWORK(s) htond(s)
#define DOUBLE_TO_HOST(s)    ntohd(s)

//===================================================================================================
/****************************************************************************************************
 * \fn uint64_t htonll(uint64_t in_value)
 * \brief  conversion of an unsigned 64 bits data (from host order to network order)
 * \param  unsigned 64 bits data source (in host order)
 * \return unsigned 64 bits data in network order
 ****************************************************************************************************/
uint64_t htonll(uint64_t in_value)
{
    // The answer is 42
    static const int num = 42;

    // Check the endianness
    if (*(reinterpret_cast<const char*>(&num)) == num)
    {
        const uint32_t high_part = htonl(static_cast<uint32_t>(in_value >> 32));
        const uint32_t low_part  = htonl(static_cast<uint32_t>(in_value & 0xFFFFFFFFLL));
        in_value = (static_cast<uint64_t>(low_part) << 32) | static_cast<uint64_t>(high_part);
    }

    return in_value;
}

/****************************************************************************************************
 * \fn uint64_t ntohll(uint64_t in_value)
 * \brief  conversion of an unsigned 64 bits data (from network order to host order)
 * \param  unsigned 64 bits data source (in network order)
 * \return unsigned 64 bits data in host order
 ****************************************************************************************************/
uint64_t ntohll(uint64_t in_value)
{
    // The answer is 42
    static const int num = 42;

    // Check the endianness
    if (*(reinterpret_cast<const char*>(&num)) == num)
    {
        const uint32_t high_part = ntohl(static_cast<uint32_t>(in_value >> 32));
        const uint32_t low_part  = ntohl(static_cast<uint32_t>(in_value & 0xFFFFFFFFLL));
        in_value = (static_cast<uint64_t>(low_part) << 32) | static_cast<uint64_t>(high_part);
    }

    return in_value;
}

/****************************************************************************************************
 * \fn double htond(double in_value)
 * \brief  conversion of a double data (from host order to network order)
 * \param  in_value double data source (in host order)
 * \return double data in network order
 ****************************************************************************************************/
double htond(double in_value)
{
    uint64_t temp = htonll(*(reinterpret_cast<uint64_t *>(&in_value)));
    return *(reinterpret_cast<double *>(&temp));
}

/****************************************************************************************************
 * \fn double ntohd(double in_value)
 * \brief  conversion of a double data (from network order to host order)
 * \param  in_value double data source (in network order)
 * \return double data in host order
 ****************************************************************************************************/
double ntohd(double in_value)
{
    uint64_t temp = ntohll(*(reinterpret_cast<uint64_t *>(&in_value)));
    return *(reinterpret_cast<double *>(&temp));
}

//===================================================================================================
// packets management was splitted into several files
//===================================================================================================
#include "NetGenericHeader.hpp"

// commands
#include "NetCommandHeader.hpp"

#include "NetCommandGetStatus.hpp"
#include "NetCommandGetCameraParameters.hpp"
#include "NetCommandGetSettings.hpp"

#include "NetCommandSetExposureTime.hpp"
#include "NetCommandSetFormatParameters.hpp"
#include "NetCommandSetAcquisitionMode.hpp"
#include "NetCommandSetAcquisitionType.hpp"

#include "NetCommandAcquire.hpp"
#include "NetCommandTerminateAcquisition.hpp"
#include "NetCommandTerminateImageRetrieve.hpp"
#include "NetCommandRetrieveImage.hpp"
#include "NetCommandInquireAcquisitionStatus.hpp"
#include "NetCommandConfigurePackets.hpp"
#include "NetCommandSetCoolingValue.hpp"
#include "NetCommandSetSingleParameter.hpp"

// acknowledge
#include "NetAcknowledge.hpp"

// image
#include "NetImageHeader.hpp"
#include "NetImage.hpp"

// answers of get commands
#include "NetGenericAnswer.hpp"
#include "NetAnswerGenericString.hpp"
#include "NetAnswerSetCoolingValue.hpp"
#include "NetAnswerSetSingleParameter.hpp"

#include "NetAnswerGetStatus.hpp"
#include "NetAnswerGetCameraParameters.hpp"
#include "NetAnswerGetSettings.hpp"

// command done answers
#include "NetAnswerCommandDone.hpp"

#include "NetAnswerSetAcquisitionMode.hpp"
#include "NetAnswerSetExposureTime.hpp"
#include "NetAnswerSetFormatParameters.hpp"
#include "NetAnswerSetAcquisitionType.hpp"
#include "NetAnswerAcquire.hpp"
#include "NetAnswerTerminateAcquisition.hpp"
#include "NetAnswerTerminateImageRetrieve.hpp"
#include "NetAnswerAcquisitionStatus.hpp"
#include "NetAnswerConfigurePackets.hpp"

//###########################################################################