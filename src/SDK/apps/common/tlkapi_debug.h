/********************************************************************************************************
 * @file    tlkapi_debug.h
 *
 * @brief   This is the header file for BLE SDK
 *
 * @author  BLE GROUP
 * @date    06,2022
 *
 * @par     Copyright (c) 2022, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
#ifndef TLKAPI_DEBUG_H_
#define TLKAPI_DEBUG_H_

#if defined(MCU_CORE_TL321X) || defined(MCU_CORE_TL721X)

/**
 * @brief	Telink debug log enable or disable
 * 			user can set it in app_config.h, default disable if user not set it
 */
#ifndef	TLKAPI_DEBUG_ENABLE
#define TLKAPI_DEBUG_ENABLE							0
#endif



/**
 * @brief	Telink debug log channel select
 * 			user can select one type in app_config.h
 */
#define TLKAPI_DEBUG_CHANNEL_UDB      				1	//USB Dump
#define TLKAPI_DEBUG_CHANNEL_GSUART   				2 	//GPIO simulate UART. only support
#define TLKAPI_DEBUG_CHANNEL_UART     				3   //hardware UART

/* if user enable debug log but not select log channel, will use "GPIO simulate UART" as default */
#ifndef	TLKAPI_DEBUG_CHANNEL
#define TLKAPI_DEBUG_CHANNEL         		 		TLKAPI_DEBUG_CHANNEL_GSUART
#endif

/**
 * @brief	default log FIFO size, user can change it in app_config.h
 */
#ifndef	TLKAPI_DEBUG_FIFO_SIZE
#define TLKAPI_DEBUG_FIFO_SIZE						288
#endif

/**
 * @brief	default log FIFO number, user can change it in app_config.h
 */
#ifndef	TLKAPI_DEBUG_FIFO_NUM
#define TLKAPI_DEBUG_FIFO_NUM						8
#endif


/* internal special UART tool with high efficiency, but not publicly release
 * so only for internal debug, customer never change this macro */
#ifndef	TLKAPI_USE_INTERNAL_SPECIAL_UART_TOOL
#define TLKAPI_USE_INTERNAL_SPECIAL_UART_TOOL		0
#endif



/**
 * @brief	Debug log control data structure, user do not need to pay attention to these
 */
typedef struct{
	u8	dbg_en;
	u8	dbg_chn;
	u8	uartSendIsBusy;
	u8	fifo_format_len;

	u16	usb_id;
	u16	fifo_data_len;
}tlk_dbg_t;
extern tlk_dbg_t tlkDbgCtl;





/**
 * @brief   	Debug log initialization when MCU power on or wake_up from deepSleep mode
 * @param[in]	none
 * @return		none
 */
int  tlkapi_debug_init(void);


/**
 * @brief   	Debug log initialization when MCU wake_up from deepSleep_retention mode
 * @param[in]	none
 * @return		none
 */
int  tlkapi_debug_deepRetn_init(void);


/**
 * @brief   	Debug log process in mainloop, output log form log FIFO if FIFO not empty
 * @param[in]	none
 * @return		none
 */
void tlkapi_debug_handler(void);


/**
 * @brief  		check if debug log busy
 * @param[in]	none
 * @return		1: debug log busy, some log pending in FIFO, not all finish;
 * 				0: debug log empty
 */
bool tlkapi_debug_isBusy(void);


/**
 * @brief  		customize USB ID for UDB channel
 * 				user can open more than one USB debug tool on PC with different USB ID
 * @param[in]	cus_usb_id - customized USB ID
 * @return		none
 */
void tlkapi_debug_customize_usb_id(u16 cus_usb_id);







/**
 * @brief	user do not need to pay attention to three APIs below, and do not use them in application.
 */
void tlkapi_send_str_data (char *str, u8 *pData, u32 data_len);
void tlkapi_send_str_u8s(char *str, int size, ...);
void tlkapi_send_str_u32s(char *str, int size, ...);



