/*
 * scaning.c
 *
 *  Created on: 20.11.2021
 *      Author: pvvx
 */
#include "tl_common.h"
#include "device.h"
#include "stack/ble/ble.h"
#include "ble_cfg.h"
#include "scaning.h"
#include "sensors.h"
#include "ccm.h"
#include "drv_uart.h"
#include "bthome_adv.h"
#include "custom_adv.h"
#include "mihome_adv.h"
#include "qingping_adv.h"
#include "app_ui.h"


//#include "cmd_parser.h"
//#include "app.h"

#if (ADV_SERVICE_ENABLE)
#define MYFIFO_BLK_SIZE		40 // min 1+6+31 = 38 bytes
#if (MTU_DATA_SIZE < MYFIFO_BLK_SIZE)
#error "MTU_DATA_SIZE < MYFIFO_BLK_SIZE !"
#endif
MYFIFO_INIT(ad_fifo, MYFIFO_BLK_SIZE, 8); 	// 40*8 = 320 bytes + sizeof(my_fifo_t)
#endif

typedef struct __attribute__((packed)) _ad_uuid16_t {
	u8 size;
	u8 type;
	u16 uuid16;
    u8 data[1]; // 1 - for check min length
} ad_uuid16_t, * pad_uuid16_t;

typedef struct __attribute__((packed)) _ad_head_t {
    u8 len;
    u8 type;
    u16 uuid16;
    u8 data[1]; // 1 - for check min_length
} ad_head_t, * pad_head_t;

//u32 off_tisk_th;
//u32 off_tisk_lm;


// BTHome v2
// bit[4:5]: 0 - 1, 1 - 0.1, 2 - 0.01, 3 - 0.001
// bit[6]: bool (on/off)
// bit[7]: signed
// 0x - unsigned 1
// 1x - unsigned 0.1
// 2x - unsigned 0.01
// 3x - unsigned 0.001
// 8x - signed 1
// 9x - signed 0.1
// Ax - signed 0.01
// Bx - signed 0.001

const u8 tblBTHome[] = {
//  0     1     2     3     4     5     6     7     8     9   	a     b     c     d     e     f
	0x01, 0x01, 0xA2, 0x21, 0x23, 0x23, 0x22, 0x22, 0xA2, 0x01, 0x33, 0x23, 0x32, 0x02, 0x02, 0x41, // 0x
	0x41, 0x41, 0x02, 0x02, 0x32, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, // 1x
	0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x01, 0x01, // 2x
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x02, 0x04, 0x12, // 3x
	0x02, 0x12, 0x33, 0x32, 0x22, 0x12, 0x11, 0x12, 0x02, 0x32, 0x12, 0x33, 0x34, 0x34, 0x34, 0x34, // 4x
	0x04, 0x32, 0x32, 0x0f, 0x0f, 0x34, 0x02, 0x81, 0x81, 0x81, 0x82, 0x84, 0xA4, 0xB2, 0x22, 0x22, // 5x
	0x81 // 6x
/*
	0xF0	device type id	uint16 (2 bytes)	F00100	1
	0xF1	firmware version	uint32 (4 bytes)	F100010204	4.2.1.0
	0xF2	firmware version	uint24 (3 bytes)	F1000106	6.1.0
*/
};

#if	USE_DEBUG_UART
u8 debug_buf[48];
#endif
u8 prev_advs[MAX_SCAN_DEVS][32];

#if USE_BINDKEY
const u8 ccm_aad = 0x11;
u8 bindkey[MAX_SCAN_DEVS][16]; /* = {
		//{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		// 401fabb39c8c0f65b88fdbf9b60d1759
		{0x40,0x1f,0xab,0xb3,0x9c,0x8c,0x0f,0x65,0xb8,0x8f,0xdb,0xf9,0xb6,0x0d,0x17,0x59 },
		// A4:C1:38:C5:B1:7B
		{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16},
		// 36f668a98f9c5aaf953ec04db093a5d9
		{0x36,0xf6,0x68,0xa9,0x8f,0x9c,0x5a,0xaf,0x95,0x3e,0xc0,0x4d,0xb0,0x93,0xa5,0xd9}
}; // for MAC1 */
#endif

