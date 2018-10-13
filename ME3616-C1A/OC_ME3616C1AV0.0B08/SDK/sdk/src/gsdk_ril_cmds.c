/*
** File   : gsdk_ril_cmds.h
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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "gsdk_api.h"
#include "gsdk_ril.h"
#include "gsdk_ril_cmds.h"
#include "at_tok.h"

#define APP_LOGLEVEL 9
#define APP_LOGTAG "RIL"
#include "gsdk_log.h"
#define MAX_AT_COUNT 1400
#define MAX_REGISTER_SIZE 1024

static unsolicited_handler_t handler;
static char g_register_buf[MAX_REGISTER_SIZE] = {0};

#define INIT_RESPONSE_BUF(resp,buf,len) \
{ \
  memset(&resp, 0, sizeof(at_response_t)); \
  resp.intermediates_buf = buf; \
  resp.intermediates_size = len; \
}

//Added by yanghaojun20180418 for RIL interface begin
static gsdk_status_t gsdk_ril_send_singleline_and_get_res(char *cmd, char *prefix, char *res_buf, int res_buf_len)
{
    at_response_t response;
    gsdk_status_t status = GSDK_ERROR;
    int buf_shift = 0;
    char line_buf[64] = {0};

    if (NULL == res_buf) {
        return GSDK_ERROR_INVALID_PARAMETER;
    }

    INIT_RESPONSE_BUF(response, line_buf, 64);
    status = gsdk_at_command_singleline(cmd, prefix, &response);
    if (status != GSDK_SUCCESS)
        return status;
    if (response.success == 0)
        return GSDK_ERROR;
    if (!response.p_intermediates)
        return GSDK_ERROR;

    if (prefix) {
        buf_shift = strlen(prefix);
        while (response.p_intermediates->line[buf_shift] == ' ') {
            ++buf_shift;
        }
    }

    strncpy(res_buf, response.p_intermediates->line + buf_shift, res_buf_len);

    return GSDK_SUCCESS;
}


gsdk_status_t gsdk_ril_get_manufacturer(char *manufacturer, int len)
{
    return gsdk_ril_send_singleline_and_get_res("AT+CGMI\r\n", NULL, manufacturer, len);
}

gsdk_status_t gsdk_ril_get_module_id(char *id, int len)
{
    return gsdk_ril_send_singleline_and_get_res("AT+CGMM\r\n", NULL, id, len);
}

gsdk_status_t gsdk_ril_get_soft_version(char *soft_version, int len)
{
    return gsdk_ril_send_singleline_and_get_res("AT+CGMR\r\n", NULL, soft_version, len);
}

gsdk_status_t gsdk_ril_get_imei(char *imei, int len)
{
    return gsdk_ril_send_singleline_and_get_res("AT+CGSN=1\r\n", NULL, imei, len);
}

gsdk_status_t gsdk_ril_get_imsi(char *imsi, int len)
{
    return gsdk_ril_send_singleline_and_get_res("AT+CIMI\r\n", NULL, imsi, len);
}


gsdk_status_t gsdk_ril_get_pcb_version(char *pcb_version, int len)
{
    return gsdk_ril_send_singleline_and_get_res("AT+ZPCB?\r\n", "+ZPCB:", pcb_version, len);
}

gsdk_status_t gsdk_ril_set_phone_functionality(int func_code)
{
    char cmd_buf[16] = {0};
    at_response_t response ;
    gsdk_status_t status;

    memset(&response, 0, sizeof(at_response_t));
    switch (func_code) {
    case PHONE_FUNC_CLOSE_RF:
    case PHONE_FUNC_CLOSE_SIM:
    case PHONE_FUNC_FULL:
    case PHONE_FUNC_MIN:
        break;
    default:
        return GSDK_ERROR_UNSUPPORTED;
    }
    sprintf(cmd_buf, "AT+CFUN=%d\r\n", func_code);

    status = gsdk_at_command(cmd_buf, &response);
    if (status != GSDK_SUCCESS) {
        return status;
    }
    if (response.success == 0) {
        return GSDK_ERROR;
    }
    return GSDK_SUCCESS;
}

gsdk_status_t gsdk_ril_set_err_info_format(int format_code)
{
    char cmd_buf[16] = {0};
    at_response_t response ;
    gsdk_status_t status;

    memset(&response, 0, sizeof(at_response_t));
    switch (format_code) {
    case ERR_FORMAT_NONE:
    case ERR_FORMAT_DIGIT:
    case ERR_FORMAT_STRING:
        break;
    default:
        return GSDK_ERROR_UNSUPPORTED;
    }
    sprintf(cmd_buf, "AT+CMEE=%d\r\n", format_code);
    status = gsdk_at_command(cmd_buf, &response);
    if (status != GSDK_SUCCESS) {
        return status;
    }
    if (response.success == 0) {
        return GSDK_ERROR;
    }
    return GSDK_SUCCESS;
}

gsdk_status_t gsdk_ril_enter_pin_code(char *pin_code)
{
    char cmd_buf[64] = {0};
    at_response_t response ;
    gsdk_status_t status;

    memset(&response, 0, sizeof(at_response_t));
    if (NULL == pin_code) {
        return GSDK_ERROR_INVALID_PARAMETER;
    }

    sprintf(cmd_buf, "AT+CPIN=\"%s\"\r\n", pin_code);
    status = gsdk_at_command(cmd_buf, &response);
    if (status != GSDK_SUCCESS) {
        return status;
    }
    if (response.success == 0) {
        return GSDK_ERROR;
    }
    return GSDK_SUCCESS;

}

gsdk_status_t gsdk_ril_enter_puk_code(char *puk_code, char *new_pin)
{
    char cmd_buf[64] = {0};
    at_response_t response ;
    gsdk_status_t status;

    memset(&response, 0, sizeof(at_response_t));
    if ((NULL == puk_code) || (NULL == new_pin)) {
        return GSDK_ERROR_INVALID_PARAMETER;
    }

    sprintf(cmd_buf, "AT+CPIN=\"%s\",\"%s\"\r\n", puk_code, new_pin);
    status = gsdk_at_command(cmd_buf, &response);
    if (status != GSDK_SUCCESS) {
        return status;
    }
    if (response.success == 0) {
        return GSDK_ERROR;
    }
    return GSDK_SUCCESS;
}

sim_status_e gsdk_ril_get_sim_status()
{
    char sim_stat[16] = {0};

    if (GSDK_SUCCESS == gsdk_ril_send_singleline_and_get_res("AT+CPIN?\r\n", "+CPIN:", sim_stat, 16)) {
        if (!strcmp(sim_stat, "READY")) {
            return SIM_STATUS_READY;
        }

        if (!strcmp(sim_stat, "SIM PIN")) {
            return SIM_STATUS_SIM_PIN;
        }

        if (!strcmp(sim_stat, "SIM PUK")) {
            return SIM_STATUS_SIM_PUK;
        }

        if (!strcmp(sim_stat, "PH-SIM PIN")) {
            return SIM_STATUS_PH_SIM_PIN;
        }

        if (!strcmp(sim_stat, "SIM PIN2")) {
            return SIM_STATUS_SIM_PIN2;
        }

        if (!strcmp(sim_stat, "SIM PUK2")) {
            return SIM_STATUS_SIM_PUK2;
        }

        if (!strcmp(sim_stat, "PH-NET PIN")) {
            return SIM_STATUS_PH_NET_PIN;
        }

    }

    return SIM_STATUS_INVALID;
}

gsdk_status_t gsdk_ril_get_iccid(char *iccid, int len)
{
    return gsdk_ril_send_singleline_and_get_res("AT*MICCID\r\n", "*MICCID:", iccid, len);
}

gsdk_status_t gsdk_ril_get_efs_reg_status(efs_reg_state_t *reg_state)
{
    at_response_t response;
    gsdk_status_t status = GSDK_ERROR;
    char *line = NULL;
    int err = 0;
    char *temp = NULL;
    char line_buf[128] = {0};

    if (NULL == reg_state) {
        return GSDK_ERROR;
    }

    INIT_RESPONSE_BUF(response, line_buf, 128);
    status = gsdk_at_command_singleline("AT+CEREG?\r\n", "+CEREG:", &response);
    if (status != GSDK_SUCCESS)
        return status;
    if (response.success == 0)
        return GSDK_ERROR;
    if (!response.p_intermediates)
        return GSDK_ERROR;

    line = response.p_intermediates->line;
    err = at_tok_start(&line);
    if (err < 0) return GSDK_ERROR;

    err = at_tok_nextint(&line, &(reg_state->m_mode));
    if (err < 0) return GSDK_ERROR;

    err = at_tok_nextint(&line, &(reg_state->m_stat));
    if (err < 0) return GSDK_ERROR;

    if (!at_tok_hasmore(&line)) {
        return status;
    }
    err = at_tok_nextstr(&line, &temp);
    if (err < 0) return GSDK_ERROR;
    strncpy(reg_state->m_tac, temp, 8);

    if (!at_tok_hasmore(&line)) {
        return status;
    }
    err = at_tok_nextstr(&line, &temp);
    if (err < 0) return GSDK_ERROR;
    strncpy(reg_state->m_ci, temp, 12);

    if (!at_tok_hasmore(&line)) {
        return status;
    }
    err = at_tok_nextint(&line, &(reg_state->m_act));
    if (err < 0) return GSDK_ERROR;

    if (!at_tok_hasmore(&line)) {
        return status;
    }
    err = at_tok_nextstr(&line, &temp);
    if (err < 0) return GSDK_ERROR;
    strncpy(reg_state->m_rac, temp, 6);

    if (!at_tok_hasmore(&line)) {
        return status;
    }
    err = at_tok_nextint(&line, &(reg_state->m_cause_type));
    if (err < 0) return GSDK_ERROR;

    if (!at_tok_hasmore(&line)) {
        return status;
    }
    err = at_tok_nextint(&line, &(reg_state->m_reject_cause));
    if (err < 0) return GSDK_ERROR;

    if (!at_tok_hasmore(&line)) {
        return status;
    }
    err = at_tok_nextstr(&line, &temp);
    if (err < 0) return GSDK_ERROR;
    strncpy(reg_state->m_active_time, temp, 6);

    if (!at_tok_hasmore(&line)) {
        return status;
    }
    err = at_tok_nextstr(&line, &temp);
    if (err < 0) return GSDK_ERROR;
    strncpy(reg_state->m_periodic_tau, temp, 6);

    return status;
}


gsdk_status_t gsdk_ril_get_operator(char *num_op, int len)
{
    at_response_t response;
    gsdk_status_t status = GSDK_ERROR;
    char *line = NULL;
    int err = 0;
    int skip = 0;
    char *result = NULL;
    char line_buf[40] = {0};

    if (NULL == num_op) {
        return GSDK_ERROR;
    }

    INIT_RESPONSE_BUF(response, line_buf, 40);
    status = gsdk_at_command_singleline("AT+COPS=3,2;+COPS?", "+COPS:", &response);

    if (status != GSDK_SUCCESS)
        return status;
    if (response.success == 0)
        return GSDK_ERROR;
    if (!response.p_intermediates)
        return GSDK_ERROR;


    line = response.p_intermediates->line;
    err = at_tok_start(&line);
    if (err < 0) return GSDK_ERROR;
    err = at_tok_nextint(&line, &skip);
    if (err < 0) return GSDK_ERROR;

    // If we're unregistered, we may just get
    // a "+COPS: 0" response
    if (!at_tok_hasmore(&line)) {
        return GSDK_ERROR;
    }

    err = at_tok_nextint(&line, &skip);
    if (err < 0) return GSDK_ERROR;

    // a "+COPS: 0, n" response is also possible
    if (!at_tok_hasmore(&line)) {
        return GSDK_ERROR;
    }

    err = at_tok_nextstr(&line, &result);
    if (err < 0) return GSDK_ERROR;

    strncpy(num_op, result, len);

    return status;
}

gsdk_status_t gsdk_ril_get_signal_strength_f1(wireless_sig_t *sig)
{
    at_response_t response;
    gsdk_status_t status = GSDK_ERROR;
    char *line = NULL;
    int err = 0;
    char line_buf[32] = {0};

    if (NULL == sig) {
        return GSDK_ERROR;
    }

    INIT_RESPONSE_BUF(response, line_buf, 32);
    status = gsdk_at_command_singleline("AT+CSQ", "+CSQ:", &response);

    if (status != GSDK_SUCCESS)
        return status;
    if (response.success == 0)
        return GSDK_ERROR;
    if (!response.p_intermediates)
        return GSDK_ERROR;

    line = response.p_intermediates->line;
    err = at_tok_start(&line);
    if (err < 0) return GSDK_ERROR;
    err = at_tok_nextint(&line, &(sig->m_f1.m_rssi));
    if (err < 0) return GSDK_ERROR;
    err = at_tok_nextint(&line, &(sig->m_f1.m_ber));
    if (err < 0) return GSDK_ERROR;

    return status;
}

gsdk_status_t gsdk_ril_get_signal_strength_f2(wireless_sig_t *sig)
{
    at_response_t response;
    gsdk_status_t status = GSDK_ERROR;
    char *line = NULL;
    int err = 0;
    char line_buf[64] = {0};

    if (NULL == sig) {
        return GSDK_ERROR;
    }

    INIT_RESPONSE_BUF(response, line_buf, 64);
    status = gsdk_at_command_singleline("AT+CESQ", "+CESQ:", &response);

    if (status != GSDK_SUCCESS)
        return status;
    if (response.success == 0)
        return GSDK_ERROR;
    if (!response.p_intermediates)
        return GSDK_ERROR;

    line = response.p_intermediates->line;
    err = at_tok_start(&line);
    if (err < 0) return GSDK_ERROR;
    err = at_tok_nextint(&line, &(sig->m_f2.m_rxlev));
    if (err < 0) return GSDK_ERROR;
    err = at_tok_nextint(&line, &(sig->m_f2.m_ber));
    if (err < 0) return GSDK_ERROR;
    err = at_tok_nextint(&line, &(sig->m_f2.m_rscp));
    if (err < 0) return GSDK_ERROR;
    err = at_tok_nextint(&line, &(sig->m_f2.m_ecno));
    if (err < 0) return GSDK_ERROR;
    err = at_tok_nextint(&line, &(sig->m_f2.m_rsrq));
    if (err < 0) return GSDK_ERROR;
    err = at_tok_nextint(&line, &(sig->m_f2.m_rsrp));
    if (err < 0) return GSDK_ERROR;

    return status;
}

gsdk_status_t gsdk_ril_get_cell_info(cell_info_t *cell_info)
{
    at_response_t response;
    gsdk_status_t status = GSDK_ERROR;
    char *line = NULL;
    int err = 0;
    at_line_t *p_cur = NULL;
    int i = 0;
    char *temp = NULL;
    char line_buf[500] = {0};

    if (NULL == cell_info) {
        return GSDK_ERROR;
    }

    INIT_RESPONSE_BUF(response, line_buf, 500);
    status = gsdk_at_command_multiline("AT*MENGINFO=0\r\n", "*MENGINFO", &response);

    if (status != GSDK_SUCCESS)
        return status;
    if (response.success == 0)
        return GSDK_ERROR;
    if (!response.p_intermediates)
        return GSDK_ERROR;

    for (p_cur = response.p_intermediates; p_cur != NULL; p_cur = p_cur->p_next) {
        line = p_cur->line;
        if (strstr(line, "MENGINFOSC")) {
            err = at_tok_start(&line);
            if (err < 0) return GSDK_ERROR;
            err = at_tok_nextint(&line, &(cell_info->m_sc.m_earfcn));
            if (err < 0) return GSDK_ERROR;
            err = at_tok_nextint(&line, &(cell_info->m_sc.m_earfcn_offset));
            if (err < 0) return GSDK_ERROR;
            err = at_tok_nextint(&line, &(cell_info->m_sc.m_pci));
            if (err < 0) return GSDK_ERROR;
            err = at_tok_nextstr(&line, &temp);
            if (err < 0) return GSDK_ERROR;
            strncpy(cell_info->m_sc.m_cellid, temp, 12);

            if (!at_tok_hasmore(&line)) {
                continue;
            }
            err = at_tok_nextint(&line, &(cell_info->m_sc.m_rsrp));
            if (err < 0) cell_info->m_sc.m_rsrp = 0;

            if (!at_tok_hasmore(&line)) {
                continue;
            }
            err = at_tok_nextint(&line, &(cell_info->m_sc.m_rsrq));
            if (err < 0) cell_info->m_sc.m_rsrq = 0;

            if (!at_tok_hasmore(&line)) {
                continue;
            }
            err = at_tok_nextint(&line, &(cell_info->m_sc.m_rssi));
            if (err < 0) cell_info->m_sc.m_rssi = 0;

            if (!at_tok_hasmore(&line)) {
                continue;
            }
            err = at_tok_nextint(&line, &(cell_info->m_sc.m_snr));
            if (err < 0) cell_info->m_sc.m_snr = 0;

            if (!at_tok_hasmore(&line)) {
                continue;
            }
            err = at_tok_nextint(&line, &(cell_info->m_sc.m_band));
            if (err < 0) return GSDK_ERROR;

            if (!at_tok_hasmore(&line)) {
                continue;
            }
            err = at_tok_nextstr(&line, &temp);
            if (err < 0) return GSDK_ERROR;
            strncpy(cell_info->m_sc.m_tac, temp, 8);

            if (!at_tok_hasmore(&line)) {
                continue;
            }
            err = at_tok_nextint(&line, &(cell_info->m_sc.m_ecl));
            if (err < 0) cell_info->m_sc.m_ecl = 0;

            if (!at_tok_hasmore(&line)) {
                continue;
            }
            err = at_tok_nextint(&line, &(cell_info->m_sc.m_tx_pwr));
            if (err < 0) cell_info->m_sc.m_tx_pwr = 0;
        } else if (strstr(line, "MENGINFONC")) {
            err = at_tok_start(&line);
            if (err < 0) return GSDK_ERROR;
            err = at_tok_nextint(&line, &(cell_info->m_nc[i].m_earfcn));
            if (err < 0) return GSDK_ERROR;
            err = at_tok_nextint(&line, &(cell_info->m_nc[i].m_earfcn_offset));
            if (err < 0) return GSDK_ERROR;
            err = at_tok_nextint(&line, &(cell_info->m_nc[i].m_pci));
            if (err < 0) return GSDK_ERROR;
            err = at_tok_nextint(&line, &(cell_info->m_nc[i].m_rsrp));
            if (err < 0) return GSDK_ERROR;
            ++i;
        } else {
            return GSDK_ERROR;
        }
    }

    return status;
}

gsdk_status_t gsdk_ril_get_data_connection_info(data_connection_info *dc_info)
{
    at_response_t response;
    gsdk_status_t status = GSDK_ERROR;
    char *line = NULL;
    int err = 0;
    char line_buf[160] = {0};

    if (NULL == dc_info) {
        return GSDK_ERROR;
    }

    INIT_RESPONSE_BUF(response, line_buf, 160);
    status = gsdk_at_command_singleline("AT*MENGINFO=1\r\n", "*MENGINFODT:", &response);

    if (status != GSDK_SUCCESS)
        return status;
    if (response.success == 0)
        return GSDK_ERROR;
    if (!response.p_intermediates)
        return GSDK_ERROR;

    line = response.p_intermediates->line;
    err = at_tok_start(&line);
    if (err < 0) return GSDK_ERROR;
    err = at_tok_nextint(&line, &(dc_info->m_rlc_ul_bler));
    if (err < 0) return GSDK_ERROR;
    err = at_tok_nextint(&line, &(dc_info->m_rlc_dl_bler));
    if (err < 0) return GSDK_ERROR;
    err = at_tok_nextint(&line, &(dc_info->m_mac_ul_bler));
    if (err < 0) return GSDK_ERROR;
    err = at_tok_nextint(&line, &(dc_info->m_mac_dl_bler));
    if (err < 0) return GSDK_ERROR;
    err = at_tok_nextint(&line, &(dc_info->m_mac_ul_total_bytes));
    if (err < 0) return GSDK_ERROR;
    err = at_tok_nextint(&line, &(dc_info->m_mac_dl_total_bytes));
    if (err < 0) return GSDK_ERROR;
    err = at_tok_nextint(&line, &(dc_info->m_mac_ul_total_harq_tx));
    if (err < 0) return GSDK_ERROR;
    err = at_tok_nextint(&line, &(dc_info->m_mac_dl_total_harq_tx));
    if (err < 0) return GSDK_ERROR;
    err = at_tok_nextint(&line, &(dc_info->m_mac_ul_harq_re_tx));
    if (err < 0) return GSDK_ERROR;
    err = at_tok_nextint(&line, &(dc_info->m_mac_dl_harq_re_tx));
    if (err < 0) return GSDK_ERROR;
    err = at_tok_nextint(&line, &(dc_info->m_rlc_ul_tput));
    if (err < 0) return GSDK_ERROR;
    err = at_tok_nextint(&line, &(dc_info->m_rlc_dl_tput));
    if (err < 0) return GSDK_ERROR;
    err = at_tok_nextint(&line, &(dc_info->m_mac_ul_tput));
    if (err < 0) return GSDK_ERROR;
    err = at_tok_nextint(&line, &(dc_info->m_mac_dl_tput));
    if (err < 0) return GSDK_ERROR;

    return status;
}

gsdk_status_t gsdk_ril_get_pdp_context(pdp_context_t *pdp_context, int cid)
{
    at_response_t response;
    gsdk_status_t status = GSDK_ERROR;
    char *line = NULL;
    int err = 0;
    char *temp = NULL;
    char cmd_buf[32];
    char *pos = NULL;
    int i = 0;
    char line_buf[160] = {0};

    if ((NULL == pdp_context) || (cid <= 0)) {
        return GSDK_ERROR;
    }

    sprintf(cmd_buf, "AT+CGCONTRDP=%d\r\n", cid);
    INIT_RESPONSE_BUF(response, line_buf, 160);
    status = gsdk_at_command_singleline(cmd_buf, "+CGCONTRDP:", &response);
    if (status != GSDK_SUCCESS)
        return status;
    if (response.success == 0)
        return GSDK_ERROR;
    if (!response.p_intermediates)
        return GSDK_ERROR;

    line = response.p_intermediates->line;
    err = at_tok_start(&line);
    if (err < 0) return GSDK_ERROR;

    err = at_tok_nextint(&line, &(pdp_context->m_cid));
    if (err < 0) return GSDK_ERROR;
    if (cid != pdp_context->m_cid) return GSDK_ERROR;

    err = at_tok_nextint(&line, &(pdp_context->m_bearer_id));
    if (err < 0) return GSDK_ERROR;

    err = at_tok_nextstr(&line, &temp);
    if (err < 0) return GSDK_ERROR;
    strncpy(pdp_context->m_apn, temp, 32);

    if (!at_tok_hasmore(&line)) {
        return status;
    }
    err = at_tok_nextstr(&line, &temp);
    if (err < 0) return GSDK_ERROR;

    pos = temp;
    for (i = 0; i < 4; ++i) {
        pos = strchr(pos, '.');
        if (NULL == pos) return GSDK_ERROR;
        ++pos;
    }
    *(pos - 1) = '\0';

    strncpy(pdp_context->m_locaddr, temp, 20);
    strncpy(pdp_context->m_submask, pos, 20);

    if (!at_tok_hasmore(&line)) {
        return status;
    }
    err = at_tok_nextstr(&line, &temp);
    if (err < 0) return GSDK_ERROR;
    strncpy(pdp_context->m_gwaddr, temp, 20);

    if (!at_tok_hasmore(&line)) {
        return status;
    }
    err = at_tok_nextstr(&line, &temp);
    if (err < 0) return GSDK_ERROR;
    strncpy(pdp_context->m_dns_prim_addr, temp, 20);

    if (!at_tok_hasmore(&line)) {
        return status;
    }
    err = at_tok_nextstr(&line, &temp);
    if (err < 0) return GSDK_ERROR;
    strncpy(pdp_context->m_dns_sec_addr, temp, 20);

    return status;
}

unsigned char gsdk_ril_is_radio_on()
{
    char buf[4] = {0};

    if (GSDK_SUCCESS == gsdk_ril_send_singleline_and_get_res("AT+CFUN?\r\n", "+CFUN:", buf, 4)) {
        if ('1' == buf[0]) {
            return 1;
        }
    }

    return 0;
}

unsigned char gsdk_ril_is_sim_ready()
{
    return ((SIM_STATUS_READY == gsdk_ril_get_sim_status()) ? (1) : (0));
}

unsigned char gsdk_ril_is_registered()
{
    efs_reg_state_t reg_state;

    memset(&reg_state, 0, sizeof(efs_reg_state_t));
    if (GSDK_SUCCESS == gsdk_ril_get_efs_reg_status(&reg_state)) {
        if ((1 == reg_state.m_stat) || (5 == reg_state.m_stat)) {
            return 1;
        }
    }

    return 0;
}

unsigned char gsdk_ril_get_local_ipaddr(char *ip_addr, int len)
{
    pdp_context_t pdp_context;

    memset(&pdp_context, 0, sizeof(pdp_context_t));

    if (GSDK_SUCCESS == gsdk_ril_get_pdp_context(&pdp_context, 1)) {
        strncpy(ip_addr, pdp_context.m_locaddr, len);
        return 1;
    }

    return 0;
}

unsigned char gsdk_ril_is_network_available()
{
    pdp_context_t pdp_context;

    memset(&pdp_context, 0, sizeof(pdp_context_t));

    pdp_context.m_cid = 1;

    if (GSDK_SUCCESS == gsdk_ril_get_pdp_context(&pdp_context, 1)) {
        return 1;
    }

    return 0;
}
//Added by yanghaojun20180418 for RIL interface end
//Added by lijiquan20180501 for setting PSM API begin
gsdk_status_t gsdk_ril_psm_set(gsdk_psm_cmd_t gsdk_psm_type, const char* gsdk_psm_cmd)
{
    gsdk_status_t status;
    at_response_t response;
    char match = 0;

    switch (gsdk_psm_type) {
    case CPSMS:
        if (strstr(gsdk_psm_cmd, "CPSMS") != NULL) {
            match = 1;
        }
        break;
    case ZSLR:
        if (strstr(gsdk_psm_cmd, "ZSLR") != NULL) {
            match = 1;
        }
        break;
    case CEDRXS:
        if (strstr(gsdk_psm_cmd, "CEDRXS") != NULL) {
            match = 1;
        }
        break;
    default:
        break;
    }

    if (match == 0) return GSDK_ERROR;

    memset(&response, 0, sizeof(at_response_t));
    status = gsdk_at_command(gsdk_psm_cmd, &response);
    if (status != GSDK_SUCCESS)
        return status;
    if (response.success == 0)
        return GSDK_ERROR;

    return GSDK_SUCCESS;
}
//Added by lijiquan20180501 for setting PSM API end

//Added by yanghaojun20180512 for GPS interface begin
#define GPS_UNSOL_PREFIX_COUNT (4)
static unsolicited_handler_t g_gps_ori_unsol_handler[GPS_UNSOL_PREFIX_COUNT];
gsdk_status_t gsdk_ril_set_gps_operation_origin(int op_code, unsolicited_handler_func gps_data_cb, void *userdata)
{
    char cmd_buf[16] = {0};
    gsdk_status_t res = GSDK_ERROR;
    int i = 0;
    static unsigned char s_need_reg_handle = 1;
    at_response_t response;

    switch (op_code) {
    case GPS_OP_LOCATE_ONCE:
    case GPS_OP_LOCATE_CONTINUOUSLY:
    case GPS_OP_LOCATE_STOP:
        break;
    default:
        return GSDK_ERROR_UNSUPPORTED;
    }
    sprintf(cmd_buf, "AT+ZGRUNARC=%d\r\n", op_code);
    memset(&response, 0, sizeof(at_response_t));
    res = gsdk_at_command(cmd_buf, &response);
    if (GSDK_SUCCESS != res) {
        return res;
    }
    if (response.success == 0) {
        return GSDK_ERROR;
    }
    if (op_code == GPS_OP_LOCATE_STOP) {
        for (i = 0; i < GPS_UNSOL_PREFIX_COUNT; i++) {
            gsdk_at_unregister_unsolicited_handler(g_gps_ori_unsol_handler + i);
        }
        s_need_reg_handle = 1;
    }

    if ((op_code == GPS_OP_LOCATE_ONCE) || (op_code == GPS_OP_LOCATE_CONTINUOUSLY)) {
        memset(&response, 0, sizeof(at_response_t));
        res = gsdk_at_command("AT+ZGNMEA=63\r\n", &response);
        if ((GSDK_SUCCESS == res) && s_need_reg_handle && response.success != 0) {

            g_gps_ori_unsol_handler[0].prefix = "$GP";
            g_gps_ori_unsol_handler[1].prefix = "$GB";
            g_gps_ori_unsol_handler[2].prefix = "$GN";
            g_gps_ori_unsol_handler[3].prefix = "+ZGPSR:";

            for (i = 0; i < GPS_UNSOL_PREFIX_COUNT; i++) {
                g_gps_ori_unsol_handler[i].func = gps_data_cb;
                g_gps_ori_unsol_handler[i].user_data = userdata;
                gsdk_at_register_unsolicited_handler(g_gps_ori_unsol_handler + i);
            }

            s_need_reg_handle = 0;
        }
    }

    return res;
}

static unsolicited_handler_t g_gps_unsol_handler[GPS_UNSOL_PREFIX_COUNT];
static char g_is_gps_started = 0;
gsdk_status_t gsdk_ril_set_gps_operation(int op_code, unsolicited_handler_func gps_data_cb, void *userdata)
{
    char cmd_buf[16] = {0};
    gsdk_status_t res = GSDK_ERROR;
    at_response_t response;
    int i = 0;

    switch (op_code) {
    case GPS_OP_LOCATE_ONCE:
    case GPS_OP_LOCATE_CONTINUOUSLY:
    case GPS_OP_LOCATE_STOP:
        break;
    default:
        return GSDK_ERROR_UNSUPPORTED;
    }

    sprintf(cmd_buf, "AT+ZGRUN=%d\r\n", op_code);
    memset(&response, 0, sizeof(at_response_t));
    res = gsdk_at_command(cmd_buf, &response);
    if (GSDK_SUCCESS != res || response.success == 0) {
        return GSDK_ERROR;
    }

    if (op_code == GPS_OP_LOCATE_STOP && g_is_gps_started == 1) {
        for (i = 0; i < GPS_UNSOL_PREFIX_COUNT; i++) {
            gsdk_at_unregister_unsolicited_handler(g_gps_unsol_handler + i);
        }        
        g_is_gps_started = 0;
    }

    if (((op_code == GPS_OP_LOCATE_ONCE) || (op_code == GPS_OP_LOCATE_CONTINUOUSLY)) && g_is_gps_started == 0) {
        g_gps_unsol_handler[0].prefix = "$GP";
        g_gps_unsol_handler[1].prefix = "$GB";
        g_gps_unsol_handler[2].prefix = "$GN";
        g_gps_unsol_handler[3].prefix = "+ZGPSR:";

        for (i = 0; i < GPS_UNSOL_PREFIX_COUNT; i++) {
            g_gps_unsol_handler[i].func = gps_data_cb;
            g_gps_unsol_handler[i].user_data = userdata;
            gsdk_at_register_unsolicited_handler(g_gps_unsol_handler + i);
        }        
        
        g_is_gps_started = 1;
    }

    return res;
}

/* 3mi gpsr*/
gsdk_status_t gsdk_ril_get_gps(void)
{
    char string[] = "AT+ZGPSRSM\r\n";
    gsdk_status_t ret = GSDK_ERROR;
    at_response_t response;

    memset(&response, 0, sizeof(at_response_t));
    ret = gsdk_at_command(string, &response);
    if (GSDK_SUCCESS != ret) return ret;
    if (response.success == 0) {
        ALOGD("GET GPS POS ERROR: %d\n", response.final_response);
        return GSDK_ERROR;
    }

    return GSDK_SUCCESS;
}

