/**********************************************************************
 * INCLUDES
 */
#include "tl_common.h"
#include "zcl_include.h"
#include "zcl_relative_humidity.h"
//#include "zcl_illuminance_measurement.h"
#include "zcl_thermostat_ui_cfg.h"
#include "app.h"
#include "adv_bthome.h"
#include "flash_eep.h"

/**********************************************************************
 * LOCAL CONSTANTS
 */

#ifndef ZCL_BASIC_SW_BUILD_ID

#define ZCL_BASIC_SW_BUILD_ID	{9 \
								,'0'+(STACK_RELEASE>>4) \
								,'0'+(STACK_RELEASE & 0xf) \
								,'0'+(STACK_BUILD>>4) \
								,'0'+(STACK_BUILD & 0xf) \
								,'-' \
								,'0'+(APP_RELEASE>>4) \
								,'0'+(APP_RELEASE & 0xf) \
								,'0'+(APP_BUILD>>4) \
								,'0'+(APP_BUILD & 0xf) \
								}

#endif

#ifndef ZCL_BASIC_DATE_CODE
#define ZCL_BASIC_DATE_CODE     	{8,'0','0','0','0','0','0','0','0'}
#endif


/**********************************************************************
 * TYPEDEFS
 */


/**********************************************************************
 * GLOBAL VARIABLES
 */
/**
 *  @brief Definition for Incoming cluster / Sever Cluster
 */
const u16 app_inClusterList1[] =
{
	ZCL_CLUSTER_GEN_BASIC,
	ZCL_CLUSTER_GEN_POWER_CFG,
#ifdef ZCL_IDENTIFY
	ZCL_CLUSTER_GEN_IDENTIFY,
#endif
#ifdef ZCL_ON_OFF
	ZCL_CLUSTER_GEN_ON_OFF,
#endif
#ifdef ZCL_POLL_CTRL
	ZCL_CLUSTER_GEN_POLL_CONTROL,
#endif
#ifdef ZCL_ILLUMINANCE_MEASUREMENT
	ZCL_CLUSTER_MS_ILLUMINANCE_MEASUREMENT,
#endif
#ifdef ZCL_TEMPERATURE_MEASUREMENT
	ZCL_CLUSTER_MS_TEMPERATURE_MEASUREMENT,
#endif
#ifdef ZCL_RELATIVE_HUMIDITY_MEASUREMENT
    ZCL_CLUSTER_MS_RELATIVE_HUMIDITY,
#endif
#ifdef ZCL_IAS_ZONE
	ZCL_CLUSTER_SS_IAS_ZONE,
#endif
};

const u16 app_inClusterList2[] =
{
#ifdef ZCL_ON_OFF
	ZCL_CLUSTER_GEN_ON_OFF,
#endif
#ifdef ZCL_ILLUMINANCE_MEASUREMENT
	ZCL_CLUSTER_MS_ILLUMINANCE_MEASUREMENT,
#endif
#ifdef ZCL_TEMPERATURE_MEASUREMENT
	ZCL_CLUSTER_MS_TEMPERATURE_MEASUREMENT,
#endif
#ifdef ZCL_RELATIVE_HUMIDITY_MEASUREMENT
    ZCL_CLUSTER_MS_RELATIVE_HUMIDITY,
#endif
};

const u16 app_inClusterList3[] =
{
#ifdef ZCL_ON_OFF
	ZCL_CLUSTER_GEN_ON_OFF,
#endif
#ifdef ZCL_ILLUMINANCE_MEASUREMENT
	ZCL_CLUSTER_MS_ILLUMINANCE_MEASUREMENT,
#endif
#ifdef ZCL_TEMPERATURE_MEASUREMENT
	ZCL_CLUSTER_MS_TEMPERATURE_MEASUREMENT,
#endif
#ifdef ZCL_RELATIVE_HUMIDITY_MEASUREMENT
    ZCL_CLUSTER_MS_RELATIVE_HUMIDITY,
#endif
};

/**
 *  @brief Definition for Outgoing cluster / Client Cluster
 */
const u16 app_outClusterList1[] =
{
#ifdef ZCL_GROUP
	ZCL_CLUSTER_GEN_GROUPS,
#endif
#ifdef ZCL_ON_OFF
	ZCL_CLUSTER_GEN_ON_OFF,
#endif
#ifdef ZCL_OTA
    ZCL_CLUSTER_OTA,
#endif
#ifdef ZCL_ZLL_COMMISSIONING
	ZCL_CLUSTER_TOUCHLINK_COMMISSIONING,
#endif
};
const u16 app_outClusterList2[] =
{
#ifdef ZCL_ON_OFF
	ZCL_CLUSTER_GEN_ON_OFF,
#endif
};
const u16 app_outClusterList3[] =
{
#ifdef ZCL_ON_OFF
	ZCL_CLUSTER_GEN_ON_OFF,
#endif
};

/**
 *  @brief Definition for Server cluster number and Client cluster number
 */
#define app_IN_CLUSTER_NUM1		(sizeof(app_inClusterList1)/sizeof(app_inClusterList1[0]))
#define app_OUT_CLUSTER_NUM1	(sizeof(app_outClusterList1)/sizeof(app_outClusterList1[0]))
#define app_IN_CLUSTER_NUM2		(sizeof(app_inClusterList2)/sizeof(app_inClusterList2[0]))
#define app_OUT_CLUSTER_NUM2	(sizeof(app_outClusterList2)/sizeof(app_outClusterList2[0]))
#define app_IN_CLUSTER_NUM3		(sizeof(app_inClusterList3)/sizeof(app_inClusterList3[0]))
#define app_OUT_CLUSTER_NUM3	(sizeof(app_outClusterList3)/sizeof(app_outClusterList3[0]))

/**
 *  @brief Definition for simple description for HA profile
 */
const af_simple_descriptor_t app_simpleDesc1 =
{
	HA_PROFILE_ID,              /* Application profile identifier */
	HA_DEV_TEMPERATURE_SENSOR,  /* Application device identifier */
	APP_ENDPOINT1,         		/* Endpoint */
	1,							/* Application device version */
	0,							/* Reserved */
	app_IN_CLUSTER_NUM1,        /* Application input cluster count */
	app_OUT_CLUSTER_NUM1,       /* Application output cluster count */
	(u16 *)app_inClusterList1,  /* Application input cluster list */
	(u16 *)app_outClusterList1  /* Application output cluster list */
};
const af_simple_descriptor_t app_simpleDesc2 =
{
	HA_PROFILE_ID,             	/* Application profile identifier */
	HA_DEV_TEMPERATURE_SENSOR,  /* Application device identifier */
	APP_ENDPOINT2,         		/* Endpoint */
	1,							/* Application device version */
	0,							/* Reserved */
	app_IN_CLUSTER_NUM2,        /* Application input cluster count */
	app_OUT_CLUSTER_NUM2,       /* Application output cluster count */
	(u16 *)app_inClusterList2,  /* Application input cluster list */
	(u16 *)app_outClusterList2  /* Application output cluster list */
};
const af_simple_descriptor_t app_simpleDesc3 =
{
	HA_PROFILE_ID,              /* Application profile identifier */
	HA_DEV_TEMPERATURE_SENSOR,  /* Application device identifier */
	APP_ENDPOINT3,         		/* Endpoint */
	1,							/* Application device version */
	0,							/* Reserved */
	app_IN_CLUSTER_NUM3,        /* Application input cluster count */
	app_OUT_CLUSTER_NUM3,       /* Application output cluster count */
	(u16 *)app_inClusterList3,  /* Application input cluster list */
	(u16 *)app_outClusterList3  /* Application output cluster list */
};


