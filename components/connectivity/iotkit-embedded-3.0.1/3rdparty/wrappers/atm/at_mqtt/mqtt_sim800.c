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
#define AT_SIM800_MQTT_IPCONN          "AT+SAPBR"
#define AT_SIM800_MQTT_MQTTCONF        "AT+SMCONF"
#define AT_SIM800_MQTT_MQTTCONN        "AT+SMCONN"
#define AT_SIM800_MQTT_MQTTSUB         "AT+SMSUB"
#define AT_SIM800_MQTT_MQTTUNSUB       "AT+SMUNSUB"
#define AT_SIM800_MQTT_MQTTPUB         "AT+SMPUB"
#define AT_SIM800_MQTT_MQTTSTATE       "AT+SMSTATE"
#define AT_SIM800_MQTT_MQTTDISCONN     "AT+SMDISC"
#define AT_SIM800_MQTT_MQTTSSL         "AT+SMSSL"

#define AT_SIM800_MQTT_MQTTRCV         "+SM"
#define AT_SIM800_MQTT_MQTTERROR       "+CME"
#define AT_SIM800_MQTT_MQTTSAPBR       "+SAPBR"
#define AT_SIM800_MQTT_MQTTOK          "OK"
#define AT_SIM800_MQTT_MQTTRCVPUB      "+SMPUBLISH"
#define AT_SIM800_MQTT_MQTTPUBRSP      "+SMPUB"
#define AT_SIM800_MQTT_MQTTSUBRSP      "+SMSUB"
#define AT_SIM800_MQTT_MQTTUNSUBRSP    "+SMUNSUB"
#define AT_SIM800_MQTT_MQTTSTATERSP    "+SMSTATE"

#define AT_SIM800_MQTT_POSTFIX         "\r\n"

/* change AT_MQTT_CMD_MAX_LEN from 1024 to 300 */
#define AT_MQTT_CMD_MAX_LEN             400
#define AT_MQTT_CMD_DEF_LEN             256
#define AT_MQTT_CMD_MIN_LEN             64
#define AT_MQTT_CMD_SUCCESS_RSP         "OK"
#define AT_MQTT_CMD_FAIL_RSP            "FAIL"
#define AT_MQTT_CMD_ERROR_RSP           "ERROR"

#define AT_MQTT_SUBSCRIBE_FAIL          128
/* change AT_MQTT_RSP_MAX_LEN from 1500 to 300 */
#define AT_MQTT_RSP_MAX_LEN             (CONFIG_MQTT_MESSAGE_MAXLEN + CONFIG_MQTT_TOPIC_MAXLEN + 20)
#define AT_MQTT_RSP_MIN_LEN             64

#define AT_MQTT_WAIT_MAX_TIMEOUT            60*1000
#define AT_MQTT_WAIT_DEF_TIMEOUT            3*1000
#define AT_MQTT_WAIT_MIN_TIMEOUT            800

#define AT_CMD_SIM_PIN_CHECK            "AT+CPIN?"
#define AT_CMD_SIGNAL_QUALITY_CHECK     "AT+CSQ"
#define AT_CMD_NETWORK_REG_CHECK        "AT+CREG?"
#define AT_CMD_GPRS_ATTACH_CHECK        "AT+CGATT?"

#define AT_CMD_ECHO_OFF   "ATE0"

#define SIM800_RETRY_MAX          80

#define AT_DEBUG_MODE
#ifdef AT_DEBUG_MODE
#define mal_err(...)                do{HAL_Printf(__VA_ARGS__);HAL_Printf("\r\n");}while(0)
#define mal_info(...)               do{HAL_Printf(__VA_ARGS__);HAL_Printf("\r\n");}while(0)
#else
#define mal_err(...)
#define mal_info(...) 
#endif

#ifdef INFRA_MEM_STATS
    #include "infra_mem_stats.h"
    #define AT_MQTT_SIM800_MALLOC(size)            LITE_malloc(size, MEM_MAGIC, "mal.ica")
    #define AT_MQTT_SIM800_FREE(ptr)               LITE_free(ptr)
#else
    #define AT_MQTT_SIM800_MALLOC(size)            HAL_Malloc(size)
    #define AT_MQTT_SIM800_FREE(ptr)               {HAL_Free((void *)ptr);ptr = NULL;}
#endif

char   at_max_len_cmd[AT_MQTT_CMD_MAX_LEN];
char   at_def_len_cmd[AT_MQTT_CMD_DEF_LEN];
char   at_min_len_cmd[AT_MQTT_CMD_MIN_LEN];

char   at_recv_rsp_buf[AT_MQTT_CMD_MAX_LEN];
iotx_mqtt_param_t g_pInitParams = {0};
int               g_isInitParamsSet = 0;

typedef enum {
    AT_MQTT_IDLE = 0,
    AT_MQTT_SEND_TYPE_SIMPLE,
    AT_MQTT_AUTH,
    AT_MQTT_SUB,
    AT_MQTT_UNSUB,
    AT_MQTT_PUB,
} at_mqtt_send_type_t;