gsdk_status_t gsdk_ril_get_gpsr(void)
{
    char string[] = "AT+ZGPSR\r\n";
    gsdk_status_t ret = GSDK_ERROR;
    at_response_t response;

    memset(&response, 0, sizeof(at_response_t));
    ret = gsdk_at_command(string, &response);
    if (GSDK_SUCCESS != ret) return ret;
    if (response.success == 0) {
        ALOGD("GET GPS POS ERROR: %d\n", response.final_response);
        return GSDK_ERROR;
    }

    return GSDK_SUCCESS;
}

gsdk_status_t gsdk_ril_set_gps_run(int flag)
{
    at_response_t response;
    gsdk_status_t status = GSDK_ERROR;
    char cmd_buf[32] = {0};

    memset(&response, 0, sizeof(response));

    sprintf(cmd_buf, "AT+ZGRUN=%d\r\n", flag);
    status = gsdk_at_command(cmd_buf, &response);

    if (status != GSDK_SUCCESS)
        return status;
    if (response.success == 0)
        return GSDK_ERROR;

    //printf("OK\n");

    return status;
}

gsdk_status_t gsdk_ril_set_gps_mode(int flag)
{
    at_response_t response;
    gsdk_status_t status = GSDK_ERROR;
    char cmd_buf[32] = {0};

    memset(&response, 0, sizeof(response));

    sprintf(cmd_buf, "AT+ZGMODE=%d\r\n", flag);
    status = gsdk_at_command(cmd_buf, &response);

    if (status != GSDK_SUCCESS)
        return status;
    if (response.success == 0)
        return GSDK_ERROR;

    //printf("OK\n");

    return status;
}

