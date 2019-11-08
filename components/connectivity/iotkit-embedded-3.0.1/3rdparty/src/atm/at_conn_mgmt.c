/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>

#include "infra_types.h"
#include "mqtt_api.h"
#include "at_wrapper.h"

#include "at_conn_mbox.h"
#include "at_conn_mgmt.h"

#define AT_DEFAULT_INPUTMBOX_SIZE  3
#define AT_DEFAULT_PAYLOAD_SIZE (CONFIG_MQTT_MESSAGE_MAXLEN + CONFIG_MQTT_TOPIC_MAXLEN + 20)

#define AT_DEFAULT_SEND_TIMEOUT_MS    1000
#define AT_DEFAULT_RECV_TIMEOUT_MS    1000

#define DNS_MAX_NAME_LENGTH          256
#define AT_IP4_ANY_ADDR            "0.0.0.0"
#define IPV4_STR_MAX_LEN             16

#define AT_MAX_PAYLOAD_SIZE          1512

#define UNUSED_ATCONN                -1

#ifdef AT_DEBUG_MODE
#define AT_DEBUG(...)       do{HAL_Printf(__VA_ARGS__);HAL_Printf("\r\n");}while(0)
#define AT_ERROR(...)       do{HAL_Printf(__VA_ARGS__);HAL_Printf("\r\n");}while(0)
#else
#define AT_DEBUG(...)
#define AT_ERROR(...)
#endif

enum netconn_state {
    NETCONN_NONE  = 0,
    NETCONN_WRITE,
    NETCONN_LISTEN,
    NETCONN_CONNECT,
    NETCONN_CLOSE
};

/** Contains all internal pointers and states used for a socket */
struct at_conn {
    /** connnection ID */
	int connid;
    /** type of the netconn (TCP) */
    enum netconn_type type;
    /** current state of the netconn */
    enum netconn_state state;
    /** remote port number */
	uint16_t remote_port;
    /** remote ip address */
    char remote_ip[IPV4_STR_MAX_LEN];
    /** data that was left from the previous read */
    void *lastdata;
    /** offset in the data that was left from the previous read */
    uint16_t lastoffset;
    /** mbox where received packets are stored until they are fetched
        by the neconn application thread. */
    at_mbox_t recvmbox;
    /** pointer buffer for mbox which is used by ringbuf module. */
    void *recvbuf[AT_DEFAULT_INPUTMBOX_SIZE];
    /** timeout to wait for sending data (which means enqueueing data for sending
        in internal buffers) in milliseconds */
    int send_timeout_ms;
    /** timeout in milliseconds to wait for new data to be received
        (or connections to arrive for listening netconns) */
    int recv_timeout_ms;
};

typedef struct at_netbuf {
    void *payload;
    uint16_t len;
    uint16_t remote_port;
    char  remote_ip[IPV4_STR_MAX_LEN];
} at_netbuf_t;

/** The global array of available at */
static struct at_conn atconnects[AT_CONN_NUM];
static void *g_atconnmutex = NULL;

#ifndef PLATFORM_HAS_DYNMEM
static at_netbuf_t atnetbuf[AT_DEFAULT_INPUTMBOX_SIZE] =
                            {{NULL, 0, 0, {'\0'}}};

typedef struct at_payload {
    uint8_t buf[AT_DEFAULT_PAYLOAD_SIZE];
    uint8_t used;
} at_payload_t;

static at_payload_t atpayload[AT_DEFAULT_INPUTMBOX_SIZE] =
                             {{{0}, 0}};
#endif

static void *alloc_payload(int size)
{
#ifdef PLATFORM_HAS_DYNMEM
    return HAL_Malloc(size);
#else
    int i;

    if (size <= 0 || size > AT_DEFAULT_PAYLOAD_SIZE) {
        return NULL;
    }

    for (i = 0; i < AT_DEFAULT_INPUTMBOX_SIZE; i++) {
        if (0 == atpayload[i].used) {
            atpayload[i].used = 1;
            return atpayload[i].buf;
        }
    }

    return NULL;
#endif
}

static void free_payload(void *payload)
{
    if (payload) {
#ifdef PLATFORM_HAS_DYNMEM
        HAL_Free(payload);
#else
        memset(payload, 0, sizeof(at_payload_t));
#endif
   }
}

static at_netbuf_t *alloc_atnetbuf(void)
{
#ifdef PLATFORM_HAS_DYNMEM
    return HAL_Malloc(sizeof(at_netbuf_t));
#else
    int i;

    for (i = 0; i < AT_DEFAULT_INPUTMBOX_SIZE; i++) {
        if (NULL == atnetbuf[i].payload) {
            return &atnetbuf[i];
        }
    }

    return NULL;
#endif
}

