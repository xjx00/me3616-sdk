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

#ifndef _GSDK_RIL_CMD_H_
#define _GSDK_RIL_CMD_H_

//Added by yanghaojun20180418 for RIL interface begin

#define PHONE_FUNC_MIN (0)
#define PHONE_FUNC_FULL (1)
#define PHONE_FUNC_CLOSE_RF (4)
#define PHONE_FUNC_CLOSE_SIM (7)

#define ERR_FORMAT_NONE (0)
#define ERR_FORMAT_DIGIT (1)
#define ERR_FORMAT_STRING (2)

//UNUSED_PARAM is used to eliminate compilation warnings
#ifndef UNUSED_PARAM
#define UNUSED_PARAM(x) ((void)(x))
#endif /* #ifndef UNUSED_PARAM */

typedef struct
{
  int m_mode;
  int m_stat;
  char m_tac[8];
  char m_ci[12];
  int m_act;
  char m_rac[6];
  int m_cause_type;
  int m_reject_cause;
  char m_active_time[12];
  char m_periodic_tau[12];
} efs_reg_state_t;

typedef union
{
  struct
  {
    int m_rssi;
    int m_ber;
  } m_f1;
  struct
  {
    int m_rxlev;
    int m_ber;
    int m_rscp;
    int m_ecno;
    int m_rsrq;
    int m_rsrp;
  } m_f2;
} wireless_sig_t;

typedef struct
{
  int m_cid;
  int m_bearer_id;
  char m_apn[32];
  char m_locaddr[20];
  char m_submask[20];
  char m_gwaddr[20];
  char m_dns_prim_addr[20];
  char m_dns_sec_addr[20];
} pdp_context_t;

#define MAX_NEIGHBOR_CELL_COUNT (4)
typedef struct
{
  struct
  {
    int m_earfcn;
    int m_earfcn_offset;
    int m_pci;
    char m_cellid[12];
    int m_rsrp;
    int m_rsrq;
    int m_rssi;
    int m_snr;
    int m_band;
    char m_tac[8];
    int m_ecl;
    int m_tx_pwr;
  } m_sc;
  struct
  {
    int m_earfcn;
    int m_earfcn_offset;
    int m_pci;
    int m_rsrp;
  } m_nc[MAX_NEIGHBOR_CELL_COUNT];
} cell_info_t;

typedef struct
{
  int m_rlc_ul_bler;
  int m_rlc_dl_bler;
  int m_mac_ul_bler;
  int m_mac_dl_bler;
  int m_mac_ul_total_bytes;
  int m_mac_dl_total_bytes;
  int m_mac_ul_total_harq_tx;
  int m_mac_dl_total_harq_tx;
  int m_mac_ul_harq_re_tx;
  int m_mac_dl_harq_re_tx;
  int m_rlc_ul_tput;
  int m_rlc_dl_tput;
  int m_mac_ul_tput;
  int m_mac_dl_tput;
} data_connection_info;

typedef enum
{
  SIM_STATUS_INVALID,
  SIM_STATUS_READY,
  SIM_STATUS_SIM_PIN,
  SIM_STATUS_SIM_PUK,
  SIM_STATUS_PH_SIM_PIN,
  SIM_STATUS_SIM_PIN2,
  SIM_STATUS_SIM_PUK2,
  SIM_STATUS_PH_NET_PIN,
  SIM_STATUS_MAX
} sim_status_e;
//Added by yanghaojun20180418 for RIL interface end


//Added chengkai20180531 for MQTT interface begin
typedef struct {
    char *server_t;
    char *port_t;
    unsigned int timeout_t;
    unsigned int bufsize_t;
    int cid_t;

} mqtt_new;

typedef struct {
    int mqtt_id_t;
    int mqtt_ver_t;
    char *client_id_t;
    int keepalive_t;
    int cleansession_t;
    int will_flag_t;
    char *will_options_t;
    char *username_t;
    char *password_t;

} mqtt_con;

