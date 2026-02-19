## V2.4.0.1

### Version
* Chips
  - TC32: TLSR8258
  - RISCV: TLSR921x(B91), TL321x, TL721x
* Driver Version
  - tl_platform_sdk V3.2.0(TL321x, TL721x)
* BLE Version
  - tl_ble_sdk V4.0.4.2(TL321x, TL721x)
* Zigbee Version
  - telink_zigbee_sdk V3.7.1.2
* Toolchain Version
  - TLSR8258: TC32 ELF GCC4.3
  - TLSR921x(B91): TL32 ELF MCULIB V5F GCC7.4
  - TL321x: TL32 ELF MCULIB V5 GCC12.2
  - TL721x: TL32 ELF MCULIB V5F GCC12.2
* IDE Version
  - Telink IDE: [Telink IDE](https://wiki.telink-semi.cn/tools_and_sdk/Tools/IDE/Telink_IDE.zip)
  - Telink V323 RDS: [telink_v323_rds](https://wiki.telink-semi.cn/tools_and_sdk/Tools/IDE/telink_v323_rds_official_windows.zip)
  - Telink IoT Studio: [TelinkIoTStudio_V2024.8](http://wiki.telink-semi.cn/tools_and_sdk/Tools/IoTStudio/TelinkIoTStudio_V2024.8.zip)
### Features
* Update sampleLight demo for level and color cluster.
### Bug Fixes
* Decorate structures that require single byte alignment with "_attributie_packed_" in BLE multiple connection stack.
* Fix the DeviceEnabled attribute handling of ZCL Basic Cluster.
* Update libraries to fix the issue of searching for end devices when the broadcast method is 0xFFFF.
* Update libraries to fix the length issue of APS encrypted data.
  - The affected SDK versions are from v2.3.2.0 to v2.4.0.0.
  - In addition, users also can fix this issue by using the tl_apsDataIndRegister() function as follows. This patch is only available for SDK versions from v2.3.2.0 to v2.4.0.0.
	~~~
	void app_apsDataIndLengthFix(void *arg){
		aps_data_ind_t *ind = (aps_data_ind_t *)arg;
		if(ind->security_status & SECURITY_IN_APSLAYER){
			ind->asduLength += 4;
		}
	}

	tl_apsDataIndRegister(app_apsDataIndLengthFix);
	~~~
### Performance
* None
### BREAKING CHANGES
* None
### CodeSize
* Units: KBytes(RAM/FLASH Size)

|   chip   | concurrent_sampleGW | concurrent_sampleLight | concurrent_sampleSwitch | concurrent_sampleLightAdv | concurrent_sampleSwitchAdv |
| :------- | :-----------------: | :--------------------: | :---------------------: | :-----------------------: | :------------------------: |
| TLSR8258 | 44 / 211            | 41 / 215               | 32 / 171                |  \				     	| \	                         |
| TLSR921x | 48 / 192            | 46 / 198               | 38 / 159                |  \						| \                          |
| TL321x   | 76 / 229            | 74 / 236               | 69 / 199                | 46 / 188	                | 47 / 151                   |
| TL721x   | 77 / 230            | 74 / 236               | 70 / 200                | 47 / 189                  | 48 / 152                   |

###版本
* 芯片
  - TC32: TLSR8258
  - RISCV: TLSR921x(B91), TL321x, TL721x
* 驱动版本
  - tl_platform_sdk V3.2.0(TL321x, TL721x)
* BLE版本
  - tl_ble_sdk V4.0.4.2(TL321x, TL721x)
* Zigbee版本
  - telink_zigbee_sdk V3.7.1.2
* 工具链版本
  - TLSR8258: TC32 ELF GCC4.3
  - TLSR921x(B91): TL32 ELF MCULIB V5F GCC7.4
  - TL321x: TL32 ELF MCULIB V5 GCC12.2
  - TL721x: TL32 ELF MCULIB V5F GCC12.2
* IDE版本
  - Telink IDE: [Telink IDE](https://wiki.telink-semi.cn/tools_and_sdk/Tools/IDE/Telink_IDE.zip)
  - Telink V323 RDS: [telink_v323_rds](https://wiki.telink-semi.cn/tools_and_sdk/Tools/IDE/telink_v323_rds_official_windows.zip)
  - Telink IoT Studio: [TelinkIoTStudio_V2024.8](http://wiki.telink-semi.cn/tools_and_sdk/Tools/IoTStudio/TelinkIoTStudio_V2024.8.zip)
### Features
* 更新sampleLight示例代码，主要针对灯的level和color
### Bug Fixes
* 修复BLE多连接协议栈中，对需要单字节对齐的结构体加_attributie_packed_修饰
* 修复ZCL Basic cluster里关于DeviceEnabled属性的处理
* 更新库修复在广播地址为0xFFFF时转发给终端设备时搜索终端设备地址的问题
* 更新库修复APS层加密数据长度问题
  - 受影响的SDK版本为v2.3.2.0 to v2.4.0.0.
  - 如果用户用到APS层加解密，可以使用tl_apsDataIndRegister()来修复此问题，参考如下。 该补丁仅限于版本为v2.3.2.0 to v2.4.0.0的SDK。
	~~~
	void app_apsDataIndLengthFix(void *arg){
		aps_data_ind_t *ind = (aps_data_ind_t *)arg;
		if(ind->security_status & SECURITY_IN_APSLAYER){
			ind->asduLength += 4;
		}
	}

	tl_apsDataIndRegister(app_apsDataIndLengthFix);
	~~~
### Performance
* None
### BREAKING CHANGES
* None

## V2.4.0.0

### Version
* Chips
  - TC32: TLSR8258
  - RISCV: TLSR921x(B91), TL321x, TL721x
* Driver Version
  - tl_platform_sdk V3.2.0(TL321x, TL721x)
* BLE Version
  - tl_ble_sdk V4.0.4.2(TL321x, TL721x)
* Zigbee Version
  - telink_zigbee_sdk V3.7.1.0
* Toolchain Version
  - TLSR8258: TC32 ELF GCC4.3
  - TLSR921x(B91): TL32 ELF MCULIB V5F GCC7.4
  - TL321x: TL32 ELF MCULIB V5 GCC12.2
  - TL721x: TL32 ELF MCULIB V5F GCC12.2
* IDE Version
  - Telink IDE: [Telink IDE](https://wiki.telink-semi.cn/tools_and_sdk/Tools/IDE/Telink_IDE.zip)
  - Telink V323 RDS: [telink_v323_rds](https://wiki.telink-semi.cn/tools_and_sdk/Tools/IDE/telink_v323_rds_official_windows.zip)
  - Telink IoT Studio: [TelinkIoTStudio_V2024.8](http://wiki.telink-semi.cn/tools_and_sdk/Tools/IoTStudio/TelinkIoTStudio_V2024.8.zip)
### Features
* Support TL321x platform
* Support TL721x platform
* Support flash protection
* Update TLSR8258 and TLSR921x driver for known issues
* Add new dual-mode demo that only support BLE advertise for TL321x and TL721x
* Remove the unused platform information
### Bug Fixes
* Protect the AES process from interruption
* Fix the issue of CRC check failure during local upgrade through HCI
* Check inter-pan packet length
* Use the correct address range when checking if it is a ev_buffer
### Performance
* Optimized power consumption during ble connection state 
* Double-detect start flag when OTA
### BREAKING CHANGES
* None
### CodeSize
* Units: KBytes(RAM/FLASH Size)

|   chip   | concurrent_sampleGW | concurrent_sampleLight | concurrent_sampleSwitch | concurrent_sampleLightAdv | concurrent_sampleSwitchAdv |
| :------- | :-----------------: | :--------------------: | :---------------------: | :-----------------------: | :------------------------: |
| TLSR8258 | 44 / 210            | 41 / 213               | 32 / 171                |  \				     	| \	                         |
| TLSR921x | 48 / 193            | 45 / 197               | 38 / 159                |  \						| \                          |
| TL321x   | 76 / 229            | 73 / 234               | 69 / 199                | 46 / 186	                | 48 / 151                   |
| TL721x   | 77 / 229            | 74 / 235               | 70 / 200                | 47 / 187                  | 48 / 152                   |

###版本
* 芯片
  - TC32: TLSR8258
  - RISCV: TLSR921x(B91), TL321x, TL721x
* 驱动版本
  - tl_platform_sdk V3.2.0(TL321x, TL721x)
* BLE版本
  - tl_ble_sdk V4.0.4.2(TL321x, TL721x)
* Zigbee版本
  - telink_zigbee_sdk V3.7.1.0
* 工具链版本
  - TLSR8258: TC32 ELF GCC4.3
  - TLSR921x(B91): TL32 ELF MCULIB V5F GCC7.4
  - TL321x: TL32 ELF MCULIB V5 GCC12.2
  - TL721x: TL32 ELF MCULIB V5F GCC12.2
* IDE版本
  - Telink IDE: [Telink IDE](https://wiki.telink-semi.cn/tools_and_sdk/Tools/IDE/Telink_IDE.zip)
  - Telink V323 RDS: [telink_v323_rds](https://wiki.telink-semi.cn/tools_and_sdk/Tools/IDE/telink_v323_rds_official_windows.zip)
  - Telink IoT Studio: [TelinkIoTStudio_V2024.8](http://wiki.telink-semi.cn/tools_and_sdk/Tools/IoTStudio/TelinkIoTStudio_V2024.8.zip)
### Features
* 支持TL321x 平台. 
* 支持TL721x 平台.
* 增加flash操作保护.
* 更新TLSR8258和TLSR921x驱动，针对发现的已知问题.
* 增加新的只支持BLE广播模式是双模示例，针对TL321x和TL721x平台.
* 移除用不到的平台信息
### Bug Fixes
* 保护AES处理流程不被中断打断
* 在通过HCI更新本地固件时，从正确flash地址获取固件CRC
* 检查inter-pan数据包长度
* 更正地址检查范围当检查地址是否是ev_buffer时
### Performance
* 优化BLE连接状态下功耗.
* OTA时双重检查固件有效标志.
### BREAKING CHANGES
* None

## V2.3.2.1
### Bugs
* buffer overflown issue under SCAN_IN_ADV_STATE mode 

### Features
* Optimized power consumption during ble connection state 

BREAKING CHANGES
* None

### Bugs
* SCAN_IN_ADV_STATE 模式下buffer溢出问题

### Features
* 优化BLE连接状态下功耗

### BREAKING CHANGES
* 无

## V2.3.2.0
### Bugs
* sync to Zigbee SDK v3.6.8.6

### Features
* scanning is supported during BLE ADV state
* add sample codes for external rf PA
* fix some misspellings

BREAKING CHANGES
* None

### Bugs
* 与Zigbee SDK v3.6.8.6同步

### Features
* BLE ADV状态下支持scanning功能
* 添加支持外部RF PA用例代码
* 纠正一些拼写错误

### BREAKING CHANGES
* 无

## V2.3.1.0
### Bugs
* rf settings should be recoveried after calling bls_ll_setAdvEnable() to avoid missing rf interrupt
* fix buffer free issue for the case incoming count of the aps encrypted data doesn't match

### Features
* Normalizes the naming of content related to private schemas
* BLE DLE is supported
* binding table compatibility. the api "app_userBindingTblRestore" can be used to restore the binding table if the aps_binding_table_t is modified by user for the older SDK earlier than v2.2.0.0 .
* add sample codes to support report for ZED device

BREAKING CHANGES
* None

### Bugs
* 调用bls_ll_setAdvEnable()之后需要恢复rf设置避免rf中断丢失
* 修正aps加密数据的count不匹配时buffer未释放的问题

### Features
* 规范一些私有条目的命名
* 绑定表兼容性问题，调用“app_userBindingTblRestore”用以恢复v2.2.0.0之前的SDK中的aps_binding_table_t结构体被修改的绑定表信息
* ZED中添加支持report用例
### BREAKING CHANGES
* 无

## V2.3.0.0
### Bugs
* need to add validation field and read back while writing operation to avoid the wrong data and firmware   being overwritten under extreme condition (drv_nv.c, drv_flash.c, ota.c)    
* the voltage detection must be used for the zbit flash to avoid firmware being overwritten(adc.c flash.c)
### Features
* discard the current packet while the rf is busy to avoid receiving the wrong data

BREAKING CHANGES
* None

### Bugs
* 写flash时添加校验标志以及回读操作，读时数据进行有效性校验，以免获取错误数据以及避免极端条件下固件被修改的风险(drv_nv.c, drv_flash.c, ota.c)
* 当采用zbit flash时，必须进行低压检测，避免固件被修改的风险(flash.c, adc.c)
### Features
* 当rf处于busy状态时，丢弃当前数据帧，以免获取错误数据.
### BREAKING CHANGES
* 无

## V2.2.0.0
### Bugs
* group(with multi-ep) can't removed when receiving remove group command with group-casting
* fix the issue that the tx interrupt maybe miss when calling the "bls_ll_setAdvEnable" 
* can't remove the child device when receiving Remove Device command  

### Features
* upgrade the group/binding/scene table to support changing the table size
* support gp sink
* add functions for flash write protection
* add voltage detection
* use zigbee stack v3.6.8.1

BREAKING CHANGES
* add a paramter "tcAddr" in the function of the "bdb_join_direct()".

### Bugs
* 收到组播的remove group时，当group table存在多个Ep时，不能正常删除
* 关闭ble广播模式导致zigbee模式下tx中断丢失
* 无法通过Remove Device正常删除子设备
### Features
* 更新group/binding/scene table从而支持动态修改table大小
* 支持gp sink
* 添加flash写保护代码
* 添加电池检测代码
* 同步zigbee stack v3.6.8.1

### BREAKING CHANGES
* bdb_join_direct（）添加了参数"tcAddr"

## V2.1.0.0

### Bugs
* None

###Features
* compatible with 8258 and b91
* use zigbee stack v3.6.7.4

BREAKING CHANGES
* Change the directory structure to adapt to the multi-platform(8258/b91) SDK.

### Bugs
* None

###Features
* sdk兼容8258, b91
* 使用zigbee stack v3.6.7.4

### BREAKING CHANGES
* 更改目录结构以适配多平(8258/b91)SDK.
### CodeSize
* Flash:
  - ZC:    205kb
  - ZR:    205kb
  - ZED:   159kb
* RAM:
  - ZC:   43kb
  - ZR:   40kb
  - ZED:  31kb


## V1.6.2.0

### Bugs
* ZED can't factory new reset successfully while disconnecting with the network. (this issue only exsits on sdk-v1.6.0.0, v1.6.1.0)
* scene copy command issue
* change ota check/validation condition

### BREAKING CHANGES
* None

### Bugs
* ZED设备断连时无法正常factory new reset(这一问题仅存在于sdk-v1.6.0.0, v1.6.1.0)
* scene copy command处理问题
* 加强ota image的校验条件
### BREAKING CHANGES
* 无

## V1.6.1.0

### Features
* add configuration to enable/disable zigbee security 

### BREAKING CHANGES
* None

### Features
* zigbee加密与否由应用层配置

### BREAKING CHANGES
* 无
## V1.6.0.0

### Features
* update drivers is to fix 8258 exceptional interruption caused by system ticker.
* binding table searching issue
* the rssi value is changed from RSSI to (RSSI - 110)
* add callback for nwk layer is to decrease the code size
* improve the performance for the mac pending data(pending data keeps valid till it's be transmitted successfully or expired)
* improve routing performance
* change configuration for performance

### BREAKING CHANGES
* None

### Features
* 更新驱动修正由system ticker引起的异常中断
* binding table搜索问题
* rssi值由RSSI修正为(RSSI-110)
* 网络层添加回调函数以减小代码大小
* 优化mac pending data处理(pending一直保持有效直到成功被发送或超时)
* 优化路由
* 修改网络配置参数，以提高网络性能

### BREAKING CHANGES
* 无

## V1.5.7

### Feautures
* update flash driver(based on the driver sdk "telink_b85m_driver_sdk V1.1.0")
* add voltage calibration for flash 
* optimize the radom generator
* update the inforamtion in NV when the operation channel is changed
* optimize the message processing in ZCL layer(use task schedule directly)

### BREAKING CHANGES
* None

### Feautures
* 更新flash驱动(基于驱动sdk telink_b85m_driver_sdk V1.1.0)
* 添加flash 的电压校准
*  优化随机数产生器的性能
*  工作channel发生改变后，将信息更新到NV
* 优化ZCL层消息处理(直接采用任务调度)
### BREAKING CHANGES
* 无
## V1.5.6

### Bugs
* nwk update command should be ignored when the attribute of the "PendingNetworkUpdatePANID" of the wwah  isn't 0xff
* fix the buffer issue while receiving group-casting packet

### Feautures
* the permitJoin command shouldn't be sent out after network formation
* add battery detection to avoid to operate flash in low voltage.

### BREAKING CHANGES
* None

### Bugs
* 当Wwah 的PendingNetworkUpdatePANID不为0xff时，忽略nwk update命令
* 收到组播数据时，buffer访问出错

### Feautures
* 组网设备成功后，不发送permit join
* 添加电池检测，避免低压下操作flash

### BREAKING CHANGES
* 无.

## V1.5.5

### Bugs
*  the field in the writeAttrResp should only include the info of the attribute which status isn't successful.  
*  should free the buffer when manyToOne routing table is full(just for coordinator).

### Feautures
* improve the mac tx/rx performance
  1. simplify sending the ack packet,
  2. the incomming packets don't be discarded while it's waiting the ack
* do rf rx switch in rf_performCCA() to avoid changing the rf setting again after ble sync interrupt has occured.
* timer event number can be configured by user, and limit the broadcast timer event number(by TIMER_EVENT_SIZE_BRC) to avoid system exception
* update driver for supporting different flash(open source for flash.c)

### BREAKING CHANGES
* None

### Bugs
* writeAttrResp数据中仅包含状态为错误的attr的信息
* manyToOne routing table满，存在buffer未被释放的问题(仅对 coordinator)
### Feautures
* 提高mac tx/rx的性能（简化了ack数据包的发送过程； 等待ack时不再丢弃收到的数据)
* csam-cca时的模式切换放到rf_performCCA() ，避免ble同步中断到来后异常操作rf模块 
* timer事件table大小可由用户配置，另外限制广播timer事件避免系统异常
* 更新驱动用以支持不同型号的flash(flash.c开源)

### BREAKING CHANGES
* 无

## V1.5.4

### Bugs
*  buffer free issue while doing join(data request confirmation occurs after associate confirmation)

### Feautures
* add ble master demo

### BREAKING CHANGES
* None

### Bugs
* 入网过程中buffer释放异常(当data request confirm在associate confirmation之后产生)

### Feautures
* 添加ble master用例

### BREAKING CHANGES
* 无

## V1.5.3

### Bugs
*  system timer interrupt has still run even if ble is idle.
*  call ble_task_restart/ble_task_stop in zigbee mode, the interupt mask will be cleared
*  buffer free issue while doing join

### Feautures
* add ble master functoinality
* add ble scan during ADV 

### BREAKING CHANGES
* None

### Bugs
* ble处于idle时，system timer中断仍然工作
* zigbee模式下调用task_restart/ble_task_stop?导致rf中断mask位被清除
* 入网过程中buffer释放异常

### Feautures
* 添加ble master功能
* ble ADV状态添加scan功能

### BREAKING CHANGES
* ?
## V1.5.2

### Bugs
*  add usb_dp_pullup_en in lib/libdrivers_8258.a, or compile error while enable "ZBHCI_USB_CDC"?now usb cdc works
*  system ram exception will occur if the "ZB_BUF_POOL_NUM" or“APS_BINDING_TABLE_NUM”is modified in sdk
*  nib.ieeeAddress didn't set after touch link，so that the address information is wrong when doing binding
*  binding address table shouldn't be updated after group binding 

###Feautures
* mac retry number controlled by mac-pib.frameRetryNum
* update for following tc policy for ZC
* use "various" instead of "define" for some settings
* add aps_group.c ro open source code

### BREAKING CHANGES
* None

### Bugs
* 在lib/libdrivers_8258.a里，添加“usb_dp_pullup_en“函数，否则打开”ZBHCI_USB_CDC"编译出错，CDC可工?* SDK里修?ZB_BUF_POOL_NUM" or“APS_BINDING_TABLE_NUM”会导致内存异常
* touchlink结束后，nib.ieeeAddress没有正确赋值，导致绑定时地址出现异常?* 组绑定后，绑定地址表不应该更新

###Feautures
* mac重传由mac-pib.frameRetryNum控制
* ZC 遵循tc policy
* 宏改为变量，可有用户配置
* 开?aps_group.c

### BREAKING CHANGES
* ?

## V1.5.1

### Bugs
* Ble/Zigbee use different RF Tx Power(zb: 10dBm, ble: 3dBm),  the Tx Power should be set back while doing mode switch

### BREAKING CHANGES
* None

### Bugs
* Ble/Zigbee使用不同的Rf Tx power(zb: 10dBm, ble: 3dBm); 模式切换时，tx power应该设回相应的?
### BREAKING CHANGES
* ?


## V1.5.0

### Features
* remove bindlist when forget dev
* enable Green Power for Route/Coordinator 
* update the formula for converting the rssi to Lqi
* make the timeout of transport key can be configurable

### Bug Fixes
* fix the bug that the parent neighbor table can't been updated when the parent node is changed after orphan 
  joining is done
* Fix mac ack timeout issue
* Scene table store/restore issue

### BREAKING CHANGES
* None

### Features
* 设备离网，bindList清除
* 对于Route/Coordinator，使能Green Power
* 更新rssi转化到Lqi的折算公?* 入网时tranport key超时时间可由用户层控?
### Bug Fixes
* 修复终端节点orphan join回连时父节点的neighbor table没有有效更新的问题?* mac ack回复慢的问题
* scene table保存、恢复的问题

### BREAKING CHANGES
* ?