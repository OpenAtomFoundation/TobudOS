/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>

#include "infra_config.h"
#include "mqtt_api.h"

#include "at_wrapper.h"
#include "at_parser.h"
#include "at_api.h"

#define TAG "at_mk3060_wifi"

#define CMD_FAIL_RSP "ERROR"

#define MAX_DATA_LEN   4096
#define MAX_DOMAIN_LEN 256
#define DATA_LEN_MAX 10
#define LINK_ID_MAX 5
#define SEM_WAIT_DURATION 5000

#define AT_CMD_EHCO_OFF "AT+UARTE=OFF"
#define STOP_CMD "AT+CIPSTOP"
#define STOP_CMD_LEN (sizeof(STOP_CMD)+1+1+5+1)

#define STOP_AUTOCONN_CMD "AT+CIPAUTOCONN"
#define STOP_AUTOCONN_CMD_LEN (sizeof(STOP_AUTOCONN_CMD)+1+1+5+1)

#ifdef AT_DEBUG_MODE
#define at_conn_hal_err(...)               do{HAL_Printf(__VA_ARGS__);HAL_Printf("\r\n");}while(0)
#define at_conn_hal_warning(...)           do{HAL_Printf(__VA_ARGS__);HAL_Printf("\r\n");}while(0)
#define at_conn_hal_info(...)              do{HAL_Printf(__VA_ARGS__);HAL_Printf("\r\n");}while(0)
#define at_conn_hal_debug(...)             do{HAL_Printf(__VA_ARGS__);HAL_Printf("\r\n");}while(0)
#else
#define at_conn_hal_err(...)               do{HAL_Printf(__VA_ARGS__);HAL_Printf("\r\n");}while(0)
#define at_conn_hal_warning(...)
#define at_conn_hal_info(...)              do{HAL_Printf(__VA_ARGS__);HAL_Printf("\r\n");}while(0)
#define at_conn_hal_debug(...)
#endif

void *HAL_SemaphoreCreate(void);
void HAL_SemaphoreDestroy(void *sem);
void HAL_SemaphorePost(void *sem);
int HAL_SemaphoreWait(void *sem, uint32_t timeout_ms);
typedef int (*at_data_check_cb_t)(char data);

/* Change to include data slink for each link id respectively. <TODO> */
typedef struct link_s {
    int fd;
    void* sem_start;
    void* sem_close;
} link_t;

static link_t g_link[LINK_ID_MAX];
static void* g_link_mutex;

static char localipaddr[16];

static int socket_data_info_get(char *buf, uint32_t buflen, at_data_check_cb_t valuecheck);
static int socket_data_len_check(char data);

#define WIFI_SSID "Yuemewifi-3766"
#define WIFI_PWD  "aos12345"
#define WIFI_TIMEOUT  20000
static uint8_t gotip = 0;

#define MK3080_MAX_PAYLOAD_SIZE (CONFIG_MQTT_MESSAGE_MAXLEN + CONFIG_MQTT_TOPIC_MAXLEN + 20)

#ifndef PLATFORM_HAS_DYNMEM
static uint8_t payload[MK3080_MAX_PAYLOAD_SIZE] = {0};
#endif

static uint64_t _get_time_ms(void)
{
    return HAL_UptimeMs();
}

static uint64_t _time_left(uint64_t t_end, uint64_t t_now)
{
    uint64_t t_left;

    if (t_end > t_now) {
        t_left = t_end - t_now;
    } else {
        t_left = 0;
    }

    return t_left;
}

static int at_connect_wifi(char *ssid, char *pwd, uint32_t timeout_ms)
{
    char conn_str[100]= {0};
    char out[20] = {0};
    uint64_t t_end, t_left;

    t_end = _get_time_ms() + timeout_ms;

    HAL_Snprintf(conn_str, 100, "AT+WJAP=%s,%s", ssid, pwd);

    if (at_send_wait_reply(conn_str, strlen(conn_str), true, NULL,
                           0, out, sizeof(out), NULL) < 0){
        return -1;
    }

    if (strstr(out, "ERROR") != NULL) {
        return -1;
    }

    while(0 == gotip) {
        HAL_SleepMs(50);

        t_left = _time_left(t_end, _get_time_ms());
        if (0 == t_left) {
            at_conn_hal_err("wifi connect timeout!\n");
            return -1;
        }

#ifndef PLATFORM_HAS_OS
        at_yield(NULL, 0, NULL, 100);
#endif
    }

    return 0;
}

