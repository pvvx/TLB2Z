/*
 * scaning.c
 *
 *  Created on: 20.11.2021
 *      Author: pvvx
 */
#include "tl_common.h"
#include "app.h"
#include "stack/ble/ble.h"
#include "ble_cfg.h"
#include "ble_scaning.h"
#include "sensors.h"
#include "ccm.h"
#include "drv_uart.h"
#include "adv_bthome.h"
#include "adv_custom.h"
#include "adv_mihome.h"
#include "adv_qingping.h"
#include "app_ui.h"

#if USE_DEBUG_PRINTF
#define USE_DEBUG_SCAN		2	// 0,1,2,3
#else
#define USE_DEBUG_SCAN		0	// 0,1,2,3
#endif

#define MAX_ADV_BUF_SIZE	32

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
	0x01, 0x01, 0xA2, 0x22, 0x23, 0x23, 0x22, 0x22, 0xA2, 0x01, 0x33, 0x23, 0x32, 0x02, 0x02, 0x41, // 0x
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

_attribute_custom_bss_
u8 prev_advs[MAX_SCAN_DEVS][MAX_ADV_BUF_SIZE];

#if USE_BINDKEY
const u8 ccm_aad = 0x11;
u8 bindkey[MAX_SCAN_DEVS][16];
#endif

u8 update_enable[MAX_SCAN_DEVS];
u8 dev_MAC[MAX_SCAN_DEVS][6];

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
#if	USE_DEBUG_SCAN > 1
  sws_printf("f: %d\n", x);
#endif
  return x;
}

static void zb_batteryVoltage(u8 bat, u8 n) {
	g_zcl_powerAttrs[n].batteryVoltage = (220 + bat) / 10;
	g_zcl_powerAttrs[n].batteryPercentage = bat << 1;
	update_enable[n] |= FLG_UPDATE_BAT | FLG_UPDATE_VBAT | FLG_UPDATE_FLG;
}

#ifdef ZCL_ILLUMINANCE_MEASUREMENT

u32 ble_illuminance[MAX_SCAN_DEVS] = {0xffffffff,0xffffffff,0xffffffff};

// Таблица 10000 * log10(i/10) для i = 10..100 (мантисса от 1.0 до 10.0 с шагом 0.1)
static const u16 log_table[91] = {
	   0,  414,	 792, 1139, 1461, 1761, 2041, 2304, 2553, 2788, // 1.0..1.9
	3010, 3222, 3424, 3617, 3802, 3979, 4150, 4314, 4472, 4624, // 2.0..2.9
	4771, 4914, 5052, 5185, 5315, 5441, 5563, 5682, 5798, 5911, // 3.0..3.9
	6021, 6128, 6232, 6335, 6435, 6532, 6628, 6721, 6812, 6902, // 4.0..4.9
	6990, 7076, 7160, 7243, 7324, 7404, 7482, 7559, 7634, 7709, // 5.0..5.9
	7782, 7853, 7924, 7993, 8062, 8129, 8195, 8261, 8325, 8388, // 6.0..6.9
	8451, 8513, 8573, 8633, 8692, 8751, 8808, 8865, 8921, 8976, // 7.0..7.9
	9031, 9085, 9138, 9191, 9243, 9294, 9345, 9395, 9445, 9494, // 8.0..8.9
	9542, 9590, 9638, 9685, 9731, 9777, 9823, 9868, 9912, 9956, // 9.0..9.9
   10000														  // 10.0
};

// Степени десяти: 10^0 .. 10^6 (последняя для проверки границы)
static const u32 powers[8] = {1, 10, 100, 1000, 10000, 100000, 1000000, 10000000};

/**
 * Вычисляет 10000 * log10(x) для 1 <= x <= 3576000
 * с использованием целочисленной арифметики и таблицы логарифмов.
 */
