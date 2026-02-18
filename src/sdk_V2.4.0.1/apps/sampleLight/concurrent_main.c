/********************************************************************************************************
 * @file    concurrent_main.c
 *
 * @brief   This is the source file for concurrent_main
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
#include "zigbee_ble_switch.h"

extern void user_zb_init(bool isRetention);
extern void user_ble_init(void);
volatile u8 T_taskRunCnt = 0;
volatile u8 T_taskBleTrig = 0;

static void app_bleStopRestart(void){
	T_taskRunCnt++;
	/* for debugging ble start/stop */
	if(T_taskBleTrig){
		if(T_taskBleTrig == 0x01){
			ble_task_stop();
		}else if(T_taskBleTrig == 0x02){
			ble_task_restart();
		}
		T_taskBleTrig = 0;
	}
}


int main(void){
	startup_state_e state = drv_platform_init();
	u8 isRetention = (state == SYSTEM_DEEP_RETENTION) ? 1 : 0;

#if VOLTAGE_DETECT_ENABLE
	/*
	 * !!!in order to avoid error data written in flash,
	 * recommend setting VOLTAGE_DETECT_ENABLE as 1 to get the stable/safe voltage
	 */
	bool powerOn = (state == SYSTEM_BOOT) ? 1 : 0;
	u16 voltage = 0;
	bool pending = 1;
	while(pending){
		voltage = voltage_detect(powerOn, VOLTAGE_SAFETY_THRESHOLD);
		if(voltage > VOLTAGE_SAFETY_THRESHOLD){
			pending = 0;
		}
	}
#endif

	os_init(isRetention);

#if 0
	extern void moduleTest_start(void);
	CURRENT_SLOT_SET(DUALMODE_SLOT_ZIGBEE);
	moduleTest_start();
#endif

#if PA_ENABLE
	/* external RF PA used */
	rf_paInit(PA_TX, PA_RX);
#endif

	//init for ZB
	user_zb_init(isRetention);

	//init for BLE
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

		/* sample code to restart/stop ble task */
		app_bleStopRestart();

	}

	return 0;
}

