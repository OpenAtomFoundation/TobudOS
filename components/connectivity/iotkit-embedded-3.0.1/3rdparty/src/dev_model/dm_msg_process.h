/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#ifndef _DM_MSG_PROCESS_H_
#define _DM_MSG_PROCESS_H_

#define DM_URI_SERVICE_DELIMITER '/'

extern const char DM_URI_SYS_PREFIX[]          DM_READ_ONLY;
extern const char DM_URI_EXT_SESSION_PREFIX[]  DM_READ_ONLY;
extern const char DM_URI_EXT_NTP_PREFIX[]      DM_READ_ONLY;
extern const char DM_URI_EXT_ERROR_PREFIX[]    DM_READ_ONLY;
extern const char DM_URI_REPLY_SUFFIX[]        DM_READ_ONLY;
extern const char DM_URI_OTA_DEVICE_INFORM[]   DM_READ_ONLY;
extern const char DM_URI_THING_PROPERTY_DESIRED_GET[]  DM_READ_ONLY;
extern const char DM_URI_THING_PROPERTY_DESIRED_DELETE[]  DM_READ_ONLY;
extern const char DM_URI_THING_PROPERTY_DESIRED_GET_REPLY[] DM_READ_ONLY;
extern const char DM_URI_THING_PROPERTY_DESIRED_DELETE_REPLY[] DM_READ_ONLY;
/* From Cloud To Local Request And Response*/
extern const char DM_URI_THING_MODEL_DOWN_RAW[]              DM_READ_ONLY;
extern const char DM_URI_THING_MODEL_DOWN_RAW_REPLY[]        DM_READ_ONLY;

/* From Local To Cloud Request And Response*/
extern const char DM_URI_THING_MODEL_UP_RAW[]                DM_READ_ONLY;
extern const char DM_URI_THING_MODEL_UP_RAW_REPLY[]          DM_READ_ONLY;

#if !defined(DEVICE_MODEL_RAWDATA_SOLO)
    extern const char DM_URI_RRPC_REQUEST_WILDCARD[]             DM_READ_ONLY;
    /* From Cloud To Local Request And Response*/
    extern const char DM_URI_THING_SERVICE_PROPERTY_SET[]        DM_READ_ONLY;
    extern const char DM_URI_THING_SERVICE_PROPERTY_SET_REPLY[]  DM_READ_ONLY;
    extern const char DM_URI_THING_SERVICE_PROPERTY_GET[]        DM_READ_ONLY;
    extern const char DM_URI_THING_SERVICE_PROPERTY_GET_REPLY[]  DM_READ_ONLY;
    extern const char DM_URI_THING_SERVICE_REQUEST_WILDCARD[]    DM_READ_ONLY;
    extern const char DM_URI_THING_SERVICE_REQUEST_WILDCARD2[]   DM_READ_ONLY;
    extern const char DM_URI_THING_SERVICE_REQUEST[]             DM_READ_ONLY;
    extern const char DM_URI_THING_SERVICE_RESPONSE[]            DM_READ_ONLY;


    /* From Local To Cloud Request And Response*/
    extern const char DM_URI_THING_EVENT_PROPERTY_POST[]         DM_READ_ONLY;
    extern const char DM_URI_THING_EVENT_PROPERTY_POST_REPLY[]   DM_READ_ONLY;
    #ifdef LOG_REPORT_TO_CLOUD
        extern const char DM_URI_THING_LOG_POST[]                    DM_READ_ONLY;
    #endif
    extern const char DM_URI_THING_EVENT_POST[]                  DM_READ_ONLY;
    extern const char DM_URI_THING_EVENT_POST_REPLY[]            DM_READ_ONLY;
    extern const char DM_URI_THING_EVENT_POST_REPLY_WILDCARD[]   DM_READ_ONLY;
    extern const char DM_URI_THING_DEVICEINFO_UPDATE[]           DM_READ_ONLY;
    extern const char DM_URI_THING_DEVICEINFO_UPDATE_REPLY[]     DM_READ_ONLY;
    extern const char DM_URI_THING_DEVICEINFO_DELETE[]           DM_READ_ONLY;
    extern const char DM_URI_THING_DEVICEINFO_DELETE_REPLY[]     DM_READ_ONLY;
    extern const char DM_URI_THING_DSLTEMPLATE_GET[]             DM_READ_ONLY;
    extern const char DM_URI_THING_DSLTEMPLATE_GET_REPLY[]       DM_READ_ONLY;
    extern const char DM_URI_THING_DYNAMICTSL_GET[]              DM_READ_ONLY;
    extern const char DM_URI_THING_DYNAMICTSL_GET_REPLY[]        DM_READ_ONLY;
    extern const char DM_URI_NTP_REQUEST[]                       DM_READ_ONLY;
    extern const char DM_URI_NTP_RESPONSE[]                      DM_READ_ONLY;
