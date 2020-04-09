/**
* iperf-liked network performance tool
*
*/

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <lwip/sockets.h>
#include "iperf/iperf.h"
#include "netif/ethernetif.h"
#include "osal/osal.h"
#include "utils/debug/log.h"


#define IPERF_PORT          5001
#define IPERF_BUFSZ         TCP_MSS

#define IPERF_MODE_STOP     0
#define IPERF_MODE_SERVER   1
#define IPERF_MODE_CLIENT   2

#define IPERF_MAX_ARGV      8
#define TCP_CLIENT_WITH_RAW_API       (1)

#define IPERF_PRINTF(format, ...)       LOG(LOG_LVL_INFO, format, ##__VA_ARGS__)

typedef struct {
    volatile int mode;
    int port;
    int delay;
    int pkt_len;
    char host[16];
    OS_Thread_t iperf_thr;
    OS_Semaphore_t kill_signal;
} IPERF_PARAM;
static IPERF_PARAM param = {IPERF_MODE_STOP, IPERF_PORT, };

typedef struct {
    int32_t id;
    unsigned int tv_sec;
    unsigned int tv_usec;
} UDP_datagram_t;
typedef struct {
    /*
     * flags is a bitmap for different options
     * the most significant bits are for determining
     * which information is available. So 1.7 uses
     * 0x80000000 and the next time information is added
     * the 1.7 bit will be set and 0x40000000 will be
     * set signifying additional information. If no
     * information bits are set then the header is ignored.
     * The lowest order diferentiates between dualtest and
     * tradeoff modes, wheither the speaker needs to start
     * immediately or after the audience finishes.
     */
    int32_t flags;
    int32_t num_threads;
    int32_t port;
    int32_t buffer_len;
    int32_t win_band;
    int32_t amount;
} client_hdr_t;
#define IPERF_HEADER_VERSION1 0x80000000

static unsigned char *iperf_get_data(int size)
{
    unsigned char *buffer;
    int i;

    buffer = (unsigned char *)OS_Malloc(size);
    if (buffer == NULL) {
        return NULL;
    }
    for(i = 0; i < size; i++){
        *(buffer + i) = '0' + (i%10);
    }
    return buffer;
}
static void iperf_udp_client(void *thread_param)
{
    int sock;
    int flag = 1;
    unsigned char *buffer;
    struct sockaddr_in server;
    uint32_t packet_count = 0;
    uint32_t tick;
    int send_size;
    UDP_datagram_t *udp_h;
    client_hdr_t *client_h;

    send_size = IPERF_BUFSZ > 1470 ? 1470 : IPERF_BUFSZ;
    buffer = iperf_get_data(send_size);
    if (buffer == NULL) {
        goto __exit;
    }

    while(param.mode != IPERF_MODE_STOP) {
        if(LINK_DOWN == ethernetif_get_link_state()) {
            IPERF_PRINTF("%s network link down, skip...\r\n", __func__);
            OS_SemaphoreWait(&param.kill_signal, 1000);
            continue;
        }
        sock = socket(PF_INET, SOCK_DGRAM, 0);
        if(sock < 0) {
            IPERF_PRINTF("can't create socket!! exit\r\n");
            goto __exit;
        }

        setsockopt(sock,
                   IPPROTO_IP,     /* set option at TCP level */
                   IP_TOS,     /* name of option */
                   (void *) &flag,  /* the cast is historical cruft */
                   sizeof(flag));    /* length of option value */

        server.sin_family = AF_INET;
        server.sin_port = htons(param.port);
        server.sin_addr.s_addr = inet_addr(param.host);

        // Init UDP data header
        udp_h = (UDP_datagram_t *)&buffer[0];
        client_h = (client_hdr_t *)&buffer[sizeof(UDP_datagram_t)];
        client_h->flags = htonl(IPERF_HEADER_VERSION1);
        client_h->num_threads = htonl(1);
        client_h->port = htons(param.port);
        client_h->buffer_len = 0;
        client_h->win_band = htonl(10000000);//UDP Bandwidth 1 MBytes/sec

        IPERF_PRINTF("iperf udp client mode run...\r\n");
        while (param.mode != IPERF_MODE_STOP) {
            tick = OS_GetTicks();
            udp_h->id = htonl(packet_count++);
            udp_h->tv_sec = htonl(tick / OS_HZ);
            udp_h->tv_usec = htonl((tick % OS_HZ) * 1000);
            sendto(sock, buffer, send_size, 0, (struct sockaddr *)&server, sizeof(struct sockaddr_in));
            if(param.delay > 0) {
                OS_SemaphoreWait(&param.kill_signal, param.delay);
            }
        }
        closesocket(sock);
        IPERF_PRINTF("Disconnected, iperf udp client shut down!\r\n");
    }

__exit:
    if(buffer){
        OS_Free(buffer);
    }
    OS_SemaphoreDelete(&param.kill_signal);
    OS_ThreadDelete(&param.iperf_thr);
}