static void handle_tcp_udp_client_conn_state(uint8_t link_id)
{
    char s[32] = {0};

    at_read(s, 6);
    if (strstr(s, "CLOSED") != NULL) {
        at_conn_hal_info("Server closed event.");
        if (g_link[link_id].sem_close) {
            at_conn_hal_debug(TAG, "sem is going to be waked up: 0x%x", &g_link[link_id].sem_close);
            HAL_SemaphorePost(g_link[link_id].sem_close); /* wakeup send task */
        }
        at_conn_hal_info("Server conn (%d) closed.", link_id);
    } else if (strstr(s, "CONNEC") != NULL) {
        at_conn_hal_info("Server conn (%d) successful.", link_id);
        at_read(s, 3);
        if (g_link[link_id].sem_start) {
            at_conn_hal_debug("sem is going to be waked up: 0x%x", &g_link[link_id].sem_start);
            HAL_SemaphorePost(g_link[link_id].sem_start); /*  wakeup send task */
        }
    } else if (strstr(s, "DISCON") != NULL) {
        at_conn_hal_info("Server conn (%d) disconnected.", link_id);
        at_read(s, 6);
    } else {
        at_conn_hal_warning("No one handle this unkown event!!!");
    }
}

static int socket_data_len_check(char data)
{
    if (data > '9' || data < '0') {
        return -1;
    }
    return 0;
}

static int socket_data_info_get(char *buf, uint32_t buflen, at_data_check_cb_t valuecheck)
{
    uint32_t i = 0;

    if (NULL == buf || 0 == buflen) {
        return -1;
    }

    do {
        at_read(&buf[i], 1);
        if (buf[i] == ',') {
            buf[i] = 0;
            break;
        }
        if (i >= buflen) {
            at_conn_hal_err("Too long length of data.reader is %s \r\n", buf);
            return -1;
        }
        if (NULL != valuecheck) {
            if (valuecheck(buf[i])) {
                at_conn_hal_err("Invalid string!!!, reader is %s \r\n", buf);
                return -1;
            }
        }
        i++;
    } while (1);

    return 0;
}

static void handle_socket_data()
{
    int link_id = 0;
    int ret = 0;
    uint32_t len = 0;
    char reader[16] = {0};
    char *recvdata = NULL;
    struct at_conn_input param;

    /* Eat the "OCKET," */
    at_read(reader, 6);
    if (memcmp(reader, "OCKET,", strlen("OCKET,")) != 0) {
        at_conn_hal_err("0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x invalid event format!!!\r\n",
             reader[0], reader[1], reader[2], reader[3], reader[4], reader[5]);
        return;
    }

    memset(reader, 0, sizeof(reader));
    ret = socket_data_info_get(reader, 1, &socket_data_len_check);
    if (ret) {
        at_conn_hal_err("Invalid link id 0x%02x !!!\r\n", reader[0]);
        return;
    }
    link_id = reader[0] - '0';

    memset(reader, 0, sizeof(reader));
    /* len */
    ret = socket_data_info_get(reader, sizeof(reader), &socket_data_len_check);
    if (ret) {
        at_conn_hal_err("Invalid datalen %s !!!\r\n", reader);
        return;
    }

    len = atoi(reader);
    if (len > MAX_DATA_LEN) {
        at_conn_hal_err("invalid input socket data len %d \r\n", len);
        return;
    }
    /* Prepare socket data */
#ifdef PLATFORM_HAS_DYNMEM
    recvdata = (char *)HAL_Malloc(len);
#else
    if (len <= MK3080_MAX_PAYLOAD_SIZE) {
        recvdata = (char *)payload;
    }
#endif
    if (!recvdata) {
        at_conn_hal_err("Error: %s %d out of memory, len is %d. \r\n", __func__, __LINE__, len);
        return;
    }

    ret = at_read(recvdata, len);
    if (ret != len) {
        at_conn_hal_err("at read error recv %d want %d!\n", ret, len);
        goto err;
    }

    memset(reader, 0, sizeof(reader));
    at_read(reader, 2);
    if (strncmp(reader, AT_RECV_PREFIX, 2) != 0) {
        at_conn_hal_err("at fail to read delimiter %s after data %s!\n", AT_RECV_PREFIX, reader);
        goto err;
    }

    if (g_link[link_id].fd >= 0) {
        param.fd = g_link[link_id].fd;
        param.data = recvdata;
        param.datalen = len;
        param.remote_ip = NULL;
        param.remote_port = 0;

        /* TODO get recv data src ip and port*/
        if (IOT_ATM_Input(&param) != 0) {
            at_conn_hal_err(" %s socket %d get data len %d fail to post to at_conn, drop it\n",
                 __func__, g_link[link_id].fd, len);
        }
    }

    at_conn_hal_debug("%s socket data on link %d with length %d posted to at_conn\n",
         __func__, link_id, len);

err:
#ifdef PLATFORM_HAS_DYNMEM
    HAL_Free(recvdata);
#endif
    return;
}

