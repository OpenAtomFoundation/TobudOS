/*
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright (C) 2016 THL A29 Limited, a Tencent company. All rights reserved.

 * Licensed under the MIT License (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT

 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "qcloud_iot_import.h"
#include "qcloud_iot_export.h"   

#ifdef LOG_UPLOAD

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

#include "qcloud_iot_common.h" 
#include "log_upload.h"
#include "utils_httpc.h"
#include "utils_timer.h"
#include "utils_hmac.h"
#include "lite-utils.h"



/* log post header format */
#define TIMESTAMP_SIZE 10
#define SIGNATURE_SIZE 40
#define CTRL_BYTES_SIZE 4
//LOG_BUF_FIXED_HEADER_SIZE = 112
#define LOG_BUF_FIXED_HEADER_SIZE   (SIGNATURE_SIZE + CTRL_BYTES_SIZE + MAX_SIZE_OF_PRODUCT_ID + MAX_SIZE_OF_DEVICE_NAME + TIMESTAMP_SIZE)

/* do immediate log update if buffer is lower than this threshold (about two max log item) */
#define LOG_LOW_BUFFER_THRESHOLD    (LOG_UPLOAD_BUFFER_SIZE/4)

/* log upload buffer */
static char *sg_log_buffer = NULL;
static uint32_t sg_write_index = LOG_BUF_FIXED_HEADER_SIZE;

#define SIGN_KEY_SIZE 24
static char sg_sign_key[SIGN_KEY_SIZE+1] = {0};

/* Log upload feature switch */
/* To check log http server return msg or not */
#define LOG_CHECK_HTTP_RET_CODE


typedef struct {

    const char *url;
    const char *ca_crt;
    int port;
    HTTPClient http;            /* http client */
    HTTPClientData http_data;   /* http client data */

} LogHTTPStruct;

static LogHTTPStruct *sg_http_c;

typedef struct {

    const char *product_id;
    const char *device_name;
    void *mqtt_client;
    bool upload_only_in_comm_err;

    void *lock_buf;
    Timer upload_timer;
#ifndef LOG_UPDATE_TIME_WHEN_UPLOAD    
    Timer time_update_timer;
#endif
    long system_time;

    LogSaveFunc     save_func; 
    LogReadFunc     read_func; 
    LogDelFunc      del_func;
    LogGetSizeFunc  get_size_func;
    bool log_save_enabled;

    bool uploader_init_done;

} LogUploaderStruct;

static LogUploaderStruct *sg_uploader;
static bool sg_log_uploader_init_done = false;

#ifdef AUTH_MODE_CERT
static int _gen_key_from_file( const char *file_path)
{
    FILE *fp;
    int len;
    char line_buf[128] = {0};

    if( ( fp = fopen( file_path, "r" ) ) == NULL ) {
        UPLOAD_ERR("fail to open cert file %s", file_path);
        return -1;
    }

    /* find the begin line */
    do {
        if (NULL == fgets(line_buf, sizeof(line_buf), fp)) {
            UPLOAD_ERR("fail to fgets file %s", file_path);
            return -1;
        }
    } while (strstr(line_buf, "-----BEGIN ") == NULL);

    if (feof(fp)) {
        UPLOAD_ERR("invalid cert file %s", file_path);
        return -1;
    }
    
    if (NULL == fgets(line_buf, sizeof(line_buf), fp)) {
        UPLOAD_ERR("fail to fgets file %s", file_path);
        return -1;
    }
    
    len = strlen(line_buf);
    memcpy(sg_sign_key, line_buf, len>SIGN_KEY_SIZE?SIGN_KEY_SIZE:len);
    UPLOAD_DBG("sign key %s", sg_sign_key);
    
    fclose(fp);

    return 0;
}
#endif


static long _get_system_time(void)
{
#ifdef SYSTEM_COMM
    if (sg_uploader->mqtt_client == NULL)
        return 0;
    
    long sys_time = 0;
    int rc = IOT_Get_SysTime(sg_uploader->mqtt_client, &sys_time);
    if (rc == QCLOUD_RET_SUCCESS)
        return sys_time;
    else
        return 0;
#else
    return 0;     
#endif    
}

