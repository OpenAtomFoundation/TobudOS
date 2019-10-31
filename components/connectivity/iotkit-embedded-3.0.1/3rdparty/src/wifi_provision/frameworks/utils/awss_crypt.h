/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __AWSS_CRYPT_H__
#define __AWSS_CRYPT_H__

enum {
    SEC_LVL_OPEN = 0,       /* open */
    SEC_LVL_AES256,         /* AES256 */
    SEC_LVL_AES128_DEFAULT, /* AES128 with default key */
    SEC_LVL_AES128_PRODUCT, /* AES128 with key from product_sec */
    SEC_LVL_AES128_DEVICE,  /* AES128 with key from device_sec */
    SEC_LVL_AES128_MANU,    /* AES128 with key from manufacturer_sec */
};

int aes_decrypt_string(char *cipher, char *plain, int len, int cipher_hex, int sec_lvl, char cbc, const char *rand);

int awss_get_encrypt_type();
#endif
