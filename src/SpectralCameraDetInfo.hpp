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
/// return the detector pixel size in meter
//-----------------------------------------------------------------------------
void Camera::getPixelSize(double& sizex,    ///< [out] horizontal pixel size
                          double& sizey)    ///< [out] vertical   pixel size
{
    DEB_MEMBER_FUNCT();
    
    sizex = Camera::g_pixel_size_x;
    sizey = Camera::g_pixel_size_y;
    DEB_RETURN() << DEB_VAR2(sizex, sizey); 
}

//-----------------------------------------------------------------------------
/// return the detector Max image size 
//-----------------------------------------------------------------------------
void Camera::getDetectorMaxImageSize(Size& size) ///< [out] image dimensions
{
    DEB_MEMBER_FUNCT();
    size = Size(CameraControl::getConstInstance()->getWidthMax (), CameraControl::getConstInstance()->getHeightMax());
}

//-----------------------------------------------------------------------------
/// return the detector image size 
//-----------------------------------------------------------------------------
void Camera::getDetectorImageSize(Size& size) ///< [out] image dimensions
{
    DEB_MEMBER_FUNCT();
    getDetectorMaxImageSize(size);

    DEB_TRACE() << "Size (" << DEB_VAR2(size.getWidth(), size.getHeight()) << ")";
}

//-----------------------------------------------------------------------------
/// return the image type
//-----------------------------------------------------------------------------
void Camera::getImageType(ImageType& type)
{
    DEB_MEMBER_FUNCT();

    std::size_t pixel_depth = CameraControl::getConstInstance()->getPixelDepth();

    switch( pixel_depth )
    {
        case 16:
            type = Bpp16;
            break;
        default:
            THROW_HW_ERROR(Error) << "No compatible image type";
    }
}

//-----------------------------------------------------
//! Camera::setImageType()
//-----------------------------------------------------
void Camera::setImageType(ImageType type)
{
    DEB_MEMBER_FUNCT();
    DEB_TRACE() << "Camera::setImageType - " << DEB_VAR1(type);
    switch(type)
    {
        case Bpp16:
        {
            break;
        }
        default:
            THROW_HW_ERROR(Error) << "This pixel format of the camera is not managed, only 16 bits cameras are already managed!";
            break;
    }

    DEB_TRACE() << "SetImageType: " << type;
}

//-----------------------------------------------------------------------------
/// return the detector type
//-----------------------------------------------------------------------------
void Camera::getDetectorType(std::string& type) ///< [out] detector type
{
    DEB_MEMBER_FUNCT();
    type = "Spectral";
}

//-----------------------------------------------------------------------------
/// return the detector model
//-----------------------------------------------------------------------------
void Camera::getDetectorModel(std::string& type) ///< [out] detector model
{
    DEB_MEMBER_FUNCT();

    std::string model         = CameraControl::getConstInstance()->getModel();
    std::string serial_number = CameraControl::getConstInstance()->getSerialNumber();
    type = model + " (SN:" + serial_number + ")";
}