gsdk_status_t gsdk_ril_set_gps_nmea(int flag)
{
    at_response_t response;
    gsdk_status_t status = GSDK_ERROR;
    char cmd_buf[32] = {0};

    memset(&response, 0, sizeof(response));

    sprintf(cmd_buf, "AT+ZGNMEA=%d\r\n", flag);
    status = gsdk_at_command(cmd_buf, &response);

    if (status != GSDK_SUCCESS)
        return status;
    if (response.success == 0)
        return GSDK_ERROR;

    //printf("OK\n");

    return status;
}

gsdk_status_t gsdk_ril_set_agps_auto(int enable, int interval)
{
    at_response_t response;
    gsdk_status_t status = GSDK_ERROR;
    char cmd_buf[32] = {0};

    memset(&response, 0, sizeof(response));

    sprintf(cmd_buf, "AT+ZGAUTO=%d,%d\r\n", enable, interval);
    status = gsdk_at_command(cmd_buf, &response);

    if (status != GSDK_SUCCESS)
        return status;
    if (response.success == 0)
        return GSDK_ERROR;

    //printf("OK\n");

    return status;
}

gsdk_status_t gsdk_ril_set_gps_lock(int flag)
{
    at_response_t response;
    gsdk_status_t status = GSDK_ERROR;
    char cmd_buf[32] = {0};

    memset(&response, 0, sizeof(response));

    sprintf(cmd_buf, "AT+ZGLOCK=%d\r\n", flag);
    status = gsdk_at_command(cmd_buf, &response);

    if (status != GSDK_SUCCESS)
        return status;
    if (response.success == 0)
        return GSDK_ERROR;

    //printf("OK\n");

    return status;
}

