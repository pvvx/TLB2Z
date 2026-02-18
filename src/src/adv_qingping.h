/*
 * qingping_adv.h
 *
 *  Created on: 21.03.2024
 *      Author: pvvx
 */

#ifndef _QINGPING_ADV_H_
#define _QINGPING_ADV_H_

#define ADV_QINGPING_UUID16		0xfdcd
//=================== Qingping =================
// GATT Service 0xfdcd Qingping Technology (Beijing) Co., Ltd.
// All data little-endian

typedef struct __attribute__((packed)) _adv_qingping_t {
	u8	size;	// = ?
	u8	uid;	// = 0x16, 16-bit UUID
	u16	UUID;	// = 0xfdcd, GATT Service
	u8 	hlen;	// bit0..5 - strut size, bit6 - Event
	u8 	device_id;
	u8	MAC[6]; // [0] - lo, .. [6] - hi digits
	u8	data[2]; // 2 - for check min_length
} adv_qingping_t, * padv_qingping_t;

typedef struct __attribute__((packed)) _adv_struct_qingping_t {
	union __attribute__((packed)) {
		struct __attribute__((packed)) {
			u8	id;
			u8 	size;
		};
		u16	id_size;
	};
	union __attribute__((packed)) {
		u8	data_ub[4];
		u16	data_us[2];
		u32	data_uw;
		s8	data_ib[4];
		s16	data_is[2];
		s32	data_iw;
	};
} adv_struct_qingping_t, * padv_struct_qingping_t;

#endif /* _QINGPING_ADV_H_ */
