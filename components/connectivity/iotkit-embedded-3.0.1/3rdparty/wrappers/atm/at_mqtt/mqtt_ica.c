/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "infra_config.h"
#include "mqtt_api.h"

#include "at_wrapper.h"
#include "at_parser.h"
#include "at_api.h"
#include "at_mqtt.h"

#define AT_ICA_MQTT_MQTTMODE        "AT+IMQTTMODE"
#define AT_ICA_MQTT_MQTTOPEN        "AT+IMQTTOPEN"
#define AT_ICA_MQTT_MQTTAUTH        "AT+IMQTTAUTH"
#define AT_ICA_MQTT_MQTTPARA        "AT+IMQTTPARA"
#define AT_ICA_MQTT_MQTTCONN        "AT+IMQTTCONN"
#define AT_ICA_MQTT_MQTTPUB         "AT+IMQTTPUB"
#define AT_ICA_MQTT_MQTTPUBIN       "AT+IMQTTPUBIN"
#define AT_ICA_MQTT_MQTTSUB         "AT+IMQTTSUB"
#define AT_ICA_MQTT_MQTTUNSUB       "AT+IMQTTUNSUB"
#define AT_ICA_MQTT_MQTTSTATE       "AT+IMQTTSTATE"
#define AT_ICA_MQTT_MQTTDISCONN     "AT+IMQTTDISCONN"
#define AT_ICA_MQTT_MQTTDBG         "AT+IMQTTDBG"

#define AT_ICA_MQTT_MQTTRCV         "+IMQTT"
#define AT_ICA_MQTT_MQTTERROR       "+CME"
#define AT_ICA_MQTT_MQTTOK          "OK"
#define AT_ICA_MQTT_MQTTRCVPUB      "+IMQTTRCVPUB"
#define AT_ICA_MQTT_MQTTRCVPUBIN    "+IMQTTRCVPUBIN"
#define AT_ICA_MQTT_MQTTPINGRSP     "+IMQTTPINGRSP"
#define AT_ICA_MQTT_MQTTAUTHRSP     "+IMQTTAUTH"
#define AT_ICA_MQTT_MQTTPUBRSP      "+IMQTTPUB"
#define AT_ICA_MQTT_MQTTSUBRSP      "+IMQTTSUB"
#define AT_ICA_MQTT_MQTTUNSUBRSP    "+IMQTTUNSUB"
#define AT_ICA_MQTT_MQTTSTATERSP    "+IMQTTSTATE"

#define AT_ICA_MQTT_POSTFIX         "\r\n"

#define AT_MQTT_WAIT_FOREVER 0xffffffffu

#define AT_MQTT_CMD_MAX_LEN             400
#define AT_MQTT_CMD_SUCCESS_RSP         "OK"
#define AT_MQTT_CMD_FAIL_RSP            "FAIL"
#define AT_MQTT_CMD_ERROR_RSP           "ERROR"
#define AT_MQTT_SUBSCRIBE_FAIL          128
#define AT_MQTT_RSP_MAX_LEN             (CONFIG_MQTT_MESSAGE_MAXLEN + CONFIG_MQTT_TOPIC_MAXLEN + 20)

#define AT_MQTT_WAIT_TIMEOUT            10*1000

#define mal_err(...)                do{HAL_Printf(__VA_ARGS__);HAL_Printf("\r\n");}while(0)

#ifdef INFRA_MEM_STATS
    #include "infra_mem_stats.h"
    #define AT_MQTT_ICA_MALLOC(size)            LITE_malloc(size, MEM_MAGIC, "mal.ica")
    #define AT_MQTT_ICA_FREE(ptr)               LITE_free(ptr)
#else
    #define AT_MQTT_ICA_MALLOC(size)            HAL_Malloc(size)
    #define AT_MQTT_ICA_FREE(ptr)               {HAL_Free((void *)ptr);ptr = NULL;}
#endif

char  at_recv_rsp_buf[AT_MQTT_CMD_MAX_LEN];

typedef enum {
    AT_MQTT_IDLE = 0,
    AT_MQTT_SEND_TYPE_SIMPLE,
    AT_MQTT_AUTH,
    AT_MQTT_SUB,
    AT_MQTT_UNSUB,
    AT_MQTT_PUB,
} at_mqtt_send_type_t;

