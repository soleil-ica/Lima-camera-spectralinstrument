.. _camera-spectralinstrument:

Spectral Instrument communication framework
-------------------------------------------

Introduction
.............
This document describe network communication mechanism between detector hardware, LIMA framework and Spectral Instrument SDK


NetPacket principal
........................
The NetPacket class manage communication from host to network and network to host.

Commands and answers are stored in buffer for read/write action.

Buffer stored data can contain information like string or bitmap file data. Isn't possible to directly cast memory.

NetPacket structure's
......................

* NetGenericHeader : abstract class, generic data from TCP/IP packets

  - NetCommandHeader : class with common data in all commands

   -  NetCommandAction : specific action command

  - NetGenericAnswer : class with common data in all answers

   - NetAnswerAction : specific answer for related command

Implemented commands and answers
................................
=============================== ======================== ================== ===============================================
Name                            Function Number          Command/Answer     Description
=============================== ======================== ================== ===============================================
NetGenericHeader                NA                       NA                 Abstract class, generic data from TCP/IP packets
NetCommandHeader                NA                       Command            Class with common data in all commands
NetCommandGetStatus             1011                     Command            Receives the camera status
NetCommandGetCameraParameters   1048                     Command            Retrieves Parameter Tables from the camera
NetCommandGetSettings           1041                     Command            Gets the SI Image SGL II program settings 
NetCommandSetAcquisitionMode    1034                     Command            Sets the Acquisition Mode
NetCommandSetExposureTime       1035                     Command            Sets the exposure time for subsequent acquisitions
NetCommandSetFormatParameters   1043                     Command            Sets the CCD Format Parameters
NetCommandSetAcquisitionType    1036                     Command            Sets the Acquisition Type
NetCommandAcquire               1037                     Command            Starts an acquisition that has been setup previously
NetCommandTerminateAcquisition  1018                     Command            Terminates an acquisition in progress
CommandTerminateImageRetrieve   1020                     Command            Terminates the image retrieve in progress
NetCommandRetrieveImage         1019                     Command            Retrieves the current Image
CommandInquireAcquisitionStatus 1017                     Command            Receive status of current acquisition
NetCommandConfigurePackets      1022                     Command            Sets the pixels per image packet and delay in packet sending loop  
NetAcknowledge                  NA                       NA                 This class is an acknowledge packet class
NetGenericAnswer                NA                       Answer             This class is an generic answer packet class
NetAnswerGenericString          NA                       Answer             This class is a generic string answer packet class
NetAnswerGetStatus              NA                       Answer             This class is a Get Status answer packet class
NetAnswerGetCameraParameters    NA                       Answer             This class is a Get Camera Parameters answer packet class
NetAnswerGetSettings            NA                       Answer             This class is a get settings packet class
NetAnswerCommandDone            NA                       Answer             This class is a Command Done packet class
NetAnswerSetAcquisitionMode     NA                       Answer             This class is a Command Done packet class for set acquisition mode answer
NetAnswerSetExposureTime        NA                       Answer             This class is a Command Done packet class for set exposure time answer
NetAnswerSetFormatParameters    NA                       Answer             This class is a Command Done packet class for set format parameters answer
NetAnswerSetAcquisitionType     NA                       Answer             This class is a Command Done packet class for set acquisition type answer
NetAnswerAcquire                NA                       Answer             This class is a Command Done packet class for acquire answer
NetAnswerTerminateAcquisition   NA                       Answer             This class is a Command Done packet class for terminate acquisition answer
NetAnswerTerminateImageRetrieve NA                       Answer             This class is a Command Done packet class for terminate image retrieve
NetAnswerAcquisitionStatus      NA                       Answer             This class is an acquisition status packet class (answer for InquireAcquisitionStatus command)
NetAnswerConfigurePackets       NA                       Answer             This class is a Command Done packet class for configure packets answer
NetImageHeader                  NA                       NA                 This class is an image header packet class
NetImage                        NA                       NA                 This class is an image packet class
NetCommandSetCoolingValue       1046                     Command            Turns the CCD cooling On/Off
NetAnswerSetCoolingValue        NA                       Answer             This class is a Command Done packet class for set cooling value
NetCommandSetSingleParameter    1044                     Command            Sets any single Camera Parameter
NetAnswerSetSingleParameter     NA                       Answer             this class is a Command Done packet class for set single parameter
=============================== ======================== ================== =============================================== 



