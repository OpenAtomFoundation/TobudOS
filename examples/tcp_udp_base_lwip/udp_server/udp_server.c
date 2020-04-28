#include "udp_server.h"
#include "lwip/api.h"
#include <lwip/sockets.h>
#include <lwip/err.h>
#include <lwip/sys.h>

char buff[512];	// UDP接收数据缓冲区
const unsigned short server_port = 8000;

void application_entry(void *arg)
{
    int sockfd, i;
    ssize_t err, len;
    struct sockaddr_in serveraddr, clientaddr;
    char ipstr[INET_ADDRSTRLEN]; // 16 Bytes
    socklen_t clientlen;

    MX_LWIP_Init();

    /* 构造用于UDP的套接字 */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY); //本地任意IP
    serveraddr.sin_port = htons(server_port);
    while (1) {
        bind(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
        printf("udp bind ip:ANY,port:%d\n", server_port);
        clientlen = sizeof(clientaddr);
        len = recvfrom(sockfd, buff, sizeof(buff), 0, (struct sockaddr *)&clientaddr, &clientlen);
        printf("client IP %s\tPORT %d\nrec:%s\n",  inet_ntop(AF_INET, &clientaddr.sin_addr.s_addr, ipstr, sizeof(ipstr)), ntohs(clientaddr.sin_port), buff);

        i = 0;
        while (i < len) {
            buff[i] = toupper(buff[i]);
            i++;
        }
        buff[i] = '\0';
        printf("send: %s\n", buff);
        err = sendto(sockfd, buff, strlen(buff), 0, (struct sockaddr *)&clientaddr, sizeof(clientaddr));
        if (err < 0){
            break;
        }
    }
    close(sockfd);
}