static void _update_system_time(void)
{
/* to avoid frequent get_system_time */
#define LOG_TIME_UPDATE_INTERVAL 2
 
    if(!expired(&sg_uploader->time_update_timer))
        return;

    sg_uploader->system_time = _get_system_time();        

    countdown(&sg_uploader->time_update_timer, LOG_TIME_UPDATE_INTERVAL);
}


static int _check_server_connection()
{
    int rc;

    rc = qcloud_http_client_connect(&sg_http_c->http, sg_http_c->url, sg_http_c->port, sg_http_c->ca_crt);
    if (rc != QCLOUD_RET_SUCCESS) 
        return rc;

    qcloud_http_client_close(&sg_http_c->http);
    
    return QCLOUD_RET_SUCCESS;
}

#ifdef LOG_CHECK_HTTP_RET_CODE
static bool _get_json_ret_code(char *json, int32_t* res) {
	char *v = LITE_json_value_of("Retcode", json);
	if (v == NULL) {
        UPLOAD_ERR("Invalid json content: %s", json);
		return false;
	}
	if (LITE_get_int32(res, v) != QCLOUD_RET_SUCCESS) {
        UPLOAD_ERR("Invalid json content: %s", json);
        HAL_Free(v);
		return false;
	}
    HAL_Free(v);
	return true;
}
#endif

static int _post_one_http_to_server(char *post_buf, size_t post_size)
{  
    int rc = 0;

    if(sg_http_c == NULL)
        return QCLOUD_ERR_INVAL;

    sg_http_c->http_data.post_content_type = "text/plain;charset=utf-8";
    sg_http_c->http_data.post_buf = post_buf;
    sg_http_c->http_data.post_buf_len = post_size;

    rc = qcloud_http_client_common(&sg_http_c->http, sg_http_c->url, sg_http_c->port, sg_http_c->ca_crt, 
                                                        HTTP_POST, &sg_http_c->http_data);
    if (rc != QCLOUD_RET_SUCCESS) {
        UPLOAD_ERR("qcloud_http_client_common failed, rc = %d", rc);
        return rc;
    } 
    UPLOAD_DBG("Log client POST size: %d", post_size);

#ifdef LOG_CHECK_HTTP_RET_CODE    
    /* TODO: handle recv data from log server */
    #define HTTP_RET_JSON_LENGTH 256
    #define HTTP_WAIT_RET_TIMEOUT_MS 1000
    char buf[HTTP_RET_JSON_LENGTH] = {0};
    sg_http_c->http_data.response_buf = buf;
    sg_http_c->http_data.response_buf_len = sizeof(buf);    
   
    rc = qcloud_http_recv_data(&sg_http_c->http, HTTP_WAIT_RET_TIMEOUT_MS, &sg_http_c->http_data);
    if (QCLOUD_RET_SUCCESS != rc) {
        UPLOAD_ERR("qcloud_http_recv_data failed, rc = %d", rc);        
    } else {
        int32_t ret = -1;
        
        buf[HTTP_RET_JSON_LENGTH - 1] = '\0';    // json_parse relies on a string
        if (strlen(buf)>0 && _get_json_ret_code(buf, &ret) && ret == 0) {
            UPLOAD_DBG("Log server return SUCCESS: %s", buf);
        } else {
            UPLOAD_ERR("Log server return FAIL(%d): %s", ret, buf);
            rc = QCLOUD_ERR_HTTP;
        }
    }
#endif

    qcloud_http_client_close(&sg_http_c->http);

    return rc;
    
}

static void _update_time_and_signature(char *log_buf, size_t log_size)
{
    char timestamp[TIMESTAMP_SIZE+1] = {0};
    char signature[SIGNATURE_SIZE+1]   = {0};

    /* get system time from IoT hub first */
    _update_system_time();

    /* record the timestamp for this log uploading */
    HAL_Snprintf(timestamp, TIMESTAMP_SIZE+1, "%010ld", sg_uploader->system_time);    
    memcpy(log_buf+LOG_BUF_FIXED_HEADER_SIZE-TIMESTAMP_SIZE, timestamp, strlen(timestamp));

    /* signature of this log uploading */
    utils_hmac_sha1(log_buf+SIGNATURE_SIZE, log_size - SIGNATURE_SIZE, signature, sg_sign_key, strlen(sg_sign_key));    
    memcpy(log_buf, signature, SIGNATURE_SIZE);

}

