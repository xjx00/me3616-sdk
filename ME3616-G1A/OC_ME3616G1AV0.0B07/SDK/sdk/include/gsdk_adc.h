/*
** File   : gsdk_adc.h
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
**  $Date: 2018/02/08 08:45:36GMT+08:00 $
**
*/

#ifndef _GSDK_ADC_H_
#define _GSDK_ADC_H_

typedef enum {
    ADC_CHANNEL_0 = 0,                        /**< ADC channel 0. */
    ADC_CHANNEL_1 = 1,                        /**< ADC channel 1. */
    ADC_CHANNEL_2 = 2,                        /**< ADC channel 2. */
    ADC_CHANNEL_3 = 3,                        /**< ADC channel 3. */
    ADC_CHANNEL_4 = 4,                        /**< ADC channel 4. */
    ADC_CHANNEL_5 = 5,
    ADC_CHANNEL_6 = 6,
    ADC_CHANNEL_MAX
} adc_channel_t;

typedef enum{

	ADC_0 = 0,
	ADC_1 = 1,
	//ADC_2
	ADC_3 = 3,
	ADC_4 = 4,
	ADC_MAX
}adc_num_t;

/*****************************************************************************
 * Functions
 *****************************************************************************/

/**
 * @brief       This function open a adc handle
 * @param[in]   channel is the channel that will be sampled. This parameter can only be the value of type #adc_channel_t.
 * @param[in]   adc_volt is the adc voltage(0~1400mV)
 * @return
 * #GSDK_SUCCESS if OK.
*/
gsdk_status_t gsdk_adc_read(adc_num_t adc_num,adc_channel_t adc_channel, uint32_t *adc_volt);

#endif

