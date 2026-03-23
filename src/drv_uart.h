/*
 * drv_uart.h
 *
 *  Created on: 04.03.2024
 *      Author: pvvx
 */

#ifndef _DRV_UART_H_
#define _DRV_UART_H_

//--- UART DMA buffers ----------------------------
#define DATA_BUFF_SIZE 		256 // = EXTADV_RPT_DATA_LEN_MAX + ?
#define UART_DMA_BUFF_SIZE (DATA_BUFF_SIZE+16)
#define UART_RX_BUFF_SIZE	UART_DMA_BUFF_SIZE
#define UART_TX_BUFF_SIZE	UART_DMA_BUFF_SIZE

void init_uart(int uart_baud);
int uart_send(u8 * src, u32 len);
int uart_read(u8 * des, u32 maxlen);

#endif /* _DRV_UART_H_ */