static int _post_log_to_server(char *post_buf, size_t post_size, size_t *actual_post_payload)
{
#define LOG_DELIMITER "\n\f"
    int ret = QCLOUD_RET_SUCCESS;
    /* one shot upload */
    if (post_size < MAX_HTTP_LOG_POST_SIZE) {
        _update_time_and_signature(post_buf, post_size);
        ret = _post_one_http_to_server(post_buf, post_size);
        if (QCLOUD_RET_SUCCESS == ret) {
            *actual_post_payload = post_size - LOG_BUF_FIXED_HEADER_SIZE;
        } else {
            UPLOAD_ERR("one time log send failed");
            *actual_post_payload = 0;
        }
        return ret;
    }    
        
    /* Log size is larger than one HTTP post size */ 
    /* Fragment the log and upload multi-times */    
    UPLOAD_DBG("to post large log size %d", post_size); 
    *actual_post_payload = 0;
    size_t delimiter_len = strlen(LOG_DELIMITER);
    size_t orig_post_size = post_size;
    size_t post_payload, upload_size, possible_size;
    do {
        char *next_log_buf = NULL;                        
        possible_size = 0;
        while (possible_size < MAX_HTTP_LOG_POST_SIZE){
            /*remember last valid position */
            upload_size = possible_size;
            /* locate the delimiter */
            next_log_buf = strstr(post_buf + upload_size, LOG_DELIMITER);
            if (next_log_buf == NULL) {                
                UPLOAD_ERR("Invalid log delimiter. Total sent: %d. Left: %d", 
                    *actual_post_payload+LOG_BUF_FIXED_HEADER_SIZE, post_size);                
                return QCLOUD_ERR_INVAL;

            }
            possible_size = (size_t)(next_log_buf - post_buf + delimiter_len);
            /* end of log */
            if (next_log_buf[delimiter_len] == 0 && possible_size < MAX_HTTP_LOG_POST_SIZE) {                
                upload_size = possible_size;
                break;
            }
        }

        if (upload_size == 0) {
            UPLOAD_ERR("Upload size should not be 0! Total sent: %d. Left: %d", 
                    *actual_post_payload+LOG_BUF_FIXED_HEADER_SIZE, post_size);
            return QCLOUD_ERR_FAILURE;
        }
        
        _update_time_and_signature(post_buf, upload_size);
        ret = _post_one_http_to_server(post_buf, upload_size);
        if (QCLOUD_RET_SUCCESS != ret) {                
            UPLOAD_ERR("Send log failed. Total sent: %d. Left: %d", 
                    *actual_post_payload+LOG_BUF_FIXED_HEADER_SIZE, post_size);
            return QCLOUD_ERR_FAILURE;
        }
        
        /* move the left log forward and do next upload */
        memmove(post_buf+LOG_BUF_FIXED_HEADER_SIZE, post_buf+upload_size, post_size-upload_size);
        post_payload = upload_size - LOG_BUF_FIXED_HEADER_SIZE;
        post_size -= post_payload;
        *actual_post_payload += post_payload;
        memset(post_buf+post_size, 0, orig_post_size - post_size);        
        UPLOAD_DBG("post log %d OK. Total sent: %d. Left: %d", 
            upload_size, *actual_post_payload+LOG_BUF_FIXED_HEADER_SIZE, post_size);
    } while(post_size>LOG_BUF_FIXED_HEADER_SIZE);

    return QCLOUD_RET_SUCCESS;    
}


static void _reset_log_buffer(void)
{    
    sg_write_index = LOG_BUF_FIXED_HEADER_SIZE;
    memset(sg_log_buffer+LOG_BUF_FIXED_HEADER_SIZE, 0, LOG_UPLOAD_BUFFER_SIZE - LOG_BUF_FIXED_HEADER_SIZE); 
}

static int _save_log(char *log_buf, size_t log_size)
{
    int rc = 0;
    size_t write_size, current_size = sg_uploader->get_size_func();    

    /* overwrite the previous saved log to avoid too many saved logs */
    if ((current_size + log_size) > MAX_LOG_SAVE_SIZE) {
        UPLOAD_ERR("overwrite the previous saved log. %d", current_size);
        rc = sg_uploader->del_func();
        if (rc) {
            Log_e("fail to delete previous log");
        }
    }
    
    write_size = sg_uploader->save_func(log_buf, log_size);
    if (write_size != log_size) {
        Log_e("fail to save log. RC %d - log size %d", write_size, log_size);
        rc = -1;
    } else
        rc = 0;

    return rc;

}

