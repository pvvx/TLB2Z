/*
 * uart.c
 *
 *  Created on: 04.03.2024
 *      Author: pvvx
 */
#include "tl_common.h"
#if	USE_DEBUG_UART
//#include "register_8258.h"
#include "drv_uart.h"

//--- UART DMA buffers ----------------------------

typedef struct _dma_uart_buf_t {
	volatile u32 len;
	union __attribute__((packed)) {
		u8 uc[UART_DMA_BUFF_SIZE];
		u16 uw[UART_DMA_BUFF_SIZE>>1];
		u32 ud[UART_DMA_BUFF_SIZE>>2];
	};
}dma_uart_buf_t;

dma_uart_buf_t urxb;
dma_uart_buf_t utxb;


//-------------------------------

//_attribute_ram_code_
void init_uart(int uart_baud) {
	uart_reset();  //uart module power-on again.
	uart_init_baudrate(uart_baud, CLOCK_SYS_CLOCK_HZ, PARITY_NONE, STOP_BIT_ONE);
	uart_recbuff_init((unsigned char *)&urxb, DATA_BUFF_SIZE);
	uart_dma_enable(1, 1); 	//uart data in hardware buffer moved by dma, so we need enable them first
	dma_chn_irq_enable(FLD_DMA_CHN_UART_RX | FLD_DMA_CHN_UART_TX, 1);   	//uart Tx & Rx dma irq enable
	uart_irq_enable(0, 0);
	uart_gpio_set(GPIO_TX, GPIO_RX);
	reg_dma_rx_rdy0 = FLD_DMA_IRQ_UART_RX;
}

int uart_send(u8 * src, u32 len) {
	int ret = -1; // busy
	 if (reg_uart_status1 & FLD_UART_TX_DONE ) {
//		reg_dma_rx_rdy0 = FLD_DMA_IRQ_UART_TX;
		memcpy(utxb.uc, src, len);
		utxb.len = len;
		reg_dma1_addr = (unsigned short)((unsigned int)&utxb);
		reg_dma_tx_rdy0 |= FLD_DMA_CHN_UART_TX; // start tx
		ret = len;
	}
	return ret;
}

int uart_read(u8 * des, u32 maxlen) {
	int rxlen = 0;
//	if (reg_uart_status1 & FLD_UART_RX_DONE ) {
	if(reg_dma_rx_rdy0 & FLD_DMA_IRQ_UART_RX) { // new command?
		reg_uart_status0 |= FLD_UART_CLEAR_RX_FLAG | FLD_UART_RX_ERR_FLAG;
		rxlen = urxb.len;
		if(rxlen)
			memcpy(des, urxb.uc, (rxlen > maxlen)? maxlen : rxlen);
		reg_dma_rx_rdy0 = FLD_DMA_IRQ_UART_RX;
	}
	return rxlen;
}

#endif //#if	USE_DEBUG_UART
