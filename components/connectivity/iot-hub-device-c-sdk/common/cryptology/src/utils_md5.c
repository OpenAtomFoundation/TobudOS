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
 * @file utils_md5.c
 * @brief
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2021-10-27
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2021-10-27 <td>1.0     <td>fancyxu   <td>first commit
 * </table>
 */

#include "utils_md5.h"

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

/**
 * @brief Lower md5sum.
 *
 * @param[out] md5_lower md5 sum lower
 * @param[in] md5 md5 sum
 */
static void _lower(char md5_lower[33], const char md5[33])
{
    int i = 0;
    for (i = 0; i < 32; i++) {
        md5_lower[i] = md5[i];
        if (md5[i] >= 'A' && md5[i] <= 'F') {
            md5_lower[i] += ('a' - 'A');
        }
    }
    md5_lower[32] = '\0';
}

#ifdef AUTH_WITH_NO_TLS
/**
 * @brief 32-bit integer manipulation macros (little endian)
 *
 */
#define IOT_MD5_GET_UINT32_LE(n, b, i)                                                                \
    {                                                                                                 \
        (n) = ((uint32_t)(b)[(i)]) | ((uint32_t)(b)[(i) + 1] << 8) | ((uint32_t)(b)[(i) + 2] << 16) | \
              ((uint32_t)(b)[(i) + 3] << 24);                                                         \
    }

#define IOT_MD5_PUT_UINT32_LE(n, b, i)                \
    {                                                 \
        (b)[(i)]     = (uint8_t)(((n)) & 0xFF);       \
        (b)[(i) + 1] = (uint8_t)(((n) >> 8) & 0xFF);  \
        (b)[(i) + 2] = (uint8_t)(((n) >> 16) & 0xFF); \
        (b)[(i) + 3] = (uint8_t)(((n) >> 24) & 0xFF); \
    }

/**
 * @brief Padding for md5.
 *
 */
