/*
** File   : gpio.c
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
** Author : qubo@gosuncn.cn
**
**  $Date: 2018/03/16 08:45:36GMT+08:00 $
**
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gsdk_api.h"

//#define TEST_INTERRUPT

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
        gsdk_syslog_printf("[GPIO_DEMO]: failed to open uart %d\n", status);
        return -1;
    }

    printf("[GPIO_DEMO] log init....OK\r\n");

    return 0;
}

void gpio_callback(void *user_data)
{
    printf("[GPIO_DEMO] gpio eint handle callback!\n");
}

int main(void)
{
    gsdk_status_t status;
    gsdk_handle_t hgpio;
    gpio_config_t gpio_config;
    gpio_pin_t    gpio_pin;

	gpio_driving_current_t driving;		//GPIO_DRIVING_CURRENT_4MA    = 0,        /**< Defines GPIO driving current as 4mA.  */
    									//GPIO_DRIVING_CURRENT_8MA    = 1,        /**< Defines GPIO driving current as 8mA.  */
    									//GPIO_DRIVING_CURRENT_12MA   = 2,        /**< Defines GPIO driving current as 12mA. */
    									//GPIO_DRIVING_CURRENT_16MA   = 3         /**< Defines GPIO driving current as 16mA. */
	
    gpio_pin = GPIO_PIN_26;

    log_init();

#ifdef TEST_INTERRUPT
    gpio_config.direction     = 0;
    gpio_config.pull          = 0;
    gpio_config.debounce_time = 0;
    gpio_config.int_mode      = GPIO_INT_LEVEL_HIGH;
    gpio_config.callback      = gpio_callback;
    gpio_config.user_data     = NULL;

    status = gsdk_gpio_open(gpio_pin, &gpio_config, &hgpio);
    if (status != GSDK_SUCCESS)
        goto _fail;
#else
    //led(demo board)
    gpio_config.direction     = GPIO_DIRECTION_OUTPUT;
    gpio_config.pull          = GPIO_PULL_NONE;
    gpio_config.int_mode      = GPIO_INT_DISABLE;
    gpio_config.debounce_time = 0;
    gpio_config.callback      = NULL;

    status = gsdk_gpio_open(gpio_pin, &gpio_config, &hgpio);
    if (status != GSDK_SUCCESS){
        goto _fail;
	}
	status = gsdk_gpio_get_driving_current(hgpio,gpio_pin,&driving);
	if (status != GSDK_SUCCESS){
        goto _fail;
	}
	printf("[GPIO_DEMO]Before set driving,Current driving:%d\r\n",driving);
	
	status = gsdk_gpio_set_driving_current(hgpio,gpio_pin,GPIO_DRIVING_CURRENT_16MA);
	if (status != GSDK_SUCCESS){
        goto _fail;
	}
	status = gsdk_gpio_get_driving_current(hgpio,gpio_pin,&driving);
	if (status != GSDK_SUCCESS){
        goto _fail;
	}
	printf("[GPIO_DEMO]After set driving,Current driving:%d\r\n", driving);
	
    status = gsdk_gpio_write(hgpio, GPIO_DATA_HIGH);
    if (status != GSDK_SUCCESS){
        goto _fail;
	}
#endif
    return 0;

_fail:
    gsdk_gpio_close(hgpio);
    printf("[GPIO_DEMO] Gpio test failed!\n");
    return -1;
}

