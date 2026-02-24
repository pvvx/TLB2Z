/*
 * board_zg807z.h
 *      Author: pvvx
 */
#ifndef _BOARD_ZG807Z_H_
#define _BOARD_ZG807Z_H_

#include "version_cfg.h"

#if (BOARD == BOARD_ZG807Z)
#define BLE_MODEL_STR 			{"ZG-807Z"}
#define BLE_MAN_STR 			{"TLB2Z"}
#define BLE_NAME_PEFIX 			{'B','2','Z','-'} // fix 4 char
#define ZCL_BASIC_MODEL_ID		{7,'Z','G','-','8','0','7','Z'} // ZG-807Z
#define ZCL_BASIC_MFG_NAME		{3,'B','2','Z'} // B2Z

#define ZIGBEE_TUYA_OTA 	0

// https://pvvx.github.io/ZG-807Z/
// PC3	RF RXEN
// PC4	RF TXEN
// PD4	KEY (On:GND)
// PA0	LED (On:"0")
// PB1  TX
// PB7	RX
// PA7  SWS

#define	BUTTON_ON			0
#define	BUTTON1				GPIO_PD4
#define PD4_FUNC			AS_GPIO
#define PD4_OUTPUT_ENABLE	0
#define PD4_INPUT_ENABLE	1
#define	PULL_WAKEUP_SRC_PD4	PM_PIN_PULLUP_10K

// LED
#define LED_ON				0
#define LED_OFF				(!LED_ON)

#define GPIO_LED			GPIO_PA0	// Zigbee light_blink
#define PA0_DATA_OUT		LED_OFF
#define PA0_OUTPUT_ENABLE	1
#define PA0_INPUT_ENABLE	1
#define PA0_FUNC			AS_GPIO

#define LED_FLASH_RGBE		0

// PA
#define PA_ENABLE			1
#define PA_TX				GPIO_PC4
#define PA_RX				GPIO_PC3

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

// UART
#if ZBHCI_UART
#define GPIO_TX			GPIO_PB1
#define GPIO_RX			GPIO_PB7
#define PB7_OUTPUT_ENABLE	0
#define PULL_WAKEUP_SRC_PB7 PM_PIN_PULLUP_1M
#define PB7_FUNC		AS_GPIO
//	#error please configurate uart PIN!!!!!!
#endif

// DEBUG
#if UART_PRINTF_MODE
#define	DEBUG_INFO_TX_PIN	GPIO_PB1 //print
#define PB1_FUNC			AS_GPIO
#define PB1_DATA_OUT		1
#define PB1_OUTPUT_ENABLE	1
#endif

#endif // BOARD == BOARD_ZG807Z
#endif /* _BOARD_ZG807Z_H_ */
