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
 * @file utils_hmac.c
 * @brief Support hmac
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2021-05-31
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2021-05-31 <td>1.0     <td>fancyxu   <td>first commit
 * <tr><td>2021-07-08 <td>1.1     <td>fancyxu   <td>fix code standard of IotSha1Context
 * </table>
 */

#include "utils_hmac.h"

/**
 * @brief inner/outer padding size
 *
 */
#define KEY_IO_PAD_SIZE 64

/**
 * @brief sha1 digest size
 *
 */
#define SHA1_DIGEST_SIZE 20

/**
 * @brief Binary half byte to hex char
 *
 * @param[in] hb half byte
 * @return hex char(0~f)
 */
static char _hb2hex(uint8_t hb)
{
    hb = hb & 0xF;
    return (char)(hb < 10 ? '0' + hb : hb - 10 + 'a');
}

static int _utils_hmac_sha1_process(const char *msg, int msg_len, const uint8_t *key, int key_len, unsigned char *out)
{
    IotSha1Context context;
    unsigned char  k_ipad[KEY_IO_PAD_SIZE]; /* inner padding - key XORd with ipad */
    unsigned char  k_opad[KEY_IO_PAD_SIZE]; /* outer padding - key XORd with opad */

    int i;

    /* start out by storing key in pads */
    memset(k_ipad, 0, sizeof(k_ipad));
    memset(k_opad, 0, sizeof(k_opad));
    memcpy(k_ipad, key, key_len);
    memcpy(k_opad, key, key_len);

    /* XOR key with ipad and opad values */
    for (i = 0; i < KEY_IO_PAD_SIZE; i++) {
        k_ipad[i] ^= 0x36;
        k_opad[i] ^= 0x5c;
    }

    /* perform inner SHA */
    utils_sha1_init(&context);                                  /* init context for 1st pass */
    utils_sha1_starts(&context);                                /* setup context for 1st pass */
    utils_sha1_update(&context, k_ipad, KEY_IO_PAD_SIZE);       /* start with inner pad */
    utils_sha1_update(&context, (unsigned char *)msg, msg_len); /* then text of datagram */
    utils_sha1_finish(&context, out);                           /* finish up 1st pass */

    /* perform outer SHA */
    utils_sha1_init(&context);                            /* init context for 2nd pass */
    utils_sha1_starts(&context);                          /* setup context for 2nd pass */
    utils_sha1_update(&context, k_opad, KEY_IO_PAD_SIZE); /* start with outer pad */
    utils_sha1_update(&context, out, SHA1_DIGEST_SIZE);   /* then results of 1st hash */
    utils_sha1_finish(&context, out);                     /* finish up 2nd pass */
    return 0;
}

/**
 * @brief Get digest of hmac-sha1.
 *
 * @param[in] msg message to hmac-sha1
 * @param[in] msg_len message len
 * @param[in] key key using in hmac-sha1
 * @param[in] key_len key len
 * @param[out] digest digest to calculate
 * @return 0 for success
 */
int utils_hmac_sha1(const char *msg, int msg_len, const uint8_t *key, int key_len, char *digest)
{
    unsigned char out[SHA1_DIGEST_SIZE] = {0};
    if (!msg || !digest || !key) {
        return -1;
    }

    if (key_len > KEY_IO_PAD_SIZE) {
        return -1;
    }

    _utils_hmac_sha1_process(msg, msg_len, key, key_len, out);
    memcpy(digest, out, SHA1_DIGEST_SIZE);
    return 0;
}

/**
 * @brief Get digest hex of hmac-sha1.
 *
 * @param[in] msg message to hmac-sha1
 * @param[in] msg_len message len
 * @param[in] key key using in hmac-sha1
 * @param[in] key_len key len
 * @param[out] digest digest to calculate
 * @return 0 for success
 */
int utils_hmac_sha1_hex(const char *msg, int msg_len, const uint8_t *key, int key_len, char *digest)
{
    unsigned char out[SHA1_DIGEST_SIZE] = {0};
    if (!msg || !digest || !key) {
        return -1;
    }

    if (key_len > KEY_IO_PAD_SIZE) {
        return -1;
    }
    _utils_hmac_sha1_process(msg, msg_len, key, key_len, out);
    for (int i = 0; i < SHA1_DIGEST_SIZE; ++i) {
        digest[i * 2]     = _hb2hex(out[i] >> 4);
        digest[i * 2 + 1] = _hb2hex(out[i]);
    }
    return 0;
}
