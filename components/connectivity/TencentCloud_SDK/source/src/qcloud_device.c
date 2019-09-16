#include "qcloud.h"

#if (QCLOUD_CFG_AUTH_MODE == QCLOUD_AUTH_MODE_CERT)

__QCLOUD_API__ qcloud_err_t qcloud_device_create(qcloud_device_t *device,
                                                                const char *product_id,
                                                                const char *device_name,
                                                                const char *cert_path,
                                                                const char *priv_key_path)
{
    QCLOUD_POINTER_SANITY_CHECK(device, QCLOUD_ERR_DEV_INFO);
    QCLOUD_POINTER_SANITY_CHECK(product_id, QCLOUD_ERR_DEV_INFO);
    QCLOUD_POINTER_SANITY_CHECK(device_name, QCLOUD_ERR_DEV_INFO);
    QCLOUD_POINTER_SANITY_CHECK(cert_path, QCLOUD_ERR_DEV_INFO);
    QCLOUD_POINTER_SANITY_CHECK(priv_key_path, QCLOUD_ERR_DEV_INFO);

    if (strlen(product_id) > QCLOUD_DEVICE_PRODUCT_ID_MAX ||
        strlen(device_name) > QCLOUD_DEVICE_DEVICE_NAME_MAX ||
        strlen(cert_path) > QCLOUD_PATH_MAX ||
        strlen(priv_key_path) > QCLOUD_PATH_MAX) {
        return QCLOUD_ERR_DEV_INFO;
    }

    device->product_id      = product_id;
    device->device_name     = device_name;
    device->cert_path       = cert_path;
    device->priv_key_path   = priv_key_path;

    QCLOUD_LOG_I("SDK version: %s, product ID: %s, device name: %s", QCLOUD_SDK_VERSION, product_id, device_name);

    return QCLOUD_ERR_SUCCESS;
}

#else

__QCLOUD_API__ qcloud_err_t qcloud_device_create(qcloud_device_t *device,
                                                                const char *product_id,
                                                                const char *device_name,
                                                                const char *key)
{
    QCLOUD_POINTER_SANITY_CHECK(device, QCLOUD_ERR_DEV_INFO);
    QCLOUD_POINTER_SANITY_CHECK(product_id, QCLOUD_ERR_DEV_INFO);
    QCLOUD_POINTER_SANITY_CHECK(device_name, QCLOUD_ERR_DEV_INFO);
    QCLOUD_POINTER_SANITY_CHECK(key, QCLOUD_ERR_DEV_INFO);

    if (strlen(product_id) > QCLOUD_DEVICE_PRODUCT_ID_MAX ||
        strlen(device_name) > QCLOUD_DEVICE_DEVICE_NAME_MAX ||
        strlen(key) > QCLOUD_DEVICE_KEY_MAX) {
        return QCLOUD_ERR_DEV_INFO;
    }

    device->product_id  = product_id;
    device->device_name = device_name;
    device->key         = key;

    QCLOUD_LOG_I("SDK version: %s, product ID: %s, device name: %s", QCLOUD_SDK_VERSION, product_id, device_name);

    return QCLOUD_ERR_SUCCESS;
}

#endif

