/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include "infra_config.h"

#ifdef INFRA_CJSON

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

#include <math.h>
#include <float.h>
#include <ctype.h>

#include "infra_cjson.h"
#include "infra_types.h"

typedef struct {
    const unsigned char *content;
    int length;
    int offset;
    int depth; /* How deeply nested (in arrays/objects) is the input at the current offset. */
} parse_buffer;

/* check if the given size is left to read in a given parse buffer (starting with 1) */
#define can_read(buffer, size) ((buffer != NULL) && (((buffer)->offset + size) <= (buffer)->length))
/* check if the buffer can be accessed at the given index (starting with 0) */
#define can_access_at_index(buffer, index) ((buffer != NULL) && (((buffer)->offset + index) < (buffer)->length))
#define cannot_access_at_index(buffer, index) (!can_access_at_index(buffer, index))
/* get a pointer to the buffer at the position */
#define buffer_at_offset(buffer) ((buffer)->content + (buffer)->offset)

/* Predeclare these prototypes. */
static int parse_value(lite_cjson_t *const item, parse_buffer *const input_buffer);
static int parse_string(lite_cjson_t *const item, parse_buffer *const input_buffer);
static int parse_array(lite_cjson_t *const item, parse_buffer *const input_buffer);
static int parse_object(lite_cjson_t *const item, parse_buffer *const input_buffer);

/* Utility to jump whitespace and cr/lf */
static parse_buffer *buffer_skip_whitespace(parse_buffer *const buffer)
{
    if ((buffer == NULL) || (buffer->content == NULL)) {
        return NULL;
    }

    while (can_access_at_index(buffer, 0) && (buffer_at_offset(buffer)[0] <= 32)) {
        buffer->offset++;
    }

    if (buffer->offset == buffer->length) {
        buffer->offset--;
    }

    return buffer;
}

/* skip the UTF-8 BOM (byte order mark) if it is at the beginning of a buffer */
static parse_buffer *skip_utf8_bom(parse_buffer *const buffer)
{
    if ((buffer == NULL) || (buffer->content == NULL) || (buffer->offset != 0)) {
        return NULL;
    }

    if (can_access_at_index(buffer, 4) && (strncmp((const char *)buffer_at_offset(buffer), "\xEF\xBB\xBF", 3) == 0)) {
        buffer->offset += 3;
    }

    return buffer;
}

/* Parse the input text to generate a number, and populate the result into item. */
static int parse_number(lite_cjson_t *const item, parse_buffer *const input_buffer)
{
    double number = 0;
    unsigned char *after_end = NULL;
    unsigned char number_c_string[64];
    unsigned char decimal_point = '.';
    int i = 0;

    if ((input_buffer == NULL) || (input_buffer->content == NULL)) {
        return -1;
    }

    /* copy the number into a temporary buffer and replace '.' with the decimal point
     * of the current locale (for strtod)
     * This also takes care of '\0' not necessarily being available for marking the end of the input */
    for (i = 0; (i < (sizeof(number_c_string) - 1)) && can_access_at_index(input_buffer, i); i++) {
        switch (buffer_at_offset(input_buffer)[i]) {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            case '+':
            case '-':
            case 'e':
            case 'E':
                number_c_string[i] = buffer_at_offset(input_buffer)[i];
                break;
            case '.':
                number_c_string[i] = decimal_point;
                break;

            default:
                goto loop_end;
        }
    }
loop_end:
    number_c_string[i] = '\0';
    number = strtod((const char *)number_c_string, (char **)&after_end);
    if (number_c_string == after_end) {
        return -1;
    }

    item->type = cJSON_Number;
    item->value = (char *)buffer_at_offset(input_buffer);
    item->value_length = (int)(after_end - number_c_string);
    item->value_double = number;

    /* use saturation in case of overflow */
    if (number >= INT_MAX) {
        item->value_int = INT_MAX;
    } else if (number <= INT_MIN) {
        item->value_int = INT_MIN;
    } else {
        item->value_int = (int)number;
    }

    input_buffer->offset += (int)(after_end - number_c_string);
    return 0;
}

/* Build an array from input text. */
static int parse_array(lite_cjson_t *const item, parse_buffer *const input_buffer)
{
    lite_cjson_t current_item;
    int start_pos = input_buffer->offset;
    item->size = 0;

    if (input_buffer->depth >= LITE_CJSON_NESTING_LIMIT) {
        return -1; /* to deeply nested */
    }
    input_buffer->depth++;

    if (buffer_at_offset(input_buffer)[0] != '[') {
        /* not an array */
        goto fail;
    }

    input_buffer->offset++;
    buffer_skip_whitespace(input_buffer);
    if (can_access_at_index(input_buffer, 0) && (buffer_at_offset(input_buffer)[0] == ']')) {
        /* empty array */
        goto success;
    }

    /* check if we skipped to the end of the buffer */
    if (cannot_access_at_index(input_buffer, 0)) {
        input_buffer->offset--;
        goto fail;
    }

    /* step back to character in front of the first element */
    input_buffer->offset--;
    /* loop through the comma separated array elements */
    do {
        memset(&current_item, 0, sizeof(lite_cjson_t));

        /* parse next value */
        input_buffer->offset++;
        buffer_skip_whitespace(input_buffer);
        if (parse_value(&current_item, input_buffer) != 0) {
            goto fail; /* failed to parse value */
        }
        buffer_skip_whitespace(input_buffer);

        /* printf("current item index: %d, type: %d, length: %d, value: %.*s\n", */
        /* item->size+1, current_item.type, current_item.value_length, current_item.value_length, current_item.value); */

        item->size++;
    } while (can_access_at_index(input_buffer, 0) && (buffer_at_offset(input_buffer)[0] == ','));

    if (cannot_access_at_index(input_buffer, 0) || buffer_at_offset(input_buffer)[0] != ']') {
        goto fail; /* expected end of array */
    }

success:
    input_buffer->depth--;

    item->type = cJSON_Array;
    item->value = (char *)(input_buffer->content + start_pos);
    item->value_length = input_buffer->offset - start_pos + 1;

    input_buffer->offset++;

    return 0;

fail:

    return -1;
}

