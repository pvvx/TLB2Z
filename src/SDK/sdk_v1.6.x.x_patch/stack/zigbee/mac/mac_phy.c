/********************************************************************************************************
 * @file     mac_phy.c
 *
 * @brief    mac phy APIs file
 *
 * @author
 * @date     Oct. 11, 2016
 *
 * @par      Copyright (c) 2016, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *
 *			 The information contained herein is confidential and proprietary property of Telink
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai)
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in.
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided.
 *
 *******************************************************************************************************/


/**********************************************************************
 * INCLUDES
 */
#include "../include/zb_common.h"


/**********************************************************************
 * LOCAL CONSTANTS
 */

#define RF_DROP_REASON_INVALID_CRC        0x01
#define RF_DROP_REASON_RF_BUSY            0x02
#define RF_DROP_REASON_EXPECT_ACK         0x03
#define RF_DROP_REASON_INVALIC_BEACON     0x04
#define RF_DROP_REASON_FILTER_PANID       0x05
#define RF_DROP_REASON_FILTER_DSTADDR     0x06
#define RF_DROP_REASON_FILTER_LEN         0x07
#define RF_DROP_REASON_INVALIC_FRAME_TYPE         0x08
#define MAC_FCF_SRC_ADDR_BIT   0xc0
#define RF_MAX_BLACK_LIST_SIZE  0x04

/**********************************************************************
 * LOCAL TYPES
 */
extern volatile u8 zigbee_process;
extern u8 rf_txTpGain;
volatile s8 soft_rssi;
volatile s32 sum_rssi, cnt_rssi = 1;

u8 rfMode =  RF_STATE_TX;

u8	g_zb_txPowerSet  =	RF_POWER_P10p46dBm;

_attribute_aligned_(4) u8 rf_tx_buf[ZB_RADIO_TX_HDR_LEN + 127];
_attribute_aligned_(4) u8 rf_ack_buf[ZB_RADIO_TX_HDR_LEN + 7];
/**********************************************************************
 * LOCAL VARIABLES
 */

rf_specificFunc_t rf_specificFuns;

u8 *rf_rxBuf = NULL;
extern ev_poll_callback_t rf_edDetect_ptr;

u8 fPaEn;
u32 rf_pa_txen_pin;
u32 rf_pa_rxen_pin;


#define ZB_SWTICH_TO_TXMODE()    do{ \
	if(fPaEn) {	\
		gpio_write(rf_pa_txen_pin, 1); 		\
		gpio_write(rf_pa_rxen_pin, 0); 		\
	}										\
	ZB_RADIO_TRX_SWITCH(RF_MODE_TX, LOGICCHANNEL_TO_PHYSICAL(rf_getChannel()));	\
}while(0)

#define ZB_SWTICH_TO_RXMODE()    do{ \
	if(fPaEn) {	\
		gpio_write(rf_pa_txen_pin, 0); 		\
		gpio_write(rf_pa_rxen_pin, 1); 		\
	}										\
	ZB_RADIO_TRX_SWITCH(RF_MODE_RX, LOGICCHANNEL_TO_PHYSICAL(rf_getChannel()));	\
}while(0)

/**********************************************************************
 * LOCAL FUNCTIONS
 */
 void rf_edDetect(void);

 _attribute_ram_code_ u32 mac_currentTickGet(void){
	 return clock_time();
 }

