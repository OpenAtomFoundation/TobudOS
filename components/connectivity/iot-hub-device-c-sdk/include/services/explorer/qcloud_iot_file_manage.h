/**
 * @copyright
 *
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright(C) 2018 - 2022 THL A29 Limited, a Tencent company.All rights reserved.
 *
 * Licensed under the MIT License(the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file qcloud_iot_file_manage.h
 * @brief
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2022-01-11
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2022-01-11 <td>1.0     <td>fancyxu   <td>first commit
 * </table>
 */

#ifndef IOT_HUB_DEVICE_C_SDK_INCLUDE_SERVICES_EXPLORER_QCLOUD_IOT_FILE_MANAGE_H_
#define IOT_HUB_DEVICE_C_SDK_INCLUDE_SERVICES_EXPLORER_QCLOUD_IOT_FILE_MANAGE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "qcloud_iot_common.h"

/**
 * @brief Define of max size using in file manage.
 *
 */
#define MAX_SIZE_OF_FILE_MANAGE_FILE_NAME    64
#define MAX_SIZE_OF_FILE_MANAGE_FILE_TYPE    10
#define MAX_SIZE_OF_FILE_MANAGE_FILE_VERSION 64

/**
 * @brief FileManage report type.
 *
 */
typedef enum {
    // file update
    IOT_FILE_MANAGE_REPORT_TYPE_DOWNLOADING = 0,
    IOT_FILE_MANAGE_REPORT_TYPE_UPGRADE_BEGIN,
    IOT_FILE_MANAGE_REPORT_TYPE_UPGRADE_SUCCESS,
    IOT_FILE_MANAGE_REPORT_TYPE_DOWNLOAD_TIMEOUT,
    IOT_FILE_MANAGE_REPORT_TYPE_FILE_NOT_EXIST,
    IOT_FILE_MANAGE_REPORT_TYPE_AUTH_FAIL,
    IOT_FILE_MANAGE_REPORT_TYPE_MD5_NOT_MATCH,
    IOT_FILE_MANAGE_REPORT_TYPE_UPGRADE_FAIL,
    IOT_FILE_MANAGE_REPORT_TYPE_SPACE_NOT_ENOUGH,
    // file delete
    IOT_FILE_MANAGE_REPORT_TYPE_DEL_SUCCESS,
    IOT_FILE_MANAGE_REPORT_TYPE_DEL_FAIL,
    // file post
    IOT_FILE_MANAGE_REPORT_TYPE_POST_SUCCESS,
    IOT_FILE_MANAGE_REPORT_TYPE_POST_FAIL,
} IotFileManageReportType;

/**
 * @brief FileManage file type. @see sg_file_manage_file_type_str
 *
 */
typedef enum {
    IOT_FILE_MANAGE_FILE_TYPE_UNKOWN = -1,
    IOT_FILE_MANAGE_FILE_TYPE_FILE   = 0,
    IOT_FILE_MANAGE_FILE_TYPE_AUDIO,
    IOT_FILE_MANAGE_FILE_TYPE_VOICE,
    IOT_FILE_MANAGE_FILE_TYPE_VIDEO,
} IotFileManageFileType;

/**
 * @brief File info.
 *
 */
typedef struct {
    char                  file_name[MAX_SIZE_OF_FILE_MANAGE_FILE_NAME];
    char                  file_version[MAX_SIZE_OF_FILE_MANAGE_FILE_VERSION];
    IotFileManageFileType file_type;
} IotFileManageFileInfo;

/**
 * @brief Callback of FileManage.
 *
 */
typedef struct {
    void (*update_file_callback)(UtilsJsonValue file_name, UtilsJsonValue file_type, UtilsJsonValue version,
                                 UtilsJsonValue url, UtilsJsonValue md5sum, uint32_t file_size, void *usr_data);
    void (*del_file_callback)(UtilsJsonValue file_name, UtilsJsonValue file_type, UtilsJsonValue version,
                              void *usr_data);
    void (*report_file_version_reponse_callback)(UtilsJsonValue file_list, int result_code, void *usr_data);
    void (*request_file_url_response_callback)(UtilsJsonValue url, UtilsJsonValue file_token, int result_code,
                                               void *usr_data);
} IotFileManageCallback;

/**
 * @brief File manage init, register handler to server list.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] callback @see IotFileManageCallback
 * @param[in] usr_data usr data used in callback
 * @return 0 for success, or err code (<0) @see IotReturnCode
 */
int IOT_FileManage_Init(void *client, IotFileManageCallback callback, void *usr_data);

/**
 * @brief File manage deinit, unregister handler from server list.
 *
 * @param[in,out] client pointer to mqtt client
 */
void IOT_FileManage_Deinit(void *client);

/**
 * @brief Report file manage message.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[out] buf publish message buffer
 * @param[in] buf_len buffer len
 * @param[in] report_type @see IotFileManageReportType
 * @param[in] progress progress using in  IOT_FILE_MANAGE_REPORT_TYPE_DOWNLOADING
 * @param[in] file_name_or_token token using in post event;file name using in other event
 * @param[in] version file version
 * @return packet id (>=0) when success, or err code (<0) @see IotReturnCode
 */
int IOT_FileManage_Report(void *client, char *buf, int buf_len, IotFileManageReportType report_type, int progress,
                          const char *file_name_or_token, const char *version);

/**
 * @brief Report file list to server mqtt topic.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[out] buf publish message buffer
 * @param[in] buf_len buffer len
 * @param[in] file_list file list of @see IotFileManageFileInfo; file name which is "\0" means invalid.
 * @param[in] max_num max num of file list.
 * @return packet id (>=0) when success, or err code (<0) @see IotReturnCode
 */
int IOT_FileManage_ReportFileList(void *client, char *buf, int buf_len, const IotFileManageFileInfo file_list[],
                                  int max_num);
/**
 * @brief Request url to upload.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[out] buf publish message buffer
 * @param[in] buf_len buffer len
 * @param[in] file_info file to upload, @see IotFileManageFileInfo
 * @param[in] request_id user defined, to keep unique.
 * @return packet id (>=0) when success, or err code (<0) @see IotReturnCode
 */
int IOT_FileManage_PostRequest(void *client, char *buf, int buf_len, const IotFileManageFileInfo *file_info,
                               int request_id);

/**
 * @brief Get file type according to string. @see sg_file_manage_file_type_str.
 *
 * @param[in] file_type file type string
 * @param[in] len string length
 * @return @see IotFileManageFileType
 */
IotFileManageFileType IOT_FileManage_GetFileType(const char *file_type, int len);

#ifdef __cplusplus
}
#endif

#endif  // IOT_HUB_DEVICE_C_SDK_INCLUDE_SERVICES_EXPLORER_QCLOUD_IOT_FILE_MANAGE_H_
