/*
** File   : multi_task.c
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
** demo说明：此版本示例程序为多任务并发测试demo，程序中预设置4个任务，并发进行，
**           每个任务每隔2s执行一次，实际使用中，任务的个数、大小、周期、以及任
**           务栈占空间大小均可根据需要进行配置。
** 
** Author : chengkai2@gosuncn.cn
**
**  $Date: 2018/09/03 08:45:36GMT+08:00 $
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
#include "lwip/sockets.h"
#include "lwip/ip.h"

#define EVENT_QUEUE_DEPTH 8

int customer_debuglog = 1;
int oem_debuglog = 1;

/* call this to open customer log */
int customerdebuglog_open(void)
{
    customer_debuglog = 1;
    return customer_debuglog;
}
/* call this to close customer log */
int customerdebuglog_close(void)
{
    customer_debuglog = 0;
    return customer_debuglog;
}
/* call this to open oem log */
int oemdebuglog_open(void)
{
    oem_debuglog = 1;
    return oem_debuglog;
}
/* call this to close oem log */
int oemdebuglog_close(void)
{
    oem_debuglog = 0;
    return oem_debuglog;
}
/* customer use this to printf log */
void CUSTOMER_PRINTF(char *s, ...)
{
    if (customer_debuglog == 1){
        printf(s);
    }
    else if (customer_debuglog == 0){
    }
}
/* OEM use this to printf log */
void OEM_PRINTF(char *fmt, ...)
{
    if (oem_debuglog == 1){
        printf(fmt);
    }
    else if (oem_debuglog == 0){
    }
}

typedef enum{
    IP_GET_EVENT,
    NETWORK_DISCON_EVENT,
    INVALID_EVENT
} state_event_t;

static gsdk_handle_t g_huart;
static TaskHandle_t task_handle_1 = NULL;
static TaskHandle_t task_handle_2 = NULL;
static TaskHandle_t task_handle_3 = NULL;
static TaskHandle_t task_handle_4 = NULL;

int __io_puts(char *data, int len)
{
    int ret = 0;
    if (g_huart){
        ret = gsdk_uart_write(g_huart, (uint8_t *)data, len, 1000);
    }
    return ret;
}

int log_init()
{
    uart_config_t uart_config;
    gsdk_status_t status;

    uart_config.baudrate     = UART_BAUDRATE_115200;
    uart_config.parity       = UART_PARITY_NONE;
    uart_config.stop_bit    = UART_STOP_BIT_1;
    uart_config.word_length  = UART_WORD_LENGTH_8;

    status = gsdk_uart_open(UART_0, &uart_config, &g_huart);
    if(status != GSDK_SUCCESS){
        OEM_PRINTF("failure to open uart\r\n");
        return -1;
    }
    return 0;
}

int gsdk_ril_init_func()
{
    gsdk_status_t status;
    status = gsdk_ril_init();
    if(status != GSDK_SUCCESS){
        OEM_PRINTF("ril init failed\r\n");
        return -1;
    }
    return 0;
}

int init_func()
{
    if(log_init() != 0)
        goto init_fail;
    if(gsdk_ril_init_func() != 0)
        goto init_fail;
    
    OEM_PRINTF("init_func end\r\n");
    return 0;
init_fail:
    OEM_PRINTF("init_func error\r\n");
    return -1;
}

void task_1_thread()
{
    while(1){
        vTaskDelay(2 * 1000/ portTICK_PERIOD_MS);
        OEM_PRINTF("task_1_thread running...\r\n");
    
    }
}

void task_2_thread()
{
    while(1){
        vTaskDelay(2 * 1000/ portTICK_PERIOD_MS);
        OEM_PRINTF("task_2_thread running...\r\n");

    }
}

void task_3_thread()
{
    while(1){
        vTaskDelay(2 * 1000/ portTICK_PERIOD_MS);
        OEM_PRINTF("task_3_thread running...\r\n");

    }
}

void task_4_thread()
{
    while(1){
        vTaskDelay(2 * 1000/ portTICK_PERIOD_MS);
        OEM_PRINTF("task_4_thread running...\r\n");

    }
}

int task_1()
{
    if(pdPASS != xTaskCreate(task_1_thread, "task_1_thread", 512*1, NULL, TASK_PRIORITY_LOW, &task_handle_1)){
        OEM_PRINTF("gps data send task create failured\r\n");
        return -1;
    }
    OEM_PRINTF("task_1 end\r\n");
    return 0;
}

int task_2()
{
    if(pdPASS != xTaskCreate(task_2_thread, "task_2_thread", 512*1, NULL, TASK_PRIORITY_LOW, &task_handle_2)){
        OEM_PRINTF("gps data send task create failured\r\n");
        return -1;
    }
    OEM_PRINTF("task_2 end\r\n");
    return 0;
}

int task_3()
{
    if(pdPASS != xTaskCreate(task_3_thread, "task_3_thread", 512*1, NULL, TASK_PRIORITY_LOW, &task_handle_3)){
        OEM_PRINTF("gps data send task create failured\r\n");
        return -1;
    }
    OEM_PRINTF("task_3 end\r\n");
    return 0;
}

int task_4()
{
    if(pdPASS != xTaskCreate(task_4_thread, "task_4_thread", 512*1, NULL, TASK_PRIORITY_LOW, &task_handle_4)){
        OEM_PRINTF("gps data send task create failured\r\n");
        return -1;
    }
    OEM_PRINTF("task_4 end\r\n");
    return 0;
}

int ip_wait_func()
{
    int count = 0;
    gsdk_status_t status;
    char ip_buf[64] = {0};

    while(1){
        if(count > 100){
            OEM_PRINTF("wait for IP timeout\r\n");
            return -1;
        }
        memset(ip_buf, 0, 64);
        status = gsdk_ril_get_local_ipaddr(ip_buf, 64);
        if(status == 1){
            OEM_PRINTF("get ip success!\r\n");
            break;
        }
        OEM_PRINTF("get ip failed!\r\n");
        vTaskDelay(100);
        count++;
    }
    OEM_PRINTF("ip registered\r\n");
    return 0;
}

/*
 * 多任务demo示例代码中，在任务开始前，有等待模块获取IP步骤，在测试与驻网无关的接口时，
 * 此步骤可以省略。main函数结尾处用死循环while(1)确保主函数不退出。
 */
int main()
{
    oemdebuglog_open();
    if(init_func() != 0) goto end;
    if(ip_wait_func() != 0) goto end;

    if(task_1() != 0) goto end;
    if(task_2() != 0) goto end;
    if(task_3() != 0) goto end;
    if(task_4() != 0) goto end;

    while(1){
        vTaskDelay(20 * 1000/ portTICK_PERIOD_MS);
        OEM_PRINTF("main function running...\r\n");
    }
end:
    OEM_PRINTF("main function exit!\r\n");
    return 0;
}