/**
 * @brief   	Send debug log to log FIFO, character string and data mixed mode.
 *				attention: here just send log to FIFO, can not output immediately, wait for "tlkapi debug_handler" to output log.
 * @param[in]	en - send log enable, 1: enable;  0: disable
 * @param[in]	str - character string
 * @param[in]	pData - pointer of data
 * @param[in]	len - length of data
 * @return		1: send to FIFO success; 0: send to FIFO fail
 */
#define tlkapi_send_string_data(en, str, pData, len)		if(en){tlkapi_send_str_data(str,(u8*)(u32)(pData), len);}


/**
 * @brief   	Send debug log to log FIFO, character string and data mixed mode, with variable length data, data in "unsigned int" format
 *				attention: here just send log to FIFO, can not output immediately, wait for "tlkapi debug_handler" to output log.
 * @param[in]	en - send log enable, 1: enable;  0: disable
 * @param[in]	str - character string
 * @param[in]	... - variable length data, maximum length is 8
 * @param[in]	data_len - length of data
 * @return		1: send to FIFO success; 0: send to FIFO fail
 */
#define tlkapi_send_string_u32s(en, str, ...)				if(en){tlkapi_send_str_u32s(str, COUNT_ARGS(__VA_ARGS__), ##__VA_ARGS__);}


/**
 * @brief   	Send debug log to log FIFO, character string and data mixed mode, with variable length data, data in "unsigned char" format
 *				attention: here just send log to FIFO, can not output immediately, wait for "tlkapi debug_handler" to output log.
 * @param[in]	en - send log enable, 1: enable;  0: disable
 * @param[in]	str - character string
 * @param[in]	... - variable length data, maximum length is 16
 * @param[in]	data_len - length of data
 * @return		1: send to FIFO success; 0: send to FIFO fail
 */
#define tlkapi_send_string_u8s(en, str, ...)				if(en){tlkapi_send_str_u8s(str, COUNT_ARGS(__VA_ARGS__), ##__VA_ARGS__);}





/**
 * @brief   	Send debug log to log FIFO, string only mode
 *				attention: here just send log to FIFO, can not output immediately, wait for "tlkapi debug_handler" to output log.
 * @param[in]	fmt - please refer to standard C function "printf"
 * @return		none
 */
int  tlk_printf(const char *fmt, ...);

/**
 * @brief   	Send debug log to log FIFO, string only mode
 *				attention: here just send log to FIFO, can not output immediately, wait for "tlkapi debug_handler" to output log.
 * @param[in]	en - send log enable, 1: enable;  0: disable
 * @param[in]	fmt - please refer to standard C function "printf"
 * @return		none
 */
#define tlkapi_printf(en, fmt, ...)							if(en){tlk_printf(fmt, ##__VA_ARGS__);}



#ifndef	APP_LOG_EN
#define APP_LOG_EN									0
#endif


#define BLT_APP_LOG(fmt, ...)							if(APP_LOG_EN) tlkapi_printf(1, "[APP]"fmt "\n", ##__VA_ARGS__);
#define BLT_APP_STR_LOG      							if(APP_LOG_EN) tlkapi_send_str_data
#endif


#ifndef DBG_CHN0_LOW
#define DBG_CHN0_LOW
#endif

#ifndef DBG_CHN0_HIGH
#define DBG_CHN0_HIGH
#endif

#ifndef DBG_CHN0_TOGGLE
#define DBG_CHN0_TOGGLE
#endif


#ifndef DBG_CHN1_LOW
#define DBG_CHN1_LOW
#endif

#ifndef DBG_CHN1_HIGH
#define DBG_CHN1_HIGH
#endif

#ifndef DBG_CHN1_TOGGLE
#define DBG_CHN1_TOGGLE
#endif


#ifndef DBG_CHN2_LOW
#define DBG_CHN2_LOW
#endif