#endif

extern const char DM_URI_DEV_CORE_SERVICE_DEV[]              DM_READ_ONLY;

#ifdef DEVICE_MODEL_GATEWAY
    /* From Cloud To Local Request And Response*/
    extern const char DM_URI_THING_TOPO_ADD_NOTIFY[]             DM_READ_ONLY;
    extern const char DM_URI_THING_TOPO_ADD_NOTIFY_REPLY[]       DM_READ_ONLY;
    extern const char DM_URI_THING_DISABLE[]                     DM_READ_ONLY;
    extern const char DM_URI_THING_DISABLE_REPLY[]               DM_READ_ONLY;
    extern const char DM_URI_THING_ENABLE[]                      DM_READ_ONLY;
    extern const char DM_URI_THING_ENABLE_REPLY[]                DM_READ_ONLY;
    extern const char DM_URI_THING_DELETE[]                      DM_READ_ONLY;
    extern const char DM_URI_THING_DELETE_REPLY[]                DM_READ_ONLY;
    extern const char DM_URI_THING_GATEWAY_PERMIT[]              DM_READ_ONLY;
    extern const char DM_URI_THING_GATEWAY_PERMIT_REPLY[]        DM_READ_ONLY;

    /* From Local To Cloud Request And Response*/
    extern const char DM_URI_THING_SUB_REGISTER[]                DM_READ_ONLY;
    extern const char DM_URI_THING_SUB_REGISTER_REPLY[]          DM_READ_ONLY;
    extern const char DM_URI_THING_SUB_UNREGISTER[]              DM_READ_ONLY;
    extern const char DM_URI_THING_SUB_UNREGISTER_REPLY[]        DM_READ_ONLY;
    extern const char DM_URI_THING_TOPO_ADD[]                    DM_READ_ONLY;
    extern const char DM_URI_THING_TOPO_ADD_REPLY[]              DM_READ_ONLY;
    extern const char DM_URI_THING_TOPO_DELETE[]                 DM_READ_ONLY;
    extern const char DM_URI_THING_TOPO_DELETE_REPLY[]           DM_READ_ONLY;
    extern const char DM_URI_THING_TOPO_GET[]                    DM_READ_ONLY;
    extern const char DM_URI_THING_TOPO_GET_REPLY[]              DM_READ_ONLY;
    extern const char DM_URI_THING_LIST_FOUND[]                  DM_READ_ONLY;
    extern const char DM_URI_THING_LIST_FOUND_REPLY[]            DM_READ_ONLY;
    extern const char DM_URI_COMBINE_LOGIN[]                     DM_READ_ONLY;
    extern const char DM_URI_COMBINE_LOGIN_REPLY[]               DM_READ_ONLY;
    extern const char DM_URI_COMBINE_LOGOUT[]                    DM_READ_ONLY;
    extern const char DM_URI_COMBINE_LOGOUT_REPLY[]              DM_READ_ONLY;
#endif

int dm_disp_uri_prefix_split(_IN_ const char *prefix, _IN_ char *uri, _IN_ int uri_len, _OU_ int *start, _OU_ int *end);
int dm_disp_uri_pkdn_split(_IN_ char *uri, _IN_ int uri_len, _OU_ int *start, _OU_ int *end);
int dm_disp_uri_service_specific_split(_IN_ char *uri, _IN_ int uri_len, _OU_ int *start, _OU_ int *end);
int dm_disp_uri_rrpc_request_split(_IN_ char *uri, _IN_ int uri_len, _OU_ int *start, _OU_ int *end);
int dm_disp_uri_event_specific_split(_IN_ char *uri, _IN_ int uri_len, _OU_ int *start, _OU_ int *end);

int dm_msg_proc_thing_model_down_raw(_IN_ dm_msg_source_t *source);
int dm_msg_proc_thing_model_up_raw_reply(_IN_ dm_msg_source_t *source);

