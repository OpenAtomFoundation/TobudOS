/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include "mqtt_api.h"
#include "mqtt_wrapper.h"
#include "at_mqtt.h"
#include "at_wrapper.h"

#ifndef PLATFORM_HAS_OS
    #ifdef AT_PARSER_ENABLED
        #include "at_parser.h"
    #endif
#endif

#define MAL_TIMEOUT_FOREVER -1
#define MAL_TIMEOUT_DEFAULT 3000
#define MAL_MC_PACKET_ID_MAX (65535)
#define MAL_MC_DEFAULT_BUFFER_NUM  1
#ifdef PLATFORM_HAS_DYNMEM
    #define MAL_MC_MAX_BUFFER_NUM  14
#else
    #define MAL_MC_MAX_BUFFER_NUM  1
#endif
#define MAL_MC_MAX_TOPIC_LEN   CONFIG_MQTT_TOPIC_MAXLEN
#define MAL_MC_MAX_MSG_LEN     CONFIG_MQTT_MESSAGE_MAXLEN

#define MAL_MC_DEFAULT_TIMEOUT   (8000)

#define mal_emerg(...)             do{HAL_Printf(__VA_ARGS__);HAL_Printf("\r\n");}while(0)
#define mal_crit(...)              do{HAL_Printf(__VA_ARGS__);HAL_Printf("\r\n");}while(0)
#define mal_err(...)               do{HAL_Printf(__VA_ARGS__);HAL_Printf("\r\n");}while(0)
#define mal_warning(...)           do{HAL_Printf(__VA_ARGS__);HAL_Printf("\r\n");}while(0)
#define mal_info(...)              do{HAL_Printf(__VA_ARGS__);HAL_Printf("\r\n");}while(0)
#define mal_debug(...)             do{HAL_Printf(__VA_ARGS__);HAL_Printf("\r\n");}while(0)

#ifdef PLATFORM_HAS_DYNMEM
    #ifdef INFRA_MEM_STATS
        #include "infra_mem_stats.h"
        #define mal_malloc(size)            LITE_malloc(size, MEM_MAGIC, "mqtt")
        #define mal_free(ptr)               LITE_free(ptr)
    #else
        #define mal_malloc(size)            HAL_Malloc(size)
        #define mal_free(ptr)               {HAL_Free((void *)ptr);ptr = NULL;}
    #endif /* INFRA_MEM_STATS */
#else
    #define IOTX_MC_CLIENT_MAX_COUNT 1
#endif

typedef struct at_mqtt_msg_buff_s {
    uint8_t  write_index;
    uint8_t  read_index;
    uint8_t  last_write_index;
    uint8_t  valid_flag[MAL_MC_MAX_BUFFER_NUM];
    uint8_t  buffer_num;
    char    *topic[MAL_MC_MAX_BUFFER_NUM];
    char    *msg_data[MAL_MC_MAX_BUFFER_NUM];
    void    *buffer_mutex;
} at_mqtt_msg_buff_t;
static at_mqtt_msg_buff_t    g_at_mqtt_buff_mgr;
#ifdef PLATFORM_HAS_DYNMEM
static char g_at_mqtt_topic[MAL_MC_DEFAULT_BUFFER_NUM][MAL_MC_MAX_TOPIC_LEN];
static char g_at_mqtt_msg_data[MAL_MC_DEFAULT_BUFFER_NUM][MAL_MC_MAX_MSG_LEN];
#else
static char g_at_mqtt_topic[MAL_MC_MAX_BUFFER_NUM][MAL_MC_MAX_TOPIC_LEN];
static char g_at_mqtt_msg_data[MAL_MC_MAX_BUFFER_NUM][MAL_MC_MAX_MSG_LEN];
iotx_mc_client_t g_iotx_mc_client[IOTX_MC_CLIENT_MAX_COUNT] = {0};
#endif

static iotx_mc_state_t mal_mc_get_client_state(iotx_mc_client_t *pClient);
static void mal_mc_set_client_state(iotx_mc_client_t *pClient, iotx_mc_state_t newState);

typedef struct {
    uint32_t time;
} mal_time_t;

static uint32_t mal_time_is_expired(mal_time_t *timer)
{
    uint32_t cur_time;

    if (!timer) {
        return 1;
    }

    cur_time = HAL_UptimeMs();
    /*
     *  WARNING: Do NOT change the following code until you know exactly what it do!
     *
     *  check whether it reach destination time or not.
     */
    if ((cur_time - timer->time) < (UINT32_MAX / 2)) {
        return 1;
    } else {
        return 0;
    }
}

static uint32_t mal_time_left(mal_time_t *end)
{
    uint32_t now, res;

    if (!end) {
        return 0;
    }

    if (mal_time_is_expired(end)) {
        return 0;
    }

    now = HAL_UptimeMs();
    res = end->time - now;
    return res;
}

static void mal_time_init(mal_time_t *timer)
{
    if (!timer) {
        return;
    }

    timer->time = 0;
}

static void mal_time_countdown_ms(mal_time_t *timer, uint32_t millisecond)
{
    if (!timer) {
        return;
    }

    timer->time = HAL_UptimeMs() + millisecond;
}

static int mal_mc_check_rule(char *iterm, iotx_mc_topic_type_t type)
{
    int i = 0;
    int len = 0;

    if (NULL == iterm) {
        mal_err("iterm is NULL");
        return FAIL_RETURN;
    }

    len = strlen(iterm);

    for (i = 0; i < len; i++) {
        if (TOPIC_FILTER_TYPE == type) {
            if ('+' == iterm[i] || '#' == iterm[i]) {
                if (1 != len) {
                    mal_err("the character # and + is error");
                    return FAIL_RETURN;
                }
            }
        } else {
            if ('+' == iterm[i] || '#' == iterm[i]) {
                mal_err("has character # and + is error");
                return FAIL_RETURN;
            }
        }

        if (iterm[i] < 32 || iterm[i] >= 127) {
            return FAIL_RETURN;
        }
    }
    return SUCCESS_RETURN;
}

