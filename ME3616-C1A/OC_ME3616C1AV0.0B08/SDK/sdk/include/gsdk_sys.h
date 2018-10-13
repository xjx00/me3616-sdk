/*
** File   : gsdk_sys.h
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

#ifndef _GSDK_SYS_H_
#define _GSDK_SYS_H_

/** @brief This enum defines the power on result. */
typedef enum {
    GSDK_BOOT_FIRST_UP            = 0,     /* 1st boot up */
    GSDK_BOOT_DEEP_SLEEP          = 1,     /* deep sleep */
    GSDK_BOOT_DEEPER_SLEEP        = 2,     /* power off */
    GSDK_BOOT_SYS_RESET           = 3,     /* long press shutdown or sys_reset */
    GSDK_BOOT_WDT_HW_RESET        = 4,     /* wdt hw reset */
    GSDK_BOOT_WDT_SW_RESET        = 5,     /* wdt sw reset */
    GSDK_BOOT_FORCED_SHUT_DOWN    = 6,     /* forced shut down */
    GSDK_BOOT_MODE_INVALID        = 7
} gsdk_boot_mode_t;

typedef enum {
    GSDK_BOOT_RTC_TC_WAKEUP       = 10,   
    GSDK_BOOT_RTC_EINT_WAKEUP     = 11,   
    GSDK_BOOT_RTC_ALARM_WAKEUP    = 12,   
    GSDK_BOOT_POWERKEY_WAKEUP     = 13,   
    GSDK_BOOT_INVALID_WAKEUP      = 0xff, 
} gsdk_rtc_wakeup_mode_t;

gsdk_boot_mode_t gsdk_sys_get_boot_mode(gsdk_rtc_wakeup_mode_t *rtc_mode);

#endif

