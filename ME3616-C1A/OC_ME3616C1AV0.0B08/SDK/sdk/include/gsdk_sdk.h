/*
** File   : gsdk_internal.h
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

/*
 * The definition in this file is for internal use only
 */
#ifndef _GSDK_SDK_H_
#define _GSDK_SDK_H_
	
#ifdef __cplusplus
	extern "C" {
#endif
	
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"FreeRTOS.h"
#include"semphr.h"
//#include"timer.h"
#include"task.h"
#include"event_groups.h"
#include"lwip/sockets.h"
#include"lwip/ip.h"
#include"gsdk_api.h"
#include"gsdk_ril.h"

#ifdef __cplusplus
	}
#endif
	
#endif
 
