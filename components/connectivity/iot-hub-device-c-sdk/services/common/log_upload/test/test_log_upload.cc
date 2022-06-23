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
 * @file test_broadcast.cc
 * @brief
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2021-07-18
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2021-07-18 <td>1.0     <td>fancyxu   <td>first commit
 * </table>
 */

#include <iostream>
#include <string>

#include "gtest/gtest.h"
#include "mqtt_client_test.h"
#include "qcloud_iot_common.h"

namespace mqtt_client_unittest {

/**
 * @brief Test log upload
 *
 */
TEST_F(MqttClientTest, log_upload_test) {
  LogHandleFunc func = DEFAULT_LOG_HANDLE_FUNCS;
  func.log_upload = IOT_Log_Upload_AppendToUploadBuffer;
  utils_log_init(func, LOG_LEVEL_DEBUG, 2048);

  LogUploadInitParams log_upload_init_params = DEFAULT_LOG_UPLOAD_INIT_PARAMS;
  log_upload_init_params.device_name = device_info.device_name;
  log_upload_init_params.product_id = device_info.product_id;
  log_upload_init_params.sign_key = device_info.device_secret;

  log_upload_init_params.log_buffer_size = LOG_UPLOAD_BUFFER_SIZE;
  log_upload_init_params.save_log_filename = "./tmp/upload-fail-save.log";
  log_upload_init_params.read_func = HAL_File_Read;
  log_upload_init_params.save_func = HAL_File_Write;
  log_upload_init_params.del_func = HAL_File_Del;
  log_upload_init_params.get_size_func = HAL_File_GetSize;

  ASSERT_EQ(IOT_Log_Upload_InitPre(&log_upload_init_params), 0);
  ASSERT_EQ(IOT_Log_Upload_Init(client), 0);

  uint32_t loop_cnt = 10;
  int rc = 0;
  do {
    Log_d("log upload test debug %d...", loop_cnt);
    Log_i("log upload test info %d...", loop_cnt);
    Log_w("log upload test waring %d...", loop_cnt);
    Log_e("log upload test error %d...", loop_cnt);
    rc = IOT_MQTT_Yield(client, 200);
    ASSERT_EQ(IOT_Log_Upload(rc ? true : false), 0);
  } while (loop_cnt--);

  ASSERT_EQ(IOT_Log_Upload_Deinit(), 0);

  func.log_upload = NULL;
  utils_log_init(func, LOG_LEVEL_DEBUG, 2048);
}

}  // namespace mqtt_client_unittest
