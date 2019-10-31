/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _AT_WRAPPER_H_
#define _AT_WRAPPER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <inttypes.h>

#include "infra_config.h"
#include "wrappers_defs.h"

#ifndef NULL
#define NULL (void *)0
#endif

void *HAL_Malloc(uint32_t size);
void HAL_Free(void *ptr);
void HAL_Printf(const char *fmt, ...);
int HAL_Snprintf(char *str, const int len, const char *fmt, ...);
uint64_t HAL_UptimeMs(void);
void *HAL_MutexCreate(void);
void HAL_MutexDestroy(void *mutex);
void HAL_MutexLock(void *mutex);
void HAL_MutexUnlock(void *mutex);
void HAL_SleepMs(uint32_t ms);

#ifdef PLATFORM_HAS_OS
void *HAL_SemaphoreCreate(void);
void HAL_SemaphoreDestroy(void *sem);
void HAL_SemaphorePost(void *sem);
int HAL_SemaphoreWait(void *sem, uint32_t timeout_ms);

int HAL_ThreadCreate(
                     void **thread_handle,
                     void *(*work_routine)(void *),
                     void *arg,
                     hal_os_thread_param_t *hal_os_thread_param,
                     int *stack_used);
#endif

#define HAL_WAIT_FOREVER 0xFFFFFFFFU


#if defined(AT_PARSER_ENABLED)
/*
 * UART data width
 */
typedef enum {
    DATA_WIDTH_5BIT,
    DATA_WIDTH_6BIT,
    DATA_WIDTH_7BIT,
    DATA_WIDTH_8BIT,
    DATA_WIDTH_9BIT
} hal_uart_data_width_t;

/*
 * UART stop bits
 */
typedef enum {
    STOP_BITS_1,
    STOP_BITS_2
} hal_uart_stop_bits_t;

/*
 * UART flow control
 */
typedef enum {
    FLOW_CONTROL_DISABLED,
    FLOW_CONTROL_CTS,
    FLOW_CONTROL_RTS,
    FLOW_CONTROL_CTS_RTS
} hal_uart_flow_control_t;

/*
 * UART parity
 */
typedef enum {
    NO_PARITY,
    ODD_PARITY,
    EVEN_PARITY
} hal_uart_parity_t;

/*
 * UART mode
 */
typedef enum {
    MODE_TX,
    MODE_RX,
    MODE_TX_RX
} hal_uart_mode_t;

/*
 * UART configuration
 */
typedef struct {
    uint32_t                baud_rate;
    hal_uart_data_width_t   data_width;
    hal_uart_parity_t       parity;
    hal_uart_stop_bits_t    stop_bits;
    hal_uart_flow_control_t flow_control;
    hal_uart_mode_t         mode;
} uart_config_t;

typedef struct {
    uint8_t       port;   /* uart port */
    uart_config_t config; /* uart config */
    void         *priv;   /* priv data */
} uart_dev_t;

/**
 * Initialises a UART interface
 *
 *
 * @param[in]  uart  the interface which should be initialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t HAL_AT_Uart_Init(uart_dev_t *uart);

/**
 * Deinitialises a UART interface
 *
 * @param[in]  uart  the interface which should be deinitialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t HAL_AT_Uart_Deinit(uart_dev_t *uart);

/**
 * Transmit data on a UART interface
 *
 * @param[in]  uart     the UART interface
 * @param[in]  data     pointer to the start of data
 * @param[in]  size     number of bytes to transmit
 * @param[in]  timeout  timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                      if you want to wait forever
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t HAL_AT_Uart_Send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout);

/**
 * Receive data on a UART interface
 *
 * @param[in]   uart         the UART interface
 * @param[out]  data         pointer to the buffer which will store incoming data
 * @param[in]   expect_size  number of bytes to receive
 * @param[out]  recv_size    number of bytes received
 * @param[in]   timeout      timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                           if you want to wait forever
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t HAL_AT_Uart_Recv(uart_dev_t *uart, void *data, uint32_t expect_size,
                         uint32_t *recv_size, uint32_t timeout);
#endif

#if defined(AT_TCP_ENABLED)
typedef enum {
    /* WiFi */
    TCP_SERVER,
    TCP_CLIENT,
    SSL_CLIENT,
    UDP_BROADCAST,
    UDP_UNICAST,
    /*WiFi end */
    /* Add others hereafter */
} CONN_TYPE;

