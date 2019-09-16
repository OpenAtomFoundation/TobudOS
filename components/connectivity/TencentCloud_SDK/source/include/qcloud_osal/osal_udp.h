#ifndef _OSAL_UDP_H_
#define  _OSAL_UDP_H_

__QCLOUD_OSAL__ int osal_udp_connect(const char *host, unsigned short port);

__QCLOUD_OSAL__ void osal_udp_disconnect(int sockfd);

__QCLOUD_OSAL__ qcloud_err_t osal_udp_write(int sockfd, const void *buf, size_t len, uint32_t timeout, size_t *write_len);

__QCLOUD_OSAL__ qcloud_err_t osal_udp_read(int sockfd, void *buf, size_t len, uint32_t timeout, size_t *read_len);

#endif