/* Basic */
zcl_basicAttr_t g_zcl_basicAttrs =
{
	.zclVersion 	= 0x03,
	.appVersion 	= APP_BUILD,
	.stackVersion 	= (STACK_RELEASE|STACK_BUILD),
	.hwVersion		= BOARD,
	.manuName		= ZCL_BASIC_MFG_NAME,
	.modelId		= ZCL_BASIC_MODEL_ID,
#ifdef ZCL_BASIC_SW_BUILD_ID
	.swBuildId		= ZCL_BASIC_SW_BUILD_ID,
#endif
#ifdef ZCL_BASIC_DATE_CODE
	.dateCode 		= ZCL_BASIC_DATE_CODE,
#endif
	.powerSource	= POWER_SOURCE_BATTERY,
	.deviceEnable	= TRUE
};

const zclAttrInfo_t basic_attrTbl[] =
{
	{ ZCL_ATTRID_BASIC_ZCL_VER,      		ZCL_DATA_TYPE_UINT8,    ACCESS_CONTROL_READ,  						(u8*)&g_zcl_basicAttrs.zclVersion},
	{ ZCL_ATTRID_BASIC_APP_VER,      		ZCL_DATA_TYPE_UINT8,    ACCESS_CONTROL_READ,  						(u8*)&g_zcl_basicAttrs.appVersion},
	{ ZCL_ATTRID_BASIC_STACK_VER,    		ZCL_DATA_TYPE_UINT8,    ACCESS_CONTROL_READ,  						(u8*)&g_zcl_basicAttrs.stackVersion},
	{ ZCL_ATTRID_BASIC_HW_VER,       		ZCL_DATA_TYPE_UINT8,    ACCESS_CONTROL_READ,  						(u8*)&g_zcl_basicAttrs.hwVersion},
	{ ZCL_ATTRID_BASIC_MFR_NAME,     		ZCL_DATA_TYPE_CHAR_STR, ACCESS_CONTROL_READ,  						(u8*)g_zcl_basicAttrs.manuName},
	{ ZCL_ATTRID_BASIC_MODEL_ID,     		ZCL_DATA_TYPE_CHAR_STR, ACCESS_CONTROL_READ,  						(u8*)g_zcl_basicAttrs.modelId},
	{ ZCL_ATTRID_BASIC_POWER_SOURCE, 		ZCL_DATA_TYPE_ENUM8,    ACCESS_CONTROL_READ,  						(u8*)&g_zcl_basicAttrs.powerSource},
	{ ZCL_ATTRID_BASIC_DEV_ENABLED,  		ZCL_DATA_TYPE_BOOLEAN,  ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE, (u8*)&g_zcl_basicAttrs.deviceEnable},
#ifdef ZCL_BASIC_SW_BUILD_ID
	{ ZCL_ATTRID_BASIC_SW_BUILD_ID,  		ZCL_DATA_TYPE_CHAR_STR, ACCESS_CONTROL_READ,  						(u8*)&g_zcl_basicAttrs.swBuildId},
#endif
#ifdef ZCL_BASIC_DATE_CODE
	{ ZCL_ATTRID_BASIC_DATE_CODE,           ZCL_DATA_TYPE_CHAR_STR, ACCESS_CONTROL_READ,                        (u8*)g_zcl_basicAttrs.dateCode},
#endif
	{ ZCL_ATTRID_GLOBAL_CLUSTER_REVISION, 	ZCL_DATA_TYPE_UINT16,  	ACCESS_CONTROL_READ,  						(u8*)&zcl_attr_global_clusterRevision},
};

#define ZCL_BASIC_ATTR_NUM sizeof(basic_attrTbl) / sizeof(zclAttrInfo_t)

#ifdef ZCL_IDENTIFY
/* Identify */
zcl_identifyAttr_t g_zcl_identifyAttrs =
{
	.identifyTime	= 0x0000,
};

const zclAttrInfo_t identify_attrTbl[] =
{
	{ ZCL_ATTRID_IDENTIFY_TIME,  			ZCL_DATA_TYPE_UINT16,   ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE, (u8*)&g_zcl_identifyAttrs.identifyTime },

	{ ZCL_ATTRID_GLOBAL_CLUSTER_REVISION, 	ZCL_DATA_TYPE_UINT16,  	ACCESS_CONTROL_READ,  						(u8*)&zcl_attr_global_clusterRevision},
};

#define ZCL_IDENTIFY_ATTR_NUM sizeof(identify_attrTbl) / sizeof(zclAttrInfo_t)

#endif // ZCL_BASIC_ATTR_NUM

#ifdef ZCL_POWER_CFG
/* power */
zcl_powerAttr_t g_zcl_powerAttrs[MAX_SCAN_DEVS] = {
		{
		    .batteryVoltage    = 0xff, //in 100 mV units, 0xff - unknown
		    .batteryPercentage = 0xff  //in 0,5% units, 0xff - unknown
		},
		{
		    .batteryVoltage    = 0xff, //in 100 mV units, 0xff - unknown
		    .batteryPercentage = 0xff  //in 0,5% units, 0xff - unknown
		},
		{
		    .batteryVoltage    = 0xff, //in 100 mV units, 0xff - unknown
		    .batteryPercentage = 0xff  //in 0,5% units, 0xff - unknown
		}
};

const zclAttrInfo_t powerCfg_attrTbl[] =
{
	{ ZCL_ATTRID_BATTERY_VOLTAGE,      		   		ZCL_DATA_TYPE_UINT8,	ACCESS_CONTROL_READ | ACCESS_CONTROL_REPORTABLE,	(u8*)&g_zcl_powerAttrs[0].batteryVoltage},
	{ ZCL_ATTRID_BATTERY_PERCENTAGE_REMAINING, 		ZCL_DATA_TYPE_UINT8,    ACCESS_CONTROL_READ | ACCESS_CONTROL_REPORTABLE, (u8*)&g_zcl_powerAttrs[0].batteryPercentage},

	{ ZCL_ATTRID_BATTERY_VOLTAGE+0x20, 		   		ZCL_DATA_TYPE_UINT8,    ACCESS_CONTROL_READ | ACCESS_CONTROL_REPORTABLE,	(u8*)&g_zcl_powerAttrs[1].batteryVoltage},
	{ ZCL_ATTRID_BATTERY_PERCENTAGE_REMAINING+0x20, ZCL_DATA_TYPE_UINT8,    ACCESS_CONTROL_READ | ACCESS_CONTROL_REPORTABLE, (u8*)&g_zcl_powerAttrs[1].batteryPercentage},

	{ ZCL_ATTRID_BATTERY_VOLTAGE+0x40, 		   		ZCL_DATA_TYPE_UINT8,    ACCESS_CONTROL_READ | ACCESS_CONTROL_REPORTABLE,	(u8*)&g_zcl_powerAttrs[2].batteryVoltage},
	{ ZCL_ATTRID_BATTERY_PERCENTAGE_REMAINING+0x40, ZCL_DATA_TYPE_UINT8,    ACCESS_CONTROL_READ | ACCESS_CONTROL_REPORTABLE, (u8*)&g_zcl_powerAttrs[2].batteryPercentage},
	{ ZCL_ATTRID_GLOBAL_CLUSTER_REVISION, 			ZCL_DATA_TYPE_UINT16,  	ACCESS_CONTROL_READ,  						(u8*)&zcl_attr_global_clusterRevision},
};

#define	ZCL_POWER_CFG_ATTR_NUM sizeof(powerCfg_attrTbl) / sizeof(zclAttrInfo_t)

#endif // ZCL_POWER_CFG

#ifdef ZCL_ILLUMINANCE_MEASUREMENT
zcl_illuminanceAttr_t	g_zcl_illuminanceAttrs = {
		.measuredVal = { 0xffff, 0xffff, 0xffff },
		.minMeasuredVal = 0,
		.maxMeasuredVal = 0xfffe,
#ifdef ZCL_ATTR_TOLERANCE_ENABLE
		.tolerance = 0,
#endif
#ifdef ZCL_ATTR_LIGHT_SENSOR_TYPE_ENABLE
		.lightSensorType = 0,
#endif
#ifdef ZCL_CUSTOM_ATTR_ILLUMINANCE_LEVEL
		.minLevelLx = {0,0,0}
#endif
};