static const uint8_t sg_iot_md5_padding[64] = {
    0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

/**
 * @brief Initialize md5 context.
 *
 * @param[in,out] ctx MD5 context
 */
static void _utils_md5_init(IotMd5Context *ctx)
{
    memset(ctx, 0, sizeof(IotMd5Context));
}

/**
 * @brief Start md5 calculate.
 *
 * @param[in,out] ctx MD5 context
 */
static void _utils_md5_start(IotMd5Context *ctx)
{
    ctx->total[0] = 0;
    ctx->total[1] = 0;

    ctx->state[0] = 0x67452301;
    ctx->state[1] = 0xEFCDAB89;
    ctx->state[2] = 0x98BADCFE;
    ctx->state[3] = 0x10325476;
}

/**
 * @brief Calculate md5.
 *
 * @param[in,out] ctx MD5 context
 * @param[in] data data to calculate
 */
static void _utils_md5_process(IotMd5Context *ctx, const uint8_t data[64])
{
    uint32_t X[16], A, B, C, D;

    IOT_MD5_GET_UINT32_LE(X[0], data, 0);
    IOT_MD5_GET_UINT32_LE(X[1], data, 4);
    IOT_MD5_GET_UINT32_LE(X[2], data, 8);
    IOT_MD5_GET_UINT32_LE(X[3], data, 12);
    IOT_MD5_GET_UINT32_LE(X[4], data, 16);
    IOT_MD5_GET_UINT32_LE(X[5], data, 20);
    IOT_MD5_GET_UINT32_LE(X[6], data, 24);
    IOT_MD5_GET_UINT32_LE(X[7], data, 28);
    IOT_MD5_GET_UINT32_LE(X[8], data, 32);
    IOT_MD5_GET_UINT32_LE(X[9], data, 36);
    IOT_MD5_GET_UINT32_LE(X[10], data, 40);
    IOT_MD5_GET_UINT32_LE(X[11], data, 44);
    IOT_MD5_GET_UINT32_LE(X[12], data, 48);
    IOT_MD5_GET_UINT32_LE(X[13], data, 52);
    IOT_MD5_GET_UINT32_LE(X[14], data, 56);
    IOT_MD5_GET_UINT32_LE(X[15], data, 60);

#define S(x, n) ((x << n) | ((x & 0xFFFFFFFF) >> (32 - n)))

#define P(a, b, c, d, k, s, t)      \
    {                               \
        a += F(b, c, d) + X[k] + t; \
        a = S(a, s) + b;            \
    }

    A = ctx->state[0];
    B = ctx->state[1];
    C = ctx->state[2];
    D = ctx->state[3];

#define F(x, y, z) (z ^ (x & (y ^ z)))

    P(A, B, C, D, 0, 7, 0xD76AA478);
    P(D, A, B, C, 1, 12, 0xE8C7B756);
    P(C, D, A, B, 2, 17, 0x242070DB);
    P(B, C, D, A, 3, 22, 0xC1BDCEEE);
    P(A, B, C, D, 4, 7, 0xF57C0FAF);
    P(D, A, B, C, 5, 12, 0x4787C62A);
    P(C, D, A, B, 6, 17, 0xA8304613);
    P(B, C, D, A, 7, 22, 0xFD469501);
    P(A, B, C, D, 8, 7, 0x698098D8);
    P(D, A, B, C, 9, 12, 0x8B44F7AF);
    P(C, D, A, B, 10, 17, 0xFFFF5BB1);
    P(B, C, D, A, 11, 22, 0x895CD7BE);
    P(A, B, C, D, 12, 7, 0x6B901122);
    P(D, A, B, C, 13, 12, 0xFD987193);
    P(C, D, A, B, 14, 17, 0xA679438E);
    P(B, C, D, A, 15, 22, 0x49B40821);

#undef F

#define F(x, y, z) (y ^ (z & (x ^ y)))

    P(A, B, C, D, 1, 5, 0xF61E2562);
    P(D, A, B, C, 6, 9, 0xC040B340);
    P(C, D, A, B, 11, 14, 0x265E5A51);
    P(B, C, D, A, 0, 20, 0xE9B6C7AA);
    P(A, B, C, D, 5, 5, 0xD62F105D);
    P(D, A, B, C, 10, 9, 0x02441453);
    P(C, D, A, B, 15, 14, 0xD8A1E681);
    P(B, C, D, A, 4, 20, 0xE7D3FBC8);
    P(A, B, C, D, 9, 5, 0x21E1CDE6);
    P(D, A, B, C, 14, 9, 0xC33707D6);
    P(C, D, A, B, 3, 14, 0xF4D50D87);
    P(B, C, D, A, 8, 20, 0x455A14ED);
    P(A, B, C, D, 13, 5, 0xA9E3E905);
    P(D, A, B, C, 2, 9, 0xFCEFA3F8);
    P(C, D, A, B, 7, 14, 0x676F02D9);
    P(B, C, D, A, 12, 20, 0x8D2A4C8A);

#undef F

#define F(x, y, z) (x ^ y ^ z)

    P(A, B, C, D, 5, 4, 0xFFFA3942);
    P(D, A, B, C, 8, 11, 0x8771F681);
    P(C, D, A, B, 11, 16, 0x6D9D6122);
    P(B, C, D, A, 14, 23, 0xFDE5380C);
    P(A, B, C, D, 1, 4, 0xA4BEEA44);
    P(D, A, B, C, 4, 11, 0x4BDECFA9);
    P(C, D, A, B, 7, 16, 0xF6BB4B60);
    P(B, C, D, A, 10, 23, 0xBEBFBC70);
    P(A, B, C, D, 13, 4, 0x289B7EC6);
    P(D, A, B, C, 0, 11, 0xEAA127FA);
    P(C, D, A, B, 3, 16, 0xD4EF3085);
    P(B, C, D, A, 6, 23, 0x04881D05);
    P(A, B, C, D, 9, 4, 0xD9D4D039);
    P(D, A, B, C, 12, 11, 0xE6DB99E5);
    P(C, D, A, B, 15, 16, 0x1FA27CF8);
    P(B, C, D, A, 2, 23, 0xC4AC5665);

#undef F

#define F(x, y, z) (y ^ (x | ~z))

    P(A, B, C, D, 0, 6, 0xF4292244);
    P(D, A, B, C, 7, 10, 0x432AFF97);
    P(C, D, A, B, 14, 15, 0xAB9423A7);
    P(B, C, D, A, 5, 21, 0xFC93A039);
    P(A, B, C, D, 12, 6, 0x655B59C3);
    P(D, A, B, C, 3, 10, 0x8F0CCC92);
    P(C, D, A, B, 10, 15, 0xFFEFF47D);
    P(B, C, D, A, 1, 21, 0x85845DD1);
    P(A, B, C, D, 8, 6, 0x6FA87E4F);
    P(D, A, B, C, 15, 10, 0xFE2CE6E0);
    P(C, D, A, B, 6, 15, 0xA3014314);
    P(B, C, D, A, 13, 21, 0x4E0811A1);
    P(A, B, C, D, 4, 6, 0xF7537E82);
    P(D, A, B, C, 11, 10, 0xBD3AF235);
    P(C, D, A, B, 2, 15, 0x2AD7D2BB);
    P(B, C, D, A, 9, 21, 0xEB86D391);

#undef F

    ctx->state[0] += A;
    ctx->state[1] += B;
    ctx->state[2] += C;
    ctx->state[3] += D;
}

/**
 * @brief Reset MD5 context.
 *
 * @param[in,out] ctx MD5 context
 */
void utils_md5_reset(IotMd5Context *ctx)
{
    memset(ctx, 0, sizeof(IotMd5Context));
    _utils_md5_init(ctx);
    _utils_md5_start(ctx);
}

/**
 * @brief MD5 update.
 *
 * @param[in,out] ctx MD5 context
 * @param[in] input input data
 * @param[in] ilen data length
 */
void utils_md5_update(IotMd5Context *ctx, const uint8_t *input, size_t ilen)
{
    size_t   fill;
    uint32_t left;

    if (ilen == 0) {
        return;
    }

    left = ctx->total[0] & 0x3F;
    fill = 64 - left;

    ctx->total[0] += (uint32_t)ilen;
    ctx->total[0] &= 0xFFFFFFFF;

    if (ctx->total[0] < (uint32_t)ilen) {
        ctx->total[1]++;
    }

    if (left && ilen >= fill) {
        memcpy((void *)(ctx->buffer + left), input, fill);
        _utils_md5_process(ctx, ctx->buffer);
        input += fill;
        ilen -= fill;
        left = 0;
    }

    while (ilen >= 64) {
        _utils_md5_process(ctx, input);
        input += 64;
        ilen -= 64;
    }

    if (ilen > 0) {
        memcpy((void *)(ctx->buffer + left), input, ilen);
    }
}

/**
 * @brief Finish MD5 calculation, result will store in md5sum.
 *
 * @param[in,out] ctx MD5 context
 */
void utils_md5_finish(IotMd5Context *ctx)
{
    int      i;
    uint32_t last, padn;
    uint32_t high, low;
    uint8_t  msglen[8];

    uint8_t output_tmp[16];

    high = (ctx->total[0] >> 29) | (ctx->total[1] << 3);
    low  = (ctx->total[0] << 3);

    IOT_MD5_PUT_UINT32_LE(low, msglen, 0);
    IOT_MD5_PUT_UINT32_LE(high, msglen, 4);

    last = ctx->total[0] & 0x3F;
    padn = (last < 56) ? (56 - last) : (120 - last);

    utils_md5_update(ctx, sg_iot_md5_padding, padn);
    utils_md5_update(ctx, msglen, 8);

    IOT_MD5_PUT_UINT32_LE(ctx->state[0], output_tmp, 0);
    IOT_MD5_PUT_UINT32_LE(ctx->state[1], output_tmp, 4);
    IOT_MD5_PUT_UINT32_LE(ctx->state[2], output_tmp, 8);
    IOT_MD5_PUT_UINT32_LE(ctx->state[3], output_tmp, 12);

    for (i = 0; i < 16; ++i) {
        ctx->md5sum[i * 2]     = _hb2hex(output_tmp[i] >> 4);
        ctx->md5sum[i * 2 + 1] = _hb2hex(output_tmp[i]);
    }
    ctx->md5sum[32] = '\0';
}

/**
 * @brief Compare md5sum with context.
 *
 * @param[in,out] ctx MD5 context
 * @param[in] md5sum md5sum to compare
 * @return 0 for the same
 */
int utils_md5_compare(IotMd5Context *ctx, const char md5sum[33])
{
    char md5sum_lower[33];
    _lower(md5sum_lower, md5sum);
    return strncmp(ctx->md5sum, md5sum_lower, 32);
}
#else

/**
 * @brief Reset MD5 context.
 *
 * @param[in,out] ctx MD5 context
 */
void utils_md5_reset(IotMd5Context *ctx)
{
    memset(ctx, 0, sizeof(IotMd5Context));
    mbedtls_md5_init(&ctx->ctx);
    mbedtls_md5_starts(&ctx->ctx);
}

/**
 * @brief MD5 update.
 *
 * @param[in,out] ctx MD5 context
 * @param[in] input input data
 * @param[in] ilen data length
 */
void utils_md5_update(IotMd5Context *ctx, const uint8_t *input, size_t ilen)
{
    mbedtls_md5_update(&ctx->ctx, input, ilen);
}

/**
 * @brief Finish MD5 calculation, result will store in md5sum.
 *
 * @param[in,out] ctx MD5 context
 */
void utils_md5_finish(IotMd5Context *ctx)
{
    int     i;
    uint8_t output_tmp[16];
    mbedtls_md5_finish(&ctx->ctx, output_tmp);
    for (i = 0; i < 16; ++i) {
        ctx->md5sum[i * 2]     = _hb2hex(output_tmp[i] >> 4);
        ctx->md5sum[i * 2 + 1] = _hb2hex(output_tmp[i]);
    }
    ctx->md5sum[32] = '\0';
}

/**
 * @brief Compare md5sum with context.
 *
 * @param[in,out] ctx MD5 context
 * @param[in] md5sum md5sum to compare
 * @return 0 for the same
 */
int utils_md5_compare(IotMd5Context *ctx, const char md5sum[33])
{
    char md5sum_lower[33];
    _lower(md5sum_lower, md5sum);
    return strncmp(ctx->md5sum, md5sum_lower, 32);
}
#endif