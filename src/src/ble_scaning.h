/*
 * scaning.h
 *
 *  Created on: 20.11.2021
 *      Author: pvvx
 */

#ifndef _SCANING_H_
#define _SCANING_H_

#include <stdint.h>

enum {
	FLG_UPDATE_NONE	=	0x00,
	FLG_UPDATE_BAT	=	0x01,
	FLG_UPDATE_VBAT	=	0x02,
	FLG_UPDATE_TEMP	=	0x04,
	FLG_UPDATE_HUMI	=	0x08,
	FLG_UPDATE_TRG	=	0x10,
	FLG_UPDATE_LGH	=	0x20,
	FLG_UPDATE_FLG	=	0x80
} FLG_UPDATE_E;

extern u8 update_enable[MAX_SCAN_DEVS];
extern u8 dev_MAC[MAX_SCAN_DEVS][6]; // [0] - lo, .. [6] - hi digits
#if USE_BINDKEY
extern u8 bindkey[MAX_SCAN_DEVS][16]; // for MAC1
#endif

int scanning_event_callback(u32 h, u8 *p, int n);
//void scan_task(void);

#endif /* _SCANING_H_ */