int at_ica_mqtt_atsend(char *at_cmd, int timeout_ms);
int at_ica_mqtt_client_deinit(void);
int at_ica_mqtt_client_init(void);
int at_ica_mqtt_client_state(void);
int at_ica_mqtt_client_publish(const char *topic, int qos, const char *message);
int at_ica_mqtt_client_unsubscribe(const char *topic,
                                   unsigned int *mqtt_packet_id,
                                   int *mqtt_status);
int at_ica_mqtt_client_subscribe(const char *topic,
                                 int qos,
                                 unsigned int *mqtt_packet_id,
                                 int *mqtt_status,
                                 int timeout_ms);
int at_ica_mqtt_client_conn(char *proKey, char *devName, char *devSecret, int tlsEnable);
int at_ica_mqtt_client_auth(char *proKey, char *devName, char *devSecret, int tlsEnable);
int at_ica_mqtt_client_disconn(void);

int HAL_AT_MQTT_Init(iotx_mqtt_param_t *pInitParams)
{
    return at_ica_mqtt_client_init();
}

int HAL_AT_MQTT_Deinit()
{
    return at_ica_mqtt_client_deinit();
}

int HAL_AT_MQTT_Connect(char *proKey, char *devName, char *devSecret)
{
    return at_ica_mqtt_client_conn(proKey, devName, devSecret, 0);
}

int HAL_AT_MQTT_Disconnect(void)
{
    return at_ica_mqtt_client_disconn();
}

int HAL_AT_MQTT_Subscribe(const char *topic, int qos, unsigned int *mqtt_packet_id, int *mqtt_status, int timeout_ms)
{
    return at_ica_mqtt_client_subscribe(topic, qos, mqtt_packet_id, mqtt_status, timeout_ms);
}

int HAL_AT_MQTT_Unsubscribe(const char *topic, unsigned int *mqtt_packet_id, int *mqtt_status)
{
    return at_ica_mqtt_client_unsubscribe(topic, mqtt_packet_id, mqtt_status);
}

int HAL_AT_MQTT_Publish(const char *topic, int qos, const char *message, unsigned int msg_len)
{
    return at_ica_mqtt_client_publish(topic, qos, message);
}

int HAL_AT_MQTT_State(void)
{
    return at_ica_mqtt_client_state();
}

int HAL_AT_MQTT_Connectwifi(char *at_conn_wifi)
{
#ifdef MAL_ICA_ENABLED
    char  at_cmd[64];
    /* disconnect before connect to the network */
    if (at_ica_mqtt_client_disconn() != 0) {
        return -1;
    }

    memcpy(at_cmd, at_conn_wifi, 64);
    /* connect to the network */
    if (at_ica_mqtt_atsend(at_cmd, AT_MQTT_WAIT_FOREVER) != 0) {
        return -1;
    }

    return 0;
#endif
    return -1;
}
#ifndef PLATFORM_HAS_OS
char g_ica_rsp_buff[AT_MQTT_RSP_MAX_LEN];
#else
static char              *g_ica_rsp_buff = NULL;
#endif
static volatile int       g_mqtt_connect_state = IOTX_MC_STATE_INVALID;
static volatile at_mqtt_send_type_t   g_ica_at_response = AT_MQTT_IDLE;
static volatile int       g_at_response_result = 0;
#ifdef PLATFORM_HAS_OS
static void              *g_sem_response;
#else
int                       g_at_response = 0;
#endif
static volatile int       g_response_msg_number = 0;
static int                g_response_packetid = 0;
static int                g_response_status = 0;
static int                g_public_qos = 0;

int at_ica_mqtt_atsend(char *at_cmd, int timeout_ms);

static void at_err_callback(char *at_rsp)
{
    char *temp;
    int    data;

    temp            = strtok(at_rsp, ":");
    temp            = strtok(NULL, ":");
    if ((data = strtol(temp, NULL, 0)) == 3) {
        g_at_response_result = 0;
    } else {
        g_at_response_result = -1;
    }

#ifdef PLATFORM_HAS_OS
    /* notify the sender error; */
    HAL_SemaphorePost(g_sem_response);
#else
    g_at_response ++;
#endif
    return;
}

