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
 * @file mqtt_client_test.h
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

#ifndef IOT_HUB_DEVICE_C_SDK_SERVICES_COMMON_MQTT_CLIENT_TEST_MQTT_CLIENT_TEST_H_
#define IOT_HUB_DEVICE_C_SDK_SERVICES_COMMON_MQTT_CLIENT_TEST_MQTT_CLIENT_TEST_H_

#include "gtest/gtest.h"
#include "qcloud_iot_common.h"

namespace mqtt_client_unittest {

/**
 * @brief test fixture of mqtt client
 *
 */
class MqttClientTest : public testing::Test {
 protected:
  void SetUp() override;

  void TearDown() override;

  void *client = NULL;
  DeviceInfo device_info;
};

}  // namespace mqtt_client_unittest

#endif  // IOT_HUB_DEVICE_C_SDK_SERVICES_COMMON_MQTT_CLIENT_TEST_MQTT_CLIENT_TEST_H_
