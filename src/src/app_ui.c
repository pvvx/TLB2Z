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

	if(g_devAppCtx.sta == g_devAppCtx.oriSta){
		g_devAppCtx.times--;
		if(g_devAppCtx.times <= 0){
			g_devAppCtx.timerLedEvt = NULL;
			return -1;
		}
	}

	g_devAppCtx.sta = !g_devAppCtx.sta;
	if(g_devAppCtx.sta){
		light_on();
		interval = g_devAppCtx.ledOnTime;
	}else{
		light_off();
		interval = g_devAppCtx.ledOffTime;
	}
	return interval;
}

void light_blink_start(u8 times, u16 ledOnTime, u16 ledOffTime)
{
	u32 interval = 0;
	g_devAppCtx.times = times;

	if(!g_devAppCtx.timerLedEvt){
		if(g_devAppCtx.oriSta){
			light_off();
			g_devAppCtx.sta = 0;
			interval = ledOffTime;
		}else{
			light_on();
			g_devAppCtx.sta = 1;
			interval = ledOnTime;
		}
		g_devAppCtx.ledOnTime = ledOnTime;
		g_devAppCtx.ledOffTime = ledOffTime;
		g_devAppCtx.timerLedEvt = TL_ZB_TIMER_SCHEDULE(zclLightTimerCb, NULL, interval);
	}
}

void light_blink_stop(void)
{
	if(g_devAppCtx.timerLedEvt){
		TL_ZB_TIMER_CANCEL(&g_devAppCtx.timerLedEvt);
		g_devAppCtx.timerLedEvt = NULL;
		g_devAppCtx.times = 0;
		if(g_devAppCtx.oriSta){
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
		if(!ble_wrk.keyPressed) {
			// event button on
			ble_wrk.keyPressedTime = clock_time();
			// set next adv. interval
		} else {
			// button hold
			if(clock_time_exceed(ble_wrk.keyPressedTime, 10000)) {
				// button hold > 10 ms
				if(clock_time_exceed(ble_wrk.keyPressedTime, 7000 * 1000)) {
					// button hold > 7 sec
					//ble_wrk.keyPressedTime = clock_time();
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
					if(!ble_wrk.ble_on) {
						ble_wrk.ble_on = 1;
						ble_wrk.key1flag = 1;
					}
					if(ble_wrk.adv_restore_count)
						ble_wrk.adv_restore_count = (80000/900); // 80 sec 80000/900 = 88.888..
				}
			} // button hold < 10 ms
		}
	} else {
		// ble_wrk.keyPressedTime = clock_time();
		if(!g_devAppCtx.timerLedEvt) {
#ifdef GPIO_RELAY
			if(g_devAppCtx.oriSta){
				light_on();
			}else{
				light_off();
			}
#else
			light_off();
#endif
		}
	}
	ble_wrk.keyPressed = button_on;
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
