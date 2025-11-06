/*
 * board_pb03f_kit.h
 *
 *  Created on: 12 нояб. 2023 г.
 *      Author: pvvx
 */
#ifndef _BOARD_TB03F_KIT_H_
#define _BOARD_TB03F_KIT_H_

#include "version_cfg.h"

#if (BOARD == BOARD_TB03F_KIT)

#define ZIGBEE_TUYA_OTA 	0

//// TB-03F-KIT
#define HW_VERSION 16 // DIY TB-03F-Kit
// PC2,3,4 - LED_RGB
// PB4,5 - LED1, LED2
// PB1	- UART TX
// PA0  - UART RX
// PA7  - KEY

#define	BUTTON1				GPIO_PA7
#define PA7_FUNC					AS_GPIO
#define PA7_OUTPUT_ENABLE			0
#define PA7_INPUT_ENABLE			1
#define	PULL_WAKEUP_SRC_PA7			PM_PIN_PULLUP_10K

#define GPIO_LED_B		GPIO_PC2
#define PC2_DATA_OUT		0
#define PC2_OUTPUT_ENABLE	1
#define PC2_INPUT_ENABLE	1
#define PC2_FUNC			AS_GPIO
#define PWM_LED_B		PWM0_ID

#define GPIO_LED_R		GPIO_PC3
#define PC3_DATA_OUT		0
#define PC3_OUTPUT_ENABLE	1
#define PC3_INPUT_ENABLE	1
#define PC3_FUNC			AS_GPIO
#define PWM_LED_R		PWM1_ID

#define GPIO_LED_G		GPIO_PC4
#define PC4_DATA_OUT		0
#define PC4_OUTPUT_ENABLE	1
#define PC4_INPUT_ENABLE	1
#define PC4_FUNC			AS_GPIO
#define PWM_LED_G		PWM2_ID

#define GPIO_LED_E		GPIO_PB4	// tx adv
#define PB4_DATA_OUT		0
#define PB4_OUTPUT_ENABLE	1
#define PB4_INPUT_ENABLE	1
#define PB4_FUNC			AS_GPIO

#define GPIO_LED_W		GPIO_PB5
#define PB5_DATA_OUT		0
#define PB5_OUTPUT_ENABLE	1
#define PB5_INPUT_ENABLE	1
#define PB5_FUNC			AS_GPIO

// DISPLAY
#define	USE_DISPLAY			0

#define USE_SCAN			1
// I2C
#define I2C_CLOCK			100000 // Hz

#define SENSOR_TYPE 		SENSOR_BLE

#define USE_SENSOR_ID		0
#define I2C_SCL 			GPIO_PC0
#define I2C_SDA 			GPIO_PC1
#define I2C_GROUP 			I2C_GPIO_GROUP_C0C1
#define PULL_WAKEUP_SRC_PC0	PM_PIN_PULLUP_10K
#define PULL_WAKEUP_SRC_PC1	PM_PIN_PULLUP_10K

// LED
#define LED_ON				1
#define LED_OFF				0

#define GPIO_LED			GPIO_LED_W // Zigbee

// VBAT
#define SHL_ADC_VBAT		C5P // see in adc.h ADC_InputPchTypeDef
#define GPIO_VBAT			GPIO_PC5 // missing pin on case TLSR825x
#define PC5_INPUT_ENABLE	0
#define PC5_DATA_OUT		1
#define PC5_OUTPUT_ENABLE	1
#define PC5_FUNC			AS_GPIO

// UART
#if 1 // ZBHCI_UART
#define GPIO_TX			GPIO_PB1
#define GPIO_RX			GPIO_PA0
#define PA0_OUTPUT_ENABLE	0
#define PULL_WAKEUP_SRC_PA0 PM_PIN_PULLUP_1M
#define PA0_FUNC		AS_GPIO
//	#error please configurate uart PIN!!!!!!
#endif

// DEBUG
#if UART_PRINTF_MODE
	#define	DEBUG_INFO_TX_PIN	    GPIO_SWS //print
#endif

#endif // BOARD == TB03F_KIT
#endif /* _BOARD_TB03F_KIT_H_ */
