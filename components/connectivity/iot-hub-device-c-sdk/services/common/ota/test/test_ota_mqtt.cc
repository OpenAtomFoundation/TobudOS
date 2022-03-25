/**
 * @copyright
 *
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright(C) 2018 - 2021 THL A29 Limited, a Tencent company.All rights reserved.
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
 * @date 2021-10-19
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2021-10-19 <td>1.0     <td>fancyxu   <td>first commit
 * </table>
 */

#include <iostream>
#include <string>

#include "gtest/gtest.h"
#include "mqtt_client_test.h"
#include "qcloud_iot_common.h"

namespace mqtt_client_unittest {

void _update_firmware_callback(UtilsJsonValue version, UtilsJsonValue url, UtilsJsonValue md5sum, uint32_t file_size,
                               void *usr_data) {
  Log_i("recv firmware: version=%.*s|url=%.*s|md5sum=%.*s|file_size=%u", version.value_len, version.value,
        url.value_len, url.value, md5sum.value_len, md5sum.value, file_size);
}

void _report_version_reply_callback(int result_code, void *usr_data) { Log_i("recv code=%d", result_code); }

/**
 * @brief Test ota mqtt.
 *
 */
TEST_F(MqttClientTest, ota_mqtt) {
  char buf[256];
  int buf_len = sizeof(buf);
  const char *version = "1.0.0";
  IotOTAUpdateCallback ota_callback = {
      .update_firmware_callback = _update_firmware_callback,
      .report_version_reply_callback = _report_version_reply_callback,
  };
  ASSERT_EQ(IOT_OTA_Init(client, ota_callback, client), 0);
  ASSERT_GE(IOT_OTA_ReportVersion(client, buf, buf_len, version), 0);
  ASSERT_GE(IOT_OTA_ReportProgress(client, buf, buf_len, IOT_OTA_REPORT_TYPE_DOWNLOADING, 0, version), 0);
  ASSERT_GE(IOT_OTA_ReportProgress(client, buf, buf_len, IOT_OTA_REPORT_TYPE_DOWNLOADING, 100, version), 0);
  ASSERT_GE(IOT_OTA_ReportProgress(client, buf, buf_len, IOT_OTA_REPORT_TYPE_UPGRADE_BEGIN, 0, version), 0);
  ASSERT_GE(IOT_OTA_ReportProgress(client, buf, buf_len, IOT_OTA_REPORT_TYPE_UPGRADE_SUCCESS, 0, version), 0);
  ASSERT_GE(IOT_OTA_ReportProgress(client, buf, buf_len, IOT_OTA_REPORT_TYPE_DOWNLOAD_TIMEOUT, 0, version), 0);
  ASSERT_GE(IOT_OTA_ReportProgress(client, buf, buf_len, IOT_OTA_REPORT_TYPE_FILE_NOT_EXIST, 0, version), 0);
  ASSERT_GE(IOT_OTA_ReportProgress(client, buf, buf_len, IOT_OTA_REPORT_TYPE_AUTH_FAIL, 0, version), 0);
  ASSERT_GE(IOT_OTA_ReportProgress(client, buf, buf_len, IOT_OTA_REPORT_TYPE_MD5_NOT_MATCH, 0, version), 0);
  ASSERT_GE(IOT_OTA_ReportProgress(client, buf, buf_len, IOT_OTA_REPORT_TYPE_UPGRADE_FAIL, 0, version), 0);
  IOT_OTA_Deinit(client);
}

}  // namespace mqtt_client_unittest