static void at_succ_callback(void)
{
    g_at_response_result = 0;
#ifdef PLATFORM_HAS_OS
    /* notify the sender ok; */
    HAL_SemaphorePost(g_sem_response);
#else
    g_at_response ++;
#endif
    return;
}

static void sub_callback(char *at_rsp)
{
    char  *temp;

    if (strstr(at_rsp, AT_MQTT_CMD_ERROR_RSP)) {
        g_at_response_result = -1;

#ifdef PLATFORM_HAS_OS
        /* notify the sender fail; */
        HAL_SemaphorePost(g_sem_response);
#else
    g_at_response ++;
#endif
        return;
    } else if (NULL != strstr(at_rsp, AT_ICA_MQTT_MQTTSUBRSP)) {
        /* get status/packet_id */
        if (NULL != strstr(at_rsp, ",")) {
            g_at_response_result = 0;

            temp            = strtok(at_rsp, ":");

            if (temp != NULL) {
                temp            = strtok(NULL, ",");
            } else {
                mal_err("subscribe rsp param invalid 1");
                g_at_response_result = -1;

#ifdef PLATFORM_HAS_OS
                HAL_SemaphorePost(g_sem_response);
#else
                g_at_response ++;
#endif
                return;
            }

            if (temp != NULL) {
                g_response_packetid = strtol(temp, NULL, 0);

                temp            = strtok(NULL, "\r\n");
            } else {
                mal_err("subscribe rsp param invalid 2");
                g_at_response_result = -1;

#ifdef PLATFORM_HAS_OS
                HAL_SemaphorePost(g_sem_response);
#else
                g_at_response ++;
#endif
                return;
            }

            if (temp != NULL) {
                g_response_status = strtol(temp, NULL, 0);
            } else {
                mal_err("subscribe rsp param invalid 3");
                g_at_response_result = -1;

#ifdef PLATFORM_HAS_OS
                HAL_SemaphorePost(g_sem_response);
#else
                g_at_response ++;
#endif
                return;
            }

#ifdef PLATFORM_HAS_OS
            /* notify the sender ok; */
            HAL_SemaphorePost(g_sem_response);
#else
            g_at_response ++;
#endif
        }
    }

    return;
}

static void unsub_callback(char *at_rsp)
{
    char  *temp;
    if (strstr(at_rsp, AT_MQTT_CMD_ERROR_RSP)) {
        g_at_response_result = -1;

#ifdef PLATFORM_HAS_OS
        /* notify the sender fail; */
        HAL_SemaphorePost(g_sem_response);
#else
        g_at_response ++;
#endif

        return;
    } else if (NULL != strstr(at_rsp, AT_ICA_MQTT_MQTTUNSUBRSP)) {
        /* get status/packet_id */
        if (NULL != strstr(at_rsp, ",")) {
            g_at_response_result = 0;

            temp            = strtok(at_rsp, ":");

            if (temp != NULL) {
                temp            = strtok(NULL, ",");
            } else {
                mal_err("subscribe rsp param invalid 1");

                return;
            }

            if (temp != NULL) {
                g_response_packetid = strtol(temp, NULL, 0);

                temp            = strtok(NULL, "\r\n");
            } else {
                mal_err("subscribe rsp param invalid 2");

                return;
            }

            if (temp != NULL) {
                g_response_status = strtol(temp, NULL, 0);
            } else {
                mal_err("subscribe rsp param invalid 3");

                return;
            }

            mal_err("unsub: %s", g_ica_rsp_buff);
            mal_err("unsub packetid: %d, sta: %d", g_response_packetid, g_response_status);

#ifdef PLATFORM_HAS_OS
            /* notify the sender ok; */
            HAL_SemaphorePost(g_sem_response);
#else
            g_at_response ++;
#endif
        }
    }

    return;
}

