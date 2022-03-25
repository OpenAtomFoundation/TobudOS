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
 * @file test_cryptology.cc
 * @brief test cryptology
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2021-07-07
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2021-07-07 <td>1.0     <td>fancyxu   <td>first commit
 * <tr><td>2021-07-08 <td>1.1     <td>fancyxu   <td>fix code standard of IotSha1Context
 * </table>
 */

#include <iostream>
#include <string>

#include "gtest/gtest.h"
#include "utils_base64.h"
#include "utils_hmac.h"
#include "utils_md5.h"
#include "utils_sha1.h"

namespace cryptology_unittest {

/**
 * @brief Test base64.
 *
 */
TEST(CryptologyTest, base64) {
  const uint8_t base64_test_dec[64] = {0x24, 0x48, 0x6E, 0x56, 0x87, 0x62, 0x5A, 0xBD, 0xBF, 0x17, 0xD9, 0xA2, 0xC4,
                                       0x17, 0x1A, 0x01, 0x94, 0xED, 0x8F, 0x1E, 0x11, 0xB3, 0xD7, 0x09, 0x0C, 0xB6,
                                       0xE9, 0x10, 0x6F, 0x22, 0xEE, 0x13, 0xCA, 0xB3, 0x07, 0x05, 0x76, 0xC9, 0xFA,
                                       0x31, 0x6C, 0x08, 0x34, 0xFF, 0x8D, 0xC2, 0x6C, 0x38, 0x00, 0x43, 0xE9, 0x54,
                                       0x97, 0xAF, 0x50, 0x4B, 0xD1, 0x41, 0xBA, 0x95, 0x31, 0x5A, 0x0B, 0x97};

  const uint8_t base64_test_enc[] =
      "JEhuVodiWr2/F9mixBcaAZTtjx4Rs9cJDLbpEG8i7hPK"
      "swcFdsn6MWwINP+Nwmw4AEPpVJevUEvRQbqVMVoLlw==";

  size_t len;
  uint8_t buffer[128];

  ASSERT_EQ(utils_base64encode(buffer, sizeof(buffer), &len, base64_test_dec, sizeof(base64_test_dec)), 0);
  ASSERT_EQ(memcmp(base64_test_enc, buffer, len), 0);
  ASSERT_EQ(utils_base64decode(buffer, sizeof(buffer), &len, base64_test_enc, sizeof(base64_test_enc) - 1), 0);
  ASSERT_EQ(memcmp(base64_test_dec, buffer, len), 0);
}

/**
 * @brief Test hmac.
 *
 */
TEST(CryptologyTest, hmac) {
  /**
   * @brief HMAC-SHA1
   *
   */
  const char test_buf[] = "Here is a test for hmac-sha1!";
  const uint8_t key[] = "0123456789123456";
  const char result[] = "614b650ffefff7862c1bc7fdd9de4e472a8184c4";

  char buf[128] = {0};

  ASSERT_EQ(utils_hmac_sha1(test_buf, strlen(test_buf), key, sizeof(key), buf), 0);
  ASSERT_EQ(memcmp(buf, result, sizeof(result)), 0);
}

/**
 * @brief Test sha1.
 *
 */
TEST(CryptologyTest, sha1) {
  /*
   * FIPS-180-1 test vectors
   */
  const uint8_t sha1_test_buf[3][57] = {{"abc"}, {"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"}, {""}};

  const int sha1_test_buflen[3] = {3, 56, 1000};

  const uint8_t sha1_test_sum[3][20] = {
      {
          0xA9, 0x99, 0x3E, 0x36, 0x47, 0x06, 0x81, 0x6A, 0xBA, 0x3E,
          0x25, 0x71, 0x78, 0x50, 0xC2, 0x6C, 0x9C, 0xD0, 0xD8, 0x9D,
      },
      {
          0x84, 0x98, 0x3E, 0x44, 0x1C, 0x3B, 0xD2, 0x6E, 0xBA, 0xAE,
          0x4A, 0xA1, 0xF9, 0x51, 0x29, 0xE5, 0xE5, 0x46, 0x70, 0xF1,
      },
      {
          0x34, 0xAA, 0x97, 0x3C, 0xD4, 0xC4, 0xDA, 0xA4, 0xF6, 0x1E,
          0xEB, 0x2B, 0xDB, 0xAD, 0x27, 0x31, 0x65, 0x34, 0x01, 0x6F,
      },
  };

  int i, j, buflen, rc = 0;
  uint8_t buf[1024];
  uint8_t sha1sum[20];

  IotSha1Context ctx;

  utils_sha1_init(&ctx);

  for (i = 0; i < 3; i++) {
    utils_sha1_starts(&ctx);

    if (i == 2) {
      memset(buf, 'a', buflen = 1000);
      for (j = 0; j < 1000; j++) {
        utils_sha1_update(&ctx, buf, buflen);
      }
    } else {
      utils_sha1_update(&ctx, sha1_test_buf[i], sha1_test_buflen[i]);
    }

    utils_sha1_finish(&ctx, sha1sum);
    ASSERT_EQ(memcmp(sha1sum, sha1_test_sum[i], 20), 0);
  }

  utils_sha1_free(&ctx);

  utils_sha1(sha1_test_buf[0], 3, sha1sum);
  ASSERT_EQ(memcmp(sha1sum, sha1_test_sum[0], 20), 0);
}

/**
 * @brief Test md5.
 *
 */
TEST(CryptologyTest, md5) {
  /*
   * RFC 1321 test vectors
   */
  const uint8_t md5_test_buf[7][81] = {
      {""},
      {"a"},
      {"abc"},
      {"message digest"},
      {"abcdefghijklmnopqrstuvwxyz"},
      {"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"},
      {"12345678901234567890123456789012345678901234567890123456789012345678901234567890"},
  };

  const size_t md5_test_buflen[7] = {0, 1, 3, 14, 26, 62, 80};

  const char md5_test_sum[7][33] = {
      {"D41D8CD98F00B204E9800998ECF8427E"}, {"0CC175B9C0F1B6A831C399E269772661"}, {"900150983CD24FB0D6963F7D28E17F72"},
      {"F96B697D7CB7938D525A2F31AAF161D0"}, {"C3FCD3D76192E4007DFB496CCA67E13B"}, {"D174AB98D277D9F5A5611C2C9F419D9F"},
      {"57EDF4A22BE3C955AC49DA2E2107B67A"},
  };

  /*
   * Checkup routine
   */
  IotMd5Context md5_ctx;
  for (int i = 0; i < 7; i++) {
    utils_md5_reset(&md5_ctx);
    utils_md5_update(&md5_ctx, md5_test_buf[i], md5_test_buflen[i]);
    utils_md5_finish(&md5_ctx);
    ASSERT_EQ(utils_md5_compare(&md5_ctx, md5_test_sum[i]), 0);
  }
}

}  // namespace cryptology_unittest
