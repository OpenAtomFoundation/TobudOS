/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "iotx_dm_internal.h"

#ifdef DEPRECATED_LINKKIT

typedef int (*dm_shw_data_parse)(_IN_ dm_shw_data_value_t *data_value, _IN_ lite_cjson_t *root);
typedef int (*dm_shw_array_parse)(_IN_ dm_shw_data_value_t *data_value, _IN_ lite_cjson_t *root);

typedef struct {
    dm_shw_data_type_e type;
    const char *name;
    dm_shw_data_parse  func_parse;
    dm_shw_array_parse func_array_parse;
} dm_tsl_alink_mapping_t;

/* Data Parse */
static int _dm_shw_int_parse(_IN_ dm_shw_data_value_t *data_value, _IN_ lite_cjson_t *root);
static int _dm_shw_float_parse(_IN_ dm_shw_data_value_t *data_value, _IN_ lite_cjson_t *root);
static int _dm_shw_double_parse(_IN_ dm_shw_data_value_t *data_value, _IN_ lite_cjson_t *root);
static int _dm_shw_text_parse(_IN_ dm_shw_data_value_t *data_value, _IN_ lite_cjson_t *root);
static int _dm_shw_enum_parse(_IN_ dm_shw_data_value_t *data_value, _IN_ lite_cjson_t *root);
static int _dm_shw_date_parse(_IN_ dm_shw_data_value_t *data_value, _IN_ lite_cjson_t *root);
static int _dm_shw_bool_parse(_IN_ dm_shw_data_value_t *data_value, _IN_ lite_cjson_t *root);
static int _dm_shw_array_parse(_IN_ dm_shw_data_value_t *data_value, _IN_ lite_cjson_t *root);
static int _dm_shw_struct_parse(_IN_ dm_shw_data_value_t *data_value, _IN_ lite_cjson_t *root);
static int _dm_shw_property_parse(_IN_ dm_shw_data_t *property, _IN_ lite_cjson_t *root);

/* Array Data Parse */
static int _dm_shw_array_int_parse(_IN_ dm_shw_data_value_t *data_value, _IN_ lite_cjson_t *root);
static int _dm_shw_array_float_parse(_IN_ dm_shw_data_value_t *data_value, _IN_ lite_cjson_t *root);
static int _dm_shw_array_double_parse(_IN_ dm_shw_data_value_t *data_value, _IN_ lite_cjson_t *root);
static int _dm_shw_array_text_parse(_IN_ dm_shw_data_value_t *data_value, _IN_ lite_cjson_t *root);
static int _dm_shw_array_enum_parse(_IN_ dm_shw_data_value_t *data_value, _IN_ lite_cjson_t *root);
static int _dm_shw_array_date_parse(_IN_ dm_shw_data_value_t *data_value, _IN_ lite_cjson_t *root);
static int _dm_shw_array_bool_parse(_IN_ dm_shw_data_value_t *data_value, _IN_ lite_cjson_t *root);
static int _dm_shw_array_array_parse(_IN_ dm_shw_data_value_t *data_value, _IN_ lite_cjson_t *root);
static int _dm_shw_array_struct_parse(_IN_ dm_shw_data_value_t *data_value, _IN_ lite_cjson_t *root);

static dm_tsl_alink_mapping_t g_dm_tsl_alink_mapping[] = {
    {DM_SHW_DATA_TYPE_NONE,     "none",     NULL,                      NULL                          },
    {DM_SHW_DATA_TYPE_INT,      "int",      _dm_shw_int_parse,      _dm_shw_array_int_parse      },
    {DM_SHW_DATA_TYPE_FLOAT,    "float",    _dm_shw_float_parse,    _dm_shw_array_float_parse    },
    {DM_SHW_DATA_TYPE_DOUBLE,   "double",   _dm_shw_double_parse,   _dm_shw_array_double_parse   },
    {DM_SHW_DATA_TYPE_TEXT,     "text",     _dm_shw_text_parse,     _dm_shw_array_text_parse     },
    {DM_SHW_DATA_TYPE_ENUM,     "enum",     _dm_shw_enum_parse,     _dm_shw_array_enum_parse     },
    {DM_SHW_DATA_TYPE_DATE,     "date",     _dm_shw_date_parse,     _dm_shw_array_date_parse     },
    {DM_SHW_DATA_TYPE_BOOL,     "bool",     _dm_shw_bool_parse,     _dm_shw_array_bool_parse     },
    {DM_SHW_DATA_TYPE_ARRAY,    "array",    _dm_shw_array_parse,    _dm_shw_array_array_parse    },
    {DM_SHW_DATA_TYPE_STRUCT,   "struct",   _dm_shw_struct_parse,   _dm_shw_array_struct_parse   }
};

static int _dm_shw_get_type(_IN_ const char *name, _IN_ int name_len, _OU_ dm_shw_data_type_e *type)
{
    int index = 0;

    if (name == NULL || name_len <= 0 || type == NULL) {
        return DM_INVALID_PARAMETER;
    }

    for (index = 0; index < sizeof(g_dm_tsl_alink_mapping) / sizeof(dm_tsl_alink_mapping_t); index++) {
        if (strlen(g_dm_tsl_alink_mapping[index].name) == name_len &&
            memcmp(g_dm_tsl_alink_mapping[index].name, name, name_len) == 0) {
            *type = g_dm_tsl_alink_mapping[index].type;
            return SUCCESS_RETURN;
        }
    }

    return FAIL_RETURN;
}

static int _dm_shw_int_parse(_IN_ dm_shw_data_value_t *data_value, _IN_ lite_cjson_t *root)
{
    return SUCCESS_RETURN;
}