static int _handle_saved_log(void)
{    
    int rc = QCLOUD_RET_SUCCESS;
    size_t whole_log_size = sg_uploader->get_size_func();
    if (whole_log_size > 0) {
        /* only do the job when connection is OK */
        if (_check_server_connection() != QCLOUD_RET_SUCCESS)
            return QCLOUD_ERR_FAILURE;
    
        size_t buf_size = whole_log_size + LOG_BUF_FIXED_HEADER_SIZE + 1;
        char *log_buf = HAL_Malloc(buf_size);
        if (log_buf != NULL) {
            /* read the whole log to buffer */
            size_t read_len = sg_uploader->read_func(log_buf + LOG_BUF_FIXED_HEADER_SIZE, whole_log_size);
            if (read_len == whole_log_size) {                
                size_t upload_size = whole_log_size + LOG_BUF_FIXED_HEADER_SIZE;

                /* copy header from global log buffer */
                memcpy(log_buf, sg_log_buffer, LOG_BUF_FIXED_HEADER_SIZE);
                log_buf[buf_size - 1] = 0;

                size_t actual_post_payload;
                rc = _post_log_to_server(log_buf, upload_size, &actual_post_payload);
                if (rc == QCLOUD_RET_SUCCESS || rc == QCLOUD_ERR_INVAL) {
                    Log_d("handle saved log done! Size: %d. upload paylod: %d", whole_log_size, actual_post_payload);
                    sg_uploader->del_func();
                }    
                HAL_Free(log_buf);                
            } else {
                Log_e("fail to read whole saved log. Size: %u - read: %u", whole_log_size, read_len);
                HAL_Free(log_buf);
                return QCLOUD_ERR_FAILURE;
            }            
    
        } else {
            Log_e("Malloc failed, size: %u", buf_size);
            return QCLOUD_ERR_FAILURE;
        }
    }

    return rc;

}


void set_log_mqtt_client(void *client)
{
    if (!sg_log_uploader_init_done)
        return ;
    
    sg_uploader->mqtt_client = client;
}

void set_log_upload_in_comm_err(bool value)
{
    if (!sg_log_uploader_init_done)
        return ;
    
    sg_uploader->upload_only_in_comm_err = value;
}


int append_to_upload_buffer(const char *log_content, size_t log_size)
{
    if (!sg_log_uploader_init_done)
        return -1;
    
    if (log_content == NULL || log_size <= 0) {
        UPLOAD_ERR("invalid log content!");
        return -1;
    }

    if (HAL_MutexTryLock(sg_uploader->lock_buf) != 0) {
        UPLOAD_ERR("trylock buffer failed!");        
        return -1;
    }
    
    if ((sg_write_index + log_size + 1) > LOG_UPLOAD_BUFFER_SIZE) {        
        countdown_ms(&sg_uploader->upload_timer, 0);
        HAL_MutexUnlock(sg_uploader->lock_buf);
        UPLOAD_ERR("log upload buffer is not enough!");
        return -1;
    }

    memcpy(sg_log_buffer+sg_write_index, log_content, log_size);
    
    sg_write_index += log_size;

    /* replace \r\n to \n\f as delimiter */
    sg_log_buffer[sg_write_index - 1] = '\f';
    sg_log_buffer[sg_write_index - 2] = '\n';    

    HAL_MutexUnlock(sg_uploader->lock_buf);
    return 0;
}


void clear_upload_buffer(void)
{
    if (!sg_log_uploader_init_done)
        return ;

    HAL_MutexLock(sg_uploader->lock_buf);
    _reset_log_buffer();
    HAL_MutexUnlock(sg_uploader->lock_buf);
    
}


