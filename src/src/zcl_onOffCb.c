/********************************************************************************************************
 * @file    zcl_onOffCb.c
 *
 * @brief   This is the source file for zcl_onOffCb
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
#include "zb_api.h"
#include "zcl_include.h"
#ifdef ZCL_ON_OFF
#include "app.h"
#include "app_ui.h"
#include "ble_scaning.h"

/**********************************************************************
 * LOCAL CONSTANTS
 */
#define ZCL_ONOFF_TIMER_INTERVAL        100 //the timer interval to change the offWaitTime/onTime attribute of the ONOFF cluster

/**********************************************************************
 * LOCAL VARIABLES
 */
static ev_timer_event_t *onWithTimedOffTimerEvt[MAX_SCAN_DEVS];

/**********************************************************************
 * FUNCTIONS
 */
void remoteCmdOnOff(u8 srcEp, u8 cmd) {
    epInfo_t dstEpInfo;
    TL_SETSTRUCTCONTENT(dstEpInfo, 0);

    dstEpInfo.profileId = HA_PROFILE_ID;

    dstEpInfo.dstAddrMode = APS_DSTADDR_EP_NOTPRESETNT;

    /* command 0x00 - off, 0x01 - on, 0x02 - toggle */

    switch(cmd) {
        case ZCL_CMD_ONOFF_OFF:
            zcl_onOff_offCmd(srcEp, &dstEpInfo, FALSE);
            break;
        case ZCL_CMD_ONOFF_ON:
            zcl_onOff_onCmd(srcEp, &dstEpInfo, FALSE);
            break;
        case ZCL_CMD_ONOFF_TOGGLE:
            zcl_onOff_toggleCmd(srcEp, &dstEpInfo, FALSE);
            break;
        default:
            break;
    }
}

/*********************************************************************
 * @fn      app_onOffUpdate
 *
 * @brief
 *
 * @param   ZCL_CMD_ONOFF_ON / ZCL_ONOFF_STATUS_OFF / ZCL_CMD_ONOFF_TOGGLE
 *
 * @return  None
 */
void app_onOffUpdate(u8 cmd, u8 n)
{
    zcl_onOffAttr_t *pOnOff = zcl_onoffAttrGet(n);
    bool onOff = ZCL_ONOFF_STATUS_ON;

    if (cmd == ZCL_CMD_ONOFF_ON) {
        onOff = ZCL_ONOFF_STATUS_ON;
    } else if (cmd == ZCL_CMD_ONOFF_OFF) {
        onOff = ZCL_ONOFF_STATUS_OFF;
    } else if (cmd == ZCL_CMD_ONOFF_TOGGLE) {
        onOff = (pOnOff->onOff == ZCL_ONOFF_STATUS_ON) ? ZCL_ONOFF_STATUS_OFF
                                                       : ZCL_ONOFF_STATUS_ON;
    } else {
        return;
    }

    //update attributes
    if (onOff == ZCL_ONOFF_STATUS_ON) {
        pOnOff->globalSceneControl = TRUE;
        pOnOff->onOff = ZCL_ONOFF_STATUS_ON;
        if (pOnOff->onTime == 0) {
            pOnOff->offWaitTime = 0;
        }
    } else {
        pOnOff->onOff = ZCL_ONOFF_STATUS_OFF;
        pOnOff->onTime = 0;
    }
	if(dev_MAC[n][5] == 0) {
#ifdef GPIO_RELAY
		if(!n) {
			gpio_write(GPIO_RELAY, onOff);
			g_sensorAppCtx.oriSta = onOff;
			if(onOff){
				light_on();
			}else{
				light_off();
		   }
		}
#endif
		g_zcl_onOffAttrs[n].onOffrm = onOff;
		remoteCmdOnOff(n + APP_ENDPOINT1, onOff);
	} else {
		if(onOff == ZCL_ONOFF_STATUS_OFF) {
			// отключить (срабатывание от BLE)
			pOnOff->onOffrm = ZCL_ONOFF_STATUS_OFF;
			remoteCmdOnOff(n + APP_ENDPOINT1, ZCL_ONOFF_STATUS_OFF);
		} else {
			// работа от BLE
		}
	}
   	zcl_onOffAttr_save(n);
}

/*********************************************************************
 * @fn      app_onWithTimedOffTimerCb
 *
 * @brief   timer event to process the ON_WITH_TIMED_OFF command
 *
 * @param   arg
 *
 * @return  0: timer continue on; -1: timer will be canceled
 */
