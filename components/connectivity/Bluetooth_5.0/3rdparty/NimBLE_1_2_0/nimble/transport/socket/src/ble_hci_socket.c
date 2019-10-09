/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

/*
 * Provides HCI transport over sockets. Either over
 * TCP sockets, or Linux bluetooth socket.
 *
 * E.g. to connect to TCP target, start in another window
 * socat -x PIPE:/dev/ttyACM1 TCP4-LISTEN:14433,fork,reuseaddr
 * When building this package, set BLE_SOCK_USE_TCP=1 and
 * BLE_SOCK_TCP_PORT controls the target port this transport
 * connects to.
 *
 * To use Linux Bluetooth sockets, create an interface:
 * sudo hciattach -r -n /dev/ttyUSB0 any 57600
 * And build this package with BLE_SOCK_USE_LINUX_BLUE=1,
 * BLE_SOCK_LINUX_DEV=<interface index of the target interface>
 *
 */
#include "syscfg/syscfg.h"

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <sys/socket.h>

#if MYNEWT_VAL(BLE_SOCK_USE_TCP)
#include <sys/errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#if MYNEWT_VAL(BLE_SOCK_USE_LINUX_BLUE)
#include <sys/errno.h>
#define BTPROTO_HCI 1
#define HCI_CHANNEL_RAW		0
#define HCI_CHANNEL_USER	1
#define HCIDEVUP	_IOW('H', 201, int)
#define HCIDEVDOWN	_IOW('H', 202, int)
#define HCIDEVRESET	_IOW('H', 203, int)
#define HCIGETDEVLIST	_IOR('H', 210, int)

struct sockaddr_hci {
        sa_family_t    hci_family;
        unsigned short hci_dev;
        unsigned short hci_channel;
};
#endif

#include <fcntl.h>
#include <sys/ioctl.h>

#include "sysinit/sysinit.h"
#include "os/os.h"
#include "mem/mem.h"

#include "stats/stats.h"

/* BLE */
#include "nimble/ble.h"
#include "nimble/nimble_opt.h"
#include "nimble/hci_common.h"
#include "nimble/nimble_npl.h"
#include "nimble/ble_hci_trans.h"
#include "socket/ble_hci_socket.h"

/***
 * NOTES:
 * The UART HCI transport doesn't use event buffer priorities.  All incoming
 * and outgoing events use buffers from the same pool.
 *
 * The "skip" definitions are here so that when buffers cannot be allocated,
 * the command or acl packets are simply skipped so that the HCI interface
 * does not lose synchronization and resets dont (necessarily) occur.
 */

STATS_SECT_START(hci_sock_stats)
    STATS_SECT_ENTRY(imsg)
    STATS_SECT_ENTRY(icmd)
    STATS_SECT_ENTRY(ievt)
    STATS_SECT_ENTRY(iacl)
    STATS_SECT_ENTRY(ibytes)
    STATS_SECT_ENTRY(ierr)
    STATS_SECT_ENTRY(imem)
    STATS_SECT_ENTRY(omsg)
    STATS_SECT_ENTRY(oacl)
    STATS_SECT_ENTRY(ocmd)
    STATS_SECT_ENTRY(oevt)
    STATS_SECT_ENTRY(obytes)
    STATS_SECT_ENTRY(oerr)
STATS_SECT_END

STATS_SECT_DECL(hci_sock_stats) hci_sock_stats;
STATS_NAME_START(hci_sock_stats)
    STATS_NAME(hci_sock_stats, imsg)
    STATS_NAME(hci_sock_stats, icmd)
    STATS_NAME(hci_sock_stats, ievt)
    STATS_NAME(hci_sock_stats, iacl)
    STATS_NAME(hci_sock_stats, ibytes)
    STATS_NAME(hci_sock_stats, ierr)
    STATS_NAME(hci_sock_stats, imem)
    STATS_NAME(hci_sock_stats, omsg)
    STATS_NAME(hci_sock_stats, oacl)
    STATS_NAME(hci_sock_stats, ocmd)
    STATS_NAME(hci_sock_stats, oevt)
    STATS_NAME(hci_sock_stats, obytes)
    STATS_NAME(hci_sock_stats, oerr)
STATS_NAME_END(hci_sock_stats)

/***
 * NOTES:
 * The "skip" definitions are here so that when buffers cannot be allocated,
 * the command or acl packets are simply skipped so that the HCI interface
 * does not lose synchronization and resets dont (necessarily) occur.
 */