static void pub_callback(char *at_rsp)
{
    char  *temp;
    if (strstr(at_rsp, AT_MQTT_CMD_ERROR_RSP)) {
        g_at_response_result = -1;

#ifdef PLATFORM_HAS_OS
        /* notify the sender fail; */
        HAL_SemaphorePost(g_sem_response);
#else
        g_at_response ++;
#endif

        return;
    } else if (NULL != strstr(at_rsp, AT_ICA_MQTT_MQTTPUBRSP)) {
        /* get status/packet_id */
        if ((NULL != strstr(at_rsp, ","))
            || (0 == g_public_qos)) {

            temp            = strtok(at_rsp, ":");

            if (temp != NULL) {
                if (0 == g_public_qos) {
                    temp    = strtok(NULL, "\r\n");
                } else {
                    temp    = strtok(NULL, ",");
                }
            } else {
                mal_err("public get packetid error");

                return;
            }

            if (temp != NULL) {
                g_response_packetid = strtol(temp, NULL, 0);
            } else {
                mal_err("public parse packetid error");

                return;
            }

            if (0 != g_public_qos) {
                temp            = strtok(NULL, "\r\n");

                if (temp != NULL) {
                    g_response_status = strtol(temp, NULL, 0);
                } else {
                    mal_err("public parse status error");

                    return;
                }
            }

            g_at_response_result = 0;

#ifdef PLATFORM_HAS_OS
            /* notify the sender ok; */
            HAL_SemaphorePost(g_sem_response);
#else
            g_at_response ++;
#endif
        }
    }

    return;
}

static void state_change_callback(char *at_rsp)
{
    char *temp;

    if (NULL == at_rsp) {
        return;
    }

    temp = strtok(at_rsp, ":");

    if (temp != NULL) {
        temp = strtok(NULL, "\r\n");

        if (temp != NULL) {
            g_mqtt_connect_state = strtol(temp, NULL, 0);
        }
    }
    return;
}

static void recv_data_callback(char *at_rsp)
{
    char     *temp = NULL;
    char     *topic_ptr = NULL;
    char     *msg_ptr = NULL;
    unsigned int  msg_len = 0;
    struct at_mqtt_input param;

    if (NULL == at_rsp) {
        return;
    }

    /* try to get msg id */
    temp = strtok(g_ica_rsp_buff, ":");

    if (temp != NULL) {
        temp  = strtok(NULL, ",");

        if (temp != NULL) {
            /* packet_id = strtol(temp, NULL, 0); */
        } else {
            mal_err("packet id error");

            return;
        }
    } else {
        mal_err("packet id not found");

        return;
    }

    /* try to get topic string */
    temp = strtok(NULL, "\"");

    if (temp != NULL) {
        temp[strlen(temp)] = '\0';

        topic_ptr = temp;
    } else {
        mal_err("publish topic not found");

        return;
    }

    /* try to get payload string */
    temp = strtok(NULL, ",");

    if (temp != NULL) {
        msg_len = strtol(temp, NULL, 0);

        while (*temp++ != '\"');

        msg_ptr = temp;

        msg_ptr[msg_len] = '\0';

        param.topic = topic_ptr;
        param.topic_len = strlen(topic_ptr);
        param.message = msg_ptr;
        param.msg_len = strlen(msg_ptr);

        if (IOT_ATM_Input(&param) != 0) {
            mal_err("hand data to uplayer fail!\n");
        }
        return;
    } else {
        mal_err("publish data not found");

        return;
    }

}

