/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "iotx_dm_internal.h"

#ifdef LOG_REPORT_TO_CLOUD

#include "dev_model_api.h"

const char THING_LOG_POST_PARAMS_HEAD[] =
            "\"%.*s %s %ld ";
const char THING_LOG_POST_PARAMS_BODY[] =
            "%s %ld ";
const char THING_LOG_POST_PARAMS_END[] =
            "%s %ld\",";

char *g_log_poll = NULL;
static char *current_log_pos = NULL;

int remove_log_poll()
{
    if (NULL != g_log_poll) {
        HAL_Free(g_log_poll);
        g_log_poll = NULL;
        current_log_pos = NULL;
    }
    return 0;
}


unsigned int push_log(const char *input_log, int input_log_size)
{
    if (NULL == current_log_pos || NULL == input_log || input_log_size <= 0) {
        dm_log_debug("invalid params");
        return (unsigned int)-1;
    }
    memcpy(current_log_pos, input_log, input_log_size);
    current_log_pos += input_log_size;
    return (current_log_pos - g_log_poll);
}

unsigned int add_tail()
{
    const char *tail = "]}";
    current_log_pos -= 1;
    return push_log(tail, strlen(tail));
}

void add_log_header()
{
    const char *subprefix = "{\"template\": \"traceContext logContent\",\"contents\":[";
    int sublen = strlen(subprefix);
    push_log(subprefix, sublen);
}

int reset_log_poll()
{
    if (NULL == g_log_poll) {
        dm_log_err("log buffer is NULL");
        return -1;
    }
    memset(g_log_poll, 0, LOG_POLL_SIZE);
    current_log_pos = g_log_poll;
    add_log_header();
    return 0;
}

int create_log_poll()
{
    int ret;
    remove_log_poll();
    g_log_poll = HAL_Malloc(LOG_POLL_SIZE);
    ret = reset_log_poll();
    return ret;
}

static int switch_status = 0; /* 0 for switch off; 1 for switch on */
static unsigned int sample_interval = 5;
static unsigned int sample_count  = 1000;

#define MSG_ID_LEN (64)
char msg_array[MSG_ID_LEN] = {0};
int check_target_msg(const char *input, int len)
{
    /* do not upload log when swith is off */
    if (0 == switch_status) {
        return -1;
    }
    if (NULL == input || len <= 0) {
        dm_log_err("invalid params");
        return -1;
    }
    return strncmp(input, msg_array, len);
}

static unsigned int msg_num = 0;
/* return 0 for success; -1 for failure */
int set_target_msg(const char *input, int len)
{
    if (0 == switch_status) {
        return -1;
    }
    if ((msg_num % sample_interval == 0) && (msg_num < sample_count)) {
        if (NULL == input || len <= 0) {
            dm_log_err("invalid params");
            return -1;
        }
        strncpy(msg_array, input, len);
        return 0;
    }
    return -1;
}

void parse_msg_id(_IN_ char *payload, _IN_ int payload_len, _OU_ dm_msg_request_payload_t *request)
{
    lite_cjson_t lite;

    if (payload == NULL || payload_len <= 0 || request == NULL) {
        return;
    }

    dm_utils_json_parse(payload, payload_len, cJSON_Object, &lite);
    dm_utils_json_object_item(&lite, DM_MSG_KEY_ID, strlen(DM_MSG_KEY_ID), cJSON_String, &request->id);
}

int stop_sample()
{
    if (current_log_pos > g_log_poll) {
        dm_mgr_upstream_thing_log_post(0, NULL, 0, 1);
    }
    switch_status = 0;
    dm_log_info("stop sample");
    return remove_log_poll();
}