int at_sim800_mqtt_atsend(char *at_cmd, int timeout_ms);
int at_sim800_mqtt_client_deinit(void);
int at_sim800_mqtt_client_init(iotx_mqtt_param_t * pInitParams);
int at_sim800_mqtt_client_state(void);
int at_sim800_mqtt_client_publish(const char *topic, int qos, const char *message);
int at_sim800_mqtt_client_unsubscribe(const char *topic,
                                   unsigned int *mqtt_packet_id,
                                   int *mqtt_status);
int at_sim800_mqtt_client_subscribe(const char *topic,
                                 int qos,
                                 unsigned int *mqtt_packet_id,
                                 int *mqtt_status,
                                 int timeout_ms);
int at_sim800_mqtt_client_conn(void);
int at_sim800_mqtt_client_ssl(int tlsEnable);
int at_sim800_mqtt_client_disconn(void);
void at_sim800_mqtt_client_reconn(void);

static int at_sim800_gprs_got_ip(void);
static int at_sim800_gprs_status_check(void);
static int at_sim800_gprs_ip_init(void);

int HAL_AT_MQTT_Init(iotx_mqtt_param_t *pInitParams)
{
    return at_sim800_mqtt_client_init(pInitParams);
}

int HAL_AT_MQTT_Deinit()
{
    return at_sim800_mqtt_client_deinit();
}

int HAL_AT_MQTT_Connect(char *proKey, char *devName, char *devSecret)
{
    /* Sim800 module doesn't use Ali ICA MQTT. So don't need to 
     use prokey, devName, devSecret. Use username and password instead
     which is assined in the init params. */
    (void) proKey;
    (void) devName;
    (void) devSecret;
	
    if(g_isInitParamsSet != 0) {
        return at_sim800_mqtt_client_conn();
    }
    else {
        mal_err("HAL_AT_MQTT_Connect failed, because init params are not configured.");
        return -1;
    }
}

int HAL_AT_MQTT_Disconnect(void)
{
    return at_sim800_mqtt_client_disconn();
}

int HAL_AT_MQTT_Subscribe(const char *topic, int qos, unsigned int *mqtt_packet_id, int *mqtt_status, int timeout_ms)
{
    return at_sim800_mqtt_client_subscribe(topic, qos, mqtt_packet_id, mqtt_status, timeout_ms);
}

int HAL_AT_MQTT_Unsubscribe(const char *topic, unsigned int *mqtt_packet_id, int *mqtt_status)
{
    return at_sim800_mqtt_client_unsubscribe(topic, mqtt_packet_id, mqtt_status);
}

int HAL_AT_MQTT_Publish(const char *topic, int qos, const char *message, unsigned int msg_len)
{
    return at_sim800_mqtt_client_publish(topic, qos, message);
}

int HAL_AT_MQTT_State(void)
{
    return at_sim800_mqtt_client_state();
}

#define SAPBR_STATUS_CONNECTING        0
#define SAPBR_STATUS_CONNECTED         1
#define SAPBR_STATUS_CLOSING           2
#define SAPBR_STATUS_CLOSED            3
#define SAPBR_STATUS_INVALID           4

#ifndef PLATFORM_HAS_OS
char g_sim800_rsp_buff[AT_MQTT_RSP_MAX_LEN];
#else
static char              *g_sim800_rsp_buff = NULL;
#endif
static volatile int       g_mqtt_connect_state = IOTX_MC_STATE_INVALID;
static volatile int       g_sapbr_status = SAPBR_STATUS_INVALID;
static volatile at_mqtt_send_type_t   g_sim800_at_response = AT_MQTT_IDLE;
static volatile int       g_at_response_result = 0;
#ifdef PLATFORM_HAS_OS
static void              *g_sem_response;
#endif
static volatile int       g_response_msg_number = 0;
static int                g_response_packetid = 0;
static int                g_response_status = 0;
static int                g_public_qos = 0;

int at_sim800_mqtt_atsend(char *at_cmd, int timeout_ms);

static void recv_sapbr_callback(char *at_rsp)
{
    char *temp;

    g_at_response_result = -1;

    if (NULL == at_rsp) {
        return;
    }

    mal_info("recv sapbr at_rsp =%s", at_rsp);

    temp = strtok(at_rsp, ",");

    if (temp != NULL) {
        temp = strtok(NULL, ",");
        if (temp != NULL) {
            int state = strtol(temp, NULL, 0);
            switch(state) {
                case SAPBR_STATUS_CONNECTING:
                case SAPBR_STATUS_CONNECTED:
                case SAPBR_STATUS_CLOSING:
                case SAPBR_STATUS_CLOSED:
                    g_sapbr_status = state;
                    mal_info("g_sapbr_status =%d", g_sapbr_status);
                    g_at_response_result = 0;
                    break;
                default:
                    g_sapbr_status = SAPBR_STATUS_INVALID;
                    break;
            }
        }
    }
#ifdef PLATFORM_HAS_OS
    /* notify the sender error; */
    HAL_SemaphorePost(g_sem_response);
#endif
    return ;
}

