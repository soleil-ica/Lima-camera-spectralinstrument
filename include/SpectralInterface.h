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
#ifndef SPECTRALINTERFACE_H
#define SPECTRALINTERFACE_H
#include "SpectralCompatibility.h"
#include "lima/HwInterface.h"
#include "lima/HwEventCtrlObj.h"
#include "SpectralCamera.h"
#include "SpectralDetInfoCtrlObj.h"
#include "SpectralSyncCtrlObj.h"
#include "SpectralBinCtrlObj.h"
#include "SpectralRoiCtrlObj.h"


namespace lima
{
    namespace Spectral
    {
	class Interface;

/*******************************************************************
 * \class Interface
 * \brief Spectral hardware interface
 *******************************************************************/

	class LIBSPECTRAL_API Interface : public HwInterface
	{
	    DEB_CLASS_NAMESPC(DebModCamera, "SpectralInterface", "Spectral");

	public:
	    Interface(Camera& cam);
	    virtual ~Interface();

	    //- From HwInterface
	    virtual void    getCapList(CapList&) const;
	    virtual void    reset(ResetLevel reset_level);
	    virtual void    prepareAcq();
	    virtual void    startAcq();
	    virtual void    stopAcq();
	    virtual void    getStatus(StatusType& status);
	    virtual int     getNbHwAcquiredFrames();
    
		//! get the camera object to access it directly from client
		Camera& getCamera() { return m_cam;}

	private:
	    Camera&          m_cam     ;
	    CapList          m_cap_list;
	    DetInfoCtrlObj   m_det_info;
	    SyncCtrlObj      m_sync    ;
	    BinCtrlObj       m_bin     ;
	    RoiCtrlObj       m_roi     ;
        HwEventCtrlObj * m_event   ;
	    mutable Cond     m_cond    ;
	};

    } // namespace Spectral
} // namespace lima

#endif // SpectralINTERFACE_H
