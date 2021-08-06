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
/// Check is the given binning values are supported, rise an exception otherwise
//-----------------------------------------------------------------------------
void Camera::checkBin(Bin & hw_bin) ///< [out] binning values to update
{
    DEB_MEMBER_FUNCT();

    if ( (hw_bin.getX() != hw_bin.getY()) || (!isBinningSupported(hw_bin.getX())) )
    {
        DEB_ERROR() << "Binning values not supported";
        THROW_HW_ERROR(Error) << "Binning values not supported";
    }

    DEB_RETURN() << DEB_VAR1(hw_bin);
}

//-----------------------------------------------------------------------------
/// set the new binning mode
//-----------------------------------------------------------------------------
void Camera::setBin(const Bin & set_bin) ///< [in] binning values objects
{
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(set_bin);

    // Change the roi by sending a command to the hardware
    CameraControl::getInstance()->setBinning(set_bin.getX(), set_bin.getY());

    DEB_RETURN() << DEB_VAR1(set_bin);
}

//-----------------------------------------------------------------------------
/// Get the current binning mode
//-----------------------------------------------------------------------------
void Camera::getBin(Bin & hw_bin) ///< [out] binning values object
{
    DEB_MEMBER_FUNCT();
    
    hw_bin = Bin( static_cast<int>(CameraControl::getConstInstance()->getSerialBinning  ()),
                  static_cast<int>(CameraControl::getConstInstance()->getParallelBinning()));

    DEB_RETURN() << DEB_VAR1(hw_bin);
}

//-----------------------------------------------------------------------------
/// Check if a binning value is supported
/*
@return true if the given binning value exists
*/
//-----------------------------------------------------------------------------
bool Camera::isBinningSupported(const int bin_value)    ///< [in] binning value to chck for
{
    DEB_MEMBER_FUNCT();
    return true;
}

//-----------------------------------------------------------------------------
/// Tells if binning is available
/*!
@return always true, hw binning mode is supported
*/
//-----------------------------------------------------------------------------
bool Camera::isBinningAvailable()
{
    DEB_MEMBER_FUNCT();
    return true;
}