/*
 * Wifi station event handler. include:
 * +WEVENT:AP_UP
 * +WEVENT:AP_DOWN
 * +WEVENT:STATION_UP
 * +WEVENT:STATION_DOWN
 */
static void mk3060wifi_event_handler(void *arg, char *buf, int buflen)
{
    char eventhead[4] = {0};
    char eventotal[16] = {0};

    at_read(eventhead, 3);
    if (strcmp(eventhead, "AP_") == 0) {
        at_read(eventotal, 2);
        if (strcmp(eventotal, "UP") == 0) {

        } else if (strcmp(eventotal, "DO") == 0) {
            /*eat WN*/
            at_read(eventotal, 2);
        } else {
            at_conn_hal_err("!!!Error: wrong WEVENT AP string received. %s\r\n", eventotal);
            return;
        }
    } else if (strcmp(eventhead, "STA") == 0) {
        at_read(eventotal, 7);
        if (strcmp(eventotal, "TION_UP") == 0) {
            gotip = 1;
        } else if (strcmp(eventotal, "TION_DO") == 0) {
            /*eat WN*/
            at_read(eventotal, 2);
            memset(localipaddr, 0, sizeof(localipaddr));
            gotip = 0;
        } else {
            at_conn_hal_err("!!!Error: wrong WEVENT STATION string received. %s\r\n", eventotal);
            return;
        }
    } else {
        at_conn_hal_err("!!!Error: wrong WEVENT string received. %s\r\n", eventhead);
        return;
    }

    return;
}


/**
 * Network connection state event handler. Events includes:
 *   1. +CIPEVENT:id,SERVER,CONNECTED
 *   2. +CIPEVENT:id,SERVER,CLOSED
 *   3. +CIPEVENT:CLIENT,CONNECTED,ip,port
 *   4. +CIPEVENT:CLIENT,CLOSED,ip,port
 *   5. +CIPEVENT:id,UDP,CONNECTED
 *   6. +CIPEVENT:id,UDP,CLOSED
 *   7. +CIPEVENT:SOCKET,id,len,data
 *   8. +CIPEVENT:UDP_BROADCAST,ip,port,id,len,data
 */
static void net_event_handler(void *arg, char *buf, int buflen)
{
    char c;
    char s[32] = {0};

    at_read(&c, 1);
    if (c >= '0' && c < ('0' + LINK_ID_MAX)) {
        int link_id = c - '0';
        at_read(&c, 1);
        if (c != ',') {
            at_conn_hal_err("!!!Error: wrong CIPEVENT string. 0x%02x\r\n", c);
            return;
        }
        at_read(&c, 1);
        if (c == 'S') {
            at_conn_hal_debug("%s server conn state event, linkid: %d.", __func__, link_id);
            /* Eat the "ERVER," */
            at_read(s, 6);
            if (memcmp(s, "ERVER,", strlen("ERVER,")) != 0) {
                at_conn_hal_err("invalid event format 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x",
                     s[0], s[1], s[2], s[3], s[4], s[5]);
                return;
            }
            handle_tcp_udp_client_conn_state(link_id);
        } else if (c == 'U') {
            at_conn_hal_debug("%s UDP conn state event.", __func__);
            /* Eat the "DP," */
            at_read(s, 3);
            if (memcmp(s, "DP,", strlen("DP,")) != 0) {
                at_conn_hal_err("%s invalid event format 0x%02x 0x%02x 0x%02x \r\n", __FUNCTION__, s[0], s[1], s[2]);
                return;
            }
            handle_tcp_udp_client_conn_state(link_id);
        } else {
            at_conn_hal_err( "!!!Error: wrong CIPEVENT string 0x%02x at line %d\r\n", c, __LINE__);
            return ;
        }
    } else if (c == 'S') {
        at_conn_hal_debug("%s socket data event.", __func__);
        handle_socket_data();
    } else {
        at_conn_hal_err("!!!Error: wrong CIPEVENT string received. 0x%02x\r\n", c);
        return;
    }

    at_conn_hal_debug("%s exit.", __func__);
}