u8	update_enable[MAX_SCAN_DEVS];

u8	dev_MAC[MAX_SCAN_DEVS][6]; /* = {
//		{0x27,0xdb,0x64,0x38,0xc1,0xa4}, // A4C13864DB27

		{0x74,0xf0,0x19,0x38,0xc1,0xa4}, // A4:C1:38:19:F0:74  A4C13819F074
		// A4:C1:38:C5:B1:7B
		{0x7b,0xb1,0xc5,0x38,0xc1,0xa4},	//  e7 2e 01 83 48 6e
		//{0x6e,0x48,0x83,0x01,0x2e,0xe7},	//  e7 2e 01 83 48 6e
		{0xf7,0x7a,0x25,0x65,0x11,0x2c}	// 2C:11:65:25:7A:F7 // A4C138C5B17B
}; // [0] - lo, .. [6] - hi digits
*/

// sz         ctrID devID cnt     mac
// 1a 16 95fe 5858  5b05  40  74f01938c1a4 a1d0d76485 5a0900 cc74485c
// 16 16 95FE 4859  0312  72  01 4C 04 CD CC B0 41 5F0700 E9A442FD

#define BIAS 127
#define K 8
#define N 23
/* conversion float to u16 or i16 mul 100 */
int float_pf2i_x100(u8 * pf) {
  unsigned f = (pf[3]<<24)|(pf[2]<<16)|(pf[1]<<8)|pf[0];
  unsigned s = f >> (K + N);
  unsigned exp = (f >> N) & 0xFF;
  unsigned frac = f & 0x7FFFFF;

  /* Denormalized values round to 0 */
  if (exp == 0)
    return 0;
  /* f is NaN */
  if (exp == 0xFF)
    return 32768;
  /* Normalized values */
  int x;
  int E = exp - BIAS + 7; // * 128
  /* Normalized value less than 0, return 0 */
  if (E < 0)
    return 0;
  /* Overflow 16 bit condition */
  if (E > 15)
    return 32768;
  x = 1 << E;
  //if (E < N)
    x |= frac >> (N - E);
  //else
  //  x |= frac << (E - N);
  // x div 1.28
  x *= 51200;
  x += 0x8000; // round
  x >>= 16;
  /* Negative values */
  if (s == 1)
    x = ~x + 1;
  return x;
}

