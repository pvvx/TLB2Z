/*
 * cmd_parser.c
 *
 *  Created on: 21.03.2024
 *      Author: pvvx
 */
#include "tl_common.h"
#include "app.h"
#include "stack/ble/ble.h"
#include "ble_cfg.h"
#include "flash.h"
#include "ble_scaning.h"

//#include "zb_ble_switch.h"
#include "flash_eep.h"
//#include "drv_uart.h"
#include "ble_cmd_parser.h"


int RxTxWrite(void * p) {
	uint8_t send_buf[SEND_BUFFER_SIZE];
	int i;
	rf_packet_att_data_t *req = (rf_packet_att_data_t*) p;
	uint32_t len = req->l2cap - 3;
	if (len) {
		uint8_t cmd = req->dat[0];
		send_buf[0] = cmd;
		send_buf[1] = 0; // no err
		uint32_t olen = 0;
		if (cmd == CMD_ID_DEV_ID) { // Get DEV_ID
			send_buf[1] = BOARD;
			send_buf[2] = APP_BUILD;
			send_buf[3] = APP_RELEASE;
			olen = 4;
#if USE_BINDKEY
		} else if (cmd == CMD_ID_BKEY) {
			i = req->dat[1];
			if(i < MAX_SCAN_DEVS) {
				if (len > 16 + 1) {
					memcpy(bindkey[i], &req->dat[2], 16);
#if USE_EEP
					flash_write_cfg(bindkey[i], 0, EEP_ID_BKEY(i), 16);
#else
					nv_flashWriteNew(1, NV_MODULE_APP, NV_ITEM_APP_BLE_KEY + i,
					        		16,
									bindkey[i]);
#endif
				}
#if USE_EEP
				if (flash_read_cfg(bindkey[i], 0, EEP_ID_BKEY(i), 16) == 16) {
#else
					if(nv_flashReadNew(1, NV_MODULE_APP, NV_ITEM_APP_BLE_KEY + i,
					        		16,
									bindkey[i]) == NV_SUCC) {
#endif

					memcpy(&send_buf[2], bindkey[i], 16);
					send_buf[1] = i;
					olen = 17 + 1;
				} else { // No bindkey in EEP!
					send_buf[1] = i;
					send_buf[2] = 0;
					olen = 3;
				}
			} else {
				send_buf[1] = 0xff;
				olen = 2;
			}
#endif
		} else if (cmd == CMD_ID_DEV_MAC) {
			i = req->dat[1];
			if(i < MAX_SCAN_DEVS) {
				if (len > 6 + 1) {
					memcpy(dev_MAC[i], &req->dat[2], 6);
#if USE_EEP
					flash_write_cfg(dev_MAC[i], 0, EEP_ID_DMAC(i), 6);
#else
					nv_flashWriteNew(1, NV_MODULE_APP, NV_ITEM_APP_BLE_MAC + i,
					        		6,
									dev_MAC[i]);
#endif
				}

#if USE_EEP
				if (flash_read_cfg(dev_MAC[i], 0, EEP_ID_DMAC(i), 6) == 6) {
#else
				if(nv_flashReadNew(1, NV_MODULE_APP, NV_ITEM_APP_BLE_MAC + i,
					        		6,
									dev_MAC[i]) == NV_SUCC) {
#endif
					memcpy(&send_buf[2], dev_MAC[i], 6);
					send_buf[1] = i;
					olen = 7 + 1;
				} else { // No DMAC in EEP!
					send_buf[1] = i;
					send_buf[2] = 0;
					olen = 3;
				}
			} else {
				send_buf[1] = 0xff;
				olen = 2;
			}
		} else if (cmd == CMD_ID_ILLUMI) {
			if (len > sizeof(zcl_nv_illuminance_t)) {
				memcpy(&g_zcl_illuminanceAttrs.minLevelLx,
					&req->dat[1],
					sizeof(g_zcl_illuminanceAttrs.minLevelLx));
				zcl_illuminance_save();
			}
			memcpy(&send_buf[1],
				g_zcl_illuminanceAttrs.minLevelLx,
				sizeof(g_zcl_illuminanceAttrs.minLevelLx));
			olen = sizeof(g_zcl_illuminanceAttrs.minLevelLx) + 1;
#ifdef ZCL_CUSTOM_ATTR_ONOFF_BLE_TYPE
		} else if (cmd == CMD_ID_CMDONOFF) {
			if(len > 3) {
				g_zcl_onOffAttrs[0].onoffbType = req->dat[1];
				g_zcl_onOffAttrs[1].onoffbType = req->dat[2];
				g_zcl_onOffAttrs[2].onoffbType = req->dat[3];
				zcl_onOffAttr_save(0);
				zcl_onOffAttr_save(1);
				zcl_onOffAttr_save(2);
			}
			send_buf[1] = g_zcl_onOffAttrs[0].onoffbType;
			send_buf[2] = g_zcl_onOffAttrs[1].onoffbType;
			send_buf[3] = g_zcl_onOffAttrs[2].onoffbType;
			olen = 4;
#endif
		// Debug commands (unsupported in different versions!):
#if USE_EEP
		} else if (cmd == CMD_ID_EEP_RW && len > 2) {
			send_buf[1] = req->dat[1];
			send_buf[2] = req->dat[2];
			olen = req->dat[1] | (req->dat[2] << 8);
			if(len > 3) {
				flash_write_cfg(&req->dat[3], 0, olen, len - 3);
			}
			int16_t i = flash_read_cfg(&send_buf[3], 0, olen, SEND_BUFFER_SIZE - 3);
			if(i < 0) {
				send_buf[1] = (uint8_t)(i & 0xff); // Error
				olen = 2;
			} else
				olen = i + 3;
#endif
		} else {
			send_buf[1] = 0xff; // Error cmd
			olen = 2;
		}
		if (olen)
			bls_att_pushNotifyData(RxTx_CMD_OUT_DP_H, send_buf, olen);
	}


	return 0;
}