static u16 calk_10000_log10(u32 x) {
	if (x < 1)
		return 0;
	else if(x >= 3576000)
		return 65534;

	// Определяем порядок B: 10^B <= x < 10^(B+1)
	u32 B = 0;
	while (powers[B + 1] <= x) {
		B++;
	}
	u32 p = powers[B];	// 10^B

	// Масштабируем x для получения мантиссы с одним знаком после запятой
	u32 tmp = x * 10;		// x * 10
	u32 a = (int)(tmp / p);	// целая часть мантиссы *10 (от 10 до 99)
	u32 r = (int)(tmp % p);	// остаток для интерполяции

	u32 index = a - 10;		// индекс в таблице (0..89)
	u32 base = B * 10000;	// вклад порядка

	// Линейная интерполяция между соседними значениями таблицы
	u32 diff = log_table[index + 1] - log_table[index];
	// Округление: добавляем половину знаменателя перед делением
	u32 add = (diff * r + (p >> 1)) / p;

	return (u16)(base + log_table[index] + add);
}

static void zb_illuminance(u32 lx, u8 n) {
	ble_illuminance[n] = lx;
	g_zcl_illuminanceAttrs.measuredVal[n] = calk_10000_log10(lx);
#if	USE_DEBUG_SCAN > 1
	sws_printf("i: %d,%d\n", lx, g_zcl_illuminanceAttrs.measuredVal[n]);
#endif
	update_enable[n] |= FLG_UPDATE_TRG | FLG_UPDATE_LGH | FLG_UPDATE_FLG;
}

#endif // ZCL_ILLUMINANCE_MEASUREMENT


//u8 x_buf[16];
__attribute__((optimize("-Os")))
void filter_xiaomi_ad(padv_xiaomi_t p, int n) {

	int len = p->size;
	u8 * pb;
	u8 * pmac = dev_MAC[n];
	if(len > sizeof(adv_xiaomi_t) - 6) {
#if	USE_DEBUG_SCAN
		sws_printf("x[%d]: ", n);
		sws_print_hex_dump((u8 *)p, len + 1);
		sws_putchar('\n');
#endif
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
#if	USE_DEBUG_SCAN
					sws_puts("ccm err!\n");
#endif
					return;
				}
#if	USE_DEBUG_SCAN > 1
				sws_puts("x: ");
				sws_print_hex_dump(pb, len);
				sws_putchar('\n');
#endif
			}
#else
			if(p->ctrID & 0x08)
					return;
