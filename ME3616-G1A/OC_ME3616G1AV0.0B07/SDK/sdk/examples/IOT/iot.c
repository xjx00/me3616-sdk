/*
** File   : iot.c
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
** brief:This demo be aimed at introduce iot operation.Include link iot platform,
** send data to iot platform,recv data from iot platform and delete iot link.
**
** Author : liyong2@gosuncn.cn
**
** $Date: 2018/05/24 08:45:36GMT+08:00 $
**
*/

#include "gsdk_sdk.h"

#define DEBUG_LOG(fmt,...) printf("[LOGD]:Function:%s,Line:%d,"fmt,__func__,__LINE__,##__VA_ARGS__);
#define WARN_LOG(fmt,...) printf("[LOGW]:"fmt,##__VA_ARGS__);

#define MAX_IP_BYTE 20
#define MAX_HEX_SIZE 1024
#define MAX_REGISTER_SIZE 1024
#define LIFETIME 6000
#define IMEI_SIZE 32

static gsdk_handle_t g_huart;

static char recv_buf[MAX_REGISTER_SIZE] = {0};

int __io_puts(char *data, int len)
{
    int ret = 0;
    if (g_huart) {
        ret = gsdk_uart_write(g_huart, (uint8_t *)data, len, 1000);
    }
    return ret;
}

/*information that come from iot appear will execute this function*/
static int handle_func(const char *s, const char *extra, int len, void *data)
{
    (void)extra;
    (void)len;
    WARN_LOG("infomation come from iot is %s\r\n", s);
    if (strlen(s) > (MAX_REGISTER_SIZE - 1)) {
        WARN_LOG("the information is too large ,please change the size of recv_buf\r\n");
    } else {
        strncpy(data, s, MAX_REGISTER_SIZE);
    }
    return GSDK_AT_UNSOLICITED_HANDLED;
}

void str_to_hex(char *out, const char *in, int len)
{
    int i;
    for (i = 0; i < len; i++) {
        sprintf(out + (i * 2), "%02x", in[i]);
    }
}

int main(void)
{
    uart_config_t uart_config;

    gsdk_status_t status             = GSDK_ERROR;
    int ret 						 = -1;
    uart_config.baudrate             = UART_BAUDRATE_115200;
    uart_config.parity               = UART_PARITY_NONE;
    uart_config.stop_bit             = UART_STOP_BIT_1;
    uart_config.word_length          = UART_WORD_LENGTH_8;
    char ip[MAX_IP_BYTE]             = {0};
    char imei_buf[IMEI_SIZE]         = {0};
    int lifetime                     = LIFETIME;
    char business_data[]             = {"aabbcc1234567890"};
    char hex_user_data[MAX_HEX_SIZE] = {0};
    int count                        = 0;
    char srv_ip[MAX_IP_BYTE]         = {"180.101.147.115"};
    int srv_port                     = 5683;

    status = gsdk_uart_open(UART_0, &uart_config, &g_huart);
    if (status != GSDK_SUCCESS) {
        DEBUG_LOG("[PRINTF_DEMO]: failed to open uart %d\n", status);
        return -1;
    }
    WARN_LOG("open uart0 success\r\n");
	
	/*Before call gsdk_ril_XXX function,this function must be called*/
    status = gsdk_ril_init();
    if (status != GSDK_SUCCESS) {
        DEBUG_LOG(" AT CMD init failed\n");
        goto _fail;
    }

    /*Get NB module IMEI number*/
    status = gsdk_ril_get_imei(imei_buf, IMEI_SIZE);
    if (GSDK_ERROR == status) {
        DEBUG_LOG("Get imei number cmd failed\r\n");
        goto _fail;
    }

    WARN_LOG("Get imei number success,imei:%s\r\n", imei_buf + 7);

    /*wai for ip appear*/
    while (1) {
        if (count > 100) {
            WARN_LOG("wait for IP timeout\r\n");
            goto _fail;
        }
        ret = gsdk_ril_get_local_ipaddr(ip, MAX_IP_BYTE);
        if (ret == 1)
            break;
        vTaskDelay(500);
        count++;
    }
    WARN_LOG("ip is %s\r\n", ip);

    /*add function for handle unsolicited information relate to "+M2MCLIRECV:"*/
    unsolicited_handler_t handler;
    handler.prefix = "+M2MCLIRECV:";
    handler.func = handle_func;
    handler.user_data = recv_buf;
    status = gsdk_at_register_unsolicited_handler(&handler);
    if (status != GSDK_SUCCESS) {
        DEBUG_LOG("some error hanppened in handle2,status is %d\r\n", status);
        goto _fail;
    }

    /*set link iot*/
    status = gsdk_ril_set_link_iot(imei_buf + 7, srv_ip, srv_port, lifetime);
    if (status == GSDK_ERROR) {
        DEBUG_LOG("set link cmd is failed\r\n");
        goto _fail;
    } else if (status == GSDK_ERROR_UNINITIALIZED) {
        DEBUG_LOG("not register success,please do delete operation and set link again\r\n");
        goto _fail;
    } else if (status == GSDK_ERROR_UNSUPPORTED) {
        DEBUG_LOG("this maybe device not register previously,or signal is bad\r\n");
        goto _fail;
    } else if (status != GSDK_SUCCESS) {
        DEBUG_LOG("unknown error happen in set link iot\r\n");
        goto _fail;
    } else {
        WARN_LOG("set link iot success\r\n");
    }

    /*send update information to iot*/
    status = gsdk_ril_update_to_iot();
    if (status == GSDK_ERROR) {
        DEBUG_LOG("update iot cmd is failed\r\n");
        goto _fail;
    } else if (status != GSDK_SUCCESS) {
        DEBUG_LOG("unknown error happen in update iot\r\n");
        goto _fail;
    } else {
        WARN_LOG("update iot cmd is success\r\n");
    }

    /*data to hex*/
    str_to_hex(hex_user_data, business_data, strlen(business_data));

    /*send data to iot*/
    status = gsdk_ril_send_data_to_iot(hex_user_data);
    if (status == GSDK_ERROR) {
        DEBUG_LOG("send data to iot cmd is failed\r\n");
        goto _fail;
    } else if (status != GSDK_SUCCESS) {
        DEBUG_LOG("unknown error happen in send data to iot\r\n");
        goto _fail;
    } else {
        WARN_LOG("send data iot cmd is success\r\n");
    }

    /*wait for data from iot*/
    count = 0;
    while (1) {
        if (count > 100) {
            DEBUG_LOG("wait for receive information timeout\r\n");
            goto _fail;
        }
        if (0 == strncmp(recv_buf, "+M2MCLIRECV:", 12)) {
            WARN_LOG("send data to iot is success\r\n");
            break;
        }
        vTaskDelay(500 / portTICK_PERIOD_MS);
        count++;
    }

    /*delete link with iot*/
    status = gsdk_ril_del_link_iot();
    if (status == GSDK_ERROR) {
        DEBUG_LOG("delete link iot cmd failed\r\n");
        goto _fail;
    } else if (status == GSDK_ERROR_TIMEOUT) {
        DEBUG_LOG("wait for information timeout,maybe signal bad\r\n");
        goto _fail;
    } else if (status != GSDK_SUCCESS) {
        DEBUG_LOG("unknown error happen in delete link iot\r\n");
        goto _fail;
    } else {
        WARN_LOG("delete iot is succcess\r\n");
    }

    WARN_LOG("data from iot is %s\r\n", recv_buf);
    gsdk_uart_close(g_huart);
    g_huart = NULL;
	
_fail:
    return 0;
}
