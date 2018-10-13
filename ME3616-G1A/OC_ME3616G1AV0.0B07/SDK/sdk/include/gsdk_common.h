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

#ifndef _GSDK_COMMON_H_
#define _GSDK_COMMON_H_

#include <stdint.h>

#ifdef GDSK_INTERNAL
#include "gsdk_internal.h"
#endif

typedef enum {
    GSDK_ERROR_UNSUPPORTED                  = -9,
    GSDK_ERROR_UNINITIALIZED                = -8,
    GSDK_ERROR_HARDWARE                     = -7,
    GSDK_ERROR_INTERNAL                     = -6,
    GSDK_ERROR_TIMEOUT                      = -5,
    GSDK_ERROR_DEVCIE_BUSY                  = -4,
    GSDK_ERROR_INSUFFICIENT_RESOURCE        = -3,
    GSDK_ERROR_INVALID_PARAMETER            = -2,
    GSDK_ERROR                              = -1,
    GSDK_SUCCESS                            = 0, 
    GSDK_OK                                 = 0  
} gsdk_status_t;

#define GSDK_WAIT_FOREVER                   (-1)

typedef void *gsdk_handle_t;

#define NUM_ELEMS(x)            (sizeof(x)/sizeof(x[0]))
#define ALIGN(x,a)              __ALIGN_MASK(x, (a)-1)
#define __ALIGN_MASK(x,mask)    (((x)+(mask))&~(mask))

#ifndef GDSK_INTERNAL
#define RETSRAM_DATA __attribute__ ((__section__(".retsram_data")))
#endif

#endif

