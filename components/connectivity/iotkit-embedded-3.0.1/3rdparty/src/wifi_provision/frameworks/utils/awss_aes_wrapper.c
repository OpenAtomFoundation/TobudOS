#include "wifi_provision_internal.h"

#if defined(HAL_CRYPTO)
/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdlib.h>
#include "infra_compat.h"
#if defined(INFRA_AES)
#include "infra_aes.h"
#endif

#define AES_BLOCK_SIZE 16

#if defined(INFRA_AES)
typedef struct {
    infra_aes_context ctx;
    uint8_t iv[16];
    uint8_t key[16];
} platform_aes_t;
#endif

p_HAL_Aes128_t awss_Aes128_Init(
            const uint8_t *key,
            const uint8_t *iv)
{
#if defined(INFRA_AES)
    int ret = 0;
    platform_aes_t *p_aes128 = NULL;

    if (!key || !iv) return p_aes128;

    p_aes128 = (platform_aes_t *)calloc(1, sizeof(platform_aes_t));
    if (!p_aes128) return p_aes128;

    infra_aes_init(&p_aes128->ctx);

    ret = infra_aes_setkey_dec(&p_aes128->ctx, key, 128);

    if (ret == 0) {
        memcpy(p_aes128->iv, iv, 16);
        memcpy(p_aes128->key, key, 16);
    } else {
        free(p_aes128);
        p_aes128 = NULL;
    }

    return (p_HAL_Aes128_t *)p_aes128;
#else
    return (p_HAL_Aes128_t *)HAL_Aes128_Init(key, iv, HAL_AES_DECRYPTION);
#endif
}

int awss_Aes128_Destroy(p_HAL_Aes128_t aes)
{
#if defined(INFRA_AES)
    if (!aes) return -1;

    infra_aes_free(&((platform_aes_t *)aes)->ctx);
    free(aes);

    return 0;
#else
    return HAL_Aes128_Destroy(aes);
#endif
}

int awss_Aes128_Cbc_Decrypt(
            p_HAL_Aes128_t aes,
            const void *src,
            size_t blockNum,
            void *dst)
{
#if defined(INFRA_AES)
    int i   = 0;
    int ret = -1;
    platform_aes_t *p_aes128 = (platform_aes_t *)aes;

    if (!aes || !src || !dst) return ret;

    for (i = 0; i < blockNum; ++i) {
        ret = infra_aes_crypt_cbc(&p_aes128->ctx, INFRA_AES_DECRYPT, AES_BLOCK_SIZE,
                                    p_aes128->iv, src, dst);
        src += 16;
        dst += 16;
    }

    return ret;
#else
    return HAL_Aes128_Cbc_Decrypt(aes, src, blockNum, dst);
#endif
}

int awss_Aes128_Cfb_Decrypt(
            p_HAL_Aes128_t aes,
            const void *src,
            size_t length,
            void *dst)
{
#if defined(INFRA_AES)
    size_t offset = 0;
    int ret = -1;
    platform_aes_t *p_aes128 = (platform_aes_t *)aes;

    if (!aes || !src || !dst) return ret;

    ret = infra_aes_setkey_enc(&p_aes128->ctx, p_aes128->key, 128);
    ret = infra_aes_crypt_cfb128(&p_aes128->ctx, INFRA_AES_DECRYPT, length,
                                   &offset, p_aes128->iv, src, dst);
    return ret;
#else
    return HAL_Aes128_Cfb_Decrypt(aes, src, length, dst);
#endif
}

#endif  /* #if defined(HAL_CRYPTO) */