static void free_atnetbuf(at_netbuf_t *netbuf)
{
    if (netbuf) {
#ifdef PLATFORM_HAS_DYNMEM
        HAL_Free(netbuf);
#else
        memset(netbuf, 0, sizeof(at_netbuf_t));
#endif
    }
}

static struct at_conn *get_conn(int c)
{
    struct at_conn *conn = NULL;

    if ((c < 0) || (c >= AT_CONN_NUM)) {
        AT_DEBUG("get_conn(%d): invalid", c);
        return NULL;
    }

    conn = &atconnects[c];

    if (UNUSED_ATCONN == conn->connid) {
        AT_DEBUG("get_conn(%d): not active", c);
        return NULL;
    }

    return conn;
}

static int at_newconn(void)
{
    int i;

    for (i = 0; i < AT_CONN_NUM; i++) {
        if (atconnects[i].connid == UNUSED_ATCONN) {
        	if (at_mbox_new(&atconnects[i].recvmbox,
                            AT_DEFAULT_INPUTMBOX_SIZE,
                            atconnects[i].recvbuf) != 0) {
                AT_ERROR("fai to new input mail box size %d \n", AT_DEFAULT_INPUTMBOX_SIZE);
                return -1;
            }
            
            atconnects[i].type            = NETCONN_INVALID;
            atconnects[i].state           = NETCONN_NONE;
            atconnects[i].lastdata        = NULL;
            atconnects[i].lastoffset      = 0;
            atconnects[i].connid          = i;
            atconnects[i].send_timeout_ms = AT_DEFAULT_SEND_TIMEOUT_MS;
            atconnects[i].recv_timeout_ms = AT_DEFAULT_RECV_TIMEOUT_MS;
            return i;
        }
    }

    return -1;
}

static void at_drainconn(struct at_conn *conn)
{
    at_netbuf_t *mem;

    if (NULL == conn)
    	return;

    if (at_mbox_valid(&conn->recvmbox)) {
        while (at_mbox_tryfetch(&conn->recvmbox, (void **)(&mem)) != AT_MBOX_EMPTY) {
            if (mem != NULL) {
                if (mem->payload) {
                    free_payload(mem->payload);
                    mem->payload = NULL;
                }
                free_atnetbuf(mem);
            }
        }
        at_mbox_free(&conn->recvmbox);
        at_mbox_set_invalid(&conn->recvmbox);
    }

    return;
}    

static int at_freeconn(struct at_conn *conn)
{
    at_netbuf_t  *buf = NULL;

    if (NULL == conn)
    	return -1;
    
    if (NULL != conn->lastdata) {
        buf = (at_netbuf_t *) conn->lastdata;

        if (buf->payload) {
            free_payload(buf->payload);
            buf->payload = NULL;
        }

        free_atnetbuf(buf);
    }

    conn->lastdata   = NULL;
    conn->lastoffset = 0;

    at_drainconn(conn);   

    conn->type       = NETCONN_INVALID;
    conn->state      = NETCONN_NONE;
    conn->connid     = UNUSED_ATCONN;

    return 0;
}

static int at_conn_fetch(struct at_conn *conn, at_netbuf_t **new_buf)
{
    uint32_t ret = 0;
    void *buf = NULL;

    if (NULL == conn || NULL == new_buf) {
        return -1;
    }

    if (!at_mbox_valid(&conn->recvmbox)) {
        AT_ERROR("conn %d invalid recvmbox\n", conn->connid);
        return -1;
    }

    ret = at_mbox_fetch(&conn->recvmbox, &buf, conn->recv_timeout_ms);
    if (ret == AT_MBOX_TIMEOUT) {
        AT_ERROR("at conn %d fetch data time out %d\n", conn->connid, conn->recv_timeout_ms);
        return -1;
    }

    *new_buf = buf;
    return 0;
}