/*********************************************************************
 * @fn      rf_regProtocolSpecific
 *
 * @brief   Register protocol specific RF functions.
 *            Note: This function must be called before rf_init() and rf_reset()
 *
 * @param   txCbFunc - tx done callback function
 * @param   rxCbFunc - rx done callback function
 *
 * @return  none
 */
 _CODE_MAC_ void rf_regProtocolSpecific(rf_specificFunc_t* funcs)
{
    rf_specificFuns.initFunc  = funcs->initFunc;
    rf_specificFuns.resetFunc = funcs->resetFunc;
}

 void rf802154_tx_ready(u8* buf, u8 len)
 {
  	/* Fill the telink RF header */
  	rf_tx_buf[0] = len+1;
  	rf_tx_buf[1] = 0;
  	rf_tx_buf[2] = 0;
  	rf_tx_buf[3] = 0;

  	rf_tx_buf[4] = len+2;
  	memcpy(rf_tx_buf+5, buf, len);

  	DBG_ZIGBEE_STATUS(0x01);
 }

 _attribute_ram_code_ void rf802154_tx(u8* buf, u8 len)
{
 	/* Fill the telink RF header */
 	rf_setTrxState(RF_STATE_TX);
 	//reg_rf_irq_status = (FLD_RF_IRQ_RX | FLD_RF_IRQ_TX);
 	ZB_RADIO_TX_DONE_CLR;
 	ZB_RADIO_RX_DONE_CLR;
 	ZB_RADIO_TX_START(rf_tx_buf);//Manual mode

 	DBG_ZIGBEE_STATUS(0x05);
}

/*********************************************************************
 * @fn      rf_reset
 *
 * @brief   Reset RF module and do the RF power up.
 *
 * @param   none
 *
 * @return  none
 */
 _CODE_MAC_ void phy_reset(void)
{
    /* Add real RF HW initialize function */
    rf_setTxPower(g_zb_txPowerSet);

    rf_setChannel(g_zbMacPib.phyChannelCur);

    rf_setTrxState(RF_STATE_TX);

    if (rf_specificFuns.resetFunc) {
        rf_specificFuns.resetFunc();
    }

}


/*********************************************************************
 * @fn      rf_init
 *
 * @brief   Initialize RF module and do the RF power up.
 *
 * @param   none
 *
 * @return  none
 */
 _CODE_MAC_ void rf_init(void)
{
    phy_reset();

    /* Protocol specific initialization */
    if (rf_specificFuns.initFunc) {
        rf_specificFuns.initFunc();
    }


    ZB_RADIO_TRX_CFG((RF_PKT_BUFF_LEN));

    ZB_RADIO_RX_ENABLE;
	ZB_RADIO_TX_ENABLE;
	ZB_TIMESTAMP_ENABLE;

    /* Register ED-Scan polling function, but disable it at begining. */
    ev_on_poll(EV_POLL_ED_DETECT, rf_edDetect);
	ev_disable_poll(EV_POLL_ED_DETECT);

	rf_setTxPower(g_zb_txPowerSet);

}


 /* reconfig PHY when system recovery from deep sleep */
void mac_phyReconfig(void){
	rf_setTxPower(g_zb_txPowerSet);
	rf_setChannel(g_zbMacPib.phyChannelCur);
	rf_setTrxState(RF_STATE_TX);

	if(rf_rxBuf){
		rf_setRxBuf(rf_rxBuf);			//set the RX buffer
	}else{
		rf_setRxBuf(tl_getRxBuf());
	}
	ZB_RADIO_TRX_CFG((RF_PKT_BUFF_LEN));
	ZB_RADIO_RX_ENABLE;
	ZB_RADIO_TX_ENABLE;
}


 _CODE_MAC_ void mac_resetPhy(void){
	 ZB_RADIO_RESET();
 }

/*********************************************************************
 * @fn      rf_paInit
 *
 * @brief   Initialize PA.
 *
 * @param   none
 *
 * @param   none
 *
 * @return  none
 */
 _CODE_MAC_ void rf_paInit(u32 TXEN_pin, u32 RXEN_pin)
{
    rf_pa_txen_pin = TXEN_pin;
    rf_pa_rxen_pin = RXEN_pin;

    gpio_set_func(rf_pa_txen_pin, AS_GPIO);
    gpio_set_output_en(rf_pa_txen_pin, 1);
    gpio_write(rf_pa_txen_pin, 0);

    // RXEN
    gpio_set_func(rf_pa_rxen_pin, AS_GPIO);
    gpio_set_output_en(rf_pa_rxen_pin, 1);
    gpio_write(rf_pa_rxen_pin, 1);

    fPaEn = 1;
}

_attribute_ram_code_ void rf_paShutDown(void)
{
    if(fPaEn) {
        gpio_write(rf_pa_txen_pin, 0); // PA TX_DIS
        gpio_write(rf_pa_rxen_pin, 0); // PA RX_DIS
    }
}