static void iperf_udp_server(void *thread_param)
{
    int sock;
    int rcv_len;
    unsigned char *buffer;
    struct sockaddr_in server;
    struct sockaddr_in sender;
    int sender_len, r_size;
    uint64_t sentlen;
    uint32_t pcount = 0, last_pcount = 0;
    volatile uint32_t lost = 0, total = 0;
    uint32_t tick1, tick2;
    float f;
    char speed[64] = { 0 };
    struct timeval timeout;
    UDP_datagram_t *udp_h;

    rcv_len = 1470;
    buffer = (unsigned char *)OS_Malloc(rcv_len);
    if (buffer == NULL) {
        goto __exit;
    }
    memset(buffer, 0, rcv_len);
    while(param.mode != IPERF_MODE_STOP) {
        if(LINK_DOWN == ethernetif_get_link_state()) {
            IPERF_PRINTF("%s network link down, skip...\r\n", __func__);
            OS_SemaphoreWait(&param.kill_signal, 1000);
            continue;
        }
        sock = socket(PF_INET, SOCK_DGRAM, 0);
        if(sock < 0) {
            IPERF_PRINTF("can't create socket!! exit\r\n");
            OS_Free(buffer);
            goto __exit;
        }
        server.sin_family = AF_INET;
        server.sin_port = htons(param.port);
        server.sin_addr.s_addr = inet_addr("0.0.0.0");

        timeout.tv_sec = 2;
        timeout.tv_usec = 0;
        if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) == -1) {
            IPERF_PRINTF("setsockopt failed!!\r\n");
            closesocket(sock);
            OS_Free(buffer);
            goto __exit;
        }
        if (bind(sock, (struct sockaddr *)&server, sizeof(struct sockaddr_in)) < 0) {
            IPERF_PRINTF("iperf udp server bind failed!! exit\r\n");
            closesocket(sock);
            OS_Free(buffer);
            goto __exit;
        }
        IPERF_PRINTF("iperf udp server start up!\r\n");
        while (param.mode != IPERF_MODE_STOP) {
            tick1 = OS_GetTicks();
            tick2 = tick1;
            lost = 0;
            total = 0;
            sentlen = 0;
            while ((param.mode != IPERF_MODE_STOP) && ((tick2 - tick1) < (OS_HZ * 5))) {
                r_size = recvfrom(sock, buffer, rcv_len, 0, (struct sockaddr *)&sender, (socklen_t*)&sender_len);
                if ((r_size > 0) && (r_size > 200)) {//sizeof(UDP_datagram_t)
                    udp_h = (UDP_datagram_t *)buffer;
                    pcount = (uint32_t)ntohl(udp_h->id);
                    if (last_pcount < pcount) {
                        lost += pcount - last_pcount - 1;
                        total += pcount - last_pcount;
                    } else if (last_pcount == pcount){
                        total++;
                    }
                    last_pcount = pcount;
                    sentlen += r_size;
                }
                tick2 = OS_GetTicks();
            }
            if (sentlen > 0) {
                f = (float)(sentlen * OS_HZ / 125 / (tick2 - tick1));
                f /= 1000.0f;
                snprintf(speed, sizeof(speed), "%.4f Mbps! lost:%d total:%d\r\n", f, lost, total);
                IPERF_PRINTF("%s", speed);
            }
        }
        closesocket(sock);
    }
    OS_Free(buffer);

__exit:
    IPERF_PRINTF("Disconnected, iperf udp server shut down!\r\n");
    OS_SemaphoreDelete(&param.kill_signal);
    OS_ThreadDelete(&param.iperf_thr);
}

#if TCP_CLIENT_WITH_RAW_API
#include "lwip/tcp.h"
static unsigned char *iperf_tx_buffer = NULL;
static void iperf_client_conn_err(void *arg, err_t err)
{
    param.mode = IPERF_MODE_STOP;
}

