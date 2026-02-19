/********************************************************************************************************
 * @file    tlkapi_debug.c
 *
 * @brief   This is the source file for BLE SDK
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
#if defined(MCU_CORE_TL321X) || defined(MCU_CORE_TL721X)
#include "tl_common.h"
#include "stack/ble/ble.h"
#include "tlkapi_debug.h"

#include <stdarg.h>

_attribute_ble_data_retention_ tlk_dbg_t tlkDbgCtl = {
	.usb_id = 0x120,
};

_attribute_ble_data_retention_ my_fifo_t   *tlkapi_print_fifo = NULL;



#if (TLKAPI_DEBUG_ENABLE)




//MYFIFO_INIT_IRAM(print_fifo, TLKAPI_DEBUG_FIFO_SIZE, TLKAPI_DEBUG_FIFO_NUM);
u8   print_fifo_b[TLKAPI_DEBUG_FIFO_SIZE * TLKAPI_DEBUG_FIFO_NUM];

_attribute_ble_data_retention_	  my_fifo_t	  print_fifo = {
												TLKAPI_DEBUG_FIFO_SIZE,
												TLKAPI_DEBUG_FIFO_NUM,
												0,
												0,
												print_fifo_b};









/**
 * @brief  		customize USB ID for UDB channel
 * 				user can open more than one USB debug tool on PC with different USB ID
 * @param[in]	cus_usb_id - customized USB ID
 * @return		none
 */
void tlkapi_debug_customize_usb_id(u16 cus_usb_id)
{
	tlkDbgCtl.usb_id = cus_usb_id;
}




/**
 * @brief   	Debug log initialization when MCU power on or wake_up from deepSleep mode
 * @param[in]	none
 * @return		none
 */
int tlkapi_debug_init(void)
{
	tlkDbgCtl.dbg_en = 1;

	if (!tlkapi_print_fifo){
		tlkapi_print_fifo = &print_fifo;
	}
	tlkapi_print_fifo->wptr = tlkapi_print_fifo->rptr = 0;

	#if (TLKAPI_DEBUG_CHANNEL == TLKAPI_DEBUG_CHANNEL_GSUART)
		tlkDbgCtl.dbg_chn = TLKAPI_DEBUG_CHANNEL_GSUART;
	#endif

	#if (TLKAPI_USE_INTERNAL_SPECIAL_UART_TOOL)
		tlkDbgCtl.fifo_format_len = 12;
	#else
		tlkDbgCtl.fifo_format_len = 0;
	#endif

	tlkDbgCtl.fifo_data_len = tlkapi_print_fifo->size - tlkDbgCtl.fifo_format_len;

	return 0;
}


/**
 * @brief   	Debug log initialization when MCU wake_up from deepSleep_retention mode
 * @param[in]	none
 * @return		none
 */
int tlkapi_debug_deepRetn_init(void)
{

	#if (TLKAPI_DEBUG_CHANNEL == TLKAPI_DEBUG_CHANNEL_GSUART)
		gpio_set_gpio_en(DEBUG_INFO_TX_PIN);
		gpio_write(DEBUG_INFO_TX_PIN, 1);
		gpio_set_output_en(DEBUG_INFO_TX_PIN, 1);
	#endif

	return 0;
}



/**
 * @brief   	Debug log process in mainloop, output log form log FIFO if FIFO not empty
 * @param[in]	none
 * @return		none
 */
_attribute_ram_code_sec_noinline_
void tlkapi_debug_handler(void)
{
	uint08 *pData;
	if (tlkapi_print_fifo->wptr != tlkapi_print_fifo->rptr){
		pData =  tlkapi_print_fifo->p + (tlkapi_print_fifo->rptr++ & (tlkapi_print_fifo->num - 1)) * tlkapi_print_fifo->size;
	}
	else{
		return;
	}
	uint16 dataLen = ((uint16)pData[1] << 8) | pData[0];
	for(int i=0; i<dataLen; i++){
		drv_putchar(pData[4+i]);
	}
}


#endif
/**
 * @brief  		check if debug log busy
 * @param[in]	none
 * @return		1: debug log busy, some log pending in FIFO, not all finish;
 * 				0: debug log empty
 */
bool tlkapi_debug_isBusy(void)
{
	if(tlkapi_print_fifo){
		return (tlkapi_print_fifo->wptr != tlkapi_print_fifo->rptr);
	}
	else{
		return 0;
	}
}


