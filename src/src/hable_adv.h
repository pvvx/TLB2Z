/*
 * hable_adv.h
 *
 *  Created on: 21.03.2024
 *      Author: pvvx
 */

#ifndef _HABLE_ADV_H_
#define _HABLE_ADV_H_

//=================== HAble =================

#define ADV_HA_BLE_NS_UUID16 0x181C // 16-bit UUID Service 0x181C HA_BLE, no security
#define ADV_HA_BLE_SE_UUID16 0x181E // 16-bit UUID Service 0x181E HA_BLE, security enable

// https://github.com/custom-components/ble_monitor/issues/548
typedef enum {
	HaBleID_PacketId = 0,	//0x00, uint8
	HaBleID_battery,      //0x01, uint8, %
	HaBleID_temperature,  //0x02, sint16, 0.01 °C
	HaBleID_humidity,     //0x03, uint16, 0.01 %
	HaBleID_pressure,     //0x04, uint24, 0.01 hPa
	HaBleID_illuminance,  //0x05, uint24, 0.01 lux
	HaBleID_weight,       //0x06, uint16, 0.01 kg
	HaBleID_weight_s,     //0x07, string, kg
	HaBleID_dewpoint,     //0x08, sint16, 0.01 °C
	HaBleID_count,        //0x09,	uint8/16/24/32
	HaBleID_energy,       //0x0A, uint24, 0.001 kWh
	HaBleID_power,        //0x0B, uint24, 0.01 W
	HaBleID_voltage,      //0x0C, uint16, 0.001 V
	HaBleID_pm2x5,        //0x0D, uint16, kg/m3
	HaBleID_pm10,         //0x0E, uint16, kg/m3
	HaBleID_boolean,      //0x0F, uint8
	HaBleID_switch,		  //0x10
	HaBleID_opened		  //0x11
} HaBleIDs_e;

// Type bit 5-7
typedef enum {
	HaBleType_uint = 0,
	HaBleType_sint = (1<<5),
	HaBleType_float = (2<<5),
	HaBleType_string  = (3<<5),
	HaBleType_MAC  = (4<<5)
} HaBleTypes_e;

typedef struct __attribute__((packed)) _adv_head_uuid16_t {
	u8	size;   // =
	u8	type;	// = 0x16, 16-bit UUID
	u16	UUID;	// = 0x181C, GATT Service HA_BLE
} adv_head_uuid16_t, * padv_head_uuid16_t;

// HA_BLE event, no security
typedef struct __attribute__((packed)) _adv_ha_ble_ns_ev1_t {
	adv_head_uuid16_t head;
/*	struct {
		u8		p_st;
		u8		p_id;	// = HaBleID_PacketId
		u8		pid;	// PacketId (!= measurement count)
	} st; */
	u8 data[3+3];
} adv_ha_ble_ns_evn_t, * padv_ha_ble_ns_evn_t;





#endif /* _HABLE_ADV_H_ */
