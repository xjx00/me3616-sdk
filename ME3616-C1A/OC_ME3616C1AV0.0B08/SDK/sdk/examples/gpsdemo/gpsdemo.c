/*
** File   : gpsdemo.c
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
** Author : liyong2@gosuncn.cn
**
**  $Date: 2018/05/25 15:00:36GMT+08:00 $
**
**
**
**
**
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gsdk_api.h"
#include "FreeRTOS.h"
#include "gsdk_ril.h"
#include "semphr.h"
#include "timers.h"
#include "task.h"

#define ONE_SEC 100
#define GET_GPS_PERIOD 20

static gsdk_handle_t g_huart;
xTimerHandle xTimerHandle1;

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
        gsdk_syslog_printf("[OEM_DEMO]: failed to open uart %d\n\r", status);
        return -1;
    }

    printf("[OEM_DEMO] log init....OK\n\r");

    return 0;
}

int gps_callback(const char *s, const char *extra, int len, void *user_data)
{
    printf("[GPS_INFO] gps_info received: %s\n\r", s);
	return GSDK_AT_UNSOLICITED_HANDLED;
}

void vTimerCallback1()
{
    printf("TimerCallback1 triggered !\n\r");
    gsdk_ril_get_gps();
}

int main()
{
    gsdk_status_t status = GSDK_SUCCESS;

    log_init();

    status = gsdk_ril_init();
    if (status != GSDK_SUCCESS) {
        printf("[OEM_DEMO] AT CMD init failed\n\r");
        goto _fail;
    }
    
	vTaskDelay(ONE_SEC * 2);
	
    status = gsdk_ril_set_gps_operation(2, gps_callback, NULL);
    if (status != GSDK_SUCCESS) {
        printf("[OEM_DEMO] Interface ERROR!!!\n");
        goto _fail;
    }

    xTimerHandle1 = xTimerCreate("Timer1",
                                 GET_GPS_PERIOD * ONE_SEC,
                                 pdTRUE,
                                 (void *) 0,
                                 (TimerCallbackFunction_t) vTimerCallback1);

    if (pdFAIL == xTimerStart(xTimerHandle1, 500)) {
        printf("start timer 1 failed!");
    }

    printf("all initialization finished!\n\r");
_fail:

    return 0;
}