const zclAttrInfo_t illuminanceMeasure_attrTbl1[] = {
    { ZCL_ATTRID_MEASURED_VALUE,          ZCL_DATA_TYPE_UINT16, ACCESS_CONTROL_READ | ACCESS_CONTROL_REPORTABLE, (u8*)&g_zcl_illuminanceAttrs.measuredVal[0] },
    { ZCL_ATTRID_MIN_MEASURED_VALUE,      ZCL_DATA_TYPE_UINT16, ACCESS_CONTROL_READ, (u8*)&g_zcl_illuminanceAttrs.minMeasuredVal },
    { ZCL_ATTRID_MAX_MEASURED_VALUE,      ZCL_DATA_TYPE_UINT16, ACCESS_CONTROL_READ, (u8*)&g_zcl_illuminanceAttrs.maxMeasuredVal },
#ifdef ZCL_ATTR_TOLERANCE_ENABLE
    { ZCL_ATTRID_TOLERANCE,               ZCL_DATA_TYPE_UINT16, ACCESS_CONTROL_READ, (u8*)&g_zcl_illuminanceAttrs.tolerance },
#endif
#ifdef ZCL_ATTR_LIGHT_SENSOR_TYPE_ENABLE
    { ZCL_ATTRID_LIGHT_SENSOR_TYPE,       ZCL_DATA_TYPE_ENUM8,  ACCESS_CONTROL_READ, (u8*)&g_zcl_illuminanceAttrs.lightSensorType },
#endif
#ifdef ZCL_CUSTOM_ATTR_ILLUMINANCE_LEVEL
	{ ZCL_CUSTOM_ATTR_ILLUMINANCE_LEVEL,  ZCL_DATA_TYPE_UINT16,  ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE, (u8*)&g_zcl_illuminanceAttrs.minLevelLx[0]},
#endif
    { ZCL_ATTRID_GLOBAL_CLUSTER_REVISION, ZCL_DATA_TYPE_UINT16, ACCESS_CONTROL_READ, (u8*)&zcl_attr_global_clusterRevision },
};

#define ZCL_ILLUMINANCE_ATTR_NUM1  (sizeof(illuminanceMeasure_attrTbl1) / sizeof(zclAttrInfo_t))

const zclAttrInfo_t illuminanceMeasure_attrTbl2[] = {
    { ZCL_ATTRID_MEASURED_VALUE,          ZCL_DATA_TYPE_UINT16, ACCESS_CONTROL_READ | ACCESS_CONTROL_REPORTABLE, (u8*)&g_zcl_illuminanceAttrs.measuredVal[1] },
    { ZCL_ATTRID_MIN_MEASURED_VALUE,      ZCL_DATA_TYPE_UINT16, ACCESS_CONTROL_READ, (u8*)&g_zcl_illuminanceAttrs.minMeasuredVal },
    { ZCL_ATTRID_MAX_MEASURED_VALUE,      ZCL_DATA_TYPE_UINT16, ACCESS_CONTROL_READ, (u8*)&g_zcl_illuminanceAttrs.maxMeasuredVal },
#ifdef ZCL_ATTR_TOLERANCE_ENABLE
    { ZCL_ATTRID_TOLERANCE,               ZCL_DATA_TYPE_UINT16, ACCESS_CONTROL_READ, (u8*)&g_zcl_illuminanceAttrs.tolerance },
#endif
#ifdef ZCL_ATTR_LIGHT_SENSOR_TYPE_ENABLE
    { ZCL_ATTRID_LIGHT_SENSOR_TYPE,       ZCL_DATA_TYPE_ENUM8,  ACCESS_CONTROL_READ, (u8*)&g_zcl_illuminanceAttrs.lightSensorType },
#endif
#ifdef ZCL_CUSTOM_ATTR_ILLUMINANCE_LEVEL
	{ ZCL_CUSTOM_ATTR_ILLUMINANCE_LEVEL,  ZCL_DATA_TYPE_UINT16,  ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE, (u8*)&g_zcl_illuminanceAttrs.minLevelLx[1]},
#endif
    { ZCL_ATTRID_GLOBAL_CLUSTER_REVISION, ZCL_DATA_TYPE_UINT16, ACCESS_CONTROL_READ, (u8*)&zcl_attr_global_clusterRevision },
};

#define ZCL_ILLUMINANCE_ATTR_NUM2  (sizeof(illuminanceMeasure_attrTbl2) / sizeof(zclAttrInfo_t))

const zclAttrInfo_t illuminanceMeasure_attrTbl3[] = {
    { ZCL_ATTRID_MEASURED_VALUE,          ZCL_DATA_TYPE_UINT16, ACCESS_CONTROL_READ | ACCESS_CONTROL_REPORTABLE, (u8*)&g_zcl_illuminanceAttrs.measuredVal[2] },
    { ZCL_ATTRID_MIN_MEASURED_VALUE,      ZCL_DATA_TYPE_UINT16, ACCESS_CONTROL_READ, (u8*)&g_zcl_illuminanceAttrs.minMeasuredVal },
    { ZCL_ATTRID_MAX_MEASURED_VALUE,      ZCL_DATA_TYPE_UINT16, ACCESS_CONTROL_READ, (u8*)&g_zcl_illuminanceAttrs.maxMeasuredVal },
#ifdef ZCL_ATTR_TOLERANCE_ENABLE
    { ZCL_ATTRID_TOLERANCE,               ZCL_DATA_TYPE_UINT16, ACCESS_CONTROL_READ, (u8*)&g_zcl_illuminanceAttrs.tolerance },
#endif
#ifdef ZCL_ATTR_LIGHT_SENSOR_TYPE_ENABLE
    { ZCL_ATTRID_LIGHT_SENSOR_TYPE,       ZCL_DATA_TYPE_ENUM8,  ACCESS_CONTROL_READ, (u8*)&g_zcl_illuminanceAttrs.lightSensorType },
#endif
#ifdef ZCL_CUSTOM_ATTR_ILLUMINANCE_LEVEL
	{ ZCL_CUSTOM_ATTR_ILLUMINANCE_LEVEL,  ZCL_DATA_TYPE_UINT16,  ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE, (u8*)&g_zcl_illuminanceAttrs.minLevelLx[2]},
#endif
    { ZCL_ATTRID_GLOBAL_CLUSTER_REVISION, ZCL_DATA_TYPE_UINT16, ACCESS_CONTROL_READ, (u8*)&zcl_attr_global_clusterRevision },
};

#define ZCL_ILLUMINANCE_ATTR_NUM3  (sizeof(illuminanceMeasure_attrTbl3) / sizeof(zclAttrInfo_t))

#endif // ZCL_ILLUMINANCE_MEASUREMENT

#ifdef ZCL_IAS_ZONE
/* IAS Zone */
zcl_iasZoneAttr_t g_zcl_iasZoneAttrs =
{
	.zoneState		= ZONE_STATE_NOT_ENROLLED,
	.zoneType		= ZONE_TYPE_CONTACT_SWITCH,
	.zoneStatus		= 0x00,
	.iasCieAddr		= {0x00},
	.zoneId 		= ZCL_ZONE_ID_INVALID,
};

