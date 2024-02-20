# RFID-BT-Nucleo-Lock
Project of an RFID access control system configured with a mobile application via a BT connection on a STM32L476RG

The system is based on the STM32L476RG microcontroller, using RFID technology to authenticate users and make access decisions, and Bluetooth technology has been implemented for communication with a mobile application on Android, used to configure the access control system, obtain login history data and remotely open the solenoid lock. using a connected control relay and power source.
Both-way communication is protected from interception using the RSA message encryption algorithm. The program part of the microcontroller was developed in C using the CubeMX programming environment. The mobile application was written in the Android Studio framework using Java using the Bluetooth API.
The operating algorithm is described in block diagrams and the connection of modules in electrical diagrams. The work shows three possibilities of mounting the end device based on an integrated PCB, on a universal board or a breadboard.
