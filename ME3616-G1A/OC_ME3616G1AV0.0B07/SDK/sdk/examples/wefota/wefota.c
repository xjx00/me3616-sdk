/*
** File   : wefota.c
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
** Author : liyinhai@gosuncn.cn
**
**  $Date: 2018/05/26 09:48:55GMT+08:00 $
**
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gsdk_api.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "gsdk_ril.h"
#include "lwip/sockets.h"
#include "lwip/ip.h"

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
        gsdk_syslog_printf("[WEFOTA_DEMO]: failed to open uart %d\n", status);
        return -1;
    }

    printf("[WEFOTA_DEMO] log init....OK\r\n");

    return 0;
}

int main(void)
{
    gsdk_status_t     status;

    log_init();

	vTaskDelay(3 * 1000 / portTICK_RATE_MS);

    printf("enter wefota.c\n");
    status = gsdk_ril_init();     //init gsdk_ril for at command

    if (status != GSDK_SUCCESS) {
        printf("[OEM_DEMO] AT CMD init failed\n");
    }

    status =  gsdk_ril_fota_settv("ME3616C1AV1.0B03");  //set target version command(at+fotatv=ME3616C1AV1.0B03)

    status = gsdk_ril_fota_ctr();  //start wefota (at+fotactr)
    printf("exit wefota.c\n");
    return 0;
}
