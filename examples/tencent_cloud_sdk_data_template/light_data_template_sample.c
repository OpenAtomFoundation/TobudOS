/*
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright (C) 2016 THL A29 Limited, a Tencent company. All rights reserved.

 * Licensed under the MIT License (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT

 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "qcloud.h"
#include "tos.h"

#if (QCLOUD_CFG_EVENT_EN > 0u)

#define EVENT_MESSAGE_MAX           (64)
#define EVENT_NAME_MAX              (64)

typedef enum event_status_en {
    EVENT_STATUS_LIGHT_OFF,
    EVENT_STATUS_LIGHT_ON,
} event_status_t;

static template_bool_t      event_status;
static template_string_t    event_message[EVENT_MESSAGE_MAX + 1];
static template_float_t     event_voltage;
static template_string_t    event_name[EVENT_NAME_MAX + 1];
static template_int_t       event_error_code;

static shadow_dev_property_t event_property_status[] = {
    { .key = "status",      .data = &event_status,      .type = JSON_DATA_TYPE_BOOL },
    { .key = "message",     .data = event_message,      .type = JSON_DATA_TYPE_STRING },
};

static shadow_dev_property_t event_property_low_voltage[] = {
    { .key = "voltage",     .data = &event_voltage,     .type = JSON_DATA_TYPE_FLOAT },
};

static shadow_dev_property_t event_property_hardware_fault[] = {
    { .key = "name",        .data = event_name,         .type = JSON_DATA_TYPE_STRING },
    { .key = "error_code",  .data = &event_error_code,  .type = JSON_DATA_TYPE_INT32},
};

static qcloud_event_t events[] = {
    {
        .event_name             = "status_report",
        .type                   = "info",
        .timestamp              = 0,
        .event_payload_count    = sizeof(event_property_status) / sizeof(event_property_status[0]),
        .event_payload          = event_property_status,
    },
    {
        .event_name             = "low_voltage",
        .type                   = "alert",
        .timestamp              = 0,
        .event_payload_count    = sizeof(event_property_low_voltage) / sizeof(event_property_low_voltage[0]),
        .event_payload          = event_property_low_voltage,
    },
    {
        .event_name             = "hardware_fault",
        .type                   = "fault",
        .timestamp              = 0,
        .event_payload_count    = sizeof(event_property_hardware_fault) / sizeof(event_property_hardware_fault[0]),
        .event_payload          = event_property_hardware_fault,
    },
};

#define EVENT_COUNTS            (sizeof(events) / sizeof(events[0]))

#define EVENT_FLAG_STATUS           (1u << 0)
#define EVENT_FLAG_LOW_VOLTAGE      (1u << 1)
#define EVENT_FLAG_HARDWARE_FAULT   (1u << 2)

static uint32_t event_flags = 0;

void event_flag_set(uint32_t flag)
{
	event_flags |= flag & 0xffffffff;
}

void event_flag_unset(uint32_t flag)
{
	event_flags &= (~flag) & 0xffffffff;
}

uint32_t event_flag_get(void)
{
	return event_flags;
}

void event_flag_clear(void)
{
    event_flags = 0;
}

/*-----------------event config end	-------------------*/

static void update_event_timestamp(qcloud_event_t *event)
{
#ifdef EVENT_TIMESTAMP_USED
    event->timestamp = (uint32_t)tos_systick_get(); //should be UTC and accurate
#else
    event->timestamp = 0;
#endif
}

static void on_event_post_handler(void *client, mqtt_incoming_msg_t *msg)
{
    QCLOUD_LOG_D("reply:%.*s", msg->payload_len, msg->payload);
}

#endif

/////////////////////////////////////////////////////////////////////////////////////
// dealing with product property template

#define LIGHT_NAME_MAX      64

typedef enum light_color_en {
    LIGHT_COLOR_RED     = 0,
    LIGHT_COLOR_GREEN   = 1,
    LIGHT_COLOR_BLUE    = 2,
} light_color_t; // a enum to describe the color of a light

typedef enum light_switch_state_en {
    LIGHT_SWTICH_STATE_OFF    = 0,
    LIGHT_SWTICH_STATE_ON     = 1,
} light_switch_state_t; // a enum to describe the switch state of a light

