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
#include "tl_common.h"
#include "zb_common.h"
#include "stack/ble/ble_config.h"
#include "stack/ble/ble_common.h"
#include "stack/ble/ble.h"
#include "zigbee_ble_switch.h"
#include "zcl_include.h"
#include "sampleSwitch.h"
#include "app_pm.h"
#include "app_ui.h"

extern void user_zb_init(bool isRetention);
extern void user_ble_init(bool isRetention);
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

	os_init(isRetention);

#if PM_ENABLE
    g_switchAppCtx.keyPressDebounce = 3;
	app_pm_init();
#endif

#if VOLTAGE_DETECT_ENABLE
	/*
	 * !!!in order to avoid error data written in flash,
	 * recommend setting VOLTAGE_DETECT_ENABLE as 1 to get the stable/safe voltage
	 */
	bool powerOn = (state == SYSTEM_BOOT) ? 1 : 0;
	u16 voltage = 0;

	bool pending = 1;
	while(pending){
		voltage = voltage_detect(powerOn, APP_VOLTAGE_THRESHOLD_CUTOFF);
		if(voltage <= APP_VOLTAGE_THRESHOLD_CUTOFF){
#if PM_ENABLE
			/* !!!entering cut off mode is recommended */
			app_enterCutOffMode();
#endif
		}else{
			pending = 0;
		}
	}
#endif

	//extern void moduleTest_start(void);
	//moduleTest_start();
#if PA_ENABLE
	rf_paInit(PA_TX, PA_RX);
#endif

	user_zb_init(isRetention);
	user_ble_init(isRetention);
	if(CURRENT_SLOT_GET() == DUALMODE_SLOT_BLE){
		ble_radio_init();
	}else{
#if !BLE_ACTIVE_BY_UI
		ble_task_restart();
		ble_advertiseTickUpdate();
#else
		switch_to_zb_context();
#endif
	}

	drv_enable_irq();

#if (MODULE_WATCHDOG_ENABLE)
	drv_wd_setInterval(600);
    drv_wd_start();
#endif

	while(1){
#if (MODULE_WATCHDOG_ENABLE)
		drv_wd_clear();
#endif

#if (VOLTAGE_DETECT_ENABLE)
		voltage = voltage_detect(0, APP_VOLTAGE_THRESHOLD_CUTOFF);
		if(voltage <= APP_VOLTAGE_THRESHOLD_CUTOFF){
#if PM_ENABLE
			/* !!!entering cut off mode is recommended */
			app_enterCutOffMode();
#endif
			continue;
		}

#if PM_ENABLE
		set_detect_voltage(voltage);
		if(voltage <= APP_VOLTAGE_THRESHOLD_LOW_VOL){
			/* TODO: enter low voltage warning mode */
		}
#endif
#endif

		concurrent_mode_main_loop();
		app_key_handler();

		/* sample code to restart/stop ble task */
		app_bleStopRestart();

#if PM_ENABLE
		if(g_switchAppCtx.keyPressDebounce > 0){
			g_switchAppCtx.keyPressDebounce--;
		}else{
			app_pm_task();
		}
#endif
	}

	return 0;
}

