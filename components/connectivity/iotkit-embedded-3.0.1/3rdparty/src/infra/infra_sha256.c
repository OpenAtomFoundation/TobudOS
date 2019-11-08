/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include "infra_config.h"

#ifdef INFRA_SHA256

#define INFRA_SHA256_SMALLER

#include <stdlib.h>
#include <string.h>
#include "infra_sha256.h"

#define SHA256_KEY_IOPAD_SIZE   (64)
#define SHA256_DIGEST_SIZE      (32)

/*
 * 32-bit integer manipulation macros (big endian)
 */
#ifndef GET_UINT32_BE
#define GET_UINT32_BE(n,b,i)                            \
    do {                                                    \
        (n) = ( (uint32_t) (b)[(i)    ] << 24 )             \
              | ( (uint32_t) (b)[(i) + 1] << 16 )             \
              | ( (uint32_t) (b)[(i) + 2] <<  8 )             \
              | ( (uint32_t) (b)[(i) + 3]       );            \
    } while( 0 )
#endif

#ifndef PUT_UINT32_BE
#define PUT_UINT32_BE(n,b,i)                            \
    do {                                                    \
        (b)[(i)    ] = (unsigned char) ( (n) >> 24 );       \
        (b)[(i) + 1] = (unsigned char) ( (n) >> 16 );       \
        (b)[(i) + 2] = (unsigned char) ( (n) >>  8 );       \
        (b)[(i) + 3] = (unsigned char) ( (n)       );       \
    } while( 0 )
#endif


static void utils_sha256_zeroize(void *v, uint32_t n)
{
    volatile unsigned char *p = v;
    while (n--) {
        *p++ = 0;
    }
}

void utils_sha256_init(iot_sha256_context *ctx)
{
    memset(ctx, 0, sizeof(iot_sha256_context));
}

void utils_sha256_free(iot_sha256_context *ctx)
{
    if (NULL == ctx) {
        return;
    }

    utils_sha256_zeroize(ctx, sizeof(iot_sha256_context));
}

void utils_sha256_starts(iot_sha256_context *ctx)
{
    int is224 = 0;
    ctx->total[0] = 0;
    ctx->total[1] = 0;

    if (is224 == 0) {
        /* SHA-256 */
        ctx->state[0] = 0x6A09E667;
        ctx->state[1] = 0xBB67AE85;
        ctx->state[2] = 0x3C6EF372;
        ctx->state[3] = 0xA54FF53A;
        ctx->state[4] = 0x510E527F;
        ctx->state[5] = 0x9B05688C;
        ctx->state[6] = 0x1F83D9AB;
        ctx->state[7] = 0x5BE0CD19;
    }

    ctx->is224 = is224;
}

static const uint32_t K[] = {
    0x428A2F98, 0x71374491, 0xB5C0FBCF, 0xE9B5DBA5,
    0x3956C25B, 0x59F111F1, 0x923F82A4, 0xAB1C5ED5,
    0xD807AA98, 0x12835B01, 0x243185BE, 0x550C7DC3,
    0x72BE5D74, 0x80DEB1FE, 0x9BDC06A7, 0xC19BF174,
    0xE49B69C1, 0xEFBE4786, 0x0FC19DC6, 0x240CA1CC,
    0x2DE92C6F, 0x4A7484AA, 0x5CB0A9DC, 0x76F988DA,
    0x983E5152, 0xA831C66D, 0xB00327C8, 0xBF597FC7,
    0xC6E00BF3, 0xD5A79147, 0x06CA6351, 0x14292967,
    0x27B70A85, 0x2E1B2138, 0x4D2C6DFC, 0x53380D13,
    0x650A7354, 0x766A0ABB, 0x81C2C92E, 0x92722C85,
    0xA2BFE8A1, 0xA81A664B, 0xC24B8B70, 0xC76C51A3,
    0xD192E819, 0xD6990624, 0xF40E3585, 0x106AA070,
    0x19A4C116, 0x1E376C08, 0x2748774C, 0x34B0BCB5,
    0x391C0CB3, 0x4ED8AA4A, 0x5B9CCA4F, 0x682E6FF3,
    0x748F82EE, 0x78A5636F, 0x84C87814, 0x8CC70208,
    0x90BEFFFA, 0xA4506CEB, 0xBEF9A3F7, 0xC67178F2,
};

#define  SHR(x,n) ((x & 0xFFFFFFFF) >> n)
#define ROTR(x,n) (SHR(x,n) | (x << (32 - n)))