#endif
			padv_struct_xiaomi_t ps = (padv_struct_xiaomi_t)pb;
			while(ps->size + 3 <= len) {
				if(ps->id == MI_DATA_ID_TempAndHumidity && ps->size >= 4) { // Temp + Humi
					g_zcl_temperatureAttrs.measuredValue[n] = ps->data_is[0]*10; // in 0.1 C
					g_zcl_relHumidityAttrs.measuredValue[n] = ps->data_us[1]*10;  // in 0.1 %
					update_enable[n] |= FLG_UPDATE_TEMP | FLG_UPDATE_HUMI | FLG_UPDATE_FLG;
				 // Battery
				} else if(ps->size >= 1 && (ps->id == MI_DATA_ID_Power
						|| ps->id == MI_DATA1_ID_Battery
						|| ps->id == MI_DATA2_ID_Battery
						|| ps->id == MI_DATA3_ID_Battery)) { // Battery
					zb_batteryVoltage(ps->data_ub[0], n);
				 // Temperature
				} else if(ps->id == MI_DATA_ID_Temperature && ps->size >= 2) { // Temperature
					g_zcl_temperatureAttrs.measuredValue[n] = ps->data_is[0]*10; // in 0.1 C
					update_enable[n] |= FLG_UPDATE_TEMP | FLG_UPDATE_FLG;
				} else if(ps->id == MI_DATA_ID_SwitchTemperature && ps->size >= 2) { // Switch + Temperature
					g_zcl_temperatureAttrs.measuredValue[n] = ps->data_ib[1]*100; // in 1 C
					update_enable[n] |= FLG_UPDATE_TEMP | FLG_UPDATE_FLG;
				} else if((ps->id == MI_DATAF_ID_Temperature || ps->id == MI_DATAF_ID2_Temperature)
					&& ps->size >= 4) { // Temperature, float
					g_zcl_temperatureAttrs.measuredValue[n] = float_pf2i_x100(ps->data_ub);
					update_enable[n] |= FLG_UPDATE_TEMP | FLG_UPDATE_FLG;
				// Humidity
				} else if((ps->id == MI_DATA1_ID_Humidity || ps->id == MI_DATA1_ID2_Humidity)
					&& ps->size >= 1) { // Humidity, byte
					g_zcl_relHumidityAttrs.measuredValue[n] = ps->data_ub[0]*100; // in 1 %
					update_enable[n] |= FLG_UPDATE_HUMI | FLG_UPDATE_FLG;
				} else if(ps->id == MI_DATA_ID_SwitchHumidity && ps->size >= 2) { // Switch + Humidity
					g_zcl_relHumidityAttrs.measuredValue[n] = ps->data_us[1]*10;  // in 0.1 %
					update_enable[n] |= FLG_UPDATE_HUMI | FLG_UPDATE_FLG;
				} else if((ps->id == MI_DATAF_ID_Humidity || ps->id == MI_DATAF_ID2_Humidity)
					&& ps->size >= 4) { // Humidity, float
					g_zcl_relHumidityAttrs.measuredValue[n] = float_pf2i_x100(ps->data_ub);
					update_enable[n] |= FLG_UPDATE_HUMI | FLG_UPDATE_FLG;
#if SCAN_TRG_ENABLE
				// Motion
				} else if((ps->id == MI_DATA_EV_Motion)&&(ps->size >= 1)) { // Motion
					g_zcl_onOffAttrs.ble_trigger[n] = ps->data_ub[0];
					update_enable[n] |= FLG_UPDATE_TRG | FLG_UPDATE_FLG;
				} else if((ps->id == MI_DATA_EV_MovingWithLight)&&(ps->size >= 3)) { // Moving With Light 0f0003 540f00 / 0f0003 620e00
					g_zcl_onOffAttrs.ble_trigger[n] = 1;
#ifdef ZCL_ILLUMINANCE_MEASUREMENT
					zb_illuminance(ps->data_us[0] | (ps->data_ub[3] << 16), n);
#else
					update_enable[n] |= FLG_UPDATE_TRG | FLG_UPDATE_FLG;
#endif
				} else if((ps->id == MI_DATA_ID_NoOneMoves)&&(ps->size >= 4)) { // No one moves over time / 171004 3c000000 / 171004 78000000 / 1710042c010000 / 171004 58020000
					g_zcl_onOffAttrs.ble_trigger[n] = 0;
#ifdef ZCL_ILLUMINANCE_MEASUREMENT
					zb_illuminance(ps->data_us[0] | (ps->data_ub[3] << 16), n);
#else
					update_enable[n] |= FLG_UPDATE_TRG | FLG_UPDATE_FLG;
#endif

#endif
#ifdef ZCL_ILLUMINANCE_MEASUREMENT
				} else if((ps->id == MI_DATA_ID_LightIlluminance) && ps->size >= 3) { // Light Illuminance 071003 af1500
					zb_illuminance(ps->data_us[0] | (ps->data_ub[3] << 16), n);
#endif
				}
				len -= ps->size + 3;
				ps = (padv_struct_xiaomi_t)((u32)ps + ps->size + 3);
			}
		}
	}
}