/* Build an object from the text. */
static int parse_object(lite_cjson_t *const item, parse_buffer *const input_buffer)
{
    lite_cjson_t current_item_key;
    lite_cjson_t current_item_value;
    int start_pos = input_buffer->offset;
    item->size = 0;

    if (input_buffer->depth >= LITE_CJSON_NESTING_LIMIT) {
        return -1; /* to deeply nested */
    }
    input_buffer->depth++;

    if (cannot_access_at_index(input_buffer, 0) || (buffer_at_offset(input_buffer)[0] != '{')) {
        goto fail; /* not an object */
    }

    input_buffer->offset++;
    buffer_skip_whitespace(input_buffer);
    if (can_access_at_index(input_buffer, 0) && (buffer_at_offset(input_buffer)[0] == '}')) {
        goto success; /* empty object */
    }

    /* check if we skipped to the end of the buffer */
    if (cannot_access_at_index(input_buffer, 0)) {
        input_buffer->offset--;
        goto fail;
    }

    /* step back to character in front of the first element */
    input_buffer->offset--;
    /* loop through the comma separated array elements */
    do {
        memset(&current_item_key, 0, sizeof(lite_cjson_t));
        memset(&current_item_value, 0, sizeof(lite_cjson_t));

        /* parse the name of the child */
        input_buffer->offset++;
        buffer_skip_whitespace(input_buffer);
        if (parse_string(&current_item_key, input_buffer) != 0) {
            goto fail; /* faile to parse name */
        }
        buffer_skip_whitespace(input_buffer);

        if (cannot_access_at_index(input_buffer, 0) || (buffer_at_offset(input_buffer)[0] != ':')) {
            goto fail; /* invalid object */
        }

        /* parse the value */
        input_buffer->offset++;
        buffer_skip_whitespace(input_buffer);
        if (parse_value(&current_item_value, input_buffer) != 0) {
            goto fail; /* failed to parse value */
        }
        buffer_skip_whitespace(input_buffer);

        /* printf("Current Object [Index: %d], [Key Length: %d, Key Value: %.*s], [Value Length: %d, Value: %.*s]\n", */
        /* item->size+1,current_item_key.value_length,current_item_key.value_length,current_item_key.value, */
        /* current_item_value.value_length,current_item_value.value_length,current_item_value.value); */

        item->size++;
    } while (can_access_at_index(input_buffer, 0) && (buffer_at_offset(input_buffer)[0] == ','));

    if (cannot_access_at_index(input_buffer, 0) || (buffer_at_offset(input_buffer)[0] != '}')) {
        goto fail; /* expected end of object */
    }

success:
    input_buffer->depth--;

    item->type = cJSON_Object;
    item->value = (char *)(input_buffer->content + start_pos);
    item->value_length = input_buffer->offset - start_pos + 1;

    input_buffer->offset++;

    return 0;

fail:

    return -1;
}

/* Parse the input text into an unescaped cinput, and populate item. */
static int parse_string(lite_cjson_t *const item, parse_buffer *const input_buffer)
{
    const unsigned char *input_pointer = buffer_at_offset(input_buffer) + 1;
    const unsigned char *input_end = buffer_at_offset(input_buffer) + 1;
    /* unsigned char *output_pointer = NULL; */
    /* unsigned char *output = NULL; */

    /* not a string */
    if (buffer_at_offset(input_buffer)[0] != '\"') {
        /* printf("not a string"); */
        goto fail;
    }

    {
        /* calculate approximate size of the output (overestimate) */
        /* int allocation_length = 0; */
        int skipped_bytes = 0;
        while (((int)(input_end - input_buffer->content) < input_buffer->length) && (*input_end != '\"')) {
            /* is escape sequence */
            if (input_end[0] == '\\') {
                if ((int)(input_end + 1 - input_buffer->content) >= input_buffer->length) {
                    /* prevent buffer overflow when last input character is a backslash */
                    goto fail;
                }
                skipped_bytes++;
                input_end++;
            }
            input_end++;
        }
        if (((int)(input_end - input_buffer->content) >= input_buffer->length) || (*input_end != '\"')) {
            /* printf("end error\n"); */
            goto fail; /* string ended unexpectedly */
        }
#if 0
        /* This is at most how much we need for the output */
        allocation_length = (int)(input_end - buffer_at_offset(input_buffer)) - skipped_bytes;
        output = (unsigned char *)input_buffer->hooks.allocate(allocation_length + sizeof(""));
        if (output == NULL) {
            goto fail; /* allocation failure */
        }
#endif
    }

#if 0
    output_pointer = output;
    /* loop through the string literal */
    while (input_pointer < input_end) {
        if (*input_pointer != '\\') {
            *output_pointer++ = *input_pointer++;
        }
        /* escape sequence */
        else {
            unsigned char sequence_length = 2;
            if ((input_end - input_pointer) < 1) {
                goto fail;
            }

            switch (input_pointer[1]) {
                case 'b':
                    *output_pointer++ = '\b';
                    break;
                case 'f':
                    *output_pointer++ = '\f';
                    break;
                case 'n':
                    *output_pointer++ = '\n';
                    break;
                case 'r':
                    *output_pointer++ = '\r';
                    break;
                case 't':
                    *output_pointer++ = '\t';
                    break;
                case '\"':
                case '\\':
                case '/':
                    *output_pointer++ = input_pointer[1];
                    break;

                /* UTF-16 literal */
                case 'u':
                    sequence_length = utf16_literal_to_utf8(input_pointer, input_end, &output_pointer);
                    if (sequence_length == 0) {
                        /* failed to convert UTF16-literal to UTF-8 */
                        goto fail;
                    }
                    break;

                default:
                    goto fail;
            }
            input_pointer += sequence_length;
        }
    }

    /* zero terminate the output */
    *output_pointer = '\0';
#endif

    item->type = cJSON_String;
    item->value = (char *)input_pointer;
    item->value_length = input_end - input_pointer;

    input_buffer->offset = (int)(input_end - input_buffer->content);
    input_buffer->offset++;

    return 0;

fail:
#if 0
    if (output != NULL) {
        input_buffer->hooks.deallocate(output);
    }
#endif
    if (input_pointer != NULL) {
        input_buffer->offset = (int)(input_pointer - input_buffer->content);
    }

    return -1;
}

/* Parser core - when encountering text, process appropriately. */
static int parse_value(lite_cjson_t *const lite, parse_buffer *const input_buffer)
{
    if ((input_buffer == NULL) || (input_buffer->content == NULL)) {
        return -1; /* no input */
    }

    /* parse the different types of values */
    /* null */
    if (can_read(input_buffer, 4) && (strncmp((const char *)buffer_at_offset(input_buffer), "null", 4) == 0)) {
        lite->type = cJSON_NULL;
        lite->value = (char *)buffer_at_offset(input_buffer);
        lite->value_length = 4;

        input_buffer->offset += 4;
        return 0;
    }
    /* false */
    if (can_read(input_buffer, 5) && (strncmp((const char *)buffer_at_offset(input_buffer), "false", 5) == 0)) {
        lite->type = cJSON_False;
        lite->value = (char *)buffer_at_offset(input_buffer);
        lite->value_length = 5;

        input_buffer->offset += 5;
        return 0;
    }
    /* true */
    if (can_read(input_buffer, 4) && (strncmp((const char *)buffer_at_offset(input_buffer), "true", 4) == 0)) {
        lite->type = cJSON_True;
        lite->value = (char *)buffer_at_offset(input_buffer);
        lite->value_length = 4;

        input_buffer->offset += 4;
        return 0;
    }
    /* string */
    if (can_access_at_index(input_buffer, 0) && (buffer_at_offset(input_buffer)[0] == '\"')) {
        /* printf("parse string\n"); */
        return parse_string(lite, input_buffer);
    }

    /* number */
    if (can_access_at_index(input_buffer, 0) && ((buffer_at_offset(input_buffer)[0] == '-')
            || ((buffer_at_offset(input_buffer)[0] >= '0') && (buffer_at_offset(input_buffer)[0] <= '9')))) {
        /* printf("parse number\n"); */
        return parse_number(lite, input_buffer);
    }

    /* array */
    if (can_access_at_index(input_buffer, 0) && (buffer_at_offset(input_buffer)[0] == '[')) {
        /* printf("parse array\n"); */
        return parse_array(lite, input_buffer);
    }

    /* object */
    if (can_access_at_index(input_buffer, 0) && (buffer_at_offset(input_buffer)[0] == '{')) {
        return parse_object(lite, input_buffer);
    }

    return -1;
}

