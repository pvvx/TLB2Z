/********************************************************************************************************
 * @file    zb_appCb.c
 *
 *******************************************************************************************************/


/**********************************************************************
 * INCLUDES
 */
#include "tl_common.h"
#include "zb_api.h"
#include "zcl_include.h"
#include "bdb.h"
#include "ota.h"
#include "app.h"
#include "app_ui.h"
#include "zb_reporting.h"

/**********************************************************************
 * LOCAL CONSTANTS
 */


/**********************************************************************
 * TYPEDEFS
 */


/**********************************************************************
 * LOCAL FUNCTIONS
 */
void zbdemo_bdbInitCb(u8 status, u8 joinedNetwork);
void zbdemo_bdbCommissioningCb(u8 status, void *arg);
void zbdemo_bdbIdentifyCb(u8 endpoint, u16 srcAddr, u16 identifyTime);
void zbdemo_bdbFindBindSuccessCb(findBindDst_t *pDstInfo);

/**********************************************************************
 * LOCAL VARIABLES
 */
bdb_appCb_t g_zbDemoBdbCb =
{
	zbdemo_bdbInitCb,
	zbdemo_bdbCommissioningCb,
	zbdemo_bdbIdentifyCb,
	zbdemo_bdbFindBindSuccessCb
};

#ifdef ZCL_OTA
ota_callBack_t app_otaCb =
{
	app_otaProcessMsgHandler,
};
#endif

static bool ota_processing = FALSE;
/**********************************************************************
 * FUNCTIONS
 */
s32 app_bdbNetworkSteerStart(void *arg){

	bdb_networkSteerStart();

	g_devAppCtx.timerSteerEvt = NULL;
	return -1;
}

#if FIND_AND_BIND_SUPPORT
s32 app_bdbFindAndBindStart(void *arg){
	BDB_ATTR_GROUP_ID_SET(0x1234);//only for initiator
	bdb_findAndBindStart(BDB_COMMISSIONING_ROLE_INITIATOR);

	g_switchAppCtx.bdbFBTimerEvt = NULL;
	return -1;
}
#endif

s32 app_rejoinBackoff(void *arg){
    static bool rejoinMode = REJOIN_SECURITY;

	if(zb_isDeviceFactoryNew()){
		g_devAppCtx.timerRejoinBackoffEvt = NULL;
		return -1;
	}
    zb_rejoinSecModeSet(rejoinMode);
    zb_rejoinReq(zb_apsChannelMaskGet(), g_bdbAttrs.scanDuration);

    rejoinMode = !rejoinMode;
    return 0;
}

/*********************************************************************
 * @fn      zbdemo_bdbInitCb
 *
 * @brief   application callback for bdb initiation
 *
 * @param   status - the status of bdb init BDB_INIT_STATUS_SUCCESS or BDB_INIT_STATUS_FAILURE
 *
 * @param   joinedNetwork  - 1: node is on a network, 0: node isn't on a network
 *
 * @return  None
 */
void zbdemo_bdbInitCb(u8 status, u8 joinedNetwork){
	if(status == BDB_INIT_STATUS_SUCCESS){
		/*
		 * for non-factory-new device:
		 * 		load zcl data from NV, start poll rate, start ota query, bdb_networkSteerStart
		 *
		 * for factory-new device:
		 * 		steer a network
		 *
		 */
		if(joinedNetwork){
			zb_setPollRate(DEFAULT_POLL_RATE);

#ifdef ZCL_OTA
			ota_queryStart(OTA_PERIODIC_QUERY_INTERVAL);
#endif

#ifdef ZCL_POLL_CTRL
			app_zclCheckInStart();
#endif
		}else{
			u16 jitter = 0;
			do{
				jitter = zb_random() % 0x0fff;
			}while(jitter == 0);

			if(g_devAppCtx.timerSteerEvt){
				TL_ZB_TIMER_CANCEL(&g_devAppCtx.timerSteerEvt);
			}
			///time_soff = 0;
			g_devAppCtx.timerSteerEvt = TL_ZB_TIMER_SCHEDULE(app_bdbNetworkSteerStart, NULL, jitter);
		}
	}
#if REJOIN_FAILURE_TIMER
	else
	{
		if(joinedNetwork){
			zb_rejoinReqWithBackOff(zb_apsChannelMaskGet(), g_bdbAttrs.scanDuration);
		}
	}
#else
	else
	{
		if(joinedNetwork){
			if(!g_devAppCtx.timerRejoinBackoffEvt){
				g_devAppCtx.timerRejoinBackoffEvt = TL_ZB_TIMER_SCHEDULE(app_rejoinBackoff, NULL, 10 * 1000);
			}
		}
	}
#endif
}