static void at_ica_mqtt_client_rsp_callback(void *arg, char *rspinfo, int rsplen)
{
    if (NULL == rspinfo || rsplen == 0) {
        mal_err("invalid input of rsp callback");
        return;
    }
    if (NULL == g_ica_rsp_buff) {
        mal_err("g_ica_rsp_buff rsp is NULL");
        return;
    }

    if (rsplen > AT_MQTT_RSP_MAX_LEN) {
        mal_err("rsp len(%d) exceed max len", rsplen);
        return;
    }

    memcpy(g_ica_rsp_buff, rspinfo, rsplen);
    g_ica_rsp_buff[rsplen] = '\0';

    if (0 == memcmp(g_ica_rsp_buff,
                    AT_ICA_MQTT_MQTTERROR,
                    strlen(AT_ICA_MQTT_MQTTERROR))) {

        at_err_callback(g_ica_rsp_buff);
    } else if (0 == memcmp(g_ica_rsp_buff,
                           AT_ICA_MQTT_MQTTRCVPUB,
                           strlen(AT_ICA_MQTT_MQTTRCVPUB))) { /* Receive Publish Data */

        recv_data_callback(g_ica_rsp_buff);
    } else if (0 == memcmp(g_ica_rsp_buff,
                           AT_ICA_MQTT_MQTTSTATERSP,
                           strlen(AT_ICA_MQTT_MQTTSTATERSP))) {  /* Receive Mqtt Status Change */

        state_change_callback(g_ica_rsp_buff);
    } else {
        switch (g_ica_at_response) {  /* nothing to process */

            case AT_MQTT_IDLE:

                break;

            case AT_MQTT_SEND_TYPE_SIMPLE:

                if (0 == memcmp(g_ica_rsp_buff,
                                AT_MQTT_CMD_SUCCESS_RSP,
                                strlen(AT_MQTT_CMD_SUCCESS_RSP))) {

                    at_succ_callback();
                } else {

                    mal_err("invalid success type %s", g_ica_rsp_buff);
                }

                break;

            case AT_MQTT_AUTH:

                if (0 == memcmp(g_ica_rsp_buff,
                                AT_ICA_MQTT_MQTTAUTHRSP,
                                strlen(AT_ICA_MQTT_MQTTAUTHRSP))) {

                    if (NULL != strstr(g_ica_rsp_buff, AT_MQTT_CMD_SUCCESS_RSP)) {
                        at_succ_callback();
                    }
                }

                break;

            case AT_MQTT_SUB:
                sub_callback(g_ica_rsp_buff);
                break;

            case AT_MQTT_UNSUB:
                unsub_callback(g_ica_rsp_buff);
                break;

            case AT_MQTT_PUB:
                pub_callback(g_ica_rsp_buff);
                break;

            default:
                break;

        }
    }

    return;
}

int at_ica_mqtt_client_disconn(void)
{
    char   at_cmd[64];

    memset(at_cmd, 0, 64);

    /* connect to the network */
    HAL_Snprintf(at_cmd,
                 64,
                 "%s\r\n",
                 AT_ICA_MQTT_MQTTDISCONN);

    /* disconnect from server */
    if (0 != at_ica_mqtt_atsend(at_cmd, AT_MQTT_WAIT_TIMEOUT)) {
        mal_err("disconnect at command fail");

        return -1;
    }

    return 0;
}

int at_ica_mqtt_client_auth(char *proKey, char *devName, char *devSecret, int tlsEnable)
{
    char        at_cmd[AT_MQTT_CMD_MAX_LEN];

    if ((proKey == NULL) || (devName == NULL) || (devSecret == NULL)) {

        mal_err("auth param should not be NULL");

        return -1;
    }

    /* set tls mode before auth */
    if (tlsEnable) {
        memset(at_cmd, 0, AT_MQTT_CMD_MAX_LEN);

        HAL_Snprintf(at_cmd,
                     AT_MQTT_CMD_MAX_LEN - 1,
                     "%s=%d\r\n",
                     AT_ICA_MQTT_MQTTMODE,
                     1);

        if (0 != at_ica_mqtt_atsend(at_cmd, AT_MQTT_WAIT_TIMEOUT)) {

            mal_err("tls at command fail");

            return -1;
        }
    }

    /* submit auth */
    memset(at_cmd, 0, AT_MQTT_CMD_MAX_LEN);

    HAL_Snprintf(at_cmd,
                 AT_MQTT_CMD_MAX_LEN - 1,
                 "%s=\"%s\",\"%s\",\"%s\"\r\n",
                 AT_ICA_MQTT_MQTTAUTH,
                 proKey, devName, devSecret);

    if (0 != at_ica_mqtt_atsend(at_cmd, AT_MQTT_WAIT_TIMEOUT)) {

        mal_err("auth at command fail");

        return -1;
    }

    return 0;
}