/****************************public interface*********************/
int at_conn_input(struct at_conn_input *param)
{
    int  s = -1;
    void *data = NULL;
    int len = 0;
    char *remote_ip = NULL;
    uint16_t remote_port = 0;
    struct at_conn *conn = NULL;
    at_netbuf_t *buf  = NULL;

    if (NULL == param) {
        AT_ERROR("at conn input param NULL\n");
        return -1;
    }

    s = param->fd;
    data = param->data;
    len = param->datalen;
    remote_ip = param->remote_ip;
    remote_port = param->remote_port;

    if (NULL == data || 0 == len) {
        AT_ERROR("low level invalid input data\n");
        return -1;
    }

    if (remote_ip != NULL &&
        strlen(remote_ip) > IPV4_STR_MAX_LEN) {
        AT_ERROR("invalid ip string");
        return -1;
    }

    conn = get_conn(s);
    if (NULL == conn) {
        AT_ERROR("conn %d doesn't exist\n", s);
        return -1;
    }

    if (conn->connid < 0) {
        AT_ERROR("conn %d invalid connid\n", s);
        return -1;
    }

    if (!at_mbox_valid(&conn->recvmbox)) {
        AT_ERROR("invalid conn to input packet\n");
        return -1;
    }

    buf = alloc_atnetbuf();
    if (NULL == buf) {
        AT_ERROR("alloc at net buf size %d fail\n", sizeof(at_netbuf_t));
        return -1;
    }
    memset(buf, 0, sizeof(*buf));

    buf->payload = alloc_payload(len);
    if (NULL == buf->payload) {
        free_atnetbuf(buf);
        AT_ERROR("alloc payload size %d fail\n", len);
        return -1;
    }

    memcpy(buf->payload, data, len);
    buf->len = len;
    buf->remote_port = remote_port;
    if (remote_ip)
        memcpy(buf->remote_ip, remote_ip, IPV4_STR_MAX_LEN);
    
    if (at_mbox_trypost(&conn->recvmbox, buf) != 0) {
        free_payload(buf->payload);
        buf->payload = NULL;
        free_atnetbuf(buf);
        AT_ERROR("try post recv packet fail\n");
        return -1;
    }

    return 0;
 }

int at_conn_init(void)
{
    static int at_conn_init_done = 0;
    int i;

    if (at_conn_init_done) {
        AT_ERROR("at conn have already init done\n");
        return 0;
    }

	for (i = 0; i < AT_CONN_NUM; i++) {
		atconnects[i].connid = UNUSED_ATCONN;
	}

    g_atconnmutex = HAL_MutexCreate();
    if (g_atconnmutex == NULL) {
        AT_ERROR("failed to creat g_atconnmutex \n");
        return -1;
    }

    if (HAL_AT_CONN_Init() != 0) {
        AT_ERROR("at conn low level init fail\n");
        HAL_MutexDestroy(g_atconnmutex);
        return -1;
    }

    at_conn_init_done = 1;

    return 0 ;
}

int at_conn_getaddrinfo(const char *nodename, char resultip[16])
{
	int namelen;

	if (NULL == nodename || NULL == resultip) {
		return -1;
	}
	
    namelen = strlen(nodename);
    if (namelen > DNS_MAX_NAME_LENGTH)
        return -1;

    if (HAL_AT_CONN_DomainToIp((char *)nodename, resultip) != 0) {
        AT_ERROR("domain to ip failed.");
        return -1;
    }

    return 0;
}

int at_conn_setup(netconn_type_t type)
{
	struct at_conn *conn = NULL;
    int connid = -1;

    if (type >= NETCONN_TYPE_NUM || type <= NETCONN_INVALID) {
    	return -1;
    }

    HAL_MutexLock(g_atconnmutex);
    if ((connid = at_newconn()) == -1) {
        AT_ERROR("fai to new at conn\n");
        HAL_MutexUnlock(g_atconnmutex);
    	return -1;
    }

    if ((conn = get_conn(connid)) == NULL) {
    	AT_ERROR("fai to get at conn\n");
        HAL_MutexUnlock(g_atconnmutex);
    	return -1;
    }

    conn->type = type;
    conn->state = NETCONN_NONE;
    HAL_MutexUnlock(g_atconnmutex);

    return connid;
}

int at_conn_start(int connid, char* remoteipaddr, uint16_t remoteport)
{
	char *ipv4anyadrr = AT_IP4_ANY_ADDR;
	at_conn_t statconn = {0};
    struct at_conn *conn = NULL;
 
    HAL_MutexLock(g_atconnmutex);
    conn = get_conn(connid);
    if (NULL == conn) {
        AT_ERROR("at_startconn: invalid conn\n");
        HAL_MutexUnlock(g_atconnmutex);
        return -1;
    }

    if (conn->state != NETCONN_NONE) {
        AT_ERROR("at_startconn: conn %d state is %d \n", connid, conn->state);
        HAL_MutexUnlock(g_atconnmutex);
        return -1;
    }

    statconn.fd = connid;
    statconn.r_port = remoteport;
    statconn.l_port = -1;
    statconn.addr = (char *)remoteipaddr;
    if (NULL == statconn.addr) {
        statconn.addr = ipv4anyadrr;
    }

    switch (conn->type) {
        case NETCONN_TCP:
            statconn.type = TCP_CLIENT;
            if (HAL_AT_CONN_Start(&statconn) != 0) {
                AT_ERROR("fail to setup tcp connect, remote is %s port is %d.\n", statconn.addr, remoteport);
                HAL_MutexUnlock(g_atconnmutex);
                return -1;
            }
            memcpy(conn->remote_ip, statconn.addr, IPV4_STR_MAX_LEN);
            conn->remote_port = remoteport;
            break;
        default:
            AT_ERROR("Unsupported at connection type.\n");
            HAL_MutexUnlock(g_atconnmutex);
            return -1;
    }

    /* Update at conn state */
    conn->state = NETCONN_CONNECT;
    HAL_MutexUnlock(g_atconnmutex);

    return 0;
}

