/********************************************************************************************************
 * @file    app_cfg.h
 *
 * @brief   This is the header file for app_cfg
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

#pragma once

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
/* PA */
#define PA_ENABLE									0

/* Debug mode */
#define TLKAPI_DEBUG_ENABLE							0


/* HCI interface */
#define	ZBHCI_UART									0
#define ZBHCI_USB_CDC								0
#define ZBHCI_USB_HID								0
#define ZBHCI_BLE									1

/* Board ID */
#define BOARD_8258_EVK								1
#define BOARD_8258_EVK_V1P2							2//C1T139A30_V1.2
#define BOARD_8258_DONGLE							3
#define BOARD_B91_EVK								4
#define BOARD_B91_DONGLE							5
#define BOARD_TL321X_EVK							6
#define BOARD_TL321X_DONGLE							7
#define BOARD_TL721X_EVK							8
#define BOARD_TL721X_DONGLE							9

/* Board define */
#if defined(MCU_CORE_8258)
#if (CHIP_TYPE == TLSR_8258_1M)
	#define FLASH_CAP_SIZE_1M						1
#endif
	#define BOARD									BOARD_8258_DONGLE
	#define CLOCK_SYS_CLOCK_HZ  					48000000
#elif defined(MCU_CORE_B91)
	#define FLASH_CAP_SIZE_1M						1
	#define BOARD									BOARD_B91_DONGLE//BOARD_B91_EVK
	#define CLOCK_SYS_CLOCK_HZ  					48000000
#elif defined(MCU_CORE_TL321X)
	#define FLASH_CAP_SIZE_1M						1
	#define BOARD									BOARD_TL321X_DONGLE//BOARD_TL321X_EVK
	#define CLOCK_SYS_CLOCK_HZ  					96000000
#elif defined(MCU_CORE_TL721X)
	#define FLASH_CAP_SIZE_1M						1
	#define BOARD									BOARD_TL721X_DONGLE//BOARD_TL721X_EVK
	#define CLOCK_SYS_CLOCK_HZ  					120000000
#else
	#error "MCU is undefined!"
#endif

/* Board include */
#if (BOARD == BOARD_8258_DONGLE)
	#include "board_8258_dongle.h"
#elif (BOARD == BOARD_8258_EVK_V1P2)
	#include "board_8258_evk_v1p2.h"
#elif (BOARD == BOARD_B91_EVK)
	#include "board_b91_evk.h"
#elif (BOARD == BOARD_B91_DONGLE)
	#include "board_b91_dongle.h"
#elif (BOARD == BOARD_TL321X_EVK)
	#include "board_tl321x_evk.h"
#elif (BOARD == BOARD_TL321X_DONGLE)
	#include "board_tl321x_dongle.h"
#elif (BOARD == BOARD_TL721X_EVK)
	#include "board_tl721x_evk.h"
#elif (BOARD == BOARD_TL721X_DONGLE)
	#include "board_tl721x_dongle.h"
#endif


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
#define VOLTAGE_SAFETY_THRESHOLD                    2200

/* Flash protect module */
/* Only the firmware area will be locked, the NV data area will not be locked.
 * For details, please refer to drv_flash.c file.
 */
#define FLASH_PROTECT_ENABLE						1

/* Watch dog module */
#define MODULE_WATCHDOG_ENABLE						0

/* UART module */
#if ZBHCI_UART
#define	MODULE_UART_ENABLE							1
#endif

/* USB module */
#if (ZBHCI_USB_CDC || ZBHCI_USB_HID)
#define MODULE_USB_ENABLE							1
#if ZBHCI_USB_CDC
	#define USB_CDC_ENABLE							1
	#define USB_VENDOR_ENABLE						0
#elif ZBHCI_USB_HID
	#define USB_CDC_ENABLE							0
	#define USB_VENDOR_ENABLE						1
#endif
#endif

#if (ZBHCI_USB_PRINT || ZBHCI_USB_CDC || ZBHCI_USB_HID || ZBHCI_UART || ZBHCI_BLE)
	#define ZBHCI_EN								1
#endif


/**********************************************************************
 * ZCL cluster support setting
 */
#define ZCL_ON_OFF_SUPPORT							1
#define ZCL_LEVEL_CTRL_SUPPORT						1
#define ZCL_LIGHT_COLOR_CONTROL_SUPPORT				1
#define ZCL_DOOR_LOCK_SUPPORT						1
#define ZCL_TEMPERATURE_MEASUREMENT_SUPPORT			1
#define ZCL_IAS_ZONE_SUPPORT						1
#define ZCL_POLL_CTRL_SUPPORT						1
#define ZCL_GROUP_SUPPORT							1
#define ZCL_SCENE_SUPPORT							1
#define ZCL_OTA_SUPPORT								1

#define AF_TEST_ENABLE								0


/**********************************************************************
 * Stack configuration
 */
#include "stack_cfg.h"

/**********************************************************************
 * BLE configuration
 */

//ble_single_conn
#if defined(MCU_CORE_8258) || defined(MCU_CORE_B91)
#define APP_SECURITY_ENABLE      					1
#define APP_DIRECT_ADV_ENABLE						1
//ble_multi_conn
#elif defined(MCU_CORE_TL321X) || defined(MCU_CORE_TL721X)
#define ACL_CENTRAL_MAX_NUM							0 // ACL central maximum number
#define ACL_PERIPHR_MAX_NUM							1 // ACL peripheral maximum number

///////////////////////// Feature Configuration////////////////////////////////////////////////
#define ACL_PERIPHR_SMP_ENABLE						1  //1 for smp,  0 no security
#define ACL_CENTRAL_SMP_ENABLE						0  //1 for smp,  0 no security
#define BLE_OTA_SERVER_ENABLE						0

///////////////////////// OS settings /////////////////////////////////////////////////////////
#define FREERTOS_ENABLE								0

///////////////////////// DEBUG  Configuration ////////////////////////////////////////////////
#define APP_LOG_EN									1
#define APP_CONTR_EVT_LOG_EN						1	//controller event
#define APP_SMP_LOG_EN								1

extern unsigned int  tlk_flash_mid;
extern unsigned int  tlk_flash_vendor;
extern unsigned char tlk_flash_capacity;
#endif
/**********************************************************************
 * EV configuration
 */
typedef enum{
	EV_POLL_ED_DETECT,
	EV_POLL_HCI,
    EV_POLL_IDLE,
	EV_POLL_MAX,
}ev_poll_e;

/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif
