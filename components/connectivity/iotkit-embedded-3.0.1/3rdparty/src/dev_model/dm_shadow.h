/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */


#if defined(DEPRECATED_LINKKIT)
#ifndef _DM_SHADOW_H_
#define _DM_SHADOW_H_

#include "iotx_dm_internal.h"

#define DM_SHW_KEY_SCHEMA                     "schema"
#define DM_SHW_KEY_LINK                       "link"
#define DM_SHW_KEY_PROFILE                    "profile"
#define DM_SHW_KEY_PROPERTIES                 "properties"
#define DM_SHW_KEY_EVENTS                     "events"
#define DM_SHW_KEY_SERVICES                   "services"

#define DM_SHW_KEY_PROFILE_PK                 "productKey"
#define DM_SHW_KEY_PROFILE_DN                 "deviceName"
#define DM_SHW_KEY_IDENTIFIER                 "identifier"
#define DM_SHW_KEY_NAME                       "name"
#define DM_SHW_KEY_DESC                       "desc"
#define DM_SHW_KEY_ACCESS_MODE                "accessMode"
#define DM_SHW_KEY_REQUIRED                   "required"
#define DM_SHW_KEY_METHOD                     "method"
#define DM_SHW_KEY_CALLTYPE                   "callType"
#define DM_SHW_KEY_OUTPUTDATA                 "outputData"
#define DM_SHW_KEY_INPUTDATA                  "inputData"
#define DM_SHW_KEY_DATATYPE                   "dataType"
#define DM_SHW_KEY_TYPE                       "type"
#define DM_SHW_KEY_SPECS                      "specs"
#define DM_SHW_KEY_UNIT                       "unit"
#define DM_SHW_KEY_UNITNAME                   "unitName"
#define DM_SHW_KEY_MIN                        "min"
#define DM_SHW_KEY_MAX                        "max"
#define DM_SHW_KEY_LENGTH                     "length"
#define DM_SHW_KEY_SIZE                       "size"
#define DM_SHW_KEY_ITEM                       "item"

/* Special Service And Event */
#define DM_SHW_SPECIAL_SERVICE_SET_IDENTIFIER "set"
#define DM_SHW_SPECIAL_SERVICE_SET_METHOD     "thing.service.property.set"
#define DM_SHW_SPECIAL_SERVICE_GET_IDENTIFIER "get"
#define DM_SHW_SPECIAL_SERVICE_GET_METHOD     "thing.service.property.get"
#define DM_SHW_SPECIAL_EVENT_POST_IDENTIFIER  "post"
#define DM_SHW_SPECIAL_EVENT_POST_METHOD      "thing.event.property.post"

#define DM_SHW_KEY_DELIMITER                  '.'

typedef enum {
    DM_SHW_DATA_TYPE_NONE,                    /* none */
    DM_SHW_DATA_TYPE_INT,                     /* int */
    DM_SHW_DATA_TYPE_FLOAT,                   /* float */
    DM_SHW_DATA_TYPE_DOUBLE,                  /* double */
    DM_SHW_DATA_TYPE_TEXT,                    /* string */
    DM_SHW_DATA_TYPE_ENUM,                    /* int */
    DM_SHW_DATA_TYPE_DATE,                    /* string */
    DM_SHW_DATA_TYPE_BOOL,                    /* bool,0 or 1 */
    DM_SHW_DATA_TYPE_ARRAY,                   /* support int, float, double, text */
    DM_SHW_DATA_TYPE_STRUCT,                  /* support above 8 data types */
} dm_shw_data_type_e;

typedef enum {
    DM_SHW_DATA_TARGET_SERVICE_INPUT_DATA,
    DM_SHW_DATA_TARGET_SERVICE_OUTPUT_DATA
} dm_shw_data_target_e;

typedef struct {
    dm_shw_data_type_e type;
    int size;
    void *value;
} dm_shw_data_value_complex_t;

typedef struct {
    dm_shw_data_type_e type;
    union {
        int value_int;
        float value_float;
        double value_double;
        void *value;                             /* string or complex type accroding to data type */
    };
} dm_shw_data_value_t;

