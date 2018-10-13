/*
** File   : coapdemo.c
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
** Author : chengkai2@gosuncn.cn
**
**  $Date: 2018/06/18 08:45:36GMT+08:00 $
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
    
#define MAX_IP_BYTE                 20
#define MAX_HEX_SIZE                1024
#define MAX_REGISTER_SIZE           1024
#define LIFETIME                    6000   
#define COAP_AS_CLIENT_ONLY         0
#define BOTH_CLIENT_AND_SERVER      1

static unsolicited_handler_t g_handler;
static gsdk_handle_t g_huart;
static gsdk_status_t status;

#define MAX_RECEIVE_SIZE    1024
static char unsolicited_buf[MAX_RECEIVE_SIZE] = {0};

int oem_debuglog = 1;
int coap_client_id = -1;
int coap_server_id = -1;
char ip_buf[64] = {0};
int coap_type_flag = 0;

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
    int ret = 0;
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
        //syslog_printf("[COAP_DEMO]: failed to open uart %d\n", status);
        return -1;
    }

    OEM_PRINTF("[COAP_DEMO] log init....OK\r\n");

    return 0;
}

int gsdk_ril_init_func(void)
{
    status = gsdk_ril_init();
    if (status != GSDK_SUCCESS) {
        OEM_PRINTF("[COAP_DEMO] gsdk_ril_init error!\r\n");
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

    OEM_PRINTF("[COAP_DEMO] init_func end!\r\n");
    return 0;
init_fail:
    OEM_PRINTF("[COAP_DEMO] init_func error!\r\n");
    return -1;
}


static int coap_handle_func(const char* s, const char* extra, int len, void* data)
{
    UNUSED_PARAM(extra);
    UNUSED_PARAM(len);
    UNUSED_PARAM(data);

    memset(unsolicited_buf, 0, sizeof(unsolicited_buf));
    if (strlen(s) > (MAX_REGISTER_SIZE - 1)) {
        OEM_PRINTF("[COAP_DEMO] the information is too large ,please change the size of recv_buf\r\n");
    } else {
        strncpy(unsolicited_buf, s, MAX_REGISTER_SIZE);
        OEM_PRINTF("%s\r\n\r\n", unsolicited_buf);
    }

    return 1;
}


static int coap_unsolicited_func(unsolicited_handler_t *handler)
{
    memset(handler, 0, sizeof(unsolicited_handler_t));
    handler->prefix = "+ECOAPNMI:";
    handler->func = coap_handle_func;
    handler->user_data = NULL;
    status = gsdk_at_register_unsolicited_handler(handler);
    if (status != GSDK_SUCCESS) {
        OEM_PRINTF("[COAP_DEMO] some error hanppened in handle,status is %d\r\n", status);
        return -1;
    }
    return 0;
}

int create_coap_client_func(void)
{
    char server_ip_addr[64] = "139.196.187.107";
    int server_port = 5683;
    int cid = 1;

    if(coap_type_flag == BOTH_CLIENT_AND_SERVER){
        status = gsdk_ril_create_coap_client(ip_buf, server_port, cid, &coap_client_id);
    }else{
        status = gsdk_ril_create_coap_client(server_ip_addr, server_port, cid, &coap_client_id);
    }
    if (status != GSDK_SUCCESS) {
        OEM_PRINTF("[COAP_DEMO] create_coap_client_func failed\r\n");
        return -1;
    }
    OEM_PRINTF("[COAP_DEMO] create_coap_client_func end!\r\n");
    return 0;
}

int coap_client_send_get_func(void)
{
    /************* get **************/
    int data_len = 11;
    char data_value1[32] = "40013b06b474657374c102";

    status = gsdk_ril_coap_client_send(coap_client_id, data_len, data_value1);
    if (status != GSDK_SUCCESS) {
        OEM_PRINTF("[COAP_DEMO] coap_client_send_get_func failed\r\n");
        return -1;
    }
    OEM_PRINTF("[COAP_DEMO] coap_client_send_get_func end\r\n");
    return 0;
}