static void at_err_callback(char *at_rsp)
{
    char *temp;

    temp            = strtok(at_rsp, ":");
    temp            = strtok(NULL, ":");
    if ((strtol(temp, NULL, 0)) == 3) {
        g_at_response_result = 0;
    } else {
        g_at_response_result = -1;
    }

#ifdef PLATFORM_HAS_OS
    /* notify the sender error; */
    HAL_SemaphorePost(g_sem_response);
#endif
    return;
}

static void at_succ_callback(void)
{
    g_at_response_result = 0;
#ifdef PLATFORM_HAS_OS
    /* notify the sender ok; */
    HAL_SemaphorePost(g_sem_response);
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
#endif
        return;
    } else if (NULL != strstr(at_rsp, AT_SIM800_MQTT_MQTTSUBRSP)) {
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
#endif
                return;
            }

#ifdef PLATFORM_HAS_OS
            /* notify the sender ok; */
            HAL_SemaphorePost(g_sem_response);
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
#endif

        return;
    } else if (NULL != strstr(at_rsp, AT_SIM800_MQTT_MQTTUNSUBRSP)) {
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

            mal_err("unsub: %s", g_sim800_rsp_buff);
            mal_err("unsub packetid: %d, sta: %d", g_response_packetid, g_response_status);

#ifdef PLATFORM_HAS_OS
            /* notify the sender ok; */
            HAL_SemaphorePost(g_sem_response);
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
#endif

        return;
    } else if (NULL != strstr(at_rsp, AT_SIM800_MQTT_MQTTPUBRSP)) {
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
            int state = strtol(temp, NULL, 0);
            switch(state) {
                /* disconnect */
                case 0:
                    g_mqtt_connect_state = IOTX_MC_STATE_DISCONNECTED;
                    break;
                /* connected */
                case 1:
                    g_mqtt_connect_state = IOTX_MC_STATE_CONNECTED;
                    break;
                /* invalid */
                default:
                    g_mqtt_connect_state = IOTX_MC_STATE_INVALID;
                    break;
            }
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
    temp = strtok(g_sim800_rsp_buff, ":");

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

static void at_sim800_mqtt_client_rsp_callback(void *arg, char *rspinfo, int rsplen)
{
    if (NULL == rspinfo || rsplen == 0) {
        mal_err("invalid input of rsp callback");
        return;
    }

#ifdef PLATFORM_HAS_OS
    if (NULL == g_sim800_rsp_buff) {
        mal_err("g_sim800_rsp_buff rsp is NULL");
        return;
    }
#endif

    if (rsplen > AT_MQTT_RSP_MAX_LEN) {
        mal_err("rsp len(%d) exceed max len", rsplen);
        return;
    }

    memcpy(g_sim800_rsp_buff, rspinfo, rsplen);
    g_sim800_rsp_buff[rsplen] = '\0';

    mal_err("rsp_buff=%s", g_sim800_rsp_buff);

    if (0 == memcmp(g_sim800_rsp_buff,
                    AT_SIM800_MQTT_MQTTSAPBR,
                    strlen(AT_SIM800_MQTT_MQTTSAPBR))){
        recv_sapbr_callback(g_sim800_rsp_buff);
    } else if (0 == memcmp(g_sim800_rsp_buff,
                    AT_SIM800_MQTT_MQTTERROR,
                    strlen(AT_SIM800_MQTT_MQTTERROR))) {

        at_err_callback(g_sim800_rsp_buff);
    } else if (0 == memcmp(g_sim800_rsp_buff,
                           AT_SIM800_MQTT_MQTTRCVPUB,
                           strlen(AT_SIM800_MQTT_MQTTRCVPUB))) { /* Receive Publish Data */

        recv_data_callback(g_sim800_rsp_buff);
    } else if (0 == memcmp(g_sim800_rsp_buff,
                           AT_SIM800_MQTT_MQTTSTATERSP,
                           strlen(AT_SIM800_MQTT_MQTTSTATERSP))) {  /* Receive Mqtt Status Change */

        state_change_callback(g_sim800_rsp_buff);
    } else {
        switch (g_sim800_at_response) {  /* nothing to process */

            case AT_MQTT_IDLE:

                break;

            case AT_MQTT_SEND_TYPE_SIMPLE:

                if (0 == memcmp(g_sim800_rsp_buff,
                                AT_MQTT_CMD_SUCCESS_RSP,
                                strlen(AT_MQTT_CMD_SUCCESS_RSP))) {

                    at_succ_callback();
                } else {

                    mal_err("invalid success type %s", g_sim800_rsp_buff);
                }

                break;

            case AT_MQTT_AUTH:
                /* Sim800 is not support Ali ICA MQTT, so should not reach here. */
                break;

            case AT_MQTT_SUB:
                sub_callback(g_sim800_rsp_buff);
                break;

            case AT_MQTT_UNSUB:
                unsub_callback(g_sim800_rsp_buff);
                break;

            case AT_MQTT_PUB:
                pub_callback(g_sim800_rsp_buff);
                break;

            default:
                break;

        }
    }

    return;
}

int at_sim800_mqtt_client_disconn(void)
{

    memset(at_min_len_cmd, 0, sizeof(at_min_len_cmd));

    /* connect to the network */
    HAL_Snprintf(at_min_len_cmd,
                 sizeof(at_min_len_cmd),
                 "%s\r\n",
                 AT_SIM800_MQTT_MQTTDISCONN);

    /* disconnect from server */
    if (0 != at_sim800_mqtt_atsend(at_min_len_cmd, AT_MQTT_WAIT_DEF_TIMEOUT)) {
        mal_err("disconnect at command fail");

        return -1;
    }
		
    return 0;
}

int at_sim800_mqtt_client_ssl(int tlsEnable)
{
    /* set tls mode before auth */
    if (tlsEnable) {
        memset(at_min_len_cmd, 0, sizeof(at_min_len_cmd));

        /* 1 enable tls, 0 disable tls */
        HAL_Snprintf(at_min_len_cmd,
                     sizeof(at_min_len_cmd) - 1,
                     "%s=%d\r\n",
                     AT_SIM800_MQTT_MQTTSSL,
                     1);

        if (0 != at_sim800_mqtt_atsend(at_min_len_cmd, AT_MQTT_WAIT_MIN_TIMEOUT)) {

            mal_err("tls at command fail");

            return -1;
        }
    }

    return 0;
}

int at_sim800_mqtt_client_conn()
{
    int   retry = 0;

    /* not enable ssl */
    int   tlsEnable = 0;

    if(g_isInitParamsSet == 0) {
        mal_err("at_sim800_mqtt_client_conn init parms is not configured.");
        return -1;
    }
	
    if (0 != at_sim800_mqtt_client_ssl(tlsEnable)) {
        mal_err("tls enable fail");
        return -1;
    }

    /* set mqtt server */
    memset(at_def_len_cmd, 0, sizeof(at_def_len_cmd));

    HAL_Snprintf(at_def_len_cmd,
                 sizeof(at_def_len_cmd),
                 "%s=\"%s\",\"%s:%d\"\r\n",
                 AT_SIM800_MQTT_MQTTCONF, "URL", g_pInitParams.host, g_pInitParams.port);

    if (0 != at_sim800_mqtt_atsend(at_def_len_cmd, AT_MQTT_WAIT_DEF_TIMEOUT)) {

        mal_err("conn at command fail");

        return -1;
    }

    /* clean mqtt session */
    memset(at_min_len_cmd, 0, sizeof(at_min_len_cmd));

    HAL_Snprintf(at_min_len_cmd,
                 sizeof(at_min_len_cmd),
                 "%s=\"%s\",%d\r\n",
                 AT_SIM800_MQTT_MQTTCONF, "CLEANSS", 1);

    if (0 != at_sim800_mqtt_atsend(at_min_len_cmd, AT_MQTT_WAIT_MIN_TIMEOUT)) {

        mal_err("conn at command fail");

        return -1;
    }

    do{
        retry ++;
        mal_err( "%s %d try to turn off echo: %s %d times\r\n", __func__, __LINE__, at_min_len_cmd, retry);
        at_send_wait_reply(AT_CMD_ECHO_OFF, strlen(AT_CMD_ECHO_OFF), true, NULL, 0,
                           at_min_len_cmd, sizeof(at_min_len_cmd), NULL);
    }while((strstr(at_min_len_cmd, AT_MQTT_CMD_SUCCESS_RSP) == NULL) && (retry < SIM800_RETRY_MAX));
    if(retry == SIM800_RETRY_MAX)
    {
       mal_err("try to turn off echo failed");
    }
    /* set username */
    memset(at_def_len_cmd, 0, sizeof(at_def_len_cmd));

    HAL_Snprintf(at_def_len_cmd,
                 sizeof(at_def_len_cmd),
                 "%s=\"%s\",\"%s\"\r\n",
                 AT_SIM800_MQTT_MQTTCONF, "USERNAME", g_pInitParams.username);

    if (0 != at_sim800_mqtt_atsend(at_def_len_cmd, AT_MQTT_WAIT_MIN_TIMEOUT)) {

        mal_err("set username at command fail");

        return -1;
    }
    
    /* set password */
    memset(at_def_len_cmd, 0, sizeof(at_def_len_cmd));

    HAL_Snprintf(at_def_len_cmd,
                 sizeof(at_def_len_cmd),
                 "%s=\"%s\",\"%s\"\r\n",
                 AT_SIM800_MQTT_MQTTCONF, "PASSWORD", g_pInitParams.password);

    if (0 != at_sim800_mqtt_atsend(at_def_len_cmd, AT_MQTT_WAIT_MIN_TIMEOUT)) {

        mal_err("set password at command fail");

        return -1;
    }

    /* set clientid */
    memset(at_def_len_cmd, 0, sizeof(at_def_len_cmd));

    HAL_Snprintf(at_def_len_cmd,
                 sizeof(at_def_len_cmd),
                 "%s=\"%s\",\"%s\"\r\n",
                 AT_SIM800_MQTT_MQTTCONF, "CLIENTID", g_pInitParams.client_id);

    if (0 != at_sim800_mqtt_atsend(at_def_len_cmd, AT_MQTT_WAIT_MIN_TIMEOUT)) {

        mal_err("set clientid at command fail");

        return -1;
    }

    /* set timeout */
    memset(at_min_len_cmd, 0, sizeof(at_min_len_cmd));

    HAL_Snprintf(at_min_len_cmd,
                 sizeof(at_min_len_cmd),
                 "%s=\"%s\",\"%d\"\r\n",
                 AT_SIM800_MQTT_MQTTCONF, "TIMEOUT", AT_MQTT_WAIT_MAX_TIMEOUT/1000);

    if (0 != at_sim800_mqtt_atsend(at_min_len_cmd, AT_MQTT_WAIT_MIN_TIMEOUT)) {

        mal_err("set timeout at command fail");
        return -1;
    }
		
    /* start to connect mqtt server */
    memset(at_min_len_cmd, 0, sizeof(at_min_len_cmd));
    HAL_Snprintf(at_min_len_cmd,
              sizeof(at_min_len_cmd),
              "%s\r\n",
              AT_SIM800_MQTT_MQTTCONN);

    if (0 != at_sim800_mqtt_atsend(at_min_len_cmd, AT_MQTT_WAIT_DEF_TIMEOUT)) {
        mal_err("conn at command fail");
        return -1;
    }
		
    retry = 0;
    while((g_mqtt_connect_state != IOTX_MC_STATE_CONNECTED) && (retry < SIM800_RETRY_MAX)) {
        retry ++;
        at_sim800_mqtt_client_state();
        mal_info("try to wait mqtt server ... mstate %d  retry %d", g_mqtt_connect_state, retry);
    }

    if(retry == SIM800_RETRY_MAX) {
       mal_err("conn at command wait timeout");
       return -1;
    }

    return 0;
}

void at_sim800_mqtt_client_reconn()
{
    int   ret;
	
    mal_info( "%s %d \r\n", __func__, __LINE__);

    /* check pdp status */
    ret = at_sim800_gprs_got_ip();

    if (ret) {
    	/* check gprs status */
    	ret = at_sim800_gprs_status_check();
    	if (ret) {
             mal_err( "%s %d check gprs status failed \r\n", __func__, __LINE__);
    	} else {
            ret = at_sim800_gprs_ip_init();
            if (ret) {
                mal_err( "%s %d failed \r\n", __func__, __LINE__);
            }	
        }
    } else {
        /* start to connect mqtt server */
        at_sim800_mqtt_client_conn();
    }

    return ;
}

int at_sim800_mqtt_client_subscribe(const char *topic,
                                 int qos,
                                 unsigned int *mqtt_packet_id,
                                 int *mqtt_status,
                                 int timeout_ms)
{

    if ((topic == NULL) || (mqtt_packet_id == NULL) || (mqtt_status == NULL)) {
        mal_err("subscribe param should not be NULL");
        return -1;
    }

    memset(at_min_len_cmd, 0, sizeof(at_min_len_cmd));

    HAL_Snprintf(at_min_len_cmd,
                 sizeof(at_min_len_cmd) - 1,
                 "%s=\"%s\",%d\r\n",
                 AT_SIM800_MQTT_MQTTSUB,
                 topic,
                 qos);

    if (0 != at_sim800_mqtt_atsend(at_min_len_cmd, timeout_ms)) {
        mal_err("sub at command fail");

        return -1;
    }

    return 0;
}

int at_sim800_mqtt_client_unsubscribe(const char *topic,
                                   unsigned int *mqtt_packet_id,
                                   int *mqtt_status)
{

    if ((topic == NULL) || (mqtt_packet_id == NULL) || (mqtt_status == NULL)) {
        mal_err("unsubscribe param should not be NULL");
        return -1;
    }

    memset(at_min_len_cmd, 0, sizeof(at_min_len_cmd));

    HAL_Snprintf(at_min_len_cmd,
                 sizeof(at_min_len_cmd) - 1,
                 "%s=\"%s\"\r\n",
                 AT_SIM800_MQTT_MQTTUNSUB,
                 topic);

    if (0 != at_sim800_mqtt_atsend(at_min_len_cmd, AT_MQTT_WAIT_MIN_TIMEOUT)) {
        mal_err("unsub at command fail");
        return -1;
    }

    return 0;
}

int at_sim800_mqtt_client_publish(const char *topic, int qos, const char *message)
{
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

    HAL_Snprintf(at_max_len_cmd,
                 sizeof(at_max_len_cmd) - 1,
                 "%s=\"%s\",%d,%d,\"%s\"\r\n",
                 AT_SIM800_MQTT_MQTTPUB,
                 topic,
                 qos,
                 0,
                 msg_convert);

    g_public_qos = qos;
    if (0 != at_sim800_mqtt_atsend(at_max_len_cmd, AT_MQTT_WAIT_MIN_TIMEOUT)) {
        mal_err("pub at command fail");
        return -1;
    }
    return 0;
}

int g_mqtt_client_count = 0;

int at_sim800_mqtt_client_state(void)
{
    int mode;
    int retry = 0;

    if((g_mqtt_connect_state == IOTX_MC_STATE_CONNECTED)) {
        mode = 32;
    } else {
        mode = 1;
    }

    g_mqtt_client_count ++;

    /* avoid sending too many state check request at commands */
    if(g_mqtt_client_count%mode == 0) {
        /* check mqtt state */
        memset(at_min_len_cmd, 0, sizeof(at_min_len_cmd));
        HAL_Snprintf(at_min_len_cmd,
             sizeof(at_min_len_cmd),
             "%s?\r\n",
             AT_SIM800_MQTT_MQTTSTATE);

        if (0 != at_sim800_mqtt_atsend(at_min_len_cmd, AT_MQTT_WAIT_MIN_TIMEOUT)) {
            mal_err("state at command fail");
            return -1;
        }
    }

    while((g_mqtt_connect_state != IOTX_MC_STATE_CONNECTED) &&( retry < SIM800_RETRY_MAX)) {
       at_sim800_mqtt_client_reconn();
       retry ++;

       /* check mqtt state */
        memset(at_min_len_cmd, 0, sizeof(at_min_len_cmd));
        HAL_Snprintf(at_min_len_cmd,
                 sizeof(at_min_len_cmd),
                 "%s?\r\n",
                 AT_SIM800_MQTT_MQTTSTATE);

        if (0 != at_sim800_mqtt_atsend(at_min_len_cmd, AT_MQTT_WAIT_DEF_TIMEOUT)) {
            mal_err("reconnect mqtt state at command fail");
        }
    }

    return (int)g_mqtt_connect_state;
}

static int at_sim800_gprs_status_check(void)
{
    char rsp[AT_MQTT_RSP_MIN_LEN] = {0};
    int retry = 0;

    while (true) {
        retry++;
        /*sim card status check*/
        at_send_wait_reply(AT_CMD_SIM_PIN_CHECK, strlen(AT_CMD_SIM_PIN_CHECK), true,
                           NULL, 0, rsp, AT_MQTT_RSP_MIN_LEN, NULL);
        if (strstr(rsp, AT_SIM800_MQTT_MQTTOK) == NULL) {

            if (retry > SIM800_RETRY_MAX) {
                return -1;
            }

            mal_err( "%s %d failed rsp %s retry count %d\r\n", __func__, __LINE__, rsp, retry);
        } else {
            break;
        }
    }

    memset(rsp, 0, sizeof(rsp));
    /*Signal quaility check*/
    at_send_wait_reply(AT_CMD_SIGNAL_QUALITY_CHECK, strlen(AT_CMD_SIGNAL_QUALITY_CHECK), true,
                       NULL, 0, rsp, AT_MQTT_RSP_MIN_LEN, NULL);
    if (strstr(rsp, AT_SIM800_MQTT_MQTTOK) == NULL) {
        mal_err( "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }
    mal_info( "signal quality is %s \r\n", rsp);

    memset(rsp, 0, sizeof(rsp));
    /*network registration check*/
    at_send_wait_reply(AT_CMD_NETWORK_REG_CHECK, strlen(AT_CMD_NETWORK_REG_CHECK), true,
                       NULL, 0, rsp, AT_MQTT_RSP_MIN_LEN, NULL);
    if (strstr(rsp, AT_SIM800_MQTT_MQTTOK) == NULL) {
        mal_err( "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }
    mal_info( "network registration is %s \r\n", rsp);


    memset(rsp, 0, sizeof(rsp));
    /*GPRS attach check*/
    at_send_wait_reply(AT_CMD_GPRS_ATTACH_CHECK, strlen(AT_CMD_GPRS_ATTACH_CHECK),true,
                       NULL, 0, rsp, AT_MQTT_RSP_MIN_LEN, NULL);
    if (strstr(rsp, AT_SIM800_MQTT_MQTTOK) == NULL) {
        mal_err( "%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }
    mal_info( "gprs attach check %s \r\n", rsp);

    return 0;
}

static int at_sim800_gprs_ip_init(void)
{
    
    /* check if GPRS bearer is connected */
    memset(at_min_len_cmd, 0, sizeof(at_min_len_cmd));

    HAL_Snprintf(at_min_len_cmd,
               sizeof(at_min_len_cmd),
               "%s=%d,%d\r\n",
               AT_SIM800_MQTT_IPCONN, 2, 1);	
		
    if(0 != at_sim800_mqtt_atsend(at_min_len_cmd, AT_MQTT_WAIT_DEF_TIMEOUT)) {
        mal_err( "%s %d gprs bearer is not connected\r\n", __func__, __LINE__);
    }

#if 0
    if((g_at_response_result == 0) && (g_sapbr_status == SAPBR_STATUS_CONNECTED)) {
        /*Deactivate GPRS PDP Context*/
        memset(at_min_len_cmd, 0, sizeof(at_min_len_cmd));

        HAL_Snprintf(at_min_len_cmd,
               sizeof(at_min_len_cmd),
               "%s=%d,%d\r\n",
               AT_SIM800_MQTT_IPCONN, 0, 1);								 

	while((g_sapbr_status != SAPBR_STATUS_CLOSED) 
	     || (g_sapbr_status != SAPBR_STATUS_INVALID)) {
	    retry ++;
            if(0 != at_sim800_mqtt_atsend(at_min_len_cmd, AT_MQTT_WAIT_DEF_TIMEOUT)) {
                mal_err( "%s %d send failed retry count %d \r\n", __func__, __LINE__, retry);
		continue;
            }
						
	    if( 0 != g_at_response_result) {
	        mal_err( "%s %d failed %d retry count %d g_sapbr_status=%d\r\n", __func__, __LINE__, g_at_response_result, retry, g_sapbr_status);
	    }

            if (retry > SIM800_RETRY_MAX) {
                return -1;
            }
        
        }
    }
#endif

    /* set contype gprs */
    memset(at_min_len_cmd, 0, sizeof(at_min_len_cmd));

    HAL_Snprintf(at_min_len_cmd,
               sizeof(at_min_len_cmd),
               "%s=%d,%d,\"%s\",\"%s\"\r\n",
               AT_SIM800_MQTT_IPCONN, 3, 1, "Contype", "GPRS");

    if (0 != at_sim800_mqtt_atsend(at_min_len_cmd, AT_MQTT_WAIT_DEF_TIMEOUT)) {
			  mal_err("get set contype gprs at command get response fail");
        return -1;
    }
		
    /* set apn */
    memset(at_min_len_cmd, 0, sizeof(at_min_len_cmd));

    HAL_Snprintf(at_min_len_cmd,
                 sizeof(at_min_len_cmd),
                 "%s=%d,%d,\"%s\",\"%s\"\r\n",
                 AT_SIM800_MQTT_IPCONN, 3, 1, "APN", "CMNET");

    if (0 != at_sim800_mqtt_atsend(at_min_len_cmd, AT_MQTT_WAIT_DEF_TIMEOUT)) {
	mal_err("get set apn at command response fail %d", g_at_response_result);
        return -1;
    }

    /* activate pdp */
    memset(at_min_len_cmd, 0, sizeof(at_min_len_cmd));

    HAL_Snprintf(at_min_len_cmd,
                 sizeof(at_min_len_cmd),
                 "%s=%d,%d\r\n",
                 AT_SIM800_MQTT_IPCONN, 1, 1);

    if (0 != at_sim800_mqtt_atsend(at_min_len_cmd, AT_MQTT_WAIT_DEF_TIMEOUT)) {
	mal_err("get activate pdp at command response fail %d", g_at_response_result);
        return -1;
    }

    /* check pdp status */
    memset(at_min_len_cmd, 0, sizeof(at_min_len_cmd));

    HAL_Snprintf(at_min_len_cmd,
                 sizeof(at_min_len_cmd),
                 "%s=%d,%d\r\n",
                 AT_SIM800_MQTT_IPCONN, 2, 1);

    if (0 != at_sim800_mqtt_atsend(at_min_len_cmd, AT_MQTT_WAIT_DEF_TIMEOUT)) {
        mal_err("get check pdp status response at command fail %d", g_at_response_result);
        return -1;
    }
		
    return 0;
}

static int at_sim800_gprs_got_ip(void)
{
    memset(at_max_len_cmd, 0, sizeof(at_max_len_cmd));

    HAL_Snprintf(at_max_len_cmd,
                 sizeof(at_max_len_cmd),
                 "%s=%d,%d\r\n",
                 AT_SIM800_MQTT_IPCONN, 2, 1);

    if (0 != at_sim800_mqtt_atsend(at_max_len_cmd, AT_MQTT_WAIT_DEF_TIMEOUT)) {
        mal_err("get check pdp status at command fail %d", g_at_response_result);
        return -1;
    }
		
    if(g_sapbr_status != SAPBR_STATUS_CONNECTED) {
        mal_err("check sapbr status =%d", g_sapbr_status);
        return -1;
    }

    return 0;
}

int at_sim800_mqtt_client_init(iotx_mqtt_param_t * pInitParams)
{
    int ret;
    int retry = 0;
#ifdef PLATFORM_HAS_OS
    g_sim800_rsp_buff = AT_MQTT_SIM800_MALLOC(AT_MQTT_RSP_MAX_LEN);
    if (NULL == g_sim800_rsp_buff) {
        mal_err("at sim800 mqtt client malloc buff failed");
        return -1;
    }

    if (NULL == (g_sem_response = HAL_SemaphoreCreate())) {
        if (NULL != g_sim800_rsp_buff) {
            AT_MQTT_SIM800_FREE(g_sim800_rsp_buff);

            g_sim800_rsp_buff = NULL;
        }
        mal_err("at sim800 mqtt client create sem failed");

        return -1;
    }
#else
    memset(g_sim800_rsp_buff, 0, sizeof(g_sim800_rsp_buff));
#endif

    g_mqtt_connect_state = IOTX_MC_STATE_INVALID;

    g_pInitParams.port = pInitParams->port;
    g_pInitParams.host = pInitParams->host;
    g_pInitParams.client_id = pInitParams->client_id;
    g_pInitParams.username = pInitParams->username;
    g_pInitParams.password = pInitParams->password;
    g_pInitParams.handle_event.pcontext = pInitParams->handle_event.pcontext;
    g_pInitParams.handle_event.h_fp = pInitParams->handle_event.h_fp;

    g_isInitParamsSet = 1;
    at_register_callback(AT_SIM800_MQTT_MQTTRCV,
                             AT_SIM800_MQTT_POSTFIX,
                             at_recv_rsp_buf,
                             AT_MQTT_CMD_MAX_LEN,
                             at_sim800_mqtt_client_rsp_callback,
                             NULL);

    at_register_callback(AT_SIM800_MQTT_MQTTERROR,
                             AT_SIM800_MQTT_POSTFIX,
                             at_recv_rsp_buf,
                             AT_MQTT_CMD_MIN_LEN,
                             at_sim800_mqtt_client_rsp_callback,
                             NULL);

    at_register_callback(AT_SIM800_MQTT_MQTTOK,
                             AT_SIM800_MQTT_POSTFIX,
                             at_recv_rsp_buf,
                             AT_MQTT_CMD_MIN_LEN,
                             at_sim800_mqtt_client_rsp_callback,
                             NULL);

    at_register_callback(AT_SIM800_MQTT_MQTTSAPBR,
                             AT_SIM800_MQTT_POSTFIX,
                             at_recv_rsp_buf,
                             AT_MQTT_CMD_DEF_LEN,
                             at_sim800_mqtt_client_rsp_callback,
                             NULL);

    /*turn off echo*/
    do{
        retry ++;
        mal_err( "%s %d try to turn off echo: %s %d times\r\n", __func__, __LINE__, at_min_len_cmd, retry);
    at_send_wait_reply(AT_CMD_ECHO_OFF, strlen(AT_CMD_ECHO_OFF), true, NULL, 0,
                       at_min_len_cmd, sizeof(at_min_len_cmd), NULL);
    }while((strstr(at_min_len_cmd, AT_MQTT_CMD_SUCCESS_RSP) == NULL) && (retry < SIM800_RETRY_MAX));
    
    if(retry == SIM800_RETRY_MAX)
    {
        mal_err("try to turn off echo failed");
    }

    ret = at_sim800_gprs_ip_init();
    if (ret) {
        mal_err( "%s %d failed \r\n", __func__, __LINE__);
    }

    ret = at_sim800_gprs_status_check();
    if (ret) {
        mal_err( "%s %d failed \r\n", __func__, __LINE__);
    }

    ret = at_sim800_gprs_got_ip();
    if (ret) {
        mal_err( "%s %d failed \r\n", __func__, __LINE__);
    }
    
    return 0;
}

int at_sim800_mqtt_client_deinit(void)
{
#ifdef PLATFORM_HAS_OS
    if (NULL != g_sim800_rsp_buff) {
        AT_MQTT_SIM800_FREE(g_sim800_rsp_buff);
        g_sim800_rsp_buff = NULL;
    }
    HAL_SemaphoreDestroy(g_sem_response);
#else
    memset(g_sim800_rsp_buff, 0, sizeof(g_sim800_rsp_buff));
#endif

    g_isInitParamsSet = 0;

    g_mqtt_connect_state = IOTX_MC_STATE_INVALID;

    return 0;
}

int at_sim800_mqtt_atsend(char *at_cmd, int timeout_ms)
{
    if (at_cmd == NULL) {
        return -1;
    }
    /* state error */
    if (g_sim800_at_response != AT_MQTT_IDLE) {

        mal_err("at send state is not idle (%d)", g_sim800_at_response);

        return -1;
    }

    mal_err("send: %s", at_cmd);

    if (NULL != strstr(at_cmd, AT_SIM800_MQTT_MQTTSUB)) {
        g_sim800_at_response = AT_MQTT_SUB;
    } else if (NULL != strstr(at_cmd, AT_SIM800_MQTT_MQTTUNSUB)) {
        g_sim800_at_response = AT_MQTT_UNSUB;
    } else if (NULL != strstr(at_cmd, AT_SIM800_MQTT_MQTTPUB)) {
        g_sim800_at_response = AT_MQTT_PUB;
    } else {
        g_sim800_at_response = AT_MQTT_SEND_TYPE_SIMPLE;
    }

    if (0 != at_send_no_reply(at_cmd, strlen(at_cmd), false)) {

        mal_err("at send raw api fail");

        g_sim800_at_response = AT_MQTT_IDLE;

        return -1;
    }
#ifdef PLATFORM_HAS_OS
    HAL_SemaphoreWait(g_sem_response, timeout_ms);
#else

    at_yield(NULL, 0, NULL, timeout_ms);

#endif

    g_sim800_at_response = AT_MQTT_IDLE;

    return g_at_response_result;
}