int at_conn_close(int c)
{
    struct at_conn *conn = NULL;
    int err;

    AT_DEBUG("at_close(%d)\r\n", c);

    conn = get_conn(c);
    if (NULL == conn) {
        return -1;
    }

    if (conn->state == NETCONN_CONNECT) {
        if (HAL_AT_CONN_Close(c, -1) != 0) {
            AT_DEBUG("HAL_AT_close failed.");
        }
    }

    HAL_MutexLock(g_atconnmutex);
    err = at_freeconn(conn);
    HAL_MutexUnlock(g_atconnmutex);
    if (err != 0) {
        AT_ERROR("at_freeconn failed in %s.", __func__);
        return -1;
    }

    return 0;
}

int at_conn_recvbufempty(int c)
{
    struct at_conn *conn = NULL;

    conn = get_conn(c);
    if (NULL == conn) {
        AT_ERROR("at_recvbufempty cannot get socket %d\n", c);
        return -1;
    }

    /* remain data */
    if (conn->lastdata)
        return 0;

    if (!at_mbox_valid(&conn->recvmbox)) {
        AT_ERROR("conn %d invalid recvmbox\n", c);
        return -1;
    }

    return at_mbox_empty(&conn->recvmbox);
}

int at_conn_send(int c, const void *data, uint32_t size)
{
    struct at_conn *conn = NULL;

    if (NULL == data || size == 0 || size > AT_MAX_PAYLOAD_SIZE) {
        AT_ERROR("at_conn_send fail to send, size %d\n", size);
        return -1;
    }

    conn = get_conn(c);
    if (NULL == conn) {
        AT_ERROR("at_conn_send fail to get conn %d\n", c);
        return -1;
    }

    if (conn->type == NETCONN_TCP) {
        if (conn->state == NETCONN_NONE) {
            AT_ERROR("at_conn_send connect %d state %d\n", c, conn->state);
            return -1;
        }
    }

    if (HAL_AT_CONN_Send(c, (uint8_t *)data, size, NULL, -1, conn->send_timeout_ms)) {
        AT_ERROR("c %d fail to send do nothing for now\n", c);
        return -1;
    }

    return size;
}

int at_conn_recv(int c, void *mem, uint32_t len)
{
    struct at_conn        *conn    = NULL;
    at_netbuf_t           *buf     = NULL;
    int                    off     = 0;
    uint16_t               buflen  = 0;
    uint16_t               copylen = 0;
    int                    err     = 0;
    uint8_t                done    = 0;

    if (NULL == mem || 0 == len) {
        return -1;
    }

    conn = get_conn(c);
    if (NULL == conn) {
        AT_ERROR("at_conn_recv fail to get conn %d\n", c);
        return -1;
    }

    do {
        if (conn->lastdata) {
            buf = conn->lastdata;
        } else {
            err = at_conn_fetch(conn, &buf);
            if (err != 0 || buf == NULL || buf->payload == NULL) {
                if (off > 0) {
                    return off;
                } else {
                    return -1;
                }
            }

            conn->lastdata = buf;
        }

        buflen = buf->len;
        AT_DEBUG("at_conn_recv: buflen=%u, len=%u, off=%d, lastoffset=%u\n", 
                 buflen, len, off, conn->lastoffset);

        buflen -= conn->lastoffset;
        if (len > buflen) {
            copylen = buflen;
        } else {
            copylen = len;
        }

        memcpy(&((uint8_t *)mem)[off], &((uint8_t *)buf->payload)[conn->lastoffset], copylen);
        off += copylen;

        if (NETCONN_TCP == conn->type) {
            if (len < copylen) {
                AT_ERROR("invalid copylen %d, len = %d\n", copylen, len);
                return -1;
            }

            len -= copylen;
            if (len <= 0) {
                done = 1;
            }
        } else {
            done = 1;
        }

        if ((NETCONN_TCP == conn->type) && (buflen > copylen)) {
            conn->lastdata = buf;
            conn->lastoffset += copylen;
        } else {
            conn->lastdata = NULL;
            conn->lastoffset = 0;
            free_payload(buf->payload);
            buf->payload = NULL;
            free_atnetbuf(buf);
            buf = NULL;
        }
    } while (!done);

    return off;
}