static void mk3060_uart_echo_off()
{
    char *at_echo_str = AT_CMD_EHCO_OFF;
    char out[64] = {0};

    at_send_wait_reply(at_echo_str, strlen(AT_CMD_EHCO_OFF), true,
                       NULL, 0, out, sizeof(out), NULL);
    at_conn_hal_debug("The AT response is: %s", out);
    if (strstr(out, CMD_FAIL_RSP) != NULL) {
        at_conn_hal_err("%s %d failed", __func__, __LINE__);
    }

    return;
}

static uint8_t inited = 0;

#define NET_OOB_PREFIX "+CIPEVENT:"
#define WIFIEVENT_OOB_PREFIX "+WEVENT:"
int HAL_AT_CONN_Init(void)
{
    int link;
    char cmd[STOP_AUTOCONN_CMD_LEN] = {0};
    char out[64] = {0};

    if (inited) {
        at_conn_hal_warning("at_conn component is already initialized");
        return 0;
    }

    if (NULL == (g_link_mutex = HAL_MutexCreate())) {
        at_conn_hal_err("Creating link mutex failed (%s %d).", __func__, __LINE__);
        return -1;
    }

    mk3060_uart_echo_off();

    memset(g_link, 0, sizeof(g_link));
    for (link = 0; link < LINK_ID_MAX; link++) {
        g_link[link].fd = -1;
        /*close all link */
        HAL_Snprintf(cmd, STOP_CMD_LEN - 1, "%s=%d", STOP_CMD, link);
        at_conn_hal_debug("%s %d - AT cmd to run: %s", __func__, __LINE__, cmd);

        at_send_wait_reply(cmd, strlen(cmd), true, NULL, 0, out,
                           sizeof(out), NULL);
        at_conn_hal_debug("The AT response is: %s", out);
        if (strstr(out, CMD_FAIL_RSP) != NULL) {
            at_conn_hal_debug("%s %d failed", __func__, __LINE__);
        }

        memset(cmd, 0, sizeof(cmd));

        /*close all link auto reconnect */
        HAL_Snprintf(cmd, STOP_AUTOCONN_CMD_LEN - 1, "%s=%d,0", STOP_AUTOCONN_CMD, link);
        at_conn_hal_debug("%s %d - AT cmd to run: %s", __func__, __LINE__, cmd);

        at_send_wait_reply(cmd, strlen(cmd), true, NULL, 0, out,
                           sizeof(out), NULL);
        at_conn_hal_debug("The AT response is: %s", out);
        if (strstr(out, CMD_FAIL_RSP) != NULL) {
            at_conn_hal_err("%s %d failed", __func__, __LINE__);
        }
        memset(cmd, 0, sizeof(cmd));
    }

    at_register_callback(NET_OOB_PREFIX, NULL, NULL, 0, net_event_handler, NULL);
    at_register_callback(WIFIEVENT_OOB_PREFIX, NULL, NULL, 0, mk3060wifi_event_handler, NULL);
    
    if (at_connect_wifi(WIFI_SSID, WIFI_PWD, WIFI_TIMEOUT) < 0) {
        at_conn_hal_err("%s %d failed", __func__, __LINE__);
        return -1;
    }

    inited = 1;

    return 0;
}

int HAL_AT_CONN_Deinit(void)
{
    if (!inited) {
        return 0;
    }

    HAL_MutexDestroy(g_link_mutex);

    return 0;
}

#define START_CMD "AT+CIPSTART"
#define START_CMD_LEN (sizeof(START_CMD)+1+1+13+1+MAX_DOMAIN_LEN+1+5+1+5+1)
static char *start_cmd_type_str[] = {"tcp_server", "tcp_client", \
                                     "ssl_client", "udp_broadcast", "udp_unicast"
                                    };

