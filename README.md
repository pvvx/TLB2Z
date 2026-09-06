[![english](https://img.shields.io/badge/language-english-C1C1C1?style=for-the-badge)](README.md)
[![russian](https://img.shields.io/badge/also%20available%20in-russian-blue?style=for-the-badge)](README.ru.md)

# TLB2Z
Low cost BLE Advertisements Repeater into Zigbee 3.0 on TLRS825x. 

# Repeat up to 3 BLE thermometer in Zigbee 3.0 on a Module with TLSR825x Chip

![b2z.gif](https://github.com/pvvx/TLB2Z/blob/master/web/b2z.gif)

![b2z1](https://github.com/pvvx/TLB2Z/blob/master/web/b2z1.jpg)

Use this **[TB-03F-KIT](https://docs.ai-thinker.com/en/blue_tooth)** or **[TB-04-KIT](https://docs.ai-thinker.com/en/blue_tooth)** ($3)

![tb-03f-kit.gif](https://github.com/pvvx/TLB2Z/blob/master/web/tb-03f-kit.gif)

Alternative 1: **[TB-03F](https://docs.ai-thinker.com/en/blue_tooth)** (~$1.5), lacking LEDs, USB UART, LDO. Above options are easier to set up.

![tb-03f.gif](https://github.com/pvvx/TLB2Z/blob/master/web/tb-03f.gif)

Alternative 2: **[TS0001_TZ3000_gjrubzje](https://pvvx.github.io/TS0001_TZ3000/)**

![ZigbeeSwitch](https://raw.githubusercontent.com/pvvx/pvvx.github.io/refs/heads/master/TS0001_TZ3000_fdxihpp7/img/ZigbeeSwitch.jpg)

Alternative 3: **[Zigbee Repeater HOBEIAN](https://pvvx.github.io/ZG-807Z/)**

![ZG-807Z](https://pvvx.github.io/ZG-807Z/img/ZG-807Z.jpg)

* BLE Configuration page [BLE2Zigbee.html](https://pvvx.github.io/TLB2Z/BLE2Zigbee_en.html)
* Initial programming (https://github.com/pvvx/TLB2Z/tree/master/pgm)
* Firmware and Zigbee OTA files (https://github.com/pvvx/TLB2Z/tree/master/fw)


## Warning: 

Z2M has some bugs:

Incorrectly interprets values in the Illuminance Measure cluster!

1. Doesn't understand special values – Illuminance Value = 0, 0xffff.
2. Doesn't understand constraints – min/max Measured Value.

Z2M is not recommended for use with devices operating under Zigbee 3.0 specifications.

## Main Specifications

* Receive BLE advertisements from up to 3 thermometer-hygrometers or motion-light sensors and translate the readings as a Zigbee end device.
* BLE advertisements are received 99.5% of the time: Zigbee polling occurs 16ms every 4 seconds. This ensures minimal reception loss from BLE devices.
* Zigbee device uses 3 endpoints. One for each BLE device.
* Zigbee TX +10dBm, BLE TX +10dBm (BLE is used for customization only).
* Zigbee OTA support.
* Support advertising BLE formats Xiaomi(Mi-Home), Qingping, BTHome v2, Custom.
* Support encrypted BLE advertising options with bindkey.
* Support BLE trigger On/Off ([ATC_MiThermometer](https://github.com/pvvx/ATC_MiThermometer?tab=readme-ov-file#temperature-or-humidity-trigger-gpio-pc4-lywsd03mmc-label-on-the-p9-pin)), Motion.
* Support Direct binding On/Off.
* Average consumption when operating the TB-03F-KIT module from USB (5V): 10 mA

# Customizing the module

1. Connect the programmed module to the power supply
2. Open [BLE2Zigbee.html](https://pvvx.github.io/TLB2Z/BLE2Zigbee_en.html)
3. Press the button labeled "PROG" for 1 second. This action will switch the module to the BLE connection standby mode. The waiting time for BLE connection is up to 80 seconds.
4. In [BLE2Zigbee.html](https://pvvx.github.io/TLB2Z/BLE2Zigbee_en.html) make a connection to the BLE device "B2Z-xxxxxxx".
5. Enter the MAC of the thermometers and the BindKey used if necessary.
6. Verify that the data is being received:

![b2z_html.gif](https://github.com/pvvx/TLB2Z/blob/master/web/b2z_html.gif)

7.  Close [BLE2Zigbee.html](https://pvvx.github.io/TLB2Z/BLE2Zigbee_en.html).

Note: On the TB-03F, for PROG, connect SWS to ground

## Pairing the module with Zigbee

1.  Enable pairing mode on the Zigbee coordinator or router.
2.  Press and hold the button on the module labeled "PROG" for 8 seconds. This action will reset the Zigbee bindings and settings.
3.  Trace the pairing clusters. Set the desired values for "Report".

![b2z_zha.gif](https://github.com/pvvx/TLB2Z/blob/master/web/b2z_zha.gif)

![grf.gif](https://github.com/pvvx/TLB2Z/blob/master/web/grf.gif)

## LED indication

### RGB LED blinks briefly when receiving data from BLE devices.

* Red LED color - reception from the 1rd BLE device.
* Green color LED - reception from the 2rd BLE device.
* Blue LED color - reception from the 3rd BLE device.

### White LED indicates Zigbee status. 

* If there is no pairing, the indicator flashes slowly.
* Fast blinking when pairing
* Blinks on "Identify" command

### Yellow LED indicates BLE status. 

* Blinks briefly when waiting for BLE connection.

### LED on SmartSwitch devices with a relay.

* If there is no pairing, the indicator flashes slowly.
* Fast blinking when pairing.
* Blinks on "Identify" command.
* Steady on or off - indicates the relay status.


## TLB2Z Zigbee Reportable:

| Endpoint | Cluster | Attribute | NN BLE device |
| --- | --- | --- | --- |
| 1 | 0x0001 Power Configuration | 0x0020 BatteryVoltage | 1 |
| 1 | 0x0001 Power Configuration | 0x0021 BatteryPercentageRemaining | 1 |
| 1 | 0x0001 Power Configuration | 0x0040 BatteryVoltage | 2 |
| 1 | 0x0001 Power Configuration | 0x0041 BatteryPercentageRemaining | 2 |
| 1 | 0x0001 Power Configuration | 0x0060 BatteryVoltage | 3 |
| 1 | 0x0001 Power Configuration | 0x0061 BatteryPercentageRemaining | 3 |
| 1 | 0x0006 On/Off | 0x0000 OnOff | 1 |
| 1 | 0x0400 Illuminance Measurement | 0x0000 MeasuredValue | 1 |
| 1 | 0x0401 Illuminance Level Sensing | 0x0000 LevelStatus | 1 |
| 1 | 0x0402 Temperature Measurement | 0x0000 MeasuredValue | 1 |
| 1 | 0x0405 Relative Humidity Measurement | 0x0000 MeasuredValue  | 1 |
| 2 | 0x0006 On/Off | 0x0000 OnOff | 2 |
| 2 | 0x0400 Illuminance Measurement | 0x0000 MeasuredValue | 2 |
| 2 | 0x0401 Illuminance Level Sensing | 0x0000 LevelStatus | 2 |
| 2 | 0x0402 Temperature Measurement | 0x0000 MeasuredValue | 2 |
| 2 | 0x0405 Relative Humidity Measurement | 0x0000 MeasuredValue  | 2 |
| 3 | 0x0006 On/Off | 0x0000 OnOff | 3 |
| 3 | 0x0400 Illuminance Measurement | 0x0000 MeasuredValue | 3 |
| 3 | 0x0401 Illuminance Level Sensing | 0x0000 LevelStatus | 3 |
| 3 | 0x0402 Temperature Measurement | 0x0000 MeasuredValue | 3 |
| 3 | 0x0405 Relative Humidity Measurement | 0x0000 MeasuredValue  | 3 |

* If less than 3 BLE thermometer-hygrometers are used or the thermometer-hygrometer does not transmit any parameter, then there is no need to link these clusters in the Zigbee coordinator.

### Default Report Setting:

| Name | Tmin | Tmax | Change |
| --- | --- | --- | --- |
| OnOff | 0 sec | 3600 sec | 0 |
| BatteryVoltage | 360 sec | 3600 sec | 0 |
| BatteryPercentageRemaining | 360 sec | 3600 sec | 0 |
| Illuminance Measurement | 30 sec | 180 sec | 0 |
| Illuminance Level Sensing | 0 sec | 3600 sec | 0 |
| Temperature Measurement | 30 sec | 180 sec | 10 (0.1 C) |
| Relative Humidity Measurement | 30 sec | 180 sec | 50 (0.5 %) |

* Xiaomi thermometer-hygrometers use a large data transmission interval, lasting tens of minutes. To avoid unnecessary duplicate points on the graphs, it is desirable to set appropriate (multiple) intervals in the Zigbee report.

## TLB2Z Zigbee Settings:

| Endpoint | Cluster | Attribute | NN BLE device |
| --- | --- | --- | --- |
| 1 | 0x0006 On/Off | 0x4003 StartUpOnOff | 1 |
| 2 | 0x0006 On/Off | 0x4003 StartUpOnOff | 2 |
| 3 | 0x0006 On/Off | 0x4003 StartUpOnOff | 3 |
| 1 | 0x0006 On/Off | 0xF001 Trigger BTHome Code | 1 |
| 2 | 0x0006 On/Off | 0xF001 Trigger BTHome Code | 2 |
| 3 | 0x0006 On/Off | 0xF001 Trigger BTHome Code | 3 |
| 1 | 0x0401 Illuminance Level Sensing | 0x0010 Illuminance Target Level | 1 |
| 2 | 0x0401 Illuminance Level Sensing | 0x0010 Illuminance Target Level | 2 |
| 3 | 0x0401 Illuminance Level Sensing | 0x0010 Illuminance Target Level | 3 |
