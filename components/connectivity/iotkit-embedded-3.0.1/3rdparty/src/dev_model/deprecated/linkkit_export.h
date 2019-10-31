/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef LINKKIT_EXPORT_H
#define LINKKIT_EXPORT_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdint.h>

typedef void (*handle_post_cb_fp_t)(const void *thing_id, int respons_id, int code, const char *response_message,
                                    void *ctx);
typedef void (*handle_subdev_cb_fp_t)(const void *thing_id, int code, const char *response_message, int success,
                                      void *ctx);

typedef struct _linkkit_ops {
#ifdef LOCAL_CONN_ENABLE
    int (*on_connect)(void *ctx, int cloud); /* true: cloud connection; false: local connection */
    int (*on_disconnect)(void *ctx, int cloud); /* true: cloud connection; false: local connection */
#else
    int (*on_connect)(void *ctx); /* true: cloud connection; false: local connection */
    int (*on_disconnect)(void *ctx); /* true: cloud connection; false: local connection */
#endif
    int (*raw_data_arrived)(const void *thing_id, const void *data, int len, void *ctx);
    int (*thing_create)(const void *thing_id, void *ctx);
    int (*thing_enable)(const void *thing_id, void *ctx);
    int (*thing_disable)(const void *thing_id, void *ctx);
    int (*thing_call_service)(const void *thing_id, const char *service, int request_id, void *ctx);
    int (*thing_prop_changed)(const void *thing_id, const char *property, void *ctx);
    int (*linkit_data_arrived)(const void *thing_id, const void *data, int len, void *ctx);
} linkkit_ops_t;

typedef enum _linkkit_loglevel {
    linkkit_loglevel_emerg = 0,
    linkkit_loglevel_crit,
    linkkit_loglevel_error,
    linkkit_loglevel_warning,
    linkkit_loglevel_info,
    linkkit_loglevel_debug,
} linkkit_loglevel_t;

/* domain type */
/* please sync with dm_cloud_domain_type_t */
typedef enum {
    /* shanghai */
    linkkit_cloud_domain_shanghai,
    /* singapore */
    linkkit_cloud_domain_singapore,
    /* japan */
    linkkit_cloud_domain_japan,
    /* america */
    linkkit_cloud_domain_america,
    /* germany */
    linkkit_cloud_domain_germany,

    linkkit_cloud_domain_max,
} linkkit_cloud_domain_type_t;

/* device info related operation */
typedef enum {
    linkkit_extended_info_operate_update,
    linkkit_extended_info_operate_delete,

    linkkit_deviceinfo_operate_max,
} linkkit_extended_info_operate_t;

/**
 * @brief dispatch message of queue for further process.
 *
 * @return void*
 */
void *linkkit_dispatch(void);

typedef enum {
    linkkit_opt_property_post_reply,  /* data type: int */
    linkkit_opt_event_post_reply,     /* data type: int */
    linkkit_opt_property_set_reply    /* data type: int */
} linkkit_opt_t;

/**
 * @brief get leave signal.
 *
 *
 * @return int,0 no leave signal, 1 get a leave signal
 */
int being_deprecated linkkit_is_try_leave();

/**
 * @brief start linkkit routines, and install callback funstions(async type for cloud connecting).
 *
 * @param opt, specify the option need to be set.
 * @param data, specify the option value.
 *
 * @return int, 0 when success, -1 when fail.
 */
int being_deprecated linkkit_set_opt(linkkit_opt_t opt, void *data);

/**
 * @brief start linkkit routines, and install callback funstions(async type for cloud connecting).
 *
 * @param max_buffered_msg, specify max buffered message size.
 * @param ops, callback function struct to be installed.
 * @param get_tsl_from_cloud, config if device need to get tsl from cloud(!0) or local(0), if local selected, must invoke linkkit_set_tsl to tell tsl to dm after start complete.
 * @param log_level, config log level.
 * @param user_context, user context pointer.
 * @param domain_type, specify the could server domain.
 *
 * @return int, 0 when success, -1 when fail.
 */
int being_deprecated linkkit_start(int max_buffered_msg, int get_tsl_from_cloud,
        linkkit_loglevel_t log_level,
        linkkit_ops_t *ops,
        linkkit_cloud_domain_type_t domain_type, void *user_context);

/**
 * @brief stop linkkit routines.
 *
 *
 * @return 0 when success, -1 when fail.
 */
int being_deprecated linkkit_end(void);

