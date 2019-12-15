/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "iotx_dm_internal.h"

#ifdef DEPRECATED_LINKKIT

/* #define IOTX_DM_TSL_DEVELOP_TEST */

/*****************************Internal Definition*****************************/

typedef int (*dm_shw_data_set)(_IN_ dm_shw_data_value_t *data_value, _IN_ void *value, _IN_ int value_len);
typedef int (*dm_shw_array_set)(_IN_ dm_shw_data_value_t *data_value, _IN_ void *value, _IN_ int value_len,
                                _IN_ int index);
typedef int (*dm_shw_data_get)(_IN_ dm_shw_data_value_t *data_value, _IN_ void *value);
typedef int (*dm_shw_array_get)(_IN_ dm_shw_data_value_t *data_value, _IN_ void *value, _IN_ int index);
typedef void (*dm_shw_data_free)(_IN_ dm_shw_data_value_t *data_value);
typedef void (*dm_shw_array_free)(_IN_ dm_shw_data_value_t *data_value);
typedef void (*dm_shw_data_print)(_IN_ dm_shw_data_value_t *data_value);

typedef struct {
    dm_shw_data_type_e type;
    const char *name;
    dm_shw_data_set    func_set;
    dm_shw_array_set   func_array_set;
    dm_shw_data_get    func_get;
    dm_shw_array_get   func_array_get;
    dm_shw_data_free   func_free;
    dm_shw_array_free  func_array_free;
} dm_shw_data_type_mapping_t;

/* Data Set */
static int _dm_shw_int_set(_IN_ dm_shw_data_value_t *data_value, _IN_ void *value, _IN_ int value_len);
static int _dm_shw_float_set(_IN_ dm_shw_data_value_t *data_value, _IN_ void *value, _IN_ int value_len);
static int _dm_shw_double_set(_IN_ dm_shw_data_value_t *data_value, _IN_ void *value, _IN_ int value_len);
static int _dm_shw_text_set(_IN_ dm_shw_data_value_t *data_value, _IN_ void *value, _IN_ int value_len);
static int _dm_shw_enum_set(_IN_ dm_shw_data_value_t *data_value, _IN_ void *value, _IN_ int value_len);
static int _dm_shw_date_set(_IN_ dm_shw_data_value_t *data_value, _IN_ void *value, _IN_ int value_len);
static int _dm_shw_bool_set(_IN_ dm_shw_data_value_t *data_value, _IN_ void *value, _IN_ int value_len);

/* Array Data Set */
static int _dm_shw_array_int_set(_IN_ dm_shw_data_value_t *data_value, _IN_ void *value, _IN_ int value_len,
                                 _IN_ int index);
static int _dm_shw_array_float_set(_IN_ dm_shw_data_value_t *data_value, _IN_ void *value, _IN_ int value_len,
                                   _IN_ int index);
static int _dm_shw_array_double_set(_IN_ dm_shw_data_value_t *data_value, _IN_ void *value, _IN_ int value_len,
                                    _IN_ int index);
static int _dm_shw_array_text_set(_IN_ dm_shw_data_value_t *data_value, _IN_ void *value, _IN_ int value_len,
                                  _IN_ int index);
static int _dm_shw_array_enum_set(_IN_ dm_shw_data_value_t *data_value, _IN_ void *value, _IN_ int value_len,
                                  _IN_ int index);
static int _dm_shw_array_date_set(_IN_ dm_shw_data_value_t *data_value, _IN_ void *value, _IN_ int value_len,
                                  _IN_ int index);
static int _dm_shw_array_bool_set(_IN_ dm_shw_data_value_t *data_value, _IN_ void *value, _IN_ int value_len,
                                  _IN_ int index);

/* Data Get */
static int _dm_shw_int_get(_IN_ dm_shw_data_value_t *data_value, _IN_ void *value);
static int _dm_shw_float_get(_IN_ dm_shw_data_value_t *data_value, _IN_ void *value);
static int _dm_shw_double_get(_IN_ dm_shw_data_value_t *data_value, _IN_ void *value);
static int _dm_shw_text_get(_IN_ dm_shw_data_value_t *data_value, _IN_ void *value);
static int _dm_shw_enum_get(_IN_ dm_shw_data_value_t *data_value, _IN_ void *value);
static int _dm_shw_date_get(_IN_ dm_shw_data_value_t *data_value, _IN_ void *value);
static int _dm_shw_bool_get(_IN_ dm_shw_data_value_t *data_value, _IN_ void *value);

/* Array Data Get */
static int _dm_shw_array_int_get(_IN_ dm_shw_data_value_t *data_value, _IN_ void *value, _IN_ int index);
static int _dm_shw_array_float_get(_IN_ dm_shw_data_value_t *data_value, _IN_ void *value, _IN_ int index);
static int _dm_shw_array_double_get(_IN_ dm_shw_data_value_t *data_value, _IN_ void *value, _IN_ int index);
static int _dm_shw_array_text_get(_IN_ dm_shw_data_value_t *data_value, _IN_ void *value, _IN_ int index);
static int _dm_shw_array_enum_get(_IN_ dm_shw_data_value_t *data_value, _IN_ void *value, _IN_ int index);
static int _dm_shw_array_date_get(_IN_ dm_shw_data_value_t *data_value, _IN_ void *value, _IN_ int index);
static int _dm_shw_array_bool_get(_IN_ dm_shw_data_value_t *data_value, _IN_ void *value, _IN_ int index);

/* Data Free */
static void _dm_shw_int_free(_IN_ dm_shw_data_value_t *data_value);
static void _dm_shw_float_free(_IN_ dm_shw_data_value_t *data_value);
static void _dm_shw_double_free(_IN_ dm_shw_data_value_t *data_value);
static void _dm_shw_text_free(_IN_ dm_shw_data_value_t *data_value);
static void _dm_shw_enum_free(_IN_ dm_shw_data_value_t *data_value);
static void _dm_shw_date_free(_IN_ dm_shw_data_value_t *data_value);
static void _dm_shw_bool_free(_IN_ dm_shw_data_value_t *data_value);
static void _dm_shw_array_free(_IN_ dm_shw_data_value_t *data_value);
static void _dm_shw_struct_free(_IN_ dm_shw_data_value_t *data_value);
static void _dm_shw_property_free(_IN_ dm_shw_data_t *property);

/* Array Data Free */
static void _dm_shw_array_int_free(_IN_ dm_shw_data_value_t *data_value);
static void _dm_shw_array_float_free(_IN_ dm_shw_data_value_t *data_value);
static void _dm_shw_array_double_free(_IN_ dm_shw_data_value_t *data_value);
static void _dm_shw_array_text_free(_IN_ dm_shw_data_value_t *data_value);
static void _dm_shw_array_enum_free(_IN_ dm_shw_data_value_t *data_value);
static void _dm_shw_array_date_free(_IN_ dm_shw_data_value_t *data_value);
static void _dm_shw_array_bool_free(_IN_ dm_shw_data_value_t *data_value);
static void _dm_shw_array_array_free(_IN_ dm_shw_data_value_t *data_value);
static void _dm_shw_array_struct_free(_IN_ dm_shw_data_value_t *data_value);

#if 0
    /* Data Print */
    static void _dm_shw_int_print(_IN_ dm_shw_data_value_t *data_value);
    static void _dm_shw_float_print(_IN_ dm_shw_data_value_t *data_value);
    static void _dm_shw_double_print(_IN_ dm_shw_data_value_t *data_value);
    static void _dm_shw_text_print(_IN_ dm_shw_data_value_t *data_value);
    static void _dm_shw_enum_print(_IN_ dm_shw_data_value_t *data_value);
    static void _dm_shw_date_print(_IN_ dm_shw_data_value_t *data_value);
    static void _dm_shw_bool_print(_IN_ dm_shw_data_value_t *data_value);
    static void _dm_shw_array_print(_IN_ dm_shw_data_value_t *data_value);
    static void _dm_shw_struct_print(_IN_ dm_shw_data_value_t *data_value);
    static void _dm_shw_property_print(_IN_ dm_shw_data_t *property);
#endif

/* Data Search */
static int _dm_shw_data_struct_search(_IN_ dm_shw_data_t *input, _IN_ char *key, _IN_ int key_len,
                                      _OU_ dm_shw_data_t **output, _OU_ int *index);

static dm_shw_data_type_mapping_t g_iotx_data_type_mapping[] = {
    {DM_SHW_DATA_TYPE_NONE,     "none",     NULL,                   NULL,                        NULL,                    NULL,                         NULL,                    NULL             },
    {DM_SHW_DATA_TYPE_INT,      "int",      _dm_shw_int_set,      _dm_shw_array_int_set,      _dm_shw_int_get,      _dm_shw_array_int_get,      _dm_shw_int_free,      _dm_shw_array_int_free     },
    {DM_SHW_DATA_TYPE_FLOAT,    "float",    _dm_shw_float_set,    _dm_shw_array_float_set,    _dm_shw_float_get,    _dm_shw_array_float_get,    _dm_shw_float_free,    _dm_shw_array_float_free,  },
    {DM_SHW_DATA_TYPE_DOUBLE,   "double",   _dm_shw_double_set,   _dm_shw_array_double_set,   _dm_shw_double_get,   _dm_shw_array_double_get,   _dm_shw_double_free,   _dm_shw_array_double_free, },
    {DM_SHW_DATA_TYPE_TEXT,     "text",     _dm_shw_text_set,     _dm_shw_array_text_set,     _dm_shw_text_get,     _dm_shw_array_text_get,     _dm_shw_text_free,     _dm_shw_array_text_free,   },
    {DM_SHW_DATA_TYPE_ENUM,     "enum",     _dm_shw_enum_set,     _dm_shw_array_enum_set,     _dm_shw_enum_get,     _dm_shw_array_enum_get,     _dm_shw_enum_free,     _dm_shw_array_enum_free,   },
    {DM_SHW_DATA_TYPE_DATE,     "date",     _dm_shw_date_set,     _dm_shw_array_date_set,     _dm_shw_date_get,     _dm_shw_array_date_get,     _dm_shw_date_free,     _dm_shw_array_date_free,   },
    {DM_SHW_DATA_TYPE_BOOL,     "bool",     _dm_shw_bool_set,     _dm_shw_array_bool_set,     _dm_shw_bool_get,     _dm_shw_array_bool_get,     _dm_shw_bool_free,     _dm_shw_array_bool_free,   },
    {DM_SHW_DATA_TYPE_ARRAY,    "array",    NULL,                 NULL,                       NULL,                 NULL,                       _dm_shw_array_free,    _dm_shw_array_array_free,  },
    {DM_SHW_DATA_TYPE_STRUCT,   "struct",   NULL,                 NULL,                       NULL,                 NULL,                       _dm_shw_struct_free,   _dm_shw_array_struct_free, }
};

/*****************************************************************************/

static int _dm_shw_data_array_search(_IN_ dm_shw_data_t *input, _IN_ int input_index, _IN_ char *key,
                                     _IN_ int key_len, _OU_ dm_shw_data_t **output, _OU_ int *output_index)
{
    int res = 0, deli_offset = 0;

    dm_shw_data_value_complex_t *complex_struct = (dm_shw_data_value_complex_t *)input->data_value.value;
    /* dm_log_debug("Current Key: %s, Len: %d",key,key_len);
    dm_log_debug("Current Item Identifier: %s",input->identifier); */
    res = dm_utils_memtok(key, key_len, DM_SHW_KEY_DELIMITER, 1, &deli_offset);
    if (res != SUCCESS_RETURN) {
        deli_offset = key_len;
    }

    switch (complex_struct->type) {
        case DM_SHW_DATA_TYPE_INT:
        case DM_SHW_DATA_TYPE_FLOAT:
        case DM_SHW_DATA_TYPE_DOUBLE:
        case DM_SHW_DATA_TYPE_TEXT:
        case DM_SHW_DATA_TYPE_ENUM:
        case DM_SHW_DATA_TYPE_DATE:
        case DM_SHW_DATA_TYPE_BOOL: {
            if (deli_offset != key_len) {
                return FAIL_RETURN;
            }
            if (output) {
                *output = input;
            }
            if (output_index) {
                *output_index = input_index;
            }
            return SUCCESS_RETURN;
        }
        break;
        case DM_SHW_DATA_TYPE_STRUCT: {
            dm_shw_data_t *search_data = NULL;
            if (complex_struct->value == NULL) {
                return FAIL_RETURN;
            }
            search_data = (dm_shw_data_t *)complex_struct->value + input_index;
            return _dm_shw_data_struct_search(search_data, key, deli_offset, output, output_index);
        }
        break;
        default:
            dm_log_err("Unknown Data Type: %d", complex_struct->type);
            break;
    }

    return FAIL_RETURN;
}