gsdk_status_t gsdk_ril_set_gps_rst(int flag)
{
    at_response_t response;
    gsdk_status_t status = GSDK_ERROR;
    char cmd_buf[32] = {0};

    memset(&response, 0, sizeof(response));

    sprintf(cmd_buf, "AT+ZGRST=%d\r\n", flag);
    status = gsdk_at_command(cmd_buf, &response);

    if (status != GSDK_SUCCESS)
        return status;
    if (response.success == 0)
        return GSDK_ERROR;

    //printf("OK\n");

    return status;
}

gsdk_status_t gsdk_ril_set_gps_timeout(int flag)
{
    at_response_t response;
    gsdk_status_t status = GSDK_ERROR;
    char cmd_buf[32] = {0};

    memset(&response, 0, sizeof(response));

    sprintf(cmd_buf, "AT+ZGTMOUT=%d\r\n", flag);
    status = gsdk_at_command(cmd_buf, &response);

    if (status != GSDK_SUCCESS)
        return status;
    if (response.success == 0)
        return GSDK_ERROR;

    //printf("OK\n");

    return status;
}

gsdk_status_t gsdk_ril_download_agnss_data(void)
{
    at_response_t response;
    gsdk_status_t status = GSDK_ERROR;

    memset(&response, 0, sizeof(response));

    status = gsdk_at_command("AT+ZGDATA\r\n", &response);
    if (status != GSDK_SUCCESS)
        return status;
    if (response.success == 0)
        return GSDK_ERROR;

    //printf("OK\n");

    return status;
}

gsdk_status_t gsdk_ril_send_agps_data_arc(void)
{
    at_response_t response;
    gsdk_status_t status = GSDK_ERROR;

    memset(&response, 0, sizeof(response));

    status = gsdk_at_command("AT+ZGSENDARC\r\n", &response);

    if (status != GSDK_SUCCESS)
        return status;
    if (response.success == 0)
        return GSDK_ERROR;

    //printf("OK\n");

    return status;
}

gsdk_status_t gsdk_ril_set_gps_run_arc(int flag)
{
    at_response_t response;
    gsdk_status_t status = GSDK_ERROR;
    char cmd_buf[32] = {0};

    memset(&response, 0, sizeof(response));

    sprintf(cmd_buf, "AT+ZGRUNARC=%d\r\n", flag);
    status = gsdk_at_command(cmd_buf, &response);

    if (status != GSDK_SUCCESS)
        return status;
    if (response.success == 0)
        return GSDK_ERROR;

    //printf("OK\n");

    return status;
}

gsdk_status_t gsdk_ril_download_agnss_data_arc(void)
{
    at_response_t response;
    gsdk_status_t status = GSDK_ERROR;

    memset(&response, 0, sizeof(response));

    status = gsdk_at_command("AT+ZGDATAARC\r\n", &response);
    if (status != GSDK_SUCCESS)
        return status;
    if (response.success == 0)
        return GSDK_ERROR;

    //printf("OK\n");

    return status;
}

gsdk_status_t gsdk_ril_get_agps_time_arc(char *time_string_get)
{
    at_response_t response;
    gsdk_status_t status = GSDK_ERROR;
    char *line = NULL;
    char buf[64] = {0};

    memset(&response, 0, sizeof(response));
    response.intermediates_buf = buf;
    response.intermediates_size = sizeof(buf);

    status = gsdk_at_command_singleline("AT+ZGDATAARC?\r\n", "+ZGPS:", &response);

    if (status != GSDK_SUCCESS)
        return status;
    if (response.success == 0)
        return GSDK_ERROR;
    if (!response.p_intermediates)
        return GSDK_ERROR;

    line = response.p_intermediates->line;

    strcpy(time_string_get, line + strlen("+ZGPS: TIME="));
    //printf("+ZGPS:%s\n", time_string_get);

    //printf("\r\n");
    //printf("OK\n");

    return status;
}
//Added by yanghaojun20180512 for GPS interface end

//Added by liyong20180521 for iot and onenet start
static int handle_func(const char* s, const char* extra, int len, void* data)
{
    (void)extra;
    (void)len;
    printf("infomation come from iot is %s\r\n", s);
    strncpy(data, s, MAX_REGISTER_SIZE - 1);
    return GSDK_AT_UNSOLICITED_NEXT;
}

static char g_call_added = 0;
void add_unsolicited_func()
{
    gsdk_status_t status;

    if (g_call_added == 0) {
        memset(&handler, 0, sizeof(unsolicited_handler_t));
        handler.prefix = "+M2MCLI:";
        handler.func = handle_func;
        handler.user_data = g_register_buf;
        status = gsdk_at_register_unsolicited_handler(&handler);
        if (status != GSDK_SUCCESS) {
            printf("some error hanppened in handle,status is %d\r\n", status);
            return ;
        }
        g_call_added = 1;
    }
}

gsdk_status_t gsdk_ril_set_link_iot(char *imei_data, char *ip, int port, int lifetime)
{
    char cmd_buf[128] = {0};
    at_response_t response ;
    gsdk_status_t status;
    int count = 0;
    int register_flag = 0;

    memset(&response, 0, sizeof(at_response_t));
    if (NULL == imei_data || strlen(imei_data) < 15 || lifetime < 0) {
        return GSDK_ERROR_INVALID_PARAMETER;
    }
    add_unsolicited_func();
    //if (add_unsolicited_func()) {
    //return GSDK_ERROR_INTERNAL;
    // }
    //snprintf(cmd_buf, 128, "AT+M2MCLINEW=180.101.147.115,5683,\"%s\",%d\r\n", imei_data, lifetime);
    snprintf(cmd_buf, 128, "AT+M2MCLINEW=%s,%d,\"%s\",%d\r\n", ip, port, imei_data, lifetime);
    status = gsdk_at_command(cmd_buf, &response);
    if (status != GSDK_SUCCESS) {
        return status;
    }
    if (response.success == 0) {
        return GSDK_ERROR;
    }

    while (1) {
        if (count > 100) {
            if (register_flag) {
                printf("only recv register information,not observe information\r\n");
                return GSDK_ERROR_UNINITIALIZED;
            } else {
                printf("wait for register information timeout\r\n");
                return GSDK_ERROR;
            }
        }
        if (0 == strncmp(g_register_buf, "+M2MCLI:register success", 24) || 0 == strncmp(g_register_buf, "+M2MCLI:observe success", 23)) {
            register_flag = 1;
        }
        if (register_flag) {
            if (0 == strncmp(g_register_buf, "+M2MCLI:observe success", 23)) {
                break;
            }
        }

        if (0 == strncmp(g_register_buf, "+M2MCLI:register failed", 23))
            return GSDK_ERROR_UNSUPPORTED;
        vTaskDelay(500 / portTICK_PERIOD_MS);
        count++;
    }
    return GSDK_SUCCESS;

}
gsdk_status_t gsdk_ril_del_link_iot()
{
    char cmd_buf[] = {"AT+M2MCLIDEL\r\n"};
    at_response_t response;
    gsdk_status_t status;
    int count = 0;

    memset(&response, 0, sizeof(at_response_t));
    status = gsdk_at_command(cmd_buf, &response);
    if (status != GSDK_SUCCESS) {
        return status;
    }
    if (response.success == 0) {
        return GSDK_ERROR;
    }

    while (1) {
        if (count > 100) {
            printf("wait for deregister information timeout\r\n");
            return GSDK_ERROR_TIMEOUT;
        }
        if (0 == strcmp(g_register_buf, "+M2MCLI:deregister success")) {
            break;
        }
        vTaskDelay(500 / portTICK_PERIOD_MS);
        count++;
    }

    return GSDK_SUCCESS;
}
gsdk_status_t gsdk_ril_send_data_to_iot(char *hex_data_to_iot)
{
    char cmd_buf[MAX_AT_COUNT] = {0};
    at_response_t response ;
    gsdk_status_t status;

    memset(&response, 0, sizeof(at_response_t));
    if (NULL == hex_data_to_iot || strlen(hex_data_to_iot) > 1400)
        return GSDK_ERROR_INVALID_PARAMETER;
    snprintf(cmd_buf, MAX_AT_COUNT, "AT+M2MCLISEND=%s\r\n", hex_data_to_iot);
    status = gsdk_at_command(cmd_buf, &response);
    if (status != GSDK_SUCCESS) {
        return status;
    }
    if (response.success == 0) {
        return GSDK_ERROR;
    }
    return GSDK_SUCCESS;
}
gsdk_status_t gsdk_ril_update_to_iot()
{
    char cmd_buf[] = {"AT+M2MCLIUPDATE\r\n"};
    at_response_t response ;
    gsdk_status_t status;

    memset(&response, 0, sizeof(at_response_t));
    status = gsdk_at_command(cmd_buf, &response);
    if (status != GSDK_SUCCESS) {
        return status;
    }
    if (response.success == 0) {
        return GSDK_ERROR;
    }
    /* while (1) {
         if (count > 100) {
             printf("wait for register update timeout\r\n");
             return -1;
         }
         if (0 == strcmp(register_buf, "+M2MCLI:register update success")) {
             break;
         }
         vTaskDelay(500 / portTICK_PERIOD_MS);
         count++;
     }*/
    return GSDK_SUCCESS;

}

//Add by liyong for disconnect RRC at 20180801
gsdk_status_t gsdk_ril_release_rrc(int mode)
{
    char cmd_buf[128] = {0};
    snprintf(cmd_buf, 128, "AT*MNBIOTRAI=%d\r\n", mode);
    at_response_t response ;
    gsdk_status_t status;

    memset(&response, 0, sizeof(at_response_t));
    status = gsdk_at_command(cmd_buf, &response);
    if (status != GSDK_SUCCESS) {
        return status;
    }
    if (response.success == 0) {
        return GSDK_ERROR;
    }

    return GSDK_SUCCESS;
}
//added by liyinhai 20150525 for wefota interface begin
gsdk_status_t gsdk_ril_fota_settv(char *fotatv)
{
    char cmd_buf[50] = {0};
    gsdk_status_t status = GSDK_ERROR;
	at_response_t response ;

	memset(&response, 0, sizeof(at_response_t));
    sprintf(cmd_buf, "AT+FOTATV=%s\r\n", fotatv);
    status = gsdk_at_command(cmd_buf, &response);
    if (status != GSDK_SUCCESS) {
        return status;
    }
    if (response.success == 0) {
        return GSDK_ERROR;
    }
    return status;

}
gsdk_status_t gsdk_ril_fota_ctr(void)
{
    gsdk_status_t status = GSDK_ERROR;
	at_response_t response ;

	memset(&response, 0, sizeof(at_response_t));
    //sprintf(cmd_buf, "AT+FOTATV=%s\r\n",fotatv);
    status = gsdk_at_command("AT+FOTACTR\r\n", &response);
    if (status != GSDK_SUCCESS) {
    	return status;
    }
    if (response.success == 0) {
        return GSDK_ERROR;
    }
    return status;
}

gsdk_status_t gsdk_ril_fotap_settv(char *fotasv, char *fotatv)
{
    char cmd_buf[50] = {0};
    gsdk_status_t status = GSDK_ERROR;
	at_response_t response ;

	memset(&response, 0, sizeof(at_response_t));

    sprintf(cmd_buf, "AT+FOTAPV=%s,%s\r\n", fotasv, fotatv);
    status = gsdk_at_command(cmd_buf, &response);
    if (status != GSDK_SUCCESS) {
    	return status;
    }
    if (response.success == 0) {
        return GSDK_ERROR;
    }
    return status;
}
gsdk_status_t gsdk_ril_fotap_ctr(void)
{
    gsdk_status_t status = GSDK_ERROR;
	at_response_t response ;

	memset(&response, 0, sizeof(at_response_t));

    //sprintf(cmd_buf, "AT+FOTATV=%s\r\n",fotatv);
    status = gsdk_at_command("AT+FOTAPCTR\r\n", &response);
    if (status != GSDK_SUCCESS) {
    	return status;
    }
    if (response.success == 0) {
        return GSDK_ERROR;
    }

    return status;
}

