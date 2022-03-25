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
#include "qcloud_iot_hub.h"

namespace mqtt_client_unittest {

/**
 * @brief Callback when MQTT msg arrives @see OnMessageHandler
 *
 * @param[in, out] client pointer to mqtt client
 * @param[in] message publish message from server
 * @param[in] usr_data user data of SubscribeParams, @see SubscribeParams
 */
void on_broadcast_arrived_callback(void *client, const char *msg, int msg_len, void *usr_data) {
  Log_i("Receive broadcast message:%.*s, usr data:%d", msg_len, STRING_PTR_PRINT_SANITY_CHECK(msg),
        *(reinterpret_cast<int *>(usr_data)));
  *(reinterpret_cast<int *>(usr_data)) += 1;
}

/**
 * @brief Test broadcast.
 *
 */
TEST_F(MqttClientTest, broadcast) {
  int usr_data = 0;
  ASSERT_EQ(IOT_Broadcast_Init(client, on_broadcast_arrived_callback, &usr_data), 0);
  ASSERT_EQ(IOT_Broadcast_Deinit(client), 0);
}

}  // namespace mqtt_client_unittest