#define S0(x) (ROTR(x, 7) ^ ROTR(x,18) ^  SHR(x, 3))
#define S1(x) (ROTR(x,17) ^ ROTR(x,19) ^  SHR(x,10))

#define S2(x) (ROTR(x, 2) ^ ROTR(x,13) ^ ROTR(x,22))
#define S3(x) (ROTR(x, 6) ^ ROTR(x,11) ^ ROTR(x,25))

#define F0(x,y,z) ((x & y) | (z & (x | y)))
#define F1(x,y,z) (z ^ (x & (y ^ z)))

#define R(t)                                    \
    (                                               \
            W[t] = S1(W[t -  2]) + W[t -  7] +          \
                   S0(W[t - 15]) + W[t - 16]            \
    )

#define P(a,b,c,d,e,f,g,h,x,K)                  \
    {                                               \
        temp1 = h + S3(e) + F1(e,f,g) + K + x;      \
        temp2 = S2(a) + F0(a,b,c);                  \
        d += temp1; h = temp1 + temp2;              \
    }

void utils_sha256_process(iot_sha256_context *ctx, const unsigned char data[64])
{
    uint32_t temp1, temp2, W[64];
    uint32_t A[8];
    unsigned int i;

    for (i = 0; i < 8; i++) {
        A[i] = ctx->state[i];
    }

#if defined(INFRA_SHA256_SMALLER)
    for (i = 0; i < 64; i++) {
        if (i < 16) {
            GET_UINT32_BE(W[i], data, 4 * i);
        } else {
            R(i);
        }

        P(A[0], A[1], A[2], A[3], A[4], A[5], A[6], A[7], W[i], K[i]);

        temp1 = A[7];
        A[7] = A[6];
        A[6] = A[5];
        A[5] = A[4];
        A[4] = A[3];
        A[3] = A[2];
        A[2] = A[1];
        A[1] = A[0];
        A[0] = temp1;
    }
#else /* INFRA_SHA256_SMALLER */
    for (i = 0; i < 16; i++) {
        GET_UINT32_BE(W[i], data, 4 * i);
    }

    for (i = 0; i < 16; i += 8) {
        P(A[0], A[1], A[2], A[3], A[4], A[5], A[6], A[7], W[i + 0], K[i + 0]);
        P(A[7], A[0], A[1], A[2], A[3], A[4], A[5], A[6], W[i + 1], K[i + 1]);
        P(A[6], A[7], A[0], A[1], A[2], A[3], A[4], A[5], W[i + 2], K[i + 2]);
        P(A[5], A[6], A[7], A[0], A[1], A[2], A[3], A[4], W[i + 3], K[i + 3]);
        P(A[4], A[5], A[6], A[7], A[0], A[1], A[2], A[3], W[i + 4], K[i + 4]);
        P(A[3], A[4], A[5], A[6], A[7], A[0], A[1], A[2], W[i + 5], K[i + 5]);
        P(A[2], A[3], A[4], A[5], A[6], A[7], A[0], A[1], W[i + 6], K[i + 6]);
        P(A[1], A[2], A[3], A[4], A[5], A[6], A[7], A[0], W[i + 7], K[i + 7]);
    }

    for (i = 16; i < 64; i += 8) {
        P(A[0], A[1], A[2], A[3], A[4], A[5], A[6], A[7], R(i + 0), K[i + 0]);
        P(A[7], A[0], A[1], A[2], A[3], A[4], A[5], A[6], R(i + 1), K[i + 1]);
        P(A[6], A[7], A[0], A[1], A[2], A[3], A[4], A[5], R(i + 2), K[i + 2]);
        P(A[5], A[6], A[7], A[0], A[1], A[2], A[3], A[4], R(i + 3), K[i + 3]);
        P(A[4], A[5], A[6], A[7], A[0], A[1], A[2], A[3], R(i + 4), K[i + 4]);
        P(A[3], A[4], A[5], A[6], A[7], A[0], A[1], A[2], R(i + 5), K[i + 5]);
        P(A[2], A[3], A[4], A[5], A[6], A[7], A[0], A[1], R(i + 6), K[i + 6]);
        P(A[1], A[2], A[3], A[4], A[5], A[6], A[7], A[0], R(i + 7), K[i + 7]);
    }
#endif /* INFRA_SHA256_SMALLER */

    for (i = 0; i < 8; i++) {
        ctx->state[i] += A[i];
    }
}
void utils_sha256_update(iot_sha256_context *ctx, const unsigned char *input, uint32_t ilen)
{
    size_t fill;
    uint32_t left;

    if (ilen == 0) {
        return;
    }

    left = ctx->total[0] & 0x3F;
    fill = 64 - left;

    ctx->total[0] += (uint32_t) ilen;
    ctx->total[0] &= 0xFFFFFFFF;

    if (ctx->total[0] < (uint32_t) ilen) {
        ctx->total[1]++;
    }

    if (left && ilen >= fill) {
        memcpy((void *)(ctx->buffer + left), input, fill);
        utils_sha256_process(ctx, ctx->buffer);
        input += fill;
        ilen  -= fill;
        left = 0;
    }

    while (ilen >= 64) {
        utils_sha256_process(ctx, input);
        input += 64;
        ilen  -= 64;
    }

    if (ilen > 0) {
        memcpy((void *)(ctx->buffer + left), input, ilen);
    }
}

