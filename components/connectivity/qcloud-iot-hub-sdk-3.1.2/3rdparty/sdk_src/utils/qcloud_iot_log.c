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

#include "qcloud_iot_export_log.h"

#include <string.h>

#include "qcloud_iot_import.h"
#include "log_upload.h"


static char *level_str[] = {
    "DIS", "ERR", "WRN", "INF", "DBG"
};

static LogMessageHandler sg_log_message_handler= NULL;

LOG_LEVEL g_log_print_level = eLOG_INFO;

#ifdef LOG_UPLOAD
LOG_LEVEL g_log_upload_level = eLOG_ERROR;
#else
LOG_LEVEL g_log_upload_level = eLOG_DISABLE;
#endif

static const char *_get_filename(const char *p)
{
#ifdef WIN32
    char ch = '\\';
#else
    char ch = '/';
#endif
    const char *q = strrchr(p,ch);
    if(q == NULL)
    {
        q = p;
    }
    else
    {
        q++;
    }
    return q;
}

void IOT_Log_Set_Level(LOG_LEVEL logLevel) {
    g_log_print_level = logLevel;
}

LOG_LEVEL IOT_Log_Get_Level() {
    return g_log_print_level;
}

void IOT_Log_Set_MessageHandler(LogMessageHandler handler) {
	sg_log_message_handler = handler;
}

void IOT_Log_Set_Upload_Level(LOG_LEVEL logLevel) {
    g_log_upload_level = logLevel;
}

LOG_LEVEL IOT_Log_Get_Upload_Level() {
    return g_log_upload_level;
}

int IOT_Log_Init_Uploader(LogUploadInitParams *init_params)
{
#ifdef LOG_UPLOAD
    return init_log_uploader(init_params);
#else
    return 0;
#endif
}

void IOT_Log_Fini_Uploader(void)
{
#ifdef LOG_UPLOAD
    fini_log_uploader();
	return;
#else
    return ;
#endif
}

int IOT_Log_Upload(bool force_upload)
{
#ifdef LOG_UPLOAD
    return do_log_upload(force_upload);
#else
    return 0;
#endif
}

void IOT_Log_Gen(const char *file, const char *func, const int line, const int level, const char *fmt, ...)
{
	if (level > g_log_print_level && level > g_log_upload_level) {
		return;
	}

    /* format log content */
	const char *file_name = _get_filename(file);

	char sg_text_buf[MAX_LOG_MSG_LEN + 1];
	char		*tmp_buf = sg_text_buf;
	char        *o = tmp_buf;
    memset(tmp_buf, 0, sizeof(sg_text_buf));

    o += HAL_Snprintf(o, sizeof(sg_text_buf), "%s|%s|%s|%s(%d): ", level_str[level], HAL_Timer_current(), file_name, func, line);

    va_list     ap;
    va_start(ap, fmt);
    o += vsnprintf(o, MAX_LOG_MSG_LEN - 2 - strlen(tmp_buf), fmt, ap);
    va_end(ap);

    strcat(tmp_buf, "\r\n");

#ifdef LOG_UPLOAD
    /* append to upload buffer */
    if (level <= g_log_upload_level) {
        append_to_upload_buffer(tmp_buf, strlen(tmp_buf));
    }
#endif

    if (level <= g_log_print_level) {
        /* customer defined log print handler */
    	if (sg_log_message_handler != NULL && sg_log_message_handler(tmp_buf)) {
    		return;
    	}

        /* default log handler: print to console */
        HAL_Printf("%s", tmp_buf);
    }

    return;
}

#ifdef __cplusplus
}
#endif
