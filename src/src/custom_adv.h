/*
 * custom_adv.h
 *
 *  Created on: 21.03.2024
 *      Author: pvvx
 */

#ifndef _CUSTOM_ADV_H_
#define _CUSTOM_ADV_H_

//=================== Custom =================

#define ADV_CUSTOM_UUID16	0x181A
// GATT Service 0x181A Environmental Sensing
// All data little-endian

// GPIO_TRG pin (marking "reset" on circuit board) flags:
typedef struct __attribute__((packed)) _trigger_flg_t {
	u8 	rds_input	:	1; // Reed Switch, input
	u8 	trg_output	:	1; // GPIO_TRG pin output value (pull Up/Down)
	u8 	trigger_on	:	1; // Output GPIO_TRG pin is controlled according to the set parameters threshold temperature or humidity
	u8 	temp_event	:	1; // Temperature trigger event
	u8 	humi_event	:	1; // Humidity trigger event
}trigger_flg_t;

typedef struct __attribute__((packed)) _adv_custom_t {
	u8	size;	// = 18
	u8	uid;	// = 0x16, 16-bit UUID
	u16	UUID;	// = 0x181A, GATT Service 0x181A Environmental Sensing
	u8	MAC[6]; // [0] - lo, .. [6] - hi digits
	s16	temperature; // x 0.01 degree
	u16	humidity; // x 0.01 %
	u16	battery_mv; // mV
	u8	battery_level; // 0..100 %
	u8	counter; // measurement count
	trigger_flg_t	flags;
} adv_custom_t, * padv_custom_t;

/* Encrypted pvvx beacon structs */
typedef struct __attribute__((packed)) _adv_cust_head_t {
	u8	size;		//@0 = 11
	u8	uid;		//@1 = 0x16, 16-bit UUID https://www.bluetooth.com/specifications/assigned-numbers/generic-access-profile/
	u16	UUID;		//@2..3 = GATT Service 0x181A Environmental Sensing (little-endian) (or 0x181C 'User Data'?)
	u8	counter;	//@4 0..0xff Measurement count, Serial number, used for de-duplication, different event or attribute reporting requires different Frame Counter
} adv_cust_head_t, * padv_cust_head_t;

typedef struct __attribute__((packed)) _adv_cust_data_t {
	s16	temp;		//@0
	u16	humi;		//@2
	u8	bat;		//@4
	trigger_flg_t	flags;	//@5
} adv_pvvx_data_t, * padv_pvvx_data_t;

typedef struct __attribute__((packed)) _adv_pvvx_enc_t {
	adv_cust_head_t head;	//@0
	adv_pvvx_data_t data;   //@5
	u8	mic[4];		//@11
} adv_pvvx_enc_t, * padv_pvvx_enc_t;

/* Encrypted atc beacon structs
 * https://github.com/pvvx/ATC_MiThermometer/issues/94#issuecomment-842846036 */
typedef struct __attribute__((packed)) _adv_atc_data_t {
	u8	temp;		//@0
	u8	humi;		//@1
	u8	bat;		//@2
} adv_atc_data_t, * padv_atc_data_t;

typedef struct __attribute__((packed)) _adv_atc_enc_t {
	adv_cust_head_t head;	//@0
	adv_atc_data_t data;	//@5
	u8	mic[4];		//@8..11
} adv_atc_enc_t, * padv_atc_enc_t;

/* Encrypted atc/custom nonce */
typedef struct __attribute__((packed)) _enc_beacon_nonce_t{
    u8  MAC[6];
    adv_cust_head_t head;
} enc_beacon_nonce_t;




#endif /* _CUSTOM_ADV_H_ */