static int _dm_shw_data_struct_search(_IN_ dm_shw_data_t *input, _IN_ char *key,
                                      _IN_ int key_len, _OU_ dm_shw_data_t **output, _OU_ int *index)
{
    int res = 0, item_index = 0, deli_offset = 0, partial_offset = 0;
    int partial_input_len = 0, array_input_len = 0, array_index = 0;
    dm_shw_data_t *data_item = NULL;

    /* dm_log_debug("Current Key: %.*s",key_len,key); */

    dm_shw_data_value_complex_t *complex_struct = (dm_shw_data_value_complex_t *)input->data_value.value;

    res = dm_utils_memtok(key, key_len, DM_SHW_KEY_DELIMITER, 1, &deli_offset);
    if (res != SUCCESS_RETURN) {
        deli_offset = key_len;
    }

    partial_offset = deli_offset;
    res = dm_utils_strarr_index(key, deli_offset, &partial_input_len, &array_input_len, &array_index);
    if (res == SUCCESS_RETURN) {
        /* dm_log_debug("Current Index: %d",array_index); */
        partial_offset = partial_input_len;
    }

    for (item_index = 0; item_index < complex_struct->size; item_index++) {
        data_item = (dm_shw_data_t *)complex_struct->value + item_index;
        if (strlen(data_item->identifier) != partial_offset ||
            memcmp(data_item->identifier, key, partial_offset) != 0) {
            continue;
        }

        switch (data_item->data_value.type) {
            case DM_SHW_DATA_TYPE_INT:
            case DM_SHW_DATA_TYPE_FLOAT:
            case DM_SHW_DATA_TYPE_DOUBLE:
            case DM_SHW_DATA_TYPE_TEXT:
            case DM_SHW_DATA_TYPE_ENUM:
            case DM_SHW_DATA_TYPE_DATE:
            case DM_SHW_DATA_TYPE_BOOL: {
                if (partial_input_len != 0 || deli_offset != key_len) {
                    return FAIL_RETURN;
                }
                if (output) {
                    *output = data_item;
                }
                return SUCCESS_RETURN;
            }
            break;
            case DM_SHW_DATA_TYPE_ARRAY: {
                int key_offset = (deli_offset == key_len) ? (deli_offset - 1) : (deli_offset + 1);
                int key_len_offset = (deli_offset == key_len) ? (key_len) : (deli_offset + 1);
                if ((partial_input_len == 0) && (deli_offset == key_len)) {
                    if (output) {
                        *output = data_item;
                    }
                    return SUCCESS_RETURN;
                }
                if (partial_input_len == 0) {
                    return FAIL_RETURN;
                }
                return _dm_shw_data_array_search(data_item, array_index, key + key_offset, key_len - key_len_offset, output, index);
            }
            case DM_SHW_DATA_TYPE_STRUCT: {
                if (deli_offset == key_len) {
                    if (output) {
                        *output = data_item;
                    }
                    return SUCCESS_RETURN;
                }
                if (partial_input_len != 0) {
                    return FAIL_RETURN;
                }
                return _dm_shw_data_struct_search(data_item, key + deli_offset + 1, key_len - deli_offset - 1, output, index);
            }
            default:
                dm_log_err("Unknown Data Type");
                break;
        }
    }

    return FAIL_RETURN;
}

static int _dm_shw_data_search(_IN_ dm_shw_data_t *input, _IN_ char *key,
                               _IN_ int key_len, _OU_ dm_shw_data_t **output, _OU_ int *index)
{
    int res = 0, deli_offset = 0, partial_offset = 0;
    int partial_input_len = 0, array_input_len = 0, array_index = 0;

    if (input == NULL || key == NULL || key_len <= 0) {
        return DM_INVALID_PARAMETER;
    }

    res = dm_utils_memtok(key, key_len, DM_SHW_KEY_DELIMITER, 1, &deli_offset);
    if (res != SUCCESS_RETURN) {
        deli_offset = key_len;
    }

    partial_offset = deli_offset;
    res = dm_utils_strarr_index(key, deli_offset, &partial_input_len, &array_input_len, &array_index);
    if (res == SUCCESS_RETURN) {
        /* dm_log_debug("Current Index: %d",array_index); */
        partial_offset = partial_input_len;
    }

    /* dm_log_debug("Current Input Identifier: %s",input->identifier);
    dm_log_debug("Current Compare Key: %.*s",partial_offset,key); */

    if (strlen(input->identifier) != partial_offset ||
        memcmp(input->identifier, key, partial_offset) != 0) {
        return FAIL_RETURN;
    }
    dm_log_debug("Identifier Found: %s", input->identifier);

    switch (input->data_value.type) {
        case DM_SHW_DATA_TYPE_INT:
        case DM_SHW_DATA_TYPE_FLOAT:
        case DM_SHW_DATA_TYPE_DOUBLE:
        case DM_SHW_DATA_TYPE_TEXT:
        case DM_SHW_DATA_TYPE_ENUM:
        case DM_SHW_DATA_TYPE_DATE:
        case DM_SHW_DATA_TYPE_BOOL: {
            if (partial_input_len != 0 || deli_offset != key_len) {
                return FAIL_RETURN;
            }
            if (output) {
                *output = input;
            }
            return SUCCESS_RETURN;
        }
        break;
        case DM_SHW_DATA_TYPE_ARRAY: {
            int key_offset = (deli_offset == key_len) ? (deli_offset - 1) : (deli_offset + 1);
            int key_len_offset = (deli_offset == key_len) ? (key_len) : (deli_offset + 1);
            if ((partial_input_len == 0) && (deli_offset == key_len)) {
                if (output) {
                    *output = input;
                }
                return SUCCESS_RETURN;
            }
            if (partial_input_len == 0) {
                return FAIL_RETURN;
            }
            return _dm_shw_data_array_search(input, array_index, key + key_offset, key_len - key_len_offset, output, index);
        }
        break;
        case DM_SHW_DATA_TYPE_STRUCT: {
            if (deli_offset == key_len) {
                if (output) {
                    *output = input;
                }
                return SUCCESS_RETURN;
            }
            if (partial_input_len != 0) {
                return FAIL_RETURN;
            }
            return _dm_shw_data_struct_search(input, key + deli_offset + 1, key_len - deli_offset - 1, output, index);
        }
        break;
        default:
            dm_log_err("Unknow Data Type");
            break;
    }

    return FAIL_RETURN;
}

static int _dm_shw_property_search(_IN_ dm_shw_t *shadow, _IN_ char *key, _IN_ int key_len,
                                   _OU_ dm_shw_data_t **property, _OU_ int *index)
{
    int res = 0, item_index = 0;
    dm_shw_data_t *property_item = NULL;

    if (shadow == NULL || key == NULL || key_len <= 0) {
        return DM_INVALID_PARAMETER;
    }

    if (shadow->property_number == 0 || shadow->properties == NULL) {
        return DM_TSL_PROPERTY_NOT_EXIST;
    }

    for (item_index = 0; item_index < shadow->property_number; item_index++) {
        property_item = shadow->properties + item_index;
        res = _dm_shw_data_search(property_item, key, key_len, property, index);
        if (res == SUCCESS_RETURN) {
            return SUCCESS_RETURN;
        }
    }

    return FAIL_RETURN;
}

static int _dm_shw_event_output_search(_IN_ dm_shw_data_t *outputdatas, _IN_ int number, _IN_ char *key,
                                       _IN_ int key_len, _OU_ dm_shw_data_t **event_data, _OU_ int *index)
{
    int res = 0, item_index = 0;
    dm_shw_data_t *outputdata = NULL;

    if (outputdatas == NULL || number <= 0 || key == NULL || key_len <= 0) {
        return DM_INVALID_PARAMETER;
    }

    for (item_index = 0; item_index < number; item_index++) {
        outputdata = outputdatas + item_index;
        res = _dm_shw_data_search(outputdata, key, key_len, event_data, index);
        if (res == SUCCESS_RETURN) {
            return SUCCESS_RETURN;
        }
    }

    return SUCCESS_RETURN;
}

static int _dm_shw_event_search(_IN_ dm_shw_t *shadow, _IN_ char *key, _IN_ int key_len, _OU_ dm_shw_event_t **event)
{
    int index = 0;
    dm_shw_event_t *dtsl_event = NULL;

    if (shadow == NULL || key == NULL || key_len <= 0) {
        return DM_INVALID_PARAMETER;
    }

    for (index = 0; index < shadow->event_number; index++) {
        dtsl_event = shadow->events + index;
        if ((strlen(dtsl_event->identifier) == key_len) &&
            (memcmp(dtsl_event->identifier, key, key_len) == 0)) {
            /* dm_log_debug("TSL Event Found: %s",dtsl_event->identifier); */
            if (event) {
                *event = dtsl_event;
            }
            return SUCCESS_RETURN;
        }
    }

    /* dm_log_debug("TSL Event Not Found: %.*s",key_len,key); */

    return FAIL_RETURN;
}

static int _dm_shw_service_input_output_search(_IN_ dm_shw_data_target_e type, _IN_ dm_shw_service_t *service,
        _IN_ char *key, _IN_ int key_len, _OU_ dm_shw_data_t **service_data, _OU_ int *index)
{
    int res = 0, item_index = 0, datas_number = 0;
    dm_shw_data_t *datas = NULL;
    dm_shw_data_t *data = NULL;

    if (type == DM_SHW_DATA_TARGET_SERVICE_INPUT_DATA) {
        datas = service->input_datas;
        datas_number = service->input_data_number;
    } else {
        datas = service->output_datas;
        datas_number = service->output_data_number;
    }

    for (item_index = 0; item_index < datas_number; item_index++) {
        data = datas + item_index;
        res = _dm_shw_data_search(data, key, key_len, service_data, index);
        if (res == SUCCESS_RETURN) {
            return SUCCESS_RETURN;
        }
    }

    return SUCCESS_RETURN;
}

static int _dm_shw_service_search(_IN_ dm_shw_t *shadow, _IN_ char *key, _IN_ int key_len,
                                  _OU_ dm_shw_service_t **service)
{
    int index = 0;
    dm_shw_service_t *dtsl_service = NULL;

    if (shadow == NULL || key == NULL || key_len <= 0) {
        return DM_INVALID_PARAMETER;
    }

    for (index = 0; index < shadow->service_number; index++) {
        dtsl_service = shadow->services + index;
        if ((strlen(dtsl_service->identifier) == key_len) &&
            (memcmp(dtsl_service->identifier, key, key_len) == 0)) {
            /* dm_log_debug("TSL Service Found: %s",dtsl_service->identifier); */
            if (service) {
                *service = dtsl_service;
            }
            return SUCCESS_RETURN;
        }
    }

    /* dm_log_debug("TSL Service Not Found: %.*s",key_len,key); */

    return FAIL_RETURN;
}

