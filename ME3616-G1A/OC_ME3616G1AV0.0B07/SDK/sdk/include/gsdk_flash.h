/*
** File   : gsdk_api.h
**
** Copyright (C) 2013-2018 Gosuncn. All rights reserved.
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**      http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** Author : lixingyuan@gosuncn.cn
**
**  $Date: 2018/02/08 08:45:36GMT+08:00 $
**
*/

#ifndef _GSDK_FLASH_H_
#define _GSDK_FLASH_H_

#define FLASH_ADDR_OFFSET    (0x08000000)
#define TO_FALSH_ADDR(x)     ((x) - FLASH_ADDR_OFFSET)

extern uint32_t _oem_fs_base;
extern uint32_t _oem_fs_size;
extern uint32_t _oem_rom_base;
extern uint32_t _oem_rom_size;

#define OEM_FS_BASE         TO_FALSH_ADDR((uint32_t)&_oem_fs_base)  
#define OEM_FS_SIZE         ((uint32_t)&_oem_fs_size)
#define OEM_FS_LIMIT        (OEM_FS_BASE + OEM_FS_SIZE)

#define OEM_ROM_BASE        TO_FALSH_ADDR((uint32_t)&_oem_rom_base)  
#define OEM_ROM_SIZE        ((uint32_t)&_oem_rom_size)

typedef enum {
    FLASH_BLOCK_4K  = 0,          /**< flash erase block size 4k   */
    FLASH_BLOCK_32K = 1,          /**< flash erase block size 32k */
    FLASH_BLOCK_64K = 2,          /**< flash erase block size 64k */
} flash_block_t;

/*****************************************************************************
 * Functions
 *****************************************************************************/
/**
 * @brief     flash open
 * @param[in]  base is starting address of flash
 * @param[in]  size is the size of flash
 * @return
 * #GSDK_SUCCESS on success
 */
gsdk_status_t gsdk_flash_open(uint32_t base, uint32_t size, gsdk_handle_t *phflash);


/**
 * @brief     flash close
 */
void gsdk_flash_close(gsdk_handle_t hflash);


/**
 * @brief     flash erase
 * @param[in]  start_address is starting address to erase from
 * @param[in]  block_type is the size of block to be erased
 * @return
 * #GSDK_SUCCESS on success
 * @note
 *  The start_address should be align with the block_type
 */
gsdk_status_t gsdk_flash_erase(gsdk_handle_t hflash, uint32_t start_address,  flash_block_t block_type);


/**
 * @brief     flash read
 * @param[in]  start_address is starting address to read the data from
 * @param[out]  buffer is place to hold the incoming data
 * @param[in]  length is the length of the data content
 * @return
 * #GSDK_SUCCESS on success
 */
gsdk_status_t gsdk_flash_read(gsdk_handle_t hflash, uint32_t start_address, uint8_t *buffer, uint32_t length);


/**
 * @brief     flash write
 * @param[in]  address is starting address to write from.Before the address can be written to for the first time,
 *            the address located sector or block must first be erased.
 * @param[in]  length is data length
 * @param[in]  data is source data to be written
 * @return
 * #GSDK_SUCCESS on success
 */
gsdk_status_t gsdk_flash_write(gsdk_handle_t hflash, uint32_t address, const uint8_t *data, uint32_t length);

#endif