typedef enum property_state_en {
    PROPERTY_STATE_NOCHANGE,
    PROPERTY_STATE_CHANGED,
} property_state_t; // a enum to describe whether the property state has changed

typedef struct property_wrapper_st {
    shadow_dev_property_t   property;
    property_state_t        state;
} property_wrapper_t; // a wrapper of one property(contains the property and change state)

/* describe a light property */
typedef struct light_property_st {
    property_wrapper_t switch_state;
    property_wrapper_t color;
    property_wrapper_t brightness;
    property_wrapper_t name;
} light_property_t; // all the properties of a light

// how many properties of a light?
#define LIGHT_PROPERTY_COUNT        (sizeof(light_property_t) / sizeof(property_wrapper_t))

typedef union light_property_handler_un {
    property_wrapper_t  property_wrappers[LIGHT_PROPERTY_COUNT];
    light_property_t    property_wrappers_of;
} light_property_handler_t; // a handler, user to iterate through all the properties

typedef struct light_profile_st {
    template_bool_t     switch_state;
    template_int_t      color;
    template_float_t    brightness;
    template_string_t   name[LIGHT_NAME_MAX + 1];
} light_profile_t; // just a data container

static int                          is_light_property_changed = QCLOUD_FALSE;
static int                          is_new_property_reported = QCLOUD_FALSE;
static light_profile_t              light_profile;
static light_property_handler_t     light_property_handler;

static void data_template_init(qcloud_device_t *device)
{
    memset((void *)&light_profile, 0, sizeof(light_profile_t));

    light_profile.switch_state  = LIGHT_SWTICH_STATE_OFF;
    light_profile.color         = LIGHT_COLOR_RED;
    light_profile.brightness    = 0.0;

    strncpy(light_profile.name, device->device_name, LIGHT_NAME_MAX);
    light_profile.name[LIGHT_NAME_MAX] = '\0';


    light_property_handler.property_wrappers_of.switch_state.property.key   = "power_switch";
    light_property_handler.property_wrappers_of.switch_state.property.data  = &light_profile.switch_state;
    light_property_handler.property_wrappers_of.switch_state.property.type  = JSON_DATA_TYPE_BOOL;

    light_property_handler.property_wrappers_of.color.property.key          = "color";
    light_property_handler.property_wrappers_of.color.property.data         = &light_profile.color;
    light_property_handler.property_wrappers_of.color.property.type         = JSON_DATA_TYPE_INT32;

    light_property_handler.property_wrappers_of.brightness.property.key     = "brightness";
    light_property_handler.property_wrappers_of.brightness.property.data    = &light_profile.brightness;
    light_property_handler.property_wrappers_of.brightness.property.type    = JSON_DATA_TYPE_FLOAT;

    light_property_handler.property_wrappers_of.name.property.key           = "name";
    light_property_handler.property_wrappers_of.name.property.data          = &light_profile.name;
    light_property_handler.property_wrappers_of.name.property.type          = JSON_DATA_TYPE_STRING;
};

static int is_self_property(json_data_type_t property_type)
{
    return property_type == JSON_DATA_TYPE_BOOL || // power switch
            property_type == JSON_DATA_TYPE_INT32|| // color
            property_type == JSON_DATA_TYPE_FLOAT || // brightness
            property_type == JSON_DATA_TYPE_STRING; // device name
}

static void property_do_update(shadow_dev_property_t *property)
{
    switch (property->type) {
        case JSON_DATA_TYPE_BOOL: // power switch
            light_profile.switch_state = *(template_bool_t *)property->data;
            break;

        case JSON_DATA_TYPE_INT32: // color
            light_profile.color = *(template_int_t *)property->data;
            break;

        case JSON_DATA_TYPE_FLOAT: // brightness
            light_profile.brightness = *(template_float_t *)property->data;
            break;

        case JSON_DATA_TYPE_STRING: // device name
            /* å¦‚æžœå¤šä¸ªå­—ç¬¦ä¸²å±žæ€§,æ ¹æ®pProperty->keyå€¼åŒ¹é…ï¼Œå¤„ç†å­—ç¬¦ä¸² */
            if (strcmp("name", property->key) != 0) {
                break;
            }

            memset(light_profile.name, 0, sizeof(light_profile.name));
            strncpy(light_profile.name, property->data, LIGHT_NAME_MAX);
            light_profile.name[LIGHT_NAME_MAX] = '\0';
            break;
    }
}

