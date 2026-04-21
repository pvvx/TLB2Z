/******************************************************************************
 * FileName: flash_eep.h
 *  EEP Version 2.0
 *  Author: pvvx
 ******************************************************************************/
#ifndef __FLASH_EEP_H_
#define __FLASH_EEP_H_

#ifdef __cplusplus
extern "C" {
#endif

//-EEPROM IDs------------------------------------------------------------------
#define EEP_ID_VER (0) // EEP ID blk: unsigned int = minimum supported version
#define EEP_ID_ONOFF_TYPE 0x01 // EEP ID OnOff BLE Type
#define EEP_ID_ONOFF(a) (0x10+a) // EEP ID OnOff
#define EEP_ID_BKEY(a) (0x20+a) // EEP ID bindkey
#define EEP_ID_DMAC(a) (0x30+a) // EEP ID devs mac
//--Option---------------------------------------------------------------------
#define USE_EEP_BANKS	1  // =1 - использовать два банка, = 0 - один банк
#define MAX_FOBJ_SIZE	64 // максимальный размер сохраняемых объeктов (32..512)
//--Config---------------------------------------------------------------------
//#define FLASH_SIZE				(512*1024)
#define FLASH_BASE_ADDR			0x00000000
#define FLASH_SECTOR_SIZE		4096
#define FMEMORY_EEP_BANKS_SHL 	1 // 1<<FMEMORY_EEP_BANKS_SHL = кол-во секторов для работы 2,4,8,..
#define FMEMORY_EEP_BANKS_SIZE	(FLASH_SECTOR_SIZE << FMEMORY_EEP_BANKS_SHL) // размер FMEMORY
#define FMEMORY_EEP_BASE_ADDR1	(NV_BASE_ADDRESS - (FMEMORY_EEP_BANKS_SIZE)) // 0x32000
#if USE_EEP_BANKS
#define FMEMORY_EEP_BASE_ADDR2	(FMEMORY_EEP_BASE_ADDR1 + 0x40000) // 0x72000
#endif
//-----------------------------------------------------------------------------
typedef enum {
	FMEM_NOT_FOUND = -1,	//  -1 - не найден
	FMEM_SIZE_ERR  = -2,	//  -2 - задан неверный размер
	FMEM_OVERFLOW  = -3		//  -3 - переполнение банка
} fmemory_errors_t;
//-----------------------------------------------------------------------------
#if USE_EEP_BANKS
s32 flash_read_cfg(void *ptr, unsigned int nv, unsigned int id, size_t maxsize); // возврат: размер объекта последнего сохранения, -1 - не найден, -2 - error
s32 flash_write_cfg(void *ptr, unsigned int nv, unsigned int id, size_t size);
#else
s32 flash_read_cfg(void *ptr, unsigned int id, size_t maxsize); // возврат: размер объекта последнего сохранения, -1 - не найден, -2 - error
s32 flash_write_cfg(void *ptr, unsigned int id, size_t size);
#endif
/* ver = (SW version << 16) | (HW ) version */
bool flash_supported_eep_ver(u32 min_ver, u32 new_ver);
void flash_erase_all_cfg(void);
//-----------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif


#endif /* __FLASH_EEP_H_ */
