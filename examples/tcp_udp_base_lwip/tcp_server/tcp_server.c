#include "tcp_server.h"
#include "stdio.h"
#include "string.h"
#include "ethernetif.h"
#include "lwip/api.h"
#include <lwip/sockets.h>
#include <lwip/err.h>
#include <lwip/sys.h>
#include <errno.h>

char buff[512];

void application_entry(void *arg)
{
    int n;
    int listenfd,connfd;
    struct sockaddr_in sockaddr;
    static int cnt=0;

    MX_LWIP_Init();

    memset(&sockaddr,0,sizeof(sockaddr));
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    sockaddr.sin_port = htons(8087);

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
        return;
    }

    if ((bind(listenfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr))) == -1){
        printf("bind socket error: %s(errno: %d)\n", strerror(errno), errno);
        return;
    }

    if ((listen(listenfd, 1024)) == -1) {
        printf("listen socket error: %s(errno: %d)\n", strerror(errno), errno);
        return;
    }

    printf("listen the port(8087)\n");

    for (;;) {
        if ((connfd = accept(listenfd, (struct sockaddr*)NULL, NULL)) == -1) {
            printf("accpet socket error: %s errno :%d\n",strerror(errno),errno);
            continue;
        }

        n = recv(connfd, buff, sizeof(buff), 0);
        buff[n] = '\0';
        printf("recv(%d) msg from client:%s\n", ++cnt, buff);
        if ((send(connfd, buff, strlen(buff), 0)) < 0) {
            printf("send msg error: %s errno : %d\n", strerror(errno), errno);
            break;
        }
        close(connfd);
    }

    close(listenfd);
}