int init_log_uploader(LogUploadInitParams *init_params)
{
    if (sg_log_uploader_init_done)
        return QCLOUD_RET_SUCCESS;

    if (init_params == NULL || init_params->product_id == NULL || 
            init_params->device_name == NULL || init_params->sign_key == NULL) {
        UPLOAD_ERR("invalid init parameters");
        return QCLOUD_ERR_INVAL;
    }

    int key_len = strlen(init_params->sign_key);
    if (key_len == 0) {
        UPLOAD_ERR("invalid key length");        
        return QCLOUD_ERR_INVAL;
    }

    sg_log_buffer = HAL_Malloc(LOG_UPLOAD_BUFFER_SIZE);
    if (sg_log_buffer == NULL) {
        UPLOAD_ERR("malloc log buffer failed");        
        return QCLOUD_ERR_FAILURE;
    }

    int i;
    for(i = 0; i<LOG_BUF_FIXED_HEADER_SIZE; i++)
        sg_log_buffer[i] = '#';
    
#ifdef AUTH_MODE_CERT     
    if (_gen_key_from_file(init_params->sign_key) != 0) {
        UPLOAD_ERR("gen_key_from_file failed");        
        return QCLOUD_ERR_FAILURE;
    }
    sg_log_buffer[SIGNATURE_SIZE] = 'C';
#else    
    memcpy(sg_sign_key, init_params->sign_key, key_len>SIGN_KEY_SIZE?SIGN_KEY_SIZE:key_len);
    sg_log_buffer[SIGNATURE_SIZE] = 'P';
#endif    

    memcpy(sg_log_buffer+SIGNATURE_SIZE+CTRL_BYTES_SIZE, init_params->product_id, MAX_SIZE_OF_PRODUCT_ID);
    memcpy(sg_log_buffer+SIGNATURE_SIZE+CTRL_BYTES_SIZE+MAX_SIZE_OF_PRODUCT_ID, init_params->device_name, strlen(init_params->device_name));        

    if (NULL == (sg_uploader = HAL_Malloc(sizeof(LogUploaderStruct)))) {        
        UPLOAD_ERR("allocate for LogUploaderStruct failed");
        return QCLOUD_ERR_FAILURE;
    }
    memset(sg_uploader, 0, sizeof(LogUploaderStruct));

    sg_uploader->product_id = init_params->product_id;
    sg_uploader->device_name = init_params->device_name;
    sg_uploader->mqtt_client = NULL;
    sg_uploader->system_time = 0;
    sg_uploader->upload_only_in_comm_err = false;

    /* all the call back functions are necessary to handle log save and re-upload */
    if (init_params->save_func != NULL && init_params->read_func != NULL && 
                init_params->del_func != NULL && init_params->get_size_func) {
        sg_uploader->save_func = init_params->save_func;
        sg_uploader->read_func = init_params->read_func;
        sg_uploader->del_func = init_params->del_func;
        sg_uploader->get_size_func = init_params->get_size_func;
        sg_uploader->log_save_enabled = true;
    } else
        sg_uploader->log_save_enabled = false;
    
    InitTimer(&sg_uploader->upload_timer);
    InitTimer(&sg_uploader->time_update_timer);

    if ((sg_uploader->lock_buf = HAL_MutexCreate()) == NULL) {
        UPLOAD_ERR("mutex create failed");
        return QCLOUD_ERR_FAILURE;
    }
    
    if (NULL == (sg_http_c = HAL_Malloc(sizeof(LogHTTPStruct)))) {
        HAL_MutexDestroy(sg_uploader->lock_buf);
        HAL_Free(sg_uploader);
        UPLOAD_ERR("allocate for LogHTTPStruct failed");
        return QCLOUD_ERR_FAILURE;
    }
    memset(sg_http_c, 0, sizeof(LogHTTPStruct));

    /* set http request-header parameter */
    sg_http_c->http.header = "Accept:application/json;*/*\r\n";
    sg_http_c->url = LOG_UPLOAD_SERVER_URL;
    sg_http_c->port = LOG_UPLOAD_SERVER_PORT;
    sg_http_c->ca_crt = NULL;

    _reset_log_buffer();
    sg_log_uploader_init_done = true;

    return QCLOUD_RET_SUCCESS;
    
}

void fini_log_uploader(void)
{
    if (!sg_log_uploader_init_done)
        return ;
        
    HAL_MutexLock(sg_uploader->lock_buf);
    sg_log_uploader_init_done = false; 
    if (sg_log_buffer) {
        _reset_log_buffer();
        HAL_Free(sg_log_buffer);
        sg_log_buffer = NULL;
    }
    HAL_MutexUnlock(sg_uploader->lock_buf);

    HAL_MutexDestroy(sg_uploader->lock_buf);
    sg_uploader->lock_buf = NULL;
    HAL_Free(sg_uploader);
    sg_uploader = NULL;
    HAL_Free(sg_http_c);
    sg_http_c = NULL;
}