//u8 x_buf[16];
__attribute__((optimize("-Os")))
void filter_xiaomi_ad(padv_xiaomi_t p, int n) {

	int len = p->size;
	u8 * pb;
	u8 * pmac = dev_MAC[n];
	if(len > sizeof(adv_xiaomi_t) - 6) {
		if(p->ctrID & 0x0010) { // MAC presents
			len -= sizeof(adv_xiaomi_t) - 4;
			if(memcmp(p->MAC, pmac, 6))
				return;
			//pmac = p->MAC;
			pb = &p->cap;
		} else if(len > sizeof(adv_xiaomi_t)) {
			len -= sizeof(adv_xiaomi_t) - 4 - 6;
			pb = p->MAC;
		} else
			return;
		if(((p->ctrID & (0x40 | 0x80)) == 0x40)) { // No Mesh, Data presents
			if(p->ctrID & 0x20) { // includes Capability
				if (*pb++ & 0x20) {
					pb += 2;
					len -= 2;
				}
				len--;
			}
#if USE_BINDKEY
			if((p->ctrID & 0x08) && len > 3 + 3 + 4) { // Data encrypted, len > size (min_data[3], ext_cnt[3], mic[4])
#if	USE_DEBUG_UART
				debug_buf[0] = n;
#endif
/*
				reg_clk_en1 |= FLD_CLK1_AES_EN;
				reg_rst1 |= (FLD_RST1_AES);
				reg_rst1 &= ~(FLD_RST1_AES);
*/
				u8 * pbkey = bindkey[n];
				mi_beacon_nonce_t mi_beacon_nonce;
				memcpy(mi_beacon_nonce.mac, pmac, sizeof(mi_beacon_nonce.mac));
				len -= 3 + 4; // - size (ext_cnt[3], mic[4])
				mi_beacon_nonce.pid = p->devID;
				mi_beacon_nonce.cnt = p->counter;
				mi_beacon_nonce.ext_cnt[0] = pb[len];
				mi_beacon_nonce.ext_cnt[1] = pb[len+1];
				mi_beacon_nonce.ext_cnt[2] = pb[len+2];
//				memset(x_buf, 0, sizeof(x_buf));
//				memcpy(x_buf, pb, len);
				if(aes_ccm_auth_decrypt((const unsigned char *)pbkey,
						(u8*)&mi_beacon_nonce, sizeof(mi_beacon_nonce),
						&ccm_aad, sizeof(ccm_aad),
//						x_buf, len, // crypt_data
						pb, len, // crypt_data
						pb, // decrypt data
						(u8 *)&pb[len+3], 4)) { // &mic: &crypt_data[len + size (ext_cnt[3])]
					return;
				}
#if	USE_DEBUG_UART
				memcpy(&debug_buf[1], pb, len);
				uart_send(debug_buf, len + 1);
#endif
			}
#else
			if(p->ctrID & 0x08)
					return;
#endif
			// send_debug(pb, len);
			padv_struct_xiaomi_t ps = (padv_struct_xiaomi_t)pb;
			while(ps->size + 3 <= len) {
				if(ps->id == MI_DATA_ID_TempAndHumidity && ps->size >= 4) { // Temp + Humi
					g_zcl_temperatureAttrs.measuredValue[n] = ps->data_is[0]*10; // in 0.1 C
					g_zcl_relHumidityAttrs.measuredValue[n] = ps->data_us[1]*10;  // in 0.1 %
					update_enable[n] |= FLG_UPDATE_TEMP | FLG_UPDATE_HUMI | FLG_UPDATE_FLG;
				} else if(ps->size >= 1 && (ps->id == MI_DATA_ID_Power
						|| ps->id == MI_DATA1_ID_Battery
						|| ps->id == MI_DATA2_ID_Battery
						|| ps->id == MI_DATA3_ID_Battery)) { // Battery
					g_zcl_powerAttrs[n].batteryVoltage = 30;
					g_zcl_powerAttrs[n].batteryPercentage = ps->data_ub[0] << 1;
					update_enable[n] |= FLG_UPDATE_BAT | FLG_UPDATE_VBAT | FLG_UPDATE_FLG;
				} else if(ps->id == MI_DATA_ID_Temperature && ps->size >= 2) { // Temperature
					g_zcl_temperatureAttrs.measuredValue[n] = ps->data_is[0]*10; // in 0.1 C
					update_enable[n] |= FLG_UPDATE_TEMP | FLG_UPDATE_FLG;
				} else if(ps->id == MI_DATA_ID_SwitchTemperature && ps->size >= 2) { // Temperature
					g_zcl_temperatureAttrs.measuredValue[n] = ps->data_ib[1]*100; // in 1 C
					update_enable[n] |= FLG_UPDATE_TEMP | FLG_UPDATE_FLG;
				} else if(ps->id == MI_DATAF_ID_Temperature && ps->size >= 4) { // Temperature, float
					g_zcl_temperatureAttrs.measuredValue[n] = float_pf2i_x100(ps->data_ub);
					update_enable[n] |= FLG_UPDATE_TEMP | FLG_UPDATE_FLG;
				} else if(ps->id == MI_DATAF_ID2_Temperature && ps->size >= 4) { // Temperature, float
					g_zcl_temperatureAttrs.measuredValue[n] = float_pf2i_x100(ps->data_ub);
					update_enable[n] |= FLG_UPDATE_TEMP | FLG_UPDATE_FLG;

				} else if(ps->id == MI_DATA_ID_Humidity && ps->size >= 2) { // Humidity
					g_zcl_relHumidityAttrs.measuredValue[n] = ps->data_us[0]*10;  // in 0.1 %
					update_enable[n] |= FLG_UPDATE_HUMI | FLG_UPDATE_FLG;
				} else if(ps->id == MI_DATA1_ID_Humidity && ps->size >= 1) { // Humidity, byte
					g_zcl_relHumidityAttrs.measuredValue[n] = ps->data_ib[1]*100; // in 1 %
					update_enable[n] |= FLG_UPDATE_HUMI | FLG_UPDATE_FLG;
				} else if(ps->id == MI_DATAF_ID_Humidity && ps->size >= 4) { // Humidity, float
					g_zcl_relHumidityAttrs.measuredValue[n] = float_pf2i_x100(ps->data_ub);
					update_enable[n] |= FLG_UPDATE_HUMI | FLG_UPDATE_FLG;
				} else if(ps->id == MI_DATAF_ID2_Humidity && ps->size >= 4) { // Humidity, float
					g_zcl_relHumidityAttrs.measuredValue[n] = float_pf2i_x100(ps->data_ub);
					update_enable[n] |= FLG_UPDATE_HUMI | FLG_UPDATE_FLG;
				}
/*
				 else if((ps->id == MI_DATA_EV_Motion)&&(ps->size >= 1)) { // Motion
					set_lm_out(ps->data_ub[0]);
				} else if((ps->id == MI_DATA_EV_MovingWithLight)&&(ps->size >= 3)) { // Moving With Light 0f0003 540f00 / 0f0003 620e00
					if(ps->data_ub[2])
						wrk.illuminance = -1;
					else
						wrk.illuminance = ps->data_us[0];
					wrk.motion_event = 1;
					set_lm_out(1);
				} else if((ps->id == MI_DATA_ID_LightIlluminance)&&(ps->size >= 3)) { // Light Illuminance 071003 af1500
					if(ps->data_ub[2])
						wrk.illuminance = -1;
					else
						wrk.illuminance = ps->data_us[0];
					set_lm_out(0);
				} else if((ps->id == MI_DATA_ID_NoOneMoves)&&(ps->size >= 4)) { // No one moves over time / 171004 3c000000 / 171004 78000000 / 1710042c010000 / 171004 58020000
					wrk.motion_event = ps->data_uw == 0;
					set_lm_out(wrk.motion_event);
				} else if((ps->id == MI_DATA_ID_LightIntensity)&&(ps->size >= 1)) { // Light on/off, Light Intensity 18100101 / 18100100
					wrk.light_on = ps->data_ub[0];
				}
*/
				len -= ps->size + 3;
				ps = (padv_struct_xiaomi_t)((u32)ps + ps->size + 3);
			}
		}
	}
}