void parse_switch_info(_IN_ char *payload, _IN_ int payload_len)
{
    lite_cjson_t lite, lite_sample_count, lite_sample_interval, lite_sample_target;
    const char *c1 = "Count";
    const char *c2 = "Interval";
    const char *c3 = "ProfileTarget";
    char *sample_target;
    int sample_target_len;
    const char *target = "propSet";
    int ret = -1;

    if (payload == NULL || payload_len <= 0) {
        return;
    }
    dm_utils_json_parse(payload, payload_len, cJSON_Object, &lite);
    ret = lite_cjson_object_item(&lite, c1, strlen(c1), &lite_sample_count);
    if (ret < SUCCESS_RETURN) {
        return;
    }

    ret = lite_cjson_object_item(&lite, c2, strlen(c2), &lite_sample_interval);
    if (ret < SUCCESS_RETURN) {
        return;
    }

    ret = lite_cjson_object_item(&lite, c3, strlen(c3), &lite_sample_target);
    if (ret < SUCCESS_RETURN) {
        return;
    }

    sample_count = lite_sample_count.value_int;
    sample_interval = lite_sample_interval.value_int;
    sample_target = lite_sample_target.value;
    sample_target_len  = lite_sample_target.value_length;
    dm_log_info("switch count is %d, interval is %d, target is %.*s\n", sample_count, sample_interval,
                sample_target_len, sample_target);
    /* if the target is not property set, return */
    if (0 != strncmp(sample_target, target, sample_target_len)) {
        dm_log_info("target is not propSet, return\n");
        return;
    }
    if (sample_interval <= 0) {
        dm_log_err("invalid sample interval\n");
        return;
    }
    msg_num = 0;
    /* when it switch off, force upload the remaining log */
    if (0 == sample_count) {
        ret = stop_sample();
    } else {
        switch_status = 1;
        ret = create_log_poll();
    }

    dm_log_info("log switch run status is %d\n", ret);
}

REPORT_STATE g_report_status = READY;

void send_permance_info(char *input, int input_len, char *comments, int report_format)
{
#define LOCAL_POST_LEN (150)
    char data[LOCAL_POST_LEN] = {0};
    const char *format = NULL;
    if (0 == switch_status) {
        return;
    }

    switch (report_format) {
        case 0:
            if (NULL == input || input_len <= 0) {
                dm_log_err("invalid params");
                return;
            }
            format = THING_LOG_POST_PARAMS_HEAD;
            HAL_Snprintf(data, sizeof(data), format, input_len, input,
                         comments, (long)HAL_UptimeMs());
            break;
        case 1:
            format = THING_LOG_POST_PARAMS_BODY;
            HAL_Snprintf(data, sizeof(data), format,
                         comments, (long)HAL_UptimeMs());
            break;
        case 2:
            format = THING_LOG_POST_PARAMS_END;
            HAL_Snprintf(data, sizeof(data), format,
                         comments, (long)HAL_UptimeMs());
            g_report_status = DONE;
            break;
        default:
            return;
    }
    iotx_dm_log_post(0,  data, strlen((const char *)data));
    if (2 == report_format) {
        g_report_status = READY;
    }
}

void get_msgid(char *payload, int is_cloud)
{
    const char *interest = "\"method\":\"thing.service.property.set";
    char *found;
    dm_msg_request_payload_t request;
    if (0 == switch_status || NULL == payload) {
        return;
    }

    found = strstr(payload, interest);
    if (NULL == found) {
        return;
    }
    found = strstr(payload, "{");
    if (NULL == found) {
        return;
    }
    msg_num++;
    parse_msg_id(found, strlen(found), &request);
    if (RUNNING == g_report_status) {
        dm_log_info("current working on a sample, return");
        return;
    }

    if (sample_count <= msg_num) {
        stop_sample();
        return;
    }

    /* if it does not meet the sample conditions, do NOT take sample */
    if (SUCCESS_RETURN != set_target_msg(request.id.value, request.id.value_length)) {
        return;
    }

    g_report_status = RUNNING;

    if (1 == is_cloud) {
        send_permance_info(request.id.value, request.id.value_length, "1_cloud", 0);
    } else if (0 == is_cloud) {
        send_permance_info(request.id.value, request.id.value_length, "1_alcs", 0);
    }
}
#endif