/*********************************************************************
 * @fn      zbdemo_bdbCommissioningCb
 *
 * @brief   application callback for bdb commissioning
 *
 * @param   status - the status of bdb commissioning
 *
 * @param   arg
 *
 * @return  None
 */
void zbdemo_bdbCommissioningCb(u8 status, void *arg){
	switch(status){
		case BDB_COMMISSION_STA_SUCCESS:

			light_blink_stop();

			if(!ota_processing){
				zb_setPollRate(DEFAULT_POLL_RATE);
			}else{
				zb_setPollRate(QUEUE_POLL_RATE);
			}

			if(g_devAppCtx.timerSteerEvt){
				TL_ZB_TIMER_CANCEL(&g_devAppCtx.timerSteerEvt);
			}
			if(g_devAppCtx.timerRejoinBackoffEvt){
				TL_ZB_TIMER_CANCEL(&g_devAppCtx.timerRejoinBackoffEvt);
			}
			if(!g_zbNwkCtx.joined){
				zb_rejoinReq(zb_apsChannelMaskGet(), g_bdbAttrs.scanDuration);
			}
#ifdef ZCL_POLL_CTRL
		    app_zclCheckInStart();
#endif
#ifdef ZCL_OTA
			ota_queryStart(OTA_PERIODIC_QUERY_INTERVAL);
#endif
			sws_puts("BDB#sta_succes\n");
			light_blink_start(7, 200, 200);
			break;
		case BDB_COMMISSION_STA_IN_PROGRESS:
			break;
		case BDB_COMMISSION_STA_NOT_AA_CAPABLE:
			break;
		case BDB_COMMISSION_STA_NO_NETWORK:
		case BDB_COMMISSION_STA_TCLK_EX_FAILURE:
		case BDB_COMMISSION_STA_TARGET_FAILURE:
			{
				sws_printf("BDB#failure_%02x\n",status);
				u16 jitter = 0;
				do{
					jitter = zb_random() % 0x0fff;
				}while(jitter == 0);

				if(g_devAppCtx.timerSteerEvt){
					TL_ZB_TIMER_CANCEL(&g_devAppCtx.timerSteerEvt);
				}
				g_devAppCtx.timerSteerEvt = TL_ZB_TIMER_SCHEDULE(app_bdbNetworkSteerStart, NULL, jitter);
				light_blink_start(7, 500, 500);
			}
			break;
		case BDB_COMMISSION_STA_FORMATION_FAILURE:
			break;
		case BDB_COMMISSION_STA_NO_IDENTIFY_QUERY_RESPONSE:
			break;
		case BDB_COMMISSION_STA_BINDING_TABLE_FULL:
			break;
		case BDB_COMMISSION_STA_NOT_PERMITTED:
			break;
		case BDB_COMMISSION_STA_NO_SCAN_RESPONSE:
			break;
		case BDB_COMMISSION_STA_PARENT_LOST:
			sws_puts("BDB#parent_lost\n");
			// zb_rejoinSecModeSet(REJOIN_SECURITY); // only new SDK_z
			zb_rejoinReq(zb_apsChannelMaskGet(), g_bdbAttrs.scanDuration);
			light_blink_start(5, 500, 500);
			break;
		case BDB_COMMISSION_STA_REJOIN_FAILURE:
			sws_puts("BDB#rejoin_failure\n");
			if(!g_devAppCtx.timerRejoinBackoffEvt){
				g_devAppCtx.timerRejoinBackoffEvt = TL_ZB_TIMER_SCHEDULE(app_rejoinBackoff, NULL, 60 * 1000);
			}
			light_blink_start(7, 500, 500);
			break;
		default:
			break;
	}
}