#define BLE_HCI_UART_H4_NONE        0x00
#define BLE_HCI_UART_H4_CMD         0x01
#define BLE_HCI_UART_H4_ACL         0x02
#define BLE_HCI_UART_H4_SCO         0x03
#define BLE_HCI_UART_H4_EVT         0x04
#define BLE_HCI_UART_H4_SYNC_LOSS   0x80
#define BLE_HCI_UART_H4_SKIP_CMD    0x81
#define BLE_HCI_UART_H4_SKIP_ACL    0x82

#if MYNEWT

#define BLE_SOCK_STACK_SIZE         \
    OS_STACK_ALIGN(MYNEWT_VAL(BLE_SOCK_STACK_SIZE))

struct os_task ble_sock_task;

#endif

static struct os_mempool ble_hci_sock_evt_hi_pool;
static os_membuf_t ble_hci_sock_evt_hi_buf[
        OS_MEMPOOL_SIZE(MYNEWT_VAL(BLE_HCI_EVT_HI_BUF_COUNT),
                        MYNEWT_VAL(BLE_HCI_EVT_BUF_SIZE))
];

static struct os_mempool ble_hci_sock_evt_lo_pool;
static os_membuf_t ble_hci_sock_evt_lo_buf[
        OS_MEMPOOL_SIZE(MYNEWT_VAL(BLE_HCI_EVT_LO_BUF_COUNT),
                        MYNEWT_VAL(BLE_HCI_EVT_BUF_SIZE))
];

static struct os_mempool ble_hci_sock_cmd_pool;
static os_membuf_t ble_hci_sock_cmd_buf[
        OS_MEMPOOL_SIZE(1, BLE_HCI_TRANS_CMD_SZ)
];

static struct os_mempool ble_hci_sock_acl_pool;
static struct os_mbuf_pool ble_hci_sock_acl_mbuf_pool;

#define ACL_BLOCK_SIZE  OS_ALIGN(MYNEWT_VAL(BLE_ACL_BUF_SIZE) \
                                 + BLE_MBUF_MEMBLOCK_OVERHEAD \
                                 + BLE_HCI_DATA_HDR_SZ, OS_ALIGNMENT)
/*
 * The MBUF payload size must accommodate the HCI data header size plus the
 * maximum ACL data packet length. The ACL block size is the size of the
 * mbufs we will allocate.
 */

static os_membuf_t ble_hci_sock_acl_buf[
        OS_MEMPOOL_SIZE(MYNEWT_VAL(BLE_ACL_BUF_COUNT),
                        ACL_BLOCK_SIZE)
];

static ble_hci_trans_rx_cmd_fn *ble_hci_sock_rx_cmd_cb;
static void *ble_hci_sock_rx_cmd_arg;
static ble_hci_trans_rx_acl_fn *ble_hci_sock_rx_acl_cb;
static void *ble_hci_sock_rx_acl_arg;

static struct ble_hci_sock_state {
    int sock;
    struct ble_npl_eventq evq;
    struct ble_npl_event ev;
    struct ble_npl_callout timer;

    uint16_t rx_off;
    uint8_t rx_data[512];
} ble_hci_sock_state;

#if MYNEWT_VAL(BLE_SOCK_USE_TCP)
static int s_ble_hci_device = MYNEWT_VAL(BLE_SOCK_TCP_PORT);
#elif MYNEWT_VAL(BLE_SOCK_USE_LINUX_BLUE)
static int s_ble_hci_device = MYNEWT_VAL(BLE_SOCK_LINUX_DEV);
#endif

/**
 * Allocates a buffer (mbuf) for ACL operation.
 *
 * @return                      The allocated buffer on success;
 *                              NULL on buffer exhaustion.
 */
static struct os_mbuf *
ble_hci_trans_acl_buf_alloc(void)
{
    struct os_mbuf *m;

    /*
     * XXX: note that for host only there would be no need to allocate
     * a user header. Address this later.
     */
    m = os_mbuf_get_pkthdr(&ble_hci_sock_acl_mbuf_pool,
                           sizeof(struct ble_mbuf_hdr));
    return m;
}