int lite_cjson_parse(const char *src, int src_len, lite_cjson_t *lite)
{
    parse_buffer buffer;

    if (!lite || !src || !lite || src_len <= 0) {
        return -1;
    }

    memset(&buffer, 0, sizeof(parse_buffer));
    buffer.content = (const unsigned char *)src;
    buffer.length = src_len;
    buffer.offset = 0;

    if (parse_value(lite, buffer_skip_whitespace(skip_utf8_bom(&buffer))) != 0) {
        lite->type = cJSON_Invalid;
        lite->value = NULL;
        lite->value_length = 0;
        return -1;
    }

    return 0;
}

#if 0
int lite_cjson_is_false(lite_cjson_t *lite)
{
    if (!lite) {
        return -1;
    }

    return (lite->type & 0xFF) == cJSON_False;
}

int lite_cjson_is_true(lite_cjson_t *lite)
{
    if (!lite) {
        return -1;
    }

    return (lite->type & 0xFF) == cJSON_True;
}

int lite_cjson_is_null(lite_cjson_t *lite)
{
    if (!lite) {
        return -1;
    }

    return (lite->type & 0xFF) == cJSON_NULL;
}
#endif

int lite_cjson_is_number(lite_cjson_t *lite)
{
    if (!lite) {
        return -1;
    }

    return (lite->type & 0xFF) == cJSON_Number;
}

int lite_cjson_is_string(lite_cjson_t *lite)
{
    if (!lite) {
        return -1;
    }

    return (lite->type & 0xFF) == cJSON_String;
}

int lite_cjson_is_array(lite_cjson_t *lite)
{
    if (!lite) {
        return -1;
    }

    return (lite->type & 0xFF) == cJSON_Array;
}

int lite_cjson_is_object(lite_cjson_t *lite)
{
    if (!lite) {
        return -1;
    }

    return (lite->type & 0xFF) == cJSON_Object;
}

int lite_cjson_array_item(lite_cjson_t *lite, int index, lite_cjson_t *lite_item)
{
    parse_buffer buffer;
    parse_buffer *p_buffer = &buffer;
    lite_cjson_t current_item;
    int iter_index = 0;

    if (!lite || lite->type != cJSON_Array || !lite->value ||
        index < 0 || index >= lite->size || !lite_item) {
        return -1;
    }

    memset(&buffer, 0, sizeof(parse_buffer));
    buffer.content = (const unsigned char *)lite->value;
    buffer.length = lite->value_length;
    buffer.offset = 0;

    /* int start_pos = p_buffer->offset; */

    if (buffer_at_offset(p_buffer)[0] != '[') {
        /* not an array */
        return -1;
    }

    p_buffer->offset++;
    buffer_skip_whitespace(p_buffer);
    if (can_access_at_index(p_buffer, 0) && (buffer_at_offset(p_buffer)[0] == ']')) {
        /* empty array */
        return -1;
    }

    /* check if we skipped to the end of the buffer */
    if (cannot_access_at_index(p_buffer, 0)) {
        p_buffer->offset--;
        return -1;
    }

    /* step back to character in front of the first element */
    p_buffer->offset--;
    /* loop through the comma separated array elements */
    do {
        memset(&current_item, 0, sizeof(lite_cjson_t));

        /* parse next value */
        p_buffer->offset++;
        buffer_skip_whitespace(p_buffer);
        if (parse_value(&current_item, p_buffer) != 0) {
            return -1; /* failed to parse value */
        }
        buffer_skip_whitespace(p_buffer);

        /* printf("current item index: %d, type: %d, length: %d, value: %.*s\n", */
        /* iter_index+1, current_item.type, current_item.value_length, current_item.value_length, current_item.value); */

        if (iter_index == index) {
            memcpy(lite_item, &current_item, sizeof(lite_cjson_t));
            return 0;
        }

        iter_index++;
    } while (can_access_at_index(p_buffer, 0) && (buffer_at_offset(p_buffer)[0] == ','));

    return -1;
}

static int _lite_cjson_object_item(lite_cjson_t *lite, const char *key, int key_len,
                                   lite_cjson_t *lite_item)
{
    parse_buffer buffer;
    parse_buffer *p_buffer = &buffer;
    lite_cjson_t current_item_key;
    lite_cjson_t current_item_value;
    int index = 0;

    if (!lite || lite->type != cJSON_Object || !lite->value || lite->size == 0 || !key || key_len <= 0 || !lite_item) {
        return -1;
    };

    memset(&buffer, 0, sizeof(parse_buffer));
    buffer.content = (const unsigned char *)lite->value;
    buffer.length = lite->value_length;
    buffer.offset = 0;

    /* int start_pos = p_buffer->offset; */

    if (cannot_access_at_index(p_buffer, 0) || (buffer_at_offset(p_buffer)[0] != '{')) {
        return -1; /* not an object */
    }

    p_buffer->offset++;
    buffer_skip_whitespace(p_buffer);
    if (can_access_at_index(p_buffer, 0) && (buffer_at_offset(p_buffer)[0] == '}')) {
        return -1; /* empty object */
    }

    /* check if we skipped to the end of the buffer */
    if (cannot_access_at_index(p_buffer, 0)) {
        p_buffer->offset--;
        return -1;
    }

    /* step back to character in front of the first element */
    p_buffer->offset--;
    /* loop through the comma separated array elements */
    do {
        memset(&current_item_key, 0, sizeof(lite_cjson_t));
        memset(&current_item_value, 0, sizeof(lite_cjson_t));

        /* parse the name of the child */
        p_buffer->offset++;
        buffer_skip_whitespace(p_buffer);
        if (parse_string(&current_item_key, p_buffer) != 0) {
            return -1; /* faile to parse name */
        }
        buffer_skip_whitespace(p_buffer);

        if (cannot_access_at_index(p_buffer, 0) || (buffer_at_offset(p_buffer)[0] != ':')) {
            return -1; /* invalid object */
        }

        /* parse the value */
        p_buffer->offset++;
        buffer_skip_whitespace(p_buffer);
        if (parse_value(&current_item_value, p_buffer) != 0) {
            return -1; /* failed to parse value */
        }
        buffer_skip_whitespace(p_buffer);

        /* printf("Current Object [Index: %d], [Key Length: %d, Key Value: %.*s], [Value Length: %d, Value: %.*s]\n", */
        /* index + 1, current_item_key.value_length,current_item_key.value_length,current_item_key.value, */
        /* current_item_value.value_length,current_item_value.value_length,current_item_value.value); */
        index++;

        /* printf("key: %s, ken_len: %d\n",key,key_len); */
        if ((current_item_key.value_length == key_len) &&
            memcmp(current_item_key.value, key, key_len) == 0) {
            memcpy(lite_item, &current_item_value, sizeof(lite_cjson_t));
            return 0;
        }
    } while (can_access_at_index(p_buffer, 0) && (buffer_at_offset(p_buffer)[0] == ','));

    return -1;
}

