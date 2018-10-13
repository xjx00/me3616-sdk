/*
** File   : gsdk_ril.h
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

#ifndef _GSDK_RIL_H_
#define _GSDK_RIL_H_

#include "at_tok.h"

typedef struct at_line {
    struct at_line   *p_next;
    char             line[1];
} at_line_t;

 typedef struct at_response {
    int         success;                /* true if final response indicates
                                            success (eg "OK") */
    char        final_response[64];     /* eg OK, ERROR */
    int         intermediates_size;     /* intermediate buffer size */
    char        *intermediates_buf;     /* intermediate buffer pointer  */
    at_line_t   *p_intermediates;       /* any intermediate responses */
} at_response_t;

#define GSDK_AT_UNSOLICITED_NEXT         0
#define GSDK_AT_UNSOLICITED_HANDLED      1

typedef int (*unsolicited_handler_func)(const char *s, const char *extra, int len, void *user_data);

typedef struct unsolicited_handler {
    unsolicited_handler_func        func;
    const char                      *prefix;
    void                            *user_data;
    struct unsolicited_handler      *p_next;
} unsolicited_handler_t;

gsdk_status_t gsdk_ril_init(void);
void gsdk_ril_close(void);

gsdk_status_t gsdk_at_command(const char *command,
                                at_response_t *p_response);
gsdk_status_t gsdk_at_command_singleline(      const char *command,
                                const char *response_prefix,
                                at_response_t *p_response);
gsdk_status_t gsdk_at_command_numeric(      const char *command,
                                at_response_t *p_response);
gsdk_status_t gsdk_at_command_multiline(      const char *command,
                                const char *response_prefix,
                                at_response_t *p_response);
gsdk_status_t gsdk_at_register_unsolicited_handler(unsolicited_handler_t *handler);
gsdk_status_t gsdk_at_unregister_unsolicited_handler(unsolicited_handler_t *handler);

gsdk_status_t gsdk_at_command_timeout(const char *command,
                                      at_response_t *p_response,
                                      int timeoutMs);
gsdk_status_t gsdk_at_command_singleline_timeout(const char *command,
        const char *response_prefix,
        at_response_t *p_response,
        int timeoutMs);
gsdk_status_t gsdk_at_command_numeric_timeout(const char *command,
        at_response_t *p_response,
        int timeoutMs);
gsdk_status_t gsdk_at_command_multiline_timeout(const char *command,
        const char *response_prefix,
        at_response_t *p_response,
        int timeoutMs);

#include "gsdk_ril_cmds.h"

#endif
