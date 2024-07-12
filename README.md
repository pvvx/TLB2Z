[![english](https://img.shields.io/badge/language-english-C1C1C1?style=for-the-badge)](README.md)
[![russian](https://img.shields.io/badge/also%20available%20in-russian-blue?style=for-the-badge)](README.ru.md)

# TLB2Z
Low cost BLE Advertisements Repeater into Zigbee 3.0 on TLRS825x. 

# Repeat up to 3 BLE thermometer in Zigbee 3.0 on a Module with TLSR825x Chip

![b2z.gif](https://github.com/pvvx/TLB2Z/blob/master/web/b2z.gif)

Use this **[TB-03F-KIT](https://docs.ai-thinker.com/en/blue_tooth)** or **[TB-04-KIT](https://docs.ai-thinker.com/en/blue_tooth)** ($3)

![tb-03f-kit.gif](https://github.com/pvvx/TLB2Z/blob/master/web/tb-03f-kit.gif)

Alternative  **[TB-03F](https://docs.ai-thinker.com/en/blue_tooth)** (~$1.5), lacking LEDs, USB UART, LDO. Above options are easier to set up.

![tb-03f.gif](https://github.com/pvvx/TLB2Z/blob/master/web/tb-03f.gif)

* BLE Configuration page [BLE2Zigbee.html](https://pvvx.github.io/TLB2Z/BLE2Zigbee.html)
* Initial programming (https://github.com/pvvx/TLB2Z/tree/master/pgm)
* Firmware and Zigbee OTA files (https://github.com/pvvx/TLB2Z/tree/master/fw)

## Main Specifications

* Receive BLE advertisements from up to 3 thermometer-hygrometers and translate the readings as a Zigbee end device.
* BLE advertisements are received 99.5% of the time: Zigbee polling occurs 16ms every 4 seconds. This ensures minimal reception loss from BLE devices.
* Zigbee device uses 3 endpoints. One for each BLE thermometer-hygrometer.
* Zigbee TX +10dBm, BLE TX +0dBm (BLE is used for customization only).
* Zigbee OTA support.
* Support advertising BLE formats Xiaomi(Mi-Home), Qingping, BTHome v2, Custom.
* Support encrypted BLE advertising options with bindkey.
* Average consumption when operating the TB-03F-KIT module from USB (5V): 10 mA

# Customizing the module

1.  Connect the programmed module to the power supply
2.  Open [BLE2Zigbee.html](https://pvvx.github.io/TLB2Z/BLE2Zigbee.html)
3. Press the button labeled "PROG" for 1 second. This action will switch the module to the BLE connection standby mode. The waiting time for BLE connection is up to 80 seconds.
4.  In [BLE2Zigbee.html](https://pvvx.github.io/TLB2Z/BLE2Zigbee.html) make a connection to the BLE device "B2Z-xxxxxxx".
5.  Enter the MAC of the thermometers and the BindKey used if necessary.
6.  Verify that the data is being received:

![b2z_html.gif](https://github.com/pvvx/TLB2Z/blob/master/web/b2z_html.gif)

7.  Close [BLE2Zigbee.html](https://pvvx.github.io/TLB2Z/BLE2Zigbee.html).

Note: On the TB-03F, for PROG, connect SWS to ground

## Pairing the module with Zigbee

1.  Enable pairing mode on the Zigbee coordinator or router.
2.  Press and hold the button on the module labeled "PROG" for 8 seconds. This action will reset the Zigbee bindings and settings.
3.  Trace the pairing of the temperature, humidity, and battery clusters. Set the desired values for "Report".

![b2z_zha.gif](https://github.com/pvvx/TLB2Z/blob/master/web/b2z_zha.gif)

![grf.gif](https://github.com/pvvx/TLB2Z/blob/master/web/grf.gif)

## LED indication

### RGB LED blinks briefly when receiving data from BLE devices.

* Red LED color - reception from the 1st thermometer
* Green color LED - reception from 2nd thermometer
### Blue LED color - reception from the 3rd thermometer.

### White LED indicates Zigbee status. 

* Lights up constantly if there is no pairing
* Blinks 7 times when pairing
### Blinks on "Identify" command

### Yellow LED indicates BLE status. 

* Blinks briefly when waiting for BLE connection

## TLB2Z Reportable:

| Endpoint | Cluster | Attribute | NN BLE device |
| --- | --- | --- | --- |
| 1 | 0x0001 Power Configuration | 0x0020 BatteryVoltage | 1 |
| 1 | 0x0001 Power Configuration | 0x0021 BatteryPercentageRemaining | 1 |
| 1 | 0x0402 Temperature Measurement | 0x0000 MeasuredValue | 1 |
| 1 | 0x0405 Relative Humidity Measurement | 0x0000 MeasuredValue  | 1 |
| 2 | 0x0001 Power Configuration | 0x0020 BatteryVoltage | 2 |
| 2 | 0x0001 Power Configuration | 0x0021 BatteryPercentageRemaining | 2 |
| 2 | 0x0402 Temperature Measurement | 0x0000 MeasuredValue | 2 |
| 2 | 0x0405 Relative Humidity Measurement | 0x0000 MeasuredValue  | 2 |
| 3 | 0x0001 Power Configuration | 0x0020 BatteryVoltage | 3 |
| 3 | 0x0001 Power Configuration | 0x0021 BatteryPercentageRemaining | 3 |
| 3 | 0x0402 Temperature Measurement | 0x0000 MeasuredValue | 3 |
| 3 | 0x0405 Relative Humidity Measurement | 0x0000 MeasuredValue  | 3 |

* If less than 3 BLE thermometer-hygrometers are used or the thermometer-hygrometer does not transmit any parameter, then there is no need to link these clusters in the Zigbee coordinator.
* If the BLE advertising protocol (Xiaomi, Qingping) of the thermometer does not provide for the transmission of battery voltage, then the battery voltage is substituted with 3.0V when receiving percent of charge.
* ZHA does not know how to accept 3 batteries. I.e. does not have full support for Zigbee 3.0, as it says in its advertising.

### Default Report Setting:

| Name | Tmin | Tmax | Change |
| --- | --- | --- | --- |
| BatteryVoltage | 360 sec | 3600 sec | 0 |
| BatteryPercentageRemaining | 360 sec | 3600 sec | 0 |
| Temperature Measurement | 30 sec | 180 sec | 10 (0.1 C) |
| Relative Humidity Measurement | 30 sec | 180 sec | 50 (0.5 %) |

* Xiaomi thermometer-hygrometers use a large data transmission interval, lasting tens of minutes. To avoid unnecessary duplicate points on the graphs, it is desirable to set appropriate (multiple) intervals in the Zigbee report.
