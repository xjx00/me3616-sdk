/*
** File   : tcp.c
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
**  $Date: 2018/03/16 08:45:36GMT+08:00 $
**
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gsdk_api.h"
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
        gsdk_syslog_printf("[TCP_DEMO]: failed to open uart %d\n", status);
        return -1;
    }

    printf("[TCP_DEMO] log init....OK\r\n");

    return 0;
}

int main(void)
{
    int s = 0;
    int ret = 0;
    char sendcontent[] = "GET / HTTP/1.0\r\n\r\n";
    char srv_ip_addr[] = "114.55.56.19";
    char recv_buf[500] = {0};

    log_init();

    s = socket(AF_INET, SOCK_STREAM, 0); //TCP socket
    if (0 > s) {
        gsdk_syslog_printf("Create Socket Error!");
        return -1;
    }
    
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(srv_ip_addr);
    address.sin_port = htons(80);

    if (0 > connect(s, (struct sockaddr *)&address, sizeof(address))) {
        gsdk_syslog_printf("Can Not Connect To Server IP!\n");
        return -1;
    }
    ret = send(s, sendcontent, strlen(sendcontent), 0);
    gsdk_syslog_printf("send %d bytes data", ret);
    if (ret == -1) {
        return -1;
    }
    ret = 0;
    ret = recv(s, recv_buf, 500, 0);
    gsdk_syslog_printf("recv %d bytes data", ret);
    close(s);
    return 0;
}
