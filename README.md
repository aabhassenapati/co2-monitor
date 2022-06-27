# CO2-monitor
![Labelled CO2 MONITOR](https://user-images.githubusercontent.com/38909361/175974309-d9bc69a0-2a1f-479e-8127-72a23dcc7fb5.PNG)

Repository for ongoing "SpiroMask" project at IIT Gandinagar to make accessible and accurate lung health diagnosis using measure of breathing rate with microphones and CO2 sensors, this work is on a subproject of CO2 monitoring device to be used in general purpose monitoring and for baseline CO2 level measurement for peak transmission in SpiroMask, made by Aabhas Senapati, during internship period at Sutainibility Lab, IIT Gandhinagar.

Link to project summary presentation- https://to.aabhass.in/iitgn-report

-> The Schematic Folder contains a fritzing file (.fzz) to import the circuit for PCB design.

-> The Image folder contains some images of the CO2 Monitor device.

-> The Graph folder Contains labbeled graphs of logged data from this device in differeant modes.

-> The Data folder contains .csv format datafile as saved in sd card by bthe device.

-> The Code Folder contains 2 versions of Code, version 1 being Data logging without any modes, while in version 2 of code the device runs on two modes based on hardware switching, Mode one is when switch is turned up towards point 1, then both CCS and MH data is recorded and MH data is displayed on OLED, while Mode 2 is when switch in turned down towards point 2 then only CCS data is recorded and displayed on OLED, while MH data 0 is saved.

List of Hardware used: 

-> Adafruit Feather M0 Adalogger Microcontroller

-> PCF8563 Real Time Clock Module

-> CCS811 CO2 & VOC Sensor

-> MHZ19 CO2 Sensor 

-> OLED Display

-> Switch (SPDT)

-> SD Card