/*********************************************************************
 * @fn      rf_setRxBuf
 *
 * @brief   Set RX buffer to dma and enable DMA.
 *
 * @param   pBuf - the allocated rx buffer
 *
 * @return  none
 */
void rf_setRxBuf(u8* pBuf)
{
    rf_rxBuf = pBuf;
    ZB_RADIO_RX_BUF_CLEAR(rf_rxBuf);
    ZB_RADIO_RX_BUF_SET((u16)(u32)(rf_rxBuf));
}


/*********************************************************************
 * @fn      rf_setTrxState
 *
 * @brief   Go to specified state and set related register.
 *
 * @param   state - Specified state
 *
 * @return  none
 */
_attribute_ram_code_ void rf_setTrxState(u8 state)
{
	if(!zigbee_process){
		return;
	}
#ifndef WIN32
    if (RF_STATE_RX == state || RF_STATE_ED == state) {
    	if(TL_ZB_MAC_STATUS_GET() == ZB_MAC_STATE_ACTIVE_SCAN || RF_STATE_ED == state){
    		ZB_RADIO_MODE_AUTO_GAIN();
    	}else{
    		ZB_RADIO_MODE_MAX_GAIN();
    	}

    	ZB_SWTICH_TO_RXMODE();
        rfMode = RF_STATE_RX;
    } else if (RF_STATE_TX == state) {
    	ZB_SWTICH_TO_TXMODE();
        WaitUs(ZB_TX_WAIT_US);
        rfMode = RF_STATE_TX;
    } else {
        /* Close RF */
    	rf_paShutDown();
        ZB_RADIO_TRX_SWITCH(RF_MODE_OFF,LOGICCHANNEL_TO_PHYSICAL(rf_getChannel()));
        rfMode = RF_STATE_OFF;
    }
#endif  /* WIN32 */
}

u8 rf_TrxStateGet(void){
	return rfMode;
}

/*********************************************************************
 * @fn      rf_setChannel
 *
 * @brief   Set specified channel to RF module.
 *
 * @param   ch - 11~26
 *
 * @return  none
 */
void rf_setChannel(u8 ch)
{
	if((ch <TL_ZB_MAC_CHANNEL_START)||(ch >TL_ZB_MAC_CHANNEL_STOP)){
		return;
	}
	g_zbMacPib.phyChannelCur = ch;
	ZB_RADIO_TRX_SWITCH(ZB_RADIO_TRX_STA_GET(),LOGICCHANNEL_TO_PHYSICAL(ch));

}

inline u8 rf_getChannel(void){
	return	g_zbMacPib.phyChannelCur;
}


/*********************************************************************
 * @fn      rf_setTxPower
 *
 * @brief   Set specified transmitted power.
 *
 * @param   txPower - Specified power
 *
 * @return  none
 */
void rf_setTxPower(u8 power)
{
	if(fPaEn){
		ZB_RADIO_TX_POWER_SET(ZB_RADIO_TX_0DBM);
	}else{
		ZB_RADIO_TX_POWER_SET((RF_PowerTypeDef )power);
	}
}


/*********************************************************************
 * @fn      rf_getLqi
 *
 * @brief   Get calculated Link Quality value
 *
 * @param   none
 *
 * @return  lqi result
 */
u8 rf_getLqi(u8 inRssi)
{
	rf_rxGainMode_t mode = RF_GAIN_MODE_MANU_MAX;
	if(TL_ZB_MAC_STATUS_GET() == ZB_MAC_STATE_ACTIVE_SCAN){
		mode = RF_GAIN_MODE_AUTO;
    }

	return ZB_RADIO_RSSI_TO_LQI(mode, inRssi);
}

void rf_edDetect(void)
{
    s8 rssi;
    rssi = ZB_RADIO_RSSI_GET();

    //soft_rssi = rssi;//(rssi + soft_rssi)/2;
    sum_rssi += rssi;
    if(++cnt_rssi >= 0xfffffe){
    	sum_rssi = sum_rssi/cnt_rssi;
    	cnt_rssi = 1;
    }
}

/*********************************************************************
 * @fn      rf_startED
 *
 * @brief   Start ED detect
 *
 * @param   none
 *
 * @return  none
 */