//added by liyinhai 20150525 for wefota interface end

// Added by chengkai20180531 for MQTT interface begin
gsdk_status_t gsdk_ril_get_mqtt_new_result(mqtt_new *mqtt_sig, int *mqtt_id)
{
    at_response_t response;
    gsdk_status_t status = GSDK_ERROR;
    char *line = NULL;
    int err = 0;
    char cmd_buf[128] = {0};
    char cid_t_buf[4] = {0};
    char buf[64];
    mqtt_new mqtt_new_example;

    memset(&response, 0x0, sizeof(at_response_t));
    response.intermediates_buf = buf;
    response.intermediates_size = sizeof(buf);
    memset(&mqtt_new_example, 0x0, sizeof(mqtt_new));

    if (NULL == mqtt_sig) {
        return GSDK_ERROR;
    }
    memcpy(&mqtt_new_example, mqtt_sig, sizeof(mqtt_new));

    sprintf(cmd_buf, "AT+EMQNEW=\"%s\",\"%s\",%u,%u", mqtt_new_example.server_t, mqtt_new_example.port_t,
            mqtt_new_example.timeout_t, mqtt_new_example.bufsize_t);

    if (mqtt_new_example.cid_t > 0) {
        sprintf(cid_t_buf, ",%d", mqtt_new_example.cid_t);
        strcat(cmd_buf, cid_t_buf);
    }

    strcat(cmd_buf, "\r\n");

    //printf("%s\n", cmd_buf);
    status = gsdk_at_command_singleline_timeout(cmd_buf, "+EMQNEW:", &response, 45 * 1000);

    if (status != GSDK_SUCCESS)
        return status;
    if (response.success == 0)
        return GSDK_ERROR;
    if (!response.p_intermediates)
        return GSDK_ERROR;

    line = response.p_intermediates->line;
    printf("%s\r\n", line);
    err = at_tok_start(&line);
    if (err < 0) return GSDK_ERROR;
    err = at_tok_nextint(&line, mqtt_id);
    if (err < 0) return GSDK_ERROR;
    //printf("OK\n");
    return status;
}

gsdk_status_t gsdk_ril_get_mqtt_con_result(mqtt_con *mqtt_sig)
{
    at_response_t response;
    gsdk_status_t status = GSDK_ERROR;
    char cmd_buf[128] = {0};
    char with_will_buf[128] = {0};
    char user_pwd_buf[64] = {0};
    mqtt_con mqtt_con_example;

    memset(&response, 0x0, sizeof(at_response_t));
    memset(&mqtt_con_example, 0x0, sizeof(mqtt_con));

    if (NULL == mqtt_sig) {
        return GSDK_ERROR;
    }

    memcpy(&mqtt_con_example, mqtt_sig, sizeof(mqtt_con));

    sprintf(cmd_buf, "AT+EMQCON=%d,%d,\"%s\",%d,%d,%d", mqtt_con_example.mqtt_id_t, mqtt_con_example.mqtt_ver_t,
            mqtt_con_example.client_id_t, mqtt_con_example.keepalive_t, mqtt_con_example.cleansession_t,
            mqtt_con_example.will_flag_t);


    if (1 == mqtt_con_example.will_flag_t) {
        sprintf(with_will_buf, ",\"%s\"", mqtt_con_example.will_options_t);
        strcat(cmd_buf, with_will_buf);
    }

    if ((strlen(mqtt_con_example.username_t) > 0) && (strlen(mqtt_con_example.password_t) > 0)) {
        sprintf(user_pwd_buf, ",\"%s\",\"%s\"", mqtt_con_example.username_t, mqtt_con_example.password_t);
        strcat(cmd_buf, user_pwd_buf);
    }

    strcat(cmd_buf, "\r\n");

    //printf("%s\n", cmd_buf);

    status = gsdk_at_command_timeout(cmd_buf, &response, 45 * 1000);

    if (status != GSDK_SUCCESS)
        return status;
    if (response.success == 0)
        return GSDK_ERROR;
    //printf("OK\n");
    return status;
}

gsdk_status_t gsdk_ril_get_mqtt_discon_result(int mqtt_id)
{
    at_response_t response;
    gsdk_status_t status = GSDK_ERROR;
    char cmd_buf[32] = {0};

    memset(&response, 0x0, sizeof(at_response_t));

    sprintf(cmd_buf, "AT+EMQDISCON=%d\r\n", mqtt_id);

    //printf("%s\n", cmd_buf);
    status = gsdk_at_command(cmd_buf, &response);

    if (status != GSDK_SUCCESS)
        return status;
    if (response.success == 0)
        return GSDK_ERROR;
    //printf("OK\n");
    return status;
}

gsdk_status_t gsdk_ril_get_mqtt_sub_result(mqtt_sub_unsub_t *mqtt_sig)
{
    at_response_t response;
    gsdk_status_t status = GSDK_ERROR;
    char cmd_buf[128] = {0};
    mqtt_sub_unsub_t mqtt_sub_example;

    memset(&response, 0x0, sizeof(at_response_t));
    memset(&mqtt_sub_example, 0x0, sizeof(mqtt_sub_unsub_t));

    if (NULL == mqtt_sig) {
        return GSDK_ERROR;
    }

    memcpy(&mqtt_sub_example, mqtt_sig, sizeof(mqtt_sub_unsub_t));

    sprintf(cmd_buf, "AT+EMQSUB=%d,\"%s\",%d\r\n", mqtt_sub_example.mqtt_id_t, mqtt_sub_example.topic_t,
            mqtt_sub_example.Qos_t);

    //printf("%s\n", cmd_buf);
    status = gsdk_at_command_timeout(cmd_buf, &response, 55 * 1000);

    if (status != GSDK_SUCCESS)
        return status;
    if (response.success == 0)
        return GSDK_ERROR;
    //printf("OK\n");
    return status;
}

gsdk_status_t gsdk_ril_get_mqtt_unsub_result(mqtt_sub_unsub_t *mqtt_sig)
{
    at_response_t response;
    gsdk_status_t status = GSDK_ERROR;
    char cmd_buf[128] = {0};
    mqtt_sub_unsub_t mqtt_unsub_example;

    memset(&response, 0x0, sizeof(at_response_t));
    memset(&mqtt_unsub_example, 0x0, sizeof(mqtt_sub_unsub_t));

    if (NULL == mqtt_sig) {
        return GSDK_ERROR;
    }

    memcpy(&mqtt_unsub_example, mqtt_sig, sizeof(mqtt_sub_unsub_t));

    sprintf(cmd_buf, "AT+EMQUNSUB=%d,\"%s\"\r\n", mqtt_unsub_example.mqtt_id_t, mqtt_unsub_example.topic_t);

    //printf("%s\n", cmd_buf);
    status = gsdk_at_command(cmd_buf, &response);

    if (status != GSDK_SUCCESS)
        return status;
    if (response.success == 0)
        return GSDK_ERROR;
    //printf("OK\n");
    return GSDK_SUCCESS;
}

gsdk_status_t gsdk_ril_get_mqtt_pub_result(mqtt_pub *mqtt_sig)
{
    at_response_t response;
    gsdk_status_t status = GSDK_ERROR;
    char cmd_buf[1400] = {0};
    mqtt_pub mqtt_pub_example;

    memset(&response, 0x0, sizeof(at_response_t));
    memset(&mqtt_pub_example, 0x0, sizeof(mqtt_pub));

    if (NULL == mqtt_sig) {
        return GSDK_ERROR;
    }

    memcpy(&mqtt_pub_example, mqtt_sig, sizeof(mqtt_pub));

    sprintf(cmd_buf, "AT+EMQPUB=%d,\"%s\",%d,%d,%d,%d,\"%s\"\r\n", mqtt_pub_example.mqtt_id_t, mqtt_pub_example.topic_t,
            mqtt_pub_example.Qos_t, mqtt_pub_example.retained_t, mqtt_pub_example.dup_t, mqtt_pub_example.message_len_t,
            mqtt_pub_example.message_t);

    //printf("%s\n", cmd_buf);
    status = gsdk_at_command_timeout(cmd_buf, &response, 55 * 1000);

    if (status != GSDK_SUCCESS)
        return status;
    if (response.success == 0)
        return GSDK_ERROR;
    //printf("OK\n");
    return status;
}

gsdk_status_t gsdk_ril_get_connected_mqtt_instance(int *out)
{
    at_response_t response;
    gsdk_status_t status = GSDK_ERROR;
    char *line = NULL;
    int err = 0;
    at_line_t *p_cur = NULL;
    int i = 5;
    char buf[64];

    memset(&response, 0x0, sizeof(at_response_t));
    response.intermediates_buf = buf;
    response.intermediates_size = sizeof(buf);

    status = gsdk_at_command_multiline_timeout("AT+EMQCON?\r\n", "+EMQCON:", &response, 55 * 1000);

    if (status != GSDK_SUCCESS)
        return status;
    if (response.success == 0)
        return GSDK_ERROR;
    if (!response.p_intermediates) {
        *out = i;
        return GSDK_SUCCESS;
    }

    for (p_cur = response.p_intermediates; p_cur != NULL; p_cur = p_cur->p_next) {
        line = p_cur->line;
        err = at_tok_start(&line);
        if (err < 0) return GSDK_ERROR;
        err = at_tok_nextint(&line, &i);
        printf("+EMQCON:%d\n", i);
        if (err < 0) return GSDK_ERROR;
    }
    //printf("OK\n");
    return status;
}
// Added by chengkai20180531 for MQTT interface end

// Added by chengkai20180617 for OneNet interface start
gsdk_status_t gsdk_ril_create_onenet_instance(int totalsize_t, char *config_t, int index_t, int currentsize_t,
        int flag_t, int *instance)
{
    int err = 0;
    char *line;
    char cmd_buf[MAX_AT_COUNT] = {0};
    at_response_t response;
    gsdk_status_t status;
    char buf[64];

    memset(&response, 0x0, sizeof(at_response_t));
    response.intermediates_buf = buf;
    response.intermediates_size = sizeof(buf);

    if (0 == totalsize_t) {
        strcpy(cmd_buf, "AT+MIPLCREATE\r\n");
    } else {
        snprintf(cmd_buf, MAX_AT_COUNT, "AT+MIPLCREATE=%d,\"%s\",%d,%d,%d\r\n", totalsize_t, config_t, index_t,
                 currentsize_t, flag_t);
    }
    //printf("%s\n", cmd_buf);
    status = gsdk_at_command_singleline(cmd_buf, "+MIPLCREATE:", &response);

    if (status != GSDK_SUCCESS)
        return status;
    if (response.success == 0)
        return GSDK_ERROR;
    if (!response.p_intermediates)
        return GSDK_ERROR;

    line = response.p_intermediates->line;
    err = at_tok_start(&line);
    if (err < 0) return GSDK_ERROR;
    err = at_tok_nextint(&line, instance);
    if (err < 0) return GSDK_ERROR;
    //printf("+MIPLCREATE:%d\n", *instance);
    //printf("OK\n");
    return GSDK_SUCCESS;
}

gsdk_status_t gsdk_ril_delete_onenet_instance(int instance)
{
    char cmd_buf[32] = {0};
    at_response_t response;
    gsdk_status_t status;

    memset(&response, 0x0, sizeof(at_response_t));
    snprintf(cmd_buf, 32, "AT+MIPLDELETE=%d\r\n", instance);
    //printf("%s\n", cmd_buf);
    status = gsdk_at_command(cmd_buf, &response);
    if (status != GSDK_SUCCESS)
        return GSDK_ERROR;
    if (response.success == 0)
        return GSDK_ERROR;
    //printf("OK\n");
    return GSDK_SUCCESS;
}

