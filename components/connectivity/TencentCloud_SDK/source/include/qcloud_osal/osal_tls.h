#ifndef _OSAL_TLS_H_
#define  _OSAL_TLS_H_

__QCLOUD_OSAL__ void *osal_tls_connect(qcloud_tls_opt_t *tls_opt, const char *host, int port);

__QCLOUD_OSAL__ void osal_tls_disconnect(void *handle);

__QCLOUD_OSAL__ qcloud_err_t osal_tls_write(void *handle, const void *buf, size_t len, uint32_t timeout, size_t *write_len);

__QCLOUD_OSAL__ qcloud_err_t osal_tls_read(void *handle, void *buf, size_t len, uint32_t timeout, size_t *read_len) ;

#endif

