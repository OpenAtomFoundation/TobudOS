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
 * @file test_ota_mqtt.cc
 * @brief
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2022-01-18
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2022-01-18 <td>1.0     <td>fancyxu   <td>first commit
 * </table>
 */

#include <iostream>
#include <string>

#include "gtest/gtest.h"
#include "mqtt_client_test.h"
#include "qcloud_iot_explorer.h"

namespace mqtt_client_unittest {

static void _file_manage_update_file_callback(UtilsJsonValue file_name, UtilsJsonValue file_type,
                                              UtilsJsonValue version, UtilsJsonValue url, UtilsJsonValue md5sum,
                                              uint32_t file_size, void *usr_data) {
  Log_i("recv update file: file_name=%.*s|type=%.*s|version=%.*s|url=%.*s|md5sum=%.*s|file_size=%u",
        file_name.value_len, file_name.value, file_type.value_len, file_type.value, version.value_len, version.value,
        url.value_len, url.value, md5sum.value_len, md5sum.value, file_size);
}

static void _file_manage_del_file_callback(UtilsJsonValue file_name, UtilsJsonValue file_type, UtilsJsonValue version,
                                           void *usr_data) {
  Log_i("recv del file: file_name=%.*s|type=%.*s|version=%.*s|", file_name.value_len, file_name.value,
        file_type.value_len, file_type.value, version.value_len, version.value);
}

static void _file_manage_report_file_version_reponse_callback(UtilsJsonValue file_list, int result_code,
                                                              void *usr_data) {
  Log_i("recv file version: file_list=%.*s|result_code=%d", file_list, result_code);
}

static void _file_manage_request_file_url_response_callback(UtilsJsonValue url, UtilsJsonValue file_token,
                                                            int result_code, void *usr_data) {
  Log_i("recv request response: url=%.*s|file_token=%.*s|result_code=%d", url.value_len, url.value,
        file_token.value_len, file_token.value, result_code);
}

/**
 * @brief Test file manage.
 *
 */
TEST_F(MqttClientTest, file_manage) {
  char buf[256];
  int buf_len = sizeof(buf);

  IotFileManageFileInfo file_info = {0};

  const char *file_name = "test.txt";
  const char *version = "1.0.0";

  strncpy(file_info.file_name, file_name, strlen(file_name));
  strncpy(file_info.file_version, version, strlen(version));
  file_info.file_type = IOT_FILE_MANAGE_FILE_TYPE_FILE;

  IotFileManageCallback file_manage_callback = {
      .update_file_callback = _file_manage_update_file_callback,
      .del_file_callback = _file_manage_del_file_callback,
      .report_file_version_reponse_callback = _file_manage_report_file_version_reponse_callback,
      .request_file_url_response_callback = _file_manage_request_file_url_response_callback,
  };

  ASSERT_EQ(IOT_FileManage_Init(client, file_manage_callback, client), 0);
  ASSERT_GE(IOT_FileManage_ReportFileList(client, buf, buf_len, NULL, 0), 0);
  ASSERT_GE(IOT_FileManage_Report(client, buf, buf_len, IOT_FILE_MANAGE_REPORT_TYPE_DOWNLOADING, 0, file_name, version),
            0);
  ASSERT_GE(
      IOT_FileManage_Report(client, buf, buf_len, IOT_FILE_MANAGE_REPORT_TYPE_UPGRADE_BEGIN, 100, file_name, version),
      0);
  ASSERT_GE(
      IOT_FileManage_Report(client, buf, buf_len, IOT_FILE_MANAGE_REPORT_TYPE_UPGRADE_SUCCESS, 0, file_name, version),
      0);
  ASSERT_GE(
      IOT_FileManage_Report(client, buf, buf_len, IOT_FILE_MANAGE_REPORT_TYPE_DOWNLOAD_TIMEOUT, 0, file_name, version),
      0);
  ASSERT_GE(
      IOT_FileManage_Report(client, buf, buf_len, IOT_FILE_MANAGE_REPORT_TYPE_FILE_NOT_EXIST, 0, file_name, version),
      0);
  ASSERT_GE(IOT_FileManage_Report(client, buf, buf_len, IOT_FILE_MANAGE_REPORT_TYPE_AUTH_FAIL, 0, file_name, version),
            0);
  ASSERT_GE(
      IOT_FileManage_Report(client, buf, buf_len, IOT_FILE_MANAGE_REPORT_TYPE_MD5_NOT_MATCH, 0, file_name, version), 0);
  ASSERT_GE(
      IOT_FileManage_Report(client, buf, buf_len, IOT_FILE_MANAGE_REPORT_TYPE_UPGRADE_FAIL, 0, file_name, version), 0);
  ASSERT_GE(
      IOT_FileManage_Report(client, buf, buf_len, IOT_FILE_MANAGE_REPORT_TYPE_SPACE_NOT_ENOUGH, 0, file_name, version),
      0);
  ASSERT_GE(IOT_FileManage_Report(client, buf, buf_len, IOT_FILE_MANAGE_REPORT_TYPE_DEL_SUCCESS, 0, file_name, version),
            0);
  ASSERT_GE(IOT_FileManage_Report(client, buf, buf_len, IOT_FILE_MANAGE_REPORT_TYPE_DEL_FAIL, 0, file_name, version),
            0);
  ASSERT_GE(IOT_FileManage_PostRequest(client, buf, buf_len, &file_info, 0), 0);
  ASSERT_EQ(IOT_MQTT_Yield(client, QCLOUD_IOT_MQTT_YIELD_TIMEOUT), 0);
  ASSERT_GE(
      IOT_FileManage_Report(client, buf, buf_len, IOT_FILE_MANAGE_REPORT_TYPE_POST_SUCCESS, 0, file_name, version), 0);
  ASSERT_GE(IOT_FileManage_Report(client, buf, buf_len, IOT_FILE_MANAGE_REPORT_TYPE_POST_FAIL, 0, file_name, version),
            0);

  ASSERT_EQ(IOT_FileManage_GetFileType("FILE", sizeof("FILE") - 1), IOT_FILE_MANAGE_FILE_TYPE_FILE);
  ASSERT_EQ(IOT_FileManage_GetFileType("AUDIO", sizeof("AUDIO") - 1), IOT_FILE_MANAGE_FILE_TYPE_AUDIO);
  ASSERT_EQ(IOT_FileManage_GetFileType("VOICE", sizeof("VOICE") - 1), IOT_FILE_MANAGE_FILE_TYPE_VOICE);
  ASSERT_EQ(IOT_FileManage_GetFileType("VIDEO", sizeof("VIDEO") - 1), IOT_FILE_MANAGE_FILE_TYPE_VIDEO);
  ASSERT_EQ(IOT_FileManage_GetFileType("TEXT", sizeof("TEXT") - 1), IOT_FILE_MANAGE_FILE_TYPE_UNKOWN);

  IOT_FileManage_Deinit(client);
}

}  // namespace mqtt_client_unittest