/* Fill necessary fileds according to the socket type. */
typedef struct {
    int fd; /* fd that are used in socket level */
    CONN_TYPE type;
    char *addr; /* remote ip or domain */
    int32_t r_port; /* remote port (set to -1 if not used) */
    int32_t l_port; /* local port (set to -1 if not used) */
    uint32_t tcp_keep_alive; /* tcp keep alive value (set to 0 if not used) */
} at_conn_t;

struct at_conn_input {
    int        fd;
    void      *data;
    uint32_t   datalen;
    char      *remote_ip;
    uint16_t   remote_port;
};

/**
 * Module low level init so that it's ready to setup socket connection.
 *
 * @return  0 - success, -1 - failure
 */
int HAL_AT_CONN_Init(void);


/**
 * Start a socket connection via module.
 *
 * @param[in]  conn - connect parameters which are used to setup
 *                 the socket connection.
 *
 * @return  0 - success, -1 - failure
 */
int HAL_AT_CONN_Start(at_conn_t *conn);


/**
 * Send data via module.
 * This function does not return until all data sent.
 *
 * @param[in]  fd - the file descripter to operate on.
 * @param[in]  data - pointer to data to send.
 * @param[in]  len - length of the data.
 * @param[in]  remote_ip - remote port number (optional).
 * @param[in]  remote_port - remote port number (optional).
 *
 * @return  0 - success, -1 - failure
 */
int HAL_AT_CONN_Send(int fd, uint8_t *data, uint32_t len, char remote_ip[16],
                     int32_t remote_port, int32_t timeout);


/**
 * Get IP information of the corresponding domain.
 * Currently only one IP string is returned (even when the domain
 * coresponses to mutliple IPs). Note: only IPv4 is supported.
 *
 * @param[in]   domain - the domain string.
 * @param[out]  ip - the place to hold the dot-formatted ip string.
 *
 * @return  0 - success, -1 - failure
 */
int HAL_AT_CONN_DomainToIp(char *domain, char ip[16]);


/**
 * Close the socket connection.
 *
 * @param[in]  fd - the file descripter to operate on.
 * @param[in]  remote_port - remote port number (optional).
 *
 * @return  0 - success, -1 - failure
 */
int HAL_AT_CONN_Close(int fd, int32_t remote_port);


/**
 * Destroy SAL or exit low level state if necessary.
 *
 * @return  0 - success, -1 - failure
 */
int HAL_AT_CONN_Deinit(void);

#elif defined(AT_MQTT_ENABLED)
#include "mqtt_api.h"

struct at_mqtt_input {
    char     *topic;
    uint32_t  topic_len;
    char     *message;
    uint32_t  msg_len;
};

int HAL_AT_MQTT_Init(iotx_mqtt_param_t *pInitParams);
int HAL_AT_MQTT_Deinit(void);
int HAL_AT_MQTT_Connect(char *proKey, char *devName, char *devSecret);
int HAL_AT_MQTT_Disconnect(void);
int HAL_AT_MQTT_Subscribe(const char *topic, int qos, unsigned int *mqtt_packet_id, int *mqtt_status, int timeout_ms);
int HAL_AT_MQTT_Unsubscribe(const char *topic, unsigned int *mqtt_packet_id, int *mqtt_status);
int HAL_AT_MQTT_Publish(const char *topic, int qos, const char *message, unsigned int msg_len);
int HAL_AT_MQTT_State(void);

#endif

#ifdef __cplusplus
}
#endif

#endif


