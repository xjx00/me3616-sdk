/*
 ** File   : gsdk_gpt.h
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
 ** Author : zhanglei3@gosuncn.cn
 **
 **  $Date: 2018/08/20 08:45:36GMT+08:00 $
 **
 */

#ifndef _GSDK_GPT_H_
#define _GSDK_GPT_H_

typedef enum {
    /* GPT PORT 0 used for light sleep */
    GPT_PORT_0 = 0,
    /* GPT PORT 1 can be used */
    GPT_PORT_1 = 1,
    /* GPT PORT 2 can be used */
    GPT_PORT_2 = 2,
    GPT_PORT_MAX,
} gpt_port_t;

typedef enum {
    /**< Set the GPT oneshot mode.  */
    GSDK_GPT_TYPE_ONE_SHOT = 0,
    /**< Set the GPT repeat  mode.  */
    GSDK_GPT_TYPE_REPEAT   = 1,
} gsdk_gpt_type_t;

typedef enum {
    /**< Set the GPT us mode.  */
    GSDK_GPT_US = 0,
    /**< Set the GPT ms  mode.  */
    GSDK_GPT_MS = 1,
} gsdk_gpt_um_t;

typedef struct {
    gsdk_gpt_type_t timer_type;
    gsdk_gpt_um_t   um;
    uint32_t        timer_out;
    void (*function)(void *);
    void    *data;
}gpt_config_t;

typedef struct _gsdk_gpt {
    uint32_t            magic;
    gpt_port_t          port;
    gpt_config_t        config;
} gsdk_gpt_t;

gsdk_status_t gsdk_gpt_timer_open(gpt_port_t port, gpt_config_t *config, gsdk_handle_t *pgpt);
gsdk_status_t gsdk_gpt_timer_close(gsdk_handle_t hgpt);
gsdk_status_t gsdk_gpt_timer_stop(gsdk_handle_t hgpio);
gsdk_status_t gsdk_gpt_timer_start(gsdk_handle_t hgpio);
#endif
