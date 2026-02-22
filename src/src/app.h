#ifndef _DEVICE_H_
#define _DEVICE_H_

#include "zcl_include.h"

/**********************************************************************
 * CONSTANT
 */
#define APP_ENDPOINT1  0x01
#define APP_ENDPOINT2  0x02
#define APP_ENDPOINT3  0x03 // MAX_SCAN_DEVS

/**********************************************************************
 * TYPEDEFS
 */
typedef struct{
	u8 keyType; /* ERTIFICATION_KEY or MASTER_KEY key for touch-link or distribute network
	 	 	 	 SS_UNIQUE_LINK_KEY or SS_GLOBAL_LINK_KEY for distribute network */
	u8 key[16];	/* the key used */
}app_linkKey_info_t;

typedef struct{
	// key
	u8  keyPressed;
	u8  key1flag;	// switch ble work
	volatile u8  ble_on; // adv/connect ble work
	u8  adv_restore_count; // count adv
	//
	u32 utc_time_sec;
	u16 reportupsec; // report add (sec)
	u32 secTimeTik; // used time_sec_task()
	u32 keyPressedTime;
	// timers
	ev_timer_event_t *timerLedEvt;	// timer light blink
	ev_timer_event_t *timerKeyEvt;	// timer key press
	ev_timer_event_t *timerRejoinBackoffEvt;
	ev_timer_event_t *timerSteerEvt;

#ifdef ZCL_IDENTIFY
	ev_timer_event_t *timerIdentifyEvt;
#endif

	// light blink
	u16 ledOnTime;
	u16 ledOffTime;
	u8 	oriSta;		// original state before blink
	u8 	sta;		// current state in blink
	u8 	times;		// blink times
	u8  state;
	//
	app_linkKey_info_t tcLinkKey;
} app_ctx_t;

/**
 *  @brief Defined for basic cluster attributes
 */
typedef struct{
	u8 	zclVersion;
	u8	appVersion;
	u8	stackVersion;
	u8	hwVersion;
	u8	manuName[ZCL_BASIC_MAX_LENGTH];
	u8	modelId[ZCL_BASIC_MAX_LENGTH];
	u8	swBuildId[ZCL_BASIC_MAX_LENGTH];
 	u8  dateCode[ZCL_BASIC_MAX_LENGTH];
	u8	powerSource;
	u8	deviceEnable;
}zcl_basicAttr_t;

/**
 *  @brief Defined for identify cluster attributes
 */
typedef struct{
	u16	identifyTime;
}zcl_identifyAttr_t;

/**
 *  @brief Defined for power configuration cluster attributes
 */
typedef struct{
#ifdef POWER_MAINS
	u16 mainsVoltage;
	u8  mainsFrequency;
#endif
	u8  batteryVoltage;      //0x20
	u8  batteryPercentage;   //0x21
}zcl_powerAttr_t;

typedef struct{
	u16 measuredVal[MAX_SCAN_DEVS];
	u16 minMeasuredVal;
	u16 maxMeasuredVal;
#ifdef ZCL_ATTR_TOLERANCE_ENABLE
	u16 tolerance;
#endif
#ifdef ZCL_ATTR_LIGHT_SENSOR_TYPE_ENABLE
	u8 lightSensorType;
#endif
} zcl_illuminanceAttr_t;

/**
 *  @brief Defined for on/off cluster attributes
 */
typedef struct{
	u32 ble_trigger_tik[MAX_SCAN_DEVS];
	u8  onOff[MAX_SCAN_DEVS];
	u8  ble_trigger[MAX_SCAN_DEVS];
	u16	onTime;
	u16	offWaitTime;
	u8	startUpOnOff;
	u8  globalSceneControl;
}zcl_onOffAttr_t;

/**
 *  @brief Defined for saving on/off attributes
 */
typedef struct _attribute_packed_{
	u8	onOff;
	u8	startUpOnOff;
}zcl_nv_onOff_t;

/**
 *  @brief Defined for temperature cluster attributes
 */
typedef struct {
	s16 measuredValue[MAX_SCAN_DEVS];
	s16 minValue;
	s16 maxValue;
	u16 tolerance;
}zcl_temperatureAttr_t;

/**
 *  @brief Defined for relative humidity cluster attributes
 */
typedef struct {
	u16 measuredValue[MAX_SCAN_DEVS];
	u16 minValue;
	u16 maxValue;
	u16 tolerance;
}zcl_relHumidityAttr_t;

/**
 *  @brief  Defined for poll control cluster attributes
 */