typedef struct {
    dm_shw_data_type_e type;
    int specs_number;                            /* used when type is enum and struct */
    void *specs;                                 /* nerver be used by struct */
} dm_shw_data_type_t;

typedef struct {
    char *identifier;
    dm_shw_data_value_t data_value;
} dm_shw_data_t;

typedef struct {
    char *identifier;
    int input_data_number;                       /* input_data Number */
    dm_shw_data_t *input_datas;               /* input_data array, type is dm_shw_data_t */
    int output_data_number;                      /* ouput_data Number */
    dm_shw_data_t *output_datas;              /* output_data array, type is dm_shw_data_t */
} dm_shw_event_t;

typedef struct {
    char *identifier;                            /* synchronized or asynchronized */
    int input_data_number;                       /* input_data_number */
    dm_shw_data_t *input_datas;               /* input_data array, type is dm_shw_data_t */
    int output_data_number;                      /* ouput_data Number */
    dm_shw_data_t *output_datas;              /* output_data array, type is dm_shw_data_t */
} dm_shw_service_t;

typedef struct {
    int property_number;
    dm_shw_data_t *properties;                /* property array, type is dm_shw_data_t */
    int event_number;
    dm_shw_event_t *events;                   /* event array, type is dm_shw_event_t */
    int service_number;
    dm_shw_service_t *services;               /* service array, type is dm_shw_service_t */
} dm_shw_t;

/**
 * @brief Create TSL struct from TSL string.
 *        This function used to parse TSL string into TSL struct.
 *
 * @param tsl. The TSL string in JSON format.
 * @param tsl_len. The length of tsl
 * @param shadow. The pointer of TSL Struct pointer, will be malloc memory.
 *                This memory should be free by dm_shw_destroy.
 *
 * @return success or fail.
 *
 */
int dm_shw_create(_IN_ iotx_dm_tsl_type_t type, _IN_ const char *tsl, _IN_ int tsl_len, _OU_ dm_shw_t **shadow);

/**
 * @brief Get property from TSL struct.
 *        This function used to get property from TSL struct.
 *
 * @param shadow. The pointer of TSL Struct.
 * @param key. The property compound string, format decided by data type of property as follows:
 *        int,float,double,text,enum,date,bool type: property_id
 *        array type: property_id(array)[index]
 *        struct type: property_id(struct).property_id or property_id(struct).property_id[index]
 *
 * @param key_len. The length of key.
 * @param property. The property in TSL Struct.
 *
 * @return success or fail.
 *
 */
int dm_shw_get_property_data(_IN_ dm_shw_t *shadow, _IN_ char *key, _IN_ int key_len, _OU_ void **data);

int dm_shw_get_service_input_output_data(_IN_ dm_shw_data_target_e type, _IN_ dm_shw_t *shadow, _IN_ char *key,
        _IN_ int key_len, _OU_ void **data);

/**
 * @brief Get event output data from TSL struct.
 *        This function used to get event output data from TSL struct.
 *
 * @param shadow. The pointer of TSL Struct.
 * @param key. The property compound string, format decided by data type of property as follows:
 *        int,float,double,text,enum,date,bool type: property_id
 *        array type: property_id(array)[index]
 *        struct type: property_id(struct).property_id or property_id(struct).property_id[index]
 *
 * @param key_len. The length of key.
 * @param property. The property in TSL Struct.
 *
 * @return success or fail.
 *
 */
int dm_shw_get_event_output_data(_IN_ dm_shw_t *shadow, _IN_ char *key, _IN_ int key_len, _OU_ void **data);

/**
 * @brief Get property type from TSL struct.
 *        This function used to get property type from TSL struct.
 *
 * @param property. The handle of property.
 * @param type. The data type of property
 *
 *
 * @return success or fail.
 *
 */
int dm_shw_get_data_type(_IN_ void *data, _OU_ dm_shw_data_type_e *type);

