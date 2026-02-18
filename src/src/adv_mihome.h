/*
 * mihome_adv.h
 *
 *  Created on: 21.03.2024
 *      Author: pvvx
 */

#ifndef _MIHOME_ADV_H_
#define _MIHOME_ADV_H_

#define ADV_MIHOME_UUID16	0xfe95
//=================== Mi-Home =================
// Service 0xfe95 Xiaomi Inc.

//	https://iot.mi.com/new/doc/embedded-development/ble/object-definition
enum { // mijia ble version 5, General events
	MI_DATA_EV_Base					=0x0000,
	MI_DATA_EV_Connect				=0x0001,
	MI_DATA_EV_SimplrPair			=0x0002,
	MI_DATA_EV_Motion				=0x0003,
	MI_DATA_EV_KeepAway				=0x0004,
	MI_DATA_EV_LockObsolete			=0x0005,
	MI_DATA_EV_FingerPrint			=0x0006,
	MI_DATA_EV_Door					=0x0007,
	MI_DATA_EV_Armed				=0x0008,
	MI_DATA_EV_GestureController	=0x0009,
	MI_DATA_EV_BodyTemp				=0x000a,
	MI_DATA_EV_Lock					=0x000b,
	MI_DATA_EV_Flooding				=0x000c,
	MI_DATA_EV_Smoke				=0x000d,
	MI_DATA_EV_Gas					=0x000e,
	MI_DATA_EV_MovingWithLight		=0x000f, // Someone is moving (with light)
	MI_DATA_EV_ToothbrushIncident	=0x0010,
	MI_DATA_EV_CatEyeIncident		=0x0011,
	MI_DATA_EV_WeighingEvent		=0x0012,
	MI_DATA_EV_Button				=0x1001
} MI_DATA_EV;

enum { // mijia ble version 5
	MI_DATA_ID_Sleep				=0x1002,
	MI_DATA_ID_RSSI					=0x1003,
	MI_DATA_ID_Temperature			=0x1004,
	MI_DATA_ID_SwitchTemperature	=0x1005, // 2 bytes, switch, temp in 1 C
	MI_DATA_ID_Humidity				=0x1006, // 2 bytes, switch,in 0.1 %
	MI_DATA_ID_LightIlluminance		=0x1007,
	MI_DATA_ID_SoilMoisture			=0x1008,
	MI_DATA_ID_SoilECvalue			=0x1009,
	MI_DATA_ID_Power				=0x100A,
	MI_DATA_ID_TempAndHumidity		=0x100D,
	MI_DATA_ID_Lock					=0x100E,
	MI_DATA_ID_Gate					=0x100F,
	MI_DATA_ID_Formaldehyde			=0x1010,
	MI_DATA_ID_Bind					=0x1011,
	MI_DATA_ID_Switch				=0x1012,
	MI_DATA_ID_RemAmCons			=0x1013, // Remaining amount of consumables
	MI_DATA_ID_Flooding				=0x1014,
	MI_DATA_ID_Smoke				=0x1015,
	MI_DATA_ID_Gas					=0x1016,
	MI_DATA_ID_NoOneMoves			=0x1017,
	MI_DATA_ID_LightIntensity		=0x1018,
	MI_DATA_ID_DoorSensor			=0x1019,
	MI_DATA_ID_WeightAttributes		=0x101A,
	MI_DATA_ID_NoOneMovesOverTime 	=0x101B, // No one moves over time
	MI_DATA_ID_SmartPillow			=0x101C,

	MI_DATAF_ID2_Temperature		=0x4801, // float
	MI_DATA1_ID_Humidity 			=0x4802, // 1 byte, in 1 C
	MI_DATA1_ID_Battery				=0x4803, // 1 byte, in 1 %
	MI_DATA1_ID_OpeningStatus		=0x4804, // 1 byte
	MI_DATA1_ID_Illuminance_f		=0x4805, // float
	MI_DATA1_ID_MoistureDetected    =0x4806, // 1 byte
	MI_DATAF_ID2_Humidity			=0x4808, // float
	MI_DATA1_ID_SleepState			=0x4810, // 1 byte
	MI_DATA1_ID_SnoringState		=0x4810, // 1 byte

