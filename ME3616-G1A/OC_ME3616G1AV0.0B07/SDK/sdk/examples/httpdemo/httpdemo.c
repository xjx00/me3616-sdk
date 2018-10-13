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
** Author : zhangsiyuan@gosuncn.cn
**
**  $Date: 2018/06/28 
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
    
#define MAX_IP_BYTE 128
#define MAX_HEX_SIZE 1024
#define MAX_REGISTER_SIZE 1024
#define LIFETIME 6000



static gsdk_handle_t g_huart;
static gsdk_handle_t huart;
    
#define MAX_RECEIVE_SIZE    1024




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
        //syslog_printf("[ONENET_DEMO]: failed to open uart %d\n", status);
        return -1;
    }

    printf("[HTTP_DEMO] log init....OK\r\n");

    return 0;
}

int uart_init(void)
{
    gsdk_status_t status;
    uart_config_t uart_config;
    const char hello[] = "[HTTP_DEMO] Open Uart Successfully!\r\n";

    uart_config.baudrate    = UART_BAUDRATE_9600;
    uart_config.parity      = UART_PARITY_NONE;
    uart_config.stop_bit    = UART_STOP_BIT_1;
    uart_config.word_length = UART_WORD_LENGTH_8;

    status = gsdk_uart_open(UART_2, &uart_config, &huart);
    if (status != GSDK_SUCCESS) {
        //syslog_printf("[ONENET_DEMO]: failed to open uart %d\n", status);
        return -1;
    }

    gsdk_uart_write(huart, (uint8_t *)hello, strlen(hello), 100);

    //gsdk_uart_close(huart);

    return 0;
}



int http_callback(const char *s, const char *extra, int len, void *user_data)
{
 //UNUSED_PARAM is used to eliminate compilation warnings
	UNUSED_PARAM(extra);
	UNUSED_PARAM(len);
	UNUSED_PARAM(user_data);
	
    printf(" %s\n\r", s);
	return 1;
}

int main(void)
{
    gsdk_status_t status;
    int ret = -1;
    char ip[MAX_IP_BYTE] = {0};
    int count = 0;


    /************************* http create init  *********************************/
    int httpclient_id;

    http_create http_new;

    memset(&http_new,0x0,sizeof(http_create));
    http_new.flag = 0;
    http_new.total_len = 38;
    http_new.len = 38;
    http_new.host = "http://219.144.130.27:80/";
    http_new.server_cert_len = 0;
    http_new.client_cert_len = 0;
    http_new.client_pk_len = 0;
    /************************* http send init **********************************/

     http_send send;
     memset(&send,0x0,sizeof(http_send));
     send.flag = 0;
     send.total_len = 26;
     send.len = 26;
     send.method = 0;
     send.path_len = 9;
     send.path = "/Test.txt";
     send.customer_header_len = 0;
     send.content_type_len = 0;
     send.content_string_len = 0;
    //uart0 log init
    log_init();

    //uart2 init
    //uart_init();

    printf("[HTTP_DEMO] main!\n");

    status = gsdk_ril_init();
    if (status != GSDK_SUCCESS) {
        printf(" AT CMD init failed\n");
        //return -1;
    }

    /*wait for ip appear*/
    while (1) {
        if (count > 100) {
            printf("wait for IP timeout\r\n");
            return -1;
        }
        ret = gsdk_ril_get_local_ipaddr(ip, MAX_IP_BYTE);
        if (ret == 1)
            break;
        vTaskDelay(500);
        count++;
    }
    printf("ip is %s\r\n\r\n", ip);
    

    /*************************** http create client ********************************/

    status = gsdk_ril_create_http_client(&http_new, &httpclient_id);

    if(GSDK_SUCCESS != status){
        printf("create http client failed:%d\r\n",status);
        goto _fail;
    }
    
    send.httpclient_id = httpclient_id;// httpclient_id
    /************************ http client connect *********************************/
    if(GSDK_SUCCESS == gsdk_ril_connect_http_client(httpclient_id)){
        printf("HTTP connect interface exe success.\n");
    }else{
        printf("HTTP connect error.\n");
        goto _fail;
    }
    vTaskDelay(3 * 1000 / portTICK_RATE_MS);
    /************************* http send request *********************************/
    status = gsdk_ril_send_http_client(&send, http_callback,NULL);
    if (status != GSDK_SUCCESS) {
        printf("send request failed:%d\r\n", status);
        goto _fail;
    }
    vTaskDelay(5 * 1000 / portTICK_RATE_MS);

    /************************ disconnect http client *****************************/
    status = gsdk_ril_disconnect_http_client(httpclient_id);
    if(GSDK_SUCCESS != status){
        printf("disconnect http client failed:%d\r\n",status);
        goto _fail;
     }
    vTaskDelay(3 * 1000 / portTICK_RATE_MS);
    /************************ destroy http client ********************************/
    status = gsdk_ril_destroy_http_client(httpclient_id);
    if(GSDK_SUCCESS != status){
        printf("destroy http client failed:%d\r\n",status);
        goto _fail;
    }

     
_fail:

    gsdk_uart_close(g_huart);
    g_huart = NULL;

    return 0;

}


