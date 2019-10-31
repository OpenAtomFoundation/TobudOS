/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */




#ifndef _INFRA_JSON_PARSER_H_
#define _INFRA_JSON_PARSER_H_

/* #include "iotx_utils_internal.h" */

typedef struct JSON_NV {
    int         nLen;
    int         vLen;
    int         vType;
    char       *pN;
    char       *pV;
} JSON_NV;

/**
The descriptions of the json value node type
**/
enum JSONTYPE {
    JNONE = -1,
    JSTRING = 0,
    JOBJECT,
    JARRAY,
    JNUMBER,
    JBOOLEAN,
    JTYPEMAX
};

/**
The error codes produced by the JSON parsers
**/
enum JSON_PARSE_CODE {
    JSON_PARSE_ERR,
    JSON_PARSE_OK,
    JSON_PARSE_FINISH
};

/**
The return codes produced by the JSON parsers
**/
enum JSON_PARSE_RESULT {
    JSON_RESULT_ERR = -1,
    JSON_RESULT_OK
};

typedef int (*json_parse_cb)(char *p_cName, int iNameLen, char *p_cValue, int iValueLen, int iValueType,
                             void *p_Result);

/**
* @brief Parse the JSON string, and iterate through all keys and values,
* then handle the keys and values by callback function.
*
* @param[in]  p_cJsonStr @n  The JSON string
* @param[in]  iStrLen    @n  The JSON string length
* @param[in]  pfnCB      @n  Callback function
* @param[out] p_CBData   @n  User data
* @return JSON_RESULT_OK success, JSON_RESULT_ERR failed
* @see None.
* @note None.
**/
int json_parse_name_value(char *p_cJsonStr, int iStrLen, json_parse_cb pfnCB, void *p_CBData);

/**
* @brief Get the value by a specified key from a json string
*
* @param[in]  p_cJsonStr   @n the JSON string
* @param[in]  iStrLen      @n the JSON string length
* @param[in]  p_cName      @n the specified key string
* @param[out] p_iValueLen  @n the value length
* @param[out] p_iValueType @n the value type
* @return A pointer to the value
* @see None.
* @note None.
**/
char *json_get_value_by_name(char *p_cJsonStr, int iStrLen, char *p_cName, int *p_iValueLen, int *p_iValueType);

/**
* @brief Get the value by a specified key from a json string
*
* @param[in]  p_cJsonStr   @n the JSON string
* @param[in]  iStrLen      @n the JSON string length
* @param[in]  p_cName      @n the specified key string
* @param[in]  p_cNameLen   @n the specified key string length
* @param[out] p_iValueLen  @n the value length
* @param[out] p_iValueType @n the value type
* @return A pointer to the value
* @see None.
* @note None.
**/
char *json_get_value_by_name_len(char *p_cJsonStr, int iStrLen, char *p_cName, int p_cNameLen, int *p_iValueLen,
                                 int *p_iValueType);

/**
 * @brief Get the JSON object point associate with a given type.
 *
 * @param[in] type      @n The object type
 * @param[in] str       @n The JSON string
 * @param[in] str_end   @n The end point of Json string
 * @returns The json object point with the given field type.
 * @see None.
 * @note None.
 */
char *json_get_object(int type, char *str, char *str_end);
char *json_get_next_object(int type, char *str, char *str_end, char **key, int *key_len, char **val, int *val_len,
                           int *val_type);

/**
 * @brief retrieve each key&value pair from the json string
 *
 * @param[in]  str   @n Json string to revolve
 * @param[in]  slen  @n The length of json string
 * @param[in]  pos   @n cursor
 * @param[out] key   @n pointer to the next Key object
 * @param[out] klen  @n Key object length
 * @param[out] val   @n pointer to the next Value object
 * @param[out] vlen  @n Value object length
 * @param[out] vtype @n Value object type(digital, string, object, array)
 * @see None.
 * @note None.
 */
#define json_object_for_each_kv(str, slen, pos, key, klen, val, vlen, vtype) \
    for (pos = json_get_object(JOBJECT, str, str + slen); \
         pos != NULL && *pos!= 0 && (pos=json_get_next_object(JOBJECT, pos, str + slen , &key, &klen, &val, &vlen, &vtype))!=0; )

/**
 * @brief retrieve each entry from the json array
 *
 * @param[in]  str   @n Json array to revolve
 * @param[in]  slen  @n the length of Json array
 * @param[in]  pos   @n cursor
 * @param[out] entry @n pointer to the next entry from the array
 * @param[out] len   @n entry length
 * @param[out] type  @n entry type(digital, string, object, array)
 * @see None.
 * @note None.
 */
#define json_array_for_each_entry(str, slen, pos, entry, len, type) \
    for (pos = json_get_object(JARRAY, str, str + slen); \
         pos != NULL && *pos!= 0 && (pos=json_get_next_object(JARRAY, ++pos, str + slen, 0, 0, &entry, &len, &type))!=0; )


/**
 * @brief backup the last character to register parameters,
 *          and set the end character with '\0'
 *
 * @param[in]  json_str @n json string
 * @param[in]  str_len  @n json string lenth
 * @param[out] register @n used to backup the last character
 * @see None.
 * @note None.
 */
#define backup_json_str_last_char(json_str, str_len, register) { \
        register = *((char *)json_str + str_len); \
        *((char *)json_str + str_len) = '\0';   \
    }

/**
 * @brief restore the last character from register parameters
 *
 * @param[in]  json_str @n json string
 * @param[in]  str_len  @n json string lenth
 * @param[in] register  @n used to restore the last character
 * @see None.
 * @note None.
 */
#define restore_json_str_last_char(json_str, str_len, register) { \
        *((char *)json_str + str_len) = register;    \
    }

char *LITE_json_value_of(char *key, char *src, ...);

#endif  /* __JSON_PARSER_H__ */