int at_ica_mqtt_client_conn(char *proKey, char *devName, char *devSecret, int tlsEnable)
{
    char  at_cmd[64];

    if ((proKey == NULL) || (devName == NULL) || (devSecret == NULL)) {

        mal_err("conn param should not be NULL");

        return -1;
    }

    if (0 != at_ica_mqtt_client_auth(proKey, devName, devSecret, tlsEnable)) {

        mal_err("authen fail");

        return -1;
    }

    /* connect to mqtt server */
    memset(at_cmd, 0, 64);

    HAL_Snprintf(at_cmd,
                 64,
                 "%s\r\n",
                 AT_ICA_MQTT_MQTTCONN);

    if (0 != at_ica_mqtt_atsend(at_cmd, AT_MQTT_WAIT_TIMEOUT)) {

        mal_err("conn at command fail");

        return -1;
    }

    return 0;
}

int at_ica_mqtt_client_subscribe(const char *topic,
                                 int qos,
                                 unsigned int *mqtt_packet_id,
                                 int *mqtt_status,
                                 int timeout_ms)
{
    char    at_cmd[AT_MQTT_CMD_MAX_LEN];

    if ((topic == NULL) || (mqtt_packet_id == NULL) || (mqtt_status == NULL)) {

        mal_err("subscribe param should not be NULL");

        return -1;
    }

    memset(at_cmd, 0, AT_MQTT_CMD_MAX_LEN);

    HAL_Snprintf(at_cmd,
                 AT_MQTT_CMD_MAX_LEN - 1,
                 "%s=\"%s\",%d\r\n",
                 AT_ICA_MQTT_MQTTSUB,
                 topic,
                 qos);

    if (0 != at_ica_mqtt_atsend(at_cmd, timeout_ms)) {
        mal_err("sub at command fail");

        return -1;
    }

    return 0;
}

int at_ica_mqtt_client_unsubscribe(const char *topic,
                                   unsigned int *mqtt_packet_id,
                                   int *mqtt_status)
{
    char    at_cmd[AT_MQTT_CMD_MAX_LEN];
    if ((topic == NULL) || (mqtt_packet_id == NULL) || (mqtt_status == NULL)) {

        mal_err("unsubscribe param should not be NULL");

        return -1;
    }

    memset(at_cmd, 0, AT_MQTT_CMD_MAX_LEN);

    HAL_Snprintf(at_cmd,
                 AT_MQTT_CMD_MAX_LEN - 1,
                 "%s=\"%s\"\r\n",
                 AT_ICA_MQTT_MQTTUNSUB,
                 topic);

    if (0 != at_ica_mqtt_atsend(at_cmd, AT_MQTT_WAIT_TIMEOUT)) {

        mal_err("unsub at command fail");

        return -1;
    }

    return 0;
}

int at_ica_mqtt_client_publish(const char *topic, int qos, const char *message)
{
    char    at_cmd[AT_MQTT_CMD_MAX_LEN] = {0};
    char    msg_convert[AT_MQTT_CMD_MAX_LEN] = {0};
    char   *temp;
    if ((topic == NULL) || (message == NULL)) {

        mal_err("publish param should not be NULL");

        return -1;
    }

    temp = msg_convert;

    /* for the case of " appeared in the string */
    while (*message) {
        if (*message == '\"') {
            *temp++ = '\\';
        }

        *temp++ = *message++;
    }

    HAL_Snprintf(at_cmd,
                 AT_MQTT_CMD_MAX_LEN - 1,
                 "%s=\"%s\",%d,\"%s\"\r\n",
                 AT_ICA_MQTT_MQTTPUB,
                 topic,
                 qos,
                 msg_convert);

    g_public_qos = qos;
    if (0 != at_ica_mqtt_atsend(at_cmd, AT_MQTT_WAIT_TIMEOUT)) {

        mal_err("pub at command fail");

        return -1;
    }
    return 0;
}


int at_ica_mqtt_client_state(void)
{
    int state;

    switch(g_mqtt_connect_state){
    case 0:
         state = IOTX_MC_STATE_DISCONNECTED;
         break;
    case 1:
         state = IOTX_MC_STATE_CONNECTED;
         break;
    default:
         state = IOTX_MC_STATE_INVALID;
         break;
    }

    return state;
}

