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

#ifndef _GSDK_LOG_H_
#define _GSDK_LOG_H_

#ifdef __cplusplus
extern "C" {
#endif

#define DUMP_PREFIX_NONE        0
#define DUMP_PREFIX_ADDRESS     1
#define DUMP_PREFIX_OFFSET      2

typedef enum {
    GSDK_LOG_DEBUG,
    GSDK_LOG_INFO,
    GSDK_LOG_WARNING,
    GSDK_LOG_ERROR
} gsdk_log_level_t;

void print_hex_dump_bytes(const char *prefix_str, int prefix_type, const void *buf, size_t len);

void gsdk_log_printf(const char *func, int line, gsdk_log_level_t level, const char *message, ...);
//void gsdk_log_vprintf(const char *func, int line, gsdk_log_level_t level, const char *message, va_list list);

#ifdef GDSK_INTERNAL

void gsdk_ilog_printf(const char *func, int line, gsdk_log_level_t level, const char *message, ...);
//void gsdk_ilog_vprintf(const char *func, int line, gsdk_log_level_t level, const char *message, va_list list);

#ifndef GSDK_LOGTAG 
#define GSDK_LOGTAG "GSDK"
#endif

#ifndef GSDK_LOGLEVEL   
#define GSDK_LOGLEVEL 1
#endif

#define GLOGE(fmt, ...)
#define GLOGW(fmt, ...)
#define GLOGI(fmt, ...)
#define GLOGD(fmt, ...)

#if GSDK_LOGLEVEL > 0
#undef GLOGE
#define GLOGE(fmt, ...)   gsdk_ilog_printf(__func__, __LINE__, GSDK_LOG_ERROR, fmt, ## __VA_ARGS__)
#endif

#if GSDK_LOGLEVEL > 1
#undef GLOGW
#define GLOGW(fmt, ...)   gsdk_ilog_printf(__func__, __LINE__, GSDK_LOG_WARNING, fmt, ## __VA_ARGS__)
#endif

#if GSDK_LOGLEVEL > 2
#undef GLOGI
#define GLOGI(fmt, ...)   gsdk_ilog_printf(__func__, __LINE__, GSDK_LOG_INFO, fmt, ## __VA_ARGS__)
#endif

#if GSDK_LOGLEVEL > 3
#undef GLOGD
#define GLOGD(fmt, ...)   gsdk_ilog_printf(__func__, __LINE__, GSDK_LOG_DEBUG, fmt, ## __VA_ARGS__)
#endif

#else

#ifndef APP_LOGTAG 
#define APP_LOGTAG "APP"
#endif

#ifndef APP_LOGLEVEL   
#define APP_LOGLEVEL 1
#endif

#define ALOGE(fmt, ...)
#define ALOGW(fmt, ...)
#define ALOGI(fmt, ...)
#define ALOGD(fmt, ...)

#if APP_LOGLEVEL > 0
#undef ALOGE
#define ALOGE(fmt, ...)   gsdk_log_printf(__func__, __LINE__, GSDK_LOG_ERROR, fmt, ## __VA_ARGS__)
#endif

#if APP_LOGLEVEL > 1
#undef ALOGW
#define ALOGW(fmt, ...)   gsdk_log_printf(__func__, __LINE__, GSDK_LOG_WARNING,   fmt, ## __VA_ARGS__)
#endif

#if APP_LOGLEVEL > 2
#undef ALOGI
#define ALOGI(fmt, ...)   gsdk_log_printf(__func__, __LINE__, GSDK_LOG_INFO,   fmt, ## __VA_ARGS__)
#endif

#if APP_LOGLEVEL > 3
#undef ALOGD
#define ALOGD(fmt, ...)   gsdk_log_printf(__func__, __LINE__, GSDK_LOG_DEBUG,   fmt, ## __VA_ARGS__)
#endif


#endif

#ifdef __cplusplus
}
#endif

#endif