/**
 * @brief Get event from TSL struct.
 *        This function used to get property from TSL struct.
 *
 * @param service. The handle of event.
 * @param key. The property compound string, format decided by data type of property as follows:
 *        int,float,double,text,enum,date,bool type: event_id
 *
 * @param key_len. The length of key.
 * @param property. The event in TSL Struct.
 *
 * @return success or fail.
 *
 */
int dm_shw_get_event(_IN_ dm_shw_t *shadow, _IN_ char *key, _IN_ int key_len, _OU_ void **event);

/**
 * @brief Get service from TSL struct.
 *        This function used to get property from TSL struct.
 *
 * @param shadow. The pointer of TSL Struct.
 * @param key. The property compound string, format decided by data type of property as follows:
 *        int,float,double,text,enum,date,bool type: service_id
 *
 * @param key_len. The length of key.
 * @param property. The service in TSL Struct.
 *
 * @return success or fail.
 *
 */
int dm_shw_get_service(_IN_ dm_shw_t *shadow, _IN_ char *key, _IN_ int key_len, _OU_ void **service);

/**
 * @brief Get property number from TSL struct.
 *        This function used to get property number from TSL struct.
 *
 * @param shadow. The pointer of TSL Struct.
 * @param number. The property number in TSL Struct.
 *
 * @return success or fail.
 *
 */
int dm_shw_get_property_number(_IN_ dm_shw_t *shadow, _OU_ int *number);

/**
 * @brief Get service number from TSL struct.
 *        This function used to get property number from TSL struct.
 *
 * @param shadow. The pointer of TSL Struct.
 * @param number. The service number in TSL Struct.
 *
 * @return success or fail.
 *
 */
int dm_shw_get_service_number(_IN_ dm_shw_t *shadow, _OU_ int *number);

/**
 * @brief Get event number from TSL struct.
 *        This function used to get property number from TSL struct.
 *
 * @param shadow. The pointer of TSL Struct.
 * @param number. The event number in TSL Struct.
 *
 * @return success or fail.
 *
 */
int dm_shw_get_event_number(_IN_ dm_shw_t *shadow, _OU_ int *number);

/**
 * @brief Get property reference from TSL struct by index.
 *        This function used to get property reference from TSL struct by index.
 *
 * @param shadow. The pointer of TSL Struct.
 * @param index. The index of property
 * @param property. The property reference in TSL Struct.
 *
 * @return success or fail.
 *
 */
int dm_shw_get_property_by_index(_IN_ dm_shw_t *shadow, _IN_ int index, _OU_ void **property);

/**
 * @brief Get service reference from TSL struct by index.
 *        This function used to get service reference from TSL struct by index.
 *
 * @param shadow. The pointer of TSL Struct.
 * @param index. The index of service
 * @param service. The service reference in TSL Struct.
 *
 * @return success or fail.
 *
 */
int dm_shw_get_service_by_index(_IN_ dm_shw_t *shadow, _IN_ int index, _OU_ void **service);

/**
 * @brief Get event reference from TSL struct by index.
 *        This function used to get event reference from TSL struct by index.
 *
 * @param shadow. The pointer of TSL Struct.
 * @param index. The index of event
 * @param event. The event reference in TSL Struct.
 *
 * @return success or fail.
 *
 */
int dm_shw_get_event_by_index(_IN_ dm_shw_t *shadow, _IN_ int index, _OU_ void **event);

/**
 * @brief Get service reference from TSL struct by identifier.
 *        This function used to get service reference from TSL struct by identifier.
 *
 * @param shadow. The pointer of TSL Struct.
 * @param identifier. The identifier of event
 * @param service. The service reference in TSL Struct.
 *
 * @return success or fail.
 *
 */
int dm_shw_get_service_by_identifier(_IN_ dm_shw_t *shadow, _IN_ char *identifier, _OU_ void **service);

/**
 * @brief Get event reference from TSL struct by identifier.
 *        This function used to get event reference from TSL struct by identifier.
 *
 * @param shadow. The pointer of TSL Struct.
 * @param identifier. The identifier of event
 * @param event. The event reference in TSL Struct.
 *
 * @return success or fail.
 *
 */