__attribute__((section(".data"))) unsigned char hex_table[] = "0123456789abcdef"; //improve: can not optimized to rodata
/**
 * @brief   Send debug log to log FIFO, character string and data mixed mode.
 *			attention: here just send log to FIFO, can not output immediately, wait for "tlkapi debug_handler" to output log.
*/
_attribute_ram_code_sec_noinline_
void tlkapi_send_str_data (char *str, u8 *pData, u32 data_len)
{
	if(!tlkapi_print_fifo){
		return;
	}

	if(data_len > 500){
		return;
	}

	extern int tlk_strlen(const char *str);
	int ns = str ? tlk_strlen (str) : 0;
	if (ns > tlkDbgCtl.fifo_data_len)
	{
		ns = tlkDbgCtl.fifo_data_len;
		data_len = 0;
	}
	else if (data_len + ns > tlkDbgCtl.fifo_data_len)
	{
		data_len = tlkDbgCtl.fifo_data_len - ns;
	}

	u32 r = irq_disable();

	u8 *pd =  tlkapi_print_fifo->p + (tlkapi_print_fifo->wptr & (tlkapi_print_fifo->num - 1)) * tlkapi_print_fifo->size;

	#if (TLKAPI_USE_INTERNAL_SPECIAL_UART_TOOL)
		int len = ns + data_len + 5;
		*pd++ = len;
		*pd++ = len >> 8;
		*pd++ = 0;
		*pd++ = 0;

		*pd++ = 0x95;   //special mark: 0xA695
		*pd++ = 0xA6;
		*pd++ = ns;     // string length, 1byte
		*pd++ = data_len;	    // data length, 2 byte
		*pd++ = data_len >> 8;

		while (ns--)
		{
			*pd++ = *str++;
		}
		while (data_len--)
		{
			*pd++ = *pData++;
		}
		//add a '\n' by UART tool
	#else
		u16 max_len = ((tlkDbgCtl.fifo_data_len - ns) - 3 - 4) / 3;
		if(data_len > max_len){
			data_len = max_len;
		}
		int len = ns + data_len * 3 + 3;
		*pd++ = len;
		*pd++ = len >> 8;
		*pd++ = 0;
		*pd++ = 0;
		while (ns--)
		{
			*pd++ = *str++;
		}

		*pd++ = ':';
		for (unsigned int i = 0; i < data_len; i++) {
			*pd++ = ' ';
			*pd++ = hex_table[*pData >> 4];
			*pd++ = hex_table[*pData & 0xf];
			pData++;
		}
		*pd++ = '\r';
		*pd = '\n';
	#endif

	tlkapi_print_fifo->wptr++;

	irq_restore(r);
}

#define SEND_U8S_MAX_NUM						16

_attribute_ram_code_sec_noinline_
void tlkapi_send_str_u8s(char *str, int size, ...)
{
	u8 d[SEND_U8S_MAX_NUM];
	size = min(SEND_U8S_MAX_NUM, size);
	va_list args;
	va_start( args, size);
	for(int i=0; i<size; i++)
	{
		d[i] = va_arg(args, unsigned int);
	}
	tlkapi_send_str_data(str, d, size);
	va_end( args );
}


#define SEND_U32S_MAX_NUM						8

_attribute_ram_code_sec_noinline_
void tlkapi_send_str_u32s(char *str, int size, ...)
{
	u32 d[SEND_U32S_MAX_NUM];
	size = min(SEND_U32S_MAX_NUM, size);
	va_list args;
	va_start( args, size);
	for(int i=0; i<size; i++)
	{
		d[i] = va_arg(args, unsigned int);
	}
	tlkapi_send_str_data(str, (u8*)d, size*4);
	va_end( args );
}


/**
 * @brief   	Send debug log to log FIFO, printf mode
 *				attention: here just send log to FIFO, can not output immediately, wait for "tlkapi debug_handler" to output log.
 * @param[in]	format -
 * @return
 */
int tlk_printf(const char *format, ...)
{
	if(!tlkapi_print_fifo){
		return 0;
	}

	u8 *pd =  tlkapi_print_fifo->p + (tlkapi_print_fifo->wptr & (tlkapi_print_fifo->num - 1)) * tlkapi_print_fifo->size;
	int ret;

#if ((MCU_CORE_TYPE == MCU_CORE_B91) || (MCU_CORE_TYPE == MCU_CORE_TL321X) || (MCU_CORE_TYPE == MCU_CORE_TL721X))
	va_list args;
	va_start( args, format );

	ret = vsnprintf((char*)(pd + 4), tlkDbgCtl.fifo_data_len, format, args);

	va_end( args );
#else
    #error "print_f process for other MCU !!!"
#endif

	int len = ret;
	*pd++ = len;
	*pd++ = len >> 8;
	*pd++ = 0;
	*pd++ = 0;
	//pd += ret;
	//*pd++ = '\n';

	tlkapi_print_fifo->wptr ++;

	return ret;
}
#endif