static err_t iperf_client_send_more(struct tcp_pcb * pcb)
{
    uint8_t send_more;
    err_t err;
    uint16_t txlen;

    do {
        txlen = param.pkt_len;
        send_more = 1;

        do {
            err = tcp_write(pcb, iperf_tx_buffer, txlen, 0);
            if (err ==  ERR_MEM) {
                txlen >>= 1;
            }
            if(param.delay > 0) {
                OS_SemaphoreWait(&param.kill_signal, param.delay);
            }
        } while ((err == ERR_MEM) && (txlen >= (IPERF_BUFSZ >> 1)));

        if (err != ERR_OK) {
            send_more = 0;
        }

    } while(send_more);

    tcp_output(pcb);

    return ERR_OK;
}

static err_t iperf_client_poll(void *arg, struct tcp_pcb *tpcb)
{
    return iperf_client_send_more(tpcb);
}

static err_t iperf_client_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
    return iperf_client_send_more(tpcb);
}

static err_t iperf_client_connected(void *arg, struct tcp_pcb *pcb, err_t err)
{
	tcp_sent(pcb, iperf_client_sent);
    tcp_poll(pcb, iperf_client_poll, 2);
    tcp_write(pcb, iperf_tx_buffer, param.pkt_len, 0);
    return ERR_OK;
}

static void iperf_client_close(struct tcp_pcb *tpcb)
{
    err_t err;

    if (tpcb != NULL)
    {
        tcp_arg(tpcb, NULL);
        tcp_poll(tpcb, NULL, 0);
        tcp_sent(tpcb, NULL);
        tcp_recv(tpcb, NULL);
        tcp_err(tpcb, NULL);
        err = tcp_close(tpcb);
        if (err != ERR_OK) {
            tcp_abort(tpcb);
        }
    }
}

static void iperf_client(void *thread_param)
{
    struct tcp_pcb *pcb = NULL;
    ip4_addr_t server_ip;
    err_t err = ERR_OK;
    int port;

    iperf_tx_buffer = iperf_get_data(param.pkt_len);
    while (param.mode != IPERF_MODE_STOP) {
        if(LINK_DOWN == ethernetif_get_link_state()) {
            IPERF_PRINTF("%s network link down, skip...\r\n", __func__);
            OS_SemaphoreWait(&param.kill_signal, 1000);
            continue;
        }
        pcb = tcp_new();
        server_ip.addr = inet_addr(param.host);
        port = param.port;

        err = tcp_connect(pcb, &server_ip, port, iperf_client_connected);
        if(err != ERR_OK){
            iperf_client_close(pcb);
            continue;
        } else {
            tcp_err(pcb, iperf_client_conn_err);
            IPERF_PRINTF("Connect to iperf server successful!\r\n");
        }

        while (param.mode != IPERF_MODE_STOP) {
            OS_SemaphoreWait(&param.kill_signal, 3000);
        }

        iperf_client_close(pcb);

    }

    OS_Free(iperf_tx_buffer);
    IPERF_PRINTF("Disconnected, iperf tcp client shut down!\r\n");
    OS_SemaphoreDelete(&param.kill_signal);
    OS_ThreadDelete(&param.iperf_thr);
}
#else
static void iperf_client(void *thread_param)
{
    int sock;
    int ret;
    int tips = 1;
    unsigned char *send_buf;
    uint64_t sentlen;
    uint32_t tick1, tick2, delt;
    struct sockaddr_in addr;
    int flag = 1, len;

    char speed[32] = { 0 };

    send_buf = iperf_get_data(param.pkt_len);

    while (param.mode != IPERF_MODE_STOP) {
        if(LINK_DOWN == ethernetif_get_link_state()) {
            IPERF_PRINTF("%s network link down, skip...\r\n", __func__);
            OS_SemaphoreWait(&param.kill_signal, 1000);
            continue;
        }
        sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (sock < 0) {
            IPERF_PRINTF("create socket failed!\r\n");
            OS_Delay(1);
            continue;
        }

        addr.sin_family = PF_INET;
        addr.sin_port = htons(param.port);
        addr.sin_addr.s_addr = inet_addr((char *)param.host);

        ret = connect(sock, (const struct sockaddr *)&addr, sizeof(addr));
        if (ret == -1) {
            if (tips) {
                IPERF_PRINTF("Connect to iperf server faile, Waiting for the server to open!\r\n");
                tips = 0;
            }
            closesocket(sock);
            OS_Delay(1);
            continue;
        }

        IPERF_PRINTF("Connect to iperf server successful!\r\n");

        setsockopt(sock,
                   IPPROTO_TCP,     /* set option at TCP level */
                   TCP_NODELAY,     /* name of option */
                   (void *) &flag,  /* the cast is historical cruft */
                   sizeof(int));    /* length of option value */

        sentlen = 0;
        len = param.pkt_len;
        //tick1 = OS_GetTicks();
        while (param.mode != IPERF_MODE_STOP) {
            ret = send(sock, send_buf, len, 0);
            if (ret > 0) {
                sentlen += ret;
                len = param.pkt_len;
            }else{
                len >>= 1;
            }

            tick2 = OS_GetTicks();
            delt = tick2 - tick1;
            if (delt >= OS_HZ * 5) {
                float f;

                f = (float)(sentlen * OS_HZ / 125 / delt);
                f /= 1000.0f;
                snprintf(speed, sizeof(speed), "%.4f Mbps!\r\n", f);
                IPERF_PRINTF("%s", speed);
                tick1 = tick2;
                sentlen = 0;
            }
            if(param.delay > 0) {
                OS_SemaphoreWait(&param.kill_signal, param.delay);
            }
        }

        closesocket(sock);
        tips = 1;
    }
    IPERF_PRINTF("Disconnected, iperf tcp client shut down!\r\n");
    if(send_buf){
        OS_Free(send_buf);
    }
    OS_SemaphoreDelete(&param.kill_signal);
    OS_ThreadDelete(&param.iperf_thr);
}
#endif