static int
ble_hci_sock_acl_tx(struct os_mbuf *om)
{
    struct msghdr msg;
    struct iovec iov[8];
    int i;
    struct os_mbuf *m;
    uint8_t ch;

    memset(&msg, 0, sizeof(msg));
    memset(iov, 0, sizeof(iov));

    msg.msg_iov = iov;

    ch = BLE_HCI_UART_H4_ACL;
    iov[0].iov_len = 1;
    iov[0].iov_base = &ch;
    i = 1;
    for (m = om; m; m = SLIST_NEXT(m, om_next)) {
        iov[i].iov_base = m->om_data;
        iov[i].iov_len = m->om_len;
        i++;
    }
    msg.msg_iovlen = i;

    STATS_INC(hci_sock_stats, omsg);
    STATS_INC(hci_sock_stats, oacl);
    STATS_INCN(hci_sock_stats, obytes, OS_MBUF_PKTLEN(om) + 1);
    i = sendmsg(ble_hci_sock_state.sock, &msg, 0);
    os_mbuf_free_chain(om);
    if (i != OS_MBUF_PKTLEN(om) + 1) {
        if (i < 0) {
            dprintf(1, "sendmsg() failed : %d\n", errno);
        } else {
            dprintf(1, "sendmsg() partial write: %d\n", i);
        }
        STATS_INC(hci_sock_stats, oerr);
        return BLE_ERR_MEM_CAPACITY;
    }
    return 0;
}

static int
ble_hci_sock_cmdevt_tx(uint8_t *hci_ev, uint8_t h4_type)
{
    struct msghdr msg;
    struct iovec iov[8];
    int len;
    int i;
    uint8_t ch;

    memset(&msg, 0, sizeof(msg));
    memset(iov, 0, sizeof(iov));

    msg.msg_iov = iov;
    msg.msg_iovlen = 2;

    ch = h4_type;
    iov[0].iov_len = 1;
    iov[0].iov_base = &ch;
    iov[1].iov_base = hci_ev;
    if (h4_type == BLE_HCI_UART_H4_CMD) {
        len = BLE_HCI_CMD_HDR_LEN + hci_ev[2];
        STATS_INC(hci_sock_stats, ocmd);
    } else if (h4_type == BLE_HCI_UART_H4_EVT) {
        len = BLE_HCI_EVENT_HDR_LEN + hci_ev[1];
        STATS_INC(hci_sock_stats, oevt);
    } else {
        assert(0);
    }
    iov[1].iov_len = len;

    STATS_INC(hci_sock_stats, omsg);
    STATS_INCN(hci_sock_stats, obytes, len + 1);

    i = sendmsg(ble_hci_sock_state.sock, &msg, 0);
    ble_hci_trans_buf_free(hci_ev);
    if (i != len + 1) {
        if (i < 0) {
            dprintf(1, "sendmsg() failed : %d\n", errno);
        } else {
            dprintf(1, "sendmsg() partial write: %d\n", i);
        }
        STATS_INC(hci_sock_stats, oerr);
        return BLE_ERR_MEM_CAPACITY;
    }

    return 0;
}

