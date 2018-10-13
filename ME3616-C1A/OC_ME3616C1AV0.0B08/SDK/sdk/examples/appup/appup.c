/*
** File   : appup.c
**
*i* Copyright (C) 2013-2018 Gosuncn. All rights reserved.
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
** Author : dengyi2@gosuncn.cn
** 注意: 此demo仅适用于OpenCPU-C版本.
** 注释: 此demo用于测试本地下载功能.执行demo后,重启系统即可
**  $Date: 2018/06/15 09:40:36GMT+08:00 $
**
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gsdk_api.h"

#define APPUP_FLAGS_BASE    0x002DB000
#define APPUP_ADDR          0x002DCFF4
#define APPFOTA_ADDR        0x002DBFF4
#define APPUP_FLAGS_SIZE    0x2000

static gsdk_handle_t g_huart;

int __io_puts(char *data, int len)
{
    int ret = 0;
    if (g_huart) {
        ret = gsdk_uart_write(g_huart, (uint8_t *)data, len, 1000);
    }
    return ret;
}

int log_init(void)
{
    uart_config_t uart_config;
    gsdk_status_t status;

    uart_config.baudrate    = UART_BAUDRATE_115200;
    uart_config.parity      = UART_PARITY_NONE;
    uart_config.stop_bit    = UART_STOP_BIT_1;
    uart_config.word_length = UART_WORD_LENGTH_8;

    status = gsdk_uart_open(UART_0, &uart_config, &g_huart);
    if (status != GSDK_SUCCESS) {
        printf("[APPUP_DEMO]: failed to open uart %d\r\n", status);
        return -1;
    }

    printf("[APPUP_DEMO] log init....OK\r\n");

    return 0;
}

static const char appup_flags_data[] = "0x1A2B3C4D";
static const char appfota_flags_data[] = "0x4D3C2B1A";

int main(void)
{
    char buf[sizeof(appup_flags_data)];
    char buf1[sizeof(appfota_flags_data)];
    gsdk_status_t status;
    gsdk_handle_t hflash;

    log_init();

    status = gsdk_flash_open(APPUP_FLAGS_BASE, APPUP_FLAGS_SIZE, &hflash);
    if (status != GSDK_SUCCESS)
        goto _fail;
    status = gsdk_flash_erase(hflash, APPUP_FLAGS_BASE, FLASH_BLOCK_4K);
    if (status != GSDK_SUCCESS)
        goto _fail_close;
    status = gsdk_flash_write(hflash, APPUP_ADDR, (uint8_t *)appup_flags_data, sizeof(appup_flags_data));
    if (status != GSDK_SUCCESS)
        goto _fail_close;
    status = gsdk_flash_read(hflash, APPUP_ADDR, (uint8_t *)buf, sizeof(appup_flags_data));
    printf("appup flags: %s\n", buf);
    if (status != GSDK_SUCCESS)
        goto _fail_close;
    status = gsdk_flash_write(hflash, APPFOTA_ADDR, (uint8_t *)appfota_flags_data, sizeof(appfota_flags_data));
    if (status != GSDK_SUCCESS)
        goto _fail_close;
    status = gsdk_flash_read(hflash, APPFOTA_ADDR, (uint8_t *)buf1, sizeof(appfota_flags_data));
    printf("appfota flags: %s\n", buf1);
    if (status != GSDK_SUCCESS)
        goto _fail_close;

    if (strcmp(appup_flags_data, buf)) {
        goto _fail_close;
    }

    if (strcmp(appfota_flags_data, buf1)) {
        goto _fail_close;
    }
    gsdk_flash_close(hflash);
    printf("[APPUP_DEMO] pass!\n");
    return -1;

_fail_close:
    gsdk_flash_close(hflash);
_fail:
    printf("[APPUP_DEMO] failed!\n");

    return 0;
}
