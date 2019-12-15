
#include "wifi_provision_internal.h"

p_HAL_Aes128_t awss_Aes128_Init(
            const uint8_t *key,
            const uint8_t *iv);

int awss_Aes128_Destroy(p_HAL_Aes128_t aes);

int awss_Aes128_Cbc_Decrypt(
            p_HAL_Aes128_t aes,
            const void *src,
            size_t blockNum,
            void *dst);

int awss_Aes128_Cfb_Decrypt(
            p_HAL_Aes128_t aes,
            const void *src,
            size_t length,
            void *dst);
