
/********************************************************************************************************
 * @file    app_pm.c
 *
 * @brief   This is the source file for app_pm
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
#include "zb_api.h"
#include "zcl_include.h"
#include "bdb.h"
#include "ota.h"
#include "sampleSwitch.h"
#include "app_ui.h"
#include "zigbee_ble_switch.h"
#include "stack/ble/ble_config.h"
#include "stack/ble/ble_common.h"
#include "stack/ble/ble.h"

#if PM_ENABLE

extern volatile bool g_bleConnDoing;
extern u32 blt_pm_proc(void);

extern u8 switch_to_ble;
/**
 *  @brief Definition for wakeup source and level for PM
 */
drv_pm_pinCfg_t g_switchPmCfg[] = {
	{
		BUTTON1,
		PM_WAKEUP_LEVEL
	},
	{
		BUTTON2,
		PM_WAKEUP_LEVEL
	}
};


bool app_zigbeeIdle(void){
	bool ret = 0;
	
	ret = bdb_isIdle() && !tl_stackBusy() && zb_isTaskDone();
	if(ret){
		apsCleanToStopSecondClock();
		ret &= !ev_timer_process(1);
		if(!ret){
			secondClockRun();
		}
	}

	return ret;
}

void app_pm_wakeupPinCfg(void){
	drv_pm_wakeupPinLevelChange(g_switchPmCfg, sizeof(g_switchPmCfg)/sizeof(drv_pm_pinCfg_t));
}

void app_pm_init(void){
	drv_pm_wakeupPinConfig(g_switchPmCfg, sizeof(g_switchPmCfg)/sizeof(drv_pm_pinCfg_t));
}

#if defined(MCU_CORE_8258) || defined(MCU_CORE_B91)
void app_pm_task(void){
	drv_pm_sleep_mode_e sleepMode = PM_SLEEP_MODE_DEEP_WITH_RETENTION;//PM_SLEEP_MODE_SUSPEND;//PM_SLEEP_MODE_DEEP_WITH_RETENTION;
	drv_pm_wakeup_src_e wakeupSrc = PM_WAKEUP_SRC_PAD | PM_WAKEUP_SRC_TIMER;

	/*
	 * 3 low power mode
	 * 1) in ble connection state, internal ble suspend is used,
	 * 2) in ble advertise state, system enters retention-deep(wakeup interval is ble advertise interval)
	 * 3) in ble IDLE state(just zigbee is working),
	 *    system enters retention-deep(poll rate is enabled) mode or
	 *    deep mode(poll rate is disabled)
	 *
	 * */
	if(APP_BLE_STATE_GET() == BLS_LINK_STATE_CONN || g_bleConnDoing){
		if(!bls_pm_conditionCbIsValid()){
			bls_pm_conditionCbRegister(app_zigbeeIdle);   //register it to enable ble suspend mode
		}
	}else if(APP_BLE_STATE_GET() == BLS_LINK_STATE_ADV){
		if(bls_pm_conditionCbIsValid()){
			bls_pm_conditionCbUnregister();
		}
	}

	if((APP_BLE_STATE_GET() != BLS_LINK_STATE_IDLE)){
		/*
		 * adv for ADV_IDLE_ENTER_DEEP_TIME and conn for CONN_IDLE_ENTER_DEEP_TIME no event, enter deepsleep
		 *  */
		 if(blt_pm_proc()){
			 /*
				* here call "bls_ll_setAdvEnable(BLC_ADV_DISABLE)" to let ble enter state of BLS_LINK_STATE_IDLE,
				* and then need to call ble_task_restart() to start ble task again
				*
				* */
			 if(bls_ll_setAdvEnable(BLC_ADV_DISABLE) == BLE_SUCCESS){
				 /* rf irq is cleared in the "bls_ll_setAdvEnable",
				 * so that the rf tx/rx interrupt will be missed if the "bls_ll_setAdvEnable" is called in Zigbee mode
				 */
				ZB_RF_ISR_RECOVERY;
			}
			drv_restore_irq(r);
		 }
	}

	switch_to_ble = 0;
	if(CURRENT_SLOT_GET() == DUALMODE_SLOT_ZIGBEE && app_zigbeeIdle()){
		app_pm_wakeupPinCfg();
		if(APP_BLE_STATE_GET() == BLS_LINK_STATE_IDLE){
			drv_pm_lowPowerEnter();
		}else{
			if(!is_switch_to_ble()){
				if(APP_BLE_STATE_GET() == BLS_LINK_STATE_ADV){
					drv_pm_sleep(sleepMode, wakeupSrc, get_ble_next_event_tick());
				}else if(APP_BLE_STATE_GET() == BLS_LINK_STATE_CONN || g_bleConnDoing){
					switch_to_ble = 1;
				}
			}
		}
	}

	return;
}
#elif defined(MCU_CORE_TL321X) || defined(MCU_CORE_TL721X)
void app_pm_task(void){
	/*
	 * 2 low power mode
	 * 1) in ble not idle mode, internal ble suspend is used,
	 * 2) in ble IDLE state(just zigbee is working),
	 *    system enters retention-deep(poll rate is enabled) mode or
	 *    deep mode(poll rate is disabled)
	 *
	 * */
	if(!APP_BLE_STATE_IDLE()){
		if(!bls_pm_conditionCbIsValid()){
			bls_pm_conditionCbRegister(app_zigbeeIdle);   //register it to enable ble suspend mode
		}
	}else{
		if(bls_pm_conditionCbIsValid()){
			bls_pm_conditionCbUnregister();
		}
	}

	/*
	 * adv for ADV_IDLE_ENTER_DEEP_TIME and conn for CONN_IDLE_ENTER_DEEP_TIME no event, enter deepsleep
	 *  */
	blt_pm_proc();
	switch_to_ble = 0;

	if(CURRENT_SLOT_GET() == DUALMODE_SLOT_ZIGBEE && app_zigbeeIdle()){
		app_pm_wakeupPinCfg();
		if(APP_BLE_STATE_IDLE()){
			if(!g_switchAppCtx.keyPressed){
				drv_pm_lowPowerEnter();
			}
		}else{
			switch_to_ble = 1;
		}
	}

	return;
}
#endif

void app_enterCutOffMode(void){
	/* enter cut-off mode to protect battery
	 * the system should enter ultra-low-power mode,can't be wake up or only io wakeup is allowed until a new battery is used
	 * */
	drv_pm_wakeup_src_e wakeupSrc = PM_WAKEUP_SRC_PAD;
	app_pm_wakeupPinCfg();

	//drv_pm_sleep(PM_SLEEP_MODE_DEEPSLEEP, 0, 0);   //can't be wake up
	drv_pm_sleep(PM_SLEEP_MODE_DEEPSLEEP, wakeupSrc, 0);   // only io wakeup is allowed
	SYSTEM_RESET();  //shouldn't run here
}
#endif