#if !defined(DEVICE_MODEL_RAWDATA_SOLO)
int dm_msg_proc_thing_service_property_set(_IN_ dm_msg_source_t *source, _IN_ dm_msg_dest_t *dest,
        _OU_ dm_msg_request_payload_t *request, _OU_ dm_msg_response_t *response);
int dm_msg_proc_thing_service_property_get(_IN_ dm_msg_source_t *source, _IN_ dm_msg_dest_t *dest,
        _OU_ dm_msg_request_payload_t *request, _OU_ dm_msg_response_t *response,
        _OU_ unsigned char **data, int *data_len);
int dm_msg_proc_thing_property_desired_get_reply(_IN_ dm_msg_source_t *source);
int dm_msg_proc_thing_property_desired_delete_reply(_IN_ dm_msg_source_t *source);
int dm_msg_proc_thing_service_property_post(_IN_ dm_msg_source_t *source, _IN_ dm_msg_dest_t *dest,
        _OU_ dm_msg_request_payload_t *request, _OU_ dm_msg_response_t *response);
int dm_msg_proc_thing_service_request(_IN_ dm_msg_source_t *source);
int dm_msg_proc_thing_event_post_reply(_IN_ dm_msg_source_t *source);
int dm_msg_proc_thing_deviceinfo_update_reply(_IN_ dm_msg_source_t *source);
int dm_msg_proc_thing_deviceinfo_delete_reply(_IN_ dm_msg_source_t *source);
int dm_msg_proc_thing_dynamictsl_get_reply(_IN_ dm_msg_source_t *source);
int dm_msg_proc_rrpc_request(_IN_ dm_msg_source_t *source);
int dm_disp_ntp_response(_IN_ dm_msg_source_t *source);
int dm_disp_ext_error_response(_IN_ dm_msg_source_t *source);
#endif

#ifdef DEVICE_MODEL_GATEWAY
int dm_msg_proc_thing_topo_add_notify(_IN_ dm_msg_source_t *source, _IN_ dm_msg_dest_t *dest,
                                      _OU_ dm_msg_request_payload_t *request, _OU_ dm_msg_response_t *response);
int dm_msg_proc_thing_disable(_IN_ dm_msg_source_t *source, _IN_ dm_msg_dest_t *dest,
                              _OU_ dm_msg_request_payload_t *request, _OU_ dm_msg_response_t *response);
int dm_msg_proc_thing_enable(_IN_ dm_msg_source_t *source, _IN_ dm_msg_dest_t *dest,
                             _OU_ dm_msg_request_payload_t *request, _OU_ dm_msg_response_t *response);
int dm_msg_proc_thing_delete(_IN_ dm_msg_source_t *source, _IN_ dm_msg_dest_t *dest,
                             _OU_ dm_msg_request_payload_t *request, _OU_ dm_msg_response_t *response);
int dm_msg_proc_thing_gateway_permit(_IN_ dm_msg_source_t *source, _IN_ dm_msg_dest_t *dest,
                                     _OU_ dm_msg_request_payload_t *request, _OU_ dm_msg_response_t *response);
int dm_msg_proc_thing_sub_register_reply(_IN_ dm_msg_source_t *source);
int dm_msg_proc_thing_sub_unregister_reply(_IN_ dm_msg_source_t *source);
int dm_msg_proc_thing_topo_add_reply(_IN_ dm_msg_source_t *source);
int dm_msg_proc_thing_topo_delete_reply(_IN_ dm_msg_source_t *source);
int dm_msg_proc_thing_topo_get_reply(_IN_ dm_msg_source_t *source);
int dm_msg_proc_thing_list_found_reply(_IN_ dm_msg_source_t *source);
int dm_msg_proc_combine_login_reply(_IN_ dm_msg_source_t *source);
int dm_msg_proc_combine_logout_reply(_IN_ dm_msg_source_t *source);
#endif

#ifdef ALCS_ENABLED
int dm_msg_proc_thing_dev_core_service_dev(_IN_ dm_msg_source_t *source, _IN_ dm_msg_dest_t *dest,
        _OU_ dm_msg_request_payload_t *request, _OU_ dm_msg_response_t *response,
        _OU_ unsigned char **data, int *data_len);
#endif

#endif
