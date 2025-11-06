/********************************************************************************************************
 * @file     voltage_detect.h
 *
 * @brief    voltage detection API
 *
 * @author
 * @date     Mar. 1, 2021
 *
 * @par      Copyright (c) 2020, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *
 *			 The information contained herein is confidential and proprietary property of Telink
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai)
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in.
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided.
 *
 *******************************************************************************************************/
#ifndef VOLTAGE_DETECT_H_
#define VOLTAGE_DETECT_H_
#include "tl_common.h"

#if PM_ENABLE
	#define VOLTAGE_SAFETY_THRESHOLD   2200
#else
	#define VOLTAGE_SAFETY_THRESHOLD   2500
#endif

 /*
 * configure the adc for voltage detection
 *
 * @param none
 *
 * @return none
 *
 * */
void voltage_detectInit(void);


/*
* detect the current voltage, if the voltage is less than the threshold value,
* the chip will enter special mode
*
* @param sys_start, 1: power on, 0: normal running
*
* @return none
*
* */
void voltage_protect(u8 sys_start);

#endif