int HAL_AT_CONN_Start(at_conn_t *c)
{
    int link_id;
    char cmd[START_CMD_LEN] = {0};
    char out[256] = {0};

    if (!c || !c->addr) {
        at_conn_hal_err("%s %d - invalid argument", __func__, __LINE__);
        return -1;
    }

    HAL_MutexLock(g_link_mutex);
    for (link_id = 0; link_id < LINK_ID_MAX; link_id++) {
        if (g_link[link_id].fd >= 0) {
            continue;
        } else {
            g_link[link_id].fd = c->fd;
            if (NULL == (g_link[link_id].sem_start = HAL_SemaphoreCreate())) {
                at_conn_hal_err("failed to allocate semaphore %s", __func__);
                g_link[link_id].fd = -1;
                return -1;
            }

            if (NULL == (g_link[link_id].sem_close = HAL_SemaphoreCreate())) {
                at_conn_hal_err("failed to allocate semaphore %s", __func__);
                HAL_SemaphoreDestroy(g_link[link_id].sem_start);
                g_link[link_id].fd = -1;
                return -1;
            }
            break;
        }
    }

    HAL_MutexUnlock(g_link_mutex);

    /* The caller should deal with this failure */
    if (link_id >= LINK_ID_MAX) {
        at_conn_hal_info("No link available for now, %s failed.", __func__);
        return -1;
    }

    at_conn_hal_debug("Creating %s connection ...", start_cmd_type_str[c->type]);

    switch (c->type) {
        case TCP_CLIENT:
            HAL_Snprintf(cmd, START_CMD_LEN - 5 - 1, "%s=%d,%s,%s,%d",
                     START_CMD, link_id, start_cmd_type_str[c->type],
                     c->addr, c->r_port);
            if (c->l_port >= 0) {
                HAL_Snprintf(cmd + strlen(cmd), 7, ",%d", c->l_port);
            }
            break;
        default:
            at_conn_hal_err("Invalid connection type.");
            goto err;
    }

    at_conn_hal_debug("\r\n%s %d - AT cmd to run: %s \r\n", __func__, __LINE__, cmd);

    at_send_wait_reply(cmd, strlen(cmd), true, NULL, 0, out,
                       sizeof(out), NULL);
    at_conn_hal_debug("The AT response is: %s", out);
    if (strstr(out, CMD_FAIL_RSP) != NULL) {
        at_conn_hal_err("%s %d failed", __func__, __LINE__);
        goto err;
    }

    if (HAL_SemaphoreWait(g_link[link_id].sem_start, SEM_WAIT_DURATION) != 0) {
        at_conn_hal_err("%s sem_wait failed", __func__);
        goto err;
    }

    at_conn_hal_debug("%s sem_wait succeed.", __func__);

    return 0;
err:
    HAL_MutexLock(g_link_mutex);
    if (g_link[link_id].sem_start) {
        HAL_SemaphoreDestroy(g_link[link_id].sem_start);
    }

    if (g_link[link_id].sem_close) {
        HAL_SemaphoreDestroy(g_link[link_id].sem_close);
    }
    g_link[link_id].fd = -1;
    HAL_MutexUnlock(g_link_mutex);
    return -1;
}

static int fd_to_linkid(int fd)
{
    int link_id;

    HAL_MutexLock(g_link_mutex);
    for (link_id = 0; link_id < LINK_ID_MAX; link_id++) {
        if (g_link[link_id].fd == fd) {
            break;
        }
    }

    HAL_MutexUnlock(g_link_mutex);

    return link_id;
}

#define SEND_CMD "AT+CIPSEND"
#define SEND_CMD_LEN (sizeof(SEND_CMD)+1+1+5+1+DATA_LEN_MAX+1)
int HAL_AT_CONN_Send(int fd,
                 uint8_t *data,
                 uint32_t len,
                 char remote_ip[16],
                 int32_t remote_port,
                 int32_t timeout)
{
    int link_id;
    char cmd[SEND_CMD_LEN] = {0}, out[128] = {0};

    if (!data) {
        return -1;
    }

    at_conn_hal_debug("%s on fd %d", __func__, fd);

    link_id = fd_to_linkid(fd);
    if (link_id < 0 || link_id >= LINK_ID_MAX) {
        at_conn_hal_err("No connection found for fd (%d) in %s", fd, __func__);
        return -1;
    }

    /* AT+CIPSEND=id, */
    HAL_Snprintf(cmd, SEND_CMD_LEN - 1, "%s=%d,", SEND_CMD, link_id);
    /* [remote_port,] */
    if (remote_port >= 0) {
        HAL_Snprintf(cmd + strlen(cmd), 7, "%d,", remote_port);
    }
    /* data_length */
    HAL_Snprintf(cmd + strlen(cmd), DATA_LEN_MAX + 1, "%d", len);
    at_conn_hal_debug("\r\n%s %d - AT cmd to run: %s\r\n", __func__, __LINE__, cmd);

    at_send_wait_reply((const char *)cmd, strlen(cmd), true, (const char *)data, len,
                       out, sizeof(out), NULL);

    at_conn_hal_debug("\r\nThe AT response is: %s\r\n", out);

    if (strstr(out, CMD_FAIL_RSP) != NULL) {
        at_conn_hal_debug("%s %d failed", __func__, __LINE__);
        return -1;
    }

    return 0;
}