// e72e0183486e 020106 03021a18 131695fe70205b043a6e4883012ee7090a100117
// e72e0183486e 02010603021a18141695fe70205b04376e4883012ee709041002e400
//_attribute_ram_code_
__attribute__((optimize("-Os")))
void filter_qingping_ad(padv_qingping_t p, int n) {
	padv_struct_qingping_t ps = (padv_struct_qingping_t) &p->data;
	int len = p->size;
	// ..0812 005E60342D58 0201 64 0F01 7D 0904 8C120000
	if(len > sizeof(adv_qingping_t) && (p->hlen & 0x1f) == 0x08) {
		len -= 11;
		while((ps->size + 2) <= len) {
			if(ps->id_size == 0x0401) { // Temp + Humi
				g_zcl_temperatureAttrs.measuredValue[n] = ps->data_is[0]*10; // in 0.1 C
				g_zcl_relHumidityAttrs.measuredValue[n] = ps->data_is[1]*10;  // in 0.1 %
				update_enable[n] |= FLG_UPDATE_HUMI | FLG_UPDATE_TEMP | FLG_UPDATE_FLG;
			} else if(ps->id_size == 0x0102) { // Batt %
				g_zcl_powerAttrs[n].batteryPercentage = ps->data_us[0];  // in %
				g_zcl_powerAttrs[n].batteryVoltage = 30;
				update_enable[n] |= FLG_UPDATE_BAT | FLG_UPDATE_VBAT | FLG_UPDATE_FLG;
/*
			else if(ps->id_size == 0x0408) { // Motion + Light
				wrk.motion_event = ps->data_ub[0];
				if(ps->data_ub[3])
					wrk.illuminance = -1;
				else
					wrk.illuminance = ps->data_uw >> 8;
				set_lm_out(wrk.motion_event);
			} else if(ps->id_size == 0x0409) { // Light
				if(ps->data_ub[2])
					wrk.illuminance = -1;
				else
					wrk.illuminance = ps->data_us[0];
				set_lm_out(0);
			} else if(ps->id_size == 0x0111) { // Light on/off
				wrk.light_on = ps->data_ub[0];
*/
				//				} else if(ps->id_size == 0x0207) { // Pressure
				//					extdev_pressure = ps->data_us[0];  // in 0.01
				//				} else if(ps->id_size == 0x010f) { // Count
			}
			len -= ps->size + 2;
			ps = (padv_struct_qingping_t)((u32)ps + ps->size + 2);
		}
	}
}

