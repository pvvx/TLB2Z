/********************************************************************************************************
 * @file    main.c
 *
 * @brief   This is the source file for main
 *
 *******************************************************************************************************/
#include "tl_common.h"
#include "zb_common.h"
#include "stack/ble/ble_config.h"
#include "stack/ble/ble_common.h"
#include "stack/ble/ble.h"
#include "zb_ble_switch.h"
#include "zcl_include.h"
#include "app.h"
#include "app_ui.h"

#define ID_BOOTABLE 0x544c4e4b

#define OTA1_FADDR 				0x000000
#define OTA2_FADDR 				0x020000
#define BIG_OTA1_FADDR 			0x000000 // Big OTA1
#define BIG_OTA2_FADDR 			0x040000 // Big OTA2
#define ZIGBEE_BOOT_OTA1_FADDR	0x008000 // Tuya Boot
#define ZIGBEE_BOOT_OTA2_FADDR	0x009000 // Tuya Boot
#define ZIGBEE_MAC_FADDR		0x0ff000
#define BIN_SIZE_MAX_OTA		0x036000
#define BLE_MAC_FADDR			0x076000

#define TEST_CONST_FLASH		0x33CC55AA

static const u32 flag_addr_ok = TEST_CONST_FLASH;

extern void user_init(bool isRetention);
extern void user_ble_init(bool isRetention);

#if ZIGBEE_TUYA_OTA
int flash_main(void) {
#else
int main(void) {
#endif
	u8 isRetention = (drv_platform_init() == SYSTEM_DEEP_RETENTION) ? 1 : 0;

	os_init(isRetention);

#if PA_ENABLE
	rf_paInit(PA_TX, PA_RX);
#endif

	user_init(isRetention);
	ble_radio_init();
	user_ble_init(isRetention);

	if(CURRENT_SLOT_GET() == DUALMODE_SLOT_ZIGBEE){
		switch_to_zb_context();
	}

	irq_enable(); // drv_enable_irq();

#if (MODULE_WATCHDOG_ENABLE)
	drv_wd_setInterval(600);
    drv_wd_start();
#endif

    while(1) {
#if (MODULE_WATCHDOG_ENABLE)
		drv_wd_clear();
#endif
    	concurrent_mode_main_loop();
#if PM_ENABLE
		if(loop_cycle) {
			loop_cycle = 0;
		} else {
			app_pm_task();
		}
#endif
	}
	return 0;
}

#if ZIGBEE_TUYA_OTA

/* Reformat low OTA to big OTA, Zigbee BootLoader to Zigbee no BootLoader */

_attribute_ram_code_
void tuya_zigbee_ota(void) {
	u32 id = ID_BOOTABLE;
	u32 size;
	u32 faddrr = ZIGBEE_BOOT_OTA1_FADDR;	// 0x008000
	u32 faddrw = BIG_OTA1_FADDR;	// 0x000000
	u32 buf_blk[64];	// max 256 bytes
	flash_write_status(0, 0);
	// search for start firmware address 0x008000 or 0x020000 ?
	flash_read_page(faddrr, 16, (unsigned char *) &buf_blk);
	if(buf_blk[2] != id) { // 0x008008 != bootable
		faddrr = ZIGBEE_BOOT_OTA2_FADDR;
		flash_read_page(faddrr, 16, (unsigned char *) &buf_blk);
		if(buf_blk[2] != id) { // 0x009008 != bootable
			faddrr = OTA2_FADDR;
			flash_read_page(faddrr, 16, (unsigned char *) &buf_blk);
			if(buf_blk[2] != id) // 0x020008 != bootable
				return;
		}
	}
	// Run time: ~3700 ms
	// faddrr: 0x008000 == bootable || 0x009000 == bootable
	flash_read_page(faddrr, sizeof(buf_blk), (unsigned char *) &buf_blk);
	if(buf_blk[2] == id && buf_blk[6] > FLASH_SECTOR_SIZE && buf_blk[6] < BIN_SIZE_MAX_OTA) {
		buf_blk[2] &= 0xffffffff; // clear id "bootable"
		size = buf_blk[6];
		size += FLASH_SECTOR_SIZE - 1;
		size &= ~(FLASH_SECTOR_SIZE - 1);
		size += faddrw;
		flash_erase_sector(faddrw); // 45 ms, 4 mA
		flash_write_page(faddrw, sizeof(buf_blk), (unsigned char *) &buf_blk);
		faddrr += sizeof(buf_blk);
		// size += faddrw;
		faddrw += sizeof(buf_blk);
		while(faddrw < size) {
			if((faddrw & (FLASH_SECTOR_SIZE - 1)) == 0)
				flash_erase_sector(faddrw); // 45 ms, 4 mA
				// rd-wr 4kB - 20 ms, 4 mA
				flash_read_page(faddrr, sizeof(buf_blk), (unsigned char *) &buf_blk);
			faddrr += sizeof(buf_blk);
			flash_write_page(faddrw, sizeof(buf_blk), (unsigned char *) &buf_blk);
			faddrw += sizeof(buf_blk);
		}
		// set id "bootable" to new segment
		flash_write_page(BIG_OTA1_FADDR+8, sizeof(id), (unsigned char *) &id);
		while(1)
			reg_pwdn_ctrl = BIT(5);
	}
}


_attribute_ram_code_
int main(void) {
    // Проверка на старт из Tuya boot_loder (старт с 0x20000 тут не проверяется)
	if(flag_addr_ok != TEST_CONST_FLASH
//		|| (*(u32 *)(ZIGBEE_BOOT_OTA1_FADDR + 8) == ID_BOOTABLE)
//	    || (*(u32 *)(ZIGBEE_BOOT_OTA2_FADDR + 8) == ID_BOOTABLE)
		) {
		// clock_init(SYS_CLK_24M_Crystal);
		tuya_zigbee_ota();
	}
	return flash_main();
}

#endif // ZIGBEE_TUYA_OTA