bool is_log_uploader_init(void)
{
    return sg_log_uploader_init_done;
}

static bool _check_force_upload(bool force_upload)
{    
    if (!force_upload) {
        /* Double check if the buffer is low */
        HAL_MutexLock(sg_uploader->lock_buf);
        bool is_low_buffer = (LOG_UPLOAD_BUFFER_SIZE - sg_write_index) < LOG_LOW_BUFFER_THRESHOLD ? true : false;

        /* force_upload is false and upload_only_in_comm_err is true */
        if(sg_uploader->upload_only_in_comm_err) {
            /* buffer is low but we couldn't upload now, reset buffer */
            if (is_low_buffer)
                _reset_log_buffer();

            HAL_MutexUnlock(sg_uploader->lock_buf);
            countdown_ms(&sg_uploader->upload_timer, LOG_UPLOAD_INTERVAL_MS);
            return false;    
        }
        HAL_MutexUnlock(sg_uploader->lock_buf);
        
        if (is_low_buffer) {
            /* buffer is low, handle it right now */
            return true;        
        } else {
            return expired(&sg_uploader->upload_timer);
        }

    }else
        return true;

}

int do_log_upload(bool force_upload)
{
    int rc;    
    int upload_log_size = 0;
    static bool unhandle_saved_log = true;

    if (!sg_log_uploader_init_done)
        return QCLOUD_ERR_FAILURE;

    /* double check force upload */
    if (!_check_force_upload(force_upload))
        return QCLOUD_RET_SUCCESS;

    /* handle previously saved log */
    if (sg_uploader->log_save_enabled && unhandle_saved_log) {
        rc = _handle_saved_log();
        if (rc == QCLOUD_RET_SUCCESS)
            unhandle_saved_log = false;
    }
    
    /* no more log in buffer */
    if (sg_write_index == LOG_BUF_FIXED_HEADER_SIZE)
        return QCLOUD_RET_SUCCESS;

    HAL_MutexLock(sg_uploader->lock_buf);
    upload_log_size = sg_write_index;
    HAL_MutexUnlock(sg_uploader->lock_buf);
    
    size_t actual_post_payload;
    rc = _post_log_to_server(sg_log_buffer, upload_log_size, &actual_post_payload);    
    if (rc != QCLOUD_RET_SUCCESS) {
        /* save log via user callbacks when log upload fail */
        if (sg_uploader->log_save_enabled) {
            /* new error logs should have been added, update log size */
            HAL_MutexLock(sg_uploader->lock_buf);
            /* parts of log were uploaded succesfully. Need to move the new logs forward */
            if (actual_post_payload) {
                UPLOAD_DBG("move the new log %d forward", actual_post_payload);
                memmove(sg_log_buffer+upload_log_size-actual_post_payload, 
                    sg_log_buffer+upload_log_size, sg_write_index-upload_log_size);
                sg_write_index = sg_write_index - actual_post_payload;
                memset(sg_log_buffer+sg_write_index, 0, LOG_UPLOAD_BUFFER_SIZE - sg_write_index);
            }
            upload_log_size = sg_write_index;
            HAL_MutexUnlock(sg_uploader->lock_buf);
            _save_log(sg_log_buffer+LOG_BUF_FIXED_HEADER_SIZE, upload_log_size-LOG_BUF_FIXED_HEADER_SIZE);
            unhandle_saved_log = true;
        }        
    }
    
    /* move the new log during send_log_to_server */
    HAL_MutexLock(sg_uploader->lock_buf);
    if (upload_log_size == sg_write_index) {
        _reset_log_buffer();
    } else {
        memmove(sg_log_buffer+LOG_BUF_FIXED_HEADER_SIZE, sg_log_buffer+upload_log_size, sg_write_index-upload_log_size);
        sg_write_index = sg_write_index - upload_log_size + LOG_BUF_FIXED_HEADER_SIZE;
        memset(sg_log_buffer+sg_write_index, 0, LOG_UPLOAD_BUFFER_SIZE - sg_write_index);
    }
    HAL_MutexUnlock(sg_uploader->lock_buf);
    
    countdown_ms(&sg_uploader->upload_timer, LOG_UPLOAD_INTERVAL_MS);
    
    return QCLOUD_RET_SUCCESS;
}

#endif

#ifdef __cplusplus
}
#endif