#define DOMAIN_RSP "+CIPDOMAIN:"
#define DOMAIN_CMD "AT+CIPDOMAIN"
#define DOMAIN_CMD_LEN (sizeof(DOMAIN_CMD)+MAX_DOMAIN_LEN+1)
/* Return the first IP if multiple found. */
int HAL_AT_CONN_DomainToIp(char *domain,
                                 char ip[16])
{
    char cmd[DOMAIN_CMD_LEN] = {0}, out[256] = {0}, *head, *end;

    HAL_Snprintf(cmd, DOMAIN_CMD_LEN - 1, "%s=%s", DOMAIN_CMD, domain);
    at_conn_hal_debug("%s %d - AT cmd to run: %s", __func__, __LINE__, cmd);

    at_send_wait_reply(cmd, strlen(cmd), true, NULL, 0, out,
                       sizeof(out), NULL);
    at_conn_hal_debug("The AT response is: %s", out);
    if (strstr(out, AT_RECV_SUCCESS_POSTFIX) == NULL) {
        at_conn_hal_err("%s %d failed", __func__, __LINE__);
        return -1;
    }

    /**
     * +CIPDOMAIN:1\r\n
     * 180.97.33.108\r\n
     *
     * OK\r\n
     */
    if ((head = strstr(out, DOMAIN_RSP)) == NULL) {
        at_conn_hal_err("No IP info found in result string %s \r\n.", out);
        return -1;
    }

    /* Check the format */
    head += strlen(DOMAIN_RSP);
    if (head[0] < '0' && head[0] >= ('0' + LINK_ID_MAX)) {
        at_conn_hal_err("%s %d failed", __func__, __LINE__);
        goto err;
    }


    head++;
    if (memcmp(head, AT_RECV_PREFIX, strlen(AT_RECV_PREFIX)) != 0) {
        at_conn_hal_err("%s %d failed", __func__, __LINE__);
        goto err;
    }

    /* We find the IP head */
    head += strlen(AT_RECV_PREFIX);

    end = head;
    while (((end - head) < 15) && (*end != '\r')) {
        end++;
    }
    if (((end - head) < 6) || ((end - head) > 15)) {
        goto err;
    }

    /* We find a good IP, save it. */
    memcpy(ip, head, end - head);
    ip[end - head] = '\0';
    at_conn_hal_debug("get domain %s ip %s \r\n", domain, ip);
    return 0;

err:
    at_conn_hal_err("Failed to get IP due to unexpected result string %s \r\n.", out);
    return -1;
}


int HAL_AT_CONN_Close(int fd,
                  int32_t remote_port)
{
    int link_id;
    char cmd[STOP_CMD_LEN] = {0}, out[64];

    link_id = fd_to_linkid(fd);
    if (link_id < 0 || link_id >= LINK_ID_MAX) {
        at_conn_hal_err("No connection found for fd (%d) in %s", fd, __func__);
        return -1;
    }

    HAL_Snprintf(cmd, STOP_CMD_LEN - 1, "%s=%d", STOP_CMD, link_id);
    at_conn_hal_debug("%s %d - AT cmd to run: %s", __func__, __LINE__, cmd);

    at_send_wait_reply(cmd, strlen(cmd), true, NULL, 0, out,
                       sizeof(out), NULL);
    at_conn_hal_debug("The AT response is: %s", out);
    if (strstr(out, CMD_FAIL_RSP) != NULL) {
        at_conn_hal_err("%s %d failed", __func__, __LINE__);
        goto err;
    }

    if (HAL_SemaphoreWait(g_link[link_id].sem_close, SEM_WAIT_DURATION) != 0) {
        at_conn_hal_err("%s sem_wait failed", __func__);
        goto err;
    }

    at_conn_hal_debug("%s sem_wait succeed.", __func__);
err:
    HAL_MutexLock(g_link_mutex);

    if (g_link[link_id].sem_start) {
        HAL_SemaphoreDestroy(g_link[link_id].sem_start);
    }

    if (g_link[link_id].sem_close) {
        HAL_SemaphoreDestroy(g_link[link_id].sem_close);
    }
    g_link[link_id].fd = -1;
    HAL_MutexUnlock(g_link_mutex);
    return -1;

}
