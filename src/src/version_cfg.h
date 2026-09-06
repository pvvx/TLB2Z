/********************************************************************************************************
 * @file    version_cfg.h
 *
 * @brief   This is the header file for version_cfg
 *
 * @author  Zigbee Group
 * @date    2021
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *			All rights reserved.
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
#ifndef _VERSION_CFG_H_
#define _VERSION_CFG_H_

#define BOOT_LOADER_MODE					0

/* Boot loader address. */
#define BOOT_LOADER_IMAGE_ADDR				0x0

/* APP image address. */
#if (BOOT_LOADER_MODE)
	#define APP_IMAGE_ADDR					0x8000
#else
	#define APP_IMAGE_ADDR					0x0
#endif

/* Board ID */
#define BOARD_DEBUG					1  // DIY TB-03F-Kit debug
#define BOARD_ZB_TS0001				2  // https://pvvx.github.io/Zbeacon-TS0001/
#define BOARD_TB03F_KIT				16 // DIY TB-03F-Kit
#define BOARD_TS0001_TZ3000_RBZ		43 // https://pvvx.github.io/TS0001_TZ3000/
#define BOARD_ZG807Z				44 // https://pvvx.github.io/ZG-807Z/

/* Board define */
#ifndef BOARD
#define BOARD					BOARD_DEBUG
#endif

/* Chip IDs */
#define TLSR_8267				0x00
#define TLSR_8269				0x01
#define TLSR_8258_512K			0x02
#define TLSR_8258_1M			0x03
#define TLSR_8278				0x04
#define TLSR_B91				0x05

#if defined(MCU_CORE_826x)
	#if (CHIP_8269)
		#define CHIP_TYPE		TLSR_8269
	#else
		#define CHIP_TYPE		TLSR_8267
	#endif
#elif defined(MCU_CORE_8258)
		#define CHIP_TYPE		TLSR_8258_512K	//TLSR_8258_1M
#elif defined(MCU_CORE_8278)
		#define CHIP_TYPE		TLSR_8278
#elif defined(MCU_CORE_B91)
		#define CHIP_TYPE		TLSR_B91
#endif

#define APP_RELEASE				0x01	//BCD app release "0.2"
#define APP_BUILD				0x16	//BCD app build "0.3"
#define STACK_RELEASE			0x30	//BCD stack release 3.0
#define STACK_BUILD				0x01	//BCD stack build 01

/*********************************************************************************************
 * During OTA upgrade, the upgraded device will check the rules of the following three fields.
 * Refer to ZCL OTA specification for details.
 */
#define MANUFACTURER_CODE_TELINK           	0x1141	// Telink ID
#define	IMAGE_TYPE							(0x2200 | BOARD)
#define	FILE_VERSION					  	((APP_RELEASE << 24) | (APP_BUILD << 16) | (STACK_RELEASE << 8) | STACK_BUILD)

/* Pre-compiled link configuration. */
#define IS_BOOT_LOADER_IMAGE				0
#define RESV_FOR_APP_RAM_CODE_SIZE			0
#define IMAGE_OFFSET						APP_IMAGE_ADDR

#define	DEEPRETENTION_SECTION_USED          0  // (if BLE)


#endif // _VERSION_CFG_H_