// 17 16 CDFD 08 12 005E60342D58 0201 50 0F01 59 0804 00 000000
// 17 16 CDFD 08 12 005E60342D58 0201 62 0F01 0B 0804 01830800
// 14 16 CDFD 48 12 005E60342D58 0804 01000000 0F01F4
// 11 16 CDFD 48 12 005E60342D58 1101 01 0F01F7
//_attribute_ram_code_
__attribute__((optimize("-Os")))
void filter_qingping_ad(padv_qingping_t p, int n) {
	padv_struct_qingping_t ps = (padv_struct_qingping_t) &p->data;
	int len = p->size;
#if	USE_DEBUG_SCAN
	sws_printf("q[%d]: ", n);
	sws_print_hex_dump((u8 *)p, len + 1);
	sws_putchar('\n');
#endif
	// ..0812 005E60342D58 0201 64 0F01 7D 0904 8C120000
	if(len > sizeof(adv_qingping_t) && (p->hlen & 0x1f) == 0x08) {
		len -= 11;
		while((ps->size + 2) <= len) {
			if(ps->id_size == QP_DATA_TemperatyreHumidity) { // Temp + Humi
				g_zcl_temperatureAttrs.measuredValue[n] = ps->data_is[0]*10; // in 0.1 C
				g_zcl_relHumidityAttrs.measuredValue[n] = ps->data_is[1]*10;  // in 0.1 %
				update_enable[n] |= FLG_UPDATE_HUMI | FLG_UPDATE_TEMP | FLG_UPDATE_FLG;
			} else if(ps->id_size == QP_DATA_Battery) { // Batt %
				zb_batteryVoltage(ps->data_ub[0], n);
#if SCAN_TRG_ENABLE
			} else if(ps->id_size == QP_DATA_MotionIlluminance) { // Motion + Light
				g_zcl_onOffAttrs.ble_trigger[n] = ps->data_ub[0];
				update_enable[n] |= FLG_UPDATE_TRG | FLG_UPDATE_FLG;
#ifdef ZCL_ILLUMINANCE_MEASUREMENT
				if((p->hlen & 0x40) == 0) {
					zb_illuminance(ps->data_uw >> 8, n);
				}
#endif
#endif
#ifdef ZCL_ILLUMINANCE_MEASUREMENT
			} else if(ps->id_size == QP_DATA_Illuminance) { // Light
				zb_illuminance(ps->data_uw, n);
#endif
			//	} else if(ps->id_size == QP_DATA_Pressure) { // Pressure
			//		pressure = ps->data_us[0];  // in 0.1
			//	} else if(ps->id_size == QP_DATA_light) { // light (bool)
			//     light = ps->data_us[0];
			//	} else if(ps->id_size == QP_DATA_AdvCount) { // Count
			//     adv_count = ps->data_us[0]
			//	} else if(ps->id_size == QP_DATA_Concentration) { // Concentration
			//     pm2_5 = ps->data_us[0];
			//     pm10 = ps->data_us[1];
			//	} else if(ps->id_size == QP_DATA_CO2Concentration) { // CO2 Concentration
			//     ppm = ps->data_us[0];
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
#if	USE_DEBUG_SCAN
		sws_printf("c[%d]: ", n);
		sws_print_hex_dump((u8 *)p, sizeof(adv_custom_t));
		sws_putchar('\n');
#endif
		g_zcl_temperatureAttrs.measuredValue[n] = p->temperature;
		g_zcl_relHumidityAttrs.measuredValue[n] = p->humidity;
		g_zcl_powerAttrs[n].batteryPercentage = p->battery_level << 1;
		g_zcl_powerAttrs[n].batteryVoltage = p->battery_mv/100;
#if SCAN_TRG_ENABLE
		g_zcl_onOffAttrs.ble_trigger[n] = p->flags.trg_output;
#endif
		update_enable[n] |= FLG_UPDATE_BAT | FLG_UPDATE_VBAT | FLG_UPDATE_TEMP | FLG_UPDATE_HUMI | FLG_UPDATE_TRG | FLG_UPDATE_FLG;
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
#if	USE_DEBUG_SCAN
				sws_puts("ccm err!\n");
#endif
			return;
#if	USE_DEBUG_SCAN
		sws_printf("c[%d]: ", n);
		sws_print_hex_dump((u8 *)&pp->data, sizeof(adv_pvvx_data_t));
		sws_putchar('\n');
#endif
		//ps = (padv_struct_xiaomi_t)&decrypt_data;
		g_zcl_temperatureAttrs.measuredValue[n] = pp->data.temp;
		g_zcl_relHumidityAttrs.measuredValue[n] = pp->data.humi;
		zb_batteryVoltage(pp->data.bat, n);
#if SCAN_TRG_ENABLE
		g_zcl_onOffAttrs.ble_trigger[n] = p->flags.trg_output;
#endif
		update_enable[n] |= FLG_UPDATE_BAT | FLG_UPDATE_VBAT | FLG_UPDATE_TEMP | FLG_UPDATE_HUMI | FLG_UPDATE_TRG | FLG_UPDATE_FLG;
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
#if SCAN_TRG_ENABLE
	bool next_trg = false;
#endif
	int len = p->size;
	if(len > sizeof(padv_bthome_t)) {
#if	USE_DEBUG_SCAN
		sws_printf("b[%d]: ", n);
		sws_print_hex_dump((u8 *)p, len + 1);
		sws_putchar('\n');
#endif
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
#if	USE_DEBUG_SCAN
					sws_puts("ccm err!\n");
#endif
#endif
				return;
#if	USE_DEBUG_SCAN > 1
			sws_puts("b: ");
			sws_print_hex_dump(p->data, len);
			sws_putchar('\n');
#endif
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
#ifdef ZCL_ILLUMINANCE_MEASUREMENT
				} else if(ps->type == BtHomeID_illuminance) { // uint24, 0.01 lux
					zb_illuminance(ps->data_us[0] | (ps->data_ub[3] << 16), n);
#endif
#if SCAN_TRG_ENABLE
				} else if(ps->type == BtHomeID_switch) {
					if(!next_trg) {
						g_zcl_onOffAttrs.ble_trigger[n] = ps->data_ub[0];
						update_enable[n] |= FLG_UPDATE_TRG | FLG_UPDATE_FLG;
						next_trg = true;
					}
#endif
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
	if ((h & HCI_FLAG_EVENT_BT_STD) // ble controller hci event
	 && (h & 0xff) == HCI_EVT_LE_META) {
	//----- hci le event: le adv report event -----
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
			int adlen = pa->len;
			if(adlen > sizeof(ad_uuid16_t) + 1 && adlen <= MAX_ADV_BUF_SIZE
			  && pa->data[adlen] != 0) { // rssi = pa->data[adlen] != 0
				for(n = 0; n < MAX_SCAN_DEVS; n++) {
					if(!memcmp(dev_MAC[n], pa->mac, 6)) {
						if(memcmp(prev_advs[n], pa->data, adlen)) {
							memcpy(prev_advs[n], pa->data, adlen);
							pad_uuid16_t pd = (pad_uuid16_t)pa->data;
#if	USE_DEBUG_SCAN > 2
							sws_printf("s[%d]: ", n);
							sws_print_hex_dump((u8 *)pd, adlen);
							sws_putchar('\n');
#endif
							while(adlen > sizeof(ad_uuid16_t)) {
								int len = pd->size;
								if(!len)
									break;
								len++;
								if(len <= adlen // struct size
								  && len > sizeof(ad_uuid16_t)
								  && pd->type == GAP_ADTYPE_SERVICE_DATA_UUID_16BIT) {
									if((pd->uuid16) == ADV_CUSTOM_UUID16) {
										// GATT Service 0x181A Environmental Sensing, ATC custom FW
										filter_custom_ad((adv_custom_t *)pd, n);
									} else if((pd->uuid16) == ADV_BTHOME_UUID16) {
										// GATT Service: BTHome v2
										filter_bthome_ad((adv_bthome_t *)pd, n);
									} else if((pd->uuid16) == ADV_MIHOME_UUID16) {
										// GATT Service: Xiaomi Inc.
										filter_xiaomi_ad((adv_xiaomi_t *)pd, n);
									} else if((pd->uuid16) == ADV_QINGPING_UUID16) {
										// GATT Service: Qingping Technology (Beijing) Co., Ltd.
										filter_qingping_ad((adv_qingping_t *)pd, n);
									}
#if LED_FLASH_RGBE
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
#endif
								}
								pd = (pad_uuid16_t)((u32)pd + len);
								adlen -= len;
							}
						}
						break;
					}
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