static int _lite_cjson_key_array_index(const char *key, int key_len,
                                       int *partial_key_len, int *array_key_len, int *array_index)
{
    char *bracket_pre = NULL;
    char *bracket_suf = NULL;
    int index = 0;
    int deep = 0;
    char array_index_str[10] = {0};

    if (!key || !partial_key_len || !array_key_len || !array_index) {
        return -1;
    }

    for (index = 0; index < key_len; index++) {
        switch (key[index]) {
            case '[': {
                if (deep != 0) {
                    return -1;
                }
                deep++;
                if (!bracket_pre) {
                    bracket_pre = (char *)&key[index];
                }
            }
            break;
            case ']': {
                if (deep != 1) {
                    return -1;
                }
                deep--;
                if (key[index  - 1] == '[') {
                    return -1;
                }
                if (!bracket_suf) {
                    bracket_suf = (char *)&key[index];
                }
            }
            break;
            default:
                break;

        }
    }

    if (bracket_pre && bracket_suf && ((bracket_suf - key + 1) == key_len)) {
        *partial_key_len = bracket_pre - key;
        *array_key_len = bracket_suf - key + 1;

        /* Get Index */
        memcpy(array_index_str, bracket_pre + 1, bracket_suf - bracket_pre - 1);
        *array_index = atoi(array_index_str);
        return 0;
    }

    return -1;
}

int lite_cjson_object_item(lite_cjson_t *lite, const char *key, int key_len,
                           lite_cjson_t *lite_item)
{
    int res = 0;
    char *delim = NULL;
    lite_cjson_t lite_prev;
    lite_cjson_t lite_next;
    lite_cjson_t lite_iter;
    char *key_iter = (char *)key;
    int key_iter_len = 0;
    int partial_key_len = 0;
    int array_key_len = 0;
    int array_index = 0;

    if (!lite || lite->type != cJSON_Object || !lite->value || lite->size == 0 || !key || key_len <= 0 || !lite_item) {
        return -1;
    };

    memcpy(&lite_iter, lite, sizeof(lite_cjson_t));
    memset(&lite_prev, 0, sizeof(lite_cjson_t));

    do {
        if ((delim = strchr(key_iter, '.')) != NULL) {
            /* printf("delim exist,delim : %s\n",delim); */
            memset(&lite_next, 0, sizeof(lite_cjson_t));
            partial_key_len = array_key_len = array_index = 0;
            key_iter_len = (int)(delim - key_iter);
        } else {
            key_iter_len = key_len - (key_iter - key);
            /* printf("key: %s, last key: %s, key len: %d, last key len: %d\n",key, key_iter, key_len, key_iter_len); */
        }

        if (_lite_cjson_key_array_index(key_iter, key_iter_len,
                                        &partial_key_len, &array_key_len, &array_index) == 0) {

            /* printf("partial_key_len: %d, array_key_len: %d, array_index: %d\n", partial_key_len, array_key_len, array_index); */

            res = _lite_cjson_object_item(&lite_iter, key_iter, partial_key_len, &lite_prev);
            if (res || lite_prev.type != cJSON_Array) {
                return -1;
            }
            /* printf("current array: %.*s\n",lite_prev.value_length,lite_prev.value); */

            res = lite_cjson_array_item(&lite_prev, array_index, &lite_next);
            if (res) {
                return -1;
            }
            /* printf("current array item: %.*s\n",lite_next.value_length,lite_next.value); */

            memcpy(&lite_iter, &lite_next, sizeof(lite_cjson_t));
            key_iter += array_key_len + 1;
            /* printf("key_iter: %s\n",key_iter); */
        } else {
            res = _lite_cjson_object_item(&lite_iter, key_iter, key_iter_len, &lite_prev);
            if (res) {
                return -1;
            }
            /* printf("current object: %.*s\n",lite_prev.value_length,lite_prev.value); */

            memcpy(&lite_iter, &lite_prev, sizeof(lite_cjson_t));
            key_iter = delim + 1;
        }
    } while (delim);

    /* printf("final lite cjson value: %.*s\n",lite_iter.value_length,lite_iter.value); */
    memcpy(lite_item, &lite_iter, sizeof(lite_cjson_t));

    return 0;
}

