/********************************************************************************************************
 * @file    app_ui.c
 *
 * @brief   This is the source file for app_ui
 *
 * @author  Zigbee Group
 * @date    2021
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *			All rights reserved.
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *
 *******************************************************************************************************/

/**********************************************************************
 * INCLUDES
 */
#include "tl_common.h"
#include "app.h"
#include "sensors.h"
#include "zb_api.h"
#include "zcl_include.h"
#include "app_ui.h"
#include "zb_reporting.h"
#include "stack/ble/ble_8258/ble.h"
#include "ble_cfg.h"

/**********************************************************************
 * LOCAL CONSTANTS
 */


/**********************************************************************
 * TYPEDEFS
 */


/**********************************************************************
 * LOCAL FUNCTIONS
 */
void light_on(void)
{
	gpio_write(GPIO_LED, LED_ON);
#if PM_ENABLE
#if LED_ON
	gpio_setup_up_down_resistor(GPIO_LED, PM_PIN_PULLUP_10K);
#else
	gpio_setup_up_down_resistor(GPIO_LED, PM_PIN_PULLDOWN_100K);
#endif
#endif
}

void light_off(void)
{
	gpio_write(GPIO_LED, LED_OFF);
#if PM_ENABLE
	gpio_setup_up_down_resistor(GPIO_LED, PM_PIN_UP_DOWN_FLOAT);
#endif
}

void light_init(void)
{
	light_off();
}

s32 zclLightTimerCb(void *arg)
{
	u32 interval = 0;

	if(g_sensorAppCtx.sta == g_sensorAppCtx.oriSta){
		g_sensorAppCtx.times--;
		if(g_sensorAppCtx.times <= 0){
			g_sensorAppCtx.timerLedEvt = NULL;
			return -1;
		}
	}

	g_sensorAppCtx.sta = !g_sensorAppCtx.sta;
	if(g_sensorAppCtx.sta){
		light_on();
		interval = g_sensorAppCtx.ledOnTime;
	}else{
		light_off();
		interval = g_sensorAppCtx.ledOffTime;
	}
	return interval;
}

void light_blink_start(u8 times, u16 ledOnTime, u16 ledOffTime)
{
	u32 interval = 0;
	g_sensorAppCtx.times = times;

	if(!g_sensorAppCtx.timerLedEvt){
		if(g_sensorAppCtx.oriSta){
			light_off();
			g_sensorAppCtx.sta = 0;
			interval = ledOffTime;
		}else{
			light_on();
			g_sensorAppCtx.sta = 1;
			interval = ledOnTime;
		}
		g_sensorAppCtx.ledOnTime = ledOnTime;
		g_sensorAppCtx.ledOffTime = ledOffTime;
		g_sensorAppCtx.timerLedEvt = TL_ZB_TIMER_SCHEDULE(zclLightTimerCb, NULL, interval);
	}
}

void light_blink_stop(void)
{
	if(g_sensorAppCtx.timerLedEvt){
		TL_ZB_TIMER_CANCEL(&g_sensorAppCtx.timerLedEvt);
		g_sensorAppCtx.timerLedEvt = NULL;
		g_sensorAppCtx.times = 0;
		if(g_sensorAppCtx.oriSta){
			light_on();
		}else{
			light_off();
		}
	}
}

void task_keys(void) {
	u8 button_on = gpio_read(BUTTON1)? BUTTON_ON : !BUTTON_ON;
	if(button_on) {
		// button on
		light_blink_stop();
		light_on();
		if(!g_sensorAppCtx.keyPressed) {
			// event button on
			g_sensorAppCtx.keyPressedTime = clock_time();
			// set next adv. interval
		} else {
			// button hold
			if(clock_time_exceed(g_sensorAppCtx.keyPressedTime, 10000)) {
				// button hold > 10 ms
				if(clock_time_exceed(g_sensorAppCtx.keyPressedTime, 7000 * 1000)) {
					// button hold > 7 sec
					//g_sensorAppCtx.keyPressedTime = clock_time();
#ifdef GPIO_LED
					for(int i = 0; i < 15; i++) {  // 15*0.2 = 3 sec
						gpio_write(GPIO_LED, LED_ON);
						sleep_ms(100);
						gpio_write(GPIO_LED, LED_OFF);
						sleep_ms(100);
					}
#endif
					if(!zb_isDeviceFactoryNew()) {
						tl_bdbReset2FN();
					}
					light_off();
#if PM_ENABLE
					drv_pm_sleep(PM_SLEEP_MODE_DEEPSLEEP, 0, clock_time() + 5 * CLOCK_16M_SYS_TIMER_CLK_1S);
#else
					sleep_ms(2500);
					SYSTEM_RESET();
#endif
				} else {
					// 10 ms > button hold < 7 sec
					if(!g_sensorAppCtx.ble_on) {
						g_sensorAppCtx.ble_on = 1;
						g_sensorAppCtx.key1flag = 1;
					}
					if(g_sensorAppCtx.adv_restore_count)
						g_sensorAppCtx.adv_restore_count = (80000/900); // 80 sec 80000/900 = 88.888..
				}
			} // button hold < 10 ms
		}
	} else {
		// g_sensorAppCtx.keyPressedTime = clock_time();
		if(!g_sensorAppCtx.timerLedEvt)
			light_off();
	}
	g_sensorAppCtx.keyPressed = button_on;
#if PM_ENABLE
	cpu_set_gpio_wakeup(BUTTON1, button_on , 1); // button_on: Level_Low=0, Level_High =1
#endif
}

#if LED_FLASH_RGBE

leds_tik_t leds;

void task_leds(void) {
		u32 tt = reg_system_tick;
#ifdef 	TIMER_LED_R
		if(tt - leds.tr > TIMER_LED_R) {
			leds.tr = tt;
			gpio_write(GPIO_LED_R, LED_OFF);
		}
#endif
#ifdef 	TIMER_LED_G
		if(tt - leds.tg > TIMER_LED_G) {
			leds.tg = tt;
			gpio_write(GPIO_LED_G, LED_OFF);
		}
#endif
#ifdef 	TIMER_LED_B
		if(tt - leds.tb > TIMER_LED_B) {
			leds.tb = tt;
			gpio_write(GPIO_LED_B, LED_OFF);
		}
#endif
#ifdef 	TIMER_LED_W
		if(tt - leds.tw > TIMER_LED_W) {
			leds.tw = tt;
			gpio_write(GPIO_LED_W, LED_OFF);
		}
#endif
#ifdef 	TIMER_LED_E
		if(tt - leds.te > TIMER_LED_E) {
			leds.te = tt;
			gpio_write(GPIO_LED_E, LED_OFF);
		}
#endif
}

#endif
