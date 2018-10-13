/*
** File   : gsdk_api.h
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
** Author : lixingyuan@gosuncn.cn
**
**  $Date: 2018/02/08 08:45:36GMT+08:00 $
**
*/

#ifndef _GSDK_API_H_
#define _GSDK_API_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "gsdk_common.h"
#include "gsdk_uart.h"
#include "gsdk_flash.h"
#include "gsdk_gpio.h"
#include "gsdk_gpt.h"
#include "gsdk_atport.h"
#include "gsdk_pwr.h"
#include "gsdk_rtc.h"
#include "gsdk_adc.h"
#include "gsdk_sys.h"
#include "gsdk_i2c.h"
#include "gsdk_alarm.h"
#include "gsdk_pwm.h"
#include "gsdk_spi.h"
#include "app_loader.h"
void gsdk_syslog_printf(const char *fmt, ...);
int syslog_write(const char *ptr, int len);

#ifdef __cplusplus
}
#endif

#endif