extern void app_zclIdentifyCmdHandler(u8 endpoint, u16 srcAddr, u16 identifyTime);
void zbdemo_bdbIdentifyCb(u8 endpoint, u16 srcAddr, u16 identifyTime){
#if FIND_AND_BIND_SUPPORT
	app_zclIdentifyCmdHandler(endpoint, srcAddr, identifyTime);
#endif
}
/*********************************************************************
 * @fn      zbdemo_bdbFindBindSuccessCb
 *
 * @brief   application callback for finding & binding
 *
 * @param   pDstInfo
 *
 * @return  None
 */
void zbdemo_bdbFindBindSuccessCb(findBindDst_t *pDstInfo){
#if FIND_AND_BIND_SUPPORT
	epInfo_t dstEpInfo;
	TL_SETSTRUCTCONTENT(dstEpInfo, 0);

	dstEpInfo.dstAddrMode = APS_SHORT_DSTADDR_WITHEP;
	dstEpInfo.dstAddr.shortAddr = pDstInfo->addr;
	dstEpInfo.dstEp = pDstInfo->endpoint;
	dstEpInfo.profileId = HA_PROFILE_ID;

	zcl_identify_identifyCmd(SAMPLE_SWITCH_ENDPOINT, &dstEpInfo, FALSE, 0, 0);
#endif
}

#ifdef ZCL_OTA
void app_otaProcessMsgHandler(u8 evt, u8 status)
{
	//printf("sampleSwitch_otaProcessMsgHandler: status = %x\n", status);
	ota_processing = FALSE;
	if(evt == OTA_EVT_START){
		if(status == ZCL_STA_SUCCESS){
			ota_processing = TRUE;
			zb_setPollRate(QUEUE_POLL_RATE);
		} else {

		}
	} else if(evt == OTA_EVT_COMPLETE){
		zb_setPollRate(POLL_RATE * 3);

		if(status == ZCL_STA_SUCCESS){
			ota_mcuReboot();
		} else {
			ota_queryStart(OTA_PERIODIC_QUERY_INTERVAL);
		}
	} else if(evt == OTA_EVT_IMAGE_DONE){
		zb_setPollRate(POLL_RATE * 3);
	}
}
#endif

/*********************************************************************
 * @fn      app_leaveCnfHandler
 *
 * @brief   Handler for ZDO Leave Confirm message.
 *
 * @param   pRsp - parameter of leave confirm
 *
 * @return  None
 */
void app_leaveCnfHandler(nlme_leave_cnf_t *pLeaveCnf)
{
	sws_printf("ZDO#leaveCnfHandler: %d\n", pLeaveCnf->status);
    if(pLeaveCnf->status == SUCCESS){
    	/*
		if(g_devAppCtx.timerRejoinBackoffEvt) {
			TL_ZB_TIMER_CANCEL(&g_devAppCtx.timerRejoinBackoffEvt);
		}
		*/
		SYSTEM_RESET();
    }
}

/*********************************************************************
 * @fn      app_leaveIndHandler
 *
 * @brief   Handler for ZDO leave indication message.
 *
 * @param   pInd - parameter of leave indication
 *
 * @return  None
 */
void app_leaveIndHandler(nlme_leave_ind_t *pLeaveInd)
{
    sws_printf("ZDO#leaveIndHandler: %d\n", pLeaveInd->rejoin);
    //printfArray(pLeaveInd->device_address, 8);
}


