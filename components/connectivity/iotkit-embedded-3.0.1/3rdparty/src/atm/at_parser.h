/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _AT_PARSER_H_
#define _AT_PARSER_H_

#include "infra_config.h"

/* uart config */
#define AT_UART_PORT 1
#define AT_UART_LINUX_DEV    "/dev/ttyUSB0"
#define AT_UART_BAUDRATE     115200
#define AT_UART_DATA_WIDTH   DATA_WIDTH_8BIT
#define AT_UART_PARITY       NO_PARITY
#define AT_UART_STOP_BITS    STOP_BITS_1
#define AT_UART_FLOW_CONTROL FLOW_CONTROL_DISABLED
#define AT_UART_MODE         MODE_TX_RX
#define AT_UART_TIMEOUT_MS   1000

/* Delimiter */
#define AT_RECV_PREFIX          "\r\n"
#define AT_RECV_SUCCESS_POSTFIX "OK\r\n"
#define AT_RECV_FAIL_POSTFIX    "ERROR\r\n"
#define AT_SEND_DELIMITER       "\r"

#if defined(AT_TCP_HAL_SIM800)
#define AT_CMD_DATA_INTERVAL_MS 50
#endif

#ifdef PLATFORM_HAS_OS
#define AT_SINGLE_TASK    0
#else
#define AT_SINGLE_TASK    1
#endif

#ifndef bool
#define bool unsigned char
#endif

#ifndef true
#define true 1
#endif
#ifndef false
#define false 0
#endif

typedef struct {
    char *reply_prefix;
    char *reply_success_postfix;
    char *reply_fail_postfix;
} atcmd_config_t;

typedef void (*at_recv_cb)(void *arg, char *buf, int buflen);

/**
 * initialization
 * Configuration (e.g. AT_UART_PORT, UART_BAUDRATE) can be found
 * in above macro
 */
int at_parser_init(void);

/**
 * at send (format: command + delimiter + data) and wait reply
 *
 * @param cmd at command sending buf. MUST not be NULL.
 * @param cmdlen at command length.
 * @param delimiter whether sending delimiter, usually value is true
 * @param data data sending buf. NULL if no data.
 * @param datalen data length. Zero if no data.
 * @param replybuf reply buffer. MUST not be NULL.
 * @param bufsize reply buffer size
 * @param atcmdconfig AT cmd reply format config. Use default if NULL 
 */
int at_send_wait_reply(const char *cmd, int cmdlen, bool delimiter,
                       const char *data, int datalen,
                       char *replybuf, int bufsize,
                       const atcmd_config_t *atcmdconfig);

/**
 * at send (format: data + delimiter) and does not wait reply
 *
 * @param data sending buffer.
 * @param datalen sending length.
 * @param delimiter whether sending delimiter, usually value is false
 */
int at_send_no_reply(const char *data, int datalen, bool delimiter);


/**
 * at read for certain bytes of data
 *
 * @param outbuf output buffer.
 * @param readsize read size.
 */
int at_read(char *outbuf, int readsize);


/**
 * at register callback for recv
 *
 * @param prefix interested string. Must not be NULL.
 * @param postfix intersted postfix. NULL if postfix not provided.
 * @param recvbuf recv data buffer provided by caller, NULL if postfix not provided
 * @param bufsize buffer size for recv data, zero if postfix not provided
 * @param cb callback handle function. Must not be NULL.
 * @param arg callback handle function args. NULL if not used.
 */
int at_register_callback(const char *prefix, const char *postfix, char *recvbuf,
                         int bufsize, at_recv_cb cb, void *arg);


/**
 * at yield receive function. Only used in single task scenario
 *
 * @param replybuf reply buffer.
 * @param bufsize reply buffer size.
 * @param atcmdconfig AT cmd reply format config. Use default if NULL
 * @param timeout_ms receive timeout in millisecond
 */
int at_yield(char *replybuf, int bufsize, const atcmd_config_t *atcmdconfig,
             int timeout_ms);
#endif


