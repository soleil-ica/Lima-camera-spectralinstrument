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
#ifndef SPECTRALINSTRUMENTSYNCCTRLOBJ_H
#define SPECTRALINSTRUMENTSYNCCTRLOBJ_H

#include "lima/HwInterface.h"
#include "SpectralInstrumentCamera.h"

namespace lima
{
    namespace SpectralInstrument
    {


/*******************************************************************
 * \class SyncCtrlObj
 * \brief Control object providing SpectralInstrument synchronization interface
 *******************************************************************/

	class LIBSPECTRAL_API SyncCtrlObj : public HwSyncCtrlObj
	{
	    DEB_CLASS_NAMESPC(DebModCamera, "SyncCtrlObj", "SpectralInstrument");

	public:
	    SyncCtrlObj(Camera& cam);
	    virtual ~SyncCtrlObj();
    
	    virtual bool checkTrigMode(TrigMode trig_mode);
	    virtual void setTrigMode(TrigMode  trig_mode);
	    virtual void getTrigMode(TrigMode& trig_mode);

	    virtual void setExpTime(double  exp_time);
	    virtual void getExpTime(double& exp_time);

	    virtual void setLatTime(double  lat_time);
	    virtual void getLatTime(double& lat_time);

	    virtual void setNbHwFrames(int  nb_frames);
	    virtual void getNbHwFrames(int& nb_frames);

	    virtual void getValidRanges(ValidRangesType& valid_ranges);

		virtual void setCooling(bool  cooling);
	    virtual void getCooling(bool& cooling);

		virtual void getCCDTemperatureFromCamera(float& in_out_value);

		virtual void getReadoutSpeedFromCamera(ushort& in_out_value);
		virtual void setReadoutSpeed( ushort readout_speed);

	private:
	    Camera& m_cam;
	};

    } // namespace SpectralInstrument
} // namespace lima

#endif // SPECTRALINSTRUMENTSYNCCTRLOBJ_H