int coap_client_send_put_func(void)
{
    /************* put ************/
    int data_len = 24;
    char data_value2[64] = "400325b6b876616c696461746510ff313233313233343536";

    status = gsdk_ril_coap_client_send(coap_client_id, data_len, data_value2);
    if (status != GSDK_SUCCESS) {
        OEM_PRINTF("[COAP_DEMO] coap_client_send_put_func failed!\r\n");
        return -1;
    }
    OEM_PRINTF("[COAP_DEMO] coap_client_send_put_func end!\r\n");
    return 0;
}

int coap_client_send_delete_func(void)
{
    /************* delete ************/
    int data_len = 10;
    char data_value3[32] = "40040214b47465737410";

    status = gsdk_ril_coap_client_send(coap_client_id, data_len, data_value3);
    if (status != GSDK_SUCCESS) {
        OEM_PRINTF("[COAP_DEMO] coap_client_send_delete_func failed!\r\n");
        return -1;
    }
    OEM_PRINTF("[COAP_DEMO] coap_client_send_delete_func end!\r\n");
    return 0;
}

int coap_client_send_post_func(void)
{
    /************* post ************/
    int data_len = 20;
    char data_value4[64] = "40020216b47465737410ff313233313233343536";

    status = gsdk_ril_coap_client_send(coap_client_id, data_len, data_value4);
    if (status != GSDK_SUCCESS) {
        OEM_PRINTF("[COAP_DEMO] coap_client_send_post_func failed!\r\n");
        return -1;
    }
    OEM_PRINTF("[COAP_DEMO] coap_client_send_post_func end!\r\n");
    return 0;
}

int coap_client_send_func(void)
{
    if(coap_client_send_get_func() != 0) goto fail;
    vTaskDelay(3 * 1000 / portTICK_RATE_MS);

    if(coap_client_send_put_func() != 0) goto fail;
    vTaskDelay(3 * 1000 / portTICK_RATE_MS);

    if(coap_client_send_delete_func() != 0) goto fail;
    vTaskDelay(3 * 1000 / portTICK_RATE_MS);
    
    if(coap_client_send_post_func() != 0) goto fail;

    return 0;
    OEM_PRINTF("[COAP_DEMO] coap_client_send_func end!\r\n");
fail:
    OEM_PRINTF("[COAP_DEMO] coap_client_send_func error!\r\n");
    return -1;
}

int delete_coap_client_func(void)
{
    status = gsdk_ril_coap_delete(coap_client_id);
    if (status != GSDK_SUCCESS) {
        OEM_PRINTF("[COAP_DEMO] delete_coap_client_func error!\r\n");
        return -1;
    }
    OEM_PRINTF("[COAP_DEMO] delete_coap_client_func end!\r\n");
    return 0;
}

int create_coap_server_func(void)
{
    int server_port = 5683;
    int cid = 1;

    status = gsdk_ril_create_coap_server(ip_buf, server_port, cid, &coap_server_id);
    if (status != GSDK_SUCCESS) {
        OEM_PRINTF("[COAP_DEMO] create_coap_server_func error!\r\n");
        return -1;
    }
    OEM_PRINTF("[COAP_DEMO] create_coap_server_func end!\r\n");
    return 0;
}

int ip_wait_func()
{
    int count = 0;

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

int main(void)
{
    oemdebuglog_open();
    if(init_func() != 0) goto _fail;
    if(ip_wait_func() != 0) goto _fail;
    coap_unsolicited_func(&g_handler);

    OEM_PRINTF("[COAP_DEMO] as a client, to communicate with outside server.\r\n");
    if(create_coap_client_func() != 0) goto _fail;
    if(coap_client_send_func() != 0) goto _fail;
    vTaskDelay(3 * 1000 / portTICK_RATE_MS);
    if(delete_coap_client_func() != 0) goto _fail;
    
    vTaskDelay(3 * 1000 / portTICK_RATE_MS);
    OEM_PRINTF("[COAP_DEMO] both a server and a client, to communicate.\r\n");
    coap_type_flag = BOTH_CLIENT_AND_SERVER;
    if(create_coap_server_func() != 0) goto _fail;
    if(create_coap_client_func() != 0) goto _fail;
    if(coap_client_send_get_func() != 0) goto _fail;
    OEM_PRINTF("[COAP_DEMO] coap demo over!!!\r\n");
    while(1){
        vTaskDelay(100);
    }

_fail:

    OEM_PRINTF("[COAP_DEMO] main function abnormal exit!\r\n");
    return 0;
}

