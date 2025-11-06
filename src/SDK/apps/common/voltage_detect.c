/********************************************************************************************************
 * @file     voltage_detect.c
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
#include "tl_common.h"
#include "zb_task_queue.h"
#include "zb_common.h"

volatile u32 T_voltageValue = 0;     //for debug

void voltage_detectInit(void){
	drv_adc_init();

#if defined(MCU_CORE_826x)
	drv_adc_mode_pin_set(DRV_ADC_VBAT_MODE, NOINPUT);
#elif defined(MCU_CORE_8258)
	/*
	 * 	VOLTAGE_DETECT_PIN just can use the following pin:
	    GPIO_PB0,GPIO_PB1,
		GPIO_PB2,GPIO_PB3,
		GPIO_PB4,GPIO_PB5,
		GPIO_PB6,GPIO_PB7,
		GPIO_PC4,GPIO_PC5
	 * */
	drv_adc_mode_pin_set(DRV_ADC_VBAT_MODE, VOLTAGE_DETECT_PIN);
#endif

	drv_adc_enable(1);
}

#if VOLTAGE_PROTECT_EN
void voltage_protect(u8 sys_start){
	u16 voltage = drv_get_adc_data();
	u32 curTick = clock_time();
	s32 debounceNum = VOLTAGE_DEBOUNCE_NUM;

	//printf("VDD: %d\n", voltage);
	if(sys_start || voltage < BATTERY_SAFETY_THRESHOLD){
		while(debounceNum > 0){
			if(voltage > BATTERY_SAFETY_THRESHOLD){
				debounceNum--;
			}else{
				debounceNum = VOLTAGE_DEBOUNCE_NUM;
			}

			if(clock_time_exceed(curTick, 1000 * 1000)){
#if PM_ENABLE
				pm_lowPowerEnter(PLATFORM_MODE_DEEPSLEEP, 0, 0);
#else
				SYSTEM_RESET();
#endif
			}

			voltage = drv_get_adc_data();
		}
	}

}
#endif

