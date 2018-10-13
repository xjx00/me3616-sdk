/*
** File   : gsdk_uart.h
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
**  $Date: 2018/03/12 09:32:36GMT+08:00 $
**
*/

#ifndef _GSDK_ALARM_H_
#define _GSDK_ALARM_H_


typedef void (*gsdk_alarm_callback_t)(void *user_data);

typedef struct callback_struct1 {
    void (*func)(struct callback_struct1 *callback);
    void *context;
} callback_struct_t1;

typedef struct _gsdk_alarm_config {
    int interval;
    int periodic;
    void *user_data;
    gsdk_alarm_callback_t callback;
} gsdk_alarm_config_t;

typedef struct _gsdk_alarm {
    uint32_t            magic;
    uint32_t            *p_timer_handle;
    int                 interval;
    int                 periodic;
    int                 started;
    void                *user_data;
    gsdk_alarm_callback_t callback;
    callback_struct_t1   user_callback;
} gsdk_alarm_t;

gsdk_status_t gsdk_alarm_open(gsdk_handle_t *phalarm, gsdk_alarm_config_t *config);
void gsdk_alarm_close(gsdk_handle_t halarm);
gsdk_status_t gsdk_alarm_start(gsdk_handle_t halarm);
gsdk_status_t gsdk_alarm_stop(gsdk_handle_t halarm);

#endif