static int
ble_hci_sock_rx_msg(void)
{
    struct ble_hci_sock_state *bhss;
    int len;
    struct os_mbuf *m;
    uint8_t *data;
    int sr;
    int rc;

    bhss = &ble_hci_sock_state;
    if (bhss->sock < 0) {
        return -1;
    }
    len = read(bhss->sock, bhss->rx_data + bhss->rx_off,
               sizeof(bhss->rx_data) - bhss->rx_off);
    if (len < 0) {
        return -2;
    }
    if (len == 0) {
        return -1;
    }
    bhss->rx_off += len;
    STATS_INCN(hci_sock_stats, ibytes, len);

    while (bhss->rx_off > 0) {
        switch (bhss->rx_data[0]) {
#if MYNEWT_VAL(BLE_CONTROLLER)
        case BLE_HCI_UART_H4_CMD:
            if (bhss->rx_off < BLE_HCI_CMD_HDR_LEN) {
                return -1;
            }
            len = 1 + BLE_HCI_CMD_HDR_LEN + bhss->rx_data[3];
            if (bhss->rx_off < len) {
                return -1;
            }
            STATS_INC(hci_sock_stats, imsg);
            STATS_INC(hci_sock_stats, icmd);
            data = ble_hci_trans_buf_alloc(BLE_HCI_TRANS_BUF_CMD);
            if (!data) {
                STATS_INC(hci_sock_stats, ierr);
                break;
            }
            memcpy(data, &bhss->rx_data[1], len - 1);
            OS_ENTER_CRITICAL(sr);
            rc = ble_hci_sock_rx_cmd_cb(data, ble_hci_sock_rx_cmd_arg);
            OS_EXIT_CRITICAL(sr);
            if (rc) {
                ble_hci_trans_buf_free(data);
                STATS_INC(hci_sock_stats, ierr);
                break;
            }
            break;
#endif
#if MYNEWT_VAL(BLE_HOST)
        case BLE_HCI_UART_H4_EVT:
            if (bhss->rx_off < BLE_HCI_EVENT_HDR_LEN) {
                return -1;
            }
            len = 1 + BLE_HCI_EVENT_HDR_LEN + bhss->rx_data[2];
            if (bhss->rx_off < len) {
                return -1;
            }
            STATS_INC(hci_sock_stats, imsg);
            STATS_INC(hci_sock_stats, ievt);
            data = ble_hci_trans_buf_alloc(BLE_HCI_TRANS_BUF_EVT_HI);
            if (!data) {
                STATS_INC(hci_sock_stats, ierr);
                break;
            }
            memcpy(data, &bhss->rx_data[1], len - 1);
            OS_ENTER_CRITICAL(sr);
            rc = ble_hci_sock_rx_cmd_cb(data, ble_hci_sock_rx_cmd_arg);
            OS_EXIT_CRITICAL(sr);
            if (rc) {
                ble_hci_trans_buf_free(data);
                STATS_INC(hci_sock_stats, ierr);
                return 0;
            }
            break;
#endif
        case BLE_HCI_UART_H4_ACL:
            if (bhss->rx_off < BLE_HCI_DATA_HDR_SZ) {
                return -1;
            }
            len = 1 + BLE_HCI_DATA_HDR_SZ + (bhss->rx_data[4] << 8) +
              bhss->rx_data[3];
            if (bhss->rx_off < len) {
                return -1;
            }
            STATS_INC(hci_sock_stats, imsg);
            STATS_INC(hci_sock_stats, iacl);
            m = ble_hci_trans_acl_buf_alloc();
            if (!m) {
                STATS_INC(hci_sock_stats, imem);
                break;
            }
            if (os_mbuf_append(m, &bhss->rx_data[1], len - 1)) {
                STATS_INC(hci_sock_stats, imem);
                os_mbuf_free_chain(m);
                break;
            }
            OS_ENTER_CRITICAL(sr);
            ble_hci_sock_rx_acl_cb(m, ble_hci_sock_rx_acl_arg);
            OS_EXIT_CRITICAL(sr);
            break;
        default:
            STATS_INC(hci_sock_stats, ierr);
            break;
        }
        memmove(bhss->rx_data, &bhss->rx_data[len], bhss->rx_off - len);
        bhss->rx_off -= len;
    }
    return 0;
}

static void
ble_hci_sock_rx_ev(struct ble_npl_event *ev)
{
    int rc;
    ble_npl_time_t timeout;

    rc = ble_hci_sock_rx_msg();
    if (rc == 0) {
        ble_npl_eventq_put(&ble_hci_sock_state.evq, &ble_hci_sock_state.ev);
    } else {
        rc = ble_npl_time_ms_to_ticks(10, &timeout);
        ble_npl_callout_reset(&ble_hci_sock_state.timer, timeout);
    }
}

#if MYNEWT_VAL(BLE_SOCK_USE_TCP)
static int
ble_hci_sock_config(void)
{
    struct ble_hci_sock_state *bhss = &ble_hci_sock_state;
    struct sockaddr_in sin;
    ble_npl_time_t timeout;
    int s;
    int rc;

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(s_ble_hci_device);
    sin.sin_addr.s_addr = inet_addr("127.0.0.1");
#ifdef MN_OSX
    sin.sin_len = sizeof(sin);
#endif

#if 0
    if (bhss->sock >= 0) {
        close(bhss->sock);
        bhss->sock = -1;
    }
#endif
    if (bhss->sock < 0) {
        s = socket(PF_INET, SOCK_STREAM, 0);
        if (s < 0) {
            goto err;
        }

        rc = connect(s, (struct sockaddr *)&sin, sizeof(sin));
        if (rc) {
            dprintf(1, "connect() failed: %d\n", errno);
            goto err;
        }

        rc = 1;

        rc = ioctl(s, FIONBIO, (char *)&rc);
        if (rc) {
            goto err;
        }
        bhss->sock = s;
    }
    rc = ble_npl_time_ms_to_ticks(10, &timeout);
    if (rc) {
        goto err;
    }
    ble_npl_callout_reset(&ble_hci_sock_state.timer, timeout);

    return 0;
err:
    if (s >= 0) {
        close(s);
    }
    return BLE_ERR_HW_FAIL;
}
#endif

