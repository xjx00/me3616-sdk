/*
** File   : uart.c
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
** Author : dengyi2@gosuncn.cn
** 注释: 此demo用于测试串口,利用串口工具发字符,回显字符
**  $Date: 2018/07/20 14:48:36GMT+08:00 $
**
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gsdk_api.h"

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
    gsdk_status_t status;
    uart_config_t uart_config;
    static char test[64];
    static char read[64];
    int left, snd_cnt, rcv_cnt, read_cnt, write_cnt, cnt;
    char *pbuf;
    char *pread;

    uart_config.baudrate    = UART_BAUDRATE_115200;
    uart_config.parity      = UART_PARITY_NONE;
    uart_config.stop_bit    = UART_STOP_BIT_1;
    uart_config.word_length = UART_WORD_LENGTH_8;

    status = gsdk_uart_open(UART_0, &uart_config, &g_huart);
    if (status != GSDK_SUCCESS) {
        gsdk_syslog_printf("[UART_DEMO]: failed to open uart %d\n", status);
        return -1;
    }
    printf("\r\n");
    printf("Usage:\r\n");
    printf("use uart tools,send 64 or greater than 64 character to ME3616\r\n");
    printf("that will show what you send, but only show 64 character\r\n");
    printf("\r\n");

    cnt = 64;
    pread = read;
    while(1){
	read_cnt = gsdk_uart_read(g_huart, (uint8_t *)pread, cnt, 100);
	cnt -= read_cnt;
	pread += read_cnt;
	if (cnt < 64)
	break;
    }

    cnt = 64;
    pread = read;
    while(1){
        write_cnt = gsdk_uart_write(g_huart, (uint8_t *)pread, read_cnt, 100);
        cnt -= write_cnt;
        pread += write_cnt;
        if (cnt != 0) {
            printf("\r\n");
            printf("WARRING!!! you should send 64 or greater than 64 character,thanks.\r\n");
            printf("eg:");
            printf("012345678901234567890123456789012345678901234567890123456789ABCD\r\n");
            printf("\r\n");
            break;
	}
	while(1);
    }

    left = 64;
    pbuf = test;
    while(1) {
        rcv_cnt = gsdk_uart_read(g_huart, (uint8_t *)pbuf, left, 100);
        left -= rcv_cnt;
        pbuf += rcv_cnt;
        if (left == 0)
           break;
    }
    left = 64;
    pbuf = test;
    while(1) {
        snd_cnt = gsdk_uart_write(g_huart, (uint8_t *)pbuf, left, 100);
        left -= snd_cnt;
        pbuf += snd_cnt;
        if (left == 0)
           break;

    }
    gsdk_uart_close(g_huart);
}
