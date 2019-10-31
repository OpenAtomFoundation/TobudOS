/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#ifndef _DM_UTILS_H_
#define _DM_UTILS_H_

#define DM_UTILS_UINT16_STRLEN (5)
#define DM_UTILS_UINT32_STRLEN (10)
#define DM_UTILS_UINT64_STRLEN (20)

int dm_utils_copy_direct(void *input, int input_len, void **output, int output_len);

int dm_utils_copy(void *input, int input_len, void **output, int output_len);

/**
 * @brief search array index in a string.
 *        This function used to search array index in a string.
 *
 * @param input. The string to be searched
 * @param input_len. The length of input
 * @param partial_input_len. The length of input except [xx]
 * @param array_input_len. The length of input include [xx]
 * @param array_index. The array index in [xx]
 *
 * @warning input must be type of "xxxxx[xx]"
 * @return success or fail.
 *
 */
int dm_utils_strarr_index(char *input, int input_len,
                          int *partial_input_len, int *array_input_len, int *array_index);

int dm_utils_itoa_direct(int input, char **output);
int dm_utils_itoa(int input, char **output);
int dm_utils_ftoa_direct(double input, char **output);
int dm_utils_ftoa(double input, char **output);
int dm_utils_hex_to_str(unsigned char *input, int input_len, char **output);
int dm_utils_str_to_hex(char *input, int input_len, unsigned char **output, int *output_len);
int dm_utils_memtok(char *input, int input_len, char delimiter, int index, int *offset);
int dm_utils_replace_char(char *input, int input_len, char src, char dest);
int dm_utils_service_name(const char *prefix, const char *name, char product_key[IOTX_PRODUCT_KEY_LEN + 1],
                          char device_name[IOTX_DEVICE_NAME_LEN + 1], char **service_name);
int dm_utils_uri_add_prefix(const char *prefix, char *uri, char **new_uri);
int dm_utils_json_parse(const char *payload, int payload_len, int type, lite_cjson_t *lite);
int dm_utils_json_object_item(lite_cjson_t *lite, const char *key, int key_len, int type,
                              lite_cjson_t *lite_item);
void *dm_utils_malloc(unsigned int size);
void dm_utils_free(void *ptr);
#endif
