/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#include "iotx_dm_internal.h"

int dm_utils_copy_direct(_IN_ void *input, _IN_ int input_len, _OU_ void **output, _IN_ int output_len)
{
    if (input == NULL || output == NULL || *output != NULL) {
        return DM_INVALID_PARAMETER;
    }

    *output = HAL_Malloc(output_len);
    if (*output == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(*output, 0, output_len);
    memcpy(*output, input, input_len);

    return SUCCESS_RETURN;
}

int dm_utils_copy(_IN_ void *input, _IN_ int input_len, _OU_ void **output, _IN_ int output_len)
{
    if (input == NULL || output == NULL || *output != NULL) {
        return DM_INVALID_PARAMETER;
    }

    *output = DM_malloc(output_len);
    if (*output == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(*output, 0, output_len);
    memcpy(*output, input, input_len);

    return SUCCESS_RETURN;
}

int dm_utils_strarr_index(_IN_ char *input, _IN_ int input_len,
                          _OU_ int *partial_input_len, _OU_ int *array_input_len, _OU_ int *array_index)
{
    int index = 0;
    int deep = 0;
    char *bracket_pre = NULL;
    char *bracket_suf = NULL;
    char array_index_str[10] = {0};

    if (input == NULL || input_len <= 1 || array_index == NULL) {
        return DM_INVALID_PARAMETER;
    }

    for (index = 0; index < input_len; index++) {
        switch (input[index]) {
            case '[': {
                if (deep != 0) {
                    return FAIL_RETURN;
                }
                deep++;
                if (!bracket_pre) {
                    bracket_pre = (char *)&input[index];
                }
            }
            break;
            case ']': {
                if (deep != 1) {
                    return FAIL_RETURN;
                }
                deep--;
                if (input[index  - 1] == '[') {
                    return FAIL_RETURN;
                }
                if (!bracket_suf) {
                    bracket_suf = (char *)&input[index];
                }
            }
            break;
            default:
                break;
        }
    }

    if (bracket_pre && bracket_suf && ((bracket_suf - input + 1) == input_len)) {
        if (partial_input_len) {
            *partial_input_len = bracket_pre - input;
        }
        if (array_input_len) {
            *array_input_len = bracket_suf - input + 1;
        }

        /* Get Index */
        memcpy(array_index_str, bracket_pre + 1, bracket_suf - bracket_pre - 1);
        *array_index = atoi(array_index_str);
        return SUCCESS_RETURN;
    }

    return FAIL_RETURN;
}

int dm_utils_itoa_direct(_IN_ int input, _OU_ char **output)
{
    int res = 0;
    char temp_output[10 + 1] = {0};

    if (output == NULL || *output != NULL) {
        return DM_INVALID_PARAMETER;
    }

    res = HAL_Snprintf(temp_output, 10, "%d", input);
    if (res < 0) {
        return FAIL_RETURN;
    }

    *output = HAL_Malloc(strlen(temp_output) + 1);
    if (*output == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(*output, 0, strlen(temp_output) + 1);
    memcpy(*output, temp_output, strlen(temp_output));

    return SUCCESS_RETURN;
}

int dm_utils_itoa(_IN_ int input, _OU_ char **output)
{
    int res = 0;
    char temp_output[10 + 1] = {0};

    if (output == NULL || *output != NULL) {
        return DM_INVALID_PARAMETER;
    }

    res = HAL_Snprintf(temp_output, 10, "%d", input);
    if (res < 0) {
        return FAIL_RETURN;
    }

    *output = DM_malloc(strlen(temp_output) + 1);
    if (*output == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(*output, 0, strlen(temp_output) + 1);
    memcpy(*output, temp_output, strlen(temp_output));

    return SUCCESS_RETURN;
}

int dm_utils_ftoa_direct(_IN_ double input, _OU_ char **output)
{
    int res = 0;
    char temp_output[30 + 1] = {0};

    if (output == NULL || *output != NULL) {
        return DM_INVALID_PARAMETER;
    }

    res = HAL_Snprintf(temp_output, 30, "%f", input);
    if (res < 0) {
        return FAIL_RETURN;
    }

    *output = HAL_Malloc(strlen(temp_output) + 1);
    if (*output == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(*output, 0, strlen(temp_output) + 1);
    memcpy(*output, temp_output, strlen(temp_output));

    return SUCCESS_RETURN;
}

int dm_utils_ftoa(_IN_ double input, _OU_ char **output)
{
    int res = 0;
    char temp_output[30 + 1] = {0};

    if (output == NULL || *output != NULL) {
        return DM_INVALID_PARAMETER;
    }

    res = HAL_Snprintf(temp_output, 30, "%f", input);
    if (res < 0) {
        return FAIL_RETURN;
    }

    *output = DM_malloc(strlen(temp_output) + 1);
    if (*output == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(*output, 0, strlen(temp_output) + 1);
    memcpy(*output, temp_output, strlen(temp_output));

    return SUCCESS_RETURN;
}

int dm_utils_hex_to_str(_IN_ unsigned char *input, _IN_ int input_len, _OU_ char **output)
{
    int index = 0, output_len = 0;
    unsigned char iter_char = 0;

    if (input == NULL || input_len <= 0 || output == NULL || *output != NULL) {
        return DM_INVALID_PARAMETER;
    }

    output_len = input_len * 2;
    *output = DM_malloc(output_len + 1);
    if (*output == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(*output, 0, output_len + 1);

    for (index = 0; index < input_len; index++) {
        iter_char = (input[index] >> 4) & 0x0F;
        if (iter_char <= 0x09) {
            iter_char += '0';
        } else if (iter_char >= 0x0A && iter_char <= 0x0F) {
            iter_char += 'A' - 0x0A;
        }
        (*output)[index * 2] = iter_char;

        iter_char = (input[index]) & 0x0F;
        if (iter_char <= 0x09) {
            iter_char += '0';
        } else if (iter_char >= 0x0A && iter_char <= 0x0F) {
            iter_char += 'A' - 0x0A;
        }
        (*output)[index * 2 + 1] = iter_char;
    }

    return SUCCESS_RETURN;
}

int dm_utils_str_to_hex(_IN_ char *input, _IN_ int input_len, _OU_ unsigned char **output, _OU_ int *output_len)
{
    int index = 0;
    char iter_char = 0;

    if (input == NULL || input_len <= 0 || input_len % 2 != 0 ||
        output == NULL || *output != NULL || output_len == NULL) {
        return DM_INVALID_PARAMETER;
    }

    *output_len = input_len / 2;
    *output = DM_malloc(*output_len);
    if (*output == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(*output, 0, *output_len);

    for (index = 0; index < input_len; index += 2) {
        if (input[index] >= '0' && input[index] <= '9') {
            iter_char = input[index] - '0';
        } else if (input[index] >= 'A' && input[index] <= 'F') {
            iter_char = input[index] - 'A' + 0x0A;
        }
        (*output)[index / 2] |= (iter_char << 4) & 0xF0;

        if (input[index + 1] >= '0' && input[index + 1] <= '9') {
            iter_char = input[index + 1] - '0';
        } else if (input[index + 1] >= 'A' && input[index + 1] <= 'F') {
            iter_char = input[index + 1] - 'A' + 0x0A;
        }
        (*output)[index / 2] |= (iter_char) & 0x0F;
    }

    return SUCCESS_RETURN;
}

int dm_utils_memtok(_IN_ char *input, _IN_ int input_len, _IN_ char delimiter, _IN_ int index, _OU_ int *offset)
{
    int item_index = 0;
    int count = 0;

    if (input == NULL || input_len <= 0 || offset == NULL) {
        return DM_INVALID_PARAMETER;
    }

    for (item_index = 0; item_index < input_len; item_index++) {
        if (input[item_index] == delimiter && (item_index + 1) < input_len) {
            count++;
            if (count == index) {
                *offset = item_index;
                return SUCCESS_RETURN;
            }
        }
    }

    return FAIL_RETURN;
}

int dm_utils_replace_char(_IN_ char *input, _IN_ int input_len, _IN_ char src, _IN_ char dest)
{
    int index = 0;

    if (input == NULL || input_len <= 0) {
        return DM_INVALID_PARAMETER;
    }

    for (index = 0; index < input_len; index++) {
        if (input[index] == src) {
            input[index] = dest;
        }
    }

    return SUCCESS_RETURN;
}

int dm_utils_service_name(_IN_ const char *prefix, _IN_ const char *name, _IN_ char product_key[IOTX_PRODUCT_KEY_LEN + 1],
                          _IN_ char device_name[IOTX_DEVICE_NAME_LEN + 1], _OU_ char **service_name)
{
    int prefix_len = (prefix == NULL) ? (0) : (strlen(prefix));
    int name_len = (name == NULL) ? (0) : (strlen(name));
    int service_name_len = 0;
    if ((prefix == NULL && name == NULL) || product_key == NULL || device_name == NULL ||
        service_name == NULL || *service_name != NULL) {
        return DM_INVALID_PARAMETER;
    }

    service_name_len = prefix_len + name_len + strlen(product_key) + strlen(device_name) + 1;
    *service_name = DM_malloc(service_name_len);
    if (*service_name == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(*service_name, 0, service_name_len);

    if (prefix != NULL) {
        HAL_Snprintf(*service_name, service_name_len, prefix, product_key, device_name);
    }

    if (name != NULL) {
        memcpy(*service_name + strlen(*service_name), name, name_len);
    }

    return SUCCESS_RETURN;
}

int dm_utils_uri_add_prefix(_IN_ const char *prefix, _IN_ char *uri, _OU_ char **new_uri)
{
    int new_uri_len = 0;

    if (prefix == NULL || uri == NULL || new_uri == NULL || *new_uri != NULL) {
        return DM_INVALID_PARAMETER;
    }

    new_uri_len = strlen(prefix) + strlen(uri) + 1;
    *new_uri = DM_malloc(new_uri_len);
    if (*new_uri == NULL) {
        return DM_MEMORY_NOT_ENOUGH;
    }
    memset(*new_uri, 0, new_uri_len);

    memcpy(*new_uri, prefix, strlen(prefix));
    memcpy(*new_uri + strlen(*new_uri), uri, strlen(uri));

    return SUCCESS_RETURN;
}

int dm_utils_json_parse(_IN_ const char *payload, _IN_ int payload_len, _IN_ int type, _OU_ lite_cjson_t *lite)
{
    int res = 0;

    if (payload == NULL || payload_len <= 0 || type < 0 || lite == NULL) {
        return DM_INVALID_PARAMETER;
    }
    memset(lite, 0, sizeof(lite_cjson_t));

    res = lite_cjson_parse(payload, payload_len, lite);
    if (res != SUCCESS_RETURN) {
        memset(lite, 0, sizeof(lite_cjson_t));
        return FAIL_RETURN;
    }

    if (type != cJSON_Invalid && lite->type != type) {
        memset(lite, 0, sizeof(lite_cjson_t));
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

int dm_utils_json_object_item(_IN_ lite_cjson_t *lite, _IN_ const char *key, _IN_ int key_len, _IN_ int type,
                              _OU_ lite_cjson_t *lite_item)
{
    int res = 0;

    if (lite == NULL || lite->type != cJSON_Object || key == NULL || key_len <= 0 || type < 0 || lite_item == NULL) {
        return DM_INVALID_PARAMETER;
    }

    if (lite->type != cJSON_Object) {
        dm_log_err("lite->type != cJSON_Object, %d", lite->type);
    }

    memset(lite_item, 0, sizeof(lite_cjson_t));

    res = lite_cjson_object_item(lite, key, key_len, lite_item);
    if (res != SUCCESS_RETURN) {
        /* dm_log_err(DM_UTILS_LOG_JSON_PARSE_FAILED, lite->value_length, lite->value); */
        memset(lite_item, 0, sizeof(lite_cjson_t));
        return FAIL_RETURN;
    }

    if (type != cJSON_Invalid && lite_item->type != type) {
        memset(lite_item, 0, sizeof(lite_cjson_t));
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

void *dm_utils_malloc(unsigned int size)
{
#ifdef INFRA_MEM_STATS
    return LITE_malloc(size, MEM_MAGIC, "lite_cjson");
#else
    return HAL_Malloc(size);
#endif
}

void dm_utils_free(void *ptr)
{
#ifdef INFRA_MEM_STATS
    LITE_free(ptr);
#else
    HAL_Free((void *)ptr);
#endif
}