#ifndef DBG_CHN2_HIGH
#define DBG_CHN2_HIGH
#endif

#ifndef DBG_CHN2_TOGGLE
#define DBG_CHN2_TOGGLE
#endif


#ifndef DBG_CHN3_LOW
#define DBG_CHN3_LOW
#endif

#ifndef DBG_CHN3_HIGH
#define DBG_CHN3_HIGH
#endif

#ifndef DBG_CHN3_TOGGLE
#define DBG_CHN3_TOGGLE
#endif


#ifndef DBG_CHN4_LOW
#define DBG_CHN4_LOW
#endif

#ifndef DBG_CHN4_HIGH
#define DBG_CHN4_HIGH
#endif

#ifndef DBG_CHN4_TOGGLE
#define DBG_CHN4_TOGGLE
#endif


#ifndef DBG_CHN5_LOW
#define DBG_CHN5_LOW
#endif

#ifndef DBG_CHN5_HIGH
#define DBG_CHN5_HIGH
#endif

#ifndef DBG_CHN5_TOGGLE
#define DBG_CHN5_TOGGLE
#endif


#ifndef DBG_CHN6_LOW
#define DBG_CHN6_LOW
#endif

#ifndef DBG_CHN6_HIGH
#define DBG_CHN6_HIGH
#endif

#ifndef DBG_CHN6_TOGGLE
#define DBG_CHN6_TOGGLE
#endif


#ifndef DBG_CHN7_LOW
#define DBG_CHN7_LOW
#endif

#ifndef DBG_CHN7_HIGH
#define DBG_CHN7_HIGH
#endif

#ifndef DBG_CHN7_TOGGLE
#define DBG_CHN7_TOGGLE
#endif


#ifndef DBG_CHN8_LOW
#define DBG_CHN8_LOW
#endif

#ifndef DBG_CHN8_HIGH
#define DBG_CHN8_HIGH
#endif

#ifndef DBG_CHN8_TOGGLE
#define DBG_CHN8_TOGGLE
#endif


#ifndef DBG_CHN9_LOW
#define DBG_CHN9_LOW
#endif

#ifndef DBG_CHN9_HIGH
#define DBG_CHN9_HIGH
#endif

#ifndef DBG_CHN9_TOGGLE
#define DBG_CHN9_TOGGLE
#endif


#ifndef DBG_CHN10_LOW
#define DBG_CHN10_LOW
#endif

#ifndef DBG_CHN10_HIGH
#define DBG_CHN10_HIGH
#endif

#ifndef DBG_CHN10_TOGGLE
#define DBG_CHN10_TOGGLE
#endif



#ifndef DBG_CHN11_LOW
#define DBG_CHN11_LOW
#endif

#ifndef DBG_CHN11_HIGH
#define DBG_CHN11_HIGH
#endif

#ifndef DBG_CHN11_TOGGLE
#define DBG_CHN11_TOGGLE
#endif


#ifndef DBG_CHN12_LOW
#define DBG_CHN12_LOW
#endif

#ifndef DBG_CHN12_HIGH
#define DBG_CHN12_HIGH
#endif

#ifndef DBG_CHN12_TOGGLE
#define DBG_CHN12_TOGGLE
#endif


#ifndef DBG_CHN13_LOW
#define DBG_CHN13_LOW
#endif

#ifndef DBG_CHN13_HIGH
#define DBG_CHN13_HIGH
#endif

#ifndef DBG_CHN13_TOGGLE
#define DBG_CHN13_TOGGLE
#endif


#ifndef DBG_CHN14_LOW
#define DBG_CHN14_LOW
#endif

#ifndef DBG_CHN14_HIGH
#define DBG_CHN14_HIGH
#endif

#ifndef DBG_CHN14_TOGGLE
#define DBG_CHN14_TOGGLE
#endif


#ifndef DBG_CHN15_LOW
#define DBG_CHN15_LOW
#endif

#ifndef DBG_CHN15_HIGH
#define DBG_CHN15_HIGH
#endif

