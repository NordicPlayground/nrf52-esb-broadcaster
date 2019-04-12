nrf52-esb-broadcaster
=====================
Description
-----------
This example implements a simple any-to-any RF protocol based on Enhanced Shockburst (ESB). 
It abstracts away most of the ESB configuration for simplicity, and just provides functions for initializing the library and sending data, and allows you to configure a callback for receiving data. 

The only init parameter (except for the receive callback) is a network ID, which allows multiple adjacent networks to communicate without interfering with each other. 

Any device will transmit to all devices within range as long as they have the same network ID. 
There is no packet acknowledgement, and as such a transmitting device is unaware if the packet was received or not. 

Requirements
------------
- nRF5 SDK version 15.3.0
- nRF52 DK or nRF52840 DK

The project may need modifications to work with later versions or other boards. 

To compile it, clone the repository in the /nRF5_SDK_12.1.0/examples/proprietary_rf/ folder (or any other subdirectory under /nRF5_SDK_12.1.0/examples/)

About this project
------------------
This application is one of several applications that has been built by the support team at Nordic Semiconductor, as a demo of some particular feature or use case. It has not necessarily been thoroughly tested, so there might be unknown issues. It is hence provided as-is, without any warranty. 

However, in the hope that it still may be useful also for others than the ones we initially wrote it for, we've chosen to distribute it here on GitHub. 

The application is built to be used with the official nRF5 SDK that can be downloaded from developer.nordicsemi.com