int lite_cjson_object_item_by_index(lite_cjson_t *lite, int index, lite_cjson_t *lite_item_key,
                                    lite_cjson_t *lite_item_value)
{
    parse_buffer buffer;
    parse_buffer *p_buffer = &buffer;
    lite_cjson_t current_item_key;
    lite_cjson_t current_item_value;
    /* int start_pos = p_buffer->offset; */
    int item_index = 0;

    if (!lite || lite->type != cJSON_Object || !lite->value || lite->size == 0 || index < 0 || index >= lite->size) {
        return -1;
    };

    memset(&buffer, 0, sizeof(parse_buffer));
    buffer.content = (const unsigned char *)lite->value;
    buffer.length = lite->value_length;
    buffer.offset = 0;

    if (cannot_access_at_index(p_buffer, 0) || (buffer_at_offset(p_buffer)[0] != '{')) {
        return -1; /* not an object */
    }

    p_buffer->offset++;
    buffer_skip_whitespace(p_buffer);
    if (can_access_at_index(p_buffer, 0) && (buffer_at_offset(p_buffer)[0] == '}')) {
        return -1; /* empty object */
    }

    /* check if we skipped to the end of the buffer */
    if (cannot_access_at_index(p_buffer, 0)) {
        p_buffer->offset--;
        return -1;
    }

    /* step back to character in front of the first element */
    p_buffer->offset--;
    /* loop through the comma separated array elements */
    do {
        memset(&current_item_key, 0, sizeof(lite_cjson_t));
        memset(&current_item_value, 0, sizeof(lite_cjson_t));

        /* parse the name of the child */
        p_buffer->offset++;
        buffer_skip_whitespace(p_buffer);
        if (parse_string(&current_item_key, p_buffer) != 0) {
            return -1; /* faile to parse name */
        }
        buffer_skip_whitespace(p_buffer);

        if (cannot_access_at_index(p_buffer, 0) || (buffer_at_offset(p_buffer)[0] != ':')) {
            return -1; /* invalid object */
        }

        /* parse the value */
        p_buffer->offset++;
        buffer_skip_whitespace(p_buffer);
        if (parse_value(&current_item_value, p_buffer) != 0) {
            return -1; /* failed to parse value */
        }
        buffer_skip_whitespace(p_buffer);

        /* printf("Current Object [Index: %d], [Key Length: %d, Key Value: %.*s], [Value Length: %d, Value: %.*s]\n", */
        /* index + 1, current_item_key.value_length,current_item_key.value_length,current_item_key.value, */
        /* current_item_value.value_length,current_item_value.value_length,current_item_value.value); */

        /* printf("index:%d, key: %.*s, value: %.*s\n",index, */
        /* current_item_key.value_length,current_item_key.value, */
        /* current_item_value.value_length,current_item_value.value); */

        if (item_index == index) {
            if (lite_item_key) {
                memcpy(lite_item_key, &current_item_key, sizeof(lite_cjson_t));
            }
            if (lite_item_value) {
                memcpy(lite_item_value, &current_item_value, sizeof(lite_cjson_t));
            }
            return 0;
        }

        item_index++;
    } while (can_access_at_index(p_buffer, 0) && (buffer_at_offset(p_buffer)[0] == ','));

    return -1;
}

/*** cjson create, add and print ***/
#if defined(DEVICE_MODEL_GATEWAY) || defined(ALCS_ENABLED) || defined(DEPRECATED_LINKKIT)
#define true ((cJSON_bool)1)
#define false ((cJSON_bool)0)
#define cjson_min(a, b) ((a < b) ? a : b)

typedef struct internal_hooks {
    void *(*allocate)(uint32_t size);
    void (*deallocate)(void *pointer);
    void *(*reallocate)(void *pointer, size_t size);
} internal_hooks;

typedef struct {
    unsigned char *buffer;
    size_t length;
    size_t offset;
    size_t depth; /* current nesting depth (for formatted printing) */
    cJSON_bool noalloc;
    cJSON_bool format; /* is this print a formatted print */
    internal_hooks hooks;
} printbuffer;

void *HAL_Malloc(uint32_t size);
void HAL_Free(void *ptr);

static void *internal_malloc(uint32_t size)
{
    return HAL_Malloc(size);
}

static void internal_free(void *ptr)
{
    HAL_Free(ptr);
}

static internal_hooks global_hooks = { internal_malloc, internal_free, NULL };
static cJSON_bool print_value(const lite_cjson_item_t *const item, printbuffer *const output_buffer);

void lite_cjson_init_hooks(lite_cjson_hooks *hooks)
{
    if (hooks == NULL || hooks->malloc_fn == NULL || hooks->free_fn == NULL) {
        return;
    }

    global_hooks.allocate = hooks->malloc_fn;
    global_hooks.deallocate = hooks->free_fn;
}

static unsigned char *ensure(printbuffer *const p, size_t needed)
{
    unsigned char *newbuffer = NULL;
    size_t newsize = 0;

    if ((p == NULL) || (p->buffer == NULL)) {
        return NULL;
    }

    if ((p->length > 0) && (p->offset >= p->length)) {
        /* make sure that offset is valid */
        return NULL;
    }

    if (needed > INT_MAX) {
        /* sizes bigger than INT_MAX are currently not supported */
        return NULL;
    }

    needed += p->offset + 1;
    if (needed <= p->length) {
        return p->buffer + p->offset;
    }

    if (p->noalloc) {
        return NULL;
    }

    /* calculate new buffer size */
    if (needed > (INT_MAX / 2)) {
        /* overflow of int, use INT_MAX if possible */
        if (needed <= INT_MAX) {
            newsize = INT_MAX;
        } else {
            return NULL;
        }
    } else {
        newsize = needed * 2;
    }

    if (p->hooks.reallocate != NULL) {
        /* reallocate with realloc if available */
        newbuffer = (unsigned char *)p->hooks.reallocate(p->buffer, newsize);
        if (newbuffer == NULL) {
            p->hooks.deallocate(p->buffer);
            p->length = 0;
            p->buffer = NULL;

            return NULL;
        }
    } else {
        /* otherwise reallocate manually */
        newbuffer = (unsigned char *)p->hooks.allocate(newsize);
        if (!newbuffer) {
            p->hooks.deallocate(p->buffer);
            p->length = 0;
            p->buffer = NULL;

            return NULL;
        }
        if (newbuffer) {
            memcpy(newbuffer, p->buffer, p->offset + 1);
        }
        p->hooks.deallocate(p->buffer);
    }
    p->length = newsize;
    p->buffer = newbuffer;

    return newbuffer + p->offset;
}

/* get the decimal point character of the current locale */
static unsigned char get_decimal_point(void)
{
#ifdef ENABLE_LOCALES
    struct lconv *lconv = localeconv();
    return (unsigned char) lconv->decimal_point[0];
#else
    return '.';
#endif
}

static int remove_zero(unsigned char buffer[26], int length)
{
    int idx = 0, found = 0;

    for (idx = 0; idx < 26; idx++) {
        if (buffer[idx] == '.') {
            found = 1;
            continue;
        }
        if (buffer[idx] == '\0') {
            break;
        }
    }

    if (found == 0) {
        return length;
    }

    for (; idx > 0; idx--) {
        if (buffer[idx - 1] == '0') {
            buffer[idx - 1] = '\0';
            length--;
        } else {
            if (buffer[idx - 1] == '.') {
                buffer[idx - 1] = '\0';
                length--;
            }
            break;
        }
    }

    return length;
}

