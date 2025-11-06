/********************************************************************************************************
 * @file    app_ui.h
 *
 * @brief   This is the header file for app_ui
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

#ifndef _APP_UI_H_
#define _APP_UI_H_

/**********************************************************************
 * CONSTANT
 */

/**********************************************************************
 * TYPEDEFS
 */
#if defined(GPIO_LED_R)
#define TIMER_LED_R	(2*CLOCK_16M_SYS_TIMER_CLK_1S)
#define	LED_R_ON()  { gpio_write(GPIO_LED_R, LED_ON); leds.tr = reg_system_tick; }
#else
#define	LED_R_ON()
#endif
#if defined(GPIO_LED_G)
#define TIMER_LED_G	(128*CLOCK_16M_SYS_TIMER_CLK_1MS)
#define	LED_G_ON()  { gpio_write(GPIO_LED_G, LED_ON); leds.tg = reg_system_tick; }
#else
#define	LED_G_ON()
#endif
#if defined(GPIO_LED_B)
#define TIMER_LED_B	(128*CLOCK_16M_SYS_TIMER_CLK_1MS)
#define	LED_B_ON()  { gpio_write(GPIO_LED_B, LED_ON); leds.tb = reg_system_tick; }
#else
#define	LED_B_ON()
#endif
#if 0 //defined(GPIO_LED_W) -> Zigbee GPIO_LED
#define TIMER_LED_W	(128*CLOCK_16M_SYS_TIMER_CLK_1MS)
#define	LED_W_ON()  { gpio_write(GPIO_LED_W, LED_ON); leds.tw = reg_system_tick; }
#else
#define	LED_W_ON()
#endif
#if defined(GPIO_LED_E)
#define TIMER_LED_E	(64*CLOCK_16M_SYS_TIMER_CLK_1MS)
#define	LED_E_ON()  { gpio_write(GPIO_LED_E, LED_ON); leds.te = reg_system_tick; }
#else
#define	LED_E_ON()
#endif

typedef struct {
#if defined(GPIO_LED_R)
	u32 tr;
#endif
#if defined(GPIO_LED_G)
	u32 tg;
#endif
#if defined(GPIO_LED_B)
	u32 tb;
#endif
#if 0 //defined(GPIO_LED_W)
	u32 tw;
#endif
#if defined(GPIO_LED_E)
	u32 te;
#endif
} leds_tik_t;

extern leds_tik_t leds;
void task_leds(void);

/**********************************************************************
 * FUNCTIONS
 */
void light_blink_start(u8 times, u16 ledOnTime, u16 ledOffTime);
void light_blink_stop(void);

void light_init(void);
void light_on(void);
void light_off(void);

void task_keys(void);

void read_sensor_start(u16 delayTime);

#endif	/* _APP_UI_H_ */