	MI_DATA1_PressurePresentState	=0x483c, // 1 byte
	MI_DATA4_PressurePresentDuration	=0x483d, // int32
	MI_DATA4_PressureNotPresentDuration	=0x483e, // int32
	MI_DATA4_PressurePresentTimeSet		=0x483f, // int32
	MI_DATA4_PressureNotPresentTimeSet	=0x483f, // int32

	MI_DATA1_OccupancyStatus		=0x484e, // uint8

	MI_DATAF_ID_Temperature			=0x4C01, // float
	MI_DATA1_ID2_Humidity			=0x4C02, // 1 byte, in 1 %
	MI_DATA2_ID_Battery				=0x4C03, // 1 byte, in 1 %
	MI_DATAF_ID_Humidity			=0x4C08, // float

	MI_DATA3_ID_Battery				=0x5403, // 1 byte, in 1 %

} MI_DATA_ID;

// All data little-endian
typedef struct __attribute__((packed)) _adv_xiaomi_t {
	u8	size;	// = ?
	u8	uid;	// = 0x16, 16-bit UUID
	u16	UUID;	// = 0xfe95, GATT Service
#if 0
	union { // Frame Control
		struct __attribute__((packed)) {
			u16 Factory: 		1; //0001 reserved text
			u16 Connected: 	1; //0002 reserved text
			u16 Central: 		1; //0004 Keep
			u16 isEncrypted: 	1; //0008 0: The package is not encrypted; 1: The package is encrypted
			u16 MACInclude: 	1; //0010 0: Does not include the MAC address; 1: includes a fixed MAC address (the MAC address is included for iOS to recognize this device and connect)
			u16 CapabilityInclude: 	1;  //0020 0: does not include Capability; 1: includes Capability. Before the device is bound, this bit is forced to 1
			u16 ObjectInclude:	1; //0040 0: does not contain Object; 1: contains Object
			u16 Mesh: 			1; //0080 0: does not include Mesh; 1: includes Mesh. For standard BLE access products and high security level access, this item is mandatory to 0. This item is mandatory for Mesh access to 1.
			u16 registered:	1; //0100 0: The device is not bound; 1: The device is registered and bound.
			u16 solicited:		1; //0200 0: No operation; 1: Request APP to register and bind. It is only valid when the user confirms the pairing by selecting the device on the developer platform, otherwise set to 0. The original name of this item was bindingCfm, and it was renamed to solicited "actively request, solicit" APP for registration and binding
			u16 AuthMode:		2; //0c00 0: old version certification; 1: safety certification; 2: standard certification; 3: reserved
			u16 version:		4; //f000 Version number (currently v5)
		} b; // bits
		u16	word;	// Frame Control
	} ctrID; // Frame Control
#else
	u16		ctrID;	// Frame Control
#endif
	u16	devID;
	u8	counter;
	u8	MAC[6]; // [0] - lo, .. [6] - hi digits
	union __attribute__((packed)) {
		u8	cap;	// Capability
		u8	data[3]; //
	};
} adv_xiaomi_t, * padv_xiaomi_t; // [18]

/* Encrypted mijia beacon structs */
typedef struct __attribute__((packed)) _mi_beacon_nonce_t{
    u8  mac[6];
	u16 pid;
	union {
		struct {
			u8  cnt;
			u8  ext_cnt[3];
		};
		u32 cnt32;
    };
} mi_beacon_nonce_t, * pmi_beacon_nonce_t;


typedef struct __attribute__((packed)) _adv_struct_xiaomi_t {
	u16	id;
	u8	size;
	union __attribute__((packed)) {
		u8		data_ub[4];
		u16		data_us[2];
		u32		data_uw;
		s8		data_ib[4];
		s32		data_is[2];
		s32		data_iw;
	};
} adv_struct_xiaomi_t, * padv_struct_xiaomi_t;




#endif /* _MIHOME_ADV_H_ */
