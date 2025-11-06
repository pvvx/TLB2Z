/********************************************************************************************************
 * @file    board_tb03f_kit.h
 *
 * @brief   This is the header file for board_8258_dongle
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

#pragma once

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C" {
#endif


/*******************************************************************************************************
*********************tb03f_kit with 32 Pins: Start*****************************************************
*******************************************************************************************************/
//KEY
#define	BUTTON1 					GPIO_PA7
#define PA7_FUNC					AS_GPIO
#define PA7_OUTPUT_ENABLE			0
#define PA7_INPUT_ENABLE			1
#define	PULL_WAKEUP_SRC_PA7			PM_PIN_PULLUP_1M //PM_PIN_PULLUP_10K

#define	BUTTON2 					GPIO_PA1
#define PA1_FUNC					AS_GPIO
#define PA1_OUTPUT_ENABLE			0
#define PA1_INPUT_ENABLE			1
#define	PULL_WAKEUP_SRC_PA1			PM_PIN_PULLUP_1M // PM_PIN_PULLUP_10K

//LED

#define LED_R						GPIO_PC3
#define PC3_FUNC					AS_GPIO
#define PC3_OUTPUT_ENABLE			1
#define PC3_INPUT_ENABLE			0

#define PWM_R_CHANNEL				1 //PWM1
#define PWM_R_CHANNEL_SET()			do{	\
										gpio_set_func(LED_R, AS_PWM1); 	\
									}while(0)
										// drv_pwm_n_invert(PWM_R_CHANNEL);

#define LED_G						GPIO_PC2
#define PC2_FUNC					AS_GPIO
#define PC2_OUTPUT_ENABLE			1
#define PC2_INPUT_ENABLE			0

#define PWM_G_CHANNEL				0//PWM0
#define PWM_G_CHANNEL_SET()			do{	\
										gpio_set_func(LED_G, AS_PWM0); 	\
									}while(0)


#define LED_B						GPIO_PC4
#define PC4_FUNC					AS_GPIO
#define PC4_OUTPUT_ENABLE			1
#define PC4_INPUT_ENABLE			0

#define PWM_B_CHANNEL				2//PWM2
#define PWM_B_CHANNEL_SET()			do{	\
										gpio_set_func(LED_B, AS_PWM2); 	\
									}while(0)

#define R_LIGHT_PWM_CHANNEL			PWM_R_CHANNEL
#define G_LIGHT_PWM_CHANNEL			PWM_G_CHANNEL
#define B_LIGHT_PWM_CHANNEL			PWM_B_CHANNEL
#define R_LIGHT_PWM_SET()			PWM_R_CHANNEL_SET()
#define G_LIGHT_PWM_SET()			PWM_G_CHANNEL_SET()
#define B_LIGHT_PWM_SET()			PWM_B_CHANNEL_SET()


#define LED1						GPIO_PB3
#define PB3_FUNC					AS_GPIO
#define PB3_OUTPUT_ENABLE			1
#define PB3_INPUT_ENABLE			0

#define LED3						GPIO_PB4
#define PB4_FUNC					AS_GPIO
#define PB4_OUTPUT_ENABLE			1
#define PB4_INPUT_ENABLE			0

#define LED_W						GPIO_PB3

#define LED_POWER					GPIO_PB4
#define LED_PERMIT					LED_W

#define WARM_LIGHT_PWM_CHANNEL		PWM_R_CHANNEL
#define COOL_LIGHT_PWM_CHANNEL		PWM_B_CHANNEL
#define WARM_LIGHT_PWM_SET()		PWM_R_CHANNEL_SET()
#define COOL_LIGHT_PWM_SET()		PWM_B_CHANNEL_SET()


#define	PM_WAKEUP_LEVEL		  		PM_WAKEUP_LEVEL_LOW

#if ZBHCI_UART
	#define UART_TX_PIN         	UART_TX_PB1
	#define UART_RX_PIN         	UART_RX_PA0

	#define UART_PIN_CFG()			uart_gpio_set(UART_TX_PIN, UART_RX_PIN);// uart tx/rx pin set
#endif

// UART
#if ZBHCI_UART
	#error please configurate uart PIN!!!!!!
#endif

// DEBUG
#if UART_PRINTF_MODE
	#define	DEBUG_INFO_TX_PIN	    GPIO_PD3 // print // GPIO_PB1 - TXD
#endif
/*******************************************************************************************************
*********************tb03f_kit with 32 Pins: End*****************************************************
*******************************************************************************************************/

enum{
	VK_SW1 = 0x01,
	VK_SW2 = 0x02
};

#define	KB_MAP_NORMAL	{\
		{VK_SW1,}, \
		{VK_SW2,}, }

#define	KB_MAP_NUM		KB_MAP_NORMAL
#define	KB_MAP_FN		KB_MAP_NORMAL

#define KB_DRIVE_PINS  {NULL }
#define KB_SCAN_PINS   {BUTTON1,  BUTTON2}


/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif
