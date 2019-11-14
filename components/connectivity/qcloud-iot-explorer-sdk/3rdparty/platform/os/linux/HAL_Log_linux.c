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

#include <stdio.h>
#include <stdlib.h>

#include "qcloud_iot_import.h"
#include "qcloud_iot_export_log.h"
#include "qcloud_iot_export_error.h"

#ifdef LOG_UPLOAD

#define LOG_SAVE_FILE_PATH "upload-fail-save.log"

size_t HAL_Log_Save(const char *log, size_t wLen)
{
    FILE *fp;
    size_t len;    

    if( ( fp = fopen(LOG_SAVE_FILE_PATH, "a+" ) ) == NULL ) {
        Log_e("fail to open file %s", LOG_SAVE_FILE_PATH);
        return 0;
    }    
    
    len = fwrite((void *)log, 1, wLen, fp);
    Log_d("write %d of %d to log file", len, wLen);
    
    fclose(fp);

    return len;
}


size_t HAL_Log_Read   (char *buff, size_t rLen)
{
    FILE *fp;
    size_t len;    

    if( ( fp = fopen(LOG_SAVE_FILE_PATH, "r" ) ) == NULL ) {
        Log_e("fail to open file %s", LOG_SAVE_FILE_PATH);
        return 0;
    }

    len = fread((void *)buff, 1, rLen, fp);
    Log_d("read %d of %d from log file", len, rLen);
    
    fclose(fp);

    return len;
}


int HAL_Log_Del(void)
{
    return remove(LOG_SAVE_FILE_PATH);
}


size_t HAL_Log_Get_Size(void)
{
    long length;
    FILE *fp;

    /* check if file exists */
    if (access(LOG_SAVE_FILE_PATH, 0))
        return 0;
    
    if( ( fp = fopen(LOG_SAVE_FILE_PATH, "r" ) ) == NULL ) {
        Log_e("fail to open file %s", LOG_SAVE_FILE_PATH);
        return 0;
    }    
    
    fseek(fp, 0L, SEEK_END);
    length = ftell(fp);
    fclose(fp);
    if (length > 0)
        return (size_t)length;
    else
        return 0;
}

#endif

