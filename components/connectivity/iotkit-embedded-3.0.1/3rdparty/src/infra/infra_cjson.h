/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _INFRA_CJSON_H_
#define _INFRA_CJSON_H_

/* cJSON Types: */
#define cJSON_Invalid (0)
#define cJSON_False  (1 << 0)
#define cJSON_True   (1 << 1)
#define cJSON_NULL   (1 << 2)
#define cJSON_Number (1 << 3)
#define cJSON_String (1 << 4)
#define cJSON_Array  (1 << 5)
#define cJSON_Object (1 << 6)
#define cJSON_Raw    (1 << 7) /* raw json */

#ifndef LITE_CJSON_NESTING_LIMIT
    #define LITE_CJSON_NESTING_LIMIT 1000
#endif

/* The cJSON structure: */
typedef struct lite_cjson_st {
    /* The type of the item, as above. */
    int type;

    /* The item's string, if type==cJSON_String  and type == cJSON_Raw */
    char *value;
    int value_length;

    /* The item's size, if type == cJSON_Array and type == cJSON_Object */
    int size;

    double value_double;
    int value_int;
} lite_cjson_t;

int lite_cjson_parse(const char *src, int src_len, lite_cjson_t *lite);

int lite_cjson_is_false(lite_cjson_t *lite);
int lite_cjson_is_true(lite_cjson_t *lite);
int lite_cjson_is_null(lite_cjson_t *lite);
int lite_cjson_is_number(lite_cjson_t *lite);
int lite_cjson_is_string(lite_cjson_t *lite);
int lite_cjson_is_array(lite_cjson_t *lite);
int lite_cjson_is_object(lite_cjson_t *lite);

int lite_cjson_array_item(lite_cjson_t *lite, int index, lite_cjson_t *lite_item);
int lite_cjson_object_item(
            lite_cjson_t *lite,
            const char *key,
            int key_len,
            lite_cjson_t *lite_item);
int lite_cjson_object_item_by_index(
            lite_cjson_t *lite,
            int index,
            lite_cjson_t *lite_item_key,
            lite_cjson_t *lite_item_value);


/*** lite_cjson create, add and print ***/
typedef int cJSON_bool;

#define cJSON_IsReference 256
#define cJSON_StringIsConst 512

/* The cJSON structure: */
typedef struct lite_cjson_item_t {
    struct lite_cjson_item_t *next,
               *prev;  /* next/prev allow you to walk array/object chains. Alternatively, use GetArraySize/GetArrayItem/GetObjectItem */
    struct lite_cjson_item_t
        *child;        /* An array or object item will have a child pointer pointing to a chain of the items in the array/object. */

    int type;                   /* The type of the item, as above. */

    char *valuestring;          /* The item's string, if type==cJSON_String */
    int valueint;               /* The item's number, if type==cJSON_Number */
    double valuedouble;         /* The item's number, if type==cJSON_Number */

    char *string;               /* The item's name string, if this item is the child of, or is in the list of subitems of an object. */
} lite_cjson_item_t;

typedef struct {
    void *(*malloc_fn)(unsigned int sz);
    void(*free_fn)(void *ptr);
} lite_cjson_hooks;

void lite_cjson_init_hooks(lite_cjson_hooks *hooks);

/* Render a lite_cjson_item_t entity to text for transfer/storage. Free the char* when finished. */
char  *lite_cjson_print(lite_cjson_item_t *item);
char  *lite_cjson_print_unformatted(lite_cjson_item_t *item);

/* Delete a lite_cjson_item_t entity and all subentities. */
void lite_cjson_delete(lite_cjson_item_t *item);

/* Append item to specific object */
void lite_cjson_add_item_to_array(lite_cjson_item_t *array, lite_cjson_item_t *item);
void lite_cjson_add_item_to_object(lite_cjson_item_t *object, const char *string, lite_cjson_item_t *item);

/* These calls create a lite_cjson_item_t item of the appropriate type. */
lite_cjson_item_t *lite_cjson_create_null(void);
lite_cjson_item_t *lite_cjson_create_true(void);
lite_cjson_item_t *lite_cjson_create_false(void);
lite_cjson_item_t *lite_cjson_create_bool(int b);
lite_cjson_item_t *lite_cjson_create_number(double num);
lite_cjson_item_t *lite_cjson_create_string(const char *string);
lite_cjson_item_t *lite_cjson_create_array(void);
lite_cjson_item_t *lite_cjson_create_object(void);

/* These utilities create an Array of count items. */
lite_cjson_item_t *lite_cjson_create_intArray(const int *numbers, int count);
lite_cjson_item_t *lite_cjson_create_floatArray(const float *numbers, int count);
lite_cjson_item_t *lite_cjson_create_doubleArray(const double *numbers, int count);
lite_cjson_item_t *lite_cjson_create_stringArray(const char **strings, int count);

/* Macros for creating things quickly. */
#define lite_cjson_add_null_to_object(object,name)      lite_cjson_add_item_to_object(object, name, lite_cjson_create_null())
#define lite_cjson_add_true_to_object(object,name)      lite_cjson_add_item_to_object(object, name, lite_cjson_create_true())
#define lite_cjson_add_false_to_object(object,name)     lite_cjson_add_item_to_object(object, name, lite_cjson_create_false())
#define lite_cjson_add_bool_to_object(object,name,b)        lite_cjson_add_item_to_object(object, name, lite_cjson_create_bool(b))
#define lite_cjson_add_number_to_object(object,name,n)      lite_cjson_add_item_to_object(object, name, lite_cjson_create_number(n))
#define lite_cjson_add_string_to_object(object,name,s)      lite_cjson_add_item_to_object(object, name, lite_cjson_create_string(s))
#endif