//_attribute_ram_code_
__attribute__((optimize("-Os")))
void filter_custom_ad(adv_custom_t *p, int n) {
	if(p->size == sizeof(adv_custom_t) - 1) {
		g_zcl_temperatureAttrs.measuredValue[n] = p->temperature;
		g_zcl_relHumidityAttrs.measuredValue[n] = p->humidity;
		g_zcl_powerAttrs[n].batteryPercentage = p->battery_level << 1;
		g_zcl_powerAttrs[n].batteryVoltage = p->battery_mv/100;
		update_enable[n] |= FLG_UPDATE_BAT | FLG_UPDATE_VBAT | FLG_UPDATE_TEMP | FLG_UPDATE_HUMI | FLG_UPDATE_FLG;
		//wrk.flg.rds_output = p->flags.rds_input;
		//new_trg = p->flags.trg_output;
		//new_rds = p->flags.rds_input;
		//gpio_write(GPIO_OUT_TH, p->flags.trg_output);
		//gpio_write(GPIO_OUT_RDS, p->flags.rds_input);
	}
#if USE_BINDKEY
	else if(p->size == sizeof(adv_pvvx_enc_t) - 1) {
		enc_beacon_nonce_t beacon_nonce;
		// unsigned int len = p->size - 4 - (sizeof(adv_cust_head_t)-1); // - mic[4] - head
		padv_pvvx_enc_t pp = (padv_pvvx_enc_t) p;
		memcpy(beacon_nonce.MAC, dev_MAC[n], sizeof(beacon_nonce.MAC));
		beacon_nonce.head = pp->head; // memcpy(&beacon_nonce.head, pp->head, sizeof(adv_cust_head_t));
		// u8 decrypt_data[16];
		if(aes_ccm_auth_decrypt((const unsigned char *)&bindkey[n],
				(u8*)&beacon_nonce, sizeof(beacon_nonce),
				&ccm_aad, sizeof(ccm_aad),
				(u8 *)&pp->data, sizeof(adv_pvvx_data_t), // len crypt_data
				(u8 *)&pp->data, // decrypt data
				(u8 *)&pp->mic, 4)) // &mic: &crypt_data[len + size (ext_cnt[3])]
			return;
#if	USE_DEBUG_UART
		debug_buf[0] = n;
		memcpy(&debug_buf[1], &pp->data, sizeof(adv_pvvx_data_t));
		uart_send(debug_buf, sizeof(adv_pvvx_data_t) + 1);
#endif

		//ps = (padv_struct_xiaomi_t)&decrypt_data;
		g_zcl_temperatureAttrs.measuredValue[n] = pp->data.temp;
		g_zcl_relHumidityAttrs.measuredValue[n] = pp->data.humi;
		g_zcl_powerAttrs[n].batteryPercentage = pp->data.bat << 1;
		g_zcl_powerAttrs[n].batteryVoltage = 30;
		update_enable[n] |= FLG_UPDATE_BAT | FLG_UPDATE_VBAT | FLG_UPDATE_TEMP | FLG_UPDATE_HUMI | FLG_UPDATE_FLG;
		//wrk.flg.rds_output = pp->data.flags.rds_input;
		//new_trg = pp->data.flags.trg_output;
		//new_rds = pp->data.flags.rds_input;
		//gpio_write(GPIO_OUT_TH, p->flags.trg_output);
		//gpio_write(GPIO_OUT_RDS, p->flags.rds_input);
	}
#endif
}

