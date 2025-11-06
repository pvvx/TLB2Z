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
	FLG_UPDATE_NONE	=	0,
	FLG_UPDATE_BAT	=	1,
	FLG_UPDATE_VBAT	=	2,
	FLG_UPDATE_TEMP	=	4,
	FLG_UPDATE_HUMI	=	8,
	FLG_UPDATE_FLG	=	0x10
} FLG_UPDATE_E;

u8	update_enable[MAX_SCAN_DEVS];

extern u8 dev_MAC[MAX_SCAN_DEVS][6]; // [0] - lo, .. [6] - hi digits

#if USE_BINDKEY
extern u8 bindkey[MAX_SCAN_DEVS][16]; // for MAC1
#endif

int scanning_event_callback(u32 h, u8 *p, int n);
//void scan_task(void);

#endif /* _SCANING_H_ */