const zclAttrInfo_t iasZone_attrTbl[] =
{
	{ ZCL_ATTRID_ZONE_STATE,   ZCL_DATA_TYPE_ENUM8,     ACCESS_CONTROL_READ,  						(u8*)&g_zcl_iasZoneAttrs.zoneState },
	{ ZCL_ATTRID_ZONE_TYPE,    ZCL_DATA_TYPE_ENUM16,    ACCESS_CONTROL_READ,  						(u8*)&g_zcl_iasZoneAttrs.zoneType },
	{ ZCL_ATTRID_ZONE_STATUS,  ZCL_DATA_TYPE_BITMAP16,  ACCESS_CONTROL_READ,  						(u8*)&g_zcl_iasZoneAttrs.zoneStatus },
	{ ZCL_ATTRID_IAS_CIE_ADDR, ZCL_DATA_TYPE_IEEE_ADDR, ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE, (u8*)g_zcl_iasZoneAttrs.iasCieAddr },
	{ ZCL_ATTRID_ZONE_ID,	   ZCL_DATA_TYPE_UINT8,     ACCESS_CONTROL_READ,  						(u8*)&g_zcl_iasZoneAttrs.zoneId},

	{ ZCL_ATTRID_GLOBAL_CLUSTER_REVISION, ZCL_DATA_TYPE_UINT16,  ACCESS_CONTROL_READ,  				(u8*)&zcl_attr_global_clusterRevision},
};

#define	ZCL_IASZONE_ATTR_NUM		 sizeof(iasZone_attrTbl) / sizeof(zclAttrInfo_t)
#endif


#ifdef ZCL_GROUP
/* Group */
zcl_groupAttr_t g_zcl_groupAttrs =
{
	.nameSupport	= 0,
};

const zclAttrInfo_t group_attrTbl[] =
{
	{ ZCL_ATTRID_GROUP_NAME_SUPPORT,  		ZCL_DATA_TYPE_BITMAP8,  ACCESS_CONTROL_READ,  (u8*)&g_zcl_groupAttrs.nameSupport },

	{ ZCL_ATTRID_GLOBAL_CLUSTER_REVISION, 	ZCL_DATA_TYPE_UINT16,  	ACCESS_CONTROL_READ,  (u8*)&zcl_attr_global_clusterRevision},
};

#define ZCL_GROUP_ATTR_NUM	  sizeof(group_attrTbl) / sizeof(zclAttrInfo_t)
#endif

#ifdef ZCL_TEMPERATURE_MEASUREMENT
zcl_temperatureAttr_t g_zcl_temperatureAttrs =
{
	.measuredValue	= { 0x8000, 0x8000, 0x8000 },
	.minValue 		= -5000,
	.maxValue		= 17500,
	.tolerance		= 0,
};

const zclAttrInfo_t temperature_measurement_attrTbl1[] =
{
	{ ZCL_TEMPERATURE_MEASUREMENT_ATTRID_MEASUREDVALUE,       	ZCL_DATA_TYPE_INT16,    ACCESS_CONTROL_READ | ACCESS_CONTROL_REPORTABLE, (u8*)&g_zcl_temperatureAttrs.measuredValue[0] },
	{ ZCL_TEMPERATURE_MEASUREMENT_ATTRID_MINMEASUREDVALUE,      ZCL_DATA_TYPE_INT16,    ACCESS_CONTROL_READ, (u8*)&g_zcl_temperatureAttrs.minValue },
	{ ZCL_TEMPERATURE_MEASUREMENT_ATTRID_MAXMEASUREDVALUE,      ZCL_DATA_TYPE_INT16,    ACCESS_CONTROL_READ, (u8*)&g_zcl_temperatureAttrs.maxValue },
	{ ZCL_TEMPERATURE_MEASUREMENT_ATTRID_TOLERANCE,       		ZCL_DATA_TYPE_UINT16,   ACCESS_CONTROL_READ | ACCESS_CONTROL_REPORTABLE, (u8*)&g_zcl_temperatureAttrs.tolerance },

	{ ZCL_ATTRID_GLOBAL_CLUSTER_REVISION, 	ZCL_DATA_TYPE_UINT16,  	ACCESS_CONTROL_READ,  						(u8*)&zcl_attr_global_clusterRevision},
};


const zclAttrInfo_t temperature_measurement_attrTbl2[] =
{
	{ ZCL_TEMPERATURE_MEASUREMENT_ATTRID_MEASUREDVALUE,       	ZCL_DATA_TYPE_INT16,    ACCESS_CONTROL_READ | ACCESS_CONTROL_REPORTABLE, (u8*)&g_zcl_temperatureAttrs.measuredValue[1] },
	{ ZCL_TEMPERATURE_MEASUREMENT_ATTRID_MINMEASUREDVALUE,      ZCL_DATA_TYPE_INT16,    ACCESS_CONTROL_READ, (u8*)&g_zcl_temperatureAttrs.minValue },
	{ ZCL_TEMPERATURE_MEASUREMENT_ATTRID_MAXMEASUREDVALUE,      ZCL_DATA_TYPE_INT16,    ACCESS_CONTROL_READ, (u8*)&g_zcl_temperatureAttrs.maxValue },
	{ ZCL_TEMPERATURE_MEASUREMENT_ATTRID_TOLERANCE,       		ZCL_DATA_TYPE_UINT16,   ACCESS_CONTROL_READ | ACCESS_CONTROL_REPORTABLE, (u8*)&g_zcl_temperatureAttrs.tolerance },

	{ ZCL_ATTRID_GLOBAL_CLUSTER_REVISION, 	ZCL_DATA_TYPE_UINT16,  	ACCESS_CONTROL_READ,  						(u8*)&zcl_attr_global_clusterRevision},
};


const zclAttrInfo_t temperature_measurement_attrTbl3[] =
{
	{ ZCL_TEMPERATURE_MEASUREMENT_ATTRID_MEASUREDVALUE,       	ZCL_DATA_TYPE_INT16,    ACCESS_CONTROL_READ | ACCESS_CONTROL_REPORTABLE, (u8*)&g_zcl_temperatureAttrs.measuredValue[2] },
	{ ZCL_TEMPERATURE_MEASUREMENT_ATTRID_MINMEASUREDVALUE,      ZCL_DATA_TYPE_INT16,    ACCESS_CONTROL_READ, (u8*)&g_zcl_temperatureAttrs.minValue },
	{ ZCL_TEMPERATURE_MEASUREMENT_ATTRID_MAXMEASUREDVALUE,      ZCL_DATA_TYPE_INT16,    ACCESS_CONTROL_READ, (u8*)&g_zcl_temperatureAttrs.maxValue },
	{ ZCL_TEMPERATURE_MEASUREMENT_ATTRID_TOLERANCE,       		ZCL_DATA_TYPE_UINT16,   ACCESS_CONTROL_READ | ACCESS_CONTROL_REPORTABLE, (u8*)&g_zcl_temperatureAttrs.tolerance },

	{ ZCL_ATTRID_GLOBAL_CLUSTER_REVISION, 	ZCL_DATA_TYPE_UINT16,  	ACCESS_CONTROL_READ,  						(u8*)&zcl_attr_global_clusterRevision},
};

#define	ZCL_TEMPERATURE_ATTR_NUM		 sizeof(temperature_measurement_attrTbl1) / sizeof(zclAttrInfo_t)

#endif // ZCL_TEMPERATURE_MEASUREMENT


#ifdef ZCL_RELATIVE_HUMIDITY
zcl_relHumidityAttr_t g_zcl_relHumidityAttrs =
{
	.measuredValue	= { 0xFFFF, 0xFFFF, 0xFFFF },
	.minValue 		= 0,
	.maxValue		= 9999,
	.tolerance		= 0,
};