static s32 app_onWithTimedOffTimerCb(void *arg)
{
	u8 n = (u8)((u32)arg);

    zcl_onOffAttr_t *pOnOff = zcl_onoffAttrGet(n);

    if ((pOnOff->onOff == ZCL_ONOFF_STATUS_ON) && pOnOff->onTime) {
        pOnOff->onTime--;
        if (pOnOff->onTime <= 0) {
            pOnOff->offWaitTime = 0;
            app_onOffUpdate(ZCL_CMD_ONOFF_OFF, n);
        }
    }

    if ((pOnOff->onOff == ZCL_ONOFF_STATUS_OFF) && pOnOff->offWaitTime) {
        pOnOff->offWaitTime--;
        if (pOnOff->offWaitTime <= 0) {
            onWithTimedOffTimerEvt[n] = NULL;
            return -1;
        }
    }

    if (pOnOff->onTime || pOnOff->offWaitTime) {
        return 0;
    } else {
        onWithTimedOffTimerEvt[n] = NULL;
        return -1;
    }
}

/*********************************************************************
 * @fn      app_onoff_onWithTimedOffProcess
 *
 * @brief
 *
 * @param   cmd
 *
 * @return  None
 */
static void app_onoff_onWithTimedOffProcess(zcl_onoff_onWithTimeOffCmd_t *cmd, u8 n)
{
    zcl_onOffAttr_t *pOnOff = zcl_onoffAttrGet(n);

    if (cmd->onOffCtrl.bits.acceptOnlyWhenOn && (pOnOff->onOff == ZCL_ONOFF_STATUS_OFF)) {
        return;
    }

    if (pOnOff->offWaitTime && (pOnOff->onOff == ZCL_ONOFF_STATUS_OFF)) {
        pOnOff->offWaitTime = min2(pOnOff->offWaitTime, cmd->offWaitTime);
    } else {
        pOnOff->onTime = max2(pOnOff->onTime, cmd->onTime);
        pOnOff->offWaitTime = cmd->offWaitTime;
        app_onOffUpdate(ZCL_CMD_ONOFF_ON, n);
    }

    if ((pOnOff->onTime < 0xFFFF) && (pOnOff->offWaitTime < 0xFFFF)) {
        if(pOnOff->onTime || pOnOff->offWaitTime){
            if (onWithTimedOffTimerEvt[n]) {
                TL_ZB_TIMER_CANCEL(&onWithTimedOffTimerEvt[n]);
            }
            onWithTimedOffTimerEvt[n] = TL_ZB_TIMER_SCHEDULE(app_onWithTimedOffTimerCb, (void *)((u32)n), ZCL_ONOFF_TIMER_INTERVAL);
        }
    }
}

/*********************************************************************
 * @fn      app_onoff_offWithEffectProcess
 *
 * @brief
 *
 * @param   cmd
 *
 * @return  None
 */
static void app_onoff_offWithEffectProcess(zcl_onoff_offWithEffectCmd_t *cmd, u8 n)
{
    zcl_onOffAttr_t *pOnOff = zcl_onoffAttrGet(n);
    pOnOff->globalSceneControl = FALSE;

    //TODO:

    app_onOffUpdate(ZCL_CMD_ONOFF_OFF, n);
}

/*********************************************************************
 * @fn      app_onoff_onWithRecallGlobalSceneProcess
 *
 * @brief
 *
 * @param   cmd
 *
 * @return  None
 */
static void app_onoff_onWithRecallGlobalSceneProcess(u8 n)
{
    zcl_onOffAttr_t *pOnOff = zcl_onoffAttrGet(n);
    pOnOff->globalSceneControl = TRUE;

    //TODO:
}

/*********************************************************************
 * @fn      app_onOffCb
 *
 * @brief   Handler for ZCL ONOFF command. This function will set ONOFF attribute first.
 *
 * @param	pAddrInfo
 * @param   cmdId - onoff cluster command id
 * @param	cmdPayload
 *
 * @return  status_t
 */
status_t app_onOffCb(zclIncomingAddrInfo_t *pAddrInfo, u8 cmdId, void *cmdPayload)
{
	u8 n = pAddrInfo->dstEp;
    if (n >= APP_ENDPOINT1 && n < APP_ENDPOINT1 + MAX_SCAN_DEVS) {
    	n -= APP_ENDPOINT1;
        switch (cmdId) {
        case ZCL_CMD_ONOFF_ON:
        case ZCL_CMD_ONOFF_OFF:
        case ZCL_CMD_ONOFF_TOGGLE:
            app_onOffUpdate(cmdId, n);
            break;
        case ZCL_CMD_OFF_WITH_EFFECT:
            app_onoff_offWithEffectProcess((zcl_onoff_offWithEffectCmd_t *)cmdPayload, n);
            break;
        case ZCL_CMD_ON_WITH_RECALL_GLOBAL_SCENE:
            app_onoff_onWithRecallGlobalSceneProcess(n);
            break;
        case ZCL_CMD_ON_WITH_TIMED_OFF:
            app_onoff_onWithTimedOffProcess((zcl_onoff_onWithTimeOffCmd_t *)cmdPayload, n);
            break;
        default:
            break;
        }
    }

    return ZCL_STA_SUCCESS;
}

#endif