gsdk_status_t gsdk_ril_create_onenet_object(int instance, int objectid, int instancecount,
        char *instancebitmap, int attributecount, int actioncount)
{
    char cmd_buf[MAX_AT_COUNT] = {0};
    at_response_t response;
    gsdk_status_t status;

    memset(&response, 0x0, sizeof(at_response_t));
    snprintf(cmd_buf, MAX_AT_COUNT, "AT+MIPLADDOBJ=%d,%d,%d,\"%s\",%d,%d\r\n", instance, objectid,
             instancecount, instancebitmap, attributecount, actioncount);
    //printf("%s\n", cmd_buf);
    status = gsdk_at_command(cmd_buf, &response);
    if (status != GSDK_SUCCESS)
        return GSDK_ERROR;
    if (response.success == 0)
        return GSDK_ERROR;
    //printf("OK\n");
    return GSDK_SUCCESS;
}

gsdk_status_t gsdk_ril_delete_onenet_object(int instance, int objectid)
{
    char cmd_buf[32] = {0};
    at_response_t response;
    gsdk_status_t status;

    memset(&response, 0x0, sizeof(at_response_t));
    snprintf(cmd_buf, 32, "AT+MIPLDELOBJ=%d,%d\r\n", instance, objectid);
    //printf("%s\n", cmd_buf);
    status = gsdk_at_command(cmd_buf, &response);
    if (status != GSDK_SUCCESS)
        return GSDK_ERROR;
    if (response.success == 0)
        return GSDK_ERROR;
    //printf("OK\n");
    return GSDK_SUCCESS;
}

gsdk_status_t gsdk_ril_onenet_open(int instance, int lifetime)
{
    char cmd_buf[32] = {0};
    at_response_t response;
    gsdk_status_t status;

    memset(&response, 0x0, sizeof(at_response_t));
    snprintf(cmd_buf, 32, "AT+MIPLOPEN=%d,%d\r\n", instance, lifetime);
    //printf("%s\n", cmd_buf);
    status = gsdk_at_command(cmd_buf, &response);
    if (status != GSDK_SUCCESS)
        return GSDK_ERROR;
    if (response.success == 0)
        return GSDK_ERROR;
    //printf("OK\n");
    return GSDK_SUCCESS;
}

gsdk_status_t gsdk_ril_onenet_close(int instance)
{
    char cmd_buf[32] = {0};
    at_response_t response;
    gsdk_status_t status;

    memset(&response, 0x0, sizeof(at_response_t));
    snprintf(cmd_buf, 32, "AT+MIPLCLOSE=%d\r\n", instance);
    //printf("%s\n", cmd_buf);
    status = gsdk_at_command(cmd_buf, &response);
    if (status != GSDK_SUCCESS)
        return GSDK_ERROR;
    if (response.success == 0)
        return GSDK_ERROR;
    //printf("OK\n");
    return GSDK_SUCCESS;
}

gsdk_status_t gsdk_ril_onenet_update(int instance, int lifetime, int flag)
{
    char cmd_buf[64] = {0};
    at_response_t response;
    gsdk_status_t status;
    //int count = 0;

    memset(&response, 0x0, sizeof(at_response_t));
    snprintf(cmd_buf, 64, "AT+MIPLUPDATE=%d,%d,%d\r\n", instance, lifetime, flag);
    //printf("%s\n", cmd_buf);
    status = gsdk_at_command(cmd_buf, &response);
    if (status != GSDK_SUCCESS)
        return GSDK_ERROR;
    if (response.success == 0)
        return GSDK_ERROR;
    //printf("OK\n");
    return GSDK_SUCCESS;
}

gsdk_status_t gsdk_ril_rsp_onenet_read(int instance, int msgid, int result, int objected, int instanceid,
                                       int resourceid, int valuetype, int len, int value, int index, int flag)
{
    char cmd_buf[MAX_AT_COUNT] = {0};
    at_response_t response;
    gsdk_status_t status;

    memset(&response, 0x0, sizeof(at_response_t));
    snprintf(cmd_buf, MAX_AT_COUNT, "AT+MIPLREADRSP=%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\r\n", instance, msgid,
             result, objected, instanceid, resourceid, valuetype, len, value, index, flag);
    //printf("%s\n", cmd_buf);
    status = gsdk_at_command(cmd_buf, &response);
    if (status != GSDK_SUCCESS)
        return GSDK_ERROR;
    if (response.success == 0)
        return GSDK_ERROR;
    //printf("OK\n");
    return GSDK_SUCCESS;
}

gsdk_status_t gsdk_ril_rsp_onenet_write(int instance, int msgid, int result)
{
    char cmd_buf[64] = {0};
    at_response_t response;
    gsdk_status_t status;

    memset(&response, 0x0, sizeof(at_response_t));
    snprintf(cmd_buf, 64, "AT+MIPLWRITERSP=%d,%d,%d\r\n", instance, msgid, result);
    //printf("%s\n", cmd_buf);
    status = gsdk_at_command(cmd_buf, &response);
    if (status != GSDK_SUCCESS)
        return GSDK_ERROR;
    if (response.success == 0)
        return GSDK_ERROR;
    //printf("OK\n");
    return GSDK_SUCCESS;
}

gsdk_status_t gsdk_ril_rsp_onenet_execute(int instance, int msgid, int result)
{
    char cmd_buf[64] = {0};
    at_response_t response;
    gsdk_status_t status;

    memset(&response, 0x0, sizeof(at_response_t));
    snprintf(cmd_buf, 64, "AT+MIPLEXECUTERSP=%d,%d,%d\r\n", instance, msgid, result);
    //printf("%s\n", cmd_buf);
    status = gsdk_at_command(cmd_buf, &response);
    if (status != GSDK_SUCCESS)
        return GSDK_ERROR;
    if (response.success == 0)
        return GSDK_ERROR;
    //printf("OK\n");
    return GSDK_SUCCESS;
}

gsdk_status_t gsdk_ril_rsp_onenet_observe(int instance, int msgid, int result)
{
    char cmd_buf[64] = {0};
    at_response_t response;
    gsdk_status_t status;

    memset(&response, 0x0, sizeof(at_response_t));
    snprintf(cmd_buf, 64, "AT+MIPLOBSERVERSP=%d,%d,%d\r\n", instance, msgid, result);
    //printf("%s\n", cmd_buf);
    status = gsdk_at_command(cmd_buf, &response);
    if (status != GSDK_SUCCESS)
        return GSDK_ERROR;
    if (response.success == 0)
        return GSDK_ERROR;
    //printf("OK\n");
    return GSDK_SUCCESS;
}

gsdk_status_t gsdk_ril_rsp_onenet_discover(int instance, int msgid, int result, int length,
        char *valuestring)
{
    char cmd_buf[MAX_AT_COUNT] = {0};
    at_response_t response;
    gsdk_status_t status;

    memset(&response, 0x0, sizeof(at_response_t));
    snprintf(cmd_buf, MAX_AT_COUNT, "AT+MIPLDISCOVERRSP=%d,%d,%d,%d,\"%s\"\r\n", instance, msgid,
             result, length, valuestring);
    //printf("%s\n", cmd_buf);
    status = gsdk_at_command(cmd_buf, &response);
    if (status != GSDK_SUCCESS)
        return GSDK_ERROR;
    if (response.success == 0)
        return GSDK_ERROR;
    //printf("OK\n");
    return GSDK_SUCCESS;
}

gsdk_status_t gsdk_ril_rsp_onenet_parameter(int instance, int msgid, int result)
{
    char cmd_buf[64] = {0};
    at_response_t response;
    gsdk_status_t status;

    memset(&response, 0x0, sizeof(at_response_t));
    snprintf(cmd_buf, 64, "AT+MIPLPARAMETERRSP=%d,%d,%d\r\n", instance, msgid, result);
    //printf("%s\n", cmd_buf);
    status = gsdk_at_command(cmd_buf, &response);
    if (status != GSDK_SUCCESS)
        return GSDK_ERROR;
    if (response.success == 0)
        return GSDK_ERROR;
    //printf("OK\n");
    return GSDK_SUCCESS;
}

gsdk_status_t gsdk_ril_notify_onenet(int instance, int msgid, int objectid, int instanceid, int resourceid,
                                     int valuetype, int len, int value, int index, int flag)
{
    char cmd_buf[MAX_AT_COUNT] = {0};
    at_response_t response;
    gsdk_status_t status;

    memset(&response, 0x0, sizeof(at_response_t));
    snprintf(cmd_buf, MAX_AT_COUNT, "AT+MIPLNOTIFY=%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\r\n", instance, msgid, objectid,
             instanceid, resourceid, valuetype, len, value, index, flag);
    //printf("%s\n", cmd_buf);
    status = gsdk_at_command(cmd_buf, &response);
    if (status != GSDK_SUCCESS)
        return GSDK_ERROR;
    if (response.success == 0)
        return GSDK_ERROR;
    //printf("OK\n");
    return GSDK_SUCCESS;
}


gsdk_status_t gsdk_ril_get_onenet_version(void)
{
    int err = 0;
    char *line;
    char cmd_buf[16] = {"AT+MIPLVER?"};
    at_response_t response;
    gsdk_status_t status;
    char onenet_ver[16] = {0};
    char buf[64];

    memset(&response, 0x0, sizeof(at_response_t));
    response.intermediates_buf = buf;
    response.intermediates_size = sizeof(buf);
    //printf("%s\n", cmd_buf);
    status = gsdk_at_command_singleline(cmd_buf, "+MIPLVER:", &response);
    if (status != GSDK_SUCCESS)
        return status;
    if (response.success == 0)
        return GSDK_ERROR;
    if (!response.p_intermediates)
        return GSDK_ERROR;

    line = response.p_intermediates->line;
    err = at_tok_start(&line);
    if (err < 0) return GSDK_ERROR;
    err = at_tok_nextstr(&line, (char**)&onenet_ver);
    if (err < 0) return GSDK_ERROR;

    //printf("+MIPLCREATE:%s\n", onenet_ver);
    //printf("OK\n");
    return GSDK_SUCCESS;
}
// Added by chengkai20180614 for OneNet interface end