const zclAttrInfo_t relative_humdity_attrTbl1[] =
{
	{ ZCL_RELATIVE_HUMIDITY_ATTRID_MEASUREDVALUE,       ZCL_DATA_TYPE_UINT16,    ACCESS_CONTROL_READ | ACCESS_CONTROL_REPORTABLE, (u8*)&g_zcl_relHumidityAttrs.measuredValue[0] },
	{ ZCL_RELATIVE_HUMIDITY_ATTRID_MINMEASUREDVALUE,    ZCL_DATA_TYPE_UINT16,    ACCESS_CONTROL_READ, (u8*)&g_zcl_relHumidityAttrs.minValue },
	{ ZCL_RELATIVE_HUMIDITY_ATTRID_MAXMEASUREDVALUE,    ZCL_DATA_TYPE_UINT16,    ACCESS_CONTROL_READ, (u8*)&g_zcl_relHumidityAttrs.maxValue },
	{ ZCL_RELATIVE_HUMIDITY_ATTRID_TOLERANCE,      		ZCL_DATA_TYPE_UINT16,    ACCESS_CONTROL_READ | ACCESS_CONTROL_REPORTABLE, (u8*)&g_zcl_relHumidityAttrs.tolerance },

	{ ZCL_ATTRID_GLOBAL_CLUSTER_REVISION, 	ZCL_DATA_TYPE_UINT16,  	ACCESS_CONTROL_READ,  						(u8*)&zcl_attr_global_clusterRevision},
};

const zclAttrInfo_t relative_humdity_attrTbl2[] =
{
	{ ZCL_RELATIVE_HUMIDITY_ATTRID_MEASUREDVALUE,       ZCL_DATA_TYPE_UINT16,    ACCESS_CONTROL_READ | ACCESS_CONTROL_REPORTABLE, (u8*)&g_zcl_relHumidityAttrs.measuredValue[1] },
	{ ZCL_RELATIVE_HUMIDITY_ATTRID_MINMEASUREDVALUE,    ZCL_DATA_TYPE_UINT16,    ACCESS_CONTROL_READ, (u8*)&g_zcl_relHumidityAttrs.minValue },
	{ ZCL_RELATIVE_HUMIDITY_ATTRID_MAXMEASUREDVALUE,    ZCL_DATA_TYPE_UINT16,    ACCESS_CONTROL_READ, (u8*)&g_zcl_relHumidityAttrs.maxValue },
	{ ZCL_RELATIVE_HUMIDITY_ATTRID_TOLERANCE,      		ZCL_DATA_TYPE_UINT16,    ACCESS_CONTROL_READ | ACCESS_CONTROL_REPORTABLE, (u8*)&g_zcl_relHumidityAttrs.tolerance },

	{ ZCL_ATTRID_GLOBAL_CLUSTER_REVISION, 	ZCL_DATA_TYPE_UINT16,  	ACCESS_CONTROL_READ,  						(u8*)&zcl_attr_global_clusterRevision},
};

const zclAttrInfo_t relative_humdity_attrTbl3[] =
{
	{ ZCL_RELATIVE_HUMIDITY_ATTRID_MEASUREDVALUE,       ZCL_DATA_TYPE_UINT16,    ACCESS_CONTROL_READ | ACCESS_CONTROL_REPORTABLE, (u8*)&g_zcl_relHumidityAttrs.measuredValue[2] },
	{ ZCL_RELATIVE_HUMIDITY_ATTRID_MINMEASUREDVALUE,    ZCL_DATA_TYPE_UINT16,    ACCESS_CONTROL_READ, (u8*)&g_zcl_relHumidityAttrs.minValue },
	{ ZCL_RELATIVE_HUMIDITY_ATTRID_MAXMEASUREDVALUE,    ZCL_DATA_TYPE_UINT16,    ACCESS_CONTROL_READ, (u8*)&g_zcl_relHumidityAttrs.maxValue },
	{ ZCL_RELATIVE_HUMIDITY_ATTRID_TOLERANCE,      		ZCL_DATA_TYPE_UINT16,    ACCESS_CONTROL_READ | ACCESS_CONTROL_REPORTABLE, (u8*)&g_zcl_relHumidityAttrs.tolerance },

	{ ZCL_ATTRID_GLOBAL_CLUSTER_REVISION, 	ZCL_DATA_TYPE_UINT16,  	ACCESS_CONTROL_READ,  						(u8*)&zcl_attr_global_clusterRevision},
};

#define	ZCL_RELATIVE_HUMIDITY_ATTR_NUM		 sizeof(relative_humdity_attrTbl1) / sizeof(zclAttrInfo_t)

#endif // ZCL_RELATIVE_HUMIDITY

#ifdef ZCL_POLL_CTRL
/* Poll Control */
zcl_pollCtrlAttr_t g_zcl_pollCtrlAttrs =
{
	.chkInInterval			= 3600*4, // 3600 sec, 1hr
	.longPollInterval		= 5*4,  //  5 sec
	.shortPollInterval		= 2,	// 2 qs
	.fastPollTimeout		= 10*4, // 10 sec
	.chkInIntervalMin		= 10*4, // 10 sec
	.longPollIntervalMin	= MIN_REPORT_INTERVAL*4,  // 4 sec
	.fastPollTimeoutMax		= 60*4	// 60 sec
};

const zclAttrInfo_t pollCtrl_attrTbl[] =
{
	{ ZCL_ATTRID_CHK_IN_INTERVAL,  		ZCL_DATA_TYPE_UINT32, ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE, (u8*)&g_zcl_pollCtrlAttrs.chkInInterval },
	{ ZCL_ATTRID_LONG_POLL_INTERVAL, 	ZCL_DATA_TYPE_UINT32, ACCESS_CONTROL_READ, 						  (u8*)&g_zcl_pollCtrlAttrs.longPollInterval },
	{ ZCL_ATTRID_SHORT_POLL_INTERVAL, 	ZCL_DATA_TYPE_UINT16, ACCESS_CONTROL_READ, 						  (u8*)&g_zcl_pollCtrlAttrs.shortPollInterval },
	{ ZCL_ATTRID_FAST_POLL_TIMEOUT, 	ZCL_DATA_TYPE_UINT16, ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE, (u8*)&g_zcl_pollCtrlAttrs.fastPollTimeout },
	{ ZCL_ATTRID_CHK_IN_INTERVAL_MIN, 	ZCL_DATA_TYPE_UINT32, ACCESS_CONTROL_READ, 						  (u8*)&g_zcl_pollCtrlAttrs.chkInIntervalMin},
	{ ZCL_ATTRID_LONG_POLL_INTERVAL_MIN,ZCL_DATA_TYPE_UINT32, ACCESS_CONTROL_READ, 						  (u8*)&g_zcl_pollCtrlAttrs.longPollIntervalMin },
	{ ZCL_ATTRID_FAST_POLL_TIMEOUT_MAX, ZCL_DATA_TYPE_UINT16, ACCESS_CONTROL_READ, 						  (u8*)&g_zcl_pollCtrlAttrs.fastPollTimeoutMax},

	{ ZCL_ATTRID_GLOBAL_CLUSTER_REVISION, ZCL_DATA_TYPE_UINT16,  ACCESS_CONTROL_READ,  					  (u8*)&zcl_attr_global_clusterRevision},
};

#define	ZCL_POLLCTRL_ATTR_NUM		 sizeof(pollCtrl_attrTbl) / sizeof(zclAttrInfo_t)
#endif


#ifdef ZCL_ON_OFF
/* On/Off */
zcl_onOffAttr_t g_zcl_onOffAttrs[MAX_SCAN_DEVS] =
{
	{
			.onOff				= 0,
			.globalSceneControl	= 1,
			.onTime				= 0x0000,
			.offWaitTime		= 0x0000,
			.startUpOnOff 		= ZCL_START_UP_ONOFF_SET_ONOFF_TO_OFF,
#ifdef	ZCL_CUSTOM_ATTR_ONOFF_BLE_TYPE
			.onoffbType			= BtHomeID_switch
#endif
	},
	{
			.onOff				= 0,
			.globalSceneControl	= 1,
			.onTime				= 0x0000,
			.offWaitTime		= 0x0000,
			.startUpOnOff 		= ZCL_START_UP_ONOFF_SET_ONOFF_TO_OFF,
#ifdef	ZCL_CUSTOM_ATTR_ONOFF_BLE_TYPE
			.onoffbType			= BtHomeID_switch
#endif
	},
	{
			.onOff				= 0,
			.globalSceneControl	= 1,
			.onTime				= 0x0000,
			.offWaitTime		= 0x0000,
			.startUpOnOff 		= ZCL_START_UP_ONOFF_SET_ONOFF_TO_OFF,
#ifdef	ZCL_CUSTOM_ATTR_ONOFF_BLE_TYPE
			.onoffbType			= BtHomeID_switch
#endif
	}
};

