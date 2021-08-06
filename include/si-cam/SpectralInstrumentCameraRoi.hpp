///###########################################################################
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
//############################################################################

//-----------------------------------------------------------------------------
/// checkRoi
//-----------------------------------------------------------------------------
void Camera::checkRoi(const Roi & set_roi, ///< [in]  Roi values to set
                            Roi & hw_roi ) ///< [out] Updated Roi values
{
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(set_roi);
    hw_roi = set_roi;
    DEB_RETURN() << DEB_VAR1(hw_roi);
}

//-----------------------------------------------------------------------------
/// Set the new roi
// The ROI given by LIMA has a size which depends on the binning.
// SDK Sub array are binning independants.
//-----------------------------------------------------------------------------
void Camera::setRoi(const Roi & set_roi) ///< [in] New Roi values
{
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(set_roi);

    DEB_TRACE() << "setRoi() - new values : " 
                << set_roi.getTopLeft().x           << ", " 
                << set_roi.getTopLeft().y           << ", " 
                << set_roi.getSize   ().getWidth () << ", " 
                << set_roi.getSize   ().getHeight();

    Point set_roi_topleft(set_roi.getTopLeft().x      , set_roi.getTopLeft().y       );
    Size  set_roi_size   (set_roi.getSize().getWidth(), set_roi.getSize().getHeight());

    // correction of a 0x0 ROI sent by the generic part
    if ((set_roi_size.getWidth() == 0) && (set_roi_size.getHeight() == 0))
    {
	    DEB_TRACE() << "Correcting 0x0 roi...";
        std::size_t width_max  = CameraControl::getConstInstance()->getWidthMax       ();
        std::size_t height_max = CameraControl::getConstInstance()->getHeightMax      ();
        std::size_t binning_x  = CameraControl::getConstInstance()->getSerialBinning  ();
        std::size_t binning_y  = CameraControl::getConstInstance()->getParallelBinning();

        set_roi_size = Size(width_max / binning_x, height_max / binning_y);
    }

    Roi new_roi(set_roi_topleft, set_roi_size);

	DEB_TRACE() << "setRoi(): " << set_roi_topleft.x        << ", " 
                                << set_roi_topleft.y        << ", " 
                                << set_roi_size.getWidth () << ", " 
                                << set_roi_size.getHeight();

    // Change the roi by sending a command to the hardware
    CameraControl::getInstance()->setRoi(new_roi.getTopLeft().x          ,
                                         new_roi.getTopLeft().y          ,
                                         new_roi.getSize   ().getWidth (), 
                                         new_roi.getSize   ().getHeight());
}

//-----------------------------------------------------------------------------
/// Get the current roi values
//-----------------------------------------------------------------------------
void Camera::getRoi(Roi & hw_roi) ///< [out] Roi values
{
    DEB_MEMBER_FUNCT();

    hw_roi = Roi( static_cast<int>(CameraControl::getConstInstance()->getSerialOrigin  ()),
                  static_cast<int>(CameraControl::getConstInstance()->getParallelOrigin()),
                  static_cast<int>(CameraControl::getConstInstance()->getSerialLength  ()),
                  static_cast<int>(CameraControl::getConstInstance()->getParallelLength()));
    
    DEB_RETURN() << DEB_VAR1(hw_roi);
}
