
BLE Shield Library
------------------

BLE Shield is a nRF8001 BLE controller chip with circuitry that enables BLE feature to Arduino boards.

BLE Shield Library is a software library allows Arduino board to control the BLE Shield, so that it can send/receive data to/from BLE devices such as iPhone.

Usage
-----

Put it in your Documents/Arduino/libraries folder.

If you have any difficulty with Arduino Libraries, read this:
http://arduino.cc/en/Guide/Libraries

Q&A
---

Q1: What is BT dual mode? Is the BLE Shield a dual mode device?<br/>
A1: Dual mode devices (e.g. iPhone 4S/5) are compatible with both Classic and BLE devices.<br\>
    BLE Shield is a single mode device only, so it is not compatible with the classic mode BT.

Q2: What is BLE Central role? Can the BLE Shield work in Central role?<br\>
A2: Central role devices (e.g. iPhone 4S/5) can work as a master in the BLE star network, they can perform scanning and connect to peripheral devices.
    The BLE Shield can work as peripheral role only, i.e. it can only be connected from other central rol devices.

Q3: Can I get RSSI from the BLE Shield?<br/>
A3: The nRF8001 does not provide any function call to get RSSI, so this is not possible. You can get RSSI from the BLE central side, e.g. iPhone 5.

Q4: Can I change the name (BLE Shield) to others?<br/>
A4: You need to use Nordic's nRFgo Studio and open the BLEShield.xml to edit the settings, it will generate some C source files.<br/>
    This is out of the scope of the BLE library, so you need to study their documents.

Q5: Can I change the 128-bit UUID with my own one?<br/>
A5: Same as A2, you can also set the connection interval, advertising data, etc.