//Added by zhangsiyuan20180628 for HTTP interface begin
gsdk_status_t gsdk_ril_create_http_client(http_create *http_msg, int *httpclient_id)
{
    at_response_t response;
    char buf[64];
    gsdk_status_t status = GSDK_ERROR;
    char *line = NULL;
    int err = 0;
    char cmd_buf[256] = {0};

    http_create http_client_new;

    memset(&http_client_new, 0x0, sizeof(http_create));

    if (NULL == http_msg) {
        return GSDK_ERROR;
    }

    memcpy(&http_client_new, http_msg, sizeof(http_create));

    snprintf(cmd_buf, 256, "AT+EHTTPCREATE=%d,%d,%d,\"\"%s\",,,%d,,%d,,%d,\"", http_client_new.flag, http_client_new.total_len, http_client_new.len,
             http_client_new.host, http_client_new.server_cert_len, http_client_new.client_cert_len, http_client_new.client_pk_len);

    strcat(cmd_buf, "\n");

    printf("%s\n", cmd_buf);

    memset(&response, 0, sizeof(at_response_t));
    response.intermediates_buf = buf;
    response.intermediates_size = sizeof(buf);

    status = gsdk_at_command_singleline(cmd_buf, "+EHTTPCREAT:", &response);

    if (status != GSDK_SUCCESS) {
        printf("STATUS not success\n");
        return status;
    }

    if (response.success == 0) {
        return GSDK_ERROR;
    }
    if (!response.p_intermediates) {
        return GSDK_ERROR;
    }

    line = response.p_intermediates->line;
    err = at_tok_start(&line);
    if (err < 0) return GSDK_ERROR;
    err = at_tok_nextint(&line, httpclient_id);
    //printf("id is %d,err is %d\r\n",*httpclient_id,err);
    if (err < 0) return GSDK_ERROR;
    printf("+EHTTPCREATE:%d\n", *httpclient_id);
    printf("OK\n");
    return status;
}
gsdk_status_t gsdk_ril_connect_http_client(int httpclient_id)
{
    at_response_t response;
    gsdk_status_t status = GSDK_ERROR;
    char cmd_buf[32] = {0};

    sprintf(cmd_buf, "AT+EHTTPCON=%d", httpclient_id);

    printf("%s\r\n", cmd_buf);

    memset(&response, 0, sizeof(at_response_t));

    status = gsdk_at_command(cmd_buf, &response);

    if (status != GSDK_SUCCESS)
        return status;

    if (response.success == 0)
        return GSDK_ERROR;

    printf("OK\n");

    return status;
}
static unsolicited_handler_t http_msg_handler[6];
gsdk_status_t gsdk_ril_send_http_client(http_send *http_msg, unsolicited_handler_func http_data_cb, void *userdata)
{
    at_response_t response;
    gsdk_status_t status = GSDK_ERROR;

    char cmd_buf[128] = {0};

    http_send http_client_send;

    memset(&http_client_send, 0, sizeof(http_send));

    if (NULL == http_msg) {
        return GSDK_ERROR;
    }

    memcpy(&http_client_send, http_msg, sizeof(http_send));
    //printf("id is %d\r\n",http_client_send.httpclient_id);
    //printf("id is %d\r\n",http_msg->httpclient_id);
    sprintf(cmd_buf, "AT+EHTTPSEND=%d,%d,%d\",%d,%d,%d,\"%s\",%d,,%d,,%d,\"", http_client_send.flag, http_client_send.total_len, http_client_send.len,
            http_client_send.httpclient_id, http_client_send.method, http_client_send.path_len, http_client_send.path, http_client_send.customer_header_len,
            http_client_send.content_type_len, http_client_send.content_string_len);

    //strcat(cmd_buf,"\n");

    printf("%s\n", cmd_buf);

    memset(&response, 0, sizeof(at_response_t));

    status = gsdk_at_command(cmd_buf, &response);

    if (GSDK_SUCCESS == status) {
        http_msg_handler[0].prefix = "+EHTTP";
        http_msg_handler[1].prefix = "Content_Length:";
        http_msg_handler[2].prefix = "Accept-Ranges:";
        http_msg_handler[3].prefix = "Server";
        http_msg_handler[4].prefix = "Last-Modified:";
        http_msg_handler[5].prefix = "Content-Disposition:";
        for (int i = 0; i < 6; i++) {
            http_msg_handler[i].func = http_data_cb;
            http_msg_handler[i].user_data = userdata;
            gsdk_at_register_unsolicited_handler(http_msg_handler + i);
        }
    }

    if (status != GSDK_SUCCESS)
        return status;
    if (response.success == 0)
        return GSDK_ERROR;

    printf("OK\n");

    return status;

}
gsdk_status_t gsdk_ril_disconnect_http_client(int httpclient_id)
{
    at_response_t response;
    gsdk_status_t status = GSDK_ERROR;
    char cmd_buf[32] = {0};

    sprintf(cmd_buf, "AT+EHTTPDISCON=%d", httpclient_id);

    printf("%s\n", cmd_buf);

    status = gsdk_at_command(cmd_buf, &response);

    if (status != GSDK_SUCCESS)
        return status;
    if (response.success == 0)
        return GSDK_ERROR;

    printf("OK\n");

    return status;


}
gsdk_status_t gsdk_ril_destroy_http_client(int httpclient_id)
{
    at_response_t response;
    gsdk_status_t status = GSDK_ERROR;
    char cmd_buf[32] = {0};

    sprintf(cmd_buf, "AT+EHTTPDESTROY=%d", httpclient_id);

    printf("%s\n", cmd_buf);

    memset(&response, 0, sizeof(at_response_t));

    status = gsdk_at_command(cmd_buf, &response);

    if (status != GSDK_SUCCESS)
        return status;

    if (response.success == 0)
        return GSDK_ERROR;

    printf("OK\n");

    return status;

}
//Added by zhangsiyuan20180628 for HTTP interface end

// Added by chengkai20180617 for CoAP interface start
gsdk_status_t gsdk_ril_create_coap_client(char *ip_addr, int port, int cid, int *coap_client_id)
{
    at_response_t response;
    gsdk_status_t status = GSDK_ERROR;
    char *line = NULL;
    int err = 0;
    char cmd_buf[128] = {0};
    char buf[64];

    memset(&response, 0x0, sizeof(at_response_t));
    response.intermediates_buf = buf;
    response.intermediates_size = sizeof(buf);
    sprintf(cmd_buf, "AT+ECOAPNEW=%s,%d,%d", ip_addr, port, cid);
    //printf("\r\n");
    //printf("%s\n", cmd_buf);
    status = gsdk_at_command_singleline(cmd_buf, "+ECOAPNEW:", &response);

    if (status != GSDK_SUCCESS)
        return status;
    if (response.success == 0)
        return GSDK_ERROR;
    if (!response.p_intermediates)
        return GSDK_ERROR;

    line = response.p_intermediates->line;
    err = at_tok_start(&line);
    if (err < 0) return GSDK_ERROR;
    err = at_tok_nextint(&line, coap_client_id);
    if (err < 0) return GSDK_ERROR;

    //printf("+ECOAPNEW:%d\n", *coap_client_id);
    //printf("\r\n");
    //printf("OK\n");
    return status;
}

gsdk_status_t gsdk_ril_create_coap_server(char *ip_addr, int port, int cid, int *coap_server_id)
{
    at_response_t response;
    gsdk_status_t status = GSDK_ERROR;
    char *line = NULL;
    int err = 0;
    char cmd_buf[128] = {0};
    char buf[64];

    memset(&response, 0x0, sizeof(at_response_t));
    response.intermediates_buf = buf;
    response.intermediates_size = sizeof(buf);
    sprintf(cmd_buf, "AT+ECOAPSTA=%s,%d,%d", ip_addr, port, cid);
    //printf("%s\n", cmd_buf);
    status = gsdk_at_command_singleline(cmd_buf, "+ECOAPSTA:", &response);
    if (status != GSDK_SUCCESS)
        return status;
    if (response.success == 0)
        return GSDK_ERROR;
    if (!response.p_intermediates)
        return GSDK_ERROR;

    line = response.p_intermediates->line;
    err = at_tok_start(&line);
    if (err < 0) return GSDK_ERROR;
    err = at_tok_nextint(&line, coap_server_id);
    if (err < 0) return GSDK_ERROR;
    //printf("+ECOAPSTA:%d\n", *coap_server_id);
    //printf("OK\n");
    return status;
}

gsdk_status_t gsdk_ril_coap_client_send(int coap_client_id, int data_len, char *data)
{
    at_response_t response;
    gsdk_status_t status = GSDK_ERROR;
    char cmd_buf[MAX_AT_COUNT] = {0};

    memset(&response, 0x0, sizeof(at_response_t));
    sprintf(cmd_buf, "AT+ECOAPSEND=%d,%d,%s", coap_client_id, data_len, data);
    //printf("%s\n", cmd_buf);
    status = gsdk_at_command(cmd_buf, &response);
    if (status != GSDK_SUCCESS)
        return status;
    if (response.success == 0)
        return GSDK_ERROR;
    //printf("OK\n");
    //printf("\r\n");
    return status;
}

gsdk_status_t gsdk_ril_coap_delete(int coap_client_id)
{
    at_response_t response;
    gsdk_status_t status = GSDK_ERROR;
    char cmd_buf[32] = {0};

    memset(&response, 0x0, sizeof(at_response_t));
    sprintf(cmd_buf, "AT+ECOAPDEL=%d", coap_client_id);
    //printf("%s\n", cmd_buf);
    status = gsdk_at_command(cmd_buf, &response);
    if (status != GSDK_SUCCESS)
        return status;
    if (response.success == 0)
        return GSDK_ERROR;
    //printf("OK\n");
    return status;
}
// Added by chengkai20180617 for CoAP interface end

gsdk_status_t gsdk_ril_set_network_register_status(int network_register_mode)
{
    at_response_t response;
    gsdk_status_t status = GSDK_ERROR;
    char cmd_buf[32] = {0};

    memset(&response, 0x0, sizeof(at_response_t));
    sprintf(cmd_buf, "AT+CEREG=%d\r\n", network_register_mode);
    //printf("%s\n", cmd_buf);
    status = gsdk_at_command(cmd_buf, &response);
    if (status != GSDK_SUCCESS)
        return GSDK_ERROR;
    if (response.success == 0)
        return GSDK_ERROR;

    return GSDK_SUCCESS;
}

gsdk_status_t gsdk_ril_get_network_register_status(char *network_status_string)
{
    at_response_t response;
    gsdk_status_t status = GSDK_ERROR;
    char *line = NULL;
    int err = 0;
    char buf[64];

    memset(&response, 0x0, sizeof(at_response_t));
    response.intermediates_buf = buf;
    response.intermediates_size = sizeof(buf);
    status = gsdk_at_command_singleline("AT+CEREG?\r\n", "+CEREG:", &response);
    if (status != GSDK_SUCCESS)
        return GSDK_ERROR;
    if (response.success == 0)
        return GSDK_ERROR;
    if (!response.p_intermediates)
        return GSDK_ERROR;

    line = response.p_intermediates->line;
    err = at_tok_start(&line);
    if (err < 0) return GSDK_ERROR;
    strcpy(network_status_string, line);
    return GSDK_SUCCESS;
}

gsdk_status_t gsdk_ril_get_clock(char *time_string_get)
{
    at_response_t response;
    gsdk_status_t status = GSDK_ERROR;
    char *line = NULL;
    int err = 0;
    char buf[64];

    memset(&response, 0x0, sizeof(at_response_t));
    response.intermediates_buf = buf;
    response.intermediates_size = sizeof(buf);
    status = gsdk_at_command_singleline("AT+CCLK?\r\n", "+CCLK:", &response);
    if (status != GSDK_SUCCESS)
        return GSDK_ERROR;
    if (response.success == 0)
        return GSDK_ERROR;
    if (!response.p_intermediates)
        return GSDK_ERROR;

    line = response.p_intermediates->line;
    err = at_tok_start(&line);
    if (err < 0) return GSDK_ERROR;
    strcpy(time_string_get, line);
    //printf("+CCLK:%s\n", time_string_get);

    return GSDK_SUCCESS;
}

gsdk_status_t gsdk_ril_set_clock(char *time_string_set)
{
    at_response_t response;
    gsdk_status_t status = GSDK_ERROR;
    char cmd_buf[32] = {0};

    memset(&response, 0x0, sizeof(at_response_t));
    sprintf(cmd_buf, "AT+CCLK=\"%s\"\r\n", time_string_set);
    //printf("%s\n", cmd_buf);
    status = gsdk_at_command(cmd_buf, &response);
    if (status != GSDK_SUCCESS)
        return GSDK_ERROR;
    if (response.success == 0)
        return GSDK_ERROR;

    return GSDK_SUCCESS;
}

gsdk_status_t gsdk_ril_reset_module(void)
{
    at_response_t response;
    gsdk_status_t status = GSDK_ERROR;

    memset(&response, 0x0, sizeof(at_response_t));
    status = gsdk_at_command("AT+ZRST\r\n", &response);
    if (status != GSDK_SUCCESS)
        return GSDK_ERROR;
    if (response.success == 0)
        return GSDK_ERROR;

    return GSDK_SUCCESS;
}

gsdk_status_t gsdk_ril_set_power_on_always_down(void)
{
    at_response_t response;
    gsdk_status_t status = GSDK_ERROR;

    memset(&response, 0x0, sizeof(at_response_t));
    status = gsdk_at_command("AT+EKEYURC=7\r\n", &response);
    if (status != GSDK_SUCCESS)
        return GSDK_ERROR;
    if (response.success == 0)
        return GSDK_ERROR;

    return GSDK_SUCCESS;
}