/* Render the number nicely from the given item into a string. */
static cJSON_bool print_number(const lite_cjson_item_t *const item, printbuffer *const output_buffer)
{
    unsigned char *output_pointer = NULL;
    double d = item->valuedouble;
    int length = 0;
    size_t i = 0;
    unsigned char number_buffer[26]; /* temporary buffer to print the number into */
    unsigned char decimal_point = get_decimal_point();
    float test_float;
    double test;

    if (output_buffer == NULL) {
        return false;
    }

    /* This checks for NaN and Infinity */
    if ((d * 0) != 0) {
        length = sprintf((char *)number_buffer, "null");
    } else {
        /* Try float data type */
        length = sprintf((char *)number_buffer, "%f", d);

        if ((sscanf((char *)number_buffer, "%f", &test_float) != 1) || ((double)test_float != d)) {
            /* Try 15 decimal places of precision to avoid nonsignificant nonzero digits */
            length = sprintf((char *)number_buffer, "%1.15g", d);

            /* Check whether the original double can be recovered */
            if ((sscanf((char *)number_buffer, "%lg", &test) != 1) || ((double)test != d)) {
                /* If not, print with 17 decimal places of precision */
                length = sprintf((char *)number_buffer, "%1.17g", d);
            }
        } else {
            length = remove_zero(number_buffer, length);
        }
    }

    /* sprintf failed or buffer overrun occured */
    if ((length < 0) || (length > (int)(sizeof(number_buffer) - 1))) {
        return false;
    }

    /* reserve appropriate space in the output */
    output_pointer = ensure(output_buffer, (size_t)length + sizeof(""));
    if (output_pointer == NULL) {
        return false;
    }

    /* copy the printed number to the output and replace locale
     * dependent decimal point with '.' */
    for (i = 0; i < ((size_t)length); i++) {
        if (number_buffer[i] == decimal_point) {
            output_pointer[i] = '.';
            continue;
        }

        output_pointer[i] = number_buffer[i];
    }
    output_pointer[i] = '\0';

    output_buffer->offset += (size_t)length;

    return true;
}

/* Render the cstring provided to an escaped version that can be printed. */
static cJSON_bool print_string_ptr(const unsigned char *const input, printbuffer *const output_buffer)
{
    const unsigned char *input_pointer = NULL;
    unsigned char *output = NULL;
    unsigned char *output_pointer = NULL;
    size_t output_length = 0;
    /* numbers of additional characters needed for escaping */
    size_t escape_characters = 0;

    if (output_buffer == NULL) {
        return false;
    }

    /* empty string */
    if (input == NULL) {
        output = ensure(output_buffer, sizeof("\"\""));
        if (output == NULL) {
            return false;
        }
        strcpy((char *)output, "\"\"");

        return true;
    }

    /* set "flag" to 1 if something needs to be escaped */
    for (input_pointer = input; *input_pointer; input_pointer++) {
        switch (*input_pointer) {
            case '\"':
            case '\\':
            case '\b':
            case '\f':
            case '\n':
            case '\r':
            case '\t':
                /* one character escape sequence */
                escape_characters++;
                break;
            default:
                if (*input_pointer < 32) {
                    /* UTF-16 escape sequence uXXXX */
                    escape_characters += 5;
                }
                break;
        }
    }
    output_length = (size_t)(input_pointer - input) + escape_characters;

    output = ensure(output_buffer, output_length + sizeof("\"\""));
    if (output == NULL) {
        return false;
    }

    /* no characters have to be escaped */
    if (escape_characters == 0) {
        output[0] = '\"';
        memcpy(output + 1, input, output_length);
        output[output_length + 1] = '\"';
        output[output_length + 2] = '\0';

        return true;
    }

    output[0] = '\"';
    output_pointer = output + 1;
    /* copy the string */
    for (input_pointer = input; *input_pointer != '\0'; (void)input_pointer++, output_pointer++) {
        if ((*input_pointer > 31) && (*input_pointer != '\"') && (*input_pointer != '\\')) {
            /* normal character, copy */
            *output_pointer = *input_pointer;
        } else {
            /* character needs to be escaped */
            *output_pointer++ = '\\';
            switch (*input_pointer) {
                case '\\':
                    *output_pointer = '\\';
                    break;
                case '\"':
                    *output_pointer = '\"';
                    break;
                case '\b':
                    *output_pointer = 'b';
                    break;
                case '\f':
                    *output_pointer = 'f';
                    break;
                case '\n':
                    *output_pointer = 'n';
                    break;
                case '\r':
                    *output_pointer = 'r';
                    break;
                case '\t':
                    *output_pointer = 't';
                    break;
                default:
                    /* escape and print as unicode codepoint */
                    sprintf((char *)output_pointer, "u%04x", *input_pointer);
                    output_pointer += 4;
                    break;
            }
        }
    }
    output[output_length + 1] = '\"';
    output[output_length + 2] = '\0';

    return true;
}

/* Invoke print_string_ptr (which is useful) on an item. */
static cJSON_bool print_string(const lite_cjson_item_t *const item, printbuffer *const p)
{
    return print_string_ptr((unsigned char *)item->valuestring, p);
}

/* calculate the new length of the string in a printbuffer and update the offset */
static void update_offset(printbuffer *const buffer)
{
    const unsigned char *buffer_pointer = NULL;
    if ((buffer == NULL) || (buffer->buffer == NULL)) {
        return;
    }
    buffer_pointer = buffer->buffer + buffer->offset;

    buffer->offset += strlen((const char *)buffer_pointer);
}

/* Render an array to text */
static cJSON_bool print_array(const lite_cjson_item_t *const item, printbuffer *const output_buffer)
{
    unsigned char *output_pointer = NULL;
    size_t length = 0;
    lite_cjson_item_t *current_element = item->child;

    if (output_buffer == NULL) {
        return false;
    }

    /* Compose the output array. */
    /* opening square bracket */
    output_pointer = ensure(output_buffer, 1);
    if (output_pointer == NULL) {
        return false;
    }

    *output_pointer = '[';
    output_buffer->offset++;
    output_buffer->depth++;

    while (current_element != NULL) {
        if (!print_value(current_element, output_buffer)) {
            return false;
        }
        update_offset(output_buffer);
        if (current_element->next) {
            length = (size_t)(output_buffer->format ? 2 : 1);
            output_pointer = ensure(output_buffer, length + 1);
            if (output_pointer == NULL) {
                return false;
            }
            *output_pointer++ = ',';
            if (output_buffer->format) {
                *output_pointer++ = ' ';
            }
            *output_pointer = '\0';
            output_buffer->offset += length;
        }
        current_element = current_element->next;
    }

    output_pointer = ensure(output_buffer, 2);
    if (output_pointer == NULL) {
        return false;
    }
    *output_pointer++ = ']';
    *output_pointer = '\0';
    output_buffer->depth--;

    return true;
}

