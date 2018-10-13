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
static TaskHandle_t pressure_test_task_handle_1 = NULL;
static TaskHandle_t pressure_test_task_handle_2 = NULL;
static TaskHandle_t pressure_test_task_handle_3 = NULL;
static TaskHandle_t pressure_test_task_handle_4 = NULL;

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

    return 0;
init_fail:
    return -1;
}

void pressure_test_thread_1()
{
    while(1){
        vTaskDelay(100);
        /* add test AT interface here */
        OEM_PRINTF("test thread 1 while\r\n");
    }
}

void pressure_test_thread_2()
{
    while(1){
        vTaskDelay(100);
        /* add test AT interface here */
        OEM_PRINTF("test thread 2 while\r\n");
    }
}

void pressure_test_thread_3()
{
    while(1){
        vTaskDelay(100);
        /* add test AT interface here */
        OEM_PRINTF("test thread 3 while\r\n");
    }
}

void pressure_test_thread_4()
{
    while(1){
        vTaskDelay(100);
        /* add test AT interface here */
        OEM_PRINTF("test thread 4 while\r\n");
    }
}

int pressure_test_task_1()
{
    if(pdPASS != xTaskCreate(pressure_test_thread_1, "pressure_test_thread_1", 512*1, NULL, TASK_PRIORITY_LOW, &pressure_test_task_handle_1)){
        OEM_PRINTF("gps data send task create failured\r\n");
        return -1;
    }
    return 0;
}

int pressure_test_task_2()
{
    if(pdPASS != xTaskCreate(pressure_test_thread_2, "pressure_test_thread_2", 512*1, NULL, TASK_PRIORITY_LOW, &pressure_test_task_handle_2)){
        OEM_PRINTF("gps data send task create failured\r\n");
        return -1;
    }
    return 0;
}

int pressure_test_task_3()
{
    if(pdPASS != xTaskCreate(pressure_test_thread_3, "pressure_test_thread_3", 512*1, NULL, TASK_PRIORITY_LOW, &pressure_test_task_handle_3)){
        OEM_PRINTF("gps data send task create failured\r\n");
        return -1;
    }
    return 0;
}

int pressure_test_task_4()
{
    if(pdPASS != xTaskCreate(pressure_test_thread_4, "pressure_test_thread_4", 512*1, NULL, TASK_PRIORITY_LOW, &pressure_test_task_handle_4)){
        OEM_PRINTF("gps data send task create failured\r\n");
        return -1;
    }
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

int main()
{
	oemdebuglog_open();
    if(init_func() != 0)
        goto end;
    OEM_PRINTF("init end\r\n");
	if(ip_wait_func() != 0)
		goto end;
    OEM_PRINTF("ip_wait_func end\r\n");
    if(pressure_test_task_1() != 0)
        goto end;
    OEM_PRINTF("pressure_test_task_1 end\r\n");
    if(pressure_test_task_2() != 0)
        goto end;
    OEM_PRINTF("pressure_test_task_2 end\r\n");
    if(pressure_test_task_3() != 0)
        goto end;
    OEM_PRINTF("pressure_test_task_3 end\r\n");
    if(pressure_test_task_4() != 0)
        goto end;
    OEM_PRINTF("pressure_test_task_4 end\r\n");

	while(1){
        vTaskDelay(100);
        OEM_PRINTF("main function while\r\n");
	}
end:
    OEM_PRINTF("main function exit!\r\n");
    return 0;
}