static const unsigned char sha256_padding[64] = {
    0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

void utils_sha256_finish(iot_sha256_context *ctx, uint8_t output[32])
{
    uint32_t last, padn;
    uint32_t high, low;
    unsigned char msglen[8];

    high = (ctx->total[0] >> 29)
           | (ctx->total[1] <<  3);
    low  = (ctx->total[0] <<  3);

    PUT_UINT32_BE(high, msglen, 0);
    PUT_UINT32_BE(low,  msglen, 4);

    last = ctx->total[0] & 0x3F;
    padn = (last < 56) ? (56 - last) : (120 - last);

    utils_sha256_update(ctx, sha256_padding, padn);
    utils_sha256_update(ctx, msglen, 8);

    PUT_UINT32_BE(ctx->state[0], output,  0);
    PUT_UINT32_BE(ctx->state[1], output,  4);
    PUT_UINT32_BE(ctx->state[2], output,  8);
    PUT_UINT32_BE(ctx->state[3], output, 12);
    PUT_UINT32_BE(ctx->state[4], output, 16);
    PUT_UINT32_BE(ctx->state[5], output, 20);
    PUT_UINT32_BE(ctx->state[6], output, 24);

    if (ctx->is224 == 0) {
        PUT_UINT32_BE(ctx->state[7], output, 28);
    }
}

void utils_sha256(const uint8_t *input, uint32_t ilen, uint8_t output[32])
{
    iot_sha256_context ctx;

    utils_sha256_init(&ctx);
    utils_sha256_starts(&ctx);
    utils_sha256_update(&ctx, input, ilen);
    utils_sha256_finish(&ctx, output);
    utils_sha256_free(&ctx);
}

void utils_hmac_sha256(const uint8_t *msg, uint32_t msg_len, const uint8_t *key, uint32_t key_len, uint8_t output[32])
{
    iot_sha256_context context;
    uint8_t k_ipad[SHA256_KEY_IOPAD_SIZE];    /* inner padding - key XORd with ipad  */
    uint8_t k_opad[SHA256_KEY_IOPAD_SIZE];    /* outer padding - key XORd with opad */
    int32_t i;

    if ((NULL == msg) || (NULL == key) || (NULL == output)) {
        return;
    }

    if (key_len > SHA256_KEY_IOPAD_SIZE) {
        return;
    }

    /* start out by storing key in pads */
    memset(k_ipad, 0, sizeof(k_ipad));
    memset(k_opad, 0, sizeof(k_opad));
    memcpy(k_ipad, key, key_len);
    memcpy(k_opad, key, key_len);

    /* XOR key with ipad and opad values */
    for (i = 0; i < SHA256_KEY_IOPAD_SIZE; i++) {
        k_ipad[i] ^= 0x36;
        k_opad[i] ^= 0x5c;
    }

    /* perform inner SHA */
    utils_sha256_init(&context);                                      /* init context for 1st pass */
    utils_sha256_starts(&context);                                    /* setup context for 1st pass */
    utils_sha256_update(&context, k_ipad, SHA256_KEY_IOPAD_SIZE);     /* start with inner pad */
    utils_sha256_update(&context, msg, msg_len);                      /* then text of datagram */
    utils_sha256_finish(&context, output);                            /* finish up 1st pass */

    /* perform outer SHA */
    utils_sha256_init(&context);                              /* init context for 2nd pass */
    utils_sha256_starts(&context);                            /* setup context for 2nd pass */
    utils_sha256_update(&context, k_opad, SHA256_KEY_IOPAD_SIZE);    /* start with outer pad */
    utils_sha256_update(&context, output, SHA256_DIGEST_SIZE);     /* then results of 1st hash */
    utils_sha256_finish(&context, output);                       /* finish up 2nd pass */
}

#endif

