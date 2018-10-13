/*
** File   : printf.c
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
** Author : zhangxin2@gosuncn.cn
**
**  $Date: 2018/09/04 08:45:36GMT+08:00 $
**
*/

/*
 * This example shows how to redirect the standard output to a serial port.
 */

#include "gsdk_sdk.h"

static gsdk_handle_t g_huart;

int __io_puts(char *data, int len)
{
    int ret = 0;
    if (g_huart) {
        ret = gsdk_uart_write(g_huart, (uint8_t *)data, len, 1000);
    }
    return ret;
}

int main(void)
{
    uart_config_t uart_config;
    gsdk_status_t status;
    
    uart_config.baudrate    = UART_BAUDRATE_115200;
    uart_config.parity      = UART_PARITY_NONE;
    uart_config.stop_bit    = UART_STOP_BIT_1;
    uart_config.word_length = UART_WORD_LENGTH_8;

    status = gsdk_uart_open(UART_0, &uart_config, &g_huart);    //open uart0 to print debug log
    if (status != GSDK_SUCCESS) {
        gsdk_syslog_printf("[PRINTF_DEMO]: failed to open uart %d\n", status);
        return -1;
    }

    printf("Hello, world!\r\n");			//the printf API output debug log by uart0
    printf("GCC %s\r\n", __VERSION__);

    gsdk_uart_close(g_huart);
    g_huart = NULL;
    return 0;
}

