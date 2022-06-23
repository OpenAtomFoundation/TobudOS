/**
 * @file test_service_gateway_scene.cc
 * @author {hubert} ({hubertxxu@tencent.com})
 * @brief
 * @version 1.0
 * @date 2022-06-16
 *
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
 * @par Change Log:
 * <table>
 * Date				Version		Author			Description
 * 2022-06-16		1.0			hubertxxu		first commit
 * </table>
 */

#include <iostream>
#include <string>

#include "gtest/gtest.h"
#include "mqtt_client_test.h"
#include "qcloud_iot_explorer.h"

namespace mqtt_client_unittest {
static int _gateway_scene_handles_callback(UtilsJsonValue scene_id, UtilsJsonValue scene_name,
                                           UtilsJsonValue scene_update_time, UtilsJsonValue scene_handles,
                                           void *usr_data) {
  Log_d("recv scene handles :  id:%.*s| name:%.*s| update time:%.*s| handles:%.*s", scene_id.value_len, scene_id.value,
        scene_name.value_len, scene_name.value, scene_update_time.value_len, scene_update_time.value,
        scene_handles.value_len, scene_handles.value);
  return 0;
}

static int _gateway_run_scene_callback(UtilsJsonValue scene_id, void *usr_data) {
  Log_d("run scene: id:%.*s", scene_id.value_len, scene_id.value);
  return 0;
}

static int _gateway_delete_scene_callback(UtilsJsonValue scene_id, void *usr_data) {
  Log_d("delete scene: id:%.*s", scene_id.value_len, scene_id.value);
  return 0;
}

static int _gateway_reload_scene_reply_callback(int result_code, UtilsJsonValue status, UtilsJsonValue scene_result,
                                                void *usr_data) {
  Log_d("reload scene reply: result code:%d|status:%.*s|result:%.*s", result_code, status.value_len, status.value,
        scene_result.value_len, scene_result.value);
  return 0;
}

/**
 * @brief Test gateway scene
 *
 */
TEST_F(MqttClientTest, gateway_scene) {
  char buf[4096];
  int buf_len = sizeof(buf);

  IoTGatewaySceneCallback cbs = {
      .gateway_delete_scene_callback = _gateway_delete_scene_callback,
      .gateway_reload_scene_reply_callback = _gateway_reload_scene_reply_callback,
      .gateway_run_scene_callback = _gateway_run_scene_callback,
      .gateway_scene_handles_callback = _gateway_scene_handles_callback,
  };

  IoTGatewaySceneInnerList scene_inner_list[MAX_LENGTH_INNER_SCENE_LIST];
  for (int i = 0; i < MAX_LENGTH_INNER_SCENE_LIST; i++) {
    HAL_Snprintf(scene_inner_list[i].inner_scene_id, MAX_LENGTH_INNER_SCENE_ID, "inner_scene_id_%d", i);
    HAL_Snprintf(scene_inner_list[i].inner_scene_name, MAX_LENGTH_INNER_SCENE_NAME, "inner_scene_name_%d", i);
  }

  ASSERT_EQ(IOT_GatewayScene_Init(client, cbs, client), 0);
  ASSERT_GE(IOT_GatewayScene_Reload(client, buf, buf_len), 0);
  ASSERT_GE(IOT_GatewayScene_ReportInnerList(client, buf, buf_len, scene_inner_list, MAX_LENGTH_INNER_SCENE_LIST), 0);
  IOT_GatewayScene_Deinit(client);
}

}  // namespace mqtt_client_unittest