/* check whether the topic is matched or not */
static char mal_mc_is_topic_matched(char *topicFilter, const char *topicName)
{
    char *curf;
    const char *curn;
    const char *curn_end;

    if (!topicFilter || !topicName) {
        return 0;
    }

    curf = topicFilter;
    curn = topicName;
    curn_end = curn + strlen(topicName);

    while (*curf && curn < curn_end) {
        if (*curn == '/' && *curf != '/') {
            break;
        }

        if (*curf != '+' && *curf != '#' && *curf != *curn) {
            break;
        }

        if (*curf == '+') {
            /* skip until we meet the next separator, or end of string */
            const char *nextpos = curn + 1;
            while (nextpos < curn_end && *nextpos != '/') {
                nextpos = ++curn + 1;
            }
        } else if (*curf == '#') {
            curn = curn_end - 1;    /* skip until end of string */
        }
        curf++;
        curn++;
    }

    return (curn == curn_end) && (*curf == '\0');
}

/* Check topic name */
/* 0, topic name is valid; NOT 0, topic name is invalid */
static int mal_mc_check_topic(const char *topicName, iotx_mc_topic_type_t type)
{
    int mask = 0;
    char *delim = "/";
    char *iterm = NULL;
    char topicString[MAL_MC_MAX_TOPIC_LEN];
    if (NULL == topicName || '/' != topicName[0]) {
        return FAIL_RETURN;
    }

    if (strlen(topicName) > MAL_MC_MAX_TOPIC_LEN) {
        mal_err("len of topicName(%d) exceeds 64", strlen(topicName));
        return FAIL_RETURN;
    }

    memset(topicString, 0x0, MAL_MC_MAX_TOPIC_LEN);
    strncpy(topicString, topicName, MAL_MC_MAX_TOPIC_LEN - 1);

    iterm = strtok(topicString, delim);

    if (SUCCESS_RETURN != mal_mc_check_rule(iterm, type)) {
        mal_err("run iotx_check_rule error");
        return FAIL_RETURN;
    }

    for (;;) {
        iterm = strtok(NULL, delim);

        if (iterm == NULL) {
            break;
        }

        /* The character '#' is not in the last */
        if (1 == mask) {
            mal_err("the character # is error");
            return FAIL_RETURN;
        }

        if (SUCCESS_RETURN != mal_mc_check_rule(iterm, type)) {
            mal_err("run iotx_check_rule error");
            return FAIL_RETURN;
        }

        if (iterm[0] == '#') {
            mask = 1;
        }
    }

    return SUCCESS_RETURN;
}