typedef struct {
    int mqtt_id_t;
    char *topic_t;
    int Qos_t;
    int retained_t;
    int dup_t;
    int message_len_t;
    char *message_t;

} mqtt_pub;

typedef struct {
    int mqtt_id_t;
    char *topic_t;
    int Qos_t;

} mqtt_sub_unsub_t;
//Added chengkai20180531 for MQTT interface end


gsdk_status_t gsdk_ril_get_imei(char *imei, int len);
gsdk_status_t gsdk_ril_get_module_id(char *id, int len);
//Added by yanghaojun20180418 for RIL interface begin
gsdk_status_t gsdk_ril_get_manufacturer(char *manufacturer, int len);
gsdk_status_t gsdk_ril_get_soft_version(char *soft_version, int len);
gsdk_status_t gsdk_ril_get_imsi(char *imsi, int len);
gsdk_status_t gsdk_ril_get_pcb_version(char *pcb_version, int len);
gsdk_status_t gsdk_ril_set_phone_functionality(int func_code);
gsdk_status_t gsdk_ril_set_err_info_format(int format_code);
gsdk_status_t gsdk_ril_enter_pin_code(char *pin_code);
gsdk_status_t gsdk_ril_enter_puk_code(char *puk_code, char *new_pin);
sim_status_e gsdk_ril_get_sim_status();
gsdk_status_t gsdk_ril_get_iccid(char *iccid, int len);
gsdk_status_t gsdk_ril_get_efs_reg_status(efs_reg_state_t *reg_state);
gsdk_status_t gsdk_ril_get_operator(char *num_op, int len);
gsdk_status_t gsdk_ril_get_signal_strength_f1(wireless_sig_t *sig);
gsdk_status_t gsdk_ril_get_signal_strength_f2(wireless_sig_t *sig);
gsdk_status_t gsdk_ril_get_cell_info(cell_info_t *cell_info);
gsdk_status_t gsdk_ril_get_data_connection_info(data_connection_info *dc_info);
gsdk_status_t gsdk_ril_get_pdp_context(pdp_context_t *pdp_context, int cid);
unsigned char gsdk_ril_is_radio_on();
unsigned char gsdk_ril_is_sim_ready();
unsigned char gsdk_ril_is_registered();
unsigned char gsdk_ril_get_local_ipaddr(char *ip_addr, int len);
unsigned char gsdk_ril_is_network_available();
//Added by yanghaojun20180418 for RIL interface end

//Added by liyong20180521 for iot and onenet operation
gsdk_status_t gsdk_ril_set_link_iot(char *imei_data,char *ip,int port,int lifetime);
gsdk_status_t gsdk_ril_del_link_iot();
gsdk_status_t gsdk_ril_send_data_to_iot(char *user_data);
gsdk_status_t gsdk_ril_update_to_iot();

//Add by liyong for disconnect RRC at 20180801
gsdk_status_t gsdk_ril_release_rrc(int mode);

// Added by chengkai20180531 for MQTT interface begin
gsdk_status_t gsdk_ril_get_mqtt_new_result(mqtt_new *mqtt_sig, int *mqtt_id);
gsdk_status_t gsdk_ril_get_mqtt_con_result(mqtt_con *mqtt_sig);
gsdk_status_t gsdk_ril_get_mqtt_discon_result(int mqtt_id);
gsdk_status_t gsdk_ril_get_mqtt_sub_result(mqtt_sub_unsub_t *mqtt_sig);
gsdk_status_t gsdk_ril_get_mqtt_unsub_result(mqtt_sub_unsub_t *mqtt_sig);
gsdk_status_t gsdk_ril_get_mqtt_pub_result(mqtt_pub *mqtt_sig);
gsdk_status_t gsdk_ril_get_connected_mqtt_instance(int *out);
//Added by chengkai20180531 for MQTT interface end


