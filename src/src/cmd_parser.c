/*
 * cmd_parser.c
 *
 *  Created on: 21.03.2024
 *      Author: pvvx
 */
#include "tl_common.h"
#include "device.h"
#include "stack/ble/ble.h"
#include "ble_cfg.h"
#include "flash.h"
#include "scaning.h"

//#include "zigbee_ble_switch.h"
#include "flash_eep.h"
//#include "drv_uart.h"
#include "cmd_parser.h"


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
					flash_write_cfg(bindkey[i], EEP_ID_BKEY(i), 16);
				}
				if (flash_read_cfg(bindkey[i], EEP_ID_BKEY(i), 16) == 16) {
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
					flash_write_cfg(dev_MAC[i], EEP_ID_DMAC(i), 6);
				}
				if (flash_read_cfg(dev_MAC[i], EEP_ID_DMAC(i), 6) == 6) {
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

		// Debug commands (unsupported in different versions!):

		} else if (cmd == CMD_ID_EEP_RW && len > 2) {
			send_buf[1] = req->dat[1];
			send_buf[2] = req->dat[2];
			olen = req->dat[1] | (req->dat[2] << 8);
			if(len > 3) {
				flash_write_cfg(&req->dat[3], olen, len - 3);
			}
			int16_t i = flash_read_cfg(&send_buf[3], olen, SEND_BUFFER_SIZE - 3);
			if(i < 0) {
				send_buf[1] = (uint8_t)(i & 0xff); // Error
				olen = 2;
			} else
				olen = i + 3;
		} else {
			send_buf[1] = 0xff; // Error cmd
			olen = 2;
		}
		if (olen)
			bls_att_pushNotifyData(RxTx_CMD_OUT_DP_H, send_buf, olen);
	}


	return 0;
}