int at_ica_mqtt_client_init(void)
{
#ifdef PLATFORM_HAS_OS
    g_ica_rsp_buff = AT_MQTT_ICA_MALLOC(AT_MQTT_RSP_MAX_LEN);
    if (NULL == g_ica_rsp_buff) {
        mal_err("at ica mqtt client malloc buff failed");
        return -1;
    }

    if (NULL == (g_sem_response = HAL_SemaphoreCreate())) {
        if (NULL != g_ica_rsp_buff) {
            AT_MQTT_ICA_FREE(g_ica_rsp_buff);

            g_ica_rsp_buff = NULL;
        }
        mal_err("at ica mqtt client create sem failed");

        return -1;
    }
#else
    memset(g_ica_rsp_buff, 0, AT_MQTT_RSP_MAX_LEN);
    g_at_response = 0;
#endif

    g_mqtt_connect_state = IOTX_MC_STATE_INVALID;

    at_register_callback(AT_ICA_MQTT_MQTTRCV,
                             AT_ICA_MQTT_POSTFIX,
                             at_recv_rsp_buf,
                             AT_MQTT_CMD_MAX_LEN,
                             at_ica_mqtt_client_rsp_callback,
                             NULL);

    at_register_callback(AT_ICA_MQTT_MQTTERROR,
                             AT_ICA_MQTT_POSTFIX,
                             at_recv_rsp_buf,
                             AT_MQTT_CMD_MAX_LEN,
                             at_ica_mqtt_client_rsp_callback,
                             NULL);

    at_register_callback(AT_ICA_MQTT_MQTTOK,
                             AT_ICA_MQTT_POSTFIX,
                             at_recv_rsp_buf,
                             AT_MQTT_CMD_MAX_LEN,
                             at_ica_mqtt_client_rsp_callback,
                             NULL);

    return 0;
}

int at_ica_mqtt_client_deinit(void)
{
#ifdef PLATFORM_HAS_OS
    if (NULL != g_ica_rsp_buff) {
        AT_MQTT_ICA_FREE(g_ica_rsp_buff);
        g_ica_rsp_buff = NULL;
    }
    HAL_SemaphoreDestroy(g_sem_response);
#else
    memset(g_ica_rsp_buff, 0, AT_MQTT_RSP_MAX_LEN);
    g_at_response = 0;
#endif

    g_mqtt_connect_state = IOTX_MC_STATE_INVALID;

    return 0;
}

int at_ica_mqtt_atsend(char *at_cmd, int timeout_ms)
{
    if (at_cmd == NULL) {
        return -1;
    }
    /* state error */
    if (g_ica_at_response != AT_MQTT_IDLE) {

        mal_err("at send state is not idle (%d)", g_ica_at_response);

        return -1;
    }

    mal_err("send: %s", at_cmd);

    if (NULL != strstr(at_cmd, AT_ICA_MQTT_MQTTAUTH)) {
        g_ica_at_response = AT_MQTT_AUTH;
    } else if (NULL != strstr(at_cmd, AT_ICA_MQTT_MQTTSUB)) {
        g_ica_at_response = AT_MQTT_SUB;
    } else if (NULL != strstr(at_cmd, AT_ICA_MQTT_MQTTUNSUB)) {
        g_ica_at_response = AT_MQTT_UNSUB;
    } else if (NULL != strstr(at_cmd, AT_ICA_MQTT_MQTTPUB)) {
        g_ica_at_response = AT_MQTT_PUB;
    } else {
        g_ica_at_response = AT_MQTT_SEND_TYPE_SIMPLE;
    }

    if (0 != at_send_no_reply(at_cmd, strlen(at_cmd), false)) {

        mal_err("at send raw api fail");

        g_ica_at_response = AT_MQTT_IDLE;

        return -1;
    }
#ifdef PLATFORM_HAS_OS
    HAL_SemaphoreWait(g_sem_response, timeout_ms);
#else
    if(!g_at_response)
    {
       at_yield(NULL, 0, NULL, 500);
    }
    else
    {
       g_at_response --;
    }
#endif

    g_ica_at_response = AT_MQTT_IDLE;

    return g_at_response_result;
}
