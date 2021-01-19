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
#ifndef SPECTRALROICTRLOBJ_H
#define SPECTRALROICTRLOBJ_H

#include "SpectralCompatibility.h"
#include "lima/HwRoiCtrlObj.h"
#include "SpectralCamera.h"

namespace lima
{
    namespace Spectral
    {


/*******************************************************************
 * \class RoiCtrlObj
 * \brief Control object providing Spectral Roi interface
 *******************************************************************/

	class LIBSPECTRAL_API RoiCtrlObj : public HwRoiCtrlObj
	{
	    DEB_CLASS_NAMESPC(DebModCamera, "RoiCtrlObj", "Spectral");

	public:
	    RoiCtrlObj(Camera& cam);
	    virtual ~RoiCtrlObj();

	    virtual void setRoi(const Roi& set_roi);
	    virtual void getRoi(Roi& hw_roi);
	    virtual void checkRoi(const Roi& set_roi, Roi& hw_roi);

	private:
	    Camera& m_cam;
	};


    } // namespace Spectral
} // namespace lima

#endif // SpectralROICTRLOBJ_H