void rf_startED(void)
{
#ifndef WIN32
    soft_rssi = -110;
    sum_rssi = 0;
    cnt_rssi = 0;
    ev_enable_poll(EV_POLL_ED_DETECT);
    /* Change to ED mode */
    rf_setTrxState(RF_STATE_ED);
#endif
}


/*********************************************************************
 * @fn      rf_stopED
 *
 * @brief   Stop Energy Detect
 *
 * @param   none
 *
 * @return  ED result
 */
u8 rf_stopED(void)
{
#ifndef WIN32
    u8 ed;
    u32 temp;

    if(cnt_rssi == 0) cnt_rssi = 1;
    soft_rssi = sum_rssi/cnt_rssi;

    ev_disable_poll(EV_POLL_ED_DETECT);/*WISE_FIX_ME*/
    /* Transfer the RSSI value to ED value */
    if (soft_rssi <= -106) {
        ed = 0;
    } else if (soft_rssi >= -6) {
        ed = 0xff;
    } else{
    	temp = (soft_rssi + 106) * 255;
        ed = temp/100;
    }

    return ed;

#else
    return 0;
#endif
}


_attribute_ram_code_ u8 rf_performCCA(void){
	u32 t1 = clock_time();
	s8 rssi_peak = -110;
	s8 rssi_cur = -110;
	s32 rssiSum = 0;
	s32 cnt = 1;
	u8 ret = PHY_CCA_IDLE;

	if (zigbee_process == 0){
		ret = PHY_CCA_BUSY;
		return ret;
	}

	if(rf_TrxStateGet() != RF_STATE_RX){
		rf_setTrxState(RF_STATE_RX);
		WaitUs(100);
	}

	rssi_cur = ZB_RADIO_RSSI_GET();
	rssiSum += rssi_cur;
	
	while(!clock_time_exceed(t1,128)){
		rssi_cur = ZB_RADIO_RSSI_GET();
		rssiSum += rssi_cur;
		cnt++;
	}
	rssi_peak = rssiSum/cnt;

	DBG_ZIGBEE_STATUS(0x03);


	if (rssi_peak > -50 || (rf_busyFlag & TX_BUSY)) {//Return if currently in TX state
		ret = PHY_CCA_BUSY;
	} else {
		ret = PHY_CCA_IDLE;
	}
	//irq_restore(irq);

	DBG_ZIGBEE_STATUS(0x04);

	return ret;
}

/*********************************************************************
 * @fn      rf_set
 *
 * @brief   Set rf parameter according to specified RF ID
 *
 * @param   id     - The specified RF ID
 *          pValue - The detailed rf parameter
 *          len    - The length will be set to
 *
 * @return  none
 */
void rf_set(u8 id, u8 *pValue, u8 len){
    switch (id) {
		case RF_ID_CHANNEL:
			rf_setChannel(*pValue);
			break;

		case RF_ID_TX_POWER:
			rf_setTxPower(*pValue);
			break;

		case RF_ID_RX_ONOFF:
			if (*pValue) {
				rf_setTrxState(RF_STATE_RX);
			} else {
				rf_setTrxState(RF_STATE_TX);
			}
			break;

		default:
			break;
    }
}

rx_buf_t pRxEvt;

volatile u8 rf_busyFlag = 0;

void rf_802154_init(void);
void rf_802154_reset(void);

const rf_specificFunc_t rf_802154_funcs =
{
	rf_802154_init,
	rf_802154_reset,
};

/**********************************************************************
 * LOCAL FUNCTIONS
 */
_CODE_MAC_ void rf_802154_reset(void){
    /* Reset ack buf */
    memset(rf_ack_buf, 0, 12);
    rf_ack_buf[0] = 4;
    rf_ack_buf[4] = 5;
    rf_ack_buf[5] = 0x02;
    rf_ack_buf[6] = 0x00;
}

_CODE_MAC_ void rf_802154_init(void)
{
    rf_802154_reset();
}


/*********************************************************************
 * @fn      rf_rx_irq_handler
 *
 * @brief   RX irq handler
 *
 * @param   none
 *
 * @return  none
 */
