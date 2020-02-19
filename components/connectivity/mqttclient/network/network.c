/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-09 21:30:54
 * @LastEditTime : 2020-01-14 03:38:15
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#include "platform_timer.h"
#include "platform_nettype_tcp.h"

#if MQTT_NETWORK_TYPE_TLS
#include "platform_nettype_tls.h"
#endif

int network_read(network_t *n, unsigned char *buf, int len, int timeout)
{
#if MQTT_NETWORK_TYPE_TLS
	return platform_nettype_tls_read(n, buf, len, timeout);
#else
	return platform_nettype_tcp_read(n, buf, len, timeout);
#endif
}

int network_write(network_t *n, unsigned char *buf, int len, int timeout)
{
#if MQTT_NETWORK_TYPE_TLS
	return platform_nettype_tls_write(n, buf, len, timeout);
#else
	return platform_nettype_tcp_write(n, buf, len, timeout);
#endif
}

int network_connect(network_t *n)
{
#if MQTT_NETWORK_TYPE_TLS
	return platform_nettype_tls_connect(n);
#else
	return platform_nettype_tcp_connect(n);
#endif
}

void network_disconnect(network_t *n)
{
#if MQTT_NETWORK_TYPE_TLS
	platform_nettype_tls_disconnect(n);
#else
	platform_nettype_tcp_disconnect(n);
#endif
}

int network_init(network_t* n, network_params_t* network_params)
{
	if ((NULL == n) || (NULL == network_params) || (NULL == network_params->addr) || (NULL == network_params->port))
		RETURN_ERROR(MQTT_NULL_VALUE_ERROR);
	
	n->socket = -1;
    n->read = network_read;
	n->write = network_write;
	n->connect = network_connect;
	n->disconnect = network_disconnect;
	n->network_params.addr = network_params->addr;
	n->network_params.port = network_params->port;

#if MQTT_NETWORK_TYPE_TLS
	n->network_params.network_ssl_params.ca_crt = network_params->network_ssl_params.ca_crt;
	n->network_params.network_ssl_params.ca_crt_len = strlen(n->network_params.network_ssl_params.ca_crt);
#if defined(MBEDTLS_FS_IO)
	n->network_params.network_ssl_params.cert_file = network_params->network_ssl_params.cert_file;
	n->network_params.network_ssl_params.key_file = network_params->network_ssl_params.key_file;
#else
#if defined(MBEDTLS_KEY_EXCHANGE__SOME__PSK_ENABLED)
	n->network_params.network_ssl_params.psk = network_params->network_ssl_params.psk;
	n->network_params.network_ssl_params.psk_id = network_params->network_ssl_params.psk_id;
	n->network_params.network_ssl_params.psk_length = network_params->network_ssl_params.psk_length;
#endif
#endif
	if (0 == network_params->network_ssl_params.timeout_ms)
		network_params->network_ssl_params.timeout_ms = MQTT_TLS_HANDSHAKE_TIMEOUT;
	n->network_params.network_ssl_params.timeout_ms = network_params->network_ssl_params.timeout_ms;
#endif

	RETURN_ERROR(MQTT_SUCCESS_ERROR);
}

void network_release(network_t* n)
{
	if (n->socket)
		network_disconnect(n);

	memset(n, 0, sizeof(network_t));
}

