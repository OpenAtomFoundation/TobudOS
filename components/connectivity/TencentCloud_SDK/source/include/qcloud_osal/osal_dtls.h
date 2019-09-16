#ifndef _OSAL_DTLS_H_
#define  _OSAL_DTLS_H_

__QCLOUD_OSAL__ void *osal_dtls_connect(qcloud_tls_opt_t *tls_opt, const char *host, int port);

__QCLOUD_OSAL__ void osal_dtls_disconnect(void *handle);

__QCLOUD_OSAL__ qcloud_err_t osal_dtls_write(void *handle, const void *buf, size_t len, size_t *write_len);

__QCLOUD_OSAL__ qcloud_err_t osal_dtls_read(void *handle, void *buf, size_t len, uint32_t timeout, size_t *read_len);

#endif

