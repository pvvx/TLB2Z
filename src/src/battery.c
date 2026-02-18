/*
 * battery.c
 *
 *  Created on: 18 нояб. 2023 г.
 *      Author: pvvx
 */

#include "tl_common.h"
#include "app.h"
#include "sensors.h"

#define BATTERY_SAFETY_THRESHOLD	2200 //2.2v

void battery_detect(void)
{
	adc_channel_init(SHL_ADC_VBAT);
	u16 battery_mv = get_adc_mv(0);
	if(battery_mv < BATTERY_SAFETY_THRESHOLD){
#if PM_ENABLE
		drv_pm_sleep(PM_SLEEP_MODE_DEEPSLEEP, 0, clock_time() + 3 * CLOCK_16M_SYS_TIMER_CLK_1S);
#else
		SYSTEM_RESET();
#endif
	}
}
