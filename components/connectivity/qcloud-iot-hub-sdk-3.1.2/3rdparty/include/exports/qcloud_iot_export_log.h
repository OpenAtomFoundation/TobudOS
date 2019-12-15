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

#ifndef QCLOUD_IOT_EXPORT_LOG_H_
#define QCLOUD_IOT_EXPORT_LOG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include "qcloud_iot_export_variables.h"

/**
 * SDK log print/upload level
 */
typedef enum {
    eLOG_DISABLE = 0,
    eLOG_ERROR = 1,
    eLOG_WARN = 2,
    eLOG_INFO = 3,
    eLOG_DEBUG = 4
} LOG_LEVEL;

/**
 * log print level control, only print logs with level less or equal to this variable
 */
extern LOG_LEVEL g_log_print_level;

/**
 * log upload level control, only upload logs with level less or equal to this variable
 */
extern LOG_LEVEL g_log_upload_level;

/* user's self defined log handler callback */
typedef bool (*LogMessageHandler)(const char* message);

/**
 * @brief user callback for saving/reading logs into/from NVS(files/FLASH) after upload fail/recover
 */
// callback for saving logs into NVS(files/FLASH) after upload fail 
typedef size_t (*LogSaveFunc)(const char *msg, size_t wLen);
// callback for reading logs from NVS(files/FLASH) when upload ready
typedef size_t (*LogReadFunc)(char *buff, size_t rLen);
// callback for deleting logs in NVS(files/FLASH). return 0 when success
typedef int (*LogDelFunc)();
// callback for reading the size of logs in NVS(files/FLASH). return 0 when nothing exist
typedef size_t (*LogGetSizeFunc)();

/**
 * @brief data structure to init feature of log upload
 */
typedef struct {
    /* device info */
    const char      *product_id;
    const char      *device_name;
    /* auth key, use device secret for PSK device and cert file path for cert device */
    const char      *sign_key;
    /* user callback saving/reading logs into/from NVS(files/FLASH) */
    LogSaveFunc     save_func; 
    LogReadFunc     read_func; 
    LogDelFunc      del_func;
    LogGetSizeFunc  get_size_func;
} LogUploadInitParams;


/**
 * @brief Set the global log level of print
 *
 * @param level 
 */
void IOT_Log_Set_Level(LOG_LEVEL level);

/**
 * @brief Get the global log level of print
 *
 * @return
 */
LOG_LEVEL IOT_Log_Get_Level();

/**
 * @brief Set the global log level of upload
 *
 * @param level
 */
void IOT_Log_Set_Upload_Level(LOG_LEVEL level);

/**
 * @brief Get the global log level of upload
 *
 * @return
 */
LOG_LEVEL IOT_Log_Get_Upload_Level();


/**
 * @brief Set user callback to print log into whereever you want
 *
 * @param handler function pointer of callback
 *
 */
void IOT_Log_Set_MessageHandler(LogMessageHandler handler);


/**
 * @brief Init the resource for log upload
 *
 * @param init_params init parameter
 * @return QCLOUD_RET_SUCCESS when success, or error code when fail
 *
 */
int IOT_Log_Init_Uploader(LogUploadInitParams *init_params);

/**
 * @brief Stop log upload and release the resource
 *
 * @return 
 */
void IOT_Log_Fini_Uploader(void);

/**
 * @brief Do one log upload
 *
 * @param force_upload true = upload log at once, false = upload in defined time interval 
 * @return QCLOUD_RET_SUCCESS when success, or error code when fail
 */
int IOT_Log_Upload(bool force_upload);


/**
 * @brief Generate log for print/upload, call LogMessageHandler if defined
 *
 * When LOG_UPLOAD is enabled, the log will be uploaded to cloud server
 *
 * @param file
 * @param func
 * @param line
 * @param level
 */
void IOT_Log_Gen(const char *file, const char *func, const int line, const int level, const char *fmt, ...);

/* Simple APIs for log generation in different level */
#define Log_d(fmt, ...) IOT_Log_Gen(__FILE__, __FUNCTION__, __LINE__, eLOG_DEBUG, fmt, ##__VA_ARGS__)
#define Log_i(fmt, ...) IOT_Log_Gen(__FILE__, __FUNCTION__, __LINE__, eLOG_INFO,  fmt, ##__VA_ARGS__)
#define Log_w(fmt, ...) IOT_Log_Gen(__FILE__, __FUNCTION__, __LINE__, eLOG_WARN,  fmt, ##__VA_ARGS__)
#define Log_e(fmt, ...) IOT_Log_Gen(__FILE__, __FUNCTION__, __LINE__, eLOG_ERROR, fmt, ##__VA_ARGS__)

/* Macro for debug mode */
#ifdef IOT_DEBUG
	#define IOT_FUNC_ENTRY    \
		{\
		printf("FUNC_ENTRY:   %s L#%d \n", __FUNCTION__, __LINE__);  \
		}
	#define IOT_FUNC_EXIT    \
		{\
		printf("FUNC_EXIT:   %s L#%d \n", __FUNCTION__, __LINE__);  \
		return;\
		}
	#define IOT_FUNC_EXIT_RC(x)    \
		{\
		printf("FUNC_EXIT:   %s L#%d Return Code : %ld \n", __FUNCTION__, __LINE__, (long)(x));  \
		return x; \
		}
#else
	#define IOT_FUNC_ENTRY
	#define IOT_FUNC_EXIT 			\
		{\
			return;\
		}
	#define IOT_FUNC_EXIT_RC(x)     \
		{\
			return x; \
		}
#endif

/* Macro for interval debug */
//#define LOG_UPLOAD_DEBUG 
#ifdef LOG_UPLOAD_DEBUG
#define UPLOAD_DBG(fmt, ...)   HAL_Printf(">>LOG-DBG>>%s(%d): " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define UPLOAD_DBG(...)
#endif
#define UPLOAD_ERR(fmt, ...)   HAL_Printf(">>LOG-ERR>>%s(%d): " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)


#ifdef __cplusplus
}
#endif

#endif /* QCLOUD_IOT_EXPORT_LOG_H_ */
