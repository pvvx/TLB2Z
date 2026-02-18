/********************************************************************************************************
 * @file    app_cfg.h
 *
 * @brief   This is the header file for app_cfg
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
#ifndef _APP_CFG_H_
#define _APP_CFG_H_

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C" {
#endif


/**********************************************************************
 * Version configuration
 */
#include "version_cfg.h"

/**********************************************************************
 * Product Information
 */
/* Debug mode config */
#define	DEBUG_ENABLE					0 // lcd = DeviceSysException

#define	UART_PRINTF_MODE				0
#define USB_PRINTF_MODE         		0

/* HCI interface */
#define ZBHCI_BLE						0

/* PM */
#define PM_ENABLE						0
//#define DBG_ZIGBEE_STATUS_EN			1

/* BLE SCAN */
#define USE_SCAN			1

#define CLOCK_SYS_CLOCK_HZ  		48000000 //48000000

/**********************************************************************
 * I2C driver type
 */
#define I2C_DRV_NONE	0
#define I2C_DRV_HARD	1
#define I2C_DRV_SOFT	2


/* Board include */
#if defined(BOARD)
#if BOARD == BOARD_TB03F_KIT
#include "board_tb03f_kit.h"
#elif BOARD == BOARD_TS0001_TZ3000_RBZ
#include "board_ts0001_tz3000_gjrubzje.h"
#endif
#else
#error "Define BOARD!"
#endif

#ifndef ZIGBEE_TUYA_OTA
#define ZIGBEE_TUYA_OTA 	0
#endif

/* PA */
#ifndef PA_ENABLE
#define PA_ENABLE			0
#endif

#define VOLTAGE_DETECT_ADC_PIN GPIO_VBAT

#define READ_SENSOR_TIMER_SEC 	4 // second
#define READ_SENSOR_TIMER_MS 	(READ_SENSOR_TIMER_SEC*1000) // msecond

/* Voltage detect module */
/* If VOLTAGE_DETECT_ENABLE is set,
 * 1) if MCU_CORE_826x is defined, the DRV_ADC_VBAT_MODE mode is used by default,
 * and there is no need to configure the detection IO port;
 * 2) if MCU_CORE_8258 or MCU_CORE_8278 is defined, the DRV_ADC_VBAT_MODE mode is used by default,
 * we need to configure the detection IO port, and the IO must be in a floating state.
 * 3) if MCU_CORE_B91 is defined, the DRV_ADC_BASE_MODE mode is used by default,
 * we need to configure the detection IO port, and the IO must be connected to the target under test,
 * such as VCC.
 */
#define VOLTAGE_DETECT_ENABLE						0

/* Watch dog module */
#define MODULE_WATCHDOG_ENABLE						0

/* UART module */
#define	MODULE_UART_ENABLE							0

#if (ZBHCI_USB_PRINT || ZBHCI_USB_CDC || ZBHCI_USB_HID || ZBHCI_UART)
	#define ZBHCI_EN								1
#endif

#if PA_ENABLE
#define ZB_DEFAULT_TX_POWER_IDX					RF_POWER_P0p04dBm
#define	BLE_DEFAULT_TX_POWER_IDX				RF_POWER_P0p04dBm
#else
#define ZB_DEFAULT_TX_POWER_IDX					RF_POWER_P10p46dBm
#define	BLE_DEFAULT_TX_POWER_IDX				RF_POWER_P3p01dBm
#endif
/**********************************************************************
 * ZCL cluster support setting
 */
/* BDB */
#define TOUCHLINK_SUPPORT							0
#define FIND_AND_BIND_SUPPORT						0
#define ZCL_POWER_CFG_SUPPORT						1
#ifdef CPIO_RELAY
#define ZCL_ON_OFF_SUPPORT							1
#define SCAN_TRG_ENABLE								1
#else
#define ZCL_ON_OFF_SUPPORT							0
#define SCAN_TRG_ENABLE								0
#endif
//#define ZCL_IAS_ZONE_SUPPORT						1
#define ZCL_TEMPERATURE_MEASUREMENT_SUPPORT			1
#define ZCL_RELATIVE_HUMIDITY_SUPPORT   			1
#define ZCL_THERMOSTAT_UI_CFG_SUPPORT				0
#define ZCL_POLL_CTRL_SUPPORT						1
#define ZCL_GROUP_SUPPORT							0
#define ZCL_OTA_SUPPORT								1 // set FLASH_OTA_IMAGE_MAX_SIZE - 0x2000 in drv_nv.h !
#define REJOIN_FAILURE_TIMER						1
#if TOUCHLINK_SUPPORT
#define ZCL_ZLL_COMMISSIONING_SUPPORT				1
#endif

/* BLE */
#define MAX_SCAN_DEVS						3
#define APP_SECURITY_ENABLE      			0
#define APP_DIRECT_ADV_ENABLE				1
#define BLE_APP_PM_ENABLE					PM_ENABLE
#define USE_DEVICE_INFO_CHR_UUID			1
#define USE_FLASH_SERIAL_UID				1
#define USE_BLE_OTA							0 //ZCL_OTA_SUPPORT
#define SCAN_IN_ADV_STATE					0

#define USE_BINDKEY		1

#define USE_DEBUG_UART		0

// for consistency
#if ZCL_RELATIVE_HUMIDITY_SUPPORT
#define ZCL_RELATIVE_HUMIDITY
#define ZCL_RELATIVE_HUMIDITY_MEASUREMENT
#endif

#define DEFAULT_POLL_RATE					(4 * (4 * POLL_RATE_QUARTERSECONDS)) // 4000 ms

/**********************************************************************
 * Stack configuration
 */
#include "includes/zb_config.h"
#include "stack_cfg.h"


/**********************************************************************
 * EV configuration
 */
/**********************************************************************
 * EV configuration
 */
typedef enum{
	EV_POLL_ED_DETECT,
	EV_POLL_PM,
	EV_POLL_HCI,
    EV_POLL_IDLE,
	EV_POLL_MAX,
}ev_poll_e;


/**********************************************************************
 * Sensor configuration
 */

/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif

#endif // _APP_CFG_H_
