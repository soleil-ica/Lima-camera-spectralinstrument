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
//
// SpectralCamera.h
// Created on: October 16, 2020
// Author: Cédric CASTEL

#ifndef SPECTRALCAMERA_H
#define SPECTRALCAMERA_H

// SYSTEM
#include <ostream>
#include <map>

// LIMA
#include "lima/HwBufferMgr.h"
#include "lima/HwInterface.h"
#include "lima/HwEventCtrlObj.h"
#include "lima/HwMaxImageSizeCallback.h"
#include "lima/Debug.h"

// PROJECT
#include "SpectralCompatibility.h"

#define REPORT_EVENT(desc) { \
    Event *my_event = new Event(Hardware,Event::Info, Event::Camera, Event::Default,desc); \
    m_cam.getEventCtrlObj()->reportEvent(my_event);  \
} \

namespace lima
{
namespace Spectral
{
    // pre-defines the BufferCtrlObj class
    class BufferCtrlObj;

/*******************************************************************
 * \class Camera
 * \brief object controlling the Spectral camera
 *******************************************************************/
	class LIBSPECTRAL_API Camera
	{
	    DEB_CLASS_NAMESPC(DebModCamera, "Camera", "Spectral");

    //-----------------------------------------------------------------------------
	public:
        // status values
        typedef enum Status
        {
            Init       , // starting the plugin
            Ready      , // ready to start acquisition
            Exposure   , // running an exposure
            Readout    , // running a readout
            Latency    , // running a latency
            Fault      , // acquisition stopped externally or unexpected error 

        } Status;

	//-----------------------------------------------------------------------------
	public:
        // constructor
        Camera(const std::string & connection_address          ,  // server name or IP address of the SI Image SGL II software
               unsigned long       connection_port             ,  // TCP/IP port of the SI Image SGL II software
               unsigned long       image_packet_pixels_nb      ,  // number of pixels sent into a image part TCP/IP packet
               unsigned long       image_packet_delay_micro_sec); // delay between the sending of two image part TCP/IP packets (in micro-seconds)

        // destructor
	    ~Camera();

        void init();
        void reset();
        void prepareAcq();
        void startAcq();
        void stopAcq();

	    Camera::Status getStatus();
        int  getNbHwAcquiredFrames();

        // -- detector info object
        void getImageType(ImageType& type);
        void setImageType(ImageType type);

        void getDetectorType(std::string& type);
        void getDetectorModel(std::string& model);
        void getDetectorImageSize(Size& size);
        void getDetectorMaxImageSize(Size& size);
        void getPixelSize(double& sizex, double& sizey);

        // -- Buffer control object
        HwBufferCtrlObj* getBufferCtrlObj();
        HwEventCtrlObj*  getEventCtrlObj();

        //-- Synch control object
        void setTrigMode(TrigMode mode);
        void getTrigMode(TrigMode& mode) const;
        bool checkTrigMode(TrigMode mode) const;

        void setExpTime(uint32_t   exp_time_ms);
        void getExpTime(uint32_t & exp_time_ms) const;

        void setLatTime(uint32_t   lat_time_ms);
        void getLatTime(uint32_t & lat_time_ms) const;

        void getExposureTimeRange(uint32_t & min_expo_ms, uint32_t & max_expo_ms) const;
        void getLatTimeRange     (uint32_t & min_lat_ms , uint32_t & max_lat_ms ) const;

        void setNbFrames(int   nb_frames);
        void getNbFrames(int & nb_frames) const;
	    void getNbHwAcquiredFrames(int &nb_acq_frames) const;

	    void checkRoi(const Roi& set_roi, Roi& hw_roi);
	    void setRoi(const Roi& set_roi);
	    void getRoi(Roi& hw_roi);    

	    void checkBin(Bin&);
	    void setBin(const Bin&);
	    void getBin(Bin&);
	    bool isBinningAvailable();       
        bool isBinningSupported(const int bin_value);

	    //- Spectral specific
        // access to the singleton
        static Camera * getInstance();

        // access the singleton (const version)
        static const Camera * getConstInstance();

        // configure the data update delay in msec
        void setDataUpdateDelayMsec(int in_data_update_delay_msec);

        // get the data update delay in msec
        int getDataUpdateDelayMsec() const;

        // do an update of several detector data (status, exposure time, etc...)
        bool updateData();

    //-----------------------------------------------------------------------------
	private:
        // execute a stop acq command
        void execStopAcq();

	//-----------------------------------------------------------------------------
	private:
        //-----------------------------------------------------------------------------
	    //- lima stuff
        //-----------------------------------------------------------------------------
        // Buffer control object
        SoftBufferCtrlObj   m_buffer_ctrl_obj;

        // Lima event control object
        HwEventCtrlObj      m_event_ctrl_obj;
        
        //-----------------------------------------------------------------------------
		//- Spectral
        //-----------------------------------------------------------------------------
        // used to give acess to the Camera instance like a singleton
        static Camera * g_singleton;

        // server name or IP address of the SI Image SGL II software
        std::string   m_connection_address;

        // TCP/IP port of the SI Image SGL II software
        unsigned long m_connection_port;

        // number of pixels sent into a image part TCP/IP packet
        unsigned long m_image_packet_pixels_nb;

        // delay between the sending of two image part TCP/IP packets (in micro-seconds)
        unsigned long m_image_packet_delay_micro_sec;

        // delay between the data update (status, exposure time, etc...) in msec
        int m_data_update_delay_msec;

        // simulated number of frames to acquire
        int m_nb_frames_to_acquire;

        // latency time in milli-seconds
        uint32_t m_latency_time_msec; 

        // current trigger mode
        lima::TrigMode m_trigger_mode;

		//-----------------------------------------------------------------------------
        // Constants
		//-----------------------------------------------------------------------------
        static const double g_pixel_size_x;
        static const double g_pixel_size_y;
	};
} // namespace Spectral
} // namespace lima


#endif // SpectralCAMERA_H
