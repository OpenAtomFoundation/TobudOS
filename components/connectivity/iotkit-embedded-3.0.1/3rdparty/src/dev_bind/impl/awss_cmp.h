/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __AWSS_CMP_H__
#define __AWSS_CMP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
/**
 * @brief this is a network address structure, including host(ip or host name) and port.
 */
typedef struct {
    char host[16]; /**< host ip(dotted-decimal notation) or host name(string) */
    uint16_t port; /**< udp port or tcp port */
} platform_netaddr_t;

enum {
    AWSS_LC_INIT_ROUTER = 0x01,
    AWSS_LC_INIT_PAP = 0x02,
    AWSS_LC_INIT_DEV_AP = 0x04,
    AWSS_LC_INIT_SUC = 0x08,
    AWSS_LC_INIT_BIND = 0x100,
};

struct awss_cmp_couple {
    int init_stage;
    char *topic;
    void *cb;
};

struct coap_session_ctx_t {
    void *request;
    void *remote;
    char is_mcast;
};

int awss_cmp_local_init(int init_stage);
int awss_cmp_local_deinit(int force);
int awss_cmp_online_init();
int awss_cmp_online_deinit();
int awss_token_remain_time();
int awss_token_timeout();
int awss_update_token();
int awss_report_token();
int awss_stop_report_token();

int awss_cmp_coap_cancel_packet(uint16_t msgid);
int awss_cmp_coap_register_cb(char *topic, void *cb);
int awss_cmp_coap_send(void *buf, uint32_t len, void *sa, const char *uri, void *cb, uint16_t *msgid);
int awss_cmp_coap_send_resp(void *buf, uint32_t len, void *sa, const char *uri, void* req, void *cb, uint16_t *msgid, char qos);
int awss_cmp_coap_ob_send(void *buf, uint32_t len, void *sa, const char *uri, void *cb);
int awss_cmp_coap_deinit();

int awss_cmp_mqtt_register_cb(char *topic, void *cb);
int awss_cmp_mqtt_unregister_cb(char *topic);
int awss_cmp_mqtt_send(char *topic, void *pkt, int pkt_len, int qos);

int awss_release_coap_ctx(void *session);
void *awss_cpy_coap_ctx(void *request, void *remote, char mcast);

char *awss_cmp_get_coap_payload(void *request, int *payload_len);
uint8_t awss_cmp_get_coap_code(void *request);

int online_dev_bind_monitor(void *ctx, void *resource, void *remote, void *request);

void awss_enrollee_checkin(void *pcontext, void *pclient, void *msg);
void awss_online_switchap(void *pcontext, void *pclient, void *msg);
void awss_report_enrollee_reply(void *pcontext, void *pclient, void *msg);
void awss_get_cipher_reply(void *pcontext, void *pclient, void *msg);
void awss_report_token_reply(void *pcontext, void *pclient, void *msg);
int awss_cmp_mqtt_get_payload(void *mesg, char **payload, uint32_t *playload_len);

#ifdef __cplusplus
}
#endif

#endif
