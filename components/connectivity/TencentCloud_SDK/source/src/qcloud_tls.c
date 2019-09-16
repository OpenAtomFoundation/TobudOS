#include "qcloud.h"

#if (QCLOUD_CFG_TLS_EN > 0u)

#if (QCLOUD_CFG_AUTH_MODE == QCLOUD_AUTH_MODE_CERT)
__QCLOUD_STATIC__ qcloud_err_t qcloud_tls_init_with_cert(qcloud_tls_opt_t *tls_opt, qcloud_device_t *device)
{
    if (!device->cert_path || !device->priv_key_path) {
        QCLOUD_LOG_E("cert file or key file is empty!");
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_INVAL);
    }

    QCLOUD_LOG_D("cert path: %s", device->cert_path);
    QCLOUD_LOG_D("priv key path: %s", device->priv_key_path);

    tls_opt->cert_path      = device->cert_path;
    tls_opt->priv_key_path  = device->priv_key_path;

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
}
#else

__QCLOUD_STATIC__ qcloud_err_t qcloud_tls_psk_id_generate(qcloud_tls_opt_t *tls_opt, qcloud_device_t *device)
{
    int psk_id_len = 0;

    memset(tls_opt->psk_id, 0, sizeof(tls_opt->psk_id));

    psk_id_len = osal_snprintf(tls_opt->psk_id, QCLOUD_MQTT_DEVICE_CLIENT_ID_MAX, "%s%s", device->product_id, device->device_name);
    if (psk_id_len < 0 || psk_id_len >= QCLOUD_MQTT_DEVICE_CLIENT_ID_MAX) {
        QCLOUD_LOG_E("psk id generate failed");
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_DEV_INFO);
    }

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
}

__QCLOUD_STATIC__ qcloud_err_t qcloud_tls_psk_generate(qcloud_tls_opt_t *tls_opt, qcloud_device_t *device)
{
    size_t key_len = 0, decoded_psk_len;

    if (!device->key) {
        QCLOUD_LOG_E("device key NULL!");
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_INVAL);
    }

    memset(tls_opt->psk, 0, sizeof(tls_opt->psk));

    key_len = strlen(device->key);
    if (qcloud_utils_base64_decode(tls_opt->psk, sizeof(tls_opt->psk), &decoded_psk_len,
                                            (const unsigned char *)device->key, key_len) != 0) {
        QCLOUD_LOG_E("psk decode failed!");
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_INVAL);
    }

    tls_opt->psk_len = decoded_psk_len;

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
}

__QCLOUD_STATIC__ qcloud_err_t qcloud_tls_init_with_key(qcloud_tls_opt_t *tls_opt, qcloud_device_t *device)
{
    QCLOUD_FUNC_EXIT_RC_IF_NOT(qcloud_tls_psk_generate(tls_opt, device), QCLOUD_ERR_SUCCESS, QCLOUD_ERR_FAILURE);
    QCLOUD_FUNC_EXIT_RC_IF_NOT(qcloud_tls_psk_id_generate(tls_opt, device), QCLOUD_ERR_SUCCESS, QCLOUD_ERR_FAILURE);

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
}

#endif

__QCLOUD_INTERNAL__ qcloud_err_t qcloud_tls_init(qcloud_tls_opt_t *tls_opt, qcloud_device_t *device)
{
#if (QCLOUD_CFG_AUTH_MODE == QCLOUD_AUTH_MODE_CERT)
    QCLOUD_FUNC_EXIT_RC_IF_NOT(qcloud_tls_init_with_cert(tls_opt, device), QCLOUD_ERR_SUCCESS, QCLOUD_ERR_FAILURE);
#else
    QCLOUD_FUNC_EXIT_RC_IF_NOT(qcloud_tls_init_with_key(tls_opt, device), QCLOUD_ERR_SUCCESS, QCLOUD_ERR_FAILURE);
#endif

    tls_opt->ca_cert        = qcloud_ca_cert;
    tls_opt->ca_cert_len    = strlen(tls_opt->ca_cert);
    tls_opt->handle         = NULL;
    tls_opt->timeout        = QCLOUD_TLS_HANDSHAKE_TIMEOUT;

    return QCLOUD_ERR_SUCCESS;
}