/* Render an object to text. */
static cJSON_bool print_object(const lite_cjson_item_t *const item, printbuffer *const output_buffer)
{
    unsigned char *output_pointer = NULL;
    size_t length = 0;
    lite_cjson_item_t *current_item = item->child;

    if (output_buffer == NULL) {
        return false;
    }

    /* Compose the output: */
    length = (size_t)(output_buffer->format ? 2 : 1);  /* fmt: {\n */
    output_pointer = ensure(output_buffer, length + 1);
    if (output_pointer == NULL) {
        return false;
    }

    *output_pointer++ = '{';
    output_buffer->depth++;
    if (output_buffer->format) {
        *output_pointer++ = '\n';
    }
    output_buffer->offset += length;

    while (current_item) {
        if (output_buffer->format) {
            size_t i;
            output_pointer = ensure(output_buffer, output_buffer->depth);
            if (output_pointer == NULL) {
                return false;
            }
            for (i = 0; i < output_buffer->depth; i++) {
                *output_pointer++ = '\t';
            }
            output_buffer->offset += output_buffer->depth;
        }

        /* print key */
        if (!print_string_ptr((unsigned char *)current_item->string, output_buffer)) {
            return false;
        }
        update_offset(output_buffer);

        length = (size_t)(output_buffer->format ? 2 : 1);
        output_pointer = ensure(output_buffer, length);
        if (output_pointer == NULL) {
            return false;
        }
        *output_pointer++ = ':';
        if (output_buffer->format) {
            *output_pointer++ = '\t';
        }
        output_buffer->offset += length;

        /* print value */
        if (!print_value(current_item, output_buffer)) {
            return false;
        }
        update_offset(output_buffer);

        /* print comma if not last */
        length = (size_t)((output_buffer->format ? 1 : 0) + (current_item->next ? 1 : 0));
        output_pointer = ensure(output_buffer, length + 1);
        if (output_pointer == NULL) {
            return false;
        }
        if (current_item->next) {
            *output_pointer++ = ',';
        }

        if (output_buffer->format) {
            *output_pointer++ = '\n';
        }
        *output_pointer = '\0';
        output_buffer->offset += length;

        current_item = current_item->next;
    }

    output_pointer = ensure(output_buffer, output_buffer->format ? (output_buffer->depth + 1) : 2);
    if (output_pointer == NULL) {
        return false;
    }
    if (output_buffer->format) {
        size_t i;
        for (i = 0; i < (output_buffer->depth - 1); i++) {
            *output_pointer++ = '\t';
        }
    }
    *output_pointer++ = '}';
    *output_pointer = '\0';
    output_buffer->depth--;

    return true;
}

/* Render a value to text. */
static cJSON_bool print_value(const lite_cjson_item_t *const item, printbuffer *const output_buffer)
{
    unsigned char *output = NULL;

    if ((item == NULL) || (output_buffer == NULL)) {
        return false;
    }

    switch ((item->type) & 0xFF) {
        case cJSON_NULL:
            output = ensure(output_buffer, 5);
            if (output == NULL) {
                return false;
            }
            strcpy((char *)output, "null");
            return true;

        case cJSON_False:
            output = ensure(output_buffer, 6);
            if (output == NULL) {
                return false;
            }
            strcpy((char *)output, "false");
            return true;

        case cJSON_True:
            output = ensure(output_buffer, 5);
            if (output == NULL) {
                return false;
            }
            strcpy((char *)output, "true");
            return true;

        case cJSON_Number:
            return print_number(item, output_buffer);

        case cJSON_Raw: {
            size_t raw_length = 0;
            if (item->valuestring == NULL) {
                return false;
            }

            raw_length = strlen(item->valuestring) + sizeof("");
            output = ensure(output_buffer, raw_length);
            if (output == NULL) {
                return false;
            }
            memcpy(output, item->valuestring, raw_length);
            return true;
        }

        case cJSON_String:
            return print_string(item, output_buffer);

        case cJSON_Array:
            return print_array(item, output_buffer);

        case cJSON_Object:
            return print_object(item, output_buffer);

        default:
            return false;
    }
}

static unsigned char *print(const lite_cjson_item_t *const item, cJSON_bool format, const internal_hooks *const hooks)
{
    static const size_t default_buffer_size = 256;
    printbuffer buffer[1];
    unsigned char *printed = NULL;

    memset(buffer, 0, sizeof(buffer));

    /* create buffer */
    buffer->buffer = (unsigned char *) hooks->allocate(default_buffer_size);
    buffer->length = default_buffer_size;
    buffer->format = format;
    buffer->hooks = *hooks;
    if (buffer->buffer == NULL) {
        goto fail;
    }

    /* print the value */
    if (!print_value(item, buffer)) {
        goto fail;
    }
    update_offset(buffer);

    /* check if reallocate is available */
    if (hooks->reallocate != NULL) {
        printed = (unsigned char *) hooks->reallocate(buffer->buffer, buffer->offset + 1);
        if (printed == NULL) {
            goto fail;
        }
        buffer->buffer = NULL;
    } else { /* otherwise copy the JSON over to a new buffer */
        printed = (unsigned char *) hooks->allocate(buffer->offset + 1);
        if (printed == NULL) {
            goto fail;
        }
        memcpy(printed, buffer->buffer, cjson_min(buffer->length, buffer->offset + 1));
        printed[buffer->offset] = '\0'; /* just to be sure */

        /* free the buffer */
        hooks->deallocate(buffer->buffer);
    }

    return printed;

fail:
    if (buffer->buffer != NULL) {
        hooks->deallocate(buffer->buffer);
    }

    return NULL;
}

char  *lite_cjson_print(lite_cjson_item_t *item)
{
    return (char *)print(item, true, &global_hooks);
}

char  *lite_cjson_print_unformatted(lite_cjson_item_t *item)
{
    return (char *)print(item, false, &global_hooks);
}

/* Delete a cJSON structure. */
void lite_cjson_delete(lite_cjson_item_t *item)
{
    lite_cjson_item_t *next = NULL;
    while (item != NULL) {
        next = item->next;
        if (!(item->type & cJSON_IsReference) && (item->child != NULL)) {
            lite_cjson_delete(item->child);
        }
        if (!(item->type & cJSON_IsReference) && (item->valuestring != NULL)) {
            global_hooks.deallocate(item->valuestring);
        }
        if (!(item->type & cJSON_StringIsConst) && (item->string != NULL)) {
            global_hooks.deallocate(item->string);
        }
        global_hooks.deallocate(item);
        item = next;
    }
}

static void suffix_object(lite_cjson_item_t *prev, lite_cjson_item_t *item)
{
    prev->next = item;
    item->prev = prev;
}

static cJSON_bool add_item_to_array(lite_cjson_item_t *array, lite_cjson_item_t *item)
{
    lite_cjson_item_t *child = NULL;

    if ((item == NULL) || (array == NULL)) {
        return false;
    }

    child = array->child;

    if (child == NULL) {
        /* list is empty, start new one */
        array->child = item;
    } else {
        /* append to the end */
        while (child->next) {
            child = child->next;
        }
        suffix_object(child, item);
    }

    return true;
}

void lite_cjson_add_item_to_array(lite_cjson_item_t *array, lite_cjson_item_t *item)
{
    add_item_to_array(array, item);
}

static void *cast_away_const(const void *string)
{
    return (void *)string;
}

