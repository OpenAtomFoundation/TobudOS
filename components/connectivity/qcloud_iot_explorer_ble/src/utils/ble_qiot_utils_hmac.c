/*
 * Copyright (C) 2019 THL A29 Limited, a Tencent company. All rights reserved.
 * Licensed under the MIT License (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT
 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "ble_qiot_log.h"
#include "ble_qiot_sha1.h"
#include "ble_qiot_hmac.h"

#define KEY_IOPAD_SIZE 64

int8_t utils_hb2hex(uint8_t hb)
{
    hb = hb & 0xF;
    return (int8_t)(hb < 10 ? '0' + hb : hb - 10 + 'a');
}

void utils_hmac_sha1(const char *msg, int msg_len, char *digest, const char *key, int key_len)
{
    if ((NULL == msg) || (NULL == digest) || (NULL == key)) {
        ble_qiot_log_e("parameter is Null,failed!");
        return;
    }

    if (key_len > KEY_IOPAD_SIZE) {
        ble_qiot_log_e("key_len > size(%d) of array", KEY_IOPAD_SIZE);
        return;
    }

    iot_sha1_context context;
    unsigned char    k_ipad[KEY_IOPAD_SIZE]; /* inner padding - key XORd with ipad  */
    unsigned char    k_opad[KEY_IOPAD_SIZE]; /* outer padding - key XORd with opad */
    unsigned char    out[SHA1_DIGEST_SIZE];
    int              i;

    /* start out by storing key in pads */
    memset(k_ipad, 0, sizeof(k_ipad));
    memset(k_opad, 0, sizeof(k_opad));
    memcpy(k_ipad, key, key_len);
    memcpy(k_opad, key, key_len);

    /* XOR key with ipad and opad values */
    for (i = 0; i < KEY_IOPAD_SIZE; i++) {
        k_ipad[i] ^= 0x36;
        k_opad[i] ^= 0x5c;
    }

    /* perform inner SHA */
    utils_sha1_init(&context);                                  /* init context for 1st pass */
    utils_sha1_starts(&context);                                /* setup context for 1st pass */
    utils_sha1_update(&context, k_ipad, KEY_IOPAD_SIZE);        /* start with inner pad */
    utils_sha1_update(&context, (unsigned char *)msg, msg_len); /* then text of datagram */
    utils_sha1_finish(&context, out);                           /* finish up 1st pass */

    /* perform outer SHA */
    utils_sha1_init(&context);                           /* init context for 2nd pass */
    utils_sha1_starts(&context);                         /* setup context for 2nd pass */
    utils_sha1_update(&context, k_opad, KEY_IOPAD_SIZE); /* start with outer pad */
    utils_sha1_update(&context, out, SHA1_DIGEST_SIZE);  /* then results of 1st hash */
    utils_sha1_finish(&context, out);                    /* finish up 2nd pass */

    memcpy(digest, out, SHA1_DIGEST_SIZE);
    /*    for (i = 0; i < SHA1_DIGEST_SIZE; ++i) {
            digest[i * 2]     = utils_hb2hex(out[i] >> 4);
            digest[i * 2 + 1] = utils_hb2hex(out[i]);
        }*/
}

#ifdef __cplusplus
}
#endif