void iperf_server(void *thread_param)
{
    int recv_len;
    unsigned char *recv_data;
    socklen_t sin_size;
    uint32_t tick1, tick2;
    int sock = -1, connected, bytes_received;
    uint64_t recvlen;
    struct sockaddr_in server_addr, client_addr;
    char speed[32] = { 0 };
    fd_set readset;
    struct timeval timeout;
    int flag = 1;

    recv_len = 1460;
    recv_data = (unsigned char *)OS_Malloc(recv_len);
    if (recv_data == NULL) {
        IPERF_PRINTF("No memory\r\n");
        goto __exit;
    }

    while (param.mode != IPERF_MODE_STOP) {
        if(LINK_DOWN == ethernetif_get_link_state()) {
            IPERF_PRINTF("%s network link down, skip...\r\n", __func__);
            OS_SemaphoreWait(&param.kill_signal, 1000);
            continue;
        }
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            IPERF_PRINTF("Socket error\r\n");
            goto __exit;
        }

        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(param.port);
        server_addr.sin_addr.s_addr = INADDR_ANY;
        memset(&(server_addr.sin_zero), 0x0, sizeof(server_addr.sin_zero));

        if (bind(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
            IPERF_PRINTF("Unable to bind\r\n");
            goto __exit;
        }

        if (listen(sock, 5) == -1) {
            IPERF_PRINTF("Listen error\r\n");
            goto __exit;
        }
        IPERF_PRINTF("iperf tcp server start up!\r\n");
        timeout.tv_sec = 3;
        timeout.tv_usec = 0;

        while (param.mode != IPERF_MODE_STOP) {
            FD_ZERO(&readset);
            FD_SET(sock, &readset);

            if (select(sock + 1, &readset, NULL, NULL, &timeout) == 0)
                continue;

            sin_size = sizeof(struct sockaddr_in);

            connected = accept(sock, (struct sockaddr *)&client_addr, &sin_size);

            IPERF_PRINTF("new client connected from (%s, %d)\r\n",
                         inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

            setsockopt(connected,
                       IPPROTO_TCP,     /* set option at TCP level */
                       TCP_NODELAY,     /* name of option */
                       (void *) &flag,  /* the cast is historical cruft */
                       sizeof(int));    /* length of option value */

            recvlen = 0;
            tick1 = OS_GetTicks();
            while (param.mode != IPERF_MODE_STOP) {
                bytes_received = recv(connected, recv_data, recv_len, 0);
                if (bytes_received <= 0) break;

                recvlen += bytes_received;

                tick2 = OS_GetTicks();
                if (tick2 - tick1 >= OS_HZ * 5) {
                    float f;

                    f = (float)(recvlen * OS_HZ / 125 / (tick2 - tick1));
                    f /= 1000.0f;
                    snprintf(speed, sizeof(speed), "%.4f Mbps!\r\n", f);
                    IPERF_PRINTF("%s", speed);
                    tick1 = tick2;
                    recvlen = 0;
                }
            }
            IPERF_PRINTF("client disconnected (%s, %d)\r\n",
                         inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
            if (connected >= 0) closesocket(connected);
            connected = -1;
        }
    }
    IPERF_PRINTF("iperf tcp server shut down!\r\n");
__exit:
    if (sock >= 0) closesocket(sock);
    if (recv_data) OS_Free(recv_data);
    OS_SemaphoreDelete(&param.kill_signal);
    OS_ThreadDelete(&param.iperf_thr);
}

static void iperf_usage(void)
{
    IPERF_PRINTF("Usage: iperf [-s|-c host] [options]\r\n");
    IPERF_PRINTF("       iperf [-h|--stop]\r\n");
    IPERF_PRINTF("Client/Server:\r\n");
    IPERF_PRINTF("  -u,         use UDP rather than TCP\r\n");
    IPERF_PRINTF("  -p,    #    server port to listen on/connect to (default 5001)\r\n");
    IPERF_PRINTF("Server specific:\r\n");
    IPERF_PRINTF("  -s,         run in server mode\r\n");
    IPERF_PRINTF("Client specific:\r\n");
    IPERF_PRINTF("  -c, <ip>    run in client mode, connecting to <ip>\r\n");
    IPERF_PRINTF("  -d,         Set packet delay(ms)\r\n");
    IPERF_PRINTF("  -l,         Set packet length\r\n");
    IPERF_PRINTF("\r\n");
    IPERF_PRINTF("Miscellaneous:\r\n");
    IPERF_PRINTF("  -h,         print this message and quit\r\n");
    IPERF_PRINTF("  --stop,     stop iperf program\r\n");
    IPERF_PRINTF("Example:\r\n");
    IPERF_PRINTF("Iperf TCP Server: iperf -s\r\n");
    IPERF_PRINTF("Iperf UDP Server: iperf -u -s\r\n");
    IPERF_PRINTF("Iperf TCP Client: iperf -c <ip> -p <port> -d 2 -l 1460\r\n");
    IPERF_PRINTF("Iperf UDP Client: iperf -u -c <ip> -p <port> -d 2 -l 1460\r\n");
    return;
}
static int parse_argv(char* str, int* argc, char** argv, int number)
{
    char *p;
    int num=0;
    int word_start = 1;

    if(argc == NULL || argv == NULL)
        return -1;

    p=str;

    while(*p) {
        if((*p == '\r') || (*p == '\n')) {
            *p = '\0';
            break;
        }
        if((*p == ' ') || (*p == '\t')) {
            word_start = 1;
            *p = '\0';
            p++;
            continue;
        }
        if(num >= number)
            break;

        if(word_start) {
            argv[num++] = p;
            word_start = 0;
        }
        p++;
    }

    *argc = num;

    return 0;
}

int iperf(char *args)
{
    char *argv[IPERF_MAX_ARGV] = {0};
    int argc = 0;
    int mode = 0; /* server mode */
    char *host = NULL;
    int port = IPERF_PORT;
    int use_udp = 0;
    int index = 1;
    int delay = 0;
    int len = IPERF_BUFSZ;

    parse_argv(args, &argc, argv, IPERF_MAX_ARGV);

    if (argc == 1) {
        goto __usage;
    }
    if (strcmp(argv[1], "-u") == 0) {
        index = 2;
        use_udp = 1;
    }
    if (strcmp(argv[index], "-h") == 0) goto __usage;
    else if (strcmp(argv[index], "--stop") == 0) {
        /* stop iperf */
        mode = param.mode;
        param.mode = IPERF_MODE_STOP;
        if(mode != IPERF_MODE_STOP){
            OS_SemaphoreRelease(&param.kill_signal);
        }
        return 0;
    } else if (strcmp(argv[index], "-s") == 0) {
        mode = IPERF_MODE_SERVER; /* server mode */

        /* iperf -s -p 5000 */
        if ((argc == 4) || (argc == 5)) {
            if (strcmp(argv[index + 1], "-p") == 0) {
                port = atoi(argv[index + 2]);
            } else {
                goto __usage;
            }
        }
    } else if (strcmp(argv[index], "-c") == 0) {
        mode = IPERF_MODE_CLIENT; /* client mode */
        if (argc < 3) {
            goto __usage;
        }

        host = argv[index + 1];
        if (argc == 5 || argc == 6) {
            /* iperf -c host -p port*/
            if (strcmp(argv[index + 2], "-p") == 0) {
                port = atoi(argv[index + 3]);
            } else {
                goto __usage;
            }
        } else if (argc == 7 || argc == 8) {
            /* iperf -c host -p port -d 2*/
            if (strcmp(argv[index + 2], "-p") == 0) {
                port = atoi(argv[index + 3]);
            } else {
                goto __usage;
            }
            if (strcmp(argv[index + 4], "-d") == 0) {
                delay = atoi(argv[index + 5]);
            } else {
                goto __usage;
            }
        } else if (argc == 9 || argc == 10) {
            /* iperf -c host -p port -d 2 -l 1460*/
            if (strcmp(argv[index + 2], "-p") == 0) {
                port = atoi(argv[index + 3]);
            } else {
                goto __usage;
            }
            if (strcmp(argv[index + 4], "-d") == 0) {
                delay = atoi(argv[index + 5]);
            } else {
                goto __usage;
            }
            if (strcmp(argv[index + 6], "-l") == 0) {
                len = atoi(argv[index + 7]);
                len = MIN(len, 1024*4);
            } else {
                goto __usage;
            }
        }
    } else if (strcmp(argv[index], "-h") == 0) {
        goto __usage;
    } else {
        goto __usage;
    }

    /* start iperf */
    if (param.mode == IPERF_MODE_STOP) {
        if(OS_ThreadIsValid(&(param.iperf_thr))){
            IPERF_PRINTF("Wait for the previous iperf thread stop...\r\n");
            return 0;
        }
        param.mode = mode;
        param.port = port;
        param.delay = delay;
        param.pkt_len = len;

        if (host) {
            memcpy(param.host, host, strlen(host));
        }

        if (OS_SemaphoreCreateBinary(&param.kill_signal) != OS_OK) {
            IPERF_PRINTF("OS_SemaphoreCreateBinary param.kill_signal fail.\r\n");
            return -1;
        }
        if (use_udp) {
            if (mode == IPERF_MODE_CLIENT) {
                if(OS_OK != OS_ThreadCreate(&(param.iperf_thr), "iperfc", iperf_udp_client, NULL, OS_PRIORITY_NORMAL, 6*256)) {
                    IPERF_PRINTF("OS_ThreadCreate g_iperf_thr error!\r\n");
                    return -1;
                }
            } else if (mode == IPERF_MODE_SERVER) {
                if(OS_OK != OS_ThreadCreate(&(param.iperf_thr), "iperfd", iperf_udp_server, NULL, OS_PRIORITY_NORMAL, 6*256)) {
                    IPERF_PRINTF("OS_ThreadCreate g_iperf_thr error!\r\n");
                    return -1;
                }
            }
        } else {
            if (mode == IPERF_MODE_CLIENT) {
                if(OS_OK != OS_ThreadCreate(&(param.iperf_thr), "iperfc", iperf_client, NULL, OS_PRIORITY_NORMAL, 6*256)) {
                    IPERF_PRINTF("OS_ThreadCreate g_iperf_thr error!\r\n");
                    return -1;
                }
            } else if (mode == IPERF_MODE_SERVER) {
                if(OS_OK != OS_ThreadCreate(&(param.iperf_thr), "iperfd", iperf_server, NULL, OS_PRIORITY_NORMAL, 6*256)) {
                    IPERF_PRINTF("OS_ThreadCreate g_iperf_thr error!\r\n");
                    return -1;
                }
            }
        }
    } else {
        IPERF_PRINTF("Please stop iperf firstly, by:\r\n");
        IPERF_PRINTF("iperf --stop\r\n");
    }

    return 0;

__usage:
    iperf_usage();
    return 0;
}

#if 0
//For DMA Transfer test
#include "hal/hal_dma.h"
typedef struct {
    volatile int mode;
    int dma_en;
    int len;
    int count;
    OS_Thread_t dma_thr;
} dma_tx_ctrl_t;
dma_tx_ctrl_t dma_tx_ctrl;

static void dma_tx(void *thread_param)
{
    unsigned char *send_buf, *_send_buf, *rcv_buf, *_rcv_buf;
    uint8_t offset = 0;
    int i;
    uint32_t tick1, tick2;

    send_buf = iperf_get_data(dma_tx_ctrl.len+3);
    if(((uint32_t)send_buf)&0x03){
        offset = 4-((uint32_t)send_buf&0x03);
    }
    _send_buf = send_buf + offset;

    offset = 0;
    rcv_buf = (unsigned char *)OS_Malloc(dma_tx_ctrl.len+3);
    if((uint32_t)rcv_buf&0x03){
        offset = 4-((uint32_t)rcv_buf&0x03);
    }
    _rcv_buf = rcv_buf + offset;

    while (dma_tx_ctrl.mode != IPERF_MODE_STOP) {
        if(LINK_DOWN == ethernetif_get_link_state()) {
            IPERF_PRINTF("%s network link down, skip...\r\n", __func__);
            OS_SecDelay(1);
            continue;
        }
        tick1 = OS_GetTicks();
        if(dma_tx_ctrl.dma_en == 0){
            for(i = dma_tx_ctrl.count; i > 0; i--){
                memcpy(_rcv_buf, _send_buf, dma_tx_ctrl.len);
            }
        }else{
            for(i = dma_tx_ctrl.count; i > 0; i--){
                HAL_DMA_StartTransfer_adv(DMA_CHANNEL_1, _send_buf, _rcv_buf, dma_tx_ctrl.len);
            }
        }
        tick2 = OS_GetTicks();
        IPERF_PRINTF("%s delt=%d\r\n", (dma_tx_ctrl.dma_en == 1)?"DMA":"memcpy", (tick2 - tick1));
        break;
    }
    IPERF_PRINTF("dma tx thread shut down!\r\n");
    if(send_buf){
        OS_Free(send_buf);
    }
    if(rcv_buf){
        OS_Free(rcv_buf);
    }
    OS_ThreadDelete(&dma_tx_ctrl.dma_thr);
}


//dma -e 0/1 -l 1500 -c 3000
//dma --stop
int dma_test(char *args)
{
    char *argv[IPERF_MAX_ARGV] = {0};
    int argc = 0;
    int mode = 0; /* server mode */

    DMA_InitTypeDef dma_config;

    parse_argv(args, &argc, argv, IPERF_MAX_ARGV);

    if (argc == 7) {
        dma_tx_ctrl.dma_en = atoi(argv[2]);
        dma_tx_ctrl.len = MIN(atoi(argv[4]), 4*1024);
        dma_tx_ctrl.count = atoi(argv[6]);
        mode = IPERF_MODE_CLIENT;
        if(dma_tx_ctrl.dma_en == 1){
            HAL_DMA_Init(DMA_CHANNEL_1, NULL);
            memset(&dma_config, 0, sizeof(DMA_InitTypeDef));
            dma_config.device = DMA_DEVICE_QSPI_RX;
            dma_config.type = DMA_TRANS_TYPE_MEMORY_TO_MEMORY_DMAC_FLOW_CONTROLLER;
            dma_config.msize = DMA_BURST_TRANSACTION_LENGTH_8;
            dma_config.src_inc = DMA_ADDRESS_INCREMENT;
            dma_config.dst_inc = DMA_ADDRESS_INCREMENT;
            dma_config.width = DMA_TRANSFER_WIDTH_32_BITS;
            dma_config.int_en = DMA_INTERRUPT_ENABLE;
            HAL_DMA_Config(DMA_CHANNEL_1, &dma_config);
            NVIC_EnableIRQ(DMA_IRQn);
        }
    }else if ((argc == 2) && (strcmp(argv[1], "--stop") == 0)){
        /* stop */
        if(dma_tx_ctrl.mode != IPERF_MODE_STOP){
            dma_tx_ctrl.mode = IPERF_MODE_STOP;
            if(dma_tx_ctrl.dma_en == 1){
                NVIC_DisableIRQ(DMA_IRQn);
                HAL_DMA_DeInit(DMA_CHANNEL_1);
            }
        }
        return 0;
    }
    /* start */
    if (dma_tx_ctrl.mode == IPERF_MODE_STOP) {
        dma_tx_ctrl.mode = mode;
        if(OS_OK != OS_ThreadCreate(&(dma_tx_ctrl.dma_thr), "dma_thr", dma_tx, NULL, OS_PRIORITY_NORMAL, 4*256)) {
            IPERF_PRINTF("OS_ThreadCreate dma_thr error!\r\n");
            return -1;
        }
    }

    return 0;
}
#endif

