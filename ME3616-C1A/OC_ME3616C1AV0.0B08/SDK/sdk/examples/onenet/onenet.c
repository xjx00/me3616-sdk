/*
** File   : onenet.c
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
**  $Date: 2018/06/1 08:45:36GMT+08:00 $
**
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gsdk_api.h"
#include "FreeRTOS.h"
#include "task.h"
#include "gsdk_ril.h"
#include "gsdk_ril_cmds.h"

#define MAX_IP_BYTE 20
#define MAX_HEX_SIZE 1024
#define MAX_REGISTER_SIZE 1024
#define LIFETIME 6000

static gsdk_handle_t g_huart;
static unsolicited_handler_t g_handler;

#define MAX_RECEIVE_SIZE    1024
static char unsolicited_buf[MAX_RECEIVE_SIZE] = {0};

int flag = 0;

/** onenet err number type */
enum ONEERR {
    INVALID_ID,
    BOOTSTRAP_START,
    BOOTSTRAP_SUCCESS,
    BOOTSTRAP_FAILED,
    CONNECT_SUCCESS,
    CONNECT_FAILED,
    REG_SUCCESS,
    REG_FAILED,
    REG_TIMEOUT,
    LIFETIME_TIMEOUT,
    STATUS_HALT,
    UPDATE_SUCCESS,
    UPDATE_FAILED,
    UPDATE_TIMEOUT,
    UPDATE_NEED,
    UNREG_DONE,
    RESPONSE_FAILED,
    RESPONSE_SUCCESS,
    NOTIFY_FAILED,
    NOTIFY_SUCCESS
};

int oem_debuglog = 1;
gsdk_status_t status;
int ret = -1;
char ip[MAX_IP_BYTE] = {0};
int count = 0;
int instance, lifetime;
char *index1;
int i;
char ip_buf[64];