__attribute__((optimize("-Os")))
void filter_bthome_ad(padv_bthome_t p, int n) {
	int len = p->size;
	if(len > sizeof(padv_bthome_t)) {
		len -= sizeof(adv_bthome_t) - 2; // p->data len
		if(p->ver == BtHomeID_ver_encrypt && len > 9) {
#if USE_BINDKEY
			len -= 8;
			bthome_beacon_nonce_t bthome_nonce;
			bthome_nonce.mac[5] = dev_MAC[n][0];
			bthome_nonce.mac[4] = dev_MAC[n][1];
			bthome_nonce.mac[3] = dev_MAC[n][2];
			bthome_nonce.mac[2] = dev_MAC[n][3];
			bthome_nonce.mac[1] = dev_MAC[n][4];
			bthome_nonce.mac[0] = dev_MAC[n][5];
			//memcpy(bthome_nonce.mac, dev_MAC[n], sizeof(bthome_nonce.mac));
			u8 *pb = (u8 *)&bthome_nonce.uuid16;
			u8 *pmic = (u8 *)&p->UUID;
			// UUID16, ver
			*pb++ = *pmic++;
			*pb++ = *pmic++;
			*pb++ = *pmic;
			// count32
			pmic = &p->data[len];
			*pb++ = *pmic++;
			*pb++ = *pmic++;
			*pb++ = *pmic++;
			*pb = *pmic++; // pm = &mic[4]
			if(aes_ccm_auth_decrypt((const unsigned char *)bindkey[n],
					(u8 *)&bthome_nonce, sizeof(bthome_nonce),
					NULL, 0,
					p->data, len, // len crypt_data
					p->data, // decrypt data
					pmic, 4))  // &mic: &crypt_data[len + size (ext_cnt[3])]
#endif
				return;
		} else if(p->ver != BtHomeID_ver) {
			return;
		}
		padv_bthome_sruct_t ps = (padv_bthome_sruct_t)&p->data;
		while(len > 0) {
			if(ps->type < sizeof(tblBTHome)) {
				if(ps->type == BtHomeID_temperature) {
					g_zcl_temperatureAttrs.measuredValue[n] = ps->data_is[0]; // in 0.01 C
					update_enable[n] |= FLG_UPDATE_TEMP | FLG_UPDATE_FLG;
				} else if(ps->type == BtHomeID_temperature_01) {
					g_zcl_temperatureAttrs.measuredValue[n] = ps->data_is[0]*10; // in 0.1 C
					update_enable[n] |= FLG_UPDATE_TEMP | FLG_UPDATE_FLG;
				} else if(ps->type == BtHomeID_temperature035) {
					g_zcl_temperatureAttrs.measuredValue[n] = ps->data_ib[0]*35; // in 0.35 C
					update_enable[n] |= FLG_UPDATE_TEMP | FLG_UPDATE_FLG;
				} else if(ps->type == BtHomeID_temperature8) {
					g_zcl_temperatureAttrs.measuredValue[n] = ps->data_ib[0]*100; // in 1 C
					update_enable[n] |= FLG_UPDATE_TEMP | FLG_UPDATE_FLG;
				} else if(ps->type == BtHomeID_humidity) {
					g_zcl_relHumidityAttrs.measuredValue[n] = ps->data_us[0]; // in 0.01 %
					update_enable[n] |= FLG_UPDATE_HUMI | FLG_UPDATE_FLG;
				} else if(ps->type == BtHomeID_humidity8) {
					g_zcl_relHumidityAttrs.measuredValue[n] = ps->data_ub[0]*100; // in 1 %
					update_enable[n] |= FLG_UPDATE_HUMI | FLG_UPDATE_FLG;
				} else if(ps->type == BtHomeID_battery) { // Batt %
					g_zcl_powerAttrs[n].batteryPercentage = ps->data_ub[0];  // in 1%
					update_enable[n] |= FLG_UPDATE_BAT | FLG_UPDATE_FLG;
				} else if(ps->type == BtHomeID_voltage) {
					g_zcl_powerAttrs[n].batteryVoltage = ps->data_us[0]/100; // in 0.001V
					update_enable[n] |= FLG_UPDATE_VBAT | FLG_UPDATE_FLG;
				}

			} else
					break;
			int size = (tblBTHome[ps->type] & 0x0f) + 1;
			if(size == 0x10)
				size = ps->data_ub[0] + 2;
			len -= size;
			ps = (padv_bthome_sruct_t)((u32)ps + size);
		}
	}
}