int dm_shw_create(_IN_ iotx_dm_tsl_type_t type, _IN_ const char *tsl, _IN_ int tsl_len, _OU_ dm_shw_t **shadow)
{
    int res = 0;

    if (shadow == NULL || *shadow != NULL || tsl == NULL || tsl_len <= 0) {
        return DM_INVALID_PARAMETER;
    }

    switch (type) {
        case IOTX_DM_TSL_TYPE_ALINK: {
            res = dm_tsl_alink_create(tsl, tsl_len, shadow);
        }
        break;
        case IOTX_DM_TSL_TYPE_TLV: {
            /* TODO for yusan*/
            res = FAIL_RETURN;
        }
        break;
        default:
            dm_log_err("Unknown TSL Type");
            res = FAIL_RETURN;
            break;
    }

    return res;
}

int dm_shw_get_property_data(_IN_ dm_shw_t *shadow, _IN_ char *key, _IN_ int key_len, _OU_ void **data)
{
    int res = 0;

    if (shadow == NULL || key == NULL || key_len <= 0 || data == NULL || *data != NULL) {
        return DM_INVALID_PARAMETER;
    }

    res = _dm_shw_property_search(shadow, key, key_len, (dm_shw_data_t **)data, NULL);
    if (res != SUCCESS_RETURN) {
        return DM_TSL_PROPERTY_NOT_EXIST;
    }

    return SUCCESS_RETURN;
}

int dm_shw_get_service_input_output_data(_IN_ dm_shw_data_target_e type, _IN_ dm_shw_t *shadow, _IN_ char *key,
        _IN_ int key_len, _OU_ void **data)
{
    int res = 0;
    int offset = 0, array_index = 0;
    char *pos = NULL;
    dm_shw_service_t *service = NULL;
    dm_shw_data_t *service_data = NULL;

    if (type < DM_SHW_DATA_TARGET_SERVICE_INPUT_DATA || type > DM_SHW_DATA_TARGET_SERVICE_OUTPUT_DATA || shadow == NULL
        || key == NULL || key_len <= 0) {
        return DM_INVALID_PARAMETER;
    }

    res = dm_utils_memtok(key, key_len, DM_SHW_KEY_DELIMITER, 1, &offset);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    dm_log_debug("Key: %.*s", key_len, key);

    res = _dm_shw_service_search(shadow, key, offset, &service);
    if (res != SUCCESS_RETURN) {
        return DM_TSL_EVENT_NOT_EXIST;
    }

    pos = key + offset + 1;
    dm_log_debug("TSL Service input/output Data Search, Event Data ID: %s", pos);

    res = _dm_shw_service_input_output_search(type, service, pos, strlen(pos), &service_data, &array_index);
    if (res != SUCCESS_RETURN) {
        return DM_TSL_EVENT_NOT_EXIST;
    }

    if (data) {
        *data = (void *)service_data;
    }

    return SUCCESS_RETURN;
}

int dm_shw_get_event_output_data(_IN_ dm_shw_t *shadow, _IN_ char *key, _IN_ int key_len, _OU_ void **data)
{
    int res = 0;
    int offset = 0, array_index = 0;
    char *pos = NULL;
    dm_shw_event_t *event = NULL;
    dm_shw_data_t *event_data = NULL;

    if (shadow == NULL || key == NULL || key_len <= 0) {
        return DM_INVALID_PARAMETER;
    }

    res = dm_utils_memtok(key, key_len, DM_SHW_KEY_DELIMITER, 1, &offset);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    dm_log_debug("Key: %.*s", key_len, key);

    res = _dm_shw_event_search(shadow, key, offset, &event);
    if (res != SUCCESS_RETURN) {
        return DM_TSL_EVENT_NOT_EXIST;
    }

    pos = key + offset + 1;
    /* dm_log_debug("TSL Event Output Data Search, Event Data ID: %s",pos); */

    res = _dm_shw_event_output_search(event->output_datas, event->output_data_number, pos, strlen(pos), &event_data,
                                      &array_index);
    if (res != SUCCESS_RETURN) {
        return DM_TSL_EVENT_NOT_EXIST;
    }

    if (data) {
        *data = (void *)event_data;
    }

    return SUCCESS_RETURN;
}

int dm_shw_get_data_type(_IN_ void *data, _OU_ dm_shw_data_type_e *type)
{
    dm_shw_data_t *data_item = (dm_shw_data_t *)data;

    if (data_item == NULL || type == NULL) {
        return DM_INVALID_PARAMETER;
    }

    if (data_item->data_value.type == DM_SHW_DATA_TYPE_ARRAY) {
        dm_shw_data_value_complex_t *complex_value = (dm_shw_data_value_complex_t *)data_item->data_value.value;
        *type = complex_value->type;
    } else {
        *type = data_item->data_value.type;
    }

    return SUCCESS_RETURN;
}

int dm_shw_get_event(_IN_ dm_shw_t *shadow, _IN_ char *key, _IN_ int key_len, _OU_ void **event)
{
    int index = 0;
    dm_shw_event_t *dtsl_event = NULL;

    if (shadow == NULL || key == NULL || key_len <= 0) {
        return DM_INVALID_PARAMETER;
    }

    for (index = 0; index < shadow->event_number; index++) {
        dtsl_event = shadow->events + index;
        if ((strlen(dtsl_event->identifier) == key_len) &&
            (memcmp(dtsl_event->identifier, key, key_len) == 0)) {
            if (event) {
                *event = (dm_shw_event_t *)dtsl_event;
            }
            return SUCCESS_RETURN;
        }
    }

    return FAIL_RETURN;
}

int dm_shw_get_service(_IN_ dm_shw_t *shadow, _IN_ char *key, _IN_ int key_len, _OU_ void **service)
{
    int index = 0;
    dm_shw_service_t *dtsl_service = NULL;

    if (shadow == NULL || key == NULL || key_len <= 0) {
        return DM_INVALID_PARAMETER;
    }

    for (index = 0; index < shadow->event_number; index++) {
        dtsl_service = shadow->services + index;
        if ((strlen(dtsl_service->identifier) == key_len) &&
            (memcmp(dtsl_service->identifier, key, key_len) == 0)) {
            if (service) {
                *service = (dm_shw_service_t *)dtsl_service;
            }
            return SUCCESS_RETURN;
        }
    }

    return FAIL_RETURN;
}

int dm_shw_get_property_number(_IN_ dm_shw_t *shadow, _OU_ int *number)
{
    if (shadow == NULL || number == NULL) {
        return DM_INVALID_PARAMETER;
    }

    *number = shadow->property_number;

    return SUCCESS_RETURN;
}

int dm_shw_get_service_number(_IN_ dm_shw_t *shadow, _OU_ int *number)
{
    if (shadow == NULL || number == NULL) {
        return DM_INVALID_PARAMETER;
    }

    *number = shadow->service_number;

    return SUCCESS_RETURN;
}

int dm_shw_get_event_number(_IN_ dm_shw_t *shadow, _OU_ int *number)
{
    if (shadow == NULL || number == NULL) {
        return DM_INVALID_PARAMETER;
    }

    *number = shadow->event_number;

    return SUCCESS_RETURN;
}

int dm_shw_get_property_by_index(_IN_ dm_shw_t *shadow, _IN_ int index, _OU_ void **property)
{
    if (shadow == NULL || index < 0 || index >= shadow->property_number ||
        property == NULL || *property != NULL) {
        return DM_INVALID_PARAMETER;
    }

    *property = (void *)(shadow->properties + index);

    return SUCCESS_RETURN;
}

int dm_shw_get_service_by_index(_IN_ dm_shw_t *shadow, _IN_ int index, _OU_ void **service)
{
    if (shadow == NULL || index < 0 || index >= shadow->service_number ||
        service == NULL || *service != NULL) {
        return DM_INVALID_PARAMETER;
    }

    *service = (void *)(shadow->services + index);

    return SUCCESS_RETURN;
}

int dm_shw_get_event_by_index(_IN_ dm_shw_t *shadow, _IN_ int index, _OU_ void **event)
{
    if (shadow == NULL || index < 0 || index >= shadow->event_number ||
        event == NULL || *event != NULL) {
        return DM_INVALID_PARAMETER;
    }

    *event = (void *)(shadow->events + index);

    return SUCCESS_RETURN;
}

int dm_shw_get_service_by_identifier(_IN_ dm_shw_t *shadow, _IN_ char *identifier, _OU_ void **service)
{
    int index = 0;
    dm_shw_service_t *search_service = NULL;

    if (shadow == NULL || identifier == NULL ||
        service == NULL || *service != NULL) {
        return DM_INVALID_PARAMETER;
    }

    for (index = 0; index < shadow->service_number; index++) {
        search_service = shadow->services + index;
        if ((strlen(search_service->identifier) == strlen(identifier)) &&
            (memcmp(search_service->identifier, identifier, strlen(identifier)) == 0)) {
            *service = (void *)search_service;
            return SUCCESS_RETURN;
        }
    }

    return FAIL_RETURN;
}

int dm_shw_get_event_by_identifier(_IN_ dm_shw_t *shadow, _IN_ char *identifier, _OU_ void **event)
{
    int index = 0;
    dm_shw_event_t *search_event = NULL;

    if (shadow == NULL || identifier == NULL ||
        event == NULL || *event != NULL) {
        return DM_INVALID_PARAMETER;
    }

    for (index = 0; index < shadow->event_number; index++) {
        search_event = shadow->events + index;
        if ((strlen(search_event->identifier) == strlen(identifier)) &&
            (memcmp(search_event->identifier, identifier, strlen(identifier)) == 0)) {
            *event = (void *)search_event;
            return SUCCESS_RETURN;
        }
    }

    return FAIL_RETURN;
}

int dm_shw_get_property_identifier(_IN_ void *property, _OU_ char **identifier)
{
    dm_shw_data_t *property_item = (dm_shw_data_t *)property;

    if (property_item == NULL || identifier == NULL || *identifier != NULL) {
        return DM_INVALID_PARAMETER;
    }

    *identifier = property_item->identifier;

    return SUCCESS_RETURN;
}