/**
 * @brief install user tsl.
 *
 * @param tsl, tsl string that contains json description for thing object.
 * @param tsl_len, tsl string length.
 *
 * @return pointer to thing object, NULL when fails.
 */
void *linkkit_set_tsl(const char *tsl, int tsl_len);

/* patterns: */
/* method:
 * set_property_/event_output_/service_output_value:
 * method_set, thing_id, identifier, value */

typedef enum {
    linkkit_method_set_property_value = 0,
    linkkit_method_set_event_output_value,
    linkkit_method_set_service_output_value,

    linkkit_method_set_number,
} linkkit_method_set_t;

/**
 * @brief set value to property, event output, service output items.
 *        if identifier is struct type or service output type or event output type, use '.' as delimeter like "identifier1.ientifier2"
 *        to point to specific item.
 *        value and value_str could not be NULL at the same time;
 *        if value and value_str both as not NULL, value shall be used and value_str will be ignored.
 *        if value is NULL, value_str not NULL, value_str will be used.
 *        in brief, value will be used if not NULL, value_str will be used only if value is NULL.
 *
 * @param method_set, specify set value type.
 * @param thing_id, pointer to thing object, specify which thing to set.
 * @param identifier, property, event output, service output identifier.
 * @param value. The value to be set, data type decided by data type of property as follows:
 *        int: int*, float: float*, double: double*,
 *        text: char*, enum: int*, date: char*, bool: int*
 *
 * @param value_str, value to set in string format if value is null.
 *
 * @return 0 when success, -1 when fail.
 */
int being_deprecated linkkit_set_value(linkkit_method_set_t method_set, const void *thing_id,
        const char *identifier,
        const void *value, const char *value_str);

typedef enum {
    linkkit_method_get_property_value = 0,
    linkkit_method_get_event_output_value,
    linkkit_method_get_service_input_value,
    linkkit_method_get_service_output_value,

    linkkit_method_get_number,
} linkkit_method_get_t;

/**
 * @brief get value from property, event output, service input/output items.
 *        if identifier is struct type or service input/output type or event output type, use '.' as delimeter like "identifier1.ientifier2"
 *        to point to specific item.
 *        value and value_str could not be NULL at the same time;
 *        if value and value_str both as not NULL, value shall be used and value_str will be ignored.
 *        if value is NULL, value_str not NULL, value_str will be used.
 *        in brief, value will be used if not NULL, value_str will be used only if value is NULL.
 * @param method_get, specify get value type.
 * @param thing_id, pointer to thing object, specify which thing to get.
 * @param identifier, property, event output, service input/output identifier.
 * @param value. The variable to store value, data type decided by data type of property as follows:
 *        int: int*, float: float*, double: double*,
 *        text: char**, enum: int*, date: char**, bool: int*
 *
 * @param value_str, value to get in string format. If success, memory of *value_str will be allocated,
 *        user should free the memory.
 *
 * @warning if data type is text or date, *value well be end with '\0'.
 *          the memory allocated to *value must be free by user.
 *
 * @return 0 when success, -1 when fail.
 */
int being_deprecated linkkit_get_value(linkkit_method_get_t method_get, const void *thing_id,
        const char *identifier,
        void *value, char **value_str);


/**
 * @brief answer to a service when a service requested by cloud.
 *
 * @param thing_id, pointer to thing object.
 * @param service_identifier, service identifier to answer, user should get this identifier from handle_dm_callback_fp_t type callback
 *        report that "dm_callback_type_service_requested" happened, use this function to generate response to the service sender.
 * @param response_id, id value in response payload. its value is from "dm_callback_type_service_requested" type callback function.
 *        use the same id as the request to send response as the same communication session.
 * @param code, code value in response payload. for example, 200 when service successfully executed, 400 when not successfully executed.
 *
 * @return 0 when success, -1 when fail.
 */
int being_deprecated linkkit_answer_service(const void *thing_id, const char *service_identifier,
        int response_id, int code);

/**
 * @brief answer a down raw service when a raw service requested by cloud, or invoke a up raw service to cloud.
 *
 * @param thing_id, pointer to thing object.
 * @param is_up_raw, specify up raw(not 0) or down raw reply(0).
 * @param raw_data, raw data that sent to cloud.
 * @param raw_data_length, raw data length that sent to cloud.
 *
 * @return 0 when success, -1 when fail.
 */
