/********************************************************************************************************
 * @file     nv.h
 *
 * @brief	 nv flash interface function header file.
 * 			 If undefined FLASH_SIZE_1M or defined FLASH_SIZE_1M ZERO, using 512k flash configuration.
 *
 * @author
 * @date     Oct. 8, 2016
 *
 * @par      Copyright (c) 2016, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *
 *           The information contained herein is confidential property of Telink
 *           Semiconductor (Shanghai) Co., Ltd. and is available under the terms
 *           of Commercial License Agreement between Telink Semiconductor (Shanghai)
 *           Co., Ltd. and the licensee or the terms described here-in. This heading
 *           MUST NOT be removed from this file.
 *
 *           Licensees are granted free, non-transferable use of the information in this
 *           file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided.
 *
 *******************************************************************************************************/
#pragma once

#if BOOT_LOAD_MODE
	#if BOOT_LOAD_MODE_NORMAL_FLASH_MAPPING
		#include "nv_normal_for_bootload_mode.h"
	#else
		#include "nv_only_for_bootload_mode.h"
	#endif
#else
	#include "nv_normal.h"
#endif