int dm_shw_get_service_method(_IN_ void *service, _OU_ char **method)
{
    int service_method_len = 0;
    const char *service_method_fmt = "thing.service.%s";
    dm_shw_service_t *service_item = (dm_shw_service_t *)service;

    if (service_item == NULL || method == NULL || *method != NULL) {
        return DM_INVALID_PARAMETER;
    }

    service_method_len = (strlen(service_method_fmt) + strlen(service_item->identifier) + 1);
    *method = DM_malloc(service_method_len);
    if (*method == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(*method, 0, service_method_len);
    HAL_Snprintf(*method, service_method_len, service_method_fmt, service_item->identifier);

    return SUCCESS_RETURN;
}

int dm_shw_get_event_method(_IN_ void *event, _OU_ char **method)
{
    int event_method_len = 0;
    const char *post_identifier = "post";
    const char *property_identifier = "property";
    const char *identifier = NULL;
    const char *event_method_fmt = "thing.event.%s.post";
    dm_shw_event_t *event_item = (dm_shw_event_t *)event;

    if (event_item == NULL || method == NULL || *method != NULL) {
        return DM_INVALID_PARAMETER;
    }

    /* God Damn It Special Case! */
    if ((strlen(event_item->identifier) == strlen(post_identifier)) &&
        (memcmp(event_item->identifier, post_identifier, strlen(post_identifier)) == 0)) {
        identifier = property_identifier;
    } else {
        identifier = (const char *)event_item->identifier;
    }

    event_method_len = (strlen(event_method_fmt) + strlen(identifier) + 1);
    *method = DM_malloc(event_method_len);
    if (*method == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(*method, 0, event_method_len);
    HAL_Snprintf(*method, event_method_len, event_method_fmt, identifier);

    return SUCCESS_RETURN;
}

static int _dm_shw_int_set(_IN_ dm_shw_data_value_t *data_value, _IN_ void *value, _IN_ int value_len)
{
    int int_set = (value == NULL) ? (0) : (*(int *)value);

    data_value->value_int = int_set;
    dm_log_debug("Current Int Value Be Set(Int): %d", data_value->value_int);

    return SUCCESS_RETURN;
}

static int _dm_shw_float_set(_IN_ dm_shw_data_value_t *data_value, _IN_ void *value, _IN_ int value_len)
{
    float float_set = (value == NULL) ? (0) : (*(float *)value);

    data_value->value_float = float_set;
    dm_log_debug("Current Float Value Be Set(Float): %f", data_value->value_float);

    return SUCCESS_RETURN;
}

static int _dm_shw_double_set(_IN_ dm_shw_data_value_t *data_value, _IN_ void *value, _IN_ int value_len)
{
    double double_set = (value == NULL) ? (0) : (*(double *)value);

    data_value->value_double = double_set;
    dm_log_debug("Current Double Value Be Set(Double): %f", data_value->value_double);

    return SUCCESS_RETURN;
}

static int _dm_shw_text_set(_IN_ dm_shw_data_value_t *data_value, _IN_ void *value, _IN_ int value_len)
{
    int res = 0;
    char *value_set = (value == NULL) ? ("NULL") : ((char *)value);
    int value_set_len = (value == NULL) ? (strlen("NULL")) : (value_len);

    if (data_value->value) {
        DM_free(data_value->value);
        data_value->value = NULL;
    }
    res = dm_utils_copy(value_set, value_set_len, &data_value->value, value_set_len + 1);
    if (res != SUCCESS_RETURN) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    dm_log_debug("Current Text Value Be Set(String): %s", data_value->value);

    return SUCCESS_RETURN;
}

static int _dm_shw_enum_set(_IN_ dm_shw_data_value_t *data_value, _IN_ void *value, _IN_ int value_len)
{
    int enum_set = (value == NULL) ? (0) : (*(int *)value);

    enum_set = (enum_set < 0) ? (0) : (enum_set);

    data_value->value_int = enum_set;
    dm_log_debug("Current Enum Value Be Set(Enum): %d", data_value->value_int);

    return SUCCESS_RETURN;
}

static int _dm_shw_date_set(_IN_ dm_shw_data_value_t *data_value, _IN_ void *value, _IN_ int value_len)
{
    int res = 0;
    char *value_set = (value == NULL) ? ("NULL") : ((char *)value);
    int value_set_len = (value == NULL) ? (strlen("NULL")) : (value_len);

    if (data_value->value) {
        DM_free(data_value->value);
        data_value->value = NULL;
    }
    res = dm_utils_copy(value_set, value_set_len, &data_value->value, value_set_len + 1);
    if (res != SUCCESS_RETURN) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    dm_log_debug("Current Date Value Be Set(String): %s", data_value->value);

    return SUCCESS_RETURN;
}

static int _dm_shw_bool_set(_IN_ dm_shw_data_value_t *data_value, _IN_ void *value, _IN_ int value_len)
{
    int int_set = (value == NULL) ? (0) : (*(int *)value);

    int_set = (int_set == 0) ? (int_set) : ((int_set == 1) ? (int_set) : (0));

    data_value->value_int = int_set;
    dm_log_debug("Current Bool Value Be Set(Bool): %d", data_value->value_int);

    return SUCCESS_RETURN;
}

static int _dm_shw_array_int_set(_IN_ dm_shw_data_value_t *data_value, _IN_ void *value, _IN_ int value_len,
                                 _IN_ int index)
{
    dm_shw_data_value_complex_t *complex_array = data_value->value;
    int int_set = (value == NULL) ? (0) : (*(int *)value);

    *((int *)(complex_array->value) + index) = int_set;
    dm_log_debug("Current Array Value Be Set(Int), Index: %d, Value: %d", index, *((int *)(complex_array->value) + index));

    return SUCCESS_RETURN;
}

static int _dm_shw_array_float_set(_IN_ dm_shw_data_value_t *data_value, _IN_ void *value, _IN_ int value_len,
                                   _IN_ int index)
{
    dm_shw_data_value_complex_t *complex_array = data_value->value;
    float float_set = (value == NULL) ? (0) : (*(float *)value);

    *((float *)(complex_array->value) + index) = float_set;
    dm_log_debug("Current Array Value Be Set(Float), Index: %d, Value: %f", index,
                 *((float *)(complex_array->value) + index));

    return SUCCESS_RETURN;
}

static int _dm_shw_array_double_set(_IN_ dm_shw_data_value_t *data_value, _IN_ void *value, _IN_ int value_len,
                                    _IN_ int index)
{
    dm_shw_data_value_complex_t *complex_array = data_value->value;
    double double_set = (value == NULL) ? (0) : (*(double *)value);

    *((double *)(complex_array->value) + index) = double_set;
    dm_log_debug("Current Array Value Be Set(Double), Index: %d, Value: %f", index,
                 *((double *)(complex_array->value) + index));

    return SUCCESS_RETURN;
}

static int _dm_shw_array_text_set(_IN_ dm_shw_data_value_t *data_value, _IN_ void *value, _IN_ int value_len,
                                  _IN_ int index)
{
    int res = 0;
    dm_shw_data_value_complex_t *complex_array = data_value->value;
    char *text_set = (value == NULL) ? ("NULL") : ((char *)value);
    int value_set_len = (value == NULL) ? (strlen("NULL")) : (value_len);

    if (*((char **)(complex_array->value) + index)) {
        DM_free(*((char **)(complex_array->value) + index));
        *((char **)(complex_array->value) + index) = NULL;
    }

    res = dm_utils_copy(text_set, value_set_len, (void **)((char **)(complex_array->value) + index), value_set_len + 1);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }
    dm_log_debug("Current Array Value Be Set(Text String), Index: %d, Value: %s", index,
                 *((char **)(complex_array->value) + index));

    return SUCCESS_RETURN;
}

static int _dm_shw_array_enum_set(_IN_ dm_shw_data_value_t *data_value, _IN_ void *value, _IN_ int value_len,
                                  _IN_ int index)
{
    dm_shw_data_value_complex_t *complex_array = data_value->value;
    int int_set = (value == NULL) ? (0) : (*(int *)value);

    *((int *)(complex_array->value) + index) = int_set;
    dm_log_debug("Current Array Value Be Set(Enum), Index: %d, Value: %d", index, *((int *)(complex_array->value) + index));

    return SUCCESS_RETURN;
}

static int _dm_shw_array_date_set(_IN_ dm_shw_data_value_t *data_value, _IN_ void *value, _IN_ int value_len,
                                  _IN_ int index)
{
    int res = 0;
    dm_shw_data_value_complex_t *complex_array = data_value->value;
    char *text_set = (value == NULL) ? ("NULL") : ((char *)value);
    int value_set_len = (value == NULL) ? (strlen("NULL")) : (value_len);

    if (*((char **)(complex_array->value) + index)) {
        DM_free(*((char **)(complex_array->value) + index));
        *((char **)(complex_array->value) + index) = NULL;
    }

    res = dm_utils_copy(text_set, value_set_len, (void **)((char **)(complex_array->value) + index), value_set_len + 1);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }
    dm_log_debug("Current Array Value Be Set(Date String), Index: %d, Value: %s", index,
                 *((char **)(complex_array->value) + index));

    return SUCCESS_RETURN;
}

static int _dm_shw_array_bool_set(_IN_ dm_shw_data_value_t *data_value, _IN_ void *value, _IN_ int value_len,
                                  _IN_ int index)
{
    dm_shw_data_value_complex_t *complex_array = data_value->value;
    int int_set = (value == NULL) ? (0) : (*(int *)value);

    *((int *)(complex_array->value) + index) = int_set;
    dm_log_debug("Current Array Value Be Set(Bool), Index: %d, Value: %d", index, *((int *)(complex_array->value) + index));

    return SUCCESS_RETURN;
}


static int _dm_shw_data_array_set(_IN_ dm_shw_data_value_t *data_value, _IN_ void *value, _IN_ int value_len,
                                  _IN_ int index)
{
    dm_shw_data_value_complex_t *complex_array = data_value->value;

    if (complex_array == NULL || index < 0 || index >= complex_array->size) {
        return DM_INVALID_PARAMETER;
    }

    if (g_iotx_data_type_mapping[complex_array->type].func_array_set == NULL) {
        return FAIL_RETURN;
    }

    return g_iotx_data_type_mapping[complex_array->type].func_array_set(data_value, value, value_len, index);
}

static int _dm_shw_data_set(_IN_ dm_shw_data_value_t *data_value, _IN_ void *value, _IN_ int value_len)
{
    if (g_iotx_data_type_mapping[data_value->type].func_set == NULL) {
        return FAIL_RETURN;
    }

    return g_iotx_data_type_mapping[data_value->type].func_set(data_value, value, value_len);
}

int dm_shw_set_property_value(_IN_ dm_shw_t *shadow, _IN_ char *key, _IN_ int key_len, _IN_ void *value,
                              _IN_ int value_len)
{
    int res = 0, array_index = 0;
    dm_shw_data_t *data = NULL;

    if (shadow == NULL || key == NULL || key_len <= 0) {
        return FAIL_RETURN;
    }

    dm_log_debug("Key:%d %s", key_len, key);
    res = _dm_shw_property_search(shadow, key, key_len, &data, &array_index);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    if (data->data_value.type == DM_SHW_DATA_TYPE_ARRAY) {
        dm_log_debug("Current Found Data Index: %d", array_index);
        res = _dm_shw_data_array_set(&data->data_value, value, value_len, array_index);
        if (res != SUCCESS_RETURN) {
            return DM_TSL_PROPERTY_SET_FAILED;
        }
    } else {
        res = _dm_shw_data_set(&data->data_value, value, value_len);
        if (res != SUCCESS_RETURN) {
            return DM_TSL_PROPERTY_SET_FAILED;
        }
    }

    return SUCCESS_RETURN;
}

static int _dm_shw_int_get(_IN_ dm_shw_data_value_t *data_value, _IN_ void *value)
{
    *(int *)(value) = data_value->value_int;
    return SUCCESS_RETURN;
}

static int _dm_shw_float_get(_IN_ dm_shw_data_value_t *data_value, _IN_ void *value)
{
    *(float *)(value) = data_value->value_float;
    return SUCCESS_RETURN;
}

static int _dm_shw_double_get(_IN_ dm_shw_data_value_t *data_value, _IN_ void *value)
{
    *(double *)(value) = data_value->value_double;
    return SUCCESS_RETURN;
}

