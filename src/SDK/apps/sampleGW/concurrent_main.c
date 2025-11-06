/********************************************************************************************************
 * @file    concurrent_main.c
 *
 * @brief   This is the source file for concurrent_main.c
 *
 * @author  Zigbee Group
 * @date    2021
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
 *******************************************************************************************************/

#include "zb_common.h"
#include "stack/ble/ble.h"
#include "stack/ble/ble_config.h"
#include "stack/ble/ble_common.h"
#include "zigbee_ble_switch.h"


extern void user_zb_init(bool isRetention);
extern void user_ble_init(void);

int main(void){
#if VOLTAGE_DETECT_ENABLE
	u32 tick = 0;
#endif
	startup_state_e state = drv_platform_init();
	u8 isRetention = (state == SYSTEM_DEEP_RETENTION) ? 1 : 0;

#if VOLTAGE_DETECT_ENABLE
	u16 voltage = 0;
	bool powerOn = (state == SYSTEM_BOOT) ? 1 : 0;
	/*
	 * !!!in order to avoid error data written in flash,
	 * recommend setting VOLTAGE_DETECT_ENABLE as 1 to get the stable/safe voltage
	 */
	bool pending = 1;
	while(pending){
		voltage = voltage_detect(powerOn, VOLTAGE_SAFETY_THRESHOLD);
		if(voltage > VOLTAGE_SAFETY_THRESHOLD){
			pending = 0;
		}
	}
#endif

	os_init(isRetention);

#if PA_ENABLE
	/* external RF PA used */
	rf_paInit(PA_TX, PA_RX);
#endif
	/* init for zigbee */
	user_zb_init(isRetention);

	/* init for BLE */
	ble_radio_init();
	user_ble_init();

	drv_enable_irq();

#if (MODULE_WATCHDOG_ENABLE)
	drv_wd_setInterval(600);
    drv_wd_start();
#endif

	while(1){
#if (MODULE_WATCHDOG_ENABLE)
		drv_wd_clear();
#endif

#if VOLTAGE_DETECT_ENABLE
		voltage = voltage_detect(0, VOLTAGE_SAFETY_THRESHOLD);
		if(voltage <= VOLTAGE_SAFETY_THRESHOLD){
			continue;
		}
#endif

		concurrent_mode_main_loop();
	}
	return 0;
}