// Added by chengkai20180614 for OneNet interface head file begin
gsdk_status_t gsdk_ril_create_onenet_instance(int totalsize_t, char *config_t, int index_t,
        int currentsize_t, int flag_t, int *instance);
gsdk_status_t gsdk_ril_delete_onenet_instance(int instance);
gsdk_status_t gsdk_ril_create_onenet_object(int instance, int objectid, int instancecount,
        char *instancebitmap, int attributecount, int actioncount);
gsdk_status_t gsdk_ril_delete_onenet_object(int instance, int objectid);
gsdk_status_t gsdk_ril_onenet_open(int instance, int lifetime);
gsdk_status_t gsdk_ril_onenet_close(int instance);
gsdk_status_t gsdk_ril_onenet_update(int instance, int lifetime, int flag);
gsdk_status_t gsdk_ril_rsp_onenet_read(int instance, int msgid, int result, int objected, int instanceid,
                                       int resourceid, int valuetype, int len, int value, int index,
                                       int flag);
gsdk_status_t gsdk_ril_rsp_onenet_write(int instance, int msgid, int result);
gsdk_status_t gsdk_ril_rsp_onenet_execute(int instance, int msgid, int result);
gsdk_status_t gsdk_ril_rsp_onenet_observe(int instance, int msgid, int result);
gsdk_status_t gsdk_ril_rsp_onenet_discover(int instance, int msgid, int result, int length, char *valuestring);
gsdk_status_t gsdk_ril_rsp_onenet_parameter(int instance, int msgid, int result);
gsdk_status_t gsdk_ril_notify_onenet(int instance, int msgid, int objectid, int instanceid, int resourceid,
                                     int valuetype, int len, int value, int index, int flag);
gsdk_status_t gsdk_ril_get_onenet_version(void);
// Added by chengkai20180614 for OneNet interface head file end

//Added by zhangsiyuan 20180723 for Http interface head file begin
typedef struct{
    int flag;
    int total_len;
    int len;
    char *host;
    char *username;
    char *password;
    int server_cert_len;
    char *server_cert;
    int client_cert_len;
    char *client_cert;
    int client_pk_len;
    char *client_pk;
}http_create;


typedef struct{
    int flag;
    int total_len;
    int len;
    int httpclient_id;
    int method;
    int path_len;
    char *path;
    int customer_header_len;
    char customer_header;
    int content_type_len;
    char *content_type;
    int content_string_len;
    char *content_string;
}http_send;
gsdk_status_t gsdk_ril_create_http_client(http_create *http_msg,int *httpclient_id);
gsdk_status_t gsdk_ril_connect_http_client(int httpclient_id);
gsdk_status_t gsdk_ril_send_http_client(http_send *http_msg, unsolicited_handler_func http_data_cb, void *userdata);
gsdk_status_t gsdk_ril_disconnect_http_client(int httpclient_id);
gsdk_status_t gsdk_ril_destroy_http_client(int httpclient_id);

//Added by zhangsiyuan 20180723 for Http interface head file end




// Added by chengkai20180618 for CoAP interface head file start
gsdk_status_t gsdk_ril_create_coap_client(char *ip_addr, int port, int cid, int *coap_client_id);
gsdk_status_t gsdk_ril_create_coap_server(char *ip_addr, int port, int cid, int *coap_server_id);
gsdk_status_t gsdk_ril_coap_client_send(int coap_client_id, int data_len, char *data);
gsdk_status_t gsdk_ril_coap_delete(int coap_client_id);
// Added by chengkai20180618 for CoAP interface head file end



//Added by lijiquan20180501 for setting PSM API begin
typedef enum
{
    CPSMS,
    CEDRXS,
    ZSLR
}gsdk_psm_cmd_t;
gsdk_status_t gsdk_ril_psm_set(gsdk_psm_cmd_t gsdk_psm_type, const char* gsdk_psm_cmd);
//Added by lijiquan20180501 for setting PSM API end