#if MYNEWT_VAL(BLE_SOCK_USE_LINUX_BLUE)
static int
ble_hci_sock_config(void)
{
    struct sockaddr_hci shci;
    int s;
    int rc;
    ble_npl_time_t timeout;

    memset(&shci, 0, sizeof(shci));
    shci.hci_family = AF_BLUETOOTH;
    shci.hci_dev = s_ble_hci_device;
    shci.hci_channel = HCI_CHANNEL_USER;

    if (ble_hci_sock_state.sock >= 0) {
        close(ble_hci_sock_state.sock);
        ble_hci_sock_state.sock = -1;
    }

    s = socket(PF_BLUETOOTH, SOCK_RAW, BTPROTO_HCI);
    if (s < 0) {
        dprintf(1, "socket() failed %d\n", errno);
        goto err;
    }

    /* 
     * HCI User Channel requires exclusive access to the device.
     * The device has to be down at the time of binding.
     */
    ioctl(s, HCIDEVDOWN, shci.hci_dev);

    rc = bind(s, (struct sockaddr *)&shci, sizeof(shci));
    if (rc) {
        dprintf(1, "bind() failed %d\n", errno);
        goto err;
    }

    rc = 1;

    rc = ioctl(s, FIONBIO, (char *)&rc);
    if (rc) {
        goto err;
    }
    ble_hci_sock_state.sock = s;

    rc = ble_npl_time_ms_to_ticks(10, &timeout);
    if (rc) {
        goto err;
    }
    ble_npl_callout_reset(&ble_hci_sock_state.timer, timeout);

    return 0;
err:
    if (s >= 0) {
        close(s);
    }
    return BLE_ERR_HW_FAIL;
}
#endif
/**
 * Sends an HCI event from the controller to the host.
 *
 * @param cmd                   The HCI event to send.  This buffer must be
 *                                  allocated via ble_hci_trans_buf_alloc().
 *
 * @return                      0 on success;
 *                              A BLE_ERR_[...] error code on failure.
 */
int
ble_hci_trans_ll_evt_tx(uint8_t *cmd)
{
    return ble_hci_sock_cmdevt_tx(cmd, BLE_HCI_UART_H4_EVT);
}

/**
 * Sends ACL data from controller to host.
 *
 * @param om                    The ACL data packet to send.
 *
 * @return                      0 on success;
 *                              A BLE_ERR_[...] error code on failure.
 */
int
ble_hci_trans_ll_acl_tx(struct os_mbuf *om)
{
    return ble_hci_sock_acl_tx(om);
}

/**
 * Sends an HCI command from the host to the controller.
 *
 * @param cmd                   The HCI command to send.  This buffer must be
 *                                  allocated via ble_hci_trans_buf_alloc().
 *
 * @return                      0 on success;
 *                              A BLE_ERR_[...] error code on failure.
 */
int
ble_hci_trans_hs_cmd_tx(uint8_t *cmd)
{
    return ble_hci_sock_cmdevt_tx(cmd, BLE_HCI_UART_H4_CMD);
}

/**
 * Sends ACL data from host to controller.
 *
 * @param om                    The ACL data packet to send.
 *
 * @return                      0 on success;
 *                              A BLE_ERR_[...] error code on failure.
 */
int
ble_hci_trans_hs_acl_tx(struct os_mbuf *om)
{
    return ble_hci_sock_acl_tx(om);
}

/**
 * Configures the HCI transport to call the specified callback upon receiving
 * HCI packets from the controller.  This function should only be called by by
 * host.
 *
 * @param cmd_cb                The callback to execute upon receiving an HCI
 *                                  event.
 * @param cmd_arg               Optional argument to pass to the command
 *                                  callback.
 * @param acl_cb                The callback to execute upon receiving ACL
 *                                  data.
 * @param acl_arg               Optional argument to pass to the ACL
 *                                  callback.
 */
void
ble_hci_trans_cfg_hs(ble_hci_trans_rx_cmd_fn *cmd_cb,
                     void *cmd_arg,
                     ble_hci_trans_rx_acl_fn *acl_cb,
                     void *acl_arg)
{
    ble_hci_sock_rx_cmd_cb = cmd_cb;
    ble_hci_sock_rx_cmd_arg = cmd_arg;
    ble_hci_sock_rx_acl_cb = acl_cb;
    ble_hci_sock_rx_acl_arg = acl_arg;
}