const zclAttrInfo_t onOff_attrTbl1[] =
{
	{ ZCL_ATTRID_ONOFF,  					ZCL_DATA_TYPE_BOOLEAN,  ACCESS_CONTROL_READ | ACCESS_CONTROL_REPORTABLE,  (u8*)&g_zcl_onOffAttrs[0].onOff},
	{ ZCL_ATTRID_GLOBAL_SCENE_CONTROL, 		ZCL_DATA_TYPE_BOOLEAN, 	ACCESS_CONTROL_READ, 							  (u8*)&g_zcl_onOffAttrs[0].globalSceneControl},
	{ ZCL_ATTRID_ON_TIME, 					ZCL_DATA_TYPE_UINT16, 	ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE, 	  (u8*)&g_zcl_onOffAttrs[0].onTime},
	{ ZCL_ATTRID_OFF_WAIT_TIME, 			ZCL_DATA_TYPE_UINT16, 	ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE, 	  (u8*)&g_zcl_onOffAttrs[0].offWaitTime},
	{ ZCL_ATTRID_START_UP_ONOFF, 			ZCL_DATA_TYPE_ENUM8, 	ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE, 	  (u8*)&g_zcl_onOffAttrs[0].startUpOnOff},
#ifdef	ZCL_CUSTOM_ATTR_ONOFF_BLE_TYPE
	{ ZCL_CUSTOM_ATTR_ONOFF_BLE_TYPE, 		ZCL_DATA_TYPE_UINT8, 	ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE, 	  (u8*)&g_zcl_onOffAttrs[0].onoffbType},
#endif
	{ ZCL_ATTRID_GLOBAL_CLUSTER_REVISION, 	ZCL_DATA_TYPE_UINT16,  	ACCESS_CONTROL_READ,  							  (u8*)&zcl_attr_global_clusterRevision},
};

#define ZCL_ONOFF_ATTR_NUM1	 sizeof(onOff_attrTbl1) / sizeof(zclAttrInfo_t)

const zclAttrInfo_t onOff_attrTbl2[] =
{
	{ ZCL_ATTRID_ONOFF,  					ZCL_DATA_TYPE_BOOLEAN,  ACCESS_CONTROL_READ | ACCESS_CONTROL_REPORTABLE,  (u8*)&g_zcl_onOffAttrs[1].onOff},
	{ ZCL_ATTRID_GLOBAL_SCENE_CONTROL, 		ZCL_DATA_TYPE_BOOLEAN, 	ACCESS_CONTROL_READ, 							  (u8*)&g_zcl_onOffAttrs[1].globalSceneControl},
	{ ZCL_ATTRID_ON_TIME, 					ZCL_DATA_TYPE_UINT16, 	ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE, 	  (u8*)&g_zcl_onOffAttrs[1].onTime},
	{ ZCL_ATTRID_OFF_WAIT_TIME, 			ZCL_DATA_TYPE_UINT16, 	ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE, 	  (u8*)&g_zcl_onOffAttrs[1].offWaitTime},
	{ ZCL_ATTRID_START_UP_ONOFF, 			ZCL_DATA_TYPE_ENUM8, 	ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE, 	  (u8*)&g_zcl_onOffAttrs[1].startUpOnOff},
#ifdef	ZCL_CUSTOM_ATTR_ONOFF_BLE_TYPE
	{ ZCL_CUSTOM_ATTR_ONOFF_BLE_TYPE, 		ZCL_DATA_TYPE_UINT8, 	ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE, 	  (u8*)&g_zcl_onOffAttrs[1].onoffbType},
#endif
	{ ZCL_ATTRID_GLOBAL_CLUSTER_REVISION, 	ZCL_DATA_TYPE_UINT16,  	ACCESS_CONTROL_READ,  							  (u8*)&zcl_attr_global_clusterRevision},
};

#define ZCL_ONOFF_ATTR_NUM2	 sizeof(onOff_attrTbl2) / sizeof(zclAttrInfo_t)

const zclAttrInfo_t onOff_attrTbl3[] =
{
	{ ZCL_ATTRID_ONOFF,  					ZCL_DATA_TYPE_BOOLEAN,  ACCESS_CONTROL_READ | ACCESS_CONTROL_REPORTABLE,  (u8*)&g_zcl_onOffAttrs[2].onOff},
	{ ZCL_ATTRID_GLOBAL_SCENE_CONTROL, 		ZCL_DATA_TYPE_BOOLEAN, 	ACCESS_CONTROL_READ, 							  (u8*)&g_zcl_onOffAttrs[2].globalSceneControl},
	{ ZCL_ATTRID_ON_TIME, 					ZCL_DATA_TYPE_UINT16, 	ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE, 	  (u8*)&g_zcl_onOffAttrs[2].onTime},
	{ ZCL_ATTRID_OFF_WAIT_TIME, 			ZCL_DATA_TYPE_UINT16, 	ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE, 	  (u8*)&g_zcl_onOffAttrs[2].offWaitTime},
	{ ZCL_ATTRID_START_UP_ONOFF, 			ZCL_DATA_TYPE_ENUM8, 	ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE, 	  (u8*)&g_zcl_onOffAttrs[2].startUpOnOff},
#ifdef	ZCL_CUSTOM_ATTR_ONOFF_BLE_TYPE
	{ ZCL_CUSTOM_ATTR_ONOFF_BLE_TYPE, 		ZCL_DATA_TYPE_UINT8, 	ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE, 	  (u8*)&g_zcl_onOffAttrs[2].onoffbType},
#endif
	{ ZCL_ATTRID_GLOBAL_CLUSTER_REVISION, 	ZCL_DATA_TYPE_UINT16,  	ACCESS_CONTROL_READ,  							  (u8*)&zcl_attr_global_clusterRevision},
};

#define ZCL_ONOFF_ATTR_NUM3	 sizeof(onOff_attrTbl3) / sizeof(zclAttrInfo_t)

#endif

//  set ZCL_CLUSTER_NUM_MAX (stack_cfg.h) !
/**
 *  @brief Definition for simple sensor ZCL specific cluster
 */