_attribute_ram_code_ __attribute__((optimize("-Os"))) void rf_rx_irq_handler(void)
{
    u8 *p = rf_rxBuf;
    u8 fAck = 0;
    u8 fDrop = 0;
    s32 txTime = 0;
    s32 txDelayUs = 0;

	if(RF_DMA_BUSY()){
		return;
	}

    /* disable RF rx dma */
    ZB_RADIO_RX_DISABLE;
	/* Set 1 to clear the interrupt flag */
    ZB_RADIO_RX_DONE_CLR;

    DBG_ZIGBEE_STATUS(0x10);

    if(rf_busyFlag == 0x06){
    	ZB_EXCEPTION_POST(SYS_EXCEPTTION_ZB_MAC_TX_IRQ);
    }

    g_sysDiags.macRxIrqCnt++;
#if 0
    static s32 rfRxCnt = 0;
    s8 rssi = ZB_RADION_PKT_RSSI_GET(p) - 110;
    T_rxRssiBuf[rfRxCnt++ & 0x3f] = rssi;

    T_rxRssiPass[0]++;
    if(rssi < RSSI_PASS_THRESHOLD){
    	ZB_RADIO_RX_BUF_CLEAR(rf_rxBuf);
		ZB_RADIO_RX_ENABLE;
		return;
    }
    T_rxRssiPass[1]++;
#endif

    /*************************************************************
     * rf_busyFlag & TX_BUSY, means a packet received before during csma delay call back interrupt called,
     * should drop this packet
     */
    if( (!ZB_RADIO_CRC_OK(p)) || (!ZB_RADIO_PACKET_LENGTH_OK(p)) || (rf_busyFlag&TX_BUSY) ){
    	/* diagnostics MAC layer receive error packet */
    	g_sysDiags.macRxCrcFail++;

    	ZB_RADIO_RX_BUF_CLEAR(rf_rxBuf);
        ZB_RADIO_RX_ENABLE;
        return;
    }

    /* Parse necessary field to be used later */
    u8 len = (u8)ZB_RADIO_ACTUAL_PAYLOAD_LEN(p);
    u8 *macPld = p + ZB_RADIO_RX_HDR_LEN;


    /*----------------------------------------------------------
     *  Do the filter
     */
    u8 *pSrcAddr = zb_macDataFilter(macPld, len, &fDrop, &fAck);
    if (fDrop) {
		/* Drop the packet and recover the DMA */
    	ZB_RADIO_RX_BUF_CLEAR(rf_rxBuf);
		ZB_RADIO_RX_ENABLE;
		return;
    }

    /* switch to tx in advance to let the pll stable */
 	if (macPld[0] & MAC_FCF_ACK_REQ_BIT){
 		ZB_SWTICH_TO_TXMODE();
 		txTime = clock_time();
 	}

	/* if don't have enough buffer, use current rxBuf, and drop it */
	u8 *rxNextBuf = tl_getRxBuf();
	if(!rxNextBuf){
		if (macPld[0] & MAC_FCF_ACK_REQ_BIT){
			ZB_SWTICH_TO_RXMODE();
		}

    	/* diagnostics PHY to MAC queue limit */
    	g_sysDiags.phytoMACqueuelimitreached++;

		ZB_RADIO_RX_BUF_CLEAR(rf_rxBuf);
		ZB_RADIO_RX_ENABLE;
		return;
	}


	/* Use the backup buffer to receive next packet */
	rf_rxBuf = rxNextBuf;
	ZB_RADIO_RX_BUF_CLEAR(rf_rxBuf);
	ZB_RADIO_RX_BUF_SET((u16)(u32)(rf_rxBuf));

    /*----------------------------------------------------------
	 *  Send ACK
	 */
	if (macPld[0] & MAC_FCF_ACK_REQ_BIT) {

		rf_ack_buf[ZB_RADIO_TX_HDR_LEN+2] = macPld[2];  //seqnno
		rf_ack_buf[ZB_RADIO_TX_HDR_LEN] = 0x02;

#if defined ZB_COORDINATOR_ROLE || defined ZB_ROUTER_ROLE
		u16 fcf = (macPld[1] << 8) | macPld[0];     // frame control
		u8  pldOft = tl_zbMacHdrSize(fcf);
		u8  frameType = (fcf & MAC_FCF_FRAME_TYPE_MASK) >> MAC_FCF_FRAME_TYPE_POS;

		bool cmdDataReq = ((frameType == MAC_FRAME_COMMAND) && (macPld[pldOft] == MAC_CMD_DATA_REQUEST));

		/* if has pending data, set the pending bit as 1 */
		u8 srcAddrMode = ZB_ADDR_NO_ADDR;
		if ((macPld[1] & MAC_FCF_SRC_ADDR_BIT) == 0x80){
			srcAddrMode = ZB_ADDR_16BIT_DEV_OR_BROADCAST;
		}else if ((macPld[1] & MAC_FCF_SRC_ADDR_BIT) == 0xc0) {
			srcAddrMode = ZB_ADDR_64BIT_DEV;
		}
		if(cmdDataReq && srcAddrMode){
			extern u8 tl_zbMacPendingDataCheck(u8 addrMode, u8 *addr, u8 send);
			if(MAC_SUCCESS == tl_zbMacPendingDataCheck(srcAddrMode, pSrcAddr, 1)){
				rf_ack_buf[ZB_RADIO_TX_HDR_LEN] |= (1 << 4);// (rf_framePending<<4 );
			}
		}
#else
		(void)pSrcAddr;
#endif

		DBG_ZIGBEE_STATUS(0x11);

		txDelayUs = (clock_time() - txTime) / CLOCK_SYS_CLOCK_1US;
		if(txDelayUs < ZB_TX_WAIT_US){
			WaitUs(ZB_TX_WAIT_US - txDelayUs);
		}

		DBG_ZIGBEE_STATUS(0x12);  //for debug

		/* wait until tx done,
		 *  disable tx irq here, rfMode still is RF_STATE_RX;
		 *  */
		/* clear rf interrupt mask bit*/
		ZB_RADIO_IRQ_MASK_CLR;
		/* start to send ack */
		ZB_RADIO_TX_START(rf_ack_buf);//Manual Mode

		/* wait until tx done */
		u32 cur_tick = clock_time();
		while(!ZB_RADIO_TX_DONE && !clock_time_exceed(cur_tick, 1000));

		/* clear the rx/tx done status */
		ZB_RADIO_TX_DONE_CLR;
		ZB_RADIO_RX_DONE_CLR;

		/* rf is set to rx mode again */
		ZB_SWTICH_TO_RXMODE();

		/* set interrupt mask bit again */
		ZB_RADIO_IRQ_MASK_SET;

		DBG_ZIGBEE_STATUS(0x13);   //for debug
	}

	/* enable rf rx dma again */
	ZB_RADIO_RX_ENABLE;

	/* zb_mac_receive_data handler */
	zb_macDataRecvHander(p, macPld, len, fAck, ZB_RADIO_TIMESTAMP_GET(p), RF_ZIGBEE_PACKET_RSSI_GET(p));

	DBG_ZIGBEE_STATUS(0x14);
}


/*********************************************************************
 * @fn      rf_tx_irq_handler
 *
 * @brief   Tx Interrupt handler for RF module.
 *
 * @param   none
 *
 * @return  none
 */
_attribute_ram_code_ __attribute__((optimize("-Os"))) void rf_tx_irq_handler(void){

	ZB_RADIO_TX_DONE_CLR;
	rf_busyFlag &= ~TX_BUSY;//Clear TX busy flag after receive TX done signal

	DBG_ZIGBEE_STATUS(0x15);
    g_sysDiags.macTxIrqCnt++;

    if(zigbee_process){
    	extern u8 rfMode;
    	ZB_SWTICH_TO_RXMODE();
    	rfMode = RF_STATE_RX;
    	DBG_ZIGBEE_STATUS(0x16);
    }

	zb_macDataSendHander();
}

inline bool zb_rfSwitchAllow(void){
	return (rf_busyFlag !=  (TX_BUSY | TX_ACKPACKET));
}

inline bool zb_rfTxDoing(void){
	return (rf_busyFlag & TX_BUSY);
}