/* call this to open oem log */
int oemdebuglog_open(void)
{
    oem_debuglog = 1;
    return oem_debuglog;
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

int __io_puts(char *data, int len)
{
    if (g_huart) {
        ret = gsdk_uart_write(g_huart, (uint8_t *)data, len, 1000);
    }
    return ret;
}

int log_init(void)
{
    uart_config_t uart_config;

    uart_config.baudrate    = UART_BAUDRATE_115200;
    uart_config.parity      = UART_PARITY_NONE;
    uart_config.stop_bit    = UART_STOP_BIT_1;
    uart_config.word_length = UART_WORD_LENGTH_8;

    status = gsdk_uart_open(UART_0, &uart_config, &g_huart);
    if (status != GSDK_SUCCESS) {
        return -1;
    }
    OEM_PRINTF("[OneNET_DEMO] log init....OK\r\n");

    return 0;
}

int gsdk_ril_init_func(void)
{
    status = gsdk_ril_init();
    if (status != GSDK_SUCCESS) {
        OEM_PRINTF("[OneNET_DEMO] gsdk_ril_init error!\r\n");
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
    
    OEM_PRINTF("[OneNET_DEMO] init_func end!\r\n");
    return 0;
init_fail:
    OEM_PRINTF("[OneNET_DEMO] init_func error!\r\n");
    return -1;
}

static int onenet_handle_func(const char* s, const char* extra, int len, void* data)
{
    flag = 1;
    memset(unsolicited_buf, 0, sizeof(unsolicited_buf));
    if (strlen(s) > (MAX_REGISTER_SIZE - 1)) {
        OEM_PRINTF("the information is too large ,please change the size of recv_buf\r\n");
    } else {
        strncpy(unsolicited_buf, s, MAX_REGISTER_SIZE);
        OEM_PRINTF("%s\r\n\r\n", unsolicited_buf);
    }
    flag = 0;
    return 1;
}

static int onenet_unsolicited_func(unsolicited_handler_t *handler)
{
    memset(handler, 0, sizeof(unsolicited_handler_t));
    handler->prefix = "+MIPL";
    handler->func = onenet_handle_func;
    handler->user_data = NULL;
    status = gsdk_at_register_unsolicited_handler(handler);
    if (status != GSDK_SUCCESS) {
        OEM_PRINTF("some error hanppened in handle,status is %d\r\n", status);
        return -1;
    }
    return 0;
}

int create_instance_func(void)
{
    int totalsize_t = 51;
    char config_t[1024] = "130033f10003f20021050011000000000000000d3138332e3233302e34302e333900044e554c4cf3000cea040000044e554c4c";
    int index_t = 0;
    int currentsize_t = 51;
    int flag_t = 0;

    status = gsdk_ril_create_onenet_instance(totalsize_t, config_t, index_t, currentsize_t, flag_t, &instance);
    if (status != GSDK_SUCCESS) {
        OEM_PRINTF("create_instance_func error!");
        return -1;
    }
    OEM_PRINTF("[OneNET_DEMO] create_instance_func end!\r\n");
    return 0;
}

int create_object_func(void)
{
    int object_id = 3308;
    int instance_count = 3;
    char instance_bit_map[32] = "100";
    int attribute_count = 2;
    int acction_count = 0;

    status = gsdk_ril_create_onenet_object(instance, object_id, instance_count, instance_bit_map, attribute_count, acction_count);
    if (status != GSDK_SUCCESS) {
        OEM_PRINTF("[OneNET_DEMO] create_object_func error!\r\n");
        return -1;
    }
    OEM_PRINTF("[OneNET_DEMO] create_object_func end!\r\n");
    return 0;
}

int register_onenet_func(void)
{
    /************* register onenet ************/
    lifetime = 90;
    status = gsdk_ril_onenet_open(instance, lifetime);
    if (GSDK_SUCCESS != status) {
            OEM_PRINTF("[OneNET_DEMO] register_onenet_func error!\r\n");
        return -1;
    }
    OEM_PRINTF("[OneNET_DEMO] register_onenet_func end!\r\n");
    return 0;
}

int ip_wait_func()
{
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
    OEM_PRINTF("ip_wait_func end! ip registered\r\n");
    return 0;
}

int capture_register_info_func(void)
{
    /* capture register info */
    if (0 == strncmp(unsolicited_buf, "+MIPLEVENT:", 11)) {
        index1 = strtok(unsolicited_buf, ":");
        for(i=0; i<2; i++){
            index1 = strtok(NULL, ",");
        }
        if (atoi(index1) == REG_SUCCESS) {
            OEM_PRINTF("register OK\r\n");
        }
        if (atoi(index1) == BOOTSTRAP_FAILED) {
            OEM_PRINTF("register failed\r\n");
            return -1;
        }
        memset(unsolicited_buf, 0x0, MAX_RECEIVE_SIZE);
    }
    return 0;
}

int observe_notify_func(void)
{
    /***************************** onenet observe ********************************/
    if (0 == strncmp(unsolicited_buf, "+MIPLOBSERVE:", 13)) {
        index1 = strtok(unsolicited_buf, ":");
        for(i=0; i<2; i++){
            index1 = strtok(NULL, ",");
        }
        int observe_msgid = atoi(index1);
        int observe_result = 1;
        memset(unsolicited_buf, 0x0, MAX_RECEIVE_SIZE);
        status = gsdk_ril_rsp_onenet_observe(instance, observe_msgid, observe_result);
        if (GSDK_SUCCESS != status) {
            OEM_PRINTF("onenet observe failed:%d\r\n", status);
            return -1;
        }
    
        /***************** notify ******************/
        int notify_object_id = 3308;
        int notify_instance_id = 2;
    
        status = gsdk_ril_notify_onenet(instance, observe_msgid, notify_object_id, notify_instance_id, 5900,
                                        4, 4, 1.8, 1, 1);
        status = gsdk_ril_notify_onenet(instance, observe_msgid, notify_object_id, notify_instance_id, 5750,
                                        1, 5, 3, 0, 0);
        if (GSDK_SUCCESS != status) {
            OEM_PRINTF("onenet notify failed:%d\r\n", status);
            return -1;
        }
        memset(unsolicited_buf, 0x0, MAX_RECEIVE_SIZE);
    }
    return 0;
}

int discover_func(void)
{
    /***************************** onenet discover ********************************/
    if (0 == strncmp(unsolicited_buf, "+MIPLDISCOVER:", 14)) {
        index1 = strtok(unsolicited_buf, ":");
        for(i=0; i<2; i++){
            index1 = strtok(NULL, ",");
        }
        int discover_msgid = atoi(index1);
        int discover_result = 1;
        int discover_len = 9;
        char discover_value_string[128] = "5900;5750";
        status = gsdk_ril_rsp_onenet_discover(instance, discover_msgid, discover_result, discover_len, discover_value_string);
        if (GSDK_SUCCESS != status) {
            OEM_PRINTF("onenet discover failed:%d\r\n", status);
            return -1;
        }
        memset(unsolicited_buf, 0x0, MAX_RECEIVE_SIZE);
    }
    return 0;
}

int read_func(void)
{
    /***************************** onenet read ********************************/
    if (0 == strncmp(unsolicited_buf, "+MIPLREAD:", 10)) {
        index1 = strtok(unsolicited_buf, ":");
        for(i=0; i<2; i++){
            index1 = strtok(NULL, ",");
        }
        int read_msgid = atoi(index1);
        int read_result = 1;
        int read_objected = 3308;
        int read_instance_id = 2;
        int read_resource_id = 5900;
        int read_value_type = 4;
        int read_len = 4;
        int read_value = 1;
        int read_index = 0;
        int read_flag = 0;
        status = gsdk_ril_rsp_onenet_read(instance, read_msgid, read_result, read_objected, read_instance_id,
                                          read_resource_id, read_value_type, read_len, read_value, read_index, read_flag);
        if (GSDK_SUCCESS != status) {
            OEM_PRINTF("onenet read failed:%d\r\n", status);
            return -1;
        }
        memset(unsolicited_buf, 0x0, MAX_RECEIVE_SIZE);
    }
    return 0;
}

int write_func(void)
{
    /***************************** onenet write ********************************/
    if (0 == strncmp(unsolicited_buf, "+MIPLWRITE:", 11)) {
        index1 = strtok(unsolicited_buf, ":");
        for(i=0; i<2; i++){
            index1 = strtok(NULL, ",");
        }
        int write_msgid = atoi(index1);
        int write_result = 2;
    
        status = gsdk_ril_rsp_onenet_write(instance, write_msgid, write_result);
        if (GSDK_SUCCESS != status) {
            OEM_PRINTF("onenet write failed:%d\r\n", status);
            return -1;
        }
        memset(unsolicited_buf, 0x0, MAX_RECEIVE_SIZE);
    }
    return 0;
}

int execute_func(void)
{
    /***************************** onenet execute ********************************/
    if (0 == strncmp(unsolicited_buf, "+MIPLEXECUTE:", 13)) {
        index1 = strtok(unsolicited_buf, ":");
        for(i=0; i<2; i++){
            index1 = strtok(NULL, ",");
        }
        int execute_msgid = atoi(index1);
        int execute_result = 2;
    
        status = gsdk_ril_rsp_onenet_execute(instance, execute_msgid, execute_result);
        if (GSDK_SUCCESS != status) {
            OEM_PRINTF("onenet execute failed:%d\r\n", status);
            return -1;
        }
        memset(unsolicited_buf, 0x0, MAX_RECEIVE_SIZE);
    }
    return 0;
}

void onenet_close_func(void)
{
    // close instance
    status = gsdk_ril_onenet_close(instance);
    if (status != GSDK_SUCCESS) {
        OEM_PRINTF("[OneNET_DEMO] onenet_close_func error!\r\n");
    }
    OEM_PRINTF("[OneNET_DEMO] onenet_close_func end!\r\n");
    return;
}

int main(void)
{
    oemdebuglog_open();
    if(init_func() != 0) goto _fail;
    if(ip_wait_func() != 0) goto _fail;
    
    onenet_unsolicited_func(&g_handler);
    if(create_instance_func() != 0) goto _fail;
    if(create_object_func() != 0) goto _fail;
    if(register_onenet_func() != 0) goto _fail;

    memset(unsolicited_buf, 0x0, MAX_RECEIVE_SIZE);
    while (1) {
        vTaskDelay(50);
        if (1 == flag) continue;
        if(capture_register_info_func() != 0) break;
        if(observe_notify_func() != 0) break;
        if(discover_func() != 0) break;
        if(read_func() != 0) break;
        if(write_func() != 0) break;
        if(execute_func() != 0) break;
    }
    OEM_PRINTF("[OneNET_DEMO] while function error!!!\r\n");

_fail:

    OEM_PRINTF("[OneNET_DEMO] main function abnormal exit!\r\n");
    onenet_close_func();
    return 0;
}