//Added by yanghaojun20180512 for GPS interface begin
#define GPS_OP_LOCATE_ONCE (1)
#define GPS_OP_LOCATE_CONTINUOUSLY (2)
#define GPS_OP_LOCATE_STOP (0)

gsdk_status_t gsdk_ril_set_gps_operation_origin(int op_code,unsolicited_handler_func gps_data_cb, void *userdata);
gsdk_status_t gsdk_ril_set_gps_operation(int op_code, unsolicited_handler_func gps_data_cb, void *userdata);
gsdk_status_t gsdk_ril_get_gps(void);
gsdk_status_t gsdk_ril_get_gpsr(void);
gsdk_status_t gsdk_ril_set_gps_run(int flag);
gsdk_status_t gsdk_ril_set_gps_mode(int flag);
gsdk_status_t gsdk_ril_set_gps_nmea(int flag);
gsdk_status_t gsdk_ril_set_agps_auto(int enbale, int interval);
gsdk_status_t gsdk_ril_set_gps_lock(int flag);
gsdk_status_t gsdk_ril_set_gps_rst(int flag);
gsdk_status_t gsdk_ril_set_gps_timeout(int flag);
gsdk_status_t gsdk_ril_download_agnss_data(void);
gsdk_status_t gsdk_ril_send_agps_data_arc(void);
gsdk_status_t gsdk_ril_set_gps_run_arc(int flag);
gsdk_status_t gsdk_ril_download_agnss_data_arc(void);
gsdk_status_t gsdk_ril_get_agps_time_arc(char *time_string_get);
//Added by yanghaojun20180512 for GPS interface end

//added by liyinhai 20180525 for wefota interface begin
gsdk_status_t gsdk_ril_fota_settv(char *fotatv);
gsdk_status_t gsdk_ril_fota_ctr(void);
gsdk_status_t gsdk_ril_fotap_settv(char *fotasv,char *fotatv); //used for apps downloading
gsdk_status_t gsdk_ril_fotap_ctr(void); //used for apps downloading
//added by liyinhai 20180525 for wefota interface end

gsdk_status_t gsdk_ril_set_network_register_status(int network_register_mode);
gsdk_status_t gsdk_ril_get_network_register_status(char *network_status_string);
gsdk_status_t gsdk_ril_get_clock(char *time_string_get);
gsdk_status_t gsdk_ril_set_clock(char *time_string_set);
gsdk_status_t gsdk_ril_reset_module(void);
gsdk_status_t gsdk_ril_set_power_on_always_down(void);
gsdk_status_t gsdk_ril_enable_sleep_mode(void);
gsdk_status_t gsdk_ril_set_app_boot_mode(int boot_mode);
gsdk_status_t gsdk_ril_get_app_boot_mode(int *boot_mode);
gsdk_status_t gsdk_ril_set_cfun_mode(int cfun_mode);
gsdk_status_t gsdk_ril_get_cfun_mode(int *cfun_mode);
gsdk_status_t gsdk_ril_set_search_network_mode(void);
gsdk_status_t gsdk_ril_get_search_network_mode(void);
gsdk_status_t gsdk_ril_set_hardware_reboot(void);
gsdk_status_t gsdk_ril_get_tcpbuf(int socket_id, char* tcpbuf);
gsdk_status_t gsdk_ril_write_nv(char* group_name, char* data_item_name, char* str_value, int len);
gsdk_status_t gsdk_ril_read_nv(char* group_name, char* data_item_name, char* ret_string);
gsdk_status_t gsdk_ril_delete_nv(char* group_name, char* data_item_name);
gsdk_status_t gsdk_ril_get_sleep_mode(char *sleep_mode);
gsdk_status_t gsdk_ril_set_sleep_mode(int sleep_mode);
gsdk_status_t gsdk_ril_get_identification_info(char * ident_string);
#endif
