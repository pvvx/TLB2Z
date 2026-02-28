/**********************************************************************
 * INCLUDES
 */
#include "tl_common.h"
#include "app.h"
#include "zb_api.h"
#include "zcl_include.h"
#include "bdb.h"
#include "ota.h"
#if ZBHCI_EN
#include "zbhci.h"
#endif

#include "app_ui.h"
#include "zcl_relative_humidity.h"
#include "chip_8258/timer.h"
#include "zb_reporting.h"
#include "ble_cfg.h"
#include "ble_scaning.h"
#include "flash_eep.h"

/**********************************************************************
 * LOCAL CONSTANTS
 */

/**
 *  @brief Definition for BDB finding and binding cluster
 */
u16 bdb_findBindClusterList[] =
{
	ZCL_CLUSTER_GEN_ON_OFF,
};

/**
 *  @brief Definition for BDB finding and binding cluster number
 */
#define FIND_AND_BIND_CLUSTER_NUM		(sizeof(bdb_findBindClusterList)/sizeof(bdb_findBindClusterList[0]))

/**********************************************************************
 * TYPEDEFS
 */


/**********************************************************************
 * GLOBAL VARIABLES
 */
app_ctx_t g_sensorAppCtx;

#if ZCL_OTA_SUPPORT
extern ota_callBack_t app_otaCb;

//running code firmware information
ota_preamble_t app_otaInfo = {
	.fileVer 			= FILE_VERSION,
	.imageType 			= IMAGE_TYPE,
	.manufacturerCode 	= MANUFACTURER_CODE_TELINK,
};
#endif


/**********************************************************************
 * LOCAL VARIABLES
 */

/* Must declare the application call back function which used by ZDO layer */
const zdo_appIndCb_t appCbLst = {
	bdb_zdoStartDevCnf,//start device cnf cb
	NULL,//reset cnf cb
	NULL,//device announce indication cb
	app_leaveIndHandler, //leave ind cb
	app_leaveCnfHandler, //leave cnf cb
	NULL,//nwk update ind cb
	NULL,//permit join ind cb
	NULL,//nlme sync cnf cb
	NULL,//tc join ind cb
	NULL,//tc detects that the frame counter is near limit
};

/**
 *  @brief Definition for bdb commissioning setting
 */
bdb_commissionSetting_t g_bdbCommissionSetting = {
	.linkKey.tcLinkKey.keyType = SS_GLOBAL_LINK_KEY,
	.linkKey.tcLinkKey.key = (u8 *)tcLinkKeyCentralDefault,       		//can use unique link key stored in NV

	.linkKey.distributeLinkKey.keyType = MASTER_KEY,
	.linkKey.distributeLinkKey.key = (u8 *)linkKeyDistributedMaster,  	//use linkKeyDistributedCertification before testing

	.linkKey.touchLinkKey.keyType = MASTER_KEY,
	.linkKey.touchLinkKey.key = (u8 *)touchLinkKeyMaster,   			//use touchLinkKeyCertification before testing

#if TOUCHLINK_SUPPORT
	.touchlinkEnable = 1,												/* enable touch-link */
#else
	.touchlinkEnable = 0,												/* disable touch-link */
#endif
	.touchlinkChannel = DEFAULT_CHANNEL, 								/* touch-link default operation channel for target */
	.touchlinkLqiThreshold = 0xA0,			   							/* threshold for touch-link scan req/resp command */
};

/**********************************************************************
 * FUNCTIONS
 */
static void app_zb_task(void);

char int_to_hex(u8 num) {
	const char * hex_ascii = {"0123456789ABCDEF"};
	if (num > 15) return hex_ascii[0];
	return hex_ascii[num];
}

static void app_SysException(void)
{
	sws_puts("SysException\n");
#ifdef GPIO_LED
	irq_disable();
	for(int i = 0; i < 20; i++) {
		gpio_write(GPIO_LED, LED_ON);
		sleep_ms(50);
		gpio_write(GPIO_LED, LED_OFF);
		sleep_ms(50);
	}
#endif
	SYSTEM_RESET();
}

/*********************************************************************
 * @fn      user_app_init
 *
 * @brief   This function initialize user application
 *
 * @param   None
 *
 * @return  None
 */
