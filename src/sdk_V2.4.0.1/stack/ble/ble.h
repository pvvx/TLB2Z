/********************************************************************************************************
 * @file     ble.h 
 *
 * @brief    This is the header file for ble
 *
 * @author  Zigbee Group
 * @date    2021
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *          All rights reserved.
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

#ifndef BLE_H__
#define BLE_H__

#include "ble_8258/ble.h"
#define BLE_BLT_STATE_GET()    blt_state


void ble_rf_pa_init(bool ioset_need, u32 TXEN_pin, u32 RXEN_pin);

#endif /* BLE_H_H_ */
