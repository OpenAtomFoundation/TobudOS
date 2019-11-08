/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef _AT_CONN_MGMT_H_
#define _AT_CONN_MGMT_H_

typedef enum netconn_type {
    NETCONN_INVALID  = 0,
    /** TCP IPv4 */
    NETCONN_TCP,
    NETCONN_TYPE_NUM
} netconn_type_t;

/**
 * at connection module initialization
 *
 * @param null
 *
 * @return  0 : on success, -1: error
 */
int at_conn_init(void);

/**
 * receive data from an at connection 
 *
 * @param[in]: connection id; 
 * @param[out]: pointer to output buffer
 * @param[in]: expect length
 *
 * @return  0 : on success, -1: error
 */
int at_conn_recv(int connid, void *mem, uint32_t len);

/**
 * query ip from domain address
 *
 * @param[in]: domain address
 * @param[out]: query result
 *
 * @return  0 : on success, -1: error
 */
int at_conn_getaddrinfo(const char *nodename, char resultip[16]);


/**
 * setup an new at connection
 *
 * @param[in]: connection type only tcp support 
 *
 * @return  0 : on success, -1: error
 */
int at_conn_setup(netconn_type_t type);

/**
 * start an at connection
 *
 * @param[in]: connection id
 * @param[in]: remote ip address
 * @param[in]: remote port
 *
 * @return  0 : on success, -1: error
 */
int at_conn_start(int connid, char* remoteipaddr, uint16_t remoteport);

/**
 * close an at connection
 *
 * @param[in] connection id
 *
 * @return  0 : on success, -1: error
 */
int at_conn_close(int connid);

/**
 * check whether recvbuf empty
 *
 * @param[in] connection id
 *
 * @return  0 : on success, -1: error
 */
int at_conn_recvbufempty(int connid);

/**
 * send data through an at connection
 *
 * @param[in] connection id
 * @param[in] send buf pointer
 *
 * @return  0 : on success, -1: error
 */
int at_conn_send(int connid, const void *data, uint32_t size);

#endif
