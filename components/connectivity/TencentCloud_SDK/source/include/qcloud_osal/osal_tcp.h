#ifndef _OSAL_TCP_
#define  _OSAL_TCP_

__QCLOUD_OSAL__ int osal_tcp_connect(const char *host, uint16_t port);

__QCLOUD_OSAL__ int osal_tcp_disconnect(int fd);

__QCLOUD_OSAL__ qcloud_err_t osal_tcp_write(int fd, const void *buf, size_t len, uint32_t timeout, size_t *write_len);

__QCLOUD_OSAL__ qcloud_err_t osal_tcp_read(int fd, void *buf, size_t len, uint32_t timeout, size_t *read_len);

#endif

