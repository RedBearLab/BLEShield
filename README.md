BLE Shield
==========

The BLE Shield is an Arduino compatible shield with nRF8001 BLE controller chip that enables BLE feature for Arduino boards.


Library
=======

BLE Shield Library (now called nRF8001 library) is a software library allows Arduino board to control the BLE Shield, it provides a simple TX/RX service for exchanging data between nRF8001 peripheral device and central devices such as iPhone/iPad/Android.

This repository contains hardware information and the old version of BLE Shield library for Arduino. The library file will be changed to the new repository:
[RBL_nRF8001 library](https://github.com/RedBearLab/nRF8001)

The library files here are just kept for future reference only (require [Nordic library](https://github.com/NordicSemiconductor/ble-sdk-arduino/releases) to work)

The nRF8001 library will be shared for different products like the BLE Shield and Blend Micro,  

Actually, if your are learning how BLE works rather than just exchanging data, you can use Nordic's nRF8001 SDK for Arduino with the BLE Shield directly, it has a lot of examples and tutorials for BLE beginers.<br/>
[Nordic BLE SDK for Arduino](https://github.com/NordicSemiconductor/ble-sdk-arduino)


Support & Resources
===================

[BLE Shield Forum](https://redbearlab.zendesk.com/forums/21286751-BLE-Shield)

[Nordic nRF8001 SDK for Arduino](https://redbearlab.zendesk.com/forums/21921933-Nordic-nRF8001-SDK-for-Arduino)

[Nordic Developer Zone](https://devzone.nordicsemi.com/)

[Bluetooth SIG](https://www.bluetooth.org/en-us)


Q&A
===

Q1: What is BT dual mode? Is the BLE Shield a dual mode device?<br/>
A1: Dual mode devices (e.g. iPhone 4S/5) are compatible with both Classic and BLE devices.<br\>
    BLE Shield is a single mode device only, so it is not compatible with the classic mode BT.

Q2: What is BLE Central role? Can the BLE Shield work in Central role?<br\>
A2: Central role devices (e.g. iPhone 4S/5) can work as a master in the BLE star network, they can perform scanning and connect to peripheral devices.
    The BLE Shield can work as peripheral role only, i.e. it can only be connected from other central rol devices.

Q3: Can I get RSSI from the BLE Shield?<br/>
A3: The nRF8001 does not provide any function call to get RSSI, so this is not possible. You can get RSSI from the BLE central side, e.g. iPhone 5.

Q4: Can I change the name (BLE Shield) to others?<br/>
A4: You can use the API - "ble_set_name(name)" to do that. See examples.

Q5: Can I change the 128-bit UUID with my own one?<br/>
A5: You need to use Nordic's nRFgo Studio and open the BLEShield.xml to edit the settings, it will generate some C source files using the .bat file.<br/>
You can also set the connection interval, advertising data, etc.

Q6: Can two BLE Shields talk to each other?<br/>
A6: Since the BLE Shield cannot run in Central role, so this is not possible.


License
=======

Copyright (c) 2013 RedBearLab

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal 
in the Software without restriction, including without limitation the rights 
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
