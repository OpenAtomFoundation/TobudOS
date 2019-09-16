#ifndef _QCLOUD_DEVICE_H
#define  _QCLOUD_DEVICE_H_

typedef struct qcloud_device_st {
    const char *product_id;
    const char *device_name;
#if (QCLOUD_CFG_AUTH_MODE == QCLOUD_AUTH_MODE_CERT)
    const char *cert_path;
    const char *priv_key_path;
#else
    const char *key;
#endif

#if (QCLOUD_CFG_DEVICE_DYN_REG_EN > 0u)
    char        product_key[QCLOUD_DEVICE_PRODUCT_KEY_MAX + 1];
#endif
} qcloud_device_t;

#if (QCLOUD_CFG_AUTH_MODE == QCLOUD_AUTH_MODE_CERT)
__QCLOUD_API__ qcloud_err_t qcloud_device_create(qcloud_device_t *device,
                                                                const char *product_id,
                                                                const char *device_name,
                                                                const char *cert_path,
                                                                const char *priv_key_path);
#else
__QCLOUD_API__ qcloud_err_t qcloud_device_create(qcloud_device_t *device,
                                                                const char *product_id,
                                                                const char *device_name,
                                                                const char *key);
#endif

#endif