/* Èç¹ûÓÐ×Ô¶¨ÒåµÄ×Ö·û´®»òÕßjson£¬ÐèÒªÔÚÕâÀï½âÎö */
static qcloud_err_t property_update(const char *json_doc, shadow_dev_property_t *property)
{
    QCLOUD_POINTER_SANITY_CHECK(json_doc, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(property, QCLOUD_ERR_INVAL);

    char *json_doc_mutable = NULL, *property_data = NULL;

    /* convert const char * to char * */
    json_doc_mutable = osal_malloc(strlen(json_doc));
    QCLOUD_FUNC_EXIT_RC_IF(json_doc_mutable, NULL, QCLOUD_ERR_FAILURE);
    strcpy(json_doc_mutable, json_doc);

    property_data = LITE_json_value_of((char *)property->key, json_doc_mutable);
    if (!property_data) {
        QCLOUD_LOG_D("property:%s no matched", property->key);
        osal_free(json_doc_mutable);
        return QCLOUD_ERR_FAILURE;
    }

    property_do_update(property);

    osal_free(property_data);
    osal_free(json_doc_mutable);

    return QCLOUD_ERR_SUCCESS;
}

/* æœåŠ¡ç«¯æœ‰æŽ§åˆ¶æ¶ˆæ¯ä¸‹å‘ï¼Œä¼šè§¦å‘è¿™é‡Œçš„deltaå›žè°ƒ */
static void on_property_delta_handler(void *client, const char *json_doc, uint32_t json_doc_len, shadow_dev_property_t *property)
{
    int i = 0;

    for (i = 0; i < LIGHT_PROPERTY_COUNT; ++i) {
        /* ÆäËûÊý¾ÝÀàÐÍÒÑ¾­ÔÚ_handle_deltaÁ÷³ÌÍ³Ò»´¦ÀíÁË£¬×Ö·û´®ºÍjson´®ÐèÒªÔÚÕâÀï´¦Àí£¬ÒòÎªÖ»ÓÐ²úÆ·×Ô¼º²ÅÖªµÀstring/jsonµÄ×Ô¶¨Òå½âÎö */
        if (strcmp(light_property_handler.property_wrappers[i].property.key, property->key) != 0) {
            continue;
        }

        light_property_handler.property_wrappers[i].state = PROPERTY_STATE_CHANGED;

        if (is_self_property(light_property_handler.property_wrappers[i].property.type)) {
            property_update(json_doc, &(light_property_handler.property_wrappers[i].property));
        }

        QCLOUD_LOG_I("property=%s changed", property->key);
        is_light_property_changed = QCLOUD_TRUE;
        return;
    }

    QCLOUD_LOG_E("property=%s changed no match", property->key);
}

/* ×¢²áÊý¾ÝÄ£°åÊôÐÔ */
static qcloud_err_t data_template_property_register(qcloud_shadow_client_t *client)
{
    int i = 0;
    qcloud_err_t rc;

    for (i = 0; i < LIGHT_PROPERTY_COUNT; ++i) {
        rc = qcloud_shadow_device_property_register(client, &light_property_handler.property_wrappers[i].property, on_property_delta_handler);

        if (rc != QCLOUD_ERR_SUCCESS) {
            rc = qcloud_shadow_client_destroy(client);
            QCLOUD_LOG_E("device template property register failed, err: %d", rc);
            return rc;
        }

        QCLOUD_LOG_I("data template property=%s registered.", light_property_handler.property_wrappers[i].property.key);
    }

    return QCLOUD_ERR_SUCCESS;
}
/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
__weak void OLED_Clear(void)
{
    printf("OLED Clear\n");
}

__weak void OLED_ShowString(int x, int y, uint8_t *str, int bold)
{
    printf("OLED ShowString: %s\n", (char *)str);
}

// handle the light(simulated)
static void light_change_color(const char *color)
{
    // ä½œä¸ºdemoï¼Œè¿™é‡Œç”¨oledå±å­—ç¬¦æ˜¾ç¤ºæ¥æ¨¡æ‹Ÿç¯é¢œè‰²çš„åˆ‡æ¢
    // è¿™é‡Œåº”è¯¥ç”±ç”¨æˆ·å®žçŽ°ç¡¬ä»¶æ“ä½œä»£ç ï¼Œæ¥æ”¹å˜æ™ºèƒ½ç¯çš„é¢œè‰²
    // æ­¤å¤„demoï¼Œåœ¨å¼€å‘æ¿æ˜¾ç¤ºå±ä¸Šæ˜¾ç¤ºå…·ä½“çš„é¢œè‰²
    OLED_ShowString(0, 0, (uint8_t *)color, 8);
}

static void light_change_brightness(template_float_t brightness)
{
    // ä½œä¸ºdemoï¼Œè¿™é‡Œç”¨oledå±å­—ç¬¦æ˜¾ç¤ºæ¥æ¨¡æ‹Ÿç¯äº®åº¦çš„åˆ‡æ¢
    // è¿™é‡Œåº”è¯¥ç”±ç”¨æˆ·å®žçŽ°ç¡¬ä»¶æ“ä½œä»£ç ï¼Œæ¥æ”¹å˜æ™ºèƒ½ç¯çš„äº®åº¦
    // æ­¤å¤„demoï¼Œåœ¨å¼€å‘æ¿æ˜¾ç¤ºå±ä¸Šæ˜¾ç¤ºå…·ä½“çš„äº®åº¦
    char brightness_str[12];

    snprintf(brightness_str, sizeof(brightness_str), "%f", brightness);
    brightness_str[sizeof(brightness_str) - 1] = '\0';
    OLED_ShowString(0, 2, (uint8_t *)brightness_str, 8);
}

static void light_power_on(void)
{
    // ä½œä¸ºdemoï¼Œè¿™é‡Œç”¨oledå±å­—ç¬¦æ˜¾ç¤ºæ¥æ¨¡æ‹Ÿç¯å¼€å…³çŠ¶æ€çš„åˆ‡æ¢
    OLED_Clear();
}

static void light_power_off(void)
{
    // ä½œä¸ºdemoï¼Œè¿™é‡Œç”¨oledå±å­—ç¬¦æ˜¾ç¤ºæ¥æ¨¡æ‹Ÿç¯å¼€å…³çŠ¶æ€çš„åˆ‡æ¢
    char *info = "light off";
    OLED_Clear();
    OLED_ShowString(0, 0, (uint8_t *)info, 16);
}
/////////////////////////////////////////////////////////////////////////////////////

static void incoming_messsage_handler(void *client, void *context, mqtt_event_t *event)
{
    uint16_t packet_id = *(uint16_t *)event->message;

    switch (event->type) {
        case MQTT_EVENT_UNDEF:
            QCLOUD_LOG_I("undefined event occur.");
            break;

        case MQTT_EVENT_DISCONNECT:
            QCLOUD_LOG_I("MQTT disconnect.");
            break;

        case MQTT_EVENT_RECONNECT:
            QCLOUD_LOG_I("MQTT reconnect.");
            break;

        case MQTT_EVENT_SUBCRIBE_SUCCESS:
            QCLOUD_LOG_I("subscribe success, packet-id=%u", (uint32_t)packet_id);
            break;

        case MQTT_EVENT_SUBCRIBE_TIMEOUT:
            QCLOUD_LOG_I("subscribe wait ack timeout, packet-id=%u", (uint32_t)packet_id);
            break;

        case MQTT_EVENT_SUBCRIBE_NACK:
            QCLOUD_LOG_I("subscribe nack, packet-id=%u", (uint32_t)packet_id);
            break;

        case MQTT_EVENT_PUBLISH_SUCCESS:
            QCLOUD_LOG_I("publish success, packet-id=%u", (uint32_t)packet_id);
            break;

        case MQTT_EVENT_PUBLISH_TIMEOUT:
            QCLOUD_LOG_I("publish timeout, packet-id=%u", (uint32_t)packet_id);
            break;

        case MQTT_EVENT_PUBLISH_NACK:
            QCLOUD_LOG_I("publish nack, packet-id=%u", (uint32_t)packet_id);
            break;
        default:
            QCLOUD_LOG_I("Should NOT arrive here.");
            break;
    }
}

/* Ê¾ÀýµÆ¹â¿ØÖÆ´¦ÀíÂß¼­ */
static void deal_down_stream_user_logic(void)
{
    char *color_name;

    switch (light_profile.color) {
        case LIGHT_COLOR_RED:
            color_name = " RED ";
            break;

        case LIGHT_COLOR_GREEN:
            color_name = "GREEN";
            break;

        case LIGHT_COLOR_BLUE:
            color_name = "BLUE";
            break;
    }

    if (light_profile.switch_state == LIGHT_SWTICH_STATE_ON) {
        light_power_on();
        light_change_color(color_name);
        light_change_brightness(light_profile.brightness);
    } else {
        light_power_off();
    }

#if (QCLOUD_CFG_EVENT_EN > 0u)
    if (light_property_handler.property_wrappers_of.switch_state.state == PROPERTY_STATE_CHANGED) {
        if (light_profile.switch_state == LIGHT_SWTICH_STATE_ON) {
            strcpy(event_message, "light on");
            event_status = EVENT_STATUS_LIGHT_ON;
        } else {
            strcpy(event_message, "light off");
            event_status = EVENT_STATUS_LIGHT_OFF;
        }

        event_flag_set(EVENT_FLAG_STATUS);
    }
#endif
}

/* ÓÃ»§ÐèÒªÊµÏÖµÄÉÏÐÐÊý¾ÝµÄÒµÎñÂß¼­,´Ë´¦½ö¹©Ê¾Àý */
static void deal_up_stream_user_logic(shadow_dev_property_t *properties_report[], int *count)
{
    int i, j;

    *count = 0;

    /* ÌáÈ¡³ö·¢Éú±ä¸üÁËµÄÊôÐÔ£¬ÉÏ±¨µ½ÔÆ */
    for (i = 0, j = 0; i < LIGHT_PROPERTY_COUNT; ++i) {
        if (light_property_handler.property_wrappers[i].state == PROPERTY_STATE_CHANGED) {
            properties_report[j++] = &(light_property_handler.property_wrappers[i].property);
            light_property_handler.property_wrappers[i].state = PROPERTY_STATE_NOCHANGE;
        }
    }
    *count = j;
}

static void on_shadow_update_handler(void *client,
                                                        qcloud_shadow_req_method_t method,
                                                        qcloud_shadow_req_state_t req_state,
                                                        const char *json_doc,
                                                        void *context)
{
    QCLOUD_LOG_I("recv shadow update response, request state: %d", req_state);
}

/* 5s¶¨Ê±ÉÏ±¨ÊôÐÔ×´Ì¬,¿É¸ù¾ÝÒµÎñ²Ã¼ô£¬´Ë´¦½ö¹©Ê¾Àý */
qcloud_err_t timely_reporting(shadow_dev_property_t *properties_report[], osal_timer_t *report_timer)
{
    int i;

    if (osal_timer_is_expired(report_timer)){
        for (i = 0; i < LIGHT_PROPERTY_COUNT; ++i) {
            properties_report[i] = &(light_property_handler.property_wrappers[i].property);
            osal_timer_countdown_ms(report_timer, 5000);
        }
        return QCLOUD_ERR_SUCCESS;
    }

    return QCLOUD_ERR_INVAL;
}

static qcloud_device_t device;
static qcloud_shadow_client_t shadow_client;
static char shadow_update_buffer[2048];
static shadow_dev_property_t *properties_report[LIGHT_PROPERTY_COUNT];

#if (QCLOUD_CFG_EVENT_EN > 0u)
static qcloud_event_client_t event_client;
#endif

int data_template_light_thread(void)
{
    qcloud_err_t rc;
    int properties_report_count = 0;
    osal_timer_t report_timer;

#if (QCLOUD_CFG_EVENT_EN > 0u)
    uint32_t event_flag;
    int event_count;
    qcloud_event_t *events2report[EVENT_COUNTS];
#endif

    QCLOUD_LOG_I("data template sample start");

    qcloud_device_create(&device, "XC31USKYPL", "dev001", "Pz1wK0fVJHxSojqxDuuvmg==");

    qcloud_shadow_client_create(&shadow_client, &device, incoming_messsage_handler, SHADOW_TYPE_TEMPLATE);

    light_power_off();

    data_template_init(&device);

#if (QCLOUD_CFG_EVENT_EN > 0u)
    rc = qcloud_event_client_create(&event_client, &shadow_client, &device);
    if (rc != QCLOUD_ERR_SUCCESS) {
        QCLOUD_LOG_E("event init failed: %d", rc);
        return rc;
    }
#endif

    rc = data_template_property_register(&shadow_client);
    if (rc == QCLOUD_ERR_SUCCESS) {
        QCLOUD_LOG_I("data template propertys register success");
    } else {
        QCLOUD_LOG_E("data template propertys register failed: %d", rc);
        return rc;
    }

#define SHADOW_REQUEST_TIMEOUT          (10)    // in seconds
    rc = qcloud_shadow_client_get_sync(&shadow_client, SHADOW_REQUEST_TIMEOUT);
    if (rc != QCLOUD_ERR_SUCCESS) {
        QCLOUD_LOG_E("device shadow get failed, err: %d", rc);
        return rc;
    }

    // å±žæ€§å®šæ—¶ä¸ŠæŠ¥timerï¼Œå¯ä»¥æ ¹æ®ä¸šåŠ¡éœ€è¦è£å‰ªã€‚
    osal_timer_init(&report_timer);

    while (qcloud_shadow_client_is_connected(&shadow_client) ||
            rc == QCLOUD_ERR_MQTT_ATTEMPTING_RECONNECT ||
            rc == QCLOUD_ERR_MQTT_RECONNECTED ||
            rc == QCLOUD_ERR_SUCCESS) {

        rc = qcloud_shadow_client_yield(&shadow_client, 200);
        if (rc == QCLOUD_ERR_MQTT_ATTEMPTING_RECONNECT) {
            osal_sleep_ms(1000);
            continue;
        } else if (rc != QCLOUD_ERR_SUCCESS && rc != QCLOUD_ERR_MQTT_RECONNECTED) {
			QCLOUD_LOG_E("exit with error: %d", rc);
            break;
		}

        /* ·þÎñ¶ËÏÂÐÐÏûÏ¢£¬ÒµÎñ´¦ÀíÂß¼­1Èë¿Ú */
        if (is_light_property_changed) {
            // æœåŠ¡å™¨ä¸‹å‘äº†æŽ§åˆ¶æŒ‡ä»¤ï¼Œæ›´æ”¹äº†è®¾å¤‡çš„çŠ¶æ€
            deal_down_stream_user_logic();

            /* ÒµÎñÂß¼­´¦ÀíÍêºóÐèÒªÍ¬²½Í¨Öª·þÎñ¶Ë:Éè±¸Êý¾ÝÒÑ¸üÐÂ£¬É¾³ýdseireÊý¾Ý */
            rc = qcloud_shadow_client_desire_null_construct(&shadow_client, shadow_update_buffer, sizeof(shadow_update_buffer));
            if (rc == QCLOUD_ERR_SUCCESS) {
                rc = qcloud_shadow_client_update_sync(&shadow_client, shadow_update_buffer, sizeof(shadow_update_buffer), 5);
                if (rc == QCLOUD_ERR_SUCCESS) {
                    is_light_property_changed = QCLOUD_FALSE;

                     // ÓÃ»§ÐèÒª¸ù¾ÝÒµÎñÇé¿öÐÞ¸ÄÉÏ±¨flagµÄ¸³ÖµÎ»ÖÃ,´Ë´¦½öÎªÊ¾Àý¡£
                    is_new_property_reported = QCLOUD_TRUE;
                    QCLOUD_LOG_I("shadow update(desired) success");
                } else {
                    QCLOUD_LOG_E("shadow update(desired) failed, err: %d", rc);
                }
            } else {
                QCLOUD_LOG_E("construct desire failed, err: %d", rc);
            }
        }

        /* Éè±¸ÉÏÐÐÏûÏ¢,ÒµÎñÂß¼­2Èë¿Ú */
        if (is_new_property_reported) {
            /* deltaÏûÏ¢ÊÇÊôÐÔµÄdesireºÍÊôÐÔµÄreportµÄ²îÒì¼¯£¬ÊÕµ½deseireÏûÏ¢´¦Àíºó£¬ÒªreportÊôÐÔµÄ×´Ì¬ */
            deal_up_stream_user_logic(properties_report, &properties_report_count);
            if (properties_report_count > 0) {
                rc = qcloud_shadow_client_report_construct_array(&shadow_client, shadow_update_buffer, sizeof(shadow_update_buffer),
                                                                        properties_report_count, properties_report);
                if (rc == QCLOUD_ERR_SUCCESS) {
                    QCLOUD_LOG_D("report: %s", shadow_update_buffer);
                    rc = qcloud_shadow_client_update_async(&shadow_client, shadow_update_buffer, sizeof(shadow_update_buffer),
                                                                on_shadow_update_handler, NULL, 5);
                    if (rc == QCLOUD_ERR_SUCCESS) {
                        is_new_property_reported = QCLOUD_FALSE;
                        QCLOUD_LOG_I("shadow update(reported) success");
                    } else {
                        QCLOUD_LOG_E("shadow update(reported) failed, err: %d", rc);
                    }

                } else {
                    QCLOUD_LOG_E("construct reported failed, err: %d", rc);
                }
            } else {
                QCLOUD_LOG_D("no data need to be reported or someting goes wrong");
            }
        }


        if (QCLOUD_ERR_SUCCESS == timely_reporting(properties_report, &report_timer)){
            rc = qcloud_shadow_client_report_construct_array(&shadow_client, shadow_update_buffer, sizeof(shadow_update_buffer),
                                                                    LIGHT_PROPERTY_COUNT, properties_report);
            if (rc == QCLOUD_ERR_SUCCESS) {
                QCLOUD_LOG_D("cycle report:%s", shadow_update_buffer);
                rc = qcloud_shadow_client_update_async(&shadow_client, shadow_update_buffer, sizeof(shadow_update_buffer),
                                                            on_shadow_update_handler, NULL, 5);
                if (rc == QCLOUD_ERR_SUCCESS) {
                    is_new_property_reported = QCLOUD_FALSE;
                    QCLOUD_LOG_I("shadow update(reported) success");
                } else {
                    QCLOUD_LOG_E("shadow update(reported) failed, err: %d", rc);
                }
            } else {
                QCLOUD_LOG_E("construct reported failed, err: %d", rc);
            }
        }

#if (QCLOUD_CFG_EVENT_EN > 0u)
        // ÊÂ¼þÉÏ±¨
        event_count = 0;
        event_flag = event_flag_get();
        if (EVENT_COUNTS > 0 && event_flag > 0) {
            int i = 0;
            for (i = 0; i < EVENT_COUNTS; ++i) {
                if (event_flag & (1 << i)) { // i-th event is set
                    events2report[event_count++] = &events[i];
                    update_event_timestamp(&events[i]);
                }
            }

            rc = qcloud_event_client_post(&event_client, shadow_update_buffer, sizeof(shadow_update_buffer), \
                                                event_count, events2report, on_event_post_handler);
            if (rc != QCLOUD_ERR_SUCCESS) {
                QCLOUD_LOG_E("event post failed: %d", rc);
            }

            event_flag_clear();
        }
#endif

        osal_sleep_ms(3000);
    }

    qcloud_shadow_client_destroy(&shadow_client);
#if (QCLOUD_CFG_EVENT_EN > 0u)
    qcloud_event_client_destroy(&event_client);
#endif

    return -1;
}