//////////////////////////////////////////////////////////
// scan event call back
//////////////////////////////////////////////////////////
//_attribute_ram_code_
__attribute__((optimize("-Os")))
int scanning_event_callback(u32 h, u8 *p, int n) {
	if (h & HCI_FLAG_EVENT_BT_STD) { // ble controller hci event
		if ((h & 0xff) == HCI_EVT_LE_META) {
			//----- hci le event: le adv report event -----
			// send_debug(p, 10);
#if (EXTENDED_ADV_ENABLE)
			if ((p[0] == HCI_SUB_EVT_LE_EXTENDED_ADVERTISING_REPORT) //{ // Ext ADV packet?
			|| (p[0] == HCI_SUB_EVT_LE_PERIODIC_ADVERTISING_REPORT)
			|| (p[0] == HCI_SUB_EVT_LE_ADVERTISING_REPORT))
#else
			if (p[0] == HCI_SUB_EVT_LE_ADVERTISING_REPORT)
#endif
			{ // ADV packet
				//after controller is set to scan state, it will report all the adv packet it received by this event
				event_adv_report_t *pa = (event_adv_report_t *) p;
				u32 adlen = pa->len;
				u8 rssi = pa->data[adlen];
				if (adlen && adlen < 32 && rssi != 0) { // rssi != 0
#if 0 // =1 send all adv packets
#if (ADV_SERVICE_ENABLE)
								advDataValue++;
								if (advDataCCC // Notify on?
										&& (blc_ll_getCurrentState() & BLS_LINK_STATE_CONN)) {
									u8 *s = my_fifo_wptr(&ad_fifo);
									if(s) {
										s[0] = adlen + 1 + 6;
										s[1] = pa->subcode; //rssi;
										memcpy(s + 2, pa->mac, 6);
										memcpy(s + 2 + 6, pa->data, adlen);
										my_fifo_next(&ad_fifo);
									}
								}
#endif
#else
					u32 i = 0;
					while(adlen) {
						pad_uuid16_t pd = (pad_uuid16_t)&pa->data[i];
						u32 len = pd->size + 1;
						if(len <= adlen) {
							if(len >= sizeof(ad_uuid16_t) && pd->type == GAP_ADTYPE_SERVICE_DATA_UUID_16BIT) {
#if (ADV_SERVICE_ENABLE)
								advDataValue++;
								if (advDataCCC // Notify on?
										&& (blc_ll_getCurrentState() & BLS_LINK_STATE_CONN)) {
									u8 *p = my_fifo_wptr(&ad_fifo);
									if(p) {
										p[0] = len + 1 + 6;
										p[1] = rssi;
										memcpy(p + 2, pa->mac, 6);
										memcpy(p + 2 + 6, pd, len);
										my_fifo_next(&ad_fifo);
									}
								}
#endif
								for(int n = 0; n < MAX_SCAN_DEVS; n++) {
									if (memcmp(dev_MAC[n], pa->mac, 6) == 0) {
										if(memcmp(prev_advs[n], pd, len)) {
											memcpy(prev_advs[n], pd, len);
											if((pd->uuid16) == ADV_CUSTOM_UUID16) { // GATT Service 0x181A Environmental Sensing, ATC custom FW
												filter_custom_ad((adv_custom_t *)prev_advs[n], n);
											} else if((pd->uuid16) == ADV_BTHOME_UUID16) { // GATT Service: BTHome v2
												filter_bthome_ad((adv_bthome_t *)prev_advs[n], n);
											} else if((pd->uuid16) == ADV_MIHOME_UUID16) { // GATT Service: Xiaomi Inc.
												filter_xiaomi_ad((adv_xiaomi_t *)prev_advs[n], n);
											} else if((pd->uuid16) == ADV_QINGPING_UUID16) { // GATT Service: Qingping Technology (Beijing) Co., Ltd.
												filter_qingping_ad((adv_qingping_t *)prev_advs[n], n);
											}
											switch(n) {
											case 0:
												LED_R_ON();
												break;
											case 1:
												LED_G_ON();
												break;
											case 2:
												LED_B_ON();
												break;
											}
											break;
										}
									};
								}
							}
						} else
							break;
						adlen -= len;
						i += len;
					}
#endif
				}
			}
		}
	}
	return 0;
}