static int _dm_shw_text_get(_IN_ dm_shw_data_value_t *data_value, _IN_ void *value)
{
    int res = 0;

    if (*(char **)value != NULL || data_value->value == NULL) {
        return FAIL_RETURN;
    }

    res = dm_utils_copy_direct(data_value->value, strlen(data_value->value), (void **)value, strlen(data_value->value) + 1);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

static int _dm_shw_enum_get(_IN_ dm_shw_data_value_t *data_value, _IN_ void *value)
{
    *(int *)(value) = data_value->value_int;
    return SUCCESS_RETURN;
}

static int _dm_shw_date_get(_IN_ dm_shw_data_value_t *data_value, _IN_ void *value)
{
    int res = 0;

    if (*(char **)value != NULL || data_value->value == NULL) {
        return FAIL_RETURN;
    }

    res = dm_utils_copy_direct(data_value->value, strlen(data_value->value), (void **)value, strlen(data_value->value) + 1);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

static int _dm_shw_bool_get(_IN_ dm_shw_data_value_t *data_value, _IN_ void *value)
{
    *(int *)(value) = data_value->value_int;
    return SUCCESS_RETURN;
}

static int _dm_shw_array_int_get(_IN_ dm_shw_data_value_t *data_value, _IN_ void *value, _IN_ int index)
{
    dm_shw_data_value_complex_t *complex_array = data_value->value;

    if (complex_array->value == NULL || ((int *)(complex_array->value) + index) == NULL) {
        return DM_INVALID_PARAMETER;
    }

    *((int *)value) = *((int *)(complex_array->value) + index);

    return SUCCESS_RETURN;
}

static int _dm_shw_array_float_get(_IN_ dm_shw_data_value_t *data_value, _IN_ void *value, _IN_ int index)
{
    dm_shw_data_value_complex_t *complex_array = data_value->value;

    if (complex_array->value == NULL || ((float *)(complex_array->value) + index) == NULL) {
        return DM_INVALID_PARAMETER;
    }

    *((float *)value) = *((float *)(complex_array->value) + index);

    return SUCCESS_RETURN;
}

static int _dm_shw_array_double_get(_IN_ dm_shw_data_value_t *data_value, _IN_ void *value, _IN_ int index)
{
    dm_shw_data_value_complex_t *complex_array = data_value->value;

    if (complex_array->value == NULL || ((double *)(complex_array->value) + index) == NULL) {
        return DM_INVALID_PARAMETER;
    }

    *((double *)value) = *((double *)(complex_array->value) + index);

    return SUCCESS_RETURN;
}

static int _dm_shw_array_text_get(_IN_ dm_shw_data_value_t *data_value, _IN_ void *value, _IN_ int index)
{
    int res = 0;
    dm_shw_data_value_complex_t *complex_array = data_value->value;

    if (complex_array->value == NULL || *((char **)(complex_array->value) + index) == NULL || *(char **)value != NULL) {
        return DM_INVALID_PARAMETER;
    }

    res = dm_utils_copy_direct(*((char **)(complex_array->value) + index),
                               strlen(*((char **)(complex_array->value) + index)),
                               (void **)value, strlen(*((char **)(complex_array->value) + index)) + 1);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

static int _dm_shw_array_enum_get(_IN_ dm_shw_data_value_t *data_value, _IN_ void *value, _IN_ int index)
{
    dm_shw_data_value_complex_t *complex_array = data_value->value;

    if (complex_array->value == NULL || ((int *)(complex_array->value) + index) == NULL) {
        return DM_INVALID_PARAMETER;
    }

    *((int *)value) = *((int *)(complex_array->value) + index);

    return SUCCESS_RETURN;
}

static int _dm_shw_array_date_get(_IN_ dm_shw_data_value_t *data_value, _IN_ void *value, _IN_ int index)
{
    int res = 0;
    dm_shw_data_value_complex_t *complex_array = data_value->value;

    if (complex_array->value == NULL || *((char **)(complex_array->value) + index) == NULL || *(char **)value != NULL) {
        return DM_INVALID_PARAMETER;
    }

    res = dm_utils_copy_direct(*((char **)(complex_array->value) + index),
                               strlen(*((char **)(complex_array->value) + index)),
                               (void **)value, strlen(*((char **)(complex_array->value) + index)) + 1);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

static int _dm_shw_array_bool_get(_IN_ dm_shw_data_value_t *data_value, _IN_ void *value, _IN_ int index)
{
    dm_shw_data_value_complex_t *complex_array = data_value->value;

    if (complex_array->value == NULL || ((int *)(complex_array->value) + index) == NULL) {
        return DM_INVALID_PARAMETER;
    }

    *((int *)value) = *((int *)(complex_array->value) + index);

    return SUCCESS_RETURN;
}

static int _dm_shw_data_array_get(_IN_ dm_shw_data_value_t *data_value, _IN_ void *value, _IN_ int index)
{
    dm_shw_data_value_complex_t *complex_array = data_value->value;

    if (complex_array == NULL || index < 0 || index >= complex_array->size) {
        return FAIL_RETURN;
    }

    if (g_iotx_data_type_mapping[complex_array->type].func_array_get == NULL) {
        return FAIL_RETURN;
    }

    return g_iotx_data_type_mapping[complex_array->type].func_array_get(data_value, value, index);
}

static int _dm_shw_data_get(_IN_ dm_shw_data_value_t *data_value, _IN_ void *value)
{
    if (g_iotx_data_type_mapping[data_value->type].func_get == NULL) {
        return FAIL_RETURN;
    }

    return g_iotx_data_type_mapping[data_value->type].func_get(data_value, value);
}

int dm_shw_get_property_value(_IN_ dm_shw_t *shadow, _IN_ char *key, _IN_ int key_len, _OU_ void *value)
{
    int res = 0, array_index = 0;
    dm_shw_data_t *data = NULL;

    if (shadow == NULL || key == NULL || key_len <= 0) {
        return DM_INVALID_PARAMETER;
    }

    res = _dm_shw_property_search(shadow, key, key_len, &data, &array_index);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }
    dm_log_debug("Current Found Data: %s", data->identifier);
    if (data->data_value.type == DM_SHW_DATA_TYPE_ARRAY) {
        dm_log_debug("Current Found Data Index: %d", array_index);
        res = _dm_shw_data_array_get(&data->data_value, value, array_index);
        if (res != SUCCESS_RETURN) {
            return DM_TSL_PROPERTY_GET_FAILED;
        }
    } else {
        res = _dm_shw_data_get(&data->data_value, value);
        if (res != SUCCESS_RETURN) {
            return DM_TSL_PROPERTY_GET_FAILED;
        }
    }

    return SUCCESS_RETURN;
}

int dm_shw_set_event_output_value(_IN_ dm_shw_t *shadow, _IN_ char *key, _IN_ int key_len, _IN_ void *value,
                                  _IN_ int value_len)
{
    int res = 0, array_index = 0;
    int offset = 0;
    char *pos = NULL;
    dm_shw_event_t *event = NULL;
    dm_shw_data_t *event_data = NULL;

    if (shadow == NULL || key == NULL || key_len <= 0) {
        return DM_INVALID_PARAMETER;
    }

    res = dm_utils_memtok(key, key_len, DM_SHW_KEY_DELIMITER, 1, &offset);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    dm_log_debug("Key: %.*s", key_len, key);

    res = _dm_shw_event_search(shadow, key, offset, &event);
    if (res != SUCCESS_RETURN) {
        return DM_TSL_EVENT_NOT_EXIST;
    }

    pos = key + offset + 1;
    /* dm_log_debug("TSL Event Output Data Search, Event Data ID: %s",pos); */

    res = _dm_shw_event_output_search(event->output_datas, event->output_data_number, pos, strlen(pos), &event_data,
                                      &array_index);
    if (res != SUCCESS_RETURN) {
        return DM_TSL_EVENT_NOT_EXIST;
    }

    if (event_data->data_value.type == DM_SHW_DATA_TYPE_ARRAY) {
        res = _dm_shw_data_array_set(&event_data->data_value, value, value_len, array_index);
        if (res != SUCCESS_RETURN) {
            return DM_TSL_EVENT_SET_FAILED;
        }
    } else {
        res = _dm_shw_data_set(&event_data->data_value, value, value_len);
        if (res != SUCCESS_RETURN) {
            return DM_TSL_EVENT_SET_FAILED;
        }
    }

    return SUCCESS_RETURN;
}

int dm_shw_get_event_output_value(_IN_ dm_shw_t *shadow, _IN_ char *key, _IN_ int key_len, _IN_ void *value)
{
    int res = 0;
    int offset = 0, array_index = 0;
    char *pos = NULL;
    dm_shw_event_t *event = NULL;
    dm_shw_data_t *event_data = NULL;

    if (shadow == NULL || key == NULL || key_len <= 0) {
        return DM_INVALID_PARAMETER;
    }

    res = dm_utils_memtok(key, key_len, DM_SHW_KEY_DELIMITER, 1, &offset);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    dm_log_debug("Key: %.*s", key_len, key);

    res = _dm_shw_event_search(shadow, key, offset, &event);
    if (res != SUCCESS_RETURN) {
        return DM_TSL_EVENT_NOT_EXIST;
    }

    pos = key + offset + 1;
    dm_log_debug("TSL Event Output Data Search, Event Data ID: %s", pos);

    res = _dm_shw_event_output_search(event->output_datas, event->output_data_number, pos, strlen(pos), &event_data,
                                      &array_index);
    if (res != SUCCESS_RETURN) {
        return DM_TSL_EVENT_NOT_EXIST;
    }

    if (event_data->data_value.type == DM_SHW_DATA_TYPE_ARRAY) {
        res = _dm_shw_data_array_get(&event_data->data_value, value, array_index);
        if (res != SUCCESS_RETURN) {
            return DM_TSL_EVENT_GET_FAILED;
        }
    } else {
        res = _dm_shw_data_get(&event_data->data_value, value);
        if (res != SUCCESS_RETURN) {
            return DM_TSL_EVENT_GET_FAILED;
        }
    }

    return SUCCESS_RETURN;
}

int dm_shw_set_service_input_output_value(_IN_ dm_shw_data_target_e type, _IN_ dm_shw_t *shadow, _IN_ char *key,
        _IN_ int key_len, _IN_ void *value, _IN_ int value_len)
{
    int res = 0, array_index = 0;
    int offset = 0;
    char *pos = NULL;
    dm_shw_service_t *service = NULL;
    dm_shw_data_t *service_data = NULL;

    if (type < DM_SHW_DATA_TARGET_SERVICE_INPUT_DATA || type > DM_SHW_DATA_TARGET_SERVICE_OUTPUT_DATA || shadow == NULL
        || key == NULL || key_len <= 0) {
        return DM_INVALID_PARAMETER;
    }

    res = dm_utils_memtok(key, key_len, DM_SHW_KEY_DELIMITER, 1, &offset);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    dm_log_debug("Key: %.*s", key_len, key);

    res = _dm_shw_service_search(shadow, key, offset, &service);
    if (res != SUCCESS_RETURN) {
        return DM_TSL_SERVICE_NOT_EXIST;
    }

    pos = key + offset + 1;
    dm_log_debug("TSL Service Input/Output Data Search, Event Data ID: %s", pos);

    res = _dm_shw_service_input_output_search(type, service, pos, strlen(pos), &service_data, &array_index);
    if (res != SUCCESS_RETURN) {
        return DM_TSL_SERVICE_NOT_EXIST;
    }

    if (service_data->data_value.type == DM_SHW_DATA_TYPE_ARRAY) {
        res = _dm_shw_data_array_set(&service_data->data_value, value, value_len, array_index);
        if (res != SUCCESS_RETURN) {
            return DM_TSL_SERVICE_SET_FAILED;
        }
    } else {
        res = _dm_shw_data_set(&service_data->data_value, value, value_len);
        if (res != SUCCESS_RETURN) {
            return DM_TSL_SERVICE_SET_FAILED;
        }
    }

    return SUCCESS_RETURN;
}

int dm_shw_get_service_input_output_value(_IN_ dm_shw_data_target_e type, _IN_ dm_shw_t *shadow, _IN_ char *key,
        _IN_ int key_len, _IN_ void *value)
{
    int res = 0;
    int offset = 0, array_index = 0;
    char *pos = NULL;
    dm_shw_service_t *service = NULL;
    dm_shw_data_t *service_data = NULL;

    if (shadow == NULL || key == NULL || key_len <= 0) {
        return DM_INVALID_PARAMETER;
    }

    res = dm_utils_memtok(key, key_len, DM_SHW_KEY_DELIMITER, 1, &offset);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    dm_log_debug("key: %.*s", key_len, key);

    res = _dm_shw_service_search(shadow, key, offset, &service);
    if (res != SUCCESS_RETURN) {
        return DM_TSL_SERVICE_NOT_EXIST;
    }

    pos = key + offset + 1;
    dm_log_debug("TSL Service Input/Output Data Search, Event Data ID: %s", pos);

    res = _dm_shw_service_input_output_search(type, service, pos, strlen(pos), &service_data, &array_index);
    if (res != SUCCESS_RETURN) {
        return DM_TSL_SERVICE_NOT_EXIST;
    }

    if (service_data->data_value.type == DM_SHW_DATA_TYPE_ARRAY) {
        res = _dm_shw_data_array_get(&service_data->data_value, value, array_index);
        if (res != SUCCESS_RETURN) {
            return DM_TSL_SERVICE_GET_FAILED;
        }
    } else {
        res = _dm_shw_data_get(&service_data->data_value, value);
        if (res != SUCCESS_RETURN) {
            return DM_TSL_SERVICE_GET_FAILED;
        }
    }

    return SUCCESS_RETURN;
}

static int _dm_shw_int_insert_json_item(_IN_ dm_shw_data_t *data, _IN_ lite_cjson_item_t *lite)
{
    lite_cjson_add_number_to_object(lite, data->identifier, data->data_value.value_int);

    return SUCCESS_RETURN;
}

static int _dm_shw_float_insert_json_item(_IN_ dm_shw_data_t *data, _IN_ lite_cjson_item_t *lite)
{
    lite_cjson_add_number_to_object(lite, data->identifier, data->data_value.value_float);

    return SUCCESS_RETURN;
}

static int _dm_shw_double_insert_json_item(_IN_ dm_shw_data_t *data, _IN_ lite_cjson_item_t *lite)
{
    lite_cjson_add_number_to_object(lite, data->identifier, data->data_value.value_double);

    return SUCCESS_RETURN;
}

static int _dm_shw_string_insert_json_item(_IN_ dm_shw_data_t *data, _IN_ lite_cjson_item_t *lite)
{
    char *value = (data->data_value.value == NULL) ? ("") : (data->data_value.value);
    lite_cjson_add_string_to_object(lite, data->identifier, value);

    return SUCCESS_RETURN;
}

static int _dm_shw_array_insert_json_item(_IN_ dm_shw_data_t *data, _IN_ lite_cjson_item_t *lite);
static int _dm_shw_struct_insert_json_item(_IN_ dm_shw_data_t *data, _IN_ lite_cjson_item_t *lite);
static int _dm_shw_data_insert_json_item(_IN_ dm_shw_data_t *data, _IN_ lite_cjson_item_t *lite);

static int _dm_shw_array_insert_json_item(_IN_ dm_shw_data_t *data, _IN_ lite_cjson_item_t *lite)
{
    int res = SUCCESS_RETURN, index = 0;
    lite_cjson_item_t *array = NULL, *array_item = NULL;
    dm_shw_data_value_complex_t *complex_array = NULL;

    if (data == NULL || lite == NULL) {
        return DM_INVALID_PARAMETER;
    }

    complex_array = data->data_value.value;

    if (lite->type == cJSON_Array) {
        array = lite_cjson_create_object();
        if (array == NULL) {
            return DM_MEMORY_NOT_ENOUGH;
        }
    }

    array_item = lite_cjson_create_array();
    if (array_item == NULL) {
        if (array) {
            lite_cjson_delete(array);
        }
        return DM_MEMORY_NOT_ENOUGH;
    }

    switch (complex_array->type) {
        case DM_SHW_DATA_TYPE_INT:
        case DM_SHW_DATA_TYPE_BOOL:
        case DM_SHW_DATA_TYPE_ENUM: {
            int value = 0;
            for (index = 0; index < complex_array->size; index++) {
                value = *((int *)(complex_array->value) + index);
                lite_cjson_add_item_to_array(array_item, lite_cjson_create_number((double)value));
            }
            if (lite->type == cJSON_Array) {
                lite_cjson_add_item_to_object(array, data->identifier, array_item);
                lite_cjson_add_item_to_array(lite, array);
            } else {
                lite_cjson_add_item_to_object(lite, data->identifier, array_item);
                lite_cjson_delete(array);
            }
        }
        break;
        case DM_SHW_DATA_TYPE_FLOAT: {
            float value = 0;
            for (index = 0; index < complex_array->size; index++) {
                value = *((float *)(complex_array->value) + index);
                lite_cjson_add_item_to_array(array_item, lite_cjson_create_number((double)value));
            }
            if (lite->type == cJSON_Array) {
                lite_cjson_add_item_to_object(array, data->identifier, array_item);
                lite_cjson_add_item_to_array(lite, array);
            } else {
                lite_cjson_add_item_to_object(lite, data->identifier, array_item);
                lite_cjson_delete(array);
            }
        }
        break;
        case DM_SHW_DATA_TYPE_DOUBLE: {
            double value = 0;
            for (index = 0; index < complex_array->size; index++) {
                value = *((double *)(complex_array->value) + index);
                lite_cjson_add_item_to_array(array_item, lite_cjson_create_number(value));
            }
            if (lite->type == cJSON_Array) {
                lite_cjson_add_item_to_object(array, data->identifier, array_item);
                lite_cjson_add_item_to_array(lite, array);
            } else {
                lite_cjson_add_item_to_object(lite, data->identifier, array_item);
                lite_cjson_delete(array);
            }
        }
        break;
        case DM_SHW_DATA_TYPE_TEXT:
        case DM_SHW_DATA_TYPE_DATE: {
            char *value = NULL;
            for (index = 0; index < complex_array->size; index++) {
                value = *((char **)(complex_array->value) + index);
                value = (value == NULL) ? ("") : (value);
                lite_cjson_add_item_to_array(array_item, lite_cjson_create_string((const char *)value));
            }
            if (lite->type == cJSON_Array) {
                lite_cjson_add_item_to_object(array, data->identifier, array_item);
                lite_cjson_add_item_to_array(lite, array);
            } else {
                lite_cjson_add_item_to_object(lite, data->identifier, array_item);
                lite_cjson_delete(array);
            }
        }
        break;
        case DM_SHW_DATA_TYPE_ARRAY: {
            /* TODO */
        }
        break;
        case DM_SHW_DATA_TYPE_STRUCT: {
            dm_shw_data_t *array_data = NULL;
            for (index = 0; index < complex_array->size; index++) {
                array_data = (dm_shw_data_t *)(complex_array->value) + index;
                if (array_data) {
                    _dm_shw_struct_insert_json_item(array_data, array_item);
                }
            }

            if (lite->type == cJSON_Array) {
                lite_cjson_add_item_to_object(array, data->identifier, array_item);
                lite_cjson_add_item_to_array(lite, array);
            } else {
                lite_cjson_add_item_to_object(lite, data->identifier, array_item);
                lite_cjson_delete(array);
            }
        }
        break;
        default: {
            lite_cjson_delete(array_item);
            lite_cjson_delete(array);
        }
        break;
    }

    return res;
}

static int _dm_shw_struct_insert_json_item(_IN_ dm_shw_data_t *data, _IN_ lite_cjson_item_t *lite)
{
    int res = 0, index = 0;
    lite_cjson_item_t *lite_object = NULL, *lite_item = NULL;
    dm_shw_data_t *current_data = NULL;
    dm_shw_data_value_complex_t *complex_struct = NULL;

    if (data == NULL || lite == NULL) {
        return DM_INVALID_PARAMETER;
    }

    if (lite->type == cJSON_Array) {
        lite_object = lite_cjson_create_object();
        if (lite_object == NULL) {
            return DM_MEMORY_NOT_ENOUGH;
        }
    }

    lite_item = lite_cjson_create_object();
    if (lite_item == NULL) {
        lite_cjson_delete(lite_object);
        return DM_MEMORY_NOT_ENOUGH;
    }

    complex_struct = data->data_value.value;

    for (index = 0; index < complex_struct->size; index++) {
        current_data = (dm_shw_data_t *)complex_struct->value + index;
        _dm_shw_data_insert_json_item(current_data, lite_item);
    }
    if (lite->type == cJSON_Array) {
        if (data->identifier) {
            lite_cjson_add_item_to_object(lite_object, data->identifier, lite_item);
            lite_cjson_add_item_to_array(lite, lite_object);
        } else {
            lite_cjson_add_item_to_array(lite, lite_item);
            lite_cjson_delete(lite_object);
        }
    } else {
        if (data->identifier) {
            lite_cjson_add_item_to_object(lite, data->identifier, lite_item);
            lite_cjson_delete(lite_object);
        } else {
            res = FAIL_RETURN;
            lite_cjson_delete(lite_item);
            lite_cjson_delete(lite_object);
        }
    }

    return res;
}

static int _dm_shw_data_insert_json_item(_IN_ dm_shw_data_t *data, _IN_ lite_cjson_item_t *lite)
{
    int res = 0;
    lite_cjson_item_t *data_object = NULL;

    if (data == NULL || lite == NULL) {
        return DM_INVALID_PARAMETER;
    }

    if (lite->type == cJSON_Array) {
        data_object = lite_cjson_create_object();
        if (data_object == NULL) {
            return DM_MEMORY_NOT_ENOUGH;
        }
    }

    switch (data->data_value.type) {
        case DM_SHW_DATA_TYPE_INT:
        case DM_SHW_DATA_TYPE_BOOL:
        case DM_SHW_DATA_TYPE_ENUM: {
            if (lite->type == cJSON_Array) {
                res = _dm_shw_int_insert_json_item(data, data_object);
                if (res == SUCCESS_RETURN) {
                    lite_cjson_add_item_to_array(lite, data_object);
                }
            } else {
                res = _dm_shw_int_insert_json_item(data, lite);
                lite_cjson_delete(data_object);
            }
        }
        break;
        case DM_SHW_DATA_TYPE_FLOAT: {
            if (lite->type == cJSON_Array) {
                res = _dm_shw_float_insert_json_item(data, data_object);
                if (res == SUCCESS_RETURN) {
                    lite_cjson_add_item_to_array(lite, data_object);
                }
            } else {
                res = _dm_shw_float_insert_json_item(data, lite);
                lite_cjson_delete(data_object);
            }
        }
        break;
        case DM_SHW_DATA_TYPE_DOUBLE: {
            if (lite->type == cJSON_Array) {
                res = _dm_shw_double_insert_json_item(data, data_object);
                if (res == SUCCESS_RETURN) {
                    lite_cjson_add_item_to_array(lite, data_object);
                }
            } else {
                res = _dm_shw_double_insert_json_item(data, lite);
                lite_cjson_delete(data_object);
            }
        }
        break;
        case DM_SHW_DATA_TYPE_TEXT:
        case DM_SHW_DATA_TYPE_DATE: {
            if (lite->type == cJSON_Array) {
                res = _dm_shw_string_insert_json_item(data, data_object);
                if (res == SUCCESS_RETURN) {
                    lite_cjson_add_item_to_array(lite, data_object);
                }
            } else {
                res = _dm_shw_string_insert_json_item(data, lite);
                lite_cjson_delete(data_object);
            }
        }
        break;
        case DM_SHW_DATA_TYPE_ARRAY: {
            /* dm_log_debug("DM_SHW_DATA_TYPE_ARRAY"); */
            if (lite->type == cJSON_Array) {
                res = _dm_shw_array_insert_json_item(data, data_object);
                if (res == SUCCESS_RETURN) {
                    lite_cjson_add_item_to_array(lite, data_object);
                }
            } else {
                res = _dm_shw_array_insert_json_item(data, lite);
                lite_cjson_delete(data_object);
            }
        }
        break;
        case DM_SHW_DATA_TYPE_STRUCT: {
            /* dm_log_debug("DM_SHW_DATA_TYPE_STRUCT"); */
            if (lite->type == cJSON_Array) {
                res = _dm_shw_struct_insert_json_item(data, data_object);
                if (res == SUCCESS_RETURN) {
                    lite_cjson_add_item_to_array(lite, data_object);
                }
            } else {
                res = _dm_shw_struct_insert_json_item(data, lite);
                lite_cjson_delete(data_object);
            }
        }
        break;
        default:
            lite_cjson_delete(data_object);
            res = FAIL_RETURN;
            break;
    }

    return res;
}

int dm_shw_assemble_property(_IN_ dm_shw_t *shadow, _IN_ char *identifier, _IN_ int identifier_len,
                             _IN_ lite_cjson_item_t *lite)
{
    int res = 0, index = 0;
    dm_shw_data_t *property = NULL;

    if (shadow == NULL || identifier == NULL || identifier_len <= 0 || lite == NULL || lite->type != cJSON_Object) {
        return DM_INVALID_PARAMETER;
    }

    for (index = 0; index < shadow->property_number; index++) {
        property = shadow->properties + index;
        if ((strlen(property->identifier) == identifier_len) &&
            (memcmp(property->identifier, identifier, identifier_len) == 0)) {
            /* dm_log_debug("Property Found: %.*s",identifier_len,identifier); */
            break;
        }
    }

    if (index == shadow->property_number) {
        dm_log_debug("Property Not Found: %.*s", identifier_len, identifier);
        return FAIL_RETURN;
    }

    res = _dm_shw_data_insert_json_item(property, lite);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

int dm_shw_assemble_event_output(_IN_ dm_shw_t *shadow, _IN_ char *identifier, _IN_ int identifier_len,
                                 _IN_ lite_cjson_item_t *lite)
{
    int res = 0, index = 0;
    dm_shw_data_t *event_outputdata = NULL;
    dm_shw_event_t *event = NULL;

    if (shadow == NULL || identifier == NULL || identifier_len <= 0 || lite == NULL || lite->type != cJSON_Object) {
        return DM_INVALID_PARAMETER;
    }

    for (index = 0; index < shadow->event_number; index++) {
        event = shadow->events + index;
        if ((strlen(event->identifier) == identifier_len) &&
            (memcmp(event->identifier, identifier, identifier_len) == 0)) {
            /* dm_log_debug("Event Found: %.*s",identifier_len,identifier); */
            break;
        }
    }

    if (index == shadow->event_number) {
        dm_log_debug("Event Not Found: %.*s", identifier_len, identifier);
        return FAIL_RETURN;
    }

    for (index = 0; index < event->output_data_number; index++) {
        event_outputdata = event->output_datas + index;

        res = _dm_shw_data_insert_json_item(event_outputdata, lite);
        if (res != SUCCESS_RETURN) {
            return FAIL_RETURN;
        }
    }

    return SUCCESS_RETURN;
}

int dm_shw_assemble_service_output(_IN_ dm_shw_t *shadow, _IN_ char *identifier, _IN_ int identifier_len,
                                   _IN_ lite_cjson_item_t *lite)
{
    int res = 0, index = 0;
    dm_shw_data_t *service_outputdata = NULL;
    dm_shw_service_t *service = NULL;

    if (shadow == NULL || identifier == NULL || identifier_len <= 0 || lite == NULL || lite->type != cJSON_Object) {
        return DM_INVALID_PARAMETER;
    }

    for (index = 0; index < shadow->service_number; index++) {
        service = shadow->services + index;
        if ((strlen(service->identifier) == identifier_len) &&
            (memcmp(service->identifier, identifier, identifier_len) == 0)) {
            /* dm_log_debug("Service Found: %.*s",identifier_len,identifier); */
            break;
        }
    }

    if (index == shadow->service_number) {
        dm_log_debug("Service Not Found: %.*s", identifier_len, identifier);
        return FAIL_RETURN;
    }

    for (index = 0; index < service->output_data_number; index++) {
        service_outputdata = service->output_datas + index;

        res = _dm_shw_data_insert_json_item(service_outputdata, lite);
        if (res != SUCCESS_RETURN) {
            return FAIL_RETURN;
        }
    }

    return SUCCESS_RETURN;
}

static void _dm_shw_int_free(_IN_ dm_shw_data_value_t *data_value)
{
    /* Free Value */
    /* if (data_value->value) {DM_free(data_value->value);data_value->value = NULL;} */
}

static void _dm_shw_float_free(_IN_ dm_shw_data_value_t *data_value)
{
    /* Free Value */
    /* if (data_value->value) {DM_free(data_value->value);data_value->value = NULL;} */
}

static void _dm_shw_double_free(_IN_ dm_shw_data_value_t *data_value)
{
    /* Free Value */
    /* if (data_value->value) {DM_free(data_value->value);data_value->value = NULL;} */
}

static void _dm_shw_text_free(_IN_ dm_shw_data_value_t *data_value)
{
    /* Free Value */
    if (data_value->value) {
        DM_free(data_value->value);
        data_value->value = NULL;
    }
}

static void _dm_shw_enum_free(_IN_ dm_shw_data_value_t *data_value)
{
    /* Free Value */
    /* if (data_value->value) {DM_free(data_value->value);data_value->value = NULL;} */
}

static void _dm_shw_date_free(_IN_ dm_shw_data_value_t *data_value)
{
    /* Free Value */
    if (data_value->value) {
        DM_free(data_value->value);
        data_value->value = NULL;
    }
}

static void _dm_shw_bool_free(_IN_ dm_shw_data_value_t *data_value)
{
    /* Free Value */
    /* if (data_value->value) {DM_free(data_value->value);data_value->value = NULL;} */
}

static void _dm_shw_array_int_free(_IN_ dm_shw_data_value_t *data_value)
{

}

static void _dm_shw_array_float_free(_IN_ dm_shw_data_value_t *data_value)
{

}

static void _dm_shw_array_double_free(_IN_ dm_shw_data_value_t *data_value)
{

}

static void _dm_shw_array_text_free(_IN_ dm_shw_data_value_t *data_value)
{
    int index = 0;
    dm_shw_data_value_complex_t *complex_array = data_value->value;

    /* Free Value */
    if (complex_array) {
        for (index = 0; index < complex_array->size; index++) {
            if (*((char **)(complex_array->value) + index)) {
                DM_free(*((char **)(complex_array->value) + index));
                *((char **)(complex_array->value) + index) = NULL;
            }
        }
    }
}

static void _dm_shw_array_enum_free(_IN_ dm_shw_data_value_t *data_value)
{

}

static void _dm_shw_array_date_free(_IN_ dm_shw_data_value_t *data_value)
{
    int index = 0;
    dm_shw_data_value_complex_t *complex_array = data_value->value;

    /* Free Value */
    if (complex_array) {
        for (index = 0; index < complex_array->size; index++) {
            if (*((char **)(complex_array->value) + index)) {
                DM_free(*((char **)(complex_array->value) + index));
                *((char **)(complex_array->value) + index) = NULL;
            }
        }
    }
}

static void _dm_shw_array_bool_free(_IN_ dm_shw_data_value_t *data_value)
{

}

static void _dm_shw_array_array_free(_IN_ dm_shw_data_value_t *data_value)
{

}

static void _dm_shw_array_struct_free(_IN_ dm_shw_data_value_t *data_value)
{
    int index = 0;
    dm_shw_data_t *data = NULL;

    dm_shw_data_value_complex_t *complex_struct = (dm_shw_data_value_complex_t *)data_value->value;

    if (complex_struct) {
        for (index = 0; index < complex_struct->size; index++) {
            data = (dm_shw_data_t *)complex_struct->value + index;
            _dm_shw_property_free(data);
        }
    }
}

static void _dm_shw_array_free(_IN_ dm_shw_data_value_t *data_value)
{
    dm_shw_data_value_complex_t *complex_array = data_value->value;

    /* Free Value */
    if (complex_array) {
        /* dm_log_err("complex_array->type: %d",complex_array->type); */
        if (g_iotx_data_type_mapping[complex_array->type].func_array_free != NULL) {
            g_iotx_data_type_mapping[complex_array->type].func_array_free(data_value);
        }
        if (complex_array->value) {
            DM_free(complex_array->value);
        }
        DM_free(complex_array);
        data_value->value = NULL;
    }
}

static void _dm_shw_struct_free(_IN_ dm_shw_data_value_t *data_value)
{
    int index = 0;
    dm_shw_data_t *property = NULL;
    dm_shw_data_value_complex_t *complex_array = data_value->value;

    /* Free Value */
    if (complex_array) {
        for (index = 0; index < complex_array->size; index++) {
            property = (dm_shw_data_t *)(complex_array->value) + index;
            _dm_shw_property_free(property);
        }
        if (complex_array->value) {
            DM_free(complex_array->value);
        }
        DM_free(complex_array);
        data_value->value = NULL;
    }
}

static void _dm_shw_data_free(dm_shw_data_value_t *data_value)
{
    if (g_iotx_data_type_mapping[data_value->type].func_free == NULL) {
        return;
    }
    g_iotx_data_type_mapping[data_value->type].func_free(data_value);
}

static void _dm_shw_property_free(_IN_ dm_shw_data_t *property)
{
    if (property->identifier) {
        DM_free(property->identifier);
    }
    _dm_shw_data_free(&property->data_value);
}

static void _dm_shw_properties_free(_IN_ dm_shw_data_t *properties, _IN_ int number)
{
    int index = 0;
    dm_shw_data_t *property = NULL;

    for (index = 0; index < number; index++) {
        property = properties + index;
        _dm_shw_property_free(property);
    }
}

static void _dm_shw_event_outputdata_free(_IN_ dm_shw_data_t *outputdata)
{
    if (outputdata->identifier) {
        DM_free(outputdata->identifier);
        outputdata->identifier = NULL;
    }
    _dm_shw_data_free(&outputdata->data_value);
}

static void _dm_shw_event_outputdatas_free(_IN_ dm_shw_data_t *outputdatas, _IN_ int number)
{
    int index = 0;
    dm_shw_data_t *outputdata = NULL;

    for (index = 0; index < number; index++) {
        outputdata = outputdatas + index;
        _dm_shw_event_outputdata_free(outputdata);
    }
}

static void _dm_shw_event_free(_IN_ dm_shw_event_t *event)
{
    if (event->identifier) {
        DM_free(event->identifier);
        event->identifier = NULL;
    }
    if (event->output_datas) {
        _dm_shw_event_outputdatas_free(event->output_datas, event->output_data_number);
        DM_free(event->output_datas);
        event->output_datas = NULL;
    }
}

static void _dm_shw_events_free(_IN_ dm_shw_event_t *events, _IN_ int number)
{
    int index = 0;
    dm_shw_event_t *event = NULL;

    for (index = 0; index < number; index++) {
        event = events + index;
        _dm_shw_event_free(event);
    }
}

static void _dm_shw_service_outputdata_free(_IN_ dm_shw_data_t *outputdata)
{
    if (outputdata->identifier) {
        DM_free(outputdata->identifier);
        outputdata->identifier = NULL;
    }
    _dm_shw_data_free(&outputdata->data_value);
}

static void _dm_shw_service_outputdatas_free(_IN_ dm_shw_data_t *outputdatas, _IN_ int number)
{
    int index = 0;
    dm_shw_data_t *outputdata = NULL;

    for (index = 0; index < number; index++) {
        outputdata = outputdatas + index;
        _dm_shw_service_outputdata_free(outputdata);
    }
}

static void _dm_shw_service_inputdata_free(_IN_ dm_shw_data_t *inputdata)
{
    if (inputdata->identifier) {
        DM_free(inputdata->identifier);
        inputdata->identifier = NULL;
    }
    _dm_shw_data_free(&inputdata->data_value);
}

static void _dm_shw_service_inputdatas_free(_IN_ dm_shw_data_t *inputdatas, _IN_ int number)
{
    int index = 0;
    dm_shw_data_t *inputdata = NULL;

    for (index = 0; index < number; index++) {
        inputdata = inputdatas + index;
        _dm_shw_service_inputdata_free(inputdata);
    }
}

static void _dm_shw_service_free(_IN_ dm_shw_service_t *service)
{
    if (service->identifier) {
        DM_free(service->identifier);
        service->identifier = NULL;
    }
    if (service->output_datas) {
        _dm_shw_service_outputdatas_free(service->output_datas, service->output_data_number);
        DM_free(service->output_datas);
        service->output_datas = NULL;
    }
    if (service->input_datas) {
        _dm_shw_service_inputdatas_free(service->input_datas, service->input_data_number);
        DM_free(service->input_datas);
        service->input_datas = NULL;
    }
}

static void _dm_shw_services_free(_IN_ dm_shw_service_t *services, _IN_ int number)
{
    int index = 0;
    dm_shw_service_t *service = NULL;

    for (index = 0; index < number; index++) {
        service = services + index;
        _dm_shw_service_free(service);
    }
}

void dm_shw_destroy(_IN_ dm_shw_t **shadow)
{
    if (shadow == NULL || *shadow == NULL) {
        return;
    }

    /* Free Properties */
    if ((*shadow)->properties) {
        _dm_shw_properties_free((*shadow)->properties, (*shadow)->property_number);
        DM_free((*shadow)->properties);
        (*shadow)->properties = NULL;
    }

    /* Free Events */
    if ((*shadow)->events) {
        _dm_shw_events_free((*shadow)->events, (*shadow)->event_number);
        DM_free((*shadow)->events);
        (*shadow)->events = NULL;
    }

    /* Free Services */
    if ((*shadow)->services) {
        _dm_shw_services_free((*shadow)->services, (*shadow)->service_number);
        DM_free((*shadow)->services);
        (*shadow)->services = NULL;
    }

    DM_free(*shadow);
    *shadow = NULL;
}

#if 0
static void _dm_shw_int_print(_IN_ dm_shw_data_value_t *data_value)
{
    dm_log_debug("TSL Property Value                 : %d", data_value->value_int);
}

static void _dm_shw_float_print(_IN_ dm_shw_data_value_t *data_value)
{
    dm_log_debug("TSL Property Value                 : %f", data_value->value_float);
}

static void _dm_shw_double_print(_IN_ dm_shw_data_value_t *data_value)
{
    dm_log_debug("TSL Property Value                 : %f", data_value->value_double);
}

static void _dm_shw_text_print(_IN_ dm_shw_data_value_t *data_value)
{
    dm_log_debug("TSL Property Value       : %s",
                 ((char *)data_value->value == NULL) ? ("NULL") : ((char *)data_value->value));
}

static void _dm_shw_enum_print(_IN_ dm_shw_data_value_t *data_value)
{
    dm_log_debug("TSL Property Value         : %d", data_value->value_int);
}

static void _dm_shw_date_print(_IN_ dm_shw_data_value_t *data_value)
{
    dm_log_debug("TSL Property Value         : %s",
                 ((char *)data_value->value == NULL) ? ("NULL") : ((char *)data_value->value));
}

static void _dm_shw_bool_print(_IN_ dm_shw_data_value_t *data_value)
{
    dm_log_debug("TSL Property Value         : %d", data_value->value_int);
}

static void _dm_shw_array_print(_IN_ dm_shw_data_value_t *data_value)
{
    int index = 0;
    dm_shw_data_value_complex_t *complex_array = data_value->value;

    if (complex_array == NULL) {
        dm_log_debug("TSL Property Complex Array Not Exist");
        return;
    }

    dm_log_debug("TSL Property Size: %d", complex_array->size);
    dm_log_debug("TSL Property Type: %s", g_iotx_data_type_mapping[complex_array->type].name);

    for (index = 0; index < complex_array->size; index++) {
        dm_log_debug("TSL Property Array Value Index    : %d", index);
        switch (complex_array->type) {
            case DM_SHW_DATA_TYPE_INT: {
                dm_log_debug("TSL Property Value: %d", *((int *)(complex_array->value) + index));
            }
            break;
            case DM_SHW_DATA_TYPE_FLOAT: {
                dm_log_debug("TSL Property Value: %f", *((float *)(complex_array->value) + index));
            }
            break;
            case DM_SHW_DATA_TYPE_DOUBLE: {
                dm_log_debug("TSL Property Value: %f", *((double *)(complex_array->value) + index));
            }
            break;
            case DM_SHW_DATA_TYPE_TEXT: {
                dm_log_debug("TSL Property Value: %s",
                             (*((char **)(complex_array->value) + index) == NULL) ? "NULL" : * ((char **)(data_value->value) + index));
            }
            break;
            default:
                dm_log_err("Execute Should Not Be Here!");
                break;
        }
    }
}

static void _dm_shw_struct_print(_IN_ dm_shw_data_value_t *data_value)
{
    int index = 0;
    dm_shw_data_t *property = NULL;
    dm_shw_data_value_complex_t *complex_array = data_value->value;

    dm_log_debug("TSL Property Struct Size: %d", complex_array->size);
    if (complex_array->size == 0) {
        return;
    }

    for (index = 0; index < complex_array->size; index++) {
        dm_log_debug("TSL Property Struct Index: %d", index);
        property = (dm_shw_data_t *)complex_array->value + index;
        _dm_shw_property_print(property);
        dm_log_debug("\n");
    }
}

static void _dm_shw_data_print(_IN_ dm_shw_data_value_t *data_value)
{
    dm_log_debug("TSL Property Type: %s", g_iotx_data_type_mapping[data_value->type].name);

    if (g_iotx_data_type_mapping[data_value->type].func_print == NULL) {
        return;
    }
    g_iotx_data_type_mapping[data_value->type].func_print(data_value);
}

static void _dm_shw_property_print(_IN_ dm_shw_data_t *property)
{
    dm_log_debug("TSL Property Identifier : %s", (property->identifier == NULL) ? ("NULL") : (property->identifier));
    _dm_shw_data_print(&property->data_value);
}

static void _dm_shw_properties_print(_IN_ dm_shw_data_t *properties, _IN_ int number)
{
    int index = 0;
    dm_shw_data_t *property = NULL;

    if (!properties) {
        dm_log_debug("TSL Propertie Not Exist");
    }

    dm_log_debug("TSL Property Number: %d", number);
    for (index = 0; index < number; index++) {
        property = properties + index;
        dm_log_debug("TSL Property Index      : %d", index);
        _dm_shw_property_print(property);
        dm_log_debug("\n");
    }
}

static void _dm_shw_event_outputdata_print(_IN_ dm_shw_data_t *outputdata)
{
    dm_log_debug("TSL Event Output Data Identifier : %s",
                 (outputdata->identifier == NULL) ? ("NULL") : (outputdata->identifier));
    _dm_shw_data_print(&outputdata->data_value);
}

static void _dm_shw_event_outputdatas_print(_IN_ dm_shw_data_t *outputdatas, _IN_ int number)
{
    int index = 0;
    dm_shw_data_t *outputdata = NULL;

    if (!outputdatas) {
        dm_log_debug("TSL Event Output Data Not Exist");
    }

    dm_log_debug("TSL Event Output Data Number: %d", number);
    for (index = 0; index < number; index++) {
        outputdata = outputdatas + index;
        dm_log_debug("TSL Event Output Data Index: %d", index);
        _dm_shw_event_outputdata_print(outputdata);
        dm_log_debug("\n");
    }
}

static void _dm_shw_event_print(_IN_ dm_shw_event_t *event)
{
    dm_log_debug("TSL Event Identifier : %s", (event->identifier == NULL) ? ("NULL") : (event->identifier));
    _dm_shw_event_outputdatas_print(event->output_datas, event->output_data_number);
}

static void _dm_shw_events_print(_IN_ dm_shw_event_t *events, _IN_ int number)
{
    int index = 0;
    dm_shw_event_t *event = NULL;
    if (!events) {
        dm_log_debug("TSL Events: NULL");
    }

    dm_log_debug("TSL Event Number: %d", number);
    for (index = 0; index < number; index++) {
        event = events + index;
        dm_log_debug("TSL Event Index      : %d", index);
        _dm_shw_event_print(event);
        dm_log_debug("\n");
    }
}

static void _dm_shw_service_outputdata_print(_IN_ dm_shw_data_t *outputdata)
{
    dm_log_debug("TSL Service Output Data Identifier : %s",
                 (outputdata->identifier == NULL) ? ("NULL") : (outputdata->identifier));
    _dm_shw_data_print(&outputdata->data_value);
}

static void _dm_shw_service_outputdatas_print(_IN_ dm_shw_data_t *outputdatas, _IN_ int number)
{
    int index = 0;
    dm_shw_data_t *outputdata = NULL;

    if (!outputdatas) {
        dm_log_debug("TSL Service Output Data Not Exist");
    }

    dm_log_debug("TSL Service Output Data Number: %d", number);
    for (index = 0; index < number; index++) {
        outputdata = outputdatas + index;
        dm_log_debug("TSL Service Output Data Index: %d", index);
        _dm_shw_service_outputdata_print(outputdata);
        dm_log_debug("\n");
    }
}

static void _dm_shw_service_inputdata_get_print(_IN_ dm_shw_data_t *inputdata)
{
    dm_log_debug("TSL Service Input Data Identifier : %s",
                 (inputdata->identifier == NULL) ? ("NULL") : (inputdata->identifier));
}

static void _dm_shw_service_inputdata_print(_IN_ dm_shw_data_t *inputdata)
{
    dm_log_debug("TSL Service Input Data Identifier : %s",
                 (inputdata->identifier == NULL) ? ("NULL") : (inputdata->identifier));
    _dm_shw_data_print(&inputdata->data_value);
}

static void _dm_shw_service_inputdatas_print(_IN_ dm_shw_service_t *service, _IN_ int number)
{
    int index = 0;
    dm_shw_data_t *inputdata = NULL;

    if (!service->input_datas) {
        dm_log_debug("TSL Service Output Data Not Exist");
    }

    dm_log_debug("TSL Service Output Data Number: %d", number);
    for (index = 0; index < number; index++) {
        inputdata = service->input_datas + index;
        dm_log_debug("TSL Service Output Data Index: %d", index);
        /* There Is A God-Damned Special Case For thing.service.property.get(method)/get(identifier) */
        if (strcmp(service->identifier, DM_SHW_SPECIAL_SERVICE_GET_IDENTIFIER) == 0) {
            _dm_shw_service_inputdata_get_print(inputdata);
        } else {
            _dm_shw_service_inputdata_print(inputdata);
        }
        dm_log_debug("\n");
    }
}

static void _dm_shw_service_print(_IN_ dm_shw_service_t *service)
{
    dm_log_debug("TSL Service Identifier : %s", (service->identifier == NULL) ? ("NULL") : (service->identifier));
    _dm_shw_service_outputdatas_print(service->output_datas, service->output_data_number);
    _dm_shw_service_inputdatas_print(service, service->input_data_number);
}

static void _dm_shw_services_print(_IN_ dm_shw_service_t *services, _IN_ int number)
{
    int index = 0;
    dm_shw_service_t *service = NULL;
    if (!services) {
        dm_log_debug("TSL Serivces: NULL");
    }

    dm_log_debug("TSL Service Number: %d", number);
    for (index = 0; index < number; index++) {
        service = services + index;
        dm_log_debug("TSL Service Index: %d", index);
        _dm_shw_service_print(service);
        dm_log_debug("\n");
    }
}

void dm_shw_print(_IN_ dm_shw_t *shadow)
{
    dm_log_debug("TSL Profile, Product Key: %s, Device Name: %s", shadow->profile.product_key, shadow->profile.device_name);
    _dm_shw_properties_print(shadow->properties, shadow->property_number);
    _dm_shw_events_print(shadow->events, shadow->event_number);
    _dm_shw_services_print(shadow->services, shadow->service_number);
}
#endif
#endif