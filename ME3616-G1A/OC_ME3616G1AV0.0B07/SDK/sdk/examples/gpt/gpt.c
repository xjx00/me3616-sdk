/*
** File   : gpt.c
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
** Author : zhanglei3@gosuncn.cn
**
**  $Date: 2018/08/20 08:45:36GMT+08:00 $
**
*/

/* this example shows how to use gpt timer */

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
        gsdk_syslog_printf("[GPT_DEMO]: failed to open uart %d\r\n", status);
        return -1;
    }

    printf("[GPT_DEMO] log init....OK\r\n");

    return 0;
}

/* timer callback function */
void gpt_callback(void *user_data)
{
    printf("[GPT_DEMO] echo user_data: %s!\r\n", (char *)user_data);
}

/************************************************************
  * first: open gpt timer
  * second: start gpt timer
  * last: close gpt timer
  **********************************************************/
int main(void)
{
    gsdk_status_t   status;
    gsdk_handle_t   hgpt;
    gpt_config_t    gpt_config;
    gpt_port_t      gpt_port;
    char *param = "abcdef";
    int i = 5;

    log_init();

    /*********************************************************
      * only gpt_timer1 and gpt_timer2 can be supported
      * gpt_port = GPT_PORT_1;
      * gpt_port = GPT_PORT_2;
      *******************************************************/
    gpt_port = GPT_PORT_1;

    /*********************************************************
      * only us level and ms level can be supported
      * gpt_config.um = GSDK_GPT_US;
      * gpt_config.um = GSDK_GPT_MS;
      *******************************************************/
    gpt_config.um = GSDK_GPT_MS;

    /*********************************************************
      * gpt_timer can support repeat trigger or one shot type
      * gpt_config.timer_type = GSDK_GPT_TYPE_ONE_SHOT;
      * gpt_config.timer_type = GSDK_GPT_TYPE_REPEAT;
      *******************************************************/
    gpt_config.timer_type = GSDK_GPT_TYPE_REPEAT;

    gpt_config.timer_out = 1000;
    gpt_config.function = &gpt_callback;
    gpt_config.data = param;

    /*************************************************************
      * timer instruction
      * open, start, stop, start, close
      *repeat 5 times
      ***********************************************************/
    while (i) {
        printf("open GPT_PORT_%d\r\n", gpt_port);
        status = gsdk_gpt_timer_open(gpt_port, &gpt_config, &hgpt);
        if (status != GSDK_SUCCESS) {
            printf("[GPT_DEMO] gpt open failed %d!\n", status);
            return status;
        }

        printf("start GPT_PORT_%d\r\n", gpt_port);
        status = gsdk_gpt_timer_start(hgpt);
        if (status != GSDK_SUCCESS) {
            printf("[GPT_DEMO] gpt start failed %d!\n", status);
            return status;
        }
        vTaskDelay(500);

        printf("stop GPT_PORT_%d\r\n", gpt_port);
        status = gsdk_gpt_timer_stop(hgpt);
        if (status != GSDK_SUCCESS) {
            printf("[GPT_DEMO] gpt stop failed %d!\n", status);
            return status;
        }
        vTaskDelay(100);

        printf("start GPT_PORT_%d\r\n", gpt_port);
        /* the timer must be in open or close state before starting */
        status = gsdk_gpt_timer_start(hgpt);
        if (status != GSDK_SUCCESS) {
            printf("[GPT_DEMO] gpt start failed %d!\n", status);
            return status;
        }
        vTaskDelay(500);

        printf("close GPT_PORT_%d\r\n", gpt_port);
        status = gsdk_gpt_timer_close(hgpt);
        if (status != GSDK_SUCCESS) {
            printf("[GPT_DEMO] gpt close failed %d!\n", status);
            return status;
        }
        vTaskDelay(1000);

        i--;
    }
    return 0;
}