const zcl_specClusterInfo_t g_appClusterList1[] =
{
	{ZCL_CLUSTER_GEN_BASIC, MANUFACTURER_CODE_NONE, ZCL_BASIC_ATTR_NUM, basic_attrTbl, zcl_basic_register, app_basicCb},
#ifdef ZCL_POWER_CFG
	{ZCL_CLUSTER_GEN_POWER_CFG,	MANUFACTURER_CODE_NONE,	ZCL_POWER_CFG_ATTR_NUM,	powerCfg_attrTbl, zcl_powerCfg_register, app_powerCfgCb},
#endif
#ifdef ZCL_IDENTIFY
	{ZCL_CLUSTER_GEN_IDENTIFY, MANUFACTURER_CODE_NONE, ZCL_IDENTIFY_ATTR_NUM, identify_attrTbl, zcl_identify_register, app_identifyCb},
#endif
#ifdef ZCL_GROUP
	{ZCL_CLUSTER_GEN_GROUPS, MANUFACTURER_CODE_NONE, 0, NULL, zcl_group_register, app_groupCb},
#endif
#ifdef ZCL_ON_OFF
	{ZCL_CLUSTER_GEN_ON_OFF, MANUFACTURER_CODE_NONE, ZCL_ONOFF_ATTR_NUM1, onOff_attrTbl1, zcl_onOff_register, app_onOffCb},
#endif
#ifdef ZCL_POLL_CTRL
	{ZCL_CLUSTER_GEN_POLL_CONTROL, MANUFACTURER_CODE_NONE, ZCL_POLLCTRL_ATTR_NUM, pollCtrl_attrTbl, zcl_pollCtrl_register, app_pollCtrlCb},
#endif
#ifdef ZCL_ILLUMINANCE_MEASUREMENT
	{ZCL_CLUSTER_MS_ILLUMINANCE_MEASUREMENT, MANUFACTURER_CODE_NONE, ZCL_ILLUMINANCE_ATTR_NUM1, illuminanceMeasure_attrTbl1, zcl_illuminanceMeasure_register, NULL},
#endif
#ifdef ZCL_TEMPERATURE_MEASUREMENT
	{ZCL_CLUSTER_MS_TEMPERATURE_MEASUREMENT, MANUFACTURER_CODE_NONE, ZCL_TEMPERATURE_ATTR_NUM, temperature_measurement_attrTbl1, zcl_temperature_measurement_register, 	NULL},
#endif
#ifdef ZCL_RELATIVE_HUMIDITY
	{ZCL_CLUSTER_MS_RELATIVE_HUMIDITY, MANUFACTURER_CODE_NONE, ZCL_RELATIVE_HUMIDITY_ATTR_NUM, relative_humdity_attrTbl1, zcl_relative_humidity_register, NULL},
#endif
#ifdef ZCL_IAS_ZONE
	{ZCL_CLUSTER_SS_IAS_ZONE, MANUFACTURER_CODE_NONE, ZCL_IASZONE_ATTR_NUM,	iasZone_attrTbl, zcl_iasZone_register, app_iasZoneCb},
#endif
};

const u8 APP_CB_CLUSTER_NUM1 = (sizeof(g_appClusterList1)/sizeof(g_appClusterList1[0]));

const zcl_specClusterInfo_t g_appClusterList2[] =
{
#ifdef ZCL_ON_OFF
	{ZCL_CLUSTER_GEN_ON_OFF, MANUFACTURER_CODE_NONE, ZCL_ONOFF_ATTR_NUM2, onOff_attrTbl2, zcl_onOff_register, app_onOffCb},
#endif
#ifdef ZCL_ILLUMINANCE_MEASUREMENT
	{ZCL_CLUSTER_MS_ILLUMINANCE_MEASUREMENT, MANUFACTURER_CODE_NONE, ZCL_ILLUMINANCE_ATTR_NUM2, illuminanceMeasure_attrTbl2, zcl_illuminanceMeasure_register, NULL},
#endif
#ifdef ZCL_TEMPERATURE_MEASUREMENT
	{ZCL_CLUSTER_MS_TEMPERATURE_MEASUREMENT, MANUFACTURER_CODE_NONE, ZCL_TEMPERATURE_ATTR_NUM, temperature_measurement_attrTbl2, zcl_temperature_measurement_register, NULL},
#endif
#ifdef ZCL_RELATIVE_HUMIDITY
	{ZCL_CLUSTER_MS_RELATIVE_HUMIDITY, MANUFACTURER_CODE_NONE, ZCL_RELATIVE_HUMIDITY_ATTR_NUM, relative_humdity_attrTbl2, zcl_relative_humidity_register, NULL},
#endif
};

const u8 APP_CB_CLUSTER_NUM2 = (sizeof(g_appClusterList2)/sizeof(g_appClusterList2[0]));


const zcl_specClusterInfo_t g_appClusterList3[] =
{
#ifdef ZCL_ON_OFF
	{ZCL_CLUSTER_GEN_ON_OFF, MANUFACTURER_CODE_NONE, ZCL_ONOFF_ATTR_NUM3, onOff_attrTbl3, zcl_onOff_register, app_onOffCb},
#endif
#ifdef ZCL_ILLUMINANCE_MEASUREMENT
	{ZCL_CLUSTER_MS_ILLUMINANCE_MEASUREMENT, MANUFACTURER_CODE_NONE, ZCL_ILLUMINANCE_ATTR_NUM3, illuminanceMeasure_attrTbl3, zcl_illuminanceMeasure_register, NULL},
#endif
#ifdef ZCL_TEMPERATURE_MEASUREMENT
	{ZCL_CLUSTER_MS_TEMPERATURE_MEASUREMENT, MANUFACTURER_CODE_NONE, ZCL_TEMPERATURE_ATTR_NUM, temperature_measurement_attrTbl3, zcl_temperature_measurement_register, NULL},
#endif
#ifdef ZCL_RELATIVE_HUMIDITY
	{ZCL_CLUSTER_MS_RELATIVE_HUMIDITY, MANUFACTURER_CODE_NONE, ZCL_RELATIVE_HUMIDITY_ATTR_NUM, relative_humdity_attrTbl3,	zcl_relative_humidity_register, NULL},
#endif
};

const u8 APP_CB_CLUSTER_NUM3 = (sizeof(g_appClusterList3)/sizeof(g_appClusterList3[0]));

/**********************************************************************
 * FUNCTIONS
 */

#ifdef ZCL_CUSTOM_ATTR_ILLUMINANCE_LEVEL
/*********************************************************************
 * @fn      zcl_illuminance_save
 *
 * @brief
 *
 * @param   None
 *
 * @return
 */
nv_sts_t zcl_illuminance_save(void)
{
    nv_sts_t st = NV_SUCC;
#if NV_ENABLE
    bool needSave = FALSE;
    zcl_nv_illuminance_t zcl_nv_illuminance;

    st = nv_flashReadNew(1, NV_MODULE_APP,  NV_ITEM_APP_ILLUMINANCE,
    		sizeof(zcl_nv_illuminance.minLevelLx),
			(u8 *)&zcl_nv_illuminance.minLevelLx);
    if (st == NV_SUCC) {
    	needSave = memcmp(&zcl_nv_illuminance.minLevelLx,
    				&g_zcl_illuminanceAttrs.minLevelLx,
					sizeof(g_zcl_illuminanceAttrs.minLevelLx));
    } else if (st == NV_ITEM_NOT_FOUND) {
        needSave = TRUE;
    }
    if (needSave) {
        st = nv_flashWriteNew(1, NV_MODULE_APP, NV_ITEM_APP_ILLUMINANCE,
        		sizeof(g_zcl_illuminanceAttrs.minLevelLx),
				(u8 *)&g_zcl_illuminanceAttrs.minLevelLx);
    }
#else
    st = NV_ENABLE_PROTECT_ERROR;
#endif

    return st;
}

/*********************************************************************
 * @fn      zcl_illuminance_restore
 *
 * @brief
 *
 * @param   None
 *
 * @return
 */
nv_sts_t zcl_illuminance_restore(void)
{
    nv_sts_t st = NV_SUCC;

#if NV_ENABLE
    st = nv_flashReadNew(1, NV_MODULE_APP,  NV_ITEM_APP_ILLUMINANCE,
    		sizeof(g_zcl_illuminanceAttrs.minLevelLx),
			(u8 *)&g_zcl_illuminanceAttrs.minLevelLx);
#else
    st = NV_ENABLE_PROTECT_ERROR;
#endif
    return st;
}
#endif

#ifdef ZCL_ON_OFF
/*********************************************************************
 * @fn      zcl_onOffAttr_save
 *
 * @brief
 *
 * @param   None
 *
 * @return
 */