#ifndef DBG_CHN15_TOGGLE
#define DBG_CHN15_TOGGLE
#endif


#ifndef APP_DBG_CHN_0_LOW
#define APP_DBG_CHN_0_LOW
#endif

#ifndef APP_DBG_CHN_0_HIGH
#define APP_DBG_CHN_0_HIGH
#endif

#ifndef APP_DBG_CHN_1_LOW
#define APP_DBG_CHN_1_LOW
#endif
#ifndef APP_DBG_CHN_1_HIGH
#define APP_DBG_CHN_1_HIGH
#endif


#ifndef APP_DBG_CHN_2_LOW
#define APP_DBG_CHN_2_LOW
#endif
#ifndef APP_DBG_CHN_2_HIGH
#define APP_DBG_CHN_2_HIGH
#endif

#ifndef APP_DBG_CHN_3_LOW
#define APP_DBG_CHN_3_LOW
#endif
#ifndef APP_DBG_CHN_3_HIGH
#define APP_DBG_CHN_3_HIGH
#endif

#ifndef APP_DBG_CHN_4_LOW
#define APP_DBG_CHN_4_LOW
#endif
#ifndef APP_DBG_CHN_4_HIGH
#define APP_DBG_CHN_4_HIGH
#endif

#ifndef APP_DBG_CHN_5_LOW
#define APP_DBG_CHN_5_LOW
#endif
#ifndef APP_DBG_CHN_5_HIGH
#define APP_DBG_CHN_5_HIGH
#endif

#ifndef APP_DBG_CHN_6_LOW
#define APP_DBG_CHN_6_LOW
#endif
#ifndef APP_DBG_CHN_6_HIGH
#define APP_DBG_CHN_6_HIGH
#endif

#ifndef APP_DBG_CHN_7_LOW
#define APP_DBG_CHN_7_LOW
#endif
#ifndef APP_DBG_CHN_7_HIGH
#define APP_DBG_CHN_7_HIGH
#endif

#ifndef APP_DBG_CHN_8_LOW
#define APP_DBG_CHN_8_LOW
#endif
#ifndef APP_DBG_CHN_8_HIGH
#define APP_DBG_CHN_8_HIGH
#endif

#ifndef APP_DBG_CHN_9_LOW
#define APP_DBG_CHN_9_LOW
#endif
#ifndef APP_DBG_CHN_9_HIGH
#define APP_DBG_CHN_9_HIGH
#endif

#ifndef APP_DBG_CHN_10_LOW
#define APP_DBG_CHN_10_LOW
#endif
#ifndef APP_DBG_CHN_10_HIGH
#define APP_DBG_CHN_10_HIGH
#endif

#ifndef APP_DBG_CHN_11_LOW
#define APP_DBG_CHN_11_LOW
#endif
#ifndef APP_DBG_CHN_11_HIGH
#define APP_DBG_CHN_11_HIGH
#endif

#ifndef APP_DBG_CHN_12_LOW
#define APP_DBG_CHN_12_LOW
#endif
#ifndef APP_DBG_CHN_12_HIGH
#define APP_DBG_CHN_12_HIGH
#endif

#ifndef APP_DBG_CHN_13_LOW
#define APP_DBG_CHN_13_LOW
#endif
#ifndef APP_DBG_CHN_13_HIGH
#define APP_DBG_CHN_13_HIGH
#endif

#ifndef APP_DBG_CHN_14_LOW
#define APP_DBG_CHN_14_LOW
#endif
#ifndef APP_DBG_CHN_14_HIGH
#define APP_DBG_CHN_14_HIGH
#endif

#ifndef APP_DBG_CHN_15_LOW
#define APP_DBG_CHN_15_LOW
#endif
#ifndef APP_DBG_CHN_15_HIGH
#define APP_DBG_CHN_15_HIGH
#endif

#endif /* TLKAPI_DEBUG_H_ */

