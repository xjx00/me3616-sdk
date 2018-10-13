/*
** File   : gsdk_atport.h
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

#ifndef _GSDK_ATPORT_H_
#define _GSDK_ATPORT_H_

typedef enum gsdk_atport_status {
    ATPORT_STATUS_DISABLED,
    ATPORT_STATUS_ENABLED,
} gsdk_atport_status_t;

gsdk_status_t gsdk_atport_open(gsdk_handle_t *hport);
void gsdk_atport_close(gsdk_handle_t hport);
int gsdk_atport_read(gsdk_handle_t hport, uint8_t *data, int len, int timeout_ms);
int gsdk_atport_write(gsdk_handle_t hport, const uint8_t *data, int len, int timeout_ms);
gsdk_atport_status_t gsdk_atport_status(gsdk_handle_t hport);

#endif

