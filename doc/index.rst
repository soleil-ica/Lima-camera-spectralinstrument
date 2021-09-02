.. _camera-spectralinstrument:

SpectralInstrument camera plugin
-------------------

Intoduction
```````````
The SpectralInstrument is a ...


Module configuration
````````````````````



Initialisation and Capabilities
```````````````````````````````

Implementing a new plugin for new detector is driven by the LIMA framework but the developer has some freedoms to choose which standard and specific features will be made available. This section is supposed to give you the correct information regarding how the camera is exported within the LIMA framework.

Camera initialisation
.....................


Std capabilites
...............

This plugin has been implemented in respect of the mandatory capabilites but with some limitations according to some programmer's choices. We only provide here extra information for a better understanding of the capabilities for the SpectralInstrument camera.


Optional capabilites
........................

* Cooling

  This plugin provides the ability to manage camera cooling

  Supported values : 

 - ON

 - OFF

* CCDTemperature

 The plugin provides ability get the current camera temperature

* Readout Speed

 The plugin provide ability to manage DSI sample Time by set readout speed parameter

 Supported values : 

 - 1MHz

 - 690KHz

Configuration
`````````````

No specific hardware configuration is needed.

How to use
````````````

Here is the list of accessible fonctions to configure and use the SpectralInstrument detector:

.. code-block:: cpp

