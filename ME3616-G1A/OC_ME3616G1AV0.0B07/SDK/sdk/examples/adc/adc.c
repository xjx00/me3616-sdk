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
 * This example shows how to read adc voltage
 */

#include "gsdk_sdk.h"

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

    uart_config.baudrate    = UART_BAUDRATE_115200;						//uart configuration:baudrate/prity/stop bit/word length
    uart_config.parity      = UART_PARITY_NONE;
    uart_config.stop_bit    = UART_STOP_BIT_1;
    uart_config.word_length = UART_WORD_LENGTH_8;

    status = gsdk_uart_open(UART_0, &uart_config, &g_huart);            //open uart0 for printf
    if (status != GSDK_SUCCESS) {
        gsdk_syslog_printf("[ADC_DEMO]: failed to open uart %d\n", status);		
        return -1;
    }

    printf("[ADC_DEMO] log init....OK\r\n");

    return 0;
}

/* adc maximum input voltage 1.4V */
int main(void)
{
    gsdk_status_t status;
    adc_num_t     adc_num;
    adc_channel_t adc_channel;
    uint32_t      adc_voltage;

    log_init();                     //init the uart0 for debug log

    adc_num = 0;                    //there are 4 AUXADC(10-bit) in welinkopen
                                    //AUXADC0/AUXADC1/AUXADC3/AUXADC4

                                    
    adc_channel = ADC_CHANNEL_0;    /*Collect the external voltage*/
                                    /*AUXADC0:channel0*/
                                    /*AUXADC1:channel1*/
                                    /*AUXADC3:channel3*/
                                    /*AUXADC4:channel4*/

                                    /*Collect the VABT voltage*/
                                    /*use channel6*/

    status = gsdk_adc_read(adc_num, adc_channel, &adc_voltage);   
    if (status != GSDK_SUCCESS) {
        printf("[ADC_DEMO] ADC test failed, error: %d!\n", status);
        return status;
    }

    printf("[ADC_DEMO] ADC Voltage : %d mV.\n", (int)adc_voltage);

    return 0;
}