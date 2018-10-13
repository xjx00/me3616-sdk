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
        syslog_printf("[OEM_DEMO]: failed to open uart %d\n\r", status);
        return -1;
    }

    printf("[OEM_DEMO] log init....OK\n\r");

    return 0;
}

int gps_callback(const char *s, const char *extra, int len, void *user_data)
{
	printf("[GPS_INFO] gps_info received: %s\n\r", s);
}

void vTimerCallback1()
{
	printf("TimerCallback1 triggered !\n\r");
	gsdk_ril_get_gps();
}

int main()
{
    gsdk_status_t status;
	
	log_init();
    status = gsdk_ril_init();
    if (status != GSDK_SUCCESS) {
        printf("[OEM_DEMO] AT CMD init failed\n\r");
        goto _fail;
    }
	
    status = gsdk_ril_set_gps_operation(2, gps_callback, NULL);
    if (status != GSDK_SUCCESS) {
        printf("[OEM_DEMO] Interface ERROR!!!\n");
        goto _fail;
    }
	
    xTimerHandle1 = xTimerCreate
                   //调试用， 系统不用
                   ("Timer1",
                   //定时溢出周期， 单位是任务节拍数
                   GET_GPS_PERIOD * ONE_SEC,
                   //6000,
                   //是否自动重载， 此处设置周期性执行
                   pdTRUE,
                   //记录定时器溢出次数， 初始化零, 用户自己设置
                  ( void * ) 0,
                   //回调函数
                  (TimerCallbackFunction_t) vTimerCallback1);
_fail:
				  
	return 0;
}