static void user_app_init(void)
{
	u32 reportableChange;
	/* Populate properties with compiled-in values */
	populate_date_code();
#if USE_EEP
	if (flash_supported_eep_ver(IMAGE_TYPE, (APP_RELEASE<<24) | (APP_BUILD)<<16) | IMAGE_TYPE) {
		// next start
		for(int i = 0; i < MAX_SCAN_DEVS; i++) {
			flash_read_cfg(dev_MAC[i], 0, EEP_ID_DMAC(i), 6);
#if USE_BINDKEY
			flash_read_cfg(bindkey[i], 0, EEP_ID_BKEY(i), 16);
#endif
#ifdef ZCL_ON_OFF
			zcl_onOffAttr_restore(i);
#ifdef GPIO_RELAY
			app_onOffUpdate(g_zcl_onOffAttrs[i].onOff, i);
#endif
#endif
		}
		zcl_onOffTypeAttr_restore();
	}
#else
	for(int i = 0; i < MAX_SCAN_DEVS; i++) {
		nv_flashReadNew(1, NV_MODULE_APP, NV_ITEM_APP_BLE_MAC + i,
		        		6,
						dev_MAC[i]);
#if USE_BINDKEY
		nv_flashReadNew(1, NV_MODULE_APP, NV_ITEM_APP_BLE_KEY + i,
		        		16,
						bindkey[i]);
#endif
#ifdef ZCL_ON_OFF
		zcl_onOffAttr_restore(i);
#ifdef GPIO_RELAY
	    app_onOffUpdate(g_zcl_onOffAttrs[i].onOff, i);
#endif
#endif
	}
#endif
#ifdef ZCL_CUSTOM_ATTR_ILLUMINANCE_LEVEL
	zcl_illuminance_restore();
#endif
	/* Initialize ZB stack */
	zb_init();
	/* Register stack CB */
	zb_zdoCbRegister((zdo_appIndCb_t *)&appCbLst);

	/* Register except handler for test */
	sys_exceptHandlerRegister(app_SysException);

#if ZCL_POLL_CTRL_SUPPORT
	af_powerDescPowerModeUpdate(POWER_MODE_RECEIVER_COMES_PERIODICALLY);
#else
	af_powerDescPowerModeUpdate(POWER_MODE_RECEIVER_COMES_WHEN_STIMULATED);
#endif

	/* Initialize ZCL layer */
	/* Register Incoming ZCL Foundation command/response messages */
	zcl_init(app_zclProcessIncomingMsg);

	/* Register endPoint */
	af_endpointRegister(APP_ENDPOINT1, (af_simple_descriptor_t *)&app_simpleDesc1, zcl_rx_handler, NULL);
	af_endpointRegister(APP_ENDPOINT2, (af_simple_descriptor_t *)&app_simpleDesc2, zcl_rx_handler, NULL);
	af_endpointRegister(APP_ENDPOINT3, (af_simple_descriptor_t *)&app_simpleDesc3, zcl_rx_handler, NULL);

	zcl_reportingTabInit();

	/* Register ZCL specific cluster information */
	zcl_register(APP_ENDPOINT1, APP_CB_CLUSTER_NUM1, (zcl_specClusterInfo_t *)g_appClusterList1);
	zcl_register(APP_ENDPOINT2, APP_CB_CLUSTER_NUM2, (zcl_specClusterInfo_t *)g_appClusterList2);
	zcl_register(APP_ENDPOINT3, APP_CB_CLUSTER_NUM3, (zcl_specClusterInfo_t *)g_appClusterList3);

#if ZCL_OTA_SUPPORT
    ota_init(OTA_TYPE_CLIENT, (af_simple_descriptor_t *)&app_simpleDesc1, &app_otaInfo, &app_otaCb);
#endif

	/* User's Task */
#if ZBHCI_EN
	ev_on_poll(EV_POLL_HCI, zbhciTask);
#endif
	ev_on_poll(EV_POLL_IDLE, app_zb_task);

	/* Load the pre-install code from flash */
	if(bdb_preInstallCodeLoad(&g_sensorAppCtx.tcLinkKey.keyType, g_sensorAppCtx.tcLinkKey.key) == RET_OK){
		g_bdbCommissionSetting.linkKey.tcLinkKey.keyType = g_sensorAppCtx.tcLinkKey.keyType;
		g_bdbCommissionSetting.linkKey.tcLinkKey.key = g_sensorAppCtx.tcLinkKey.key;
	}
	/* Set default reporting configuration */
	reportableChange = 0;
	for(int i = 0; i <= 0x40; i += 0x20) {
		bdb_defaultReportingCfg(
	    	APP_ENDPOINT1,
			HA_PROFILE_ID,
			ZCL_CLUSTER_GEN_POWER_CFG,
			i + ZCL_ATTRID_BATTERY_VOLTAGE,
			360,
			3600,
			(u8 *)&reportableChange
		);
	    bdb_defaultReportingCfg(
	    	APP_ENDPOINT1,
			HA_PROFILE_ID,
			ZCL_CLUSTER_GEN_POWER_CFG,
			i + ZCL_ATTRID_BATTERY_PERCENTAGE_REMAINING,
			360,
			3600,
			(u8 *)&reportableChange
		);
	}
    for(int i=APP_ENDPOINT1; i <= APP_ENDPOINT3; i++) {
#ifdef ZCL_ON_OFF
        /* OnOff */
    	reportableChange = 0;
        bdb_defaultReportingCfg(
        	i,
    		HA_PROFILE_ID,
    		ZCL_CLUSTER_GEN_ON_OFF,
    		ZCL_ATTRID_ONOFF,
    		0,
    		3600,
    		(u8 *)&reportableChange);
#endif

#ifdef ZCL_ILLUMINANCE_MEASUREMENT
        reportableChange = 10;
		bdb_defaultReportingCfg(
			i,
			HA_PROFILE_ID,
			ZCL_CLUSTER_MS_ILLUMINANCE_MEASUREMENT,
			ZCL_ATTRID_MEASURED_VALUE,
			30,
			180,
			(u8 *)&reportableChange
		);
#endif

        reportableChange = 10;
		bdb_defaultReportingCfg(
			i,
			HA_PROFILE_ID,
			ZCL_CLUSTER_MS_TEMPERATURE_MEASUREMENT,
			ZCL_TEMPERATURE_MEASUREMENT_ATTRID_MEASUREDVALUE,
			30,
			180,
			(u8 *)&reportableChange
		);

        reportableChange = 50;
		bdb_defaultReportingCfg(
			i,
			HA_PROFILE_ID,
			ZCL_CLUSTER_MS_RELATIVE_HUMIDITY,
			ZCL_RELATIVE_HUMIDITY_ATTRID_MEASUREDVALUE,
			30,
			180,
			(u8 *)&reportableChange
		);
    }

	bdb_findBindMatchClusterSet(FIND_AND_BIND_CLUSTER_NUM, bdb_findBindClusterList);

	/* Initialize BDB */
	u8 repower = drv_pm_deepSleep_flag_get() ? 0 : 1;
	bdb_init((af_simple_descriptor_t *)&app_simpleDesc1, &g_bdbCommissionSetting, &g_zbDemoBdbCb, repower);

#if 0 // Go zb_context
	u32 r = drv_disable_irq();
	switch_to_zb_context();
	drv_restore_irq(r);
#endif
}
/*********************************************************************
 * @fn      user_init
 *
 * @brief   User level initialization code.
 *
 * @param   isRetention - if it is waking up with ram retention.
 *
 * @return  None
 */
