/********************************************************************************************************
 * @file     bootload_main.c
 *
 * @brief	 Common main entry
 *
 * @author
 * @date     Dec. 1, 2017
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
#include "user_config.h"
#include "tl_common.h"
#include "voltage_detect.h"

#define FW_RUN_ADDR		0x10000
#define FW_OTA_ADDR		0x40000

extern void bootloader_withOtaCheck(unsigned int addr_load, unsigned int new_image_addr);

/* system clock configuration
 *
 * platform:
 * 		826x
 * 		8258
 * 		HAWK
 *
 *  */
#if(CLOCK_SYS_CLOCK_HZ == 24000000)
	SYS_CLK_TYPEDEF g_sysClk = SYS_CLK_24M_Crystal;
#elif(CLOCK_SYS_CLOCK_HZ == 32000000)
	SYS_CLK_TYPEDEF g_sysClk = SYS_CLK_32M_Crystal;
#elif(CLOCK_SYS_CLOCK_HZ == 16000000)
	SYS_CLK_TYPEDEF g_sysClk = SYS_CLK_16M_Crystal;
#elif(CLOCK_SYS_CLOCK_HZ == 48000000)
	SYS_CLK_TYPEDEF g_sysClk = SYS_CLK_48M_Crystal;
#else
	#error please config system clock
#endif

enum{
	SYSTEM_RETENTION_NONE,
	SYSTEM_RETENTION_EN,
};

/*
 * platform initiation:
 * 		clock_init: 	system clock selection
 * 		ZB_RADIO_INIT: 	RF module
 * 		gpio_init:		gpio configuration
 * 		ZB_TIMER_INIT:  enable a timer for zigbee mac cca-csma
 *
 * */
static u8 platform_init(void){
	u8 ret = SYSTEM_RETENTION_NONE;

#if defined(MCU_CORE_8258)
	extern void bss_section_clear(void);
	extern void data_section_load();
	if( (analog_read(0x7f) & 0x01) ){	  //recovery from deep sleep
		bss_section_clear();
		data_section_load();
	}else{
		ret = SYSTEM_RETENTION_EN;
	}
#endif

	blc_pm_select_internal_32k_crystal();

	cpu_wakeup_init();

	clock_init(/*SYS_CLK_16M_Crystal*/SYS_CLK_32M_Crystal);//BLE 16M

	gpio_init();

	return ret;
}

void irq_handler(void){

}

int main (void) {
	platform_init();

#if VOLTAGE_PROTECT_EN
	voltage_detectInit();
	voltage_protect(1);
#endif

	/* check if the zbit flash is used */
#if defined(MCU_CORE_8258)
	if(flash_is_zb()){
#if (!VOLTAGE_DETECT_ENABLE) || !defined(VOLTAGE_DETECT_ENABLE)
		voltage_detectInit();
		flash_safe_voltage_set(VOLTAGE_SAFETY_THRESHOLD);
#endif
		flash_unlock_mid13325e();  //add it for the flash which sr is expired
	}
#endif

	bootloader_withOtaCheck(FW_RUN_ADDR, FW_OTA_ADDR);

	while (1) {
		WaitMs(500);
	}
}