int dm_shw_get_event_by_identifier(_IN_ dm_shw_t *shadow, _IN_ char *identifier, _OU_ void **event);

/**
 * @brief Get property identifier from TSL struct by service handle.
 *        This function used to get property identifier from TSL struct by service handle.
 *
 * @param service. The handle of property.
 * @param method. The reference to property identifier in TSL Struct
 *
 * @return success or fail.
 *
 */
int dm_shw_get_property_identifier(_IN_ void *property, _OU_ char **identifier);

/**
 * @brief Get service method from TSL struct by service handle.
 *        This function used to get service method from TSL struct by service handle.
 *
 * @param service. The handle of service.
 * @param method. Generate method from service identifier
 *
 * @return success or fail.
 *
 */
int dm_shw_get_service_method(_IN_ void *service, _OU_ char **method);

/**
 * @brief Get event method from TSL struct by event handle.
 *        This function used to get event method from TSL struct by event handle.
 *
 * @param service. The handle of event.
 * @param method. Generate method from event identifier
 *
 * @return success or fail.
 *
 */
int dm_shw_get_event_method(_IN_ void *event, _OU_ char **method);

/**
 * @brief Set Property Value Into TSL Struct.
 *        This function used to set property value into TSL Struct.
 *
 * @param tsl. The pointer of TSL Struct.
 * @param key. The property compound string, format decided by data type of property as follows:
 *        int,float,double,text,enum,date,bool type: property_id
 *        array type: property_id(array)[index]
 *        struct type: property_id(struct).property_id or property_id(struct).property_id[index]
 *
 * @param key_len. The length of key
 * @param value. The value to be set, data type decided by data type of property as follows:
 *        int: int*, float: float*, double: double*,
 *        text: char*, enum: int*, date: char*, bool: int*
 *        attention! value can be NULL to clear property value
 * @param value_len. The length of value, only be used when type is text or data
 *
 * @return success or fail.
 *
 */
int dm_shw_set_property_value(_IN_ dm_shw_t *shadow, _IN_ char *key, _IN_ int key_len, _IN_ void *value,
                              _IN_ int value_len);

/**
 * @brief Get Property Value From TSL Struct.
 *        This function used to get property value from TSL Struct.
 *
 * @param tsl. The pointer of TSL Struct.
 * @param key. The property compound string, format decided by data type of property as follows:
 *        int,float,double,text,enum,date,bool type: property_id
 *        array type: property_id(array)[index]
 *        struct type: property_id(struct).property_id or property_id(struct).property_id[index]
 *
 * @param key_len. The length of key
 * @param value. The variable to store value, data type decided by data type of property as follows:
 *        int: int*, float: float*, double: double*,
 *        text: char**, enum: int*, date: char**, bool: int*
 *        attention! value can not be NULL
 *
 * @warning if data type is text or date, *value well be end with '\0'.
 *          the memory allocated to *value must be free by user.
 *
 * @return success or fail.
 *
 */
int dm_shw_get_property_value(_IN_ dm_shw_t *shadow, _IN_ char *key, _IN_ int key_len, _OU_ void *value);

/**
 * @brief Set event output value into TSL struct.
 *        This function used to set event output value into TSL struct.
 *
 * @param tsl. The pointer of TSL Struct.
 * @param key. The property compound string, format decided by data type of property as follows:
 *        int,float,double,text,enum,date,bool type: event_id.event_data_id
 *        array type: event_id.event_data_id(array)[index]
 *        struct type: event_id.event_data_id(struct).property_id
 *                     or event_id.event_data_id(struct).property_id[index]
 *
 * @param key_len. The length of key
 * @param value. The value to be set, data type decided by data type of property as follows:
 *        int: int*, float: float*, double: double*,
 *        text: char*, enum: int*, date: char*, bool: int*
 *        attention! value can be NULL to clear property value
 * @param value_len. The length of value, only be used when type is text or data
 *
 * @return success or fail.
 *
 */