typedef struct{
	u32	chkInInterval;
	u32	longPollInterval;
	u32	chkInIntervalMin;
	u32	longPollIntervalMin;
	u16	shortPollInterval;
	u16	fastPollTimeout;
	u16	fastPollTimeoutMax;
}zcl_pollCtrlAttr_t;


/**********************************************************************
 * GLOBAL VARIABLES
 */
extern app_ctx_t g_sensorAppCtx;

extern bdb_appCb_t g_zbDemoBdbCb;

extern bdb_commissionSetting_t g_bdbCommissionSetting;

extern const u8 APP_CB_CLUSTER_NUM1;
extern const zcl_specClusterInfo_t g_appClusterList1[];
extern const af_simple_descriptor_t app_simpleDesc1;
extern const u8 APP_CB_CLUSTER_NUM2;
extern const zcl_specClusterInfo_t g_appClusterList2[];
extern const af_simple_descriptor_t app_simpleDesc2;
extern const u8 APP_CB_CLUSTER_NUM3;
extern const zcl_specClusterInfo_t g_appClusterList3[];
extern const af_simple_descriptor_t app_simpleDesc3;


/* Attributes */
extern zcl_basicAttr_t g_zcl_basicAttrs;
extern zcl_identifyAttr_t g_zcl_identifyAttrs;
extern zcl_powerAttr_t g_zcl_powerAttrs[];
#ifdef ZCL_TEMPERATURE_MEASUREMENT
extern zcl_temperatureAttr_t g_zcl_temperatureAttrs;
#endif
#ifdef ZCL_RELATIVE_HUMIDITY_MEASUREMENT
extern zcl_relHumidityAttr_t g_zcl_relHumidityAttrs;
#endif
#ifdef ZCL_ILLUMINANCE_MEASUREMENT
extern zcl_illuminanceAttr_t	g_zcl_illuminanceAttrs;
#endif
extern zcl_pollCtrlAttr_t g_zcl_pollCtrlAttrs;
#ifdef ZCL_ON_OFF
extern zcl_onOffAttr_t g_zcl_onOffAttrs;
#define zcl_onoffAttrGet()      &g_zcl_onOffAttrs
#endif

#define zcl_iasZoneAttrGet()	&g_zcl_iasZoneAttrs
#define zcl_pollCtrlAttrGet()	&g_zcl_pollCtrlAttrs

/**********************************************************************
 * FUNCTIONS
 */
#if 0
#define pm_wait_ms(t) cpu_stall_wakeup_by_timer0(t*CLOCK_16M_SYS_TIMER_CLK_1MS);
#define pm_wait_us(t) cpu_stall_wakeup_by_timer0(t*CLOCK_16M_SYS_TIMER_CLK_1US);
#else
#define pm_wait_ms(t) sleep_us((t)*1000);
#define pm_wait_us(t) sleep_us(t);
#endif

void switch_to_zb_context(void);

void populate_date_code(void);
void app_zclProcessIncomingMsg(zclIncoming_t *pInHdlrMsg);

status_t app_basicCb(zclIncomingAddrInfo_t *pAddrInfo, u8 cmdId, void *cmdPayload);
status_t app_identifyCb(zclIncomingAddrInfo_t *pAddrInfo, u8 cmdId, void *cmdPayload);
status_t app_iasZoneCb(zclIncomingAddrInfo_t *pAddrInfo, u8 cmdId, void *cmdPayload);
status_t app_powerCfgCb(zclIncomingAddrInfo_t *pAddrInfo, u8 cmdId, void *cmdPayload);
status_t app_pollCtrlCb(zclIncomingAddrInfo_t *pAddrInfo, u8 cmdId, void *cmdPayload);
status_t app_groupCb(zclIncomingAddrInfo_t *pAddrInfo, u8 cmdId, void *cmdPayload);
void app_zclCheckInStart(void);

void app_leaveCnfHandler(nlme_leave_cnf_t *pLeaveCnf);
void app_leaveIndHandler(nlme_leave_ind_t *pLeaveInd);
void app_otaProcessMsgHandler(u8 evt, u8 status);

status_t app_onOffCb(zclIncomingAddrInfo_t *pAddrInfo, u8 cmdId, void *cmdPayload);
void app_onOffUpdate(u8 cmd);
void remoteCmdOnOff(u8 srcEp, u8 cmd);
void app_onOffInit(void);
nv_sts_t zcl_onOffAttr_restore(void);
nv_sts_t zcl_onOffAttr_save(void);

void scan_task(void);
int blt_pm_proc(void);
int zb_ble_ci_cmd_handler(u16 cmdId, u8 len, u8 *payload);
char int_to_hex(u8 num);
#if ZIGBEE_TUYA_OTA
void tuya_zigbee_ota(void);
#endif
#endif /* _DEVICE_H_ */