static unsigned char *cJSON_strdup(const unsigned char *string, const internal_hooks *const hooks)
{
    size_t length = 0;
    unsigned char *copy = NULL;

    if (string == NULL) {
        return NULL;
    }

    length = strlen((const char *)string) + sizeof("");
    copy = (unsigned char *)hooks->allocate(length);
    if (copy == NULL) {
        return NULL;
    }
    memcpy(copy, string, length);

    return copy;
}

static cJSON_bool add_item_to_object(lite_cjson_item_t *const object, const char *const string,
                                     lite_cjson_item_t *const item, const internal_hooks *const hooks, const cJSON_bool constant_key)
{
    char *new_key = NULL;
    int new_type = cJSON_Invalid;

    if ((object == NULL) || (string == NULL) || (item == NULL)) {
        return false;
    }

    if (constant_key) {
        new_key = (char *)cast_away_const(string);
        new_type = item->type | cJSON_StringIsConst;
    } else {
        new_key = (char *)cJSON_strdup((const unsigned char *)string, hooks);
        if (new_key == NULL) {
            return false;
        }

        new_type = item->type & ~cJSON_StringIsConst;
    }

    if (!(item->type & cJSON_StringIsConst) && (item->string != NULL)) {
        hooks->deallocate(item->string);
    }

    item->string = new_key;
    item->type = new_type;

    return add_item_to_array(object, item);
}

void lite_cjson_add_item_to_object(lite_cjson_item_t *object, const char *string, lite_cjson_item_t *item)
{
    add_item_to_object(object, string, item, &global_hooks, false);
}

static lite_cjson_item_t *cJSON_New_Item(const internal_hooks *const hooks)
{
    lite_cjson_item_t *node = (lite_cjson_item_t *)hooks->allocate(sizeof(lite_cjson_item_t));
    if (node) {
        memset(node, '\0', sizeof(lite_cjson_item_t));
    }

    return node;
}

lite_cjson_item_t *lite_cjson_create_null(void)
{
    lite_cjson_item_t *item = cJSON_New_Item(&global_hooks);
    if (item) {
        item->type = cJSON_NULL;
    }

    return item;
}

lite_cjson_item_t *lite_cjson_create_true(void)
{
    lite_cjson_item_t *item = cJSON_New_Item(&global_hooks);
    if (item) {
        item->type = cJSON_True;
    }

    return item;
}

lite_cjson_item_t *lite_cjson_create_false(void)
{
    lite_cjson_item_t *item = cJSON_New_Item(&global_hooks);
    if (item) {
        item->type = cJSON_False;
    }

    return item;
}

lite_cjson_item_t *lite_cjson_create_bool(cJSON_bool b)
{
    lite_cjson_item_t *item = cJSON_New_Item(&global_hooks);
    if (item) {
        item->type = b ? cJSON_True : cJSON_False;
    }

    return item;
}

lite_cjson_item_t *lite_cjson_create_number(double num)
{
    lite_cjson_item_t *item = cJSON_New_Item(&global_hooks);
    if (item) {
        item->type = cJSON_Number;
        item->valuedouble = num;

        /* use saturation in case of overflow */
        if (num >= INT_MAX) {
            item->valueint = INT_MAX;
        } else if (num <= INT_MIN) {
            item->valueint = INT_MIN;
        } else {
            item->valueint = (int)num;
        }
    }

    return item;
}
lite_cjson_item_t *lite_cjson_create_string(const char *string)
{
    lite_cjson_item_t *item = cJSON_New_Item(&global_hooks);
    if (item) {
        item->type = cJSON_String;
        item->valuestring = (char *)cJSON_strdup((const unsigned char *)string, &global_hooks);
        if (!item->valuestring) {
            lite_cjson_delete(item);
            return NULL;
        }
    }

    return item;
}

lite_cjson_item_t *lite_cjson_create_array(void)
{
    lite_cjson_item_t *item = cJSON_New_Item(&global_hooks);
    if (item) {
        item->type = cJSON_Array;
    }

    return item;
}

lite_cjson_item_t *lite_cjson_create_object(void)
{
    lite_cjson_item_t *item = cJSON_New_Item(&global_hooks);
    if (item) {
        item->type = cJSON_Object;
    }

    return item;
}

lite_cjson_item_t *lite_cjson_create_intArray(const int *numbers, int count)
{
    size_t i = 0;
    lite_cjson_item_t *n = NULL;
    lite_cjson_item_t *p = NULL;
    lite_cjson_item_t *a = NULL;

    if ((count < 0) || (numbers == NULL)) {
        return NULL;
    }

    a = lite_cjson_create_array();
    for (i = 0; a && (i < (size_t)count); i++) {
        n = lite_cjson_create_number(numbers[i]);
        if (!n) {
            lite_cjson_delete(a);
            return NULL;
        }
        if (!i) {
            a->child = n;
        } else {
            suffix_object(p, n);
        }
        p = n;
    }

    return a;
}

lite_cjson_item_t *lite_cjson_create_floatArray(const float *numbers, int count)
{
    size_t i = 0;
    lite_cjson_item_t *n = NULL;
    lite_cjson_item_t *p = NULL;
    lite_cjson_item_t *a = NULL;

    if ((count < 0) || (numbers == NULL)) {
        return NULL;
    }

    a = lite_cjson_create_array();

    for (i = 0; a && (i < (size_t)count); i++) {
        n = lite_cjson_create_number((double)numbers[i]);
        if (!n) {
            lite_cjson_delete(a);
            return NULL;
        }
        if (!i) {
            a->child = n;
        } else {
            suffix_object(p, n);
        }
        p = n;
    }

    return a;
}

lite_cjson_item_t *lite_cjson_create_doubleArray(const double *numbers, int count)
{
    size_t i = 0;
    lite_cjson_item_t *n = NULL;
    lite_cjson_item_t *p = NULL;
    lite_cjson_item_t *a = NULL;

    if ((count < 0) || (numbers == NULL)) {
        return NULL;
    }

    a = lite_cjson_create_array();

    for (i = 0; a && (i < (size_t)count); i++) {
        n = lite_cjson_create_number(numbers[i]);
        if (!n) {
            lite_cjson_delete(a);
            return NULL;
        }
        if (!i) {
            a->child = n;
        } else {
            suffix_object(p, n);
        }
        p = n;
    }

    return a;
}

lite_cjson_item_t *lite_cjson_create_stringArray(const char **strings, int count)
{
    size_t i = 0;
    lite_cjson_item_t *n = NULL;
    lite_cjson_item_t *p = NULL;
    lite_cjson_item_t *a = NULL;

    if ((count < 0) || (strings == NULL)) {
        return NULL;
    }

    a = lite_cjson_create_array();

    for (i = 0; a && (i < (size_t)count); i++) {
        n = lite_cjson_create_string(strings[i]);
        if (!n) {
            lite_cjson_delete(a);
            return NULL;
        }
        if (!i) {
            a->child = n;
        } else {
            suffix_object(p, n);
        }
        p = n;
    }

    return a;
}
#endif
#endif

