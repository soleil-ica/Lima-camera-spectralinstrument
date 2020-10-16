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

//    m_roi = new_roi;
}

//-----------------------------------------------------------------------------
/// Get the current roi values
//-----------------------------------------------------------------------------
void Camera::getRoi(Roi & hw_roi) ///< [out] Roi values
{
    DEB_MEMBER_FUNCT();

    DEB_RETURN() << DEB_VAR1(hw_roi);
}