static int _dm_shw_float_parse(_IN_ dm_shw_data_value_t *data_value, _IN_ lite_cjson_t *root)
{
    return SUCCESS_RETURN;
}

static int _dm_shw_double_parse(_IN_ dm_shw_data_value_t *data_value, _IN_ lite_cjson_t *root)
{
    return SUCCESS_RETURN;
}

static int _dm_shw_text_parse(_IN_ dm_shw_data_value_t *data_value, _IN_ lite_cjson_t *root)
{
    return SUCCESS_RETURN;
}

static int _dm_shw_enum_parse(_IN_ dm_shw_data_value_t *data_value, _IN_ lite_cjson_t *root)
{
    return SUCCESS_RETURN;
}

static int _dm_shw_date_parse(_IN_ dm_shw_data_value_t *data_value, _IN_ lite_cjson_t *root)
{
    return SUCCESS_RETURN;
}

static int _dm_shw_bool_parse(_IN_ dm_shw_data_value_t *data_value, _IN_ lite_cjson_t *root)
{
    return SUCCESS_RETURN;
}

static int _dm_shw_array_int_parse(_IN_ dm_shw_data_value_t *data_value, _IN_ lite_cjson_t *root)
{
    dm_shw_data_value_complex_t *complex_array = (dm_shw_data_value_complex_t *)data_value->value;

    complex_array->value = DM_malloc((complex_array->size) * (sizeof(int)));
    if (complex_array->value == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(complex_array->value, 0, (complex_array->size) * (sizeof(int)));

    /* Just For Test */
#ifdef IOTX_DM_TSL_DEVELOP_TEST
    int index = 0;
    for (index = 0; index < complex_array->size; index++) {
        *((int *)(complex_array->value) + index) = index + 1;
    }
#endif

    return SUCCESS_RETURN;
}

static int _dm_shw_array_float_parse(_IN_ dm_shw_data_value_t *data_value, _IN_ lite_cjson_t *root)
{
    dm_shw_data_value_complex_t *complex_array = (dm_shw_data_value_complex_t *)data_value->value;

    complex_array->value = DM_malloc((complex_array->size) * (sizeof(float)));
    if (complex_array->value == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(complex_array->value, 0, (complex_array->size) * (sizeof(float)));

    /* Just For Test */
#ifdef IOTX_DM_TSL_DEVELOP_TEST
    int index = 0;
    for (index = 0; index < complex_array->size; index++) {
        *((float *)(complex_array->value) + index) = (float)index + 0.2;
    }
#endif

    return SUCCESS_RETURN;
}

static int _dm_shw_array_double_parse(_IN_ dm_shw_data_value_t *data_value, _IN_ lite_cjson_t *root)
{
    dm_shw_data_value_complex_t *complex_array = (dm_shw_data_value_complex_t *)data_value->value;

    complex_array->value = DM_malloc((complex_array->size) * (sizeof(double)));
    if (complex_array->value == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(complex_array->value, 0, (complex_array->size) * (sizeof(double)));

    /* Just For Test */
#ifdef IOTX_DM_TSL_DEVELOP_TEST
    int index = 0;
    for (index = 0; index < complex_array->size; index++) {
        *((double *)(complex_array->value) + index) = (double)index + 0.2;
    }
#endif

    return SUCCESS_RETURN;
}

static int _dm_shw_array_text_parse(_IN_ dm_shw_data_value_t *data_value, _IN_ lite_cjson_t *root)
{
    dm_shw_data_value_complex_t *complex_array = (dm_shw_data_value_complex_t *)data_value->value;

    complex_array->value = DM_malloc((complex_array->size) * (sizeof(char *)));
    if (complex_array->value == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(complex_array->value, 0, (complex_array->size) * (sizeof(char *)));

#ifdef IOTX_DM_TSL_DEVELOP_TEST
    int index = 0;
    char temp[10] = {0};
    for (index = 0; index < complex_array->size; index++) {
        memset(temp, 0, sizeof(temp));
        HAL_Snprintf(temp, sizeof(temp), "%d", index + 1);
        *((char **)(complex_array->value) + index) = DM_malloc(strlen(temp) + 1);
        if (*((char **)(complex_array->value) + index) != NULL) {
            memset(*((char **)(complex_array->value) + index), 0, strlen(temp) + 1);
            memcpy(*((char **)(complex_array->value) + index), temp, strlen(temp));
        }
    }
#endif
    return SUCCESS_RETURN;
}

static int _dm_shw_array_enum_parse(_IN_ dm_shw_data_value_t *data_value, _IN_ lite_cjson_t *root)
{
    dm_shw_data_value_complex_t *complex_array = (dm_shw_data_value_complex_t *)data_value->value;

    complex_array->value = DM_malloc((complex_array->size) * (sizeof(int)));
    if (complex_array->value == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(complex_array->value, 0, (complex_array->size) * (sizeof(int)));

    return SUCCESS_RETURN;
}

static int _dm_shw_array_date_parse(_IN_ dm_shw_data_value_t *data_value, _IN_ lite_cjson_t *root)
{
    dm_shw_data_value_complex_t *complex_array = (dm_shw_data_value_complex_t *)data_value->value;

    complex_array->value = DM_malloc((complex_array->size) * (sizeof(char *)));
    if (complex_array->value == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(complex_array->value, 0, (complex_array->size) * (sizeof(char *)));

    return SUCCESS_RETURN;
}

static int _dm_shw_array_bool_parse(_IN_ dm_shw_data_value_t *data_value, _IN_ lite_cjson_t *root)
{
    dm_shw_data_value_complex_t *complex_array = (dm_shw_data_value_complex_t *)data_value->value;

    complex_array->value = DM_malloc((complex_array->size) * (sizeof(int)));
    if (complex_array->value == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(complex_array->value, 0, (complex_array->size) * (sizeof(int)));

    return SUCCESS_RETURN;
}

static int _dm_shw_array_array_parse(_IN_ dm_shw_data_value_t *data_value, _IN_ lite_cjson_t *root)
{
#if 0
    int res = 0;
    char size_str[DM_UTILS_UINT32_STRLEN] = {0};
    lite_cjson_t lite_item, lite_type, lite_specs;
    dm_shw_data_value_complex_t *complex_array = (dm_shw_data_value_complex_t *)data_value->value;
    dm_shw_data_value_t *data_value_next_level;
    dm_shw_data_value_complex_t *complex_array_next_level = NULL;

    if (!lite_cjson_is_object(root)) {
        return DM_INVALID_PARAMETER;
    }

    /* Allocate Memory For Next Level Data Value And Next Level Complex Array */
    data_value_next_level = DM_malloc(sizeof(dm_shw_data_value_t));
    if (data_value_next_level == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(data_value_next_level, 0, sizeof(dm_shw_data_value_t));
    data_value_next_level->type = DM_SHW_DATA_TYPE_ARRAY;

    complex_array_next_level = DM_malloc(sizeof(dm_shw_data_value_complex_t));
    if (complex_array_next_level == NULL) {
        DM_free(data_value_next_level);
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(complex_array_next_level, 0, sizeof(dm_shw_data_value_complex_t));
    complex_array->value = (void *)data_value_next_level;
    data_value_next_level->value = complex_array_next_level;

    /* Parse Size (Mandatory) */
    memset(&lite_item, 0, sizeof(lite_cjson_t));
    res = lite_cjson_object_item(root, DM_SHW_KEY_SIZE, strlen(DM_SHW_KEY_SIZE), &lite_item);
    if (res != SUCCESS_RETURN && !lite_cjson_is_string(&lite_item)) {
        return DM_JSON_PARSE_FAILED;
    }
    if (lite_item.value_length > DM_UTILS_UINT32_STRLEN) {
        return FAIL_RETURN;
    }
    memcpy(size_str, lite_item.value, lite_item.value_length);
    complex_array_next_level->size = atoi(size_str);

    dm_log_debug("TSL Property Array Array Size: %d", complex_array_next_level->size);

    /* Parse Item And Type (Mandatory) */
    memset(&lite_item, 0, sizeof(lite_cjson_t));
    res = lite_cjson_object_item(root, DM_SHW_KEY_ITEM, strlen(DM_SHW_KEY_ITEM), &lite_item);
    if (res != SUCCESS_RETURN && !lite_cjson_is_object(&lite_item)) {
        return DM_JSON_PARSE_FAILED;
    }
    memset(&lite_type, 0, sizeof(lite_cjson_t));
    res = lite_cjson_object_item(&lite_item, DM_SHW_KEY_TYPE, strlen(DM_SHW_KEY_TYPE), &lite_type);
    if (res != SUCCESS_RETURN && !lite_cjson_is_string(&lite_type)) {
        return DM_JSON_PARSE_FAILED;
    }
    res = _dm_shw_get_type(lite_type.value, lite_type.value_length, &complex_array_next_level->type);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Parse Specs (Optional) */
    memset(&lite_specs, 0, sizeof(lite_cjson_t));
    res = lite_cjson_object_item(&lite_item, DM_SHW_KEY_SPECS, strlen(DM_SHW_KEY_SPECS), &lite_specs);
    if ((complex_array_next_level->type == DM_SHW_DATA_TYPE_ARRAY
         || complex_array_next_level->type == DM_SHW_DATA_TYPE_STRUCT) &&
        (res != SUCCESS_RETURN)) {
        return DM_JSON_PARSE_FAILED;
    }

    if (g_dm_tsl_alink_mapping[complex_array_next_level->type].func_array_parse == NULL) {
        return FAIL_RETURN;
    }
    dm_log_debug("TSL Property Specs Type: %s", g_dm_tsl_alink_mapping[complex_array_next_level->type].name);

    /* Parse Array Type */
    res = g_dm_tsl_alink_mapping[complex_array->type].func_array_parse(data_value_next_level, &lite_specs);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }
#endif
    return SUCCESS_RETURN;
}

static int _dm_shw_array_struct_parse(_IN_ dm_shw_data_value_t *data_value, _IN_ lite_cjson_t *root)
{
    int res = 0, index = 0;
    dm_shw_data_value_complex_t *complex_array = (dm_shw_data_value_complex_t *)data_value->value;
    dm_shw_data_t *data = NULL;

    if (!lite_cjson_is_array(root) || root->size <= 0) {
        return DM_INVALID_PARAMETER;
    }

    dm_log_debug("Array Struct Size: %d", complex_array->size);
    complex_array->value = DM_malloc((complex_array->size) * (sizeof(dm_shw_data_t)));
    if (complex_array->value == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(complex_array->value, 0, (complex_array->size) * (sizeof(dm_shw_data_t)));

    dm_log_debug("Array Struct Spec Size: %d", root->size);
    for (index = 0; index < complex_array->size; index++) {
        data = (dm_shw_data_t *)complex_array->value + index;

        data->data_value.type = DM_SHW_DATA_TYPE_STRUCT;

        res = _dm_shw_struct_parse(&data->data_value, root);
        if (res != SUCCESS_RETURN) {
            continue;
        }
    }

    return SUCCESS_RETURN;
}

static int _dm_shw_array_parse(_IN_ dm_shw_data_value_t *data_value, _IN_ lite_cjson_t *root)
{
    int res = 0;
    char size_str[DM_UTILS_UINT32_STRLEN] = {0};
    lite_cjson_t lite_item, lite_type, lite_specs;
    dm_shw_data_value_complex_t *complex_array = NULL;

    /* dm_log_debug("DM_SHW_DATA_TYPE_ARRAY"); */

    if (root == NULL || !lite_cjson_is_object(root)) {
        return DM_INVALID_PARAMETER;
    }

    /* Allocate Memory For Data Type Specs */
    complex_array = DM_malloc(sizeof(dm_shw_data_value_complex_t));
    if (complex_array == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(complex_array, 0, sizeof(dm_shw_data_value_complex_t));
    data_value->value = (void *)complex_array;

    /* Parse Size (Mandatory) */
    memset(&lite_item, 0, sizeof(lite_cjson_t));
    res = lite_cjson_object_item(root, DM_SHW_KEY_SIZE, strlen(DM_SHW_KEY_SIZE), &lite_item);
    if (res != SUCCESS_RETURN && !lite_cjson_is_string(&lite_item)) {
        return DM_JSON_PARSE_FAILED;
    }
    if (lite_item.value_length > DM_UTILS_UINT32_STRLEN) {
        return FAIL_RETURN;
    }
    memcpy(size_str, lite_item.value, lite_item.value_length);
    complex_array->size = atoi(size_str);

    /* dm_log_debug("TSL Property Array Size: %d",complex_array->size); */

    /* Parse Item And Type (Mandatory) */
    memset(&lite_item, 0, sizeof(lite_cjson_t));
    res = lite_cjson_object_item(root, DM_SHW_KEY_ITEM, strlen(DM_SHW_KEY_ITEM), &lite_item);
    if (res != SUCCESS_RETURN && !lite_cjson_is_object(&lite_item)) {
        return DM_JSON_PARSE_FAILED;
    }
    memset(&lite_type, 0, sizeof(lite_cjson_t));
    res = lite_cjson_object_item(&lite_item, DM_SHW_KEY_TYPE, strlen(DM_SHW_KEY_TYPE), &lite_type);
    if (res != SUCCESS_RETURN && !lite_cjson_is_string(&lite_type)) {
        return DM_JSON_PARSE_FAILED;
    }
    res = _dm_shw_get_type(lite_type.value, lite_type.value_length, &complex_array->type);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }
    /* dm_log_debug("TSL Property Array Type: %d",complex_array->type); */

    /* Parse Specs (Optional) */
    memset(&lite_specs, 0, sizeof(lite_cjson_t));
    res = lite_cjson_object_item(&lite_item, DM_SHW_KEY_SPECS, strlen(DM_SHW_KEY_SPECS), &lite_specs);
    if ((complex_array->type == DM_SHW_DATA_TYPE_ARRAY || complex_array->type == DM_SHW_DATA_TYPE_STRUCT) &&
        (res != SUCCESS_RETURN)) {
        return DM_JSON_PARSE_FAILED;
    }

    if (g_dm_tsl_alink_mapping[complex_array->type].func_array_parse == NULL) {
        return FAIL_RETURN;
    }
    /* dm_log_debug("TSL Property Specs Type: %s",g_dm_tsl_alink_mapping[complex_array->type].name); */

    /* Parse Array Type */
    res = g_dm_tsl_alink_mapping[complex_array->type].func_array_parse(data_value, &lite_specs);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

static int _dm_shw_struct_parse(_IN_ dm_shw_data_value_t *data_value, _IN_ lite_cjson_t *root)
{
    int res = 0, index = 0;
    lite_cjson_t lite_item;
    dm_shw_data_t *property = NULL;
    dm_shw_data_value_complex_t *complex_struct = NULL;

    /* dm_log_debug("DM_SHW_DATA_TYPE_STRUCT"); */

    if (root == NULL || !lite_cjson_is_array(root) || root->size == 0) {
        return DM_INVALID_PARAMETER;
    }

    /* dm_log_debug("TSL Property Struct Size: %d",root->size); */

    /* Allocate Memory For Data Type Specs */
    complex_struct = DM_malloc(sizeof(dm_shw_data_value_complex_t));
    if (complex_struct == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(complex_struct, 0, sizeof(dm_shw_data_value_complex_t));
    data_value->value = (void *)complex_struct;

    complex_struct->size = root->size;

    /* Allocate Memory For Multi Identifier */
    complex_struct->value = DM_malloc((complex_struct->size) * (sizeof(dm_shw_data_t)));
    if (complex_struct->value == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(complex_struct->value, 0, (complex_struct->size) * (sizeof(dm_shw_data_t)));

    for (index = 0; index < complex_struct->size; index++) {
        memset(&lite_item, 0, sizeof(lite_cjson_t));
        property = (dm_shw_data_t *)complex_struct->value + index;
        /* dm_log_debug("TSL Property Struct Index: %d",index); */

        res = lite_cjson_array_item(root, index, &lite_item);
        if (res != SUCCESS_RETURN || !lite_cjson_is_object(&lite_item)) {
            return DM_JSON_PARSE_FAILED;
        }
        /* dm_log_debug("TSL Property Struct Property: %.*s",lite_item.value_length,lite_item.value); */
        res = _dm_shw_property_parse(property, &lite_item);
        if (res != SUCCESS_RETURN) {
            return FAIL_RETURN;
        }
    }

    return SUCCESS_RETURN;
}

static int _dm_shw_data_parse(_IN_ dm_shw_data_value_t *data_value, _IN_ lite_cjson_t *root)
{
    int res = 0;
    lite_cjson_t lite_item;

    memset(data_value, 0, sizeof(dm_shw_data_value_t));

    /* Parse Type */
    memset(&lite_item, 0, sizeof(lite_cjson_t));
    res = lite_cjson_object_item(root, DM_SHW_KEY_TYPE, strlen(DM_SHW_KEY_TYPE), &lite_item);
    if (res != SUCCESS_RETURN || !lite_cjson_is_string(&lite_item)) {
        return DM_JSON_PARSE_FAILED;
    }
    dm_log_debug("TSL Data Type: %.*s", lite_item.value_length, lite_item.value);
    res = _dm_shw_get_type(lite_item.value, lite_item.value_length, &data_value->type);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Parse Specs */
    memset(&lite_item, 0, sizeof(lite_cjson_t));
    res = lite_cjson_object_item(root, DM_SHW_KEY_SPECS, strlen(DM_SHW_KEY_SPECS), &lite_item);
    if (res == SUCCESS_RETURN) {
        /* dm_log_debug("TSL Data Specs: %.*s",lite_item.value_length,lite_item.value); */
    }

    /* Parse Type And Value */
    if (g_dm_tsl_alink_mapping[data_value->type].func_parse == NULL) {
        return FAIL_RETURN;
    }
    res = g_dm_tsl_alink_mapping[data_value->type].func_parse(data_value, &lite_item);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

static int _dm_shw_property_parse(_IN_ dm_shw_data_t *property, _IN_ lite_cjson_t *root)
{
    int res = 0;
    lite_cjson_t lite_item;

    /* Parse Identifier (Mandatory) */
    memset(&lite_item, 0, sizeof(lite_cjson_t));
    res = lite_cjson_object_item(root, DM_SHW_KEY_IDENTIFIER, strlen(DM_SHW_KEY_IDENTIFIER), &lite_item);
    if (res != SUCCESS_RETURN || !lite_cjson_is_string(&lite_item)) {
        return DM_JSON_PARSE_FAILED;
    }
    res = dm_utils_copy(lite_item.value, lite_item.value_length, (void **)(&property->identifier),
                        lite_item.value_length + 1);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }
    dm_log_debug("Identifier: %s", property->identifier);

    /* Parse DataType */
    memset(&lite_item, 0, sizeof(lite_cjson_t));
    res = lite_cjson_object_item(root, DM_SHW_KEY_DATATYPE, strlen(DM_SHW_KEY_DATATYPE), &lite_item);
    if (res != SUCCESS_RETURN || !lite_cjson_is_object(&lite_item)) {
        return DM_JSON_PARSE_FAILED;
    }
    /* dm_log_debug("TSL Property Data Type: %.*s",lite_item.value_length,lite_item.value); */
    res = _dm_shw_data_parse(&property->data_value, &lite_item);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

static int _dm_shw_properties_parse(_IN_ dm_shw_t *shadow, _IN_ lite_cjson_t *root)
{
    int res = 0, index = 0;
    lite_cjson_t lite_properties, lite_property;

    memset(&lite_properties, 0, sizeof(lite_cjson_t));
    res = lite_cjson_object_item(root, DM_SHW_KEY_PROPERTIES, strlen(DM_SHW_KEY_PROPERTIES), &lite_properties);
    if (res == SUCCESS_RETURN) {
        if (!lite_cjson_is_array(&lite_properties)) {
            return DM_JSON_PARSE_FAILED;
        }
    } else {
        return SUCCESS_RETURN;
    }

    dm_log_debug("Number: %d", lite_properties.size);
    if (lite_properties.size == 0) {
        return SUCCESS_RETURN;
    }

    /* Allocate Memory For TSL Properties Struct */
    shadow->property_number = lite_properties.size;
    shadow->properties = DM_malloc(sizeof(dm_shw_data_t) * (lite_properties.size));
    if (shadow->properties == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(shadow->properties, 0, sizeof(dm_shw_data_t) * (lite_properties.size));

    for (index = 0; index < lite_properties.size; index++) {
        memset(&lite_property, 0, sizeof(lite_cjson_t));
        res = lite_cjson_array_item(&lite_properties, index, &lite_property);
        if (res != SUCCESS_RETURN || !lite_cjson_is_object(&lite_property)) {
            return FAIL_RETURN;
        }

        dm_log_debug("Index: %d", index);
        _dm_shw_property_parse(shadow->properties + index, &lite_property);
    }

    return SUCCESS_RETURN;
}

static int _dm_shw_event_outputdata_parse(_IN_ dm_shw_t *shadow, _IN_ dm_shw_data_t *event_data,
        _IN_ lite_cjson_t *root)
{
    int res = 0;
    lite_cjson_t lite_item;

    /* Parse Identifier (Madantory) */
    memset(&lite_item, 0, sizeof(lite_cjson_t));
    res = lite_cjson_object_item(root, DM_SHW_KEY_IDENTIFIER, strlen(DM_SHW_KEY_IDENTIFIER), &lite_item);
    if (res != SUCCESS_RETURN || !lite_cjson_is_string(&lite_item)) {
        return DM_JSON_PARSE_FAILED;
    }
    res = dm_utils_copy(lite_item.value, lite_item.value_length, (void **) & (event_data->identifier),
                        lite_item.value_length + 1);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }
    dm_log_debug("Identifier: %s", event_data->identifier);

    /* Parse DataType */
    memset(&lite_item, 0, sizeof(lite_cjson_t));
    res = lite_cjson_object_item(root, DM_SHW_KEY_DATATYPE, strlen(DM_SHW_KEY_DATATYPE), &lite_item);
    if (res != SUCCESS_RETURN || !lite_cjson_is_object(&lite_item)) {
        return DM_JSON_PARSE_FAILED;
    }
    /* dm_log_debug("TSL Output Event Data Type: %.*s",lite_item.value_length,lite_item.value); */
    res = _dm_shw_data_parse(&event_data->data_value, &lite_item);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

static int _dm_shw_event_outputdatas_parse(_IN_ dm_shw_t *shadow, _IN_ dm_shw_event_t *event, _IN_ lite_cjson_t *root)
{
    int res = 0, index = 0;
    lite_cjson_t lite_item;
    dm_shw_data_t *output_data = NULL;

    dm_log_debug("Number: %d", event->output_data_number);
    if (event->output_data_number == 0) {
        return SUCCESS_RETURN;
    }

    /* Allocate Memory For Output Datas */
    event->output_datas = DM_malloc((event->output_data_number) * (sizeof(dm_shw_data_t)));
    if (event->output_datas == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(event->output_datas, 0, (event->output_data_number) * (sizeof(dm_shw_data_t)));

    for (index = 0; index < event->output_data_number; index++) {
        memset(&lite_item, 0, sizeof(lite_cjson_t));
        output_data = event->output_datas + index;

        res = lite_cjson_array_item(root, index, &lite_item);
        if (res != SUCCESS_RETURN || !lite_cjson_is_object(&lite_item)) {
            return FAIL_RETURN;
        }

        dm_log_debug("Index: %d", index);
        _dm_shw_event_outputdata_parse(shadow, output_data, &lite_item);
    }

    return SUCCESS_RETURN;
}

static int _dm_shw_event_parse(_IN_ dm_shw_t *shadow, _IN_ dm_shw_event_t *event, _IN_ lite_cjson_t *root)
{
    int res = 0;
    lite_cjson_t lite_item;

    /* Parse Identifier (Mandatory) */
    memset(&lite_item, 0, sizeof(lite_cjson_t));
    res = lite_cjson_object_item(root, DM_SHW_KEY_IDENTIFIER, strlen(DM_SHW_KEY_IDENTIFIER), &lite_item);
    if (res != SUCCESS_RETURN || !lite_cjson_is_string(&lite_item)) {
        return DM_JSON_PARSE_FAILED;
    }
    res = dm_utils_copy(lite_item.value, lite_item.value_length, (void **)(&event->identifier), lite_item.value_length + 1);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }
    dm_log_debug("Identifier: %s", event->identifier);

    /* Check If Current Event Id Is Post */
    if (((strlen(event->identifier) == strlen(DM_SHW_SPECIAL_EVENT_POST_IDENTIFIER)) &&
         (memcmp(event->identifier, DM_SHW_SPECIAL_EVENT_POST_IDENTIFIER, strlen(DM_SHW_SPECIAL_EVENT_POST_IDENTIFIER)) == 0))) {
        /* dm_log_info("TSL Special Event Identifier: %s, Ignore It",event->identifier); */
        return SUCCESS_RETURN;
    }

    /* Parse Output Data (Madantory) */
    memset(&lite_item, 0, sizeof(lite_cjson_t));
    res = lite_cjson_object_item(root, DM_SHW_KEY_OUTPUTDATA, strlen(DM_SHW_KEY_OUTPUTDATA), &lite_item);
    if (res != SUCCESS_RETURN || !lite_cjson_is_array(&lite_item)) {
        return DM_JSON_PARSE_FAILED;
    }
    event->output_data_number = lite_item.size;
    res = _dm_shw_event_outputdatas_parse(shadow, event, &lite_item);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

static int _dm_shw_events_parse(_IN_ dm_shw_t *shadow, _IN_ lite_cjson_t *root)
{
    int res = 0, index = 0;
    lite_cjson_t lite_events, lite_event;

    memset(&lite_events, 0, sizeof(lite_cjson_t));
    res = lite_cjson_object_item(root, DM_SHW_KEY_EVENTS, strlen(DM_SHW_KEY_EVENTS), &lite_events);
    if (res == SUCCESS_RETURN) {
        if (!lite_cjson_is_array(&lite_events)) {
            return DM_JSON_PARSE_FAILED;
        }
    } else {
        return SUCCESS_RETURN;
    }

    dm_log_debug("Number: %d", lite_events.size);
    if (lite_events.size == 0) {
        return SUCCESS_RETURN;
    }

    /* Allocate Memory For TSL Events Struct */
    shadow->event_number = lite_events.size;
    shadow->events = DM_malloc(sizeof(dm_shw_event_t) * (lite_events.size));
    if (shadow->events == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(shadow->events, 0, sizeof(dm_shw_event_t) * (lite_events.size));

    for (index = 0; index < lite_events.size; index++) {
        memset(&lite_event, 0, sizeof(lite_cjson_t));
        res = lite_cjson_array_item(&lite_events, index, &lite_event);
        if (res != SUCCESS_RETURN || !lite_cjson_is_object(&lite_event)) {
            return FAIL_RETURN;
        }

        dm_log_debug("Index: %d", index);
        _dm_shw_event_parse(shadow, shadow->events + index, &lite_event);
    }
    return SUCCESS_RETURN;
}

static int _dm_shw_service_outputdata_parse(_IN_ dm_shw_t *shadow, _IN_ dm_shw_data_t *service_data,
        _IN_ lite_cjson_t *root)
{
    int res = 0;
    lite_cjson_t lite_item;

    /* Parse Identifier (Madantory) */
    memset(&lite_item, 0, sizeof(lite_cjson_t));
    res = lite_cjson_object_item(root, DM_SHW_KEY_IDENTIFIER, strlen(DM_SHW_KEY_IDENTIFIER), &lite_item);
    if (res != SUCCESS_RETURN || !lite_cjson_is_string(&lite_item)) {
        return DM_JSON_PARSE_FAILED;
    }
    res = dm_utils_copy(lite_item.value, lite_item.value_length, (void **) & (service_data->identifier),
                        lite_item.value_length + 1);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }
    dm_log_debug("Identifier: %s", service_data->identifier);

    /* Parse DataType */
    memset(&lite_item, 0, sizeof(lite_cjson_t));
    res = lite_cjson_object_item(root, DM_SHW_KEY_DATATYPE, strlen(DM_SHW_KEY_DATATYPE), &lite_item);
    if (res != SUCCESS_RETURN || !lite_cjson_is_object(&lite_item)) {
        return DM_JSON_PARSE_FAILED;
    }
    /* dm_log_debug("TSL Output Service Data Type: %.*s",lite_item.value_length,lite_item.value); */
    res = _dm_shw_data_parse(&service_data->data_value, &lite_item);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

static int _dm_shw_service_outputdatas_parse(_IN_ dm_shw_t *shadow, _IN_ dm_shw_service_t *service,
        _IN_ lite_cjson_t *root)
{
    int res = 0, index = 0;
    lite_cjson_t lite_item;
    dm_shw_data_t *output_data = NULL;

    dm_log_debug("Number: %d", service->output_data_number);
    if (service->output_data_number == 0) {
        return SUCCESS_RETURN;
    }

    /* Allocate Memory For Output Datas */
    service->output_datas = DM_malloc((service->output_data_number) * (sizeof(dm_shw_data_t)));
    if (service->output_datas == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(service->output_datas, 0, (service->output_data_number) * (sizeof(dm_shw_data_t)));

    for (index = 0; index < service->output_data_number; index++) {
        memset(&lite_item, 0, sizeof(lite_cjson_t));
        output_data = service->output_datas + index;

        res = lite_cjson_array_item(root, index, &lite_item);
        if (res != SUCCESS_RETURN || !lite_cjson_is_object(&lite_item)) {
            return FAIL_RETURN;
        }

        dm_log_debug("Index: %d", index);
        _dm_shw_service_outputdata_parse(shadow, output_data, &lite_item);
    }

    return SUCCESS_RETURN;
}

static int _dm_shw_service_inputdata_parse(_IN_ dm_shw_t *shadow, _IN_ dm_shw_data_t *input_data,
        _IN_ lite_cjson_t *root)
{
    int res = 0;
    lite_cjson_t lite_item;

    if (!lite_cjson_is_object(root)) {
        return DM_INVALID_PARAMETER;
    }

    /* Parse Identifier (Madantory) */
    memset(&lite_item, 0, sizeof(lite_cjson_t));
    res = lite_cjson_object_item(root, DM_SHW_KEY_IDENTIFIER, strlen(DM_SHW_KEY_IDENTIFIER), &lite_item);
    if (res != SUCCESS_RETURN || !lite_cjson_is_string(&lite_item)) {
        return DM_JSON_PARSE_FAILED;
    }
    res = dm_utils_copy(lite_item.value, lite_item.value_length, (void **) & (input_data->identifier),
                        lite_item.value_length + 1);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }
    dm_log_debug("Identifier: %s", input_data->identifier);

    /* Parse DataType */
    memset(&lite_item, 0, sizeof(lite_cjson_t));
    res = lite_cjson_object_item(root, DM_SHW_KEY_DATATYPE, strlen(DM_SHW_KEY_DATATYPE), &lite_item);
    if (res != SUCCESS_RETURN || !lite_cjson_is_object(&lite_item)) {
        return DM_JSON_PARSE_FAILED;
    }
    /* dm_log_debug("TSL Input Service Data Type: %.*s",lite_item.value_length,lite_item.value); */
    res = _dm_shw_data_parse(&input_data->data_value, &lite_item);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

static int _dm_shw_service_inputdatas_parse(_IN_ dm_shw_t *shadow, _IN_ dm_shw_service_t *service,
        _IN_ lite_cjson_t *root)
{
    int res = 0, index = 0;
    lite_cjson_t lite_item;
    dm_shw_data_t *input_data = NULL;

    dm_log_debug("Number: %d", service->input_data_number);
    if (service->input_data_number == 0) {
        return SUCCESS_RETURN;
    }

    /* Allocate Memory For Output Datas */
    service->input_datas = DM_malloc((service->input_data_number) * (sizeof(dm_shw_data_t)));
    if (service->input_datas == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(service->input_datas, 0, (service->input_data_number) * (sizeof(dm_shw_data_t)));

    for (index = 0; index < service->input_data_number; index++) {
        memset(&lite_item, 0, sizeof(lite_cjson_t));
        input_data = service->input_datas + index;

        res = lite_cjson_array_item(root, index, &lite_item);
        if (res != SUCCESS_RETURN) {
            return FAIL_RETURN;
        }

        dm_log_debug("Index: %d", index);
        _dm_shw_service_inputdata_parse(shadow, input_data, &lite_item);
    }

    return SUCCESS_RETURN;
}

static int _dm_shw_service_parse(_IN_ dm_shw_t *shadow, _IN_ dm_shw_service_t *service, _IN_ lite_cjson_t *root)
{
    int res = 0;
    lite_cjson_t lite_item;

    /* Parse Identifier (Mandatory) */
    memset(&lite_item, 0, sizeof(lite_cjson_t));
    res = lite_cjson_object_item(root, DM_SHW_KEY_IDENTIFIER, strlen(DM_SHW_KEY_IDENTIFIER), &lite_item);
    if (res != SUCCESS_RETURN || !lite_cjson_is_string(&lite_item)) {
        return DM_JSON_PARSE_FAILED;
    }
    res = dm_utils_copy(lite_item.value, lite_item.value_length, (void **)(&service->identifier),
                        lite_item.value_length + 1);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }
    dm_log_debug("Identifier: %s", service->identifier);

    /* Check If Current Service Id Is Set Or Get */
    if (((strlen(service->identifier) == strlen(DM_SHW_SPECIAL_SERVICE_SET_IDENTIFIER)) &&
         (memcmp(service->identifier, DM_SHW_SPECIAL_SERVICE_SET_IDENTIFIER,
                 strlen(DM_SHW_SPECIAL_SERVICE_SET_IDENTIFIER)) == 0)) ||
        ((strlen(service->identifier) == strlen(DM_SHW_SPECIAL_SERVICE_GET_IDENTIFIER)) &&
         (memcmp(service->identifier, DM_SHW_SPECIAL_SERVICE_GET_IDENTIFIER,
                 strlen(DM_SHW_SPECIAL_SERVICE_GET_IDENTIFIER)) == 0))) {
        /* dm_log_info("TSL Special Service Identifier: %s, Ignore It",service->identifier); */
        return SUCCESS_RETURN;
    }

    /* Parse Output Data (Optional) */
    memset(&lite_item, 0, sizeof(lite_cjson_t));
    res = lite_cjson_object_item(root, DM_SHW_KEY_OUTPUTDATA, strlen(DM_SHW_KEY_OUTPUTDATA), &lite_item);
    if (res == SUCCESS_RETURN && lite_cjson_is_array(&lite_item)) {
        service->output_data_number = lite_item.size;
        res = _dm_shw_service_outputdatas_parse(shadow, service, &lite_item);
        if (res != SUCCESS_RETURN) {
            return FAIL_RETURN;
        }
    }

    /* Parse Input Data (Optional) */
    memset(&lite_item, 0, sizeof(lite_cjson_t));
    res = lite_cjson_object_item(root, DM_SHW_KEY_INPUTDATA, strlen(DM_SHW_KEY_INPUTDATA), &lite_item);
    if (res == SUCCESS_RETURN && lite_cjson_is_array(&lite_item)) {
        service->input_data_number = lite_item.size;
        res = _dm_shw_service_inputdatas_parse(shadow, service, &lite_item);
        if (res != SUCCESS_RETURN) {
            return FAIL_RETURN;
        }
    }

    return SUCCESS_RETURN;
}

static int _dm_shw_services_parse(_IN_ dm_shw_t *shadow, _IN_ lite_cjson_t *root)
{
    int res = 0, index = 0;
    lite_cjson_t lite_services, lite_service;
    dm_shw_service_t *service = NULL;

    memset(&lite_services, 0, sizeof(lite_cjson_t));
    res = lite_cjson_object_item(root, DM_SHW_KEY_SERVICES, strlen(DM_SHW_KEY_SERVICES), &lite_services);
    if (res == SUCCESS_RETURN) {
        if (!lite_cjson_is_array(&lite_services)) {
            return DM_JSON_PARSE_FAILED;
        }
    } else {
        return SUCCESS_RETURN;
    }

    dm_log_debug("Number: %d", lite_services.size);
    if (lite_services.size == 0) {
        return SUCCESS_RETURN;
    }

    /* Allocate Memory For TSL Services Struct */
    shadow->service_number = lite_services.size;
    shadow->services = DM_malloc(sizeof(dm_shw_service_t) * (lite_services.size));
    if (shadow->services == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(shadow->services, 0, sizeof(dm_shw_service_t) * (lite_services.size));

    for (index = 0; index < lite_services.size; index++) {
        memset(&lite_service, 0, sizeof(lite_cjson_t));
        service = shadow->services + index;

        res = lite_cjson_array_item(&lite_services, index, &lite_service);
        if (res != SUCCESS_RETURN || !lite_cjson_is_object(&lite_service)) {
            return FAIL_RETURN;
        }

        dm_log_debug("Index: %d", index);
        _dm_shw_service_parse(shadow, service, &lite_service);
    }

    return SUCCESS_RETURN;
}

int dm_tsl_alink_create(_IN_ const char *tsl, _IN_ int tsl_len, _OU_ dm_shw_t **shadow)
{
    int res = 0;
    lite_cjson_t lite_root;

    if (shadow == NULL || *shadow != NULL || tsl == NULL || tsl_len <= 0) {
        return DM_INVALID_PARAMETER;
    }

    *shadow = DM_malloc(sizeof(dm_shw_t));
    if (*shadow == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(*shadow, 0, sizeof(dm_shw_t));

    /* Parse Root */
    memset(&lite_root, 0, sizeof(lite_root));
    res = lite_cjson_parse(tsl, tsl_len, &lite_root);
    if (res != SUCCESS_RETURN || !lite_cjson_is_object(&lite_root)) {
        DM_free(*shadow);
        return DM_JSON_PARSE_FAILED;
    }

    /* Parse Properties (Mandatory) */
    res = _dm_shw_properties_parse(*shadow, &lite_root);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Parse Events (Mandatory) */
    res = _dm_shw_events_parse(*shadow, &lite_root);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Parse Services (Mandatory) */
    res = _dm_shw_services_parse(*shadow, &lite_root);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}
#endif