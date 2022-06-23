/**
 * @file test_dynreg.cc
 * @author {hubert} ({hubertxxu@tencent.com})
 * @brief
 * @version 1.0
 * @date 2022-01-26
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
 * 2022-01-26		1.0			hubertxxu		first commit
 * </table>
 */

#include "gtest/gtest.h"
#include "mqtt_client_test.h"
#include "qcloud_iot_common.h"

namespace mqtt_client_unittest {

TEST_F(MqttClientTest, dynreg_test) {
  DeviceInfo device_info;
  ASSERT_EQ(HAL_GetDevInfo(&device_info), 0);
  strncpy(device_info.device_name, "dynreg_test", 12);
  EXPECT_EQ(IOT_DynReg_Device(&device_info), 0);
}

}  // namespace mqtt_client_unittest
