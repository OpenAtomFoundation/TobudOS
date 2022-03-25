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
 * @file test_data_template.cc
 * @brief
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2021-09-29
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2021-09-29 <td>1.0     <td>fancyxu   <td>first commit
 * </table>
 */

#include <iostream>
#include <string>

#include "gtest/gtest.h"
#include "mqtt_client_test.h"
#include "qcloud_iot_common.h"
#include "qcloud_iot_explorer.h"

namespace mqtt_client_unittest {

static void _method_control_callback(UtilsJsonValue client_token, UtilsJsonValue params, void *usr_data) {
  char buf[256];
  Log_i("recv msg[%.*s]: params=%.*s", client_token.value_len, client_token.value, params.value_len, params.value);
  IOT_DataTemplate_PropertyControlReply(usr_data, buf, sizeof(buf), 0, client_token);
}

static void _method_report_reply_callback(UtilsJsonValue client_token, int code, void *usr_data) {
  Log_i("recv msg[%.*s]: code=%d", client_token.value_len, client_token.value, code);
}

static void _method_get_status_reply_callback(UtilsJsonValue client_token, int code, UtilsJsonValue reported,
                                              UtilsJsonValue control, void *usr_data) {
  char buf[256];
  Log_i("recv msg[%.*s]: code=%d|reported=%.*s|control=%.*s", client_token.value_len, client_token.value, code,
        reported.value_len, STRING_PTR_PRINT_SANITY_CHECK(reported.value), control.value_len,
        STRING_PTR_PRINT_SANITY_CHECK(control.value));
  IOT_DataTemplate_PropertyClearControl(usr_data, buf, sizeof(buf));
}

static void _method_report_info_reply_callback(UtilsJsonValue client_token, int code, void *usr_data) {
  Log_i("recv msg[%.*s]: code=%d", client_token.value_len, client_token.value, code);
}

static void _method_clear_control_reply_callback(UtilsJsonValue client_token, int code, void *usr_data) {
  Log_i("recv msg[%.*s]: code=%d", client_token.value_len, client_token.value, code);
}

static void _method_event_reply_callback(UtilsJsonValue client_token, int code, void *usr_data) {
  Log_i("recv msg[%.*s]: code=%d", client_token.value_len, client_token.value, code);
}

static void _method_action_callback(UtilsJsonValue client_token, UtilsJsonValue action_id, UtilsJsonValue params,
                                    void *usr_data) {
  char buf[256];

  Log_i("recv msg[%.*s]: action_id=%.*s|params=%.*s", client_token.value_len, client_token.value, action_id.value_len,
        action_id.value, params.value_len, params.value);

  IotDataTemplateActionReply reply = {
      .client_token = client_token,
      .code = 0,
      .response = "{\"err_code\":0}",
  };

  ASSERT_GE(IOT_DataTemplate_ActionReply(usr_data, buf, sizeof(buf), reply), 0);
}

/**
 * @brief Test data template property.
 *
 */
TEST_F(MqttClientTest, data_template_property) {
  char buf[1024];

  IotDataTemplateCallback callback = {
      .property_callback = {.method_control_callback = _method_control_callback,
                            .method_report_reply_callback = _method_report_reply_callback,
                            .method_get_status_reply_callback = _method_get_status_reply_callback,
                            .method_report_info_reply_callback = _method_report_info_reply_callback,
                            .method_clear_control_reply_callback = _method_clear_control_reply_callback},
      .event_callback = {.method_event_reply_callback = NULL},
      .action_callback = {.method_action_callback = NULL},
  };

  const char *report_info =
      "{\"module_hardinfo\":\"ESP8266\",\"module_softinfo\":\"V1.0\", \"fw_ver\":\"4.0.0\", "
      "\"imei\":\"11-22-33-44\",\"lat\":\"22.546015\",\"lon\":\"113.941125\",\"mac\":\"11:22:33:44:55:66\",\"device_"
      "label\":{\"append_"
      "info\":\"your self defined info\"}}";

  ASSERT_EQ(IOT_DataTemplate_Init(client, callback, client), 0);
  ASSERT_EQ(IOT_DataTemplate_PropertyReportInfo(client, buf, sizeof(buf), report_info), 0);
  ASSERT_EQ(IOT_DataTemplate_PropertyGetStatus(client, buf, sizeof(buf)), 0);
  IOT_DataTemplate_Deinit(client);
}

/**
 * @brief Test data template event.
 *
 */
TEST_F(MqttClientTest, data_template_event) {
  char buf[1024];
  IotDataTemplateCallback callback = {
      .property_callback = {0},
      .event_callback = {.method_event_reply_callback = _method_event_reply_callback},
      .action_callback = {.method_action_callback = _method_action_callback},
  };

  const char *report_property = "{\"power_switch\":0}";

  IotDataTemplateEventData event_data = {
      .event_id = "status_report",
      .type = IOT_DATA_TEMPLATE_EVENT_TYPE_INFO,
      .params = "{\"status\":0,\"message\":\"ok\"}",
  };

  ASSERT_EQ(IOT_DataTemplate_Init(client, callback, client), 0);
  ASSERT_GE(IOT_DataTemplate_EventPost(client, buf, sizeof(buf), event_data), 0);
  IOT_DataTemplate_Deinit(client);
}

/**
 * @brief Test data template action.
 *
 */
TEST_F(MqttClientTest, data_template_action) {
  char buf[1024];
  IotDataTemplateCallback callback = {
      .property_callback = {0},
      .event_callback = {.method_event_reply_callback = NULL},
      .action_callback = {.method_action_callback = _method_action_callback},
  };

  ASSERT_EQ(IOT_DataTemplate_Init(client, callback, client), 0);
  IOT_DataTemplate_Deinit(client);
}

}  // namespace mqtt_client_unittest