#ifndef PLATFORM_HAS_DYNMEM
static int mal_mc_check_handle_is_identical_ex(iotx_mc_topic_handle_t *messageHandlers1,
        iotx_mc_topic_handle_t *messageHandler2)
{
    int topicNameLen = 0;
    if (!messageHandlers1 || !messageHandler2) {
        return 1;
    }
    if (!(messageHandlers1->topic_filter) || !(messageHandler2->topic_filter)) {
        return 1;
    }
    topicNameLen = strlen(messageHandlers1->topic_filter);
    if (topicNameLen != strlen(messageHandler2->topic_filter)) {
        return 1;
    }
    if (0 != strncmp(messageHandlers1->topic_filter, messageHandler2->topic_filter, topicNameLen)) {
        return 1;
    }
    return 0;
}
static int mal_mc_check_handle_is_identical(iotx_mc_topic_handle_t *messageHandlers1,
        iotx_mc_topic_handle_t *messageHandler2)
{
    if (mal_mc_check_handle_is_identical_ex(messageHandlers1, messageHandler2) != 0) {
        return 1;
    }
    if (messageHandlers1->handle.h_fp != messageHandler2->handle.h_fp) {
        return 1;
    }
    if (messageHandlers1->handle.pcontext != messageHandler2->handle.pcontext) {
        return 1;
    }
    return 0;
}
#endif /* PLATFORM_HAS_DYNMEM */
/* MQTT send connect packet */
static int MALMQTTConnect(iotx_mc_client_t *pClient)
{
    char product_key[IOTX_PRODUCT_KEY_LEN + 1] = {0};
    char device_name[IOTX_DEVICE_NAME_LEN + 1] = {0};
    char device_secret[IOTX_DEVICE_SECRET_LEN + 1] = {0};

    HAL_GetProductKey(product_key);
    HAL_GetDeviceName(device_name);
    HAL_GetDeviceSecret(device_secret);

    if (0 != HAL_AT_MQTT_Connect(product_key, device_name, device_secret)) {
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

static int MALMQTTPublish(iotx_mc_client_t *c, const char *topicName, iotx_mqtt_topic_info_pt topic_msg)

{
    if (!c || !topicName || !topic_msg) {
        mal_err("MALMQTTPublish invalid parms\n");
        return FAIL_RETURN;
    }

    if (0 != HAL_AT_MQTT_Publish(topicName, topic_msg->qos, topic_msg->payload,
                                 topic_msg->payload_len)) {
        mal_err("MALMQTTPublish publish failed\n");
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

#ifdef PLATFORM_HAS_DYNMEM
static int remove_handle_from_list(iotx_mc_client_t *c, iotx_mc_topic_handle_t *h)
{
    iotx_mc_topic_handle_t **hp, *h1;
    hp = &c->first_sub_handle;
    while ((*hp) != NULL) {
        h1 = *hp;
        if (h1 == h) {
            *hp = h->next;
        } else {
            hp = &h1->next;
        }
    }

    return 0;
}
#endif

/* MQTT send subscribe packet */
static int MALMQTTSubscribe(iotx_mc_client_t *c, const char *topicFilter, iotx_mqtt_qos_t qos, unsigned int msgId,
                            iotx_mqtt_event_handle_func_fpt messageHandler, void *pcontext, int timeout_ms)
{
    int status;
    iotx_mc_topic_handle_t *h = NULL;
#ifndef PLATFORM_HAS_DYNMEM
    int idx = 0;
    int dup = 0;
#endif

    if (!c || !topicFilter || !messageHandler) {
        return FAIL_RETURN;
    }
#ifdef PLATFORM_HAS_DYNMEM
    h = mal_malloc(sizeof(iotx_mc_topic_handle_t));
    if (h == NULL) {
        mal_err("maloc failed!");
        return FAIL_RETURN;
    }
#else
    for (idx = 0; idx < IOTX_MC_SUBHANDLE_LIST_MAX_LEN; idx++) {
        if (c->list_sub_handle[idx].used == 0) {
            h = &c->list_sub_handle[idx];
            memset(h, 0, sizeof(iotx_mc_topic_handle_t));
            c->list_sub_handle[idx].used = 1;
            break;
        }
    }

    if (h == NULL) {
        mal_err("sub handle list is too short!");
        return FAIL_RETURN;
    }
#endif
#ifdef PLATFORM_HAS_DYNMEM
    memset(h, 0, sizeof(iotx_mc_topic_handle_t));

    h->topic_filter = mal_malloc(strlen(topicFilter) + 1);
    if (NULL == h->topic_filter) {
        mal_free(h);
        return FAIL_RETURN;
    }
#else
    if (strlen(topicFilter) >= CONFIG_MQTT_TOPIC_MAXLEN) {
        mal_err("sub topic length is too large!");
        memset(h, 0, sizeof(iotx_mc_topic_handle_t));
        return FAIL_RETURN;
    }
#endif
    memcpy((char *)h->topic_filter, topicFilter, strlen(topicFilter) + 1);

    h->handle.h_fp = messageHandler;
    h->handle.pcontext = pcontext;
    h->topic_type =  TOPIC_NAME_TYPE;

    HAL_MutexLock(c->lock_generic);
#ifdef PLATFORM_HAS_DYNMEM
    h->next = c->first_sub_handle;
    c->first_sub_handle = h;
#else
    for (idx = 0; idx < IOTX_MC_SUBHANDLE_LIST_MAX_LEN; idx++) {
        if (&c->list_sub_handle[idx] != h &&
            0 == mal_mc_check_handle_is_identical(&c->list_sub_handle[idx], h)) {
            mal_warning("dup sub,topic = %s", topicFilter);
            dup = 1;
        }
    }
    if (dup == 1) {
        memset(h, 0, sizeof(iotx_mc_topic_handle_t));
    }
#endif
    HAL_MutexUnlock(c->lock_generic);
    if (HAL_AT_MQTT_Subscribe(topicFilter, qos, &msgId, &status, timeout_ms) != 0) {
        return FAIL_RETURN;
    }
    return SUCCESS_RETURN;
}


/* MQTT send unsubscribe packet */
static int MALMQTTUnsubscribe(iotx_mc_client_t *c, const char *topicFilter, unsigned int msgId)
{
    int status;
    int ret;
#ifdef PLATFORM_HAS_DYNMEM
    iotx_mc_topic_handle_t *h;
#else
    int idx;
#endif

    ret = HAL_AT_MQTT_Unsubscribe(topicFilter, &msgId, &status);
    if (ret != 0) {
        return -1;
    }

#ifdef PLATFORM_HAS_DYNMEM
    for (h = c->first_sub_handle; h != NULL; h = h->next) {
        if (((strlen(topicFilter) == strlen(h->topic_filter))
             && (strcmp(topicFilter, (char *)h->topic_filter) == 0))
            || (mal_mc_is_topic_matched((char *)h->topic_filter, topicFilter))) {
            remove_handle_from_list(c, h);
        }
    }
#else
    for (idx = 0; idx < IOTX_MC_SUBHANDLE_LIST_MAX_LEN; idx++) {
        if ((c->list_sub_handle[idx].used == 1) &&
            (((strlen(topicFilter) == strlen(c->list_sub_handle[idx].topic_filter))
              && (strcmp(topicFilter, (char *)c->list_sub_handle[idx].topic_filter) == 0)) ||
             mal_mc_is_topic_matched((char *)c->list_sub_handle[idx].topic_filter, topicFilter))) {
            mal_debug("topic be matched");
            memset(&c->list_sub_handle[idx], 0, sizeof(iotx_mc_topic_handle_t));
        }
    }
#endif

    return 0;
}

/* MQTT send disconnect packet */
static int MALMQTTDisconnect(iotx_mc_client_t *c)
{
    return HAL_AT_MQTT_Disconnect();
}

/* get next packet-id */
static int mal_mc_get_next_packetid(iotx_mc_client_t *c)
{
    unsigned int id = 0;

    if (!c) {
        return FAIL_RETURN;
    }

    HAL_MutexLock(c->lock_generic);
    c->packet_id = (c->packet_id == MAL_MC_PACKET_ID_MAX) ? 1 : c->packet_id + 1;
    id = c->packet_id;
    HAL_MutexUnlock(c->lock_generic);

    return id;
}

/* handle PUBLISH packet received from remote MQTT broker */
static int iotx_mc_handle_recv_PUBLISH(iotx_mc_client_t *c, char *topic, char *msg)
{
    iotx_mqtt_topic_info_t topic_msg = {0};
    int flag_matched = 0;
    static uint64_t time_prev = 0;
    uint64_t time_curr = 0;
    /* flowControl for specific topic */
    char *filterStr = "{\"method\":\"thing.service.property.set\"";
    int filterLen = strlen(filterStr);
#ifdef PLATFORM_HAS_DYNMEM
    iotx_mc_topic_handle_t *h, *msg_handle;
#else
    iotx_mc_topic_handle_t *msg_handle;
    int idx;
#endif

    if (!c || !topic || !msg) {
        return FAIL_RETURN;
    }
    mal_debug("recv pub topic=%s msg=%s", topic, msg);

    if (0 == memcmp(msg, filterStr, filterLen)) {
        /* mal_debug("iotx_mc_handle_recv_PUBLISH match filterstring"); */
        time_curr = HAL_UptimeMs();
        if (time_curr < time_prev) {
            time_curr = time_prev;
        }
        if ((time_curr - time_prev) <= (uint64_t)50) {
            mal_debug("pub over threshould");
            return SUCCESS_RETURN;
        } else {
            time_prev = time_curr;
        }
    }

    /* we have to find the right message handler - indexed by topic */
    HAL_MutexLock(c->lock_generic);
#ifdef PLATFORM_HAS_DYNMEM
    for (h = c->first_sub_handle; h != NULL; h = h->next) {
        if (((strlen(topic) == strlen(h->topic_filter))
             && (strcmp(topic, (char *)h->topic_filter) == 0))
            || (mal_mc_is_topic_matched((char *)h->topic_filter, topic))) {
            msg_handle = h;
#else
    for (idx = 0; idx < IOTX_MC_SUBHANDLE_LIST_MAX_LEN; idx++) {
        if ((c->list_sub_handle[idx].used == 1) &&
            (((strlen(topic) == strlen(c->list_sub_handle[idx].topic_filter))
              && (strcmp(topic, (char *)c->list_sub_handle[idx].topic_filter) == 0))
             || (mal_mc_is_topic_matched((char *)c->list_sub_handle[idx].topic_filter, topic)))) {
            msg_handle = &c->list_sub_handle[idx];
#endif
            mal_debug("pub topic is matched");

            HAL_MutexUnlock(c->lock_generic);

            if (NULL != msg_handle->handle.h_fp) {
                iotx_mqtt_event_msg_t event_msg;
                topic_msg.payload = msg;
                topic_msg.payload_len = strlen(msg);
                topic_msg.ptopic = topic;
                topic_msg.topic_len = strlen(topic);
                event_msg.event_type = IOTX_MQTT_EVENT_PUBLISH_RECEIVED;
                event_msg.msg = &topic_msg;
                msg_handle->handle.h_fp(msg_handle->handle.pcontext, c, &event_msg);
                flag_matched = 1;
            }

            HAL_MutexLock(c->lock_generic);
        }
    }

    HAL_MutexUnlock(c->lock_generic);

    if (0 == flag_matched) {
        mal_debug("NO matching any topic, call default handle function");

        if (NULL != c->handle_event.h_fp) {
            iotx_mqtt_event_msg_t event_msg;

            topic_msg.payload = msg;
            topic_msg.payload_len = strlen(msg);
            topic_msg.ptopic = topic;
            topic_msg.topic_len = strlen(topic);
            event_msg.event_type = IOTX_MQTT_EVENT_PUBLISH_RECEIVED;
            event_msg.msg = &topic_msg;

            c->handle_event.h_fp(c->handle_event.pcontext, c, &event_msg);
        }
    }

    return SUCCESS_RETURN;
}

/* MQTT cycle to handle packet from remote broker */
static int mal_mc_cycle(iotx_mc_client_t *c, mal_time_t *timer)
{
    int rc = SUCCESS_RETURN;
    char *msg = NULL;
    char *topic = NULL;
    uint8_t read_index = 0;

    if (!c) {
        return FAIL_RETURN;
    }

    if (HAL_AT_MQTT_State() == IOTX_MC_STATE_CONNECTED) {
        mal_mc_set_client_state(c, IOTX_MC_STATE_CONNECTED);
    }

    if (mal_mc_get_client_state(c) != IOTX_MC_STATE_CONNECTED) {
        mal_err("mal state = %d error", mal_mc_get_client_state(c));
#ifndef PLATFORM_HAS_OS
#ifdef AT_PARSER_ENABLED
        at_yield(NULL, 0, NULL, 100);
#endif
#endif
        return MQTT_STATE_ERROR;
    }

    if (HAL_AT_MQTT_State() != IOTX_MC_STATE_CONNECTED) {
        mal_err("hal mal state = %d error", HAL_AT_MQTT_State());
        mal_mc_set_client_state(c, IOTX_MC_STATE_DISCONNECTED);
#ifndef PLATFORM_HAS_OS
#ifdef AT_PARSER_ENABLED
        at_yield(NULL, 0, NULL, 100);
#endif
#endif
        return MQTT_NETWORK_ERROR;
    }

    /* read the buf, see what work is due */
    HAL_MutexLock(g_at_mqtt_buff_mgr.buffer_mutex);
    read_index = g_at_mqtt_buff_mgr.read_index;

    if (g_at_mqtt_buff_mgr.valid_flag[read_index] == 0) {
        HAL_MutexUnlock(g_at_mqtt_buff_mgr.buffer_mutex);
        return FAIL_RETURN;
    }

    topic = g_at_mqtt_buff_mgr.topic[read_index];
    msg   = g_at_mqtt_buff_mgr.msg_data[read_index];

    rc = iotx_mc_handle_recv_PUBLISH(c, topic, msg);
    if (SUCCESS_RETURN != rc) {
        mal_err("recvPublishProc error,result = %d", rc);
    }

    memset(g_at_mqtt_buff_mgr.topic[read_index], 0, MAL_MC_MAX_TOPIC_LEN);
    memset(g_at_mqtt_buff_mgr.msg_data[read_index], 0, MAL_MC_MAX_MSG_LEN);
    g_at_mqtt_buff_mgr.valid_flag[read_index] = 0;

    read_index++;
    if (read_index >= g_at_mqtt_buff_mgr.buffer_num) {
        read_index = 0;
    }

    g_at_mqtt_buff_mgr.read_index = read_index;
    HAL_MutexUnlock(g_at_mqtt_buff_mgr.buffer_mutex);

    return rc;
}

/* get state of MQTT client */
static iotx_mc_state_t mal_mc_get_client_state(iotx_mc_client_t *pClient)
{
    iotx_mc_state_t state;
    HAL_MutexLock(pClient->lock_generic);
    state = pClient->client_state;
    HAL_MutexUnlock(pClient->lock_generic);

    return state;
}

/* set state of MQTT client */
static void mal_mc_set_client_state(iotx_mc_client_t *pClient, iotx_mc_state_t newState)
{

    HAL_MutexLock(pClient->lock_generic);
    pClient->client_state = newState;
    HAL_MutexUnlock(pClient->lock_generic);
}

static int mal_mc_recv_buf_init()
{
    int i;
    g_at_mqtt_buff_mgr.read_index  = 0;
    g_at_mqtt_buff_mgr.write_index = 0;
    g_at_mqtt_buff_mgr.last_write_index = 0;
    g_at_mqtt_buff_mgr.buffer_num = MAL_MC_DEFAULT_BUFFER_NUM;

    for (i = 0; i < MAL_MC_MAX_BUFFER_NUM; i++) {
        g_at_mqtt_buff_mgr.valid_flag[i] = 0;
#ifdef PLATFORM_HAS_DYNMEM
        if (i < MAL_MC_DEFAULT_BUFFER_NUM) {
            g_at_mqtt_buff_mgr.topic[i] = g_at_mqtt_topic[i];
            g_at_mqtt_buff_mgr.msg_data[i] = g_at_mqtt_msg_data[i];
            memset(g_at_mqtt_buff_mgr.topic[i], 0, MAL_MC_MAX_TOPIC_LEN);
            memset(g_at_mqtt_buff_mgr.msg_data[i], 0, MAL_MC_MAX_MSG_LEN);
        } else {
            g_at_mqtt_buff_mgr.topic[i] = NULL;
            g_at_mqtt_buff_mgr.msg_data[i] = NULL;
        }
#else
        g_at_mqtt_buff_mgr.topic[i] = g_at_mqtt_topic[i];
        g_at_mqtt_buff_mgr.msg_data[i] = g_at_mqtt_msg_data[i];
        memset(g_at_mqtt_buff_mgr.topic[i], 0, MAL_MC_MAX_TOPIC_LEN);
        memset(g_at_mqtt_buff_mgr.msg_data[i], 0, MAL_MC_MAX_MSG_LEN);
#endif
    }

    if (NULL == (g_at_mqtt_buff_mgr.buffer_mutex = HAL_MutexCreate())) {
        mal_err("create buffer mutex error");
        return -1;
    }

    return 0;
}

static void mal_mc_recv_buf_deinit()
{
    int i;
    g_at_mqtt_buff_mgr.read_index  = 0;
    g_at_mqtt_buff_mgr.write_index = 0;
    g_at_mqtt_buff_mgr.last_write_index = 0;

#ifdef PLATFORM_HAS_DYNMEM
    for (i = 0; i < MAL_MC_MAX_BUFFER_NUM; i++) {
        g_at_mqtt_buff_mgr.valid_flag[i] = 0;
        if (i < MAL_MC_DEFAULT_BUFFER_NUM) {
            memset(g_at_mqtt_buff_mgr.topic[i], 0, MAL_MC_MAX_TOPIC_LEN);
            memset(g_at_mqtt_buff_mgr.msg_data[i], 0, MAL_MC_MAX_MSG_LEN);
        } else {
            if (i < g_at_mqtt_buff_mgr.buffer_num) {
                if (g_at_mqtt_buff_mgr.topic[i] != NULL) {
                    mal_free(g_at_mqtt_buff_mgr.topic[i]);
                }
                if (g_at_mqtt_buff_mgr.msg_data[i] != NULL) {
                    mal_free(g_at_mqtt_buff_mgr.msg_data[i]);
                }
            }
        }
    }
#else
    for (i = 0; i < g_at_mqtt_buff_mgr.buffer_num; i++) {
        g_at_mqtt_buff_mgr.valid_flag[i] = 0;
        memset(g_at_mqtt_buff_mgr.topic[i], 0, MAL_MC_MAX_TOPIC_LEN);
        memset(g_at_mqtt_buff_mgr.msg_data[i], 0, MAL_MC_MAX_MSG_LEN);
    }
#endif

    HAL_MutexDestroy(g_at_mqtt_buff_mgr.buffer_mutex);
}

static int mal_mc_wait_for_result()
{
#ifdef PLATFORM_HAS_OS
    mal_time_t         time;
    int state = 0;
    int timeout_ms = MAL_MC_DEFAULT_TIMEOUT;
    mal_time_init(&time);
    mal_time_countdown_ms(&time, timeout_ms);
    do {
        unsigned int left_t;
        left_t = mal_time_left(&time);
        if (left_t < 100) {
            HAL_SleepMs(left_t);
        } else {
            HAL_SleepMs(100);
        }

        state = HAL_AT_MQTT_State();
    } while (!mal_time_is_expired(&time) && (state != IOTX_MC_STATE_CONNECTED));

    if (state == IOTX_MC_STATE_CONNECTED) {
        return SUCCESS_RETURN;
    } else {
        return FAIL_RETURN;
    }
#else
    int state = 0;
#ifdef AT_PARSER_ENABLED
    int timeout_ms = 1000;
#endif
    int count = 10;
    while ((count > 0) && ((state = HAL_AT_MQTT_State()) != IOTX_MC_STATE_CONNECTED)) {
#ifdef AT_PARSER_ENABLED
        at_yield(NULL, 0, NULL, timeout_ms);
#endif
        count --;
    }

    if (state == IOTX_MC_STATE_CONNECTED) {
        return SUCCESS_RETURN;
    } else {
        return FAIL_RETURN;
    }
#endif
}

static int mal_mc_disconnect(iotx_mc_client_t *pClient)
{
    int             rc = -1;

    if (NULL == pClient) {
        return NULL_VALUE_ERROR;
    }

    if (wrapper_mqtt_check_state(pClient)) {
        rc = MALMQTTDisconnect(pClient);
        mal_debug("rc = MALMQTTDisconnect() = %d", rc);
    }

    mal_mc_set_client_state(pClient, IOTX_MC_STATE_INITIALIZED);

    mal_info("mqtt disconnect!");
    return SUCCESS_RETURN;
}

int at_mqtt_input(struct at_mqtt_input *param)
{
    char         *topic;
    uint32_t      topic_len;
    char         *message;
    uint32_t      message_len;
    uint8_t       write_index;
    char         *copy_ptr;

    if (NULL == param) {
        mal_err("input param null");
        return -1;
    }

    topic = param->topic;
    topic_len = param->topic_len;
    message = param->message;
    message_len = param->msg_len;

    if ((topic == NULL) || (topic_len == 0) ||
        (message == NULL) || (message_len == 0)) {
        mal_err("input topic or message is NULL");
        return -1;
    }

    if ((topic_len >= MAL_MC_MAX_TOPIC_LEN) ||
        (message_len >= MAL_MC_MAX_MSG_LEN)) {
        mal_err("topic(%d) or message(%d) too large", topic_len, message_len);
        return -1;
    }

    HAL_MutexLock(g_at_mqtt_buff_mgr.buffer_mutex);
    write_index     = g_at_mqtt_buff_mgr.write_index;

    if ((g_at_mqtt_buff_mgr.valid_flag[write_index])
        && (g_at_mqtt_buff_mgr.buffer_num == MAL_MC_MAX_BUFFER_NUM)) {
        mal_err("buffer is full");

        HAL_MutexUnlock(g_at_mqtt_buff_mgr.buffer_mutex);
        return -1;
    }
    if (g_at_mqtt_buff_mgr.valid_flag[write_index]) {
        int last_write_index = write_index;
        g_at_mqtt_buff_mgr.last_write_index = last_write_index;
        write_index = g_at_mqtt_buff_mgr.buffer_num;
        mal_err("increase buffer to %d", g_at_mqtt_buff_mgr.buffer_num);
#ifdef PLATFORM_HAS_DYNMEM
        g_at_mqtt_buff_mgr.topic[write_index] = mal_malloc(MAL_MC_MAX_TOPIC_LEN);
        if (g_at_mqtt_buff_mgr.topic[write_index] == NULL) {
            mal_err("increase buffer failed, drop it");
            return -1;
        }
        g_at_mqtt_buff_mgr.msg_data[write_index] = mal_malloc(MAL_MC_MAX_MSG_LEN);
        if (g_at_mqtt_buff_mgr.msg_data[write_index] == NULL) {
            mal_err("increase buffer failed, drop it");
            mal_free(g_at_mqtt_buff_mgr.topic[write_index]);
            return -1;
        }
        g_at_mqtt_buff_mgr.buffer_num ++;
#else
        g_at_mqtt_buff_mgr.buffer_num ++;
        g_at_mqtt_buff_mgr.topic[g_at_mqtt_buff_mgr.buffer_num] = g_at_mqtt_topic[g_at_mqtt_buff_mgr.buffer_num];
        g_at_mqtt_buff_mgr.msg_data[g_at_mqtt_buff_mgr.buffer_num] = g_at_mqtt_msg_data[g_at_mqtt_buff_mgr.buffer_num];
        memset(g_at_mqtt_buff_mgr.topic[g_at_mqtt_buff_mgr.buffer_num], 0, MAL_MC_MAX_TOPIC_LEN);
        memset(g_at_mqtt_buff_mgr.msg_data[g_at_mqtt_buff_mgr.buffer_num], 0, MAL_MC_MAX_MSG_LEN);
#endif
    } else {
        g_at_mqtt_buff_mgr.last_write_index = 0;
    }

    copy_ptr = g_at_mqtt_buff_mgr.topic[write_index];
    memcpy(copy_ptr, topic, topic_len);
    copy_ptr = g_at_mqtt_buff_mgr.msg_data[write_index];
    memcpy(copy_ptr, message, message_len);

    g_at_mqtt_buff_mgr.valid_flag[write_index] = 1;
    write_index++;

    if (write_index >= g_at_mqtt_buff_mgr.buffer_num) {
        write_index = 0;
    }

    if (g_at_mqtt_buff_mgr.last_write_index != 0) {
        g_at_mqtt_buff_mgr.write_index = g_at_mqtt_buff_mgr.last_write_index;
    } else {
        g_at_mqtt_buff_mgr.write_index  = write_index;
    }
    HAL_MutexUnlock(g_at_mqtt_buff_mgr.buffer_mutex);

    return 0;
}

/* Initialize MQTT client */
static int mal_mc_init(iotx_mc_client_t *pClient, iotx_mqtt_param_t *pInitParams)
{
    int rc = FAIL_RETURN;
    iotx_mc_state_t mc_state = IOTX_MC_STATE_INVALID;

    if (pClient == NULL || pInitParams == NULL ||
        pInitParams->write_buf_size == 0 || pInitParams->read_buf_size == 0) {
        return NULL_VALUE_ERROR;
    }

    memset(pClient, 0, sizeof(iotx_mc_client_t));

    if (HAL_AT_MQTT_Init(pInitParams) != 0) {
        mal_err("low layer init failed");
        return FAIL_RETURN;
    }

    pClient->lock_generic = HAL_MutexCreate();
    if (!pClient->lock_generic) {
        return FAIL_RETURN;
    }

    pClient->lock_yield = HAL_MutexCreate();
    if (!pClient->lock_yield) {
        goto RETURN;
    }

    pClient->handle_event.h_fp = pInitParams->handle_event.h_fp;
    pClient->handle_event.pcontext = pInitParams->handle_event.pcontext;

    mal_mc_recv_buf_init();

    mc_state = IOTX_MC_STATE_INITIALIZED;
    rc = SUCCESS_RETURN;

RETURN:
    mal_mc_set_client_state(pClient, mc_state);

    if (rc != SUCCESS_RETURN) {
        if (pClient->lock_generic) {
            HAL_MutexDestroy(pClient->lock_generic);
            pClient->lock_generic = NULL;
        }

        if (pClient->lock_yield) {
            HAL_MutexDestroy(pClient->lock_yield);
            pClient->lock_yield = NULL;
        }
    }

    return rc;
}

/************************  Public Interface ************************/
void *wrapper_mqtt_init(iotx_mqtt_param_t *mqtt_params)
{
    int err;
    iotx_mc_client_t   *pclient;
#ifndef PLATFORM_HAS_DYNMEM
    int idx;
#endif

#ifdef PLATFORM_HAS_DYNMEM
    pclient = (iotx_mc_client_t *)mal_malloc(sizeof(iotx_mc_client_t));
    if (NULL == pclient) {
        mal_err("not enough memory.");
        if (mqtt_params != NULL) {
            mal_free(mqtt_params);
        }
        return NULL;
    }
#else
    for (idx = 0; idx < IOTX_MC_CLIENT_MAX_COUNT; idx++) {
        if (g_iotx_mc_client[idx].used == 0) {
            g_iotx_mc_client[idx].used = 1;
            pclient = &g_iotx_mc_client[idx];
            break;
        }
    }

    if (NULL == pclient) {
        mal_err("wrapper_mqtt_init IOTX_MC_CLIENT_MAX_COUNT too short: %d", IOTX_MC_CLIENT_MAX_COUNT);
        return NULL;
    }
#endif
    err = mal_mc_init(pclient, mqtt_params);
    if (SUCCESS_RETURN != err) {
        mal_err("mal_mc_init failed");
#ifdef PLATFORM_HAS_DYNMEM
        mal_free(pclient);
#else
        memset(pclient, 0, sizeof(iotx_mc_client_t));
#endif
        return NULL;
    }

    return pclient;
}

int wrapper_mqtt_connect(void *client)
{
    int rc = FAIL_RETURN;

    if (NULL == client) {
        return NULL_VALUE_ERROR;
    }

    rc = MALMQTTConnect((iotx_mc_client_t *)client);
    if (rc  != SUCCESS_RETURN) {
        mal_err("send connect packet failed");
        return  rc;
    }
    if (SUCCESS_RETURN != mal_mc_wait_for_result()) {
        mal_err("current state is not connected");
        return FAIL_RETURN;
    }

    mal_mc_set_client_state((iotx_mc_client_t *)client, IOTX_MC_STATE_CONNECTED);

    mal_info("mqtt connect success!");
    return SUCCESS_RETURN;
}

int wrapper_mqtt_yield(void *client, int timeout_ms)
{
    int                rc = SUCCESS_RETURN;
    mal_time_t         time;
    unsigned int       left_t;

    iotx_mc_client_t *pClient = (iotx_mc_client_t *)client;

    if (pClient == NULL) {
        return NULL_VALUE_ERROR;
    }

    if (timeout_ms < 0) {
        mal_err("Invalid argument, timeout_ms = %d", timeout_ms);
        return -1;
    }
    if (timeout_ms == 0) {
        timeout_ms = 10;
    }

    mal_time_init(&time);
    mal_time_countdown_ms(&time, timeout_ms);

    do {
        if (SUCCESS_RETURN != rc) {
            unsigned int left_t = mal_time_left(&time);
            /*mal_info("error occur or no data");*/
            if (left_t < 20) {
                HAL_SleepMs(left_t);
            } else {
                HAL_SleepMs(20);
            }
        }
        HAL_MutexLock(pClient->lock_yield);

        /* acquire package in cycle, such as PUBLISH */
        rc = mal_mc_cycle(pClient, &time);
        HAL_MutexUnlock(pClient->lock_yield);

        left_t = mal_time_left(&time);
        if (left_t < 10) {
            HAL_SleepMs(left_t);
        } else {
            HAL_SleepMs(10);
        }
    } while (!mal_time_is_expired(&time));

    return 0;
}

int wrapper_mqtt_check_state(void *client)
{
    if (!client) {
        return 0;
    }

    if (mal_mc_get_client_state((iotx_mc_client_t *)client) == IOTX_MC_STATE_CONNECTED) {
        return 1;
    }

    return 0;
}

int wrapper_mqtt_subscribe_sync(void *client,
                                const char *topicFilter,
                                iotx_mqtt_qos_t qos,
                                iotx_mqtt_event_handle_func_fpt topic_handle_func,
                                void *pcontext,
                                int timeout_ms)
{
    int rc = FAIL_RETURN;
    unsigned int msgId;
    iotx_mc_client_t *c = (iotx_mc_client_t *)client;

    if (NULL == client || NULL == topicFilter || strlen(topicFilter) == 0 || !topic_handle_func) {
        mal_err(" paras error");
        return NULL_VALUE_ERROR;
    }

    c = (iotx_mc_client_t *)client;
    msgId = mal_mc_get_next_packetid(c);

    if (!wrapper_mqtt_check_state(c)) {
        mal_err("mqtt client state is error,state = %d", mal_mc_get_client_state(c));
        return MQTT_STATE_ERROR;
    }

    if (0 != mal_mc_check_topic(topicFilter, TOPIC_FILTER_TYPE)) {
        mal_err("topic format is error,topicFilter = %s", topicFilter);
        return MQTT_TOPIC_FORMAT_ERROR;
    }

    mal_debug("PERFORM subscribe to '%s' (msgId=%d)", topicFilter, msgId);
    rc = MALMQTTSubscribe(c, topicFilter, qos, msgId, topic_handle_func, pcontext, timeout_ms);
    if (rc != SUCCESS_RETURN) {
        if (rc == MQTT_NETWORK_ERROR) {
            mal_mc_set_client_state(c, IOTX_MC_STATE_DISCONNECTED);
        }

        mal_err("run MQTTSubscribe error, rc = %d", rc);
        return rc;
    }

    mal_info("mqtt subscribe packet sent,topic = %s!", topicFilter);
    return msgId;
}

int wrapper_mqtt_subscribe(void *client,
                           const char *topic_filter,
                           iotx_mqtt_qos_t qos,
                           iotx_mqtt_event_handle_func_fpt topic_handle_func,
                           void *pcontext)
{
    return wrapper_mqtt_subscribe_sync(client, topic_filter, qos, topic_handle_func, pcontext, MAL_TIMEOUT_DEFAULT);
}

int wrapper_mqtt_unsubscribe(void *client, const char *topicFilter)
{
    int rc = FAIL_RETURN;
    unsigned int msgId;
    iotx_mc_client_t *c;

    if (NULL == client || NULL == topicFilter) {
        return NULL_VALUE_ERROR;
    }

    c = (iotx_mc_client_t *)client;
    msgId = mal_mc_get_next_packetid(c);

    if (0 != mal_mc_check_topic(topicFilter, TOPIC_FILTER_TYPE)) {
        mal_err("topic format is error,topicFilter = %s", topicFilter);
        return MQTT_TOPIC_FORMAT_ERROR;
    }

    if (!wrapper_mqtt_check_state(c)) {
        mal_err("mqtt client state is error,state = %d", mal_mc_get_client_state(c));
        return MQTT_STATE_ERROR;
    }

    rc = MALMQTTUnsubscribe(c, topicFilter, msgId);
    if (rc != SUCCESS_RETURN) {
        if (rc == MQTT_NETWORK_ERROR) { /* send the subscribe packet */
            mal_mc_set_client_state(c, IOTX_MC_STATE_DISCONNECTED);
        }

        mal_err("run MALMQTTUnsubscribe error!");
        return rc;
    }

    mal_info("mqtt unsubscribe packet sent,topic = %s!", topicFilter);
    return (int)msgId;
}

int wrapper_mqtt_publish(void *client, const char *topicName, iotx_mqtt_topic_info_pt topic_msg)
{
    uint16_t msg_id = 0;
    int rc = FAIL_RETURN;
    iotx_mc_client_t *c = (iotx_mc_client_t *)client;

    if (NULL == c || NULL == topicName || NULL == topic_msg || NULL == topic_msg->payload) {
        return NULL_VALUE_ERROR;
    }

    if (0 != mal_mc_check_topic(topicName, TOPIC_NAME_TYPE)) {
        mal_err("topic format is error,topicFilter = %s", topicName);
        return MQTT_TOPIC_FORMAT_ERROR;
    }

    if (!wrapper_mqtt_check_state(c)) {
        mal_err("mqtt client state is error,state = %d", mal_mc_get_client_state(c));
        return MQTT_STATE_ERROR;
    }

    if (topic_msg->qos == IOTX_MQTT_QOS1 || topic_msg->qos == IOTX_MQTT_QOS2) {
        msg_id = mal_mc_get_next_packetid(c);
        topic_msg->packet_id = msg_id;
    }

    if (topic_msg->qos == IOTX_MQTT_QOS2) {
        mal_err("MALMQTTPublish return error,MQTT_QOS2 is now not supported.");
        return MQTT_PUBLISH_QOS_ERROR;
    }

    rc = MALMQTTPublish(c, topicName, topic_msg);
    if (rc != SUCCESS_RETURN) { /* send the subscribe packet */
        if (rc == MQTT_NETWORK_ERROR) {
            mal_mc_set_client_state(c, IOTX_MC_STATE_DISCONNECTED);
        }
        mal_err("MALMQTTPublish is error, rc = %d", rc);
        return rc;
    }

    return (int)msg_id;
}

int wrapper_mqtt_release(void **client)
{
    iotx_mc_client_t *pClient;

    if (NULL == client) {
        return NULL_VALUE_ERROR;
    }

    pClient = (iotx_mc_client_t *)*client;
    if (NULL == pClient) {
        return NULL_VALUE_ERROR;
    }

    /* iotx_delete_thread(pClient); */
    mal_mc_disconnect(pClient);
    mal_mc_set_client_state(pClient, IOTX_MC_STATE_INVALID);
#ifdef PLATFORM_HAS_DYNMEM
    if (pClient->first_sub_handle != NULL) {
        iotx_mc_topic_handle_t *handler = pClient->first_sub_handle;
        iotx_mc_topic_handle_t *next_handler = pClient->first_sub_handle;
        while (handler) {
            next_handler = handler->next;
            if (handler->topic_filter != NULL) {
                mal_free(handler->topic_filter);
                handler->topic_filter = NULL;
            }
            mal_free(handler);
            handler = next_handler;
        }
    }
#else
    memset(pClient->list_sub_handle, 0, sizeof(iotx_mc_topic_handle_t) * IOTX_MC_SUBHANDLE_LIST_MAX_LEN);
#endif
    HAL_MutexDestroy(pClient->lock_generic);
    HAL_MutexDestroy(pClient->lock_yield);

    mal_mc_recv_buf_deinit();
#ifdef PLATFORM_HAS_DYNMEM
    mal_free(pClient);
    *client = NULL;
#else
    memset(pClient, 0, sizeof(iotx_mc_client_t) * IOTX_MC_CLIENT_MAX_COUNT);
#endif
    mal_info("mqtt release!");
    return SUCCESS_RETURN;
}
