#include "tcp_client.h"
#include <stdio.h>
#include <string.h>
#include "lwip/api.h"
#include <lwip/sockets.h>
#include <lwip/err.h>
#include <lwip/sys.h>
#include <errno.h>

char recvline[512];

const char *str = "Welcome to TencentOS Tiny!";
const char *server_ip = "182.254.196.11";
const unsigned short server_port = 8087;

void application_entry(void *arg)
{
    int socketfd, len;
    static int cnt=0;
    struct sockaddr_in sockaddr;
    struct timeval tv = {10, 0};

    MX_LWIP_Init();

    while (1) {
        socketfd = socket(AF_INET, SOCK_STREAM, 0);
        setsockopt(socketfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(struct timeval));
        memset(&sockaddr, 0, sizeof(sockaddr));

        sockaddr.sin_addr.s_addr = inet_addr(server_ip);
        sockaddr.sin_family = AF_INET;
        sockaddr.sin_port = htons(server_port);

        if ((connect(socketfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr))) < 0) {
            printf("connect error %s errno: %d\n", strerror(errno), errno);
            goto tcp_client_exit;
        }

        printf("send(%d): %s:%d :%s\n", ++cnt, server_ip, server_port, str);
        if ((send(socketfd, str, strlen(str), 0)) < 0) {
            printf("send msg error: %s errno : %d\n", strerror(errno), errno);
            goto tcp_client_exit;
        }

        len = recv(socketfd, recvline, sizeof(recvline), 0);
        if (len > 0) {
            recvline[len] = '\0';
            printf("recv msg from server: %s\n", recvline);
        } else {
            printf("recv err(%d)\n", len);
        }

        close(socketfd);
        tos_task_delay(1000);
    }

tcp_client_exit:
    printf("exit\n");
}