__QCLOUD_STATIC__ qcloud_err_t qcloud_tls_connect(qcloud_network_t *network)
{
	QCLOUD_POINTER_SANITY_CHECK(network, QCLOUD_ERR_INVAL);

	network->tls_opt.handle = osal_tls_connect(&(network->tls_opt), network->host, network->port);
	if (network->tls_opt.handle) {
		return QCLOUD_ERR_SUCCESS;
	}

	return QCLOUD_ERR_FAILURE;
}

__QCLOUD_STATIC__ void qcloud_tls_disconnect(qcloud_network_t *network)
{
	if (!network || !network->tls_opt.handle) {
        return;
    }

	osal_tls_disconnect(network->tls_opt.handle);
	network->tls_opt.handle = NULL;
}

__QCLOUD_STATIC__ qcloud_err_t qcloud_tls_read(qcloud_network_t *network, void *buf, size_t len, uint32_t timeout, size_t *read_len)
{
	QCLOUD_POINTER_SANITY_CHECK(network, QCLOUD_ERR_INVAL);

	return osal_tls_read(network->tls_opt.handle, buf, len, timeout, read_len);
}

__QCLOUD_STATIC__ qcloud_err_t qcloud_tls_write(qcloud_network_t *network, const void *buf, size_t len, uint32_t timeout, size_t *write_len)
{
	QCLOUD_POINTER_SANITY_CHECK(network, QCLOUD_ERR_INVAL);

	return osal_tls_write(network->tls_opt.handle, buf, len, timeout, write_len);
}

__QCLOUD_STATIC__ int qcloud_tls_is_connected(qcloud_network_t *network)
{
    return network->tls_opt.handle != NULL;
}

__QCLOUD_INTERNAL__ qcloud_err_t qcloud_network_tls_init(qcloud_network_t *network)
{
	QCLOUD_POINTER_SANITY_CHECK(network, QCLOUD_ERR_INVAL);

    network->connect        = qcloud_tls_connect;
    network->read           = qcloud_tls_read;
    network->write          = qcloud_tls_write;
    network->disconnect     = qcloud_tls_disconnect;

    network->is_connected   = qcloud_tls_is_connected;

    return QCLOUD_ERR_SUCCESS;
}

__QCLOUD_INTERNAL__ qcloud_err_t qcloud_dtls_connect(qcloud_network_t *network)
{
	QCLOUD_POINTER_SANITY_CHECK(network, QCLOUD_ERR_INVAL);

	network->tls_opt.handle = osal_dtls_connect(&(network->tls_opt), network->host, network->port);
	if (network->tls_opt.handle != 0) {
		return QCLOUD_ERR_SUCCESS;
	}

	return QCLOUD_ERR_FAILURE;
}

__QCLOUD_STATIC__ void qcloud_dtls_disconnect(qcloud_network_t *network)
{
	if (!network || !network->tls_opt.handle) {
        return;
    }

	osal_dtls_disconnect(network->tls_opt.handle);
	network->tls_opt.handle = NULL;
}

__QCLOUD_STATIC__ qcloud_err_t qcloud_dtls_read(qcloud_network_t *network, void *buf, size_t len, uint32_t timeout, size_t *read_len)
{
	QCLOUD_POINTER_SANITY_CHECK(network, QCLOUD_ERR_INVAL);

	return osal_dtls_read(network->tls_opt.handle, buf, len, timeout, read_len);
}

__QCLOUD_STATIC__ qcloud_err_t qcloud_dtls_write(qcloud_network_t *network, const void *buf, size_t len, uint32_t timeout, size_t *write_len)
{
	QCLOUD_POINTER_SANITY_CHECK(network, QCLOUD_ERR_INVAL);

	return osal_dtls_write(network->tls_opt.handle, buf, len, write_len);
}

__QCLOUD_STATIC__ int qcloud_dtls_is_connected(qcloud_network_t *network)
{
    return network->tls_opt.handle != NULL;
}

__QCLOUD_INTERNAL__ qcloud_err_t qcloud_network_dtls_init(qcloud_network_t *network)
{
	QCLOUD_POINTER_SANITY_CHECK(network, QCLOUD_ERR_INVAL);

    network->connect        = qcloud_dtls_connect;
    network->read           = qcloud_dtls_read;
    network->write          = qcloud_dtls_write;
    network->disconnect     = qcloud_dtls_disconnect;

    network->is_connected   = qcloud_dtls_is_connected;

    return QCLOUD_ERR_SUCCESS;
}

#endif

