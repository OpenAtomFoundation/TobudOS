#ifndef _QCLOUD_NETWORK_H_
#define  _QCLOUD_NETWORK_H_

typedef struct qcloud_network_st qcloud_network_t;

typedef struct qcloud_network_st {
    qcloud_err_t        (*connect)      (qcloud_network_t *);

    qcloud_err_t        (*read)         (qcloud_network_t *, void *buf, size_t len, uint32_t timeout, size_t *read_len);

    qcloud_err_t        (*write)        (qcloud_network_t *, const void *buf, size_t len, uint32_t timeout, size_t *write_len);

    void                (*disconnect)   (qcloud_network_t *);

    int                 (*is_connected) (qcloud_network_t *);

    int                 fd;

#if (QCLOUD_CFG_TLS_EN > 0u)
    qcloud_tls_opt_t    tls_opt;
#endif

    char                host[QCLOUD_SERVER_DOMAIN_MAX];   // 服务器地址
    int                 port;   // 服务器端口
} qcloud_network_t;

__QCLOUD_API__ qcloud_err_t qcloud_network_tcp_init(qcloud_network_t *network);

__QCLOUD_API__ qcloud_err_t qcloud_network_udp_init(qcloud_network_t *network);

__QCLOUD_INTERNAL__ qcloud_err_t qcloud_network_tls_init(qcloud_network_t *network);

__QCLOUD_INTERNAL__ qcloud_err_t qcloud_network_dtls_init(qcloud_network_t *network);

#endif