int dm_shw_set_event_output_value(_IN_ dm_shw_t *shadow, _IN_ char *key, _IN_ int key_len, _IN_ void *value,
                                  _IN_ int value_len);

/**
 * @brief Get event output value from TSL struct.
 *        This function used to get event output value from TSL struct.
 *
 * @param tsl. The pointer of TSL Struct.
 * @param key. The property compound string, format decided by data type of property as follows:
 *        int,float,double,text,enum,date,bool type: event_id.event_data_id
 *        array type: event_id.event_data_id(array)[index]
 *        struct type: event_id.event_data_id(struct).property_id
 *                     or event_id.event_data_id(struct).property_id[index]
 *
 * @param key_len. The length of key
 * @param value. The variable to store value, data type decided by data type of property as follows:
 *        int: int*, float: float*, double: double*,
 *        text: char**, enum: int*, date: char**, bool: int*
 *        attention! value can not be NULL
 *
 * @warning if data type is text or date, *value well be end with '\0'.
 *          the memory allocated to *value must be free by user.
 *
 * @return success or fail.
 *
 */
int dm_shw_get_event_output_value(_IN_ dm_shw_t *shadow, _IN_ char *key, _IN_ int key_len, _IN_ void *value);

int dm_shw_set_service_input_output_value(_IN_ dm_shw_data_target_e type, _IN_ dm_shw_t *shadow, _IN_ char *key,
        _IN_ int key_len, _IN_ void *value, _IN_ int value_len);
int dm_shw_get_service_input_output_value(_IN_ dm_shw_data_target_e type, _IN_ dm_shw_t *shadow, _IN_ char *key,
        _IN_ int key_len, _IN_ void *value);

/**
 * @brief Get property payload from TSL struct.
 *        This function used to get property payload from TSL struct.
 *
 * @param shadow. The pointer of TSL Struct
 * @param identifier. The Property Identifier
 * @param identifier_len. The Property Identifier Length
 * @param lite. The pointer to json array where to store property value
 *
 * @warning The payload malloc by this function and need to be free manully.
 *
 * @return success or fail.
 *
 */
int dm_shw_assemble_property(_IN_ dm_shw_t *shadow, _IN_ char *identifier, _IN_ int identifier_len,
                             _IN_ lite_cjson_item_t *lite);

/**
 * @brief Get event output payload from TSL struct.
 *        This function used to get event output payload from TSL struct.
 *
 * @param shadow. The pointer of TSL Struct
 * @param identifier. The Event Identifier
 * @param identifier_len. The Event Identifier Length
 * @param lite. The pointer to json array where to store event output value
 *
 * @warning The payload malloc by this function and need to be free manully.
 *
 * @return success or fail.
 *
 */
int dm_shw_assemble_event_output(_IN_ dm_shw_t *shadow, _IN_ char *identifier, _IN_ int identifier_len,
                                 _IN_ lite_cjson_item_t *lite);

/**
 * @brief Get service output payload from TSL struct.
 *        This function used to get service output payload from TSL struct.
 *
 * @param shadow. The pointer of TSL Struct
 * @param identifier. The Service Identifier
 * @param identifier_len. The Service Identifier Length
 * @param lite. The pointer to json array where to store service output value
 *
 * @warning The payload malloc by this function and need to be free manully.
 *
 * @return success or fail.
 *
 */
int dm_shw_assemble_service_output(_IN_ dm_shw_t *shadow, _IN_ char *identifier, _IN_ int identifier_len,
                                   _IN_ lite_cjson_item_t *lite);

/**
 * @brief Free TSL struct.
 *        This function used to free TSL struct.
 *
 * @param shadow. The pointer of TSL Struct.
 *
 * @return success or fail.
 *
 */
void dm_shw_destroy(_IN_ dm_shw_t **shadow);

#if 0
    /**
    * @brief Print detailed information of TSL struct.
    *        This function used to print detailed information of TSL struct.
    *
    * @param shadow. The pointer of TSL Struct.
    *
    * @return success or fail.
    *
    */
    void dm_shw_print(_IN_ dm_shw_t *shadow);
#endif

#endif
#endif