/**
 * Configures the HCI transport to operate with a host.  The transport will
 * execute specified callbacks upon receiving HCI packets from the controller.
 *
 * @param cmd_cb                The callback to execute upon receiving an HCI
 *                                  event.
 * @param cmd_arg               Optional argument to pass to the command
 *                                  callback.
 * @param acl_cb                The callback to execute upon receiving ACL
 *                                  data.
 * @param acl_arg               Optional argument to pass to the ACL
 *                                  callback.
 */
void
ble_hci_trans_cfg_ll(ble_hci_trans_rx_cmd_fn *cmd_cb,
                     void *cmd_arg,
                     ble_hci_trans_rx_acl_fn *acl_cb,
                     void *acl_arg)
{
    ble_hci_sock_rx_cmd_cb = cmd_cb;
    ble_hci_sock_rx_cmd_arg = cmd_arg;
    ble_hci_sock_rx_acl_cb = acl_cb;
    ble_hci_sock_rx_acl_arg = acl_arg;
}

/**
 * Allocates a flat buffer of the specified type.
 *
 * @param type                  The type of buffer to allocate; one of the
 *                                  BLE_HCI_TRANS_BUF_[...] constants.
 *
 * @return                      The allocated buffer on success;
 *                              NULL on buffer exhaustion.
 */
uint8_t *
ble_hci_trans_buf_alloc(int type)
{
    uint8_t *buf;

    switch (type) {
    case BLE_HCI_TRANS_BUF_CMD:
        buf = os_memblock_get(&ble_hci_sock_cmd_pool);
        break;
    case BLE_HCI_TRANS_BUF_EVT_HI:
        buf = os_memblock_get(&ble_hci_sock_evt_hi_pool);
        if (buf == NULL) {
            /* If no high-priority event buffers remain, try to grab a
             * low-priority one.
             */
            buf = os_memblock_get(&ble_hci_sock_evt_lo_pool);
        }
        break;

    case BLE_HCI_TRANS_BUF_EVT_LO:
        buf = os_memblock_get(&ble_hci_sock_evt_lo_pool);
        break;

    default:
        assert(0);
        buf = NULL;
    }

    return buf;
}

/**
 * Frees the specified flat buffer.  The buffer must have been allocated via
 * ble_hci_trans_buf_alloc().
 *
 * @param buf                   The buffer to free.
 */
void
ble_hci_trans_buf_free(uint8_t *buf)
{
    int rc;

    /*
     * XXX: this may look a bit odd, but the controller uses the command
     * buffer to send back the command complete/status as an immediate
     * response to the command. This was done to insure that the controller
     * could always send back one of these events when a command was received.
     * Thus, we check to see which pool the buffer came from so we can free
     * it to the appropriate pool
     */
    if (os_memblock_from(&ble_hci_sock_evt_hi_pool, buf)) {
        rc = os_memblock_put(&ble_hci_sock_evt_hi_pool, buf);
        assert(rc == 0);
    } else if (os_memblock_from(&ble_hci_sock_evt_lo_pool, buf)) {
        rc = os_memblock_put(&ble_hci_sock_evt_lo_pool, buf);
        assert(rc == 0);
    } else {
        assert(os_memblock_from(&ble_hci_sock_cmd_pool, buf));
        rc = os_memblock_put(&ble_hci_sock_cmd_pool, buf);
        assert(rc == 0);
    }
}

/**
 * Resets the HCI UART transport to a clean state.  Frees all buffers and
 * reconfigures the UART.
 *
 * @return                      0 on success;
 *                              A BLE_ERR_[...] error code on failure.
 */
int
ble_hci_trans_reset(void)
{
    int rc;

    ble_npl_callout_stop(&ble_hci_sock_state.timer);

    /* Reopen the UART. */
    rc = ble_hci_sock_config();
    if (rc != 0) {
        dprintf(1, "Failure restarting socket HCI\n");
        return rc;
    }

    return 0;
}

void
ble_hci_sock_ack_handler(void *arg)
{
    struct ble_npl_event *ev;

    while (1) {
        ev = ble_npl_eventq_get(&ble_hci_sock_state.evq, BLE_NPL_TIME_FOREVER);
        ble_npl_event_run(ev);
    }
}

