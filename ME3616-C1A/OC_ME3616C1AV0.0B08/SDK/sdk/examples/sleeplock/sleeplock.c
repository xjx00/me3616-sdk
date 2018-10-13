/*
** File   : sleeplock.c
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
** brief:This demo be aimed at
** Author : qubo@gosuncn.cn
**
**  $Date: 2018/03/16 08:45:36GMT+08:00 $
**
*/

#include "gsdk_sdk.h"

#define DEBUG_LOG(fmt,...) printf("[LOGD]:Function:%s,Line:%d,"fmt,__func__,__LINE__,##__VA_ARGS__);
#define WARN_LOG(fmt,...) printf("[LOGW]:"fmt,##__VA_ARGS__);


static gsdk_handle_t g_huart;
gsdk_handle_t hlock;
gsdk_handle_t g_alarm_handle;

int __io_puts(char *data, int len)
{
    int ret = 0;
    if (g_huart) {
        ret = gsdk_uart_write(g_huart, (uint8_t *)data, len, 1000);
    }
    return ret;
}

/*open uart0,log information out by uart0*/
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
        gsdk_syslog_printf("[PWR_DEMO]: failed to open uart %d\n", status);
        return -1;
    }

    WARN_LOG("[PWR_DEMO] log init....OK\r\n");

    return 0;
}

/*timer'callback*/
void rtc_alarm_cb(void *user_data)
{
    (void)user_data;
    gsdk_sleeplock_release(hlock);
    gsdk_sleeplock_close(hlock);
    WARN_LOG("[PWR_DEMO] lock number :%d!\r\n", gsdk_sleep_islocked());
}

/*configure timer*/
static gsdk_status_t set_alarm_timer()
{
    gsdk_alarm_config_t alarm_config;
    gsdk_status_t status;
    alarm_config.callback = rtc_alarm_cb;//timer overtime callback function will execute
    alarm_config.interval = 10;//10s
    alarm_config.periodic = 0;//this timer only execute one time
    status = gsdk_alarm_open(&g_alarm_handle, &alarm_config);
    if (status != GSDK_SUCCESS) {
        DEBUG_LOG("gsdk_alarm_open failed :%d\r\n", status);
    }
    return status;
}

int main(void)
{
    gsdk_status_t status;

    log_init();

    /*open sleep lock.if open success,the lock is valid*/
    status = gsdk_sleeplock_open("test_lock", &hlock);
    if (status != GSDK_SUCCESS) {
        DEBUG_LOG("open sleeplock fail\r\n");
        goto _fail_lock;
    }

    /*acquire lock.That is lock AP side,AP side not enter sleep*/
    status = gsdk_sleeplock_acquire(hlock);
    if (status != GSDK_SUCCESS) {
        DEBUG_LOG("acquire sleeplock fail\r\n");
        goto _fail_lock;
    }

    /*set a timer.purpose is release lock at timer's callback*/
    status = set_alarm_timer();
    if (GSDK_SUCCESS != status) {
        DEBUG_LOG("alarm set failed\r\n");
        goto _fail_lock;
    }
	
    /*start up timer*/
    status = gsdk_alarm_start(g_alarm_handle);
    if (status != GSDK_SUCCESS) {
        DEBUG_LOG("gsdk_alarm_start failed :%d\r\n", status);
        goto _fail;
    }

    vTaskDelay(12000 / portTICK_PERIOD_MS);

    return 0;
_fail:
    gsdk_alarm_close(g_alarm_handle);
_fail_lock:
    gsdk_sleeplock_close(hlock);
    WARN_LOG("[PWR_DEMO] Sleep lock test failed!\n");

    return -1;
}