nv_sts_t zcl_onOffAttr_save(u8 n)
{
    nv_sts_t st = NV_SUCC;
    zcl_nv_onOff_t zcl_nv_onOff;
    zcl_onOffAttr_t *pOnOff = zcl_onoffAttrGet(n);
#if USE_EEP
    zcl_nv_onOff.onOff = pOnOff->onOff;
    zcl_nv_onOff.startUpOnOff = pOnOff->startUpOnOff;
   	flash_write_cfg((u8 *)&zcl_nv_onOff, 1, EEP_ID_ONOFF(n), sizeof(zcl_nv_onOff_t));
#else
#if NV_ENABLE
    bool needSave = FALSE;

    st = nv_flashReadNew(1, NV_MODULE_APP,
    		NV_ITEM_APP_ON_OFF + n,
			sizeof(zcl_nv_onOff_t),
			(u8 *)&zcl_nv_onOff);
    if (st == NV_SUCC) {
        if (zcl_nv_onOff.onOff != pOnOff->onOff
            || zcl_nv_onOff.startUpOnOff != pOnOff->startUpOnOff
        	) {
            needSave = TRUE;
        }
    } else if (st == NV_ITEM_NOT_FOUND) {
        needSave = TRUE;
    }

    if (needSave) {
        zcl_nv_onOff.onOff = pOnOff->onOff;
        zcl_nv_onOff.startUpOnOff = pOnOff->startUpOnOff;
        st = nv_flashWriteNew(1, NV_MODULE_APP,
        		NV_ITEM_APP_ON_OFF + n,
				sizeof(zcl_nv_onOff_t),
				(u8 *)&zcl_nv_onOff);
    }
#else
    st = NV_ENABLE_PROTECT_ERROR;
#endif
#endif
    return st;
}
/*********************************************************************
 * @fn      zcl_onOffAttr_restore
 *
 * @brief
 *
 * @param   None
 *
 * @return
 */
nv_sts_t zcl_onOffAttr_restore(u8 n)
{
    nv_sts_t st = NV_SUCC;
    zcl_nv_onOff_t zcl_nv_onOff;
    zcl_onOffAttr_t *pOnOff = zcl_onoffAttrGet(n);
#if USE_EEP
   	flash_read_cfg((u8 *)&zcl_nv_onOff, 1, EEP_ID_ONOFF(n), sizeof(zcl_nv_onOff_t));
    pOnOff->onOff = zcl_nv_onOff.onOff;
    pOnOff->startUpOnOff = zcl_nv_onOff.startUpOnOff;
#else
#if NV_ENABLE

    st = nv_flashReadNew(1, NV_MODULE_APP,
    		NV_ITEM_APP_ON_OFF + n,
			sizeof(zcl_nv_onOff_t),
			(u8 *)&zcl_nv_onOff);
    if (st == NV_SUCC) {
        pOnOff->onOff = zcl_nv_onOff.onOff;
        pOnOff->startUpOnOff = zcl_nv_onOff.startUpOnOff;
    }
#else
    st = NV_ENABLE_PROTECT_ERROR;
#endif // NV_ENABLE
#endif // USE_EEP
    return st;
}

#ifdef ZCL_CUSTOM_ATTR_ONOFF_BLE_TYPE

nv_sts_t zcl_onOffTypeAttr_restore(void)
{
    nv_sts_t st = NV_SUCC;
    u8 onoffbType[3];
#if USE_EEP
   	if(flash_read_cfg(onoffbType,
   			0,
			EEP_ID_ONOFF_TYPE,
			sizeof(onoffbType)) != sizeof(onoffbType))
#else
    st = nv_flashReadNew(1, NV_MODULE_APP,
    		NV_ITEM_APP_ON_OFF + n,
			sizeof(zcl_nv_onOff_t),
			(u8 *)&zcl_nv_onOff);
    if (st != NV_SUCC)
#endif
    {
    	g_zcl_onOffAttrs[0].onoffbType = BtHomeID_switch;
    	g_zcl_onOffAttrs[1].onoffbType = BtHomeID_switch;
    	g_zcl_onOffAttrs[2].onoffbType = BtHomeID_switch;
    } else {
    	g_zcl_onOffAttrs[0].onoffbType = onoffbType[0];
    	g_zcl_onOffAttrs[1].onoffbType = onoffbType[1];
    	g_zcl_onOffAttrs[2].onoffbType = onoffbType[2];
    }
    return st;
}

nv_sts_t zcl_onOffTypeAttr_save(void)
{
    nv_sts_t st = NV_SUCC;
    u8 onoffbType[3];
#if USE_EEP
	onoffbType[0] = g_zcl_onOffAttrs[0].onoffbType;
	onoffbType[1] = g_zcl_onOffAttrs[1].onoffbType;
	onoffbType[2] = g_zcl_onOffAttrs[2].onoffbType;
   	flash_write_cfg(onoffbType, 0, EEP_ID_ONOFF_TYPE, sizeof(onoffbType));
#else
	needSave = TRUE;
    st = nv_flashReadNew(1, NV_MODULE_APP,
    		NV_ITEM_APP_BLE_ONOFF,
			sizeof(onoffbType),
			(u8 *)&onoffbType);
    if (st == NV_SUCC
    	&& g_zcl_onOffAttrs[0].onoffbType == onoffbType[0]
		&& g_zcl_onOffAttrs[1].onoffbType == onoffbType[1]
		&& g_zcl_onOffAttrs[2].onoffbType == onoffbType[2]) {
    	needSave = FALSE;
    }
    if(needSave) {
    	onoffbType[0] = g_zcl_onOffAttrs[0].onoffbType;
    	onoffbType[1] = g_zcl_onOffAttrs[1].onoffbType;
    	onoffbType[2] = g_zcl_onOffAttrs[2].onoffbType;
    	st = nv_flashWriteNew(1, NV_MODULE_APP,
    		NV_ITEM_APP_BLE_ONOFF,
			sizeof(onoffbType),
			(u8 *)&onoffbType);
    }
#endif
    return st;
}

#endif


#endif // ZCL_ON_OFF
/*********************************************************************
 * @fn      populate_date_code
 *
 * @brief
 *
 * @param   None
 *
 * @return
 */
__attribute__((optimize("-Os")))
void populate_date_code(void) {
	u8 month;
	if (__DATE__[0] == 'J' && __DATE__[1] == 'a' && __DATE__[2] == 'n') month = 1;
	else if (__DATE__[0] == 'F') month = 2;
	else if (__DATE__[0] == 'M' && __DATE__[1] == 'a' && __DATE__[2] == 'r') month = 3;
	else if (__DATE__[0] == 'A' && __DATE__[1] == 'p') month = 4;
	else if (__DATE__[0] == 'M' && __DATE__[1] == 'a' && __DATE__[2] == 'y') month = 5;
	else if (__DATE__[0] == 'J' && __DATE__[1] == 'u' && __DATE__[2] == 'n') month = 6;
	else if (__DATE__[0] == 'J' && __DATE__[1] == 'u' && __DATE__[2] == 'l') month = 7;
	else if (__DATE__[0] == 'A' && __DATE__[1] == 'u') month = 8;
	else if (__DATE__[0] == 'S') month = 9;
	else if (__DATE__[0] == 'O') month = 10;
	else if (__DATE__[0] == 'N') month = 11;
	else if (__DATE__[0] == 'D') month = 12;

	g_zcl_basicAttrs.dateCode[1] = __DATE__[7];
	g_zcl_basicAttrs.dateCode[2] = __DATE__[8];
	g_zcl_basicAttrs.dateCode[3] = __DATE__[9];
	g_zcl_basicAttrs.dateCode[4] = __DATE__[10];
	g_zcl_basicAttrs.dateCode[5] = '0' + month / 10;
	g_zcl_basicAttrs.dateCode[6] = '0' + month % 10;
	g_zcl_basicAttrs.dateCode[7] = __DATE__[4] >= '0' ? (__DATE__[4]) : '0';
	g_zcl_basicAttrs.dateCode[8] = __DATE__[5];
}

