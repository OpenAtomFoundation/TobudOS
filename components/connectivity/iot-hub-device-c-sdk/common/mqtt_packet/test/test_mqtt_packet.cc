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
 * @file test_mqtt_packet.cc
 * @brief unit test for mqtt packet
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2021-06-01
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2021-06-01 <td>1.0     <td>fancyxu   <td>first commit
 * </table>
 */

#include <iostream>
#include <string>

#include "gtest/gtest.h"
#include "mqtt_packet.h"

namespace mqtt_packet_unittest {
/**
 * @brief Test connect packet.
 *
 */
TEST(MQTTPacketTest, CONNECT) {
  uint8_t test_packet[] = {
      0x10, 0x77, 0x00, 0x04, 0x4d, 0x51, 0x54, 0x54, 0x04, 0xc2, 0x00, 0xf0, 0x00, 0x0e, 0x4a, 0x4e, 0x4c, 0x52,
      0x57, 0x4f, 0x33, 0x54, 0x35, 0x39, 0x74, 0x65, 0x73, 0x74, 0x00, 0x28, 0x4a, 0x4e, 0x4c, 0x52, 0x57, 0x4f,
      0x33, 0x54, 0x35, 0x39, 0x74, 0x65, 0x73, 0x74, 0x3b, 0x32, 0x31, 0x30, 0x31, 0x30, 0x34, 0x30, 0x36, 0x3b,
      0x49, 0x41, 0x5a, 0x5a, 0x4a, 0x3b, 0x32, 0x31, 0x34, 0x37, 0x34, 0x38, 0x33, 0x36, 0x34, 0x37, 0x00, 0x31,
      0x66, 0x62, 0x63, 0x39, 0x65, 0x34, 0x36, 0x33, 0x61, 0x32, 0x30, 0x64, 0x66, 0x30, 0x34, 0x35, 0x65, 0x62,
      0x66, 0x34, 0x38, 0x34, 0x35, 0x61, 0x35, 0x39, 0x62, 0x32, 0x61, 0x38, 0x35, 0x66, 0x65, 0x62, 0x34, 0x31,
      0x34, 0x31, 0x30, 0x30, 0x3b, 0x68, 0x6d, 0x61, 0x63, 0x73, 0x68, 0x61, 0x31,
  };

  uint8_t packet_buf[2048];
  std::string user_name = "JNLRWO3T59test;21010406;IAZZJ;2147483647";
  std::string password = "fbc9e463a20df045ebf4845a59b2a85feb414100;hmacsha1";

  MQTTPacketConnectOption options = {
      .mqtt_version = 4,
      .client_id = "JNLRWO3T59test",
      .keep_alive_interval = 240,
      .clean_session = 1,
      .will_flag = 0,
      .will = {0},
      .username = reinterpret_cast<char *>(const_cast<char *>(user_name.c_str())),
      .password = reinterpret_cast<char *>(const_cast<char *>(password.c_str())),
  };

  ASSERT_EQ(mqtt_connect_packet_serialize(packet_buf, sizeof(packet_buf), &options), sizeof(test_packet));
  ASSERT_EQ(memcmp(packet_buf, test_packet, sizeof(test_packet)), 0);

  ASSERT_EQ(mqtt_connect_packet_serialize(packet_buf, 1, &options), MQTT_ERR_SHORT_BUFFER);

  options.username = NULL;
  ASSERT_EQ(mqtt_connect_packet_serialize(packet_buf, sizeof(packet_buf), &options), MQTT_ERR_INVALID_PACKET_TYPE);

  options.password = NULL;
  ASSERT_EQ(mqtt_connect_packet_serialize(packet_buf, sizeof(packet_buf), &options),
            sizeof(test_packet) - user_name.length() - password.length() - 4);

  options.mqtt_version = 5;
  ASSERT_EQ(mqtt_connect_packet_serialize(packet_buf, sizeof(packet_buf), &options), MQTT_ERR_VERSION_UNSUPPORTED);
}

/**
 * @brief Test connack packet.
 *
 */
TEST(MQTTPacketTest, CONNACK) {
  uint8_t test_packet[] = {0x20, 0x02, 0x00, 0x00};
  uint8_t session_present, connack_rc;

  for (uint8_t i = CONNACK_CONNECTION_ACCEPTED; i <= CONNACK_NOT_AUTHORIZED_ERROR; i++) {
    test_packet[3] = i;
    ASSERT_EQ(mqtt_connack_packet_deserialize(test_packet, sizeof(test_packet), &session_present, &connack_rc),
              MQTT_RET_PACKET_OK);
    ASSERT_EQ(connack_rc, i);
  }

  ASSERT_EQ(mqtt_connack_packet_deserialize(test_packet, 2, &session_present, &connack_rc), MQTT_ERR_SHORT_BUFFER);

  test_packet[0] = SUBACK;
  ASSERT_EQ(mqtt_connack_packet_deserialize(test_packet, sizeof(test_packet), &session_present, &connack_rc),
            MQTT_ERR_INVALID_PACKET_TYPE);
}

/**
 * @brief Test publish packet.
 *
 */
TEST(MQTTPacketTest, PUBLISH) {
  uint8_t test_packet[] = {
      0x32, 0x40, 0x00, 0x14, 0x4a, 0x4e, 0x4c, 0x52, 0x57, 0x4f, 0x33, 0x54, 0x35, 0x39, 0x2f, 0x74, 0x65,
      0x73, 0x74, 0x2f, 0x64, 0x61, 0x74, 0x61, 0x70, 0xaf, 0x7b, 0x22, 0x61, 0x63, 0x74, 0x69, 0x6f, 0x6e,
      0x22, 0x3a, 0x20, 0x22, 0x70, 0x75, 0x62, 0x6c, 0x69, 0x73, 0x68, 0x5f, 0x74, 0x65, 0x73, 0x74, 0x22,
      0x2c, 0x20, 0x22, 0x63, 0x6f, 0x75, 0x6e, 0x74, 0x22, 0x3a, 0x20, 0x22, 0x30, 0x22, 0x7d,
  };

  uint8_t packet_buf[128];

  /**
   * @brief QoS1
   *
   */
  char serialize_topic_name[] = "JNLRWO3T59/test/data";
  int serialize_topic_len = strlen(serialize_topic_name);
  uint16_t serialize_packet_id = 28847;
  uint8_t serialize_payload[] = "{\"action\": \"publish_test\", \"count\": \"0\"}";
  int serialize_payload_len = strlen((const char *)serialize_payload);
  MQTTPublishFlags serialize_flags = {0, 1, 0};

  ASSERT_EQ(mqtt_publish_packet_serialize(packet_buf, sizeof(packet_buf), &serialize_flags, serialize_packet_id,
                                          serialize_topic_name, serialize_payload, serialize_payload_len),
            sizeof(test_packet));
  ASSERT_EQ(memcmp(packet_buf, test_packet, sizeof(test_packet)), 0);

  char *deserialize_topic_name;
  int deserialize_topic_len;
  uint16_t deserialize_packet_id = 0;
  uint8_t *deserialize_payload;
  int deserialize_payload_len;
  MQTTPublishFlags deserialize_flags;
  ASSERT_EQ(mqtt_publish_packet_deserialize(test_packet, sizeof(test_packet), &deserialize_flags,
                                            &deserialize_packet_id, &deserialize_topic_name, &deserialize_topic_len,
                                            &deserialize_payload, &deserialize_payload_len),
            0);

  ASSERT_EQ(deserialize_packet_id, serialize_packet_id);
  ASSERT_EQ(deserialize_topic_len, serialize_topic_len);
  ASSERT_EQ(deserialize_payload_len, serialize_payload_len);

  ASSERT_EQ(memcmp(&deserialize_flags, &serialize_flags, sizeof(deserialize_flags)), 0);
  ASSERT_EQ(memcmp(deserialize_topic_name, serialize_topic_name, deserialize_topic_len), 0);
  ASSERT_EQ(memcmp(deserialize_payload, serialize_payload, deserialize_payload_len), 0);

  /**
   * @brief QoS0
   *
   */
  serialize_flags.qos = 0;
  deserialize_packet_id = 0;
  ASSERT_EQ(
      mqtt_publish_packet_serialize(packet_buf, sizeof(packet_buf), &serialize_flags, serialize_packet_id,
                                    serialize_topic_name, serialize_payload, strlen((const char *)serialize_payload)),
      sizeof(test_packet) - 2);
  ASSERT_EQ(mqtt_publish_packet_deserialize(packet_buf, sizeof(packet_buf), &deserialize_flags, &deserialize_packet_id,
                                            &deserialize_topic_name, &deserialize_topic_len, &deserialize_payload,
                                            &deserialize_payload_len),
            MQTT_RET_PACKET_OK);
  ASSERT_NE(deserialize_packet_id, serialize_packet_id);

  /**
   * @brief bad packet
   *
   */
  packet_buf[0] = PUBACK;
  ASSERT_EQ(mqtt_publish_packet_deserialize(packet_buf, sizeof(packet_buf), &deserialize_flags, &deserialize_packet_id,
                                            &deserialize_topic_name, &deserialize_topic_len, &deserialize_payload,
                                            &deserialize_payload_len),
            MQTT_ERR_INVALID_PACKET_TYPE);
}

/**
 * @brief Test puback packet.
 *
 */
TEST(MQTTPacketTest, PUBACK) {
  uint8_t test_packet[] = {0x40, 0x02, 0x70, 0xaf};

  uint8_t packet_buf[128];

  uint16_t serialize_packet_id = 28847;
  ASSERT_EQ(mqtt_puback_packet_serialize(packet_buf, sizeof(packet_buf), serialize_packet_id), 4);
  ASSERT_EQ(memcmp(packet_buf, test_packet, sizeof(test_packet)), 0);

  uint16_t deserialize_packet_id;
  ASSERT_EQ(mqtt_puback_packet_deserialize(test_packet, sizeof(test_packet), &deserialize_packet_id),
            MQTT_RET_PACKET_OK);
  ASSERT_EQ(deserialize_packet_id, serialize_packet_id);
}

/**
 * @brief Test subscribe packet.
 *
 */
TEST(MQTTPacketTest, SUBSCRIBE) {
  uint8_t test_packet[] = {
      0x82, 0x19, 0x70, 0xae, 0x00, 0x14, 0x4a, 0x4e, 0x4c, 0x52, 0x57, 0x4f, 0x33, 0x54,
      0x35, 0x39, 0x2f, 0x74, 0x65, 0x73, 0x74, 0x2f, 0x64, 0x61, 0x74, 0x61, 0x00,
  };

  uint8_t packet_buf[128];

  std::string topic = "JNLRWO3T59/test/data";

  uint16_t serialize_packet_id = 28846;
  char *serialize_topic_name = reinterpret_cast<char *>(const_cast<char *>(topic.c_str()));
  int serialize_qos = 0;

  ASSERT_EQ(mqtt_subscribe_packet_serialize(packet_buf, sizeof(packet_buf), serialize_packet_id, 1,
                                            &serialize_topic_name, &serialize_qos),
            sizeof(test_packet));
  ASSERT_EQ(memcmp(packet_buf, test_packet, sizeof(test_packet)), 0);
}

/**
 * @brief Test suback packet.
 *
 */
TEST(MQTTPacketTest, SUBACK) {
  uint8_t test_packet[] = {0x90, 0x03, 0x70, 0xae, 0x00};

  int sub_count = 0;
  uint16_t packet_id;
  int granted_qos[1];
  ASSERT_EQ(mqtt_suback_packet_deserialize(test_packet, sizeof(test_packet), 1, &sub_count, &packet_id, granted_qos),
            MQTT_RET_PACKET_OK);
  ASSERT_EQ(sub_count, 1);
  ASSERT_EQ(packet_id, 28846);
  ASSERT_EQ(granted_qos[0], 0);
}

/**
 * @brief Test unsubscribe packet.
 *
 */
TEST(MQTTPacketTest, UNSUBSCRIBE) {
  uint8_t test_packet[] = {
      0xa2, 0x18, 0x35, 0x74, 0x00, 0x14, 0x4a, 0x4e, 0x4c, 0x52, 0x57, 0x4f, 0x33,
      0x54, 0x35, 0x39, 0x2f, 0x74, 0x65, 0x73, 0x74, 0x2f, 0x64, 0x61, 0x74, 0x61,
  };

  uint8_t packet_buf[128];
  uint16_t packet_id = 13684;
  std::string topic = "JNLRWO3T59/test/data";
  char *topic_name = reinterpret_cast<char *>(const_cast<char *>(topic.c_str()));

  ASSERT_EQ(mqtt_unsubscribe_packet_serialize(packet_buf, sizeof(packet_buf), packet_id, 1, &topic_name),
            sizeof(test_packet));
  ASSERT_EQ(memcmp(packet_buf, test_packet, sizeof(test_packet)), 0);
}

/**
 * @brief Test connack packet.
 *
 */
TEST(MQTTPacketTest, UNSUBACK) {
  uint8_t test_packet[] = {0xb0, 0x02, 0x92, 0x6b};

  uint16_t packet_id;

  ASSERT_EQ(mqtt_unsuback_packet_deserialize(test_packet, sizeof(test_packet), &packet_id), MQTT_RET_PACKET_OK);
  ASSERT_EQ(packet_id, 37483);
}

/**
 * @brief Test pingreq packet.
 *
 */
TEST(MQTTPacketTest, PINGREQ) {
  uint8_t test_packet[] = {0xc0, 0x00};

  uint8_t packet_buf[128];
  ASSERT_EQ(mqtt_pingreq_packet_serialize(packet_buf, sizeof(packet_buf)), sizeof(test_packet));
  ASSERT_EQ(memcmp(packet_buf, test_packet, sizeof(test_packet)), 0);
}

/**
 * @brief Test pingresp packet.
 *
 */
TEST(MQTTPacketTest, PINGRESP) {
  uint8_t test_packet[] = {0xd0, 0x00};

  ASSERT_EQ(mqtt_pingresp_packet_deserialize(test_packet, sizeof(test_packet)), MQTT_RET_PACKET_OK);
}

/**
 * @brief Test disconnect packet.
 *
 */
TEST(MQTTPacketTest, DISCONNECT) {
  uint8_t test_packet[] = {0xe0, 0x00};

  uint8_t packet_buf[128];
  ASSERT_EQ(mqtt_disconnect_packet_serialize(packet_buf, sizeof(packet_buf)), sizeof(test_packet));
  ASSERT_EQ(memcmp(packet_buf, test_packet, sizeof(test_packet)), 0);
}

}  // namespace mqtt_packet_unittest