//////////////////////////////////////////////////////////
// scan task
//////////////////////////////////////////////////////////
#if 0
u32 tisk_scan_task;

#define	OUT_OFF_TIMEOUT	(15*60) // 15 minutes

//_attribute_ram_code_
__attribute__((optimize("-Os")))
void scan_task(void) {
	if(tisk_scan_task != utc_time_sec) {
		// new sec
		tisk_scan_task = utc_time_sec;
#if (SPP_SERVICE_ENABLE)
		if(memcmp(&adv_buf.wrk.data, &wrk, sizeof(wrk))) {
			set_adv_data();
			if (sppDataCCC // Notify on?
					&& (blc_ll_getCurrentState() & BLS_LINK_STATE_CONN)) { // Connect?
				sppDataBuffer[0] = CMD_ID_INFO;
				memcpy(&sppDataBuffer[1], &wrk, sizeof(wrk));
				bls_att_pushNotifyData(SPP_Server2Client_DP_H, (u8 *)&sppDataBuffer, sizeof(wrk) + 1);
			}
		}
#endif
	}
#if (ADV_SERVICE_ENABLE)
	u8 *p = my_fifo_get(&ad_fifo);
	if(p) {
		if (advDataCCC // Notify on?
				&& (blc_ll_getCurrentState() & BLS_LINK_STATE_CONN)) { // Connect?
			if(bls_att_pushNotifyData(ADV_DP_H, p+1, p[0]) == BLE_SUCCESS)
				my_fifo_pop(&ad_fifo);
		} else
			my_fifo_pop(&ad_fifo);
	}
#endif // ADV_SERVICE_ENABLE
}

#endif