static void
ble_hci_sock_init_task(void)
{
    ble_npl_eventq_init(&ble_hci_sock_state.evq);
    ble_npl_callout_stop(&ble_hci_sock_state.timer);
    ble_npl_callout_init(&ble_hci_sock_state.timer, &ble_hci_sock_state.evq,
                    ble_hci_sock_rx_ev, NULL);

#if MYNEWT
    {
        os_stack_t *pstack;

        pstack = malloc(sizeof(os_stack_t)*BLE_SOCK_STACK_SIZE);
        assert(pstack);
        os_task_init(&ble_sock_task, "hci_sock", ble_hci_sock_ack_handler, NULL,
                     MYNEWT_VAL(BLE_SOCK_TASK_PRIO), BLE_NPL_TIME_FOREVER, pstack,
                     BLE_SOCK_STACK_SIZE);
    }
#else
/*
 * For non-Mynewt OS it is required that OS creates task for HCI SOCKET
 * to run ble_hci_sock_ack_handler.
 */

#endif

}

void
ble_hci_sock_set_device(int dev)
{
    s_ble_hci_device = dev;
}

/**
 * Initializes the UART HCI transport module.
 *
 * @return                      0 on success;
 *                              A BLE_ERR_[...] error code on failure.
 */
void
ble_hci_sock_init(void)
{
    int rc;

    /* Ensure this function only gets called by sysinit. */
    SYSINIT_ASSERT_ACTIVE();

    memset(&ble_hci_sock_state, 0, sizeof(ble_hci_sock_state));
    ble_hci_sock_state.sock = -1;

    ble_hci_sock_init_task();
    ble_npl_event_init(&ble_hci_sock_state.ev, ble_hci_sock_rx_ev, NULL);

    rc = os_mempool_init(&ble_hci_sock_acl_pool,
                         MYNEWT_VAL(BLE_ACL_BUF_COUNT),
                         ACL_BLOCK_SIZE,
                         ble_hci_sock_acl_buf,
                         "ble_hci_sock_acl_pool");
    SYSINIT_PANIC_ASSERT(rc == 0);

    rc = os_mbuf_pool_init(&ble_hci_sock_acl_mbuf_pool,
                           &ble_hci_sock_acl_pool,
                           ACL_BLOCK_SIZE,
                           MYNEWT_VAL(BLE_ACL_BUF_COUNT));
    SYSINIT_PANIC_ASSERT(rc == 0);

    /*
     * Create memory pool of HCI command buffers. NOTE: we currently dont
     * allow this to be configured. The controller will only allow one
     * outstanding command. We decided to keep this a pool in case we allow
     * allow the controller to handle more than one outstanding command.
     */
    rc = os_mempool_init(&ble_hci_sock_cmd_pool,
                         1,
                         BLE_HCI_TRANS_CMD_SZ,
                         &ble_hci_sock_cmd_buf,
                         "ble_hci_sock_cmd_pool");
    SYSINIT_PANIC_ASSERT(rc == 0);

    rc = os_mempool_init(&ble_hci_sock_evt_hi_pool,
                         MYNEWT_VAL(BLE_HCI_EVT_HI_BUF_COUNT),
                         MYNEWT_VAL(BLE_HCI_EVT_BUF_SIZE),
                         &ble_hci_sock_evt_hi_buf,
                         "ble_hci_sock_evt_hi_pool");
    SYSINIT_PANIC_ASSERT(rc == 0);

    rc = os_mempool_init(&ble_hci_sock_evt_lo_pool,
                         MYNEWT_VAL(BLE_HCI_EVT_LO_BUF_COUNT),
                         MYNEWT_VAL(BLE_HCI_EVT_BUF_SIZE),
                         ble_hci_sock_evt_lo_buf,
                         "ble_hci_sock_evt_lo_pool");
    SYSINIT_PANIC_ASSERT(rc == 0);

    rc = ble_hci_sock_config();
    SYSINIT_PANIC_ASSERT_MSG(rc == 0, "Failure configuring socket HCI");

    rc = stats_init_and_reg(STATS_HDR(hci_sock_stats),
                            STATS_SIZE_INIT_PARMS(hci_sock_stats, STATS_SIZE_32),
                            STATS_NAME_INIT_PARMS(hci_sock_stats), "hci_socket");
    SYSINIT_PANIC_ASSERT(rc == 0);
}