int being_deprecated linkkit_invoke_raw_service(const void *thing_id, int is_up_raw, void *raw_data,
        int raw_data_length);

/**
 * @brief trigger extended info update procedure.
 *
 * @param thing_id, pointer to thing object.
 * @param params, json type string that user to send to cloud.
 * @param linkkit_extended_info_operation, specify update type or delete type.
 *
 * @return 0 when success, -1 when fail.
 */

int being_deprecated linkkit_trigger_extended_info_operate(const void *thing_id, const char *params,
        linkkit_extended_info_operate_t linkkit_extended_info_operation);

/**
 * @brief trigger a event to post to cloud.
 *
 * @param thing_id, pointer to thing object.
 * @param event_identifier, event identifier to trigger.
 * @param cb, callback function of event post.
 *
 * @return >=0 when success, -1 when fail.
 */
int being_deprecated linkkit_trigger_event(const void *thing_id, const char *event_identifier,
        handle_post_cb_fp_t cb);

/**
 * @brief post property to cloud.
 *
 * @param thing_id, pointer to thing object.
 * @param property_identifier, used when trigger event with method "event.property.post", if set, post specified property, if NULL, post all.
 * @param cb, callback function of property post.
 *
 * @return >=0 when success, -1 when fail.
 */
int being_deprecated linkkit_post_property(const void *thing_id, const char *property_identifier,
        handle_post_cb_fp_t cb);

/**
 * @brief this function used to yield when want to receive or send data.
 *        if multi-thread enabled, user should NOT call this function.
 *
 * @param timeout_ms, timeout value in ms.
 *
 * @return 0 when success, -1 when fail.
 */
int being_deprecated linkkit_yield(int timeout_ms);

typedef enum {
    service_cota_callback_type_new_version_detected = 10,

    service_cota_callback_type_number,
} service_cota_callback_type_t;

typedef void (*handle_service_cota_callback_fp_t)(service_cota_callback_type_t callback_type, const char *configid,
        uint32_t  configsize,
        const char *gettype,
        const char *sign,
        const char *signmethod,
        const char *cota_url);

/**
 * @brief this function used to register callback for config ota.
 *
 * @param callback_fp, user callback which register to cota.
 *
 * @return 0 when success, -1 when fail.
 */
int being_deprecated linkkit_cota_init(handle_service_cota_callback_fp_t callback_fp);

/**
 * @brief this function used to execute cota process.
 *
 * @param data_buf, data buf that used to do ota. ota service will use this buffer to download bin.
 * @param data_buf_length, data buf length that used to do ota.
 *
 * @return 0 when success, -1 when fail.
 */
int being_deprecated linkkit_invoke_cota_service(void *data_buf, int data_buf_length);

/**
 * @brief this function used to trigger cota process.
 *
 * @param config_scope, remote config scope, should be "product".
 * @param get_type, remote config file type, should be "file".
 * @param attribute_Keys, reserved.
 * @param option, reserved.
 * @return 0 when success, -1 when fail.
 */
int being_deprecated linkkit_invoke_cota_get_config(const char *config_scope, const char *get_type,
        const char *attribute_Keys, void *option);

typedef enum {
    service_fota_callback_type_new_version_detected = 10,

    service_fota_callback_type_number,
} service_fota_callback_type_t;

typedef void (*handle_service_fota_callback_fp_t)(service_fota_callback_type_t callback_type, const char *version);

/**
 * @brief this function used to register callback for firmware ota.
 *
 * @param callback_fp, user callback which register to fota.
 *
 * @return 0 when success, -1 when fail.
 */
int being_deprecated linkkit_fota_init(handle_service_fota_callback_fp_t callback_fp);

/**
 * @brief this function used to execute fota process.
 *
 * @param data_buf, data buf that used to do ota. ota service will use this buffer to download bin.
 * @param data_buf_length, data buf length that used to do ota.
 *
 * @return 0 when success, -1 when fail.
 */
int being_deprecated linkkit_invoke_fota_service(void *data_buf, int data_buf_length);

/**
 * @brief this function used to get NTP time from cloud.
 *
 * @param ntp_reply_cb, user callback which register to ntp request.
 *                      when cloud returns ntp reply, sdk would trigger the callback function
 *
 * @return 0 when success, -1 when fail.
 */
int being_deprecated linkkit_ntp_time_request(void (*ntp_reply_cb)(const char *ntp_offset_time_ms));

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LINKKIT_EXPORT_H */