void user_init(bool isRetention)
{
#if ZBHCI_EN
	zbhciInit();
#endif
#if PM_ENABLE
	if(!isRetention){
		/* Initialize user application */
		user_app_init();
	} else {
		/* Re-config phy when system recovery from deep sleep with retention */
		mac_phyReconfig();
	}
#else
	/* Initialize user application */
	user_app_init();
#endif
}

#ifdef ZCL_ON_OFF
/**********************************************************************
 * @fn      test_ble_trigger
 *
 * @brief   test ble_trigger -> Relay On/Off
 *
 * @param   None
 *
 * @return  None
 */
static void test_ble_trigger(void) {
	u8 on_state;
	for(int n = 0; n < MAX_SCAN_DEVS; n++) {
		zcl_onOffAttr_t *pOnOff = zcl_onoffAttrGet(n);
		if(dev_MAC[n][5] && pOnOff->onOff) {
			// работа от BLE
			if(update_enable[n] & FLG_UPDATE_TRG) {
				update_enable[n] &= ~FLG_UPDATE_TRG;
				on_state = (pOnOff->ble_trigger)? ZCL_CMD_ONOFF_ON : ZCL_CMD_ONOFF_OFF;
				if(on_state != pOnOff->onOffrm) {
					sws_printf("OnOffrm %d:%d\n", n + 1, on_state);
#ifdef GPIO_RELAY
					if(!n) {
						gpio_write(GPIO_RELAY, on_state);
						g_sensorAppCtx.oriSta = on_state;
						if(on_state){
							light_on();
						}else{
							light_off();
						}
					}
#endif
					pOnOff->onOffrm = on_state;
					remoteCmdOnOff(APP_ENDPOINT1 + n, on_state);
					pOnOff->ble_trigger_tik = g_sensorAppCtx.utc_time_sec;
				}
			} else if(pOnOff->ble_trigger_tik) {
				if(g_sensorAppCtx.utc_time_sec - pOnOff->ble_trigger_tik >= 3) {
					sws_printf("OnOff2rm %d:%d\n", n + 1, pOnOff->onOffrm);
					remoteCmdOnOff(APP_ENDPOINT1 + n, pOnOff->onOffrm);
					pOnOff->ble_trigger_tik = 0;
				}
			}
		}
	}
}
#endif
/**********************************************************************
 * @fn      app_zb_task
 *
 * @brief   app_zb_task
 *
 * @param   None
 *
 * @return  None
 */
static void app_zb_task(void)
{
	if(bdb_isIdle()){
		// report handler
		if(zb_isDeviceJoinedNwk()){
			while(clock_time() - g_sensorAppCtx.secTimeTik >= CLOCK_16M_SYS_TIMER_CLK_1S) {
				g_sensorAppCtx.secTimeTik += CLOCK_16M_SYS_TIMER_CLK_1S;
				g_sensorAppCtx.reportupsec++; // + 1 sec
				g_sensorAppCtx.utc_time_sec++;
			}
			if(g_sensorAppCtx.reportupsec >= MIN_REPORT_INTERVAL) {
				app_chk_report(g_sensorAppCtx.reportupsec);
				g_sensorAppCtx.reportupsec = 0;
			}
#ifdef ZCL_ON_OFF
			else
				test_ble_trigger();
#endif
		} else {
			g_sensorAppCtx.reportupsec = 0;
			if(!g_sensorAppCtx.timerLedEvt)
				light_blink_start(128, 500, 500);
		}
	}
}

