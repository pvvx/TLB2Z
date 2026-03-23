/*
 * board_pb03f_kit.h
 *
 *  Created on: 12 нояб. 2023 г.
 *      Author: pvvx
 */
#ifndef _BOARD_TS0001_TZ3000_H_
#define _BOARD_TS0001_TZ3000_H_

#include "version_cfg.h"

#if (BOARD == BOARD_TS0001_TZ3000_RBZ)
#define BLE_MODEL_STR 			{"REL-BZ01"}
#define BLE_MAN_STR 			{"TLB2Z"}
#define BLE_NAME_PEFIX 			{'B','2','Z','-'} // fix 4 char
#define ZCL_BASIC_MODEL_ID		{8,'R','E','L','-','B','Z','0','1'} // REL-BZ01
#define ZCL_BASIC_MFG_NAME		{3,'B','2','Z'} // B2Z

#define ZIGBEE_TUYA_OTA 	1

// https://pvvx.github.io/TS0001_TZ3000/
// PB4  - LED
// PB5  - KEY
// PB1	- UART TX
// PA0  - UART RX
// PC3  - Relay

#define	BUTTON_ON			0
#define	BUTTON1				GPIO_PB5
#define PB5_FUNC			AS_GPIO
#define PB5_OUTPUT_ENABLE	0
#define PB5_INPUT_ENABLE	1
#define	PULL_WAKEUP_SRC_PB5	PM_PIN_PULLUP_10K

// LED
#define LED_ON				0
#define LED_OFF				(!LED_ON)

#define LED_FLASH_RGBE		0

#define GPIO_LED			GPIO_PB4	// Zigbee light_blink
#define PB4_DATA_OUT		LED_OFF
#define PB4_OUTPUT_ENABLE	1
#define PB4_INPUT_ENABLE	1
#define PB4_FUNC			AS_GPIO

// RELAY
#define GPIO_RELAY			GPIO_PC3
#define PC3_DATA_OUT		LED_OFF
#define PC3_OUTPUT_ENABLE	1
#define PC3_INPUT_ENABLE	1
#define PC3_FUNC			AS_GPIO


// I2C
#define USE_I2C_DRV			I2C_DRV_NONE
#if USE_I2C_DRV
#define I2C_CLOCK			100000 // Hz
#define I2C_SCL 			GPIO_PC0
#define I2C_SDA 			GPIO_PC1
#define I2C_GROUP 			I2C_GPIO_GROUP_C0C1
#define PULL_WAKEUP_SRC_PC0	PM_PIN_PULLUP_10K
#define PULL_WAKEUP_SRC_PC1	PM_PIN_PULLUP_10K
#endif

// VBAT
#define SHL_ADC_VBAT		C5P // see in adc.h ADC_InputPchTypeDef
#define GPIO_VBAT			GPIO_PC5 // missing pin on case TLSR825x
#define PC5_INPUT_ENABLE	0
#define PC5_DATA_OUT		1
#define PC5_OUTPUT_ENABLE	1
#define PC5_FUNC			AS_GPIO
#define PULL_WAKEUP_SRC_PC5	PM_PIN_PULLUP_10K

// UART
#if ZBHCI_UART
#define GPIO_TX			GPIO_PB1
#define GPIO_RX			GPIO_PA0
#define PA0_OUTPUT_ENABLE	0
#define PULL_WAKEUP_SRC_PA0 PM_PIN_PULLUP_1M
#define PA0_FUNC		AS_GPIO
//	#error please configurate uart PIN!!!!!!
#endif

// DEBUG
#if UART_PRINTF_MODE
#define	DEBUG_INFO_TX_PIN	GPIO_PB1 //print
#define PB1_FUNC			AS_GPIO
#define PB1_DATA_OUT		1
#define PB1_OUTPUT_ENABLE	1
#endif

#endif // BOARD == BOARD_TS0001_TZ3000_RBZ
#endif /* _BOARD_TS0001_TZ3000_H_ */
