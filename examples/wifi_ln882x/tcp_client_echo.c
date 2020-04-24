#include "cmsis_os.h"
#include "tcp_client_echo.h"

#include "utils/debug/art_assert.h"
#include "utils/debug/log.h"

#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "lwip/netifapi.h"

#define TCP_CLI_ECHO_STK_SIZE          512*3
void tcp_client_echo_task_entry(void *arg);
osThreadDef(tcp_client_echo_task_entry, osPriorityNormal, 1, TCP_CLI_ECHO_STK_SIZE);


#define BUFSZ   1500
static char recv_data[BUFSZ];

void tcp_client_echo_task_entry(void *arg)
{
    int ret;
    int conn;
    static uint64_t cli_recv_count = 0;

    struct sockaddr_in target_sockaddr;
    ip4_addr_t target_ser_ip;

    int optval = 0;
    int len = sizeof(int);
    struct timeval tv;

    target_ser_t target_ser = *(target_ser_t*)arg;


    if(!ipaddr_aton((const char *)target_ser.ip, &target_ser_ip))
    {
        LOG(LOG_LVL_ERROR, "[%s, %d]Convert target server ip addr failed.\r\n", __func__, __LINE__);
        return;
    }

    if ((conn = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        LOG(LOG_LVL_ERROR, "[%s, %d]Socket creat error.\r\n", __func__, __LINE__);
        return;
    }

    tv.tv_sec  = 5;
    tv.tv_usec = 0;
    setsockopt(conn, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(struct timeval));
    tv.tv_sec  = 5;
    tv.tv_usec = 0;
    setsockopt(conn, SOL_SOCKET, SO_SNDTIMEO, (char*)&tv, sizeof(struct timeval));

    target_sockaddr.sin_family = AF_INET;
    target_sockaddr.sin_port   = htons(target_ser.port);
    target_sockaddr.sin_addr.s_addr = target_ser_ip.addr;
    memset(&(target_sockaddr.sin_zero), 0, sizeof(target_sockaddr.sin_zero));

    while (1)
    {

        if(connect(conn, (struct sockaddr *)&target_sockaddr, sizeof(struct sockaddr)) == -1)
        {
            LOG(LOG_LVL_ERROR, "[%s, %d]Connect target tcp server faild!\r\n", __func__, __LINE__);
            OS_MsDelay(3000);
            continue;
        }

        LOG(LOG_LVL_ERROR, "[%s, %d]Connect target tcp server successful!\r\n", __func__, __LINE__);

        while (1)
        {
            ret = send(conn, "hello", strlen("hello"), 0);

            //---------------recv data-----------------------//
            memset(recv_data,0,sizeof(recv_data));

            ret = recv(conn, recv_data, BUFSZ-1, 0);
            getsockopt(conn, SOL_SOCKET, SO_ERROR, &optval, (socklen_t *)&len);
            if (ret < 0)
            {
                if(optval == EWOULDBLOCK){
                    LOG(LOG_LVL_ERROR, "[%s, %d] recv < 0 (recv timeout)\r\n", __func__, __LINE__ );
                    continue;
                } else if(optval == ECONNABORTED) {
                    LOG(LOG_LVL_ERROR, "[%s, %d] recv < 0 (conn abort)\r\n", __func__, __LINE__ );
                    closesocket(conn);
                    break;
                } else {
                    LOG(LOG_LVL_ERROR, "[%s, %d] recv < 0 (err optval = %d)\r\n", __func__, __LINE__,optval);
                    closesocket(conn);
                    break;
                }
            }
            else if(ret == 0)
            {
                if(optval == ERR_OK){
                    LOG(LOG_LVL_ERROR, "[%s, %d] recv = 0  ERR_OK\r\n", __func__, __LINE__);
                    continue;
                } else {
                    LOG(LOG_LVL_ERROR, "[%s, %d] recv = 0 (err optval = %d)\r\n", __func__, __LINE__,  optval);
                    closesocket(conn);
                    break;
                }
            }
            else
            {
                cli_recv_count += ret;
                LOG(LOG_LVL_ERROR, "client_recv_count = %u\r\n", cli_recv_count);
            }

            //---------------send data-----------------------//
            LOG(LOG_LVL_INFO, "received: %s\r\n", recv_data);

            getsockopt(conn, SOL_SOCKET, SO_ERROR, &optval, (socklen_t *)&len);
            if (ret < 0)
            {
                if(optval == EWOULDBLOCK){
                    LOG(LOG_LVL_ERROR, "[%s, %d] send < 0 (send timeout)\r\n", __func__, __LINE__);
                    continue;
                } else if(optval == ECONNABORTED) {
                    LOG(LOG_LVL_ERROR, "[%s, %d] send < 0 (conn abort)\r\n", __func__, __LINE__);
                    closesocket(conn);
                    break;
                } else {
                    LOG(LOG_LVL_ERROR, "[%s, %d] send < 0 (err optval = %d)\r\n", __func__, __LINE__, optval);
                    closesocket(conn);
                    break;
                }
            }
            else if(ret == 0)
            {
                if(optval == ERR_OK){
                    LOG(LOG_LVL_ERROR, "[%s, %d] send = 0  ERR_OK\r\n", __func__, __LINE__);
                    continue;
                } else {
                    LOG(LOG_LVL_ERROR, "[%s, %d] send = 0 (err optval = %d)\r\n", __func__, __LINE__, optval);
                    closesocket(conn);
                    break;
                }
            }

            OS_MsDelay(3000);
        }
    }

    closesocket(conn);
    LOG(LOG_LVL_ERROR, "[%s, %d]tcp client sock closed.\r\n", __func__, __LINE__);
}


void tcp_client_echo_task_creat(uint8_t * ser_ip, uint32_t port)
{
    target_ser_t ser = {0};
    ser.ip   = ser_ip;
    ser.port = port;

    osThreadCreate(osThread(tcp_client_echo_task_entry), &ser);
}



