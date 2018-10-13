/*
** File   : pwm.c
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
#include "FreeRTOS.h"
#include "task.h"

static gsdk_handle_t g_huart;

void gpio_callback(void *user_data)
{
    printf("[PWM_DEMO] gpio eint handle callback!\n");
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
    gsdk_status_t status;

    uart_config.baudrate    = UART_BAUDRATE_115200;
    uart_config.parity      = UART_PARITY_NONE;
    uart_config.stop_bit    = UART_STOP_BIT_1;
    uart_config.word_length = UART_WORD_LENGTH_8;

    status = gsdk_uart_open(UART_0, &uart_config, &g_huart);
    if (status != GSDK_SUCCESS) {
        gsdk_syslog_printf("[PWM_DEMO]: failed to open uart %d\n\r", status);
        return -1;
    }

    printf("[PWM_DEMO] log init....OK\n\r");

    return 0;
}

int main(void)
{
    //GPIO settings
    gsdk_handle_t hgpio;
    gpio_config_t gpio_config;
    gpio_pin_t    gpio_pin;
    gpio_pin = GPIO_PIN_19;

    log_init();

    gpio_config.direction     = GPIO_DIRECTION_INPUT;
    gpio_config.pull          = GPIO_PULL_UP;
    gpio_config.int_mode      = GPIO_INT_EDGE_FALLING;
    gpio_config.debounce_time = 10;
    gpio_config.callback      = gpio_callback;

    printf("[PWM_DEMO] Ready to open gpio!\n");

    //PWM settings
    gsdk_status_t status;
    gsdk_handle_t hpwm;
    pwm_config_t config;
    pwm_channel_t channel = PWM_CHANNEL_3;

    config.pwm_pin = GPIO_PIN_26;
    config.duty_ratio = 50;
    config.frequency = 4;
    config.src_clock = PWM_CLOCK_32KHZ;

    status = gsdk_gpio_open(gpio_pin, &gpio_config, &hgpio);
    if (status != GSDK_SUCCESS)
        goto _fail;

    status = gsdk_pwm_open(channel, &config, &hpwm);
    if (status != GSDK_SUCCESS)
        goto _fail_gpio;

    status = gsdk_pwm_start(hpwm);
    if (status != GSDK_SUCCESS)
        goto _fail_pwm;

    vTaskDelay(1000);

    status = gsdk_pwm_stop(hpwm);
    if (status != GSDK_SUCCESS)
        goto _fail_pwm;

    vTaskDelay(500);

    status = gsdk_pwm_start(hpwm);
    if (status != GSDK_SUCCESS)
        goto _fail_pwm;

    return 0;

_fail_pwm:
    gsdk_pwm_close(hpwm);
    printf("[PWM_DEMO] PWM test failed!\n");
_fail_gpio:
    gsdk_gpio_close(hgpio);
    printf("[PWM_DEMO] Gpio test failed!\n");
_fail:
    return 0;
}
