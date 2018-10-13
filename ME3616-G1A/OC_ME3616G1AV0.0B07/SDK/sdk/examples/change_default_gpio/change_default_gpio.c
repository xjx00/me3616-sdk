/*
** File   : adc.c
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
** Author : zhangxin2@gosuncn.cn
**
**  $Date: 2018/09/04 08:45:36GMT+08:00 $
**
*/

/*
 * This example shows how to modify array members for change UE default gpio configure.
 */

#include "gsdk_sdk.h"

static gsdk_handle_t g_huart;

/*  array member:                                       */
/*  member1: is changed           1:yse 0:no             */
/*  member2: gpio num                              		 */
/*  member3: function mode                               */
/*  member4: gpio direction                     			*/
/*  member5: gpio pull mode                             */
/*  member6: gpio output data                           */

/* Welinkopen default gpio configure table for customer */
/* Note: The array is only allowed to modify its members,
						the other is not allowed to change*/
const default_gpio_config_t _headware_init_table[DEFAULT_GPIO_MAX_NUM] = {
	{0,gpio_pin_5, MODE_1,gpio_direction_none,gpio_pull_none,gpio_data_none},  // GPIO5
	{0,gpio_pin_2, MODE_1,gpio_direction_none,gpio_pull_up,gpio_data_none}, // GPIO2
	{0,gpio_pin_19,MODE_2,gpio_direction_none,gpio_pull_down,gpio_data_none}, // GPIO19
	{0,gpio_pin_20,MODE_2,gpio_direction_none,gpio_pull_down,gpio_data_none}, // GPIO20
	{0,gpio_pin_21,MODE_2,gpio_direction_none,gpio_pull_down,gpio_data_none}, // GPIO21
	{0,gpio_pin_22,MODE_2,gpio_direction_none,gpio_pull_down,gpio_data_none}, // GPIO22
	{0,gpio_pin_18,MODE_3,gpio_direction_none,gpio_pull_down,gpio_data_none}, // GPIO18
	{0,gpio_pin_23,MODE_3,gpio_direction_none,gpio_pull_down,gpio_data_none}, // GPIO23
	{0,gpio_pin_25,MODE_2,gpio_direction_none,gpio_pull_down,gpio_data_none}, // GPIO25
	{0,gpio_pin_24,MODE_2,gpio_direction_none,gpio_pull_down,gpio_data_none}, // GPIO24
	{0,gpio_pin_29,MODE_0,gpio_direction_output,gpio_pull_none,gpio_data_high}, // GPIO29
	{0,gpio_pin_27,MODE_0,gpio_direction_output,gpio_pull_none,gpio_data_high}, // GPIO27
	{0,gpio_pin_26,MODE_0,gpio_direction_output,gpio_pull_none,gpio_data_none}, // GPIO26
	{0,gpio_pin_9, MODE_0,gpio_direction_input,gpio_pull_down,gpio_data_none}, // GPIO9
	{0,gpio_pin_31,MODE_0,gpio_direction_input,gpio_pull_up,gpio_data_none}, // GPIO31
	{0,gpio_pin_33,MODE_4,gpio_direction_none,gpio_pull_up,gpio_data_none}, // GPIO33
	{0,gpio_pin_34,MODE_4,gpio_direction_none,gpio_pull_up,gpio_data_none}, // GPIO34
	{0,gpio_pin_30,MODE_5,gpio_direction_none,gpio_pull_up,gpio_data_none}  // GPIO30
};

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
        gsdk_syslog_printf("[ADC_DEMO]: failed to open uart %d\n", status);
        return -1;
    }

    printf("log init....OK\r\n");

    return 0;
}

int main(void)
{
    log_init();			//init the uart0 for debug log
	
	while(1)
	{
		vTaskDelay(1);
	}
	
    return 0;
}