gsdk_status_t gsdk_ril_enable_sleep_mode(void)
{
    at_response_t response;
    gsdk_status_t status = GSDK_ERROR;

    memset(&response, 0x0, sizeof(at_response_t));
    status = gsdk_at_command("AT+ZSLR\r\n", &response);
    if (status != GSDK_SUCCESS)
        return GSDK_ERROR;
    if (response.success == 0)
        return GSDK_ERROR;

    return GSDK_SUCCESS;
}

gsdk_status_t gsdk_ril_set_app_boot_mode(int boot_mode)
{
    char cmd_buf[32] = {0};
    at_response_t response;
    gsdk_status_t status = GSDK_ERROR;
    char buf[64];

    memset(&response, 0x0, sizeof(at_response_t));
    response.intermediates_buf = buf;
    response.intermediates_size = sizeof(buf);
    sprintf(cmd_buf, "AT+ZBOOTAPPAUTO=%d\r\n", boot_mode);
    status = gsdk_at_command(cmd_buf, &response);
    if (status != GSDK_SUCCESS)
        return GSDK_ERROR;
    if (response.success == 0)
        return GSDK_ERROR;

    return GSDK_SUCCESS;
}

gsdk_status_t gsdk_ril_get_app_boot_mode(int *boot_mode)
{
    at_response_t response;
    gsdk_status_t status = GSDK_ERROR;
    char *line = NULL;
    int err = 0;
    char buf[64];

    memset(&response, 0x0, sizeof(at_response_t));
    response.intermediates_buf = buf;
    response.intermediates_size = sizeof(buf);
    status = gsdk_at_command_singleline("AT+ZBOOTAPPAUTO?", "+ZBOOTAPPAUTO:", &response);
    if (status != GSDK_SUCCESS)
        return GSDK_ERROR;
    if (response.success == 0)
        return GSDK_ERROR;
    if (!response.p_intermediates)
        return GSDK_ERROR;

    line = response.p_intermediates->line;
    err = at_tok_start(&line);
    if (err < 0) return GSDK_ERROR;
    err = at_tok_nextint(&line, boot_mode);
    if (err < 0) return GSDK_ERROR;

    return GSDK_SUCCESS;
}

gsdk_status_t gsdk_ril_set_cfun_mode(int cfun_mode)
{
    at_response_t response;
    gsdk_status_t status;
    char cmd_buf[32] = {0};

    memset(&response, 0x0, sizeof(at_response_t));
    sprintf(cmd_buf, "AT+CFUN=%d\r\n", cfun_mode);
    status = gsdk_at_command(cmd_buf, &response);
    if (status != GSDK_SUCCESS)
        return GSDK_ERROR;
    if (response.success == 0)
        return GSDK_ERROR;

    return GSDK_SUCCESS;
}

gsdk_status_t gsdk_ril_get_cfun_mode(int *cfun_mode)
{
    at_response_t response;
    gsdk_status_t status = GSDK_ERROR;
    char *line = NULL;
    int err = 0;
    char buf[64];

    memset(&response, 0x0, sizeof(at_response_t));
    response.intermediates_buf = buf;
    response.intermediates_size = sizeof(buf);
    status = gsdk_at_command_singleline("AT+CFUN?", "+CFUN:", &response);
    if (status != GSDK_SUCCESS)
        return GSDK_ERROR;
    if (response.success == 0)
        return GSDK_ERROR;
    if (!response.p_intermediates)
        return GSDK_ERROR;

    line = response.p_intermediates->line;
    err = at_tok_start(&line);
    if (err < 0) return GSDK_ERROR;
    err = at_tok_nextint(&line, cfun_mode);
    if (err < 0) return GSDK_ERROR;

    return GSDK_SUCCESS;
}

gsdk_status_t gsdk_ril_set_search_network_mode(void)
{
    at_response_t response;
    gsdk_status_t status = GSDK_ERROR;
    char cmd_buf[128] = {0};
    int i;
    char buf[64];

    memset(&response, 0x0, sizeof(at_response_t));
    response.intermediates_buf = buf;
    response.intermediates_size = sizeof(buf);
    for (i = 1; i < 7; i++) {
        sprintf(cmd_buf, "AT*MNVMW=1,\"NVDM_MODEM_CFG\",\"RECOVERY_SEARCH_STEP_%d\",0,2", i);
        if (i == 1 || i == 2 || i == 4) {
            strcat(cmd_buf, "\"0001\"");
        } else if (i == 3 || i == 5 || i == 6) {
            strcat(cmd_buf, "\"8001\"");
        }
        //printf("%s\r\n", cmd_buf);
        status = gsdk_at_command_singleline(cmd_buf, "*MNVMW:", &response);
        if (status != GSDK_SUCCESS)
            return GSDK_ERROR;
        if (response.success == 0)
            return GSDK_ERROR;
        if (!response.p_intermediates)
            return GSDK_ERROR;
    }
    return GSDK_SUCCESS;
}

gsdk_status_t gsdk_ril_get_search_network_mode(void)
{
    at_response_t response;
    gsdk_status_t status = GSDK_ERROR;
    char cmd_buf[128] = {0};
    char *line = NULL;
    int err = 0;
    int i;
    char buf[128];

    memset(&response, 0x0, sizeof(at_response_t));
    response.intermediates_buf = buf;
    response.intermediates_size = sizeof(buf);
    for (i = 1; i < 7; i++) {
        sprintf(cmd_buf, "AT*MNVMR=1,\"NVDM_MODEM_CFG\",\"RECOVERY_SEARCH_STEP_%d\",2", i);
        printf("%s\r\n", cmd_buf);
        status = gsdk_at_command_singleline(cmd_buf, "*MNVMR:", &response);
        if (status != GSDK_SUCCESS)
            return GSDK_ERROR;
        if (response.success == 0)
            return GSDK_ERROR;
        if (!response.p_intermediates)
            return GSDK_ERROR;

        line = response.p_intermediates->line;
        err = at_tok_start(&line);
        if (err < 0) return GSDK_ERROR;
        printf("%s\r\n", line);
    }
    return GSDK_SUCCESS;
}

gsdk_status_t gsdk_ril_set_hardware_reboot(void)
{
    at_response_t response;
    gsdk_status_t status;

    memset(&response, 0x0, sizeof(at_response_t));
    status = gsdk_at_command("AT+ZHWRST", &response);
    if (status != GSDK_SUCCESS)
        return GSDK_ERROR;
    if (response.success == 0)
        return GSDK_ERROR;

    return GSDK_SUCCESS;
}

gsdk_status_t gsdk_ril_get_tcpbuf(int socket_id, char* tcpbuf)
{
    at_response_t response;
    gsdk_status_t status = GSDK_ERROR;
    char *line = NULL;
    char cmd_buf[40] = {0};
    char buf[100] = {0};

    memset(&response, 0x0, sizeof(at_response_t));
    response.intermediates_buf = buf;
    response.intermediates_size = sizeof(buf);
    sprintf(cmd_buf, "AT+ESOTCPBUF=%d\r\n", socket_id);

    status = gsdk_at_command_singleline(cmd_buf, "+ESOTCPBUF", &response);

    if (status != GSDK_SUCCESS)
        return status;
    if (response.success == 0)
        return GSDK_ERROR;
    if (!response.p_intermediates)
        return GSDK_ERROR;

    line = response.p_intermediates->line;

    strcpy(tcpbuf, line + strlen("+ESOTCPBUF="));
    printf("+ESOTCPBUF:%s\n", tcpbuf);

    return status;
}

gsdk_status_t gsdk_ril_write_nv(char* group_name, char* data_item_name, char* str_value, int len)
{
    at_response_t response;
    gsdk_status_t status = GSDK_ERROR;
    char cmd_buf[1500] = {0};

    memset(&response, 0x0, sizeof(at_response_t));
    sprintf(cmd_buf, "AT+ENVDM=1,%s,%s,%d,%s\r\n", group_name, data_item_name, len, str_value);
    status = gsdk_at_command(cmd_buf, &response);

    if (status != GSDK_SUCCESS)
        return status;
    if (response.success == 0)
        return GSDK_ERROR;
    return status;
}

gsdk_status_t gsdk_ril_read_nv(char* group_name, char* data_item_name, char* ret_string)
{
    at_response_t response;
    gsdk_status_t status = GSDK_ERROR;
    char *line = NULL;
    char cmd_buf[100] = {0};
    char buf[1500];

    memset(&response, 0x0, sizeof(at_response_t));
    response.intermediates_buf = buf;
    response.intermediates_size = sizeof(buf);
    sprintf(cmd_buf, "AT+ENVDM=0,%s,%s\r\n", group_name, data_item_name);

    status = gsdk_at_command_singleline(cmd_buf, "+ENVDM:", &response);

    if (status != GSDK_SUCCESS)
        return status;
    if (!response.p_intermediates)
        return GSDK_ERROR;

    line = response.p_intermediates->line;

    strcpy(ret_string, line + strlen("+ENVDM: "));
    printf("+ENVDM: %s\n", ret_string);

    return status;
}

gsdk_status_t gsdk_ril_delete_nv(char* group_name, char* data_item_name)
{
    at_response_t response;
    gsdk_status_t status = GSDK_ERROR;
    char cmd_buf[100] = {0};

    memset(&response, 0x0, sizeof(at_response_t));
    if (data_item_name == NULL) {
        sprintf(cmd_buf, "AT+ENVDM=3,%s\r\n", group_name);
    } else {
        sprintf(cmd_buf, "AT+ENVDM=3,%s,%s\r\n", group_name, data_item_name);
    }
    status = gsdk_at_command(cmd_buf, &response);

    if (status != GSDK_SUCCESS)
        return status;
    if (response.success == 0)
        return GSDK_ERROR;
    return status;
}

gsdk_status_t gsdk_ril_set_sleep_mode(int sleep_mode)
{
    at_response_t response;
    gsdk_status_t status = GSDK_ERROR;
    char cmd_buf[32] = {0};

    memset(&response, 0x0, sizeof(at_response_t));
    sprintf(cmd_buf, "AT+ZSLR=%d\r\n", sleep_mode);
    status = gsdk_at_command(cmd_buf, &response);
    if (status != GSDK_SUCCESS) {
        return GSDK_ERROR;
    }
    if (response.success == 0) {
        return GSDK_ERROR;
    }
    return GSDK_SUCCESS;
}

gsdk_status_t gsdk_ril_get_sleep_mode(char *sleep_mode)
{
    at_response_t response;
    gsdk_status_t status = GSDK_ERROR;
    char *line = NULL;
    char buf[20];

    memset(&response, 0x0, sizeof(at_response_t));
    response.intermediates_buf = buf;
    response.intermediates_size = sizeof(buf);
    status = gsdk_at_command_singleline("AT+ZSLR?", "+ZSLR:", &response);
    if (status != GSDK_SUCCESS) {
        return status;
    }
    if (response.success == 0) {
        return GSDK_ERROR;
    }
    if (!response.p_intermediates) {
        return GSDK_ERROR;
    }
    line = response.p_intermediates->line;
    strcpy(sleep_mode, line + strlen("+ZSLR:"));
    //printf("+ZSLR:%s\r\n", sleep_mode);

    return status;
}

gsdk_status_t gsdk_ril_get_identification_info(char * ident_string)
{
    at_response_t response;
    gsdk_status_t status = GSDK_ERROR;
    char *line = NULL;
    at_line_t *p_cur = NULL;
    char line_buf[500] = {0};

    INIT_RESPONSE_BUF(response, line_buf, 500);
    status = gsdk_at_command_multiline("ATI", "", &response);
    if (status != GSDK_SUCCESS) {
        return GSDK_ERROR;
    }
    if (response.success == 0) {
        return GSDK_ERROR;
    }
    if (!response.p_intermediates) {
        return GSDK_ERROR;
    }
    for (p_cur = response.p_intermediates; p_cur != NULL; p_cur = p_cur->p_next) {
        line = p_cur->line;
        //printf("%s\r\n", line);
        strcat(ident_string, line);
        strcat(ident_string, "\r\n");
    }
    return GSDK_SUCCESS;
}

