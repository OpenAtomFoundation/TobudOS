#include "udp_client.h"
#include "lwip/api.h"
#include <lwip/sockets.h>
#include <lwip/err.h>
#include <lwip/sys.h>

char buff[512];

const char *str = "Welcome to TencentOS Tiny!";
const char *server_ip = "182.254.196.11";
const unsigned short server_port = 8000;

void application_entry(void *arg)
{
    int confd;
    ssize_t reclen;
    struct timeval tv = {10, 0};
    struct sockaddr_in serveraddr;

    MX_LWIP_Init();

    // 1. 创建一个socket
    confd = socket(AF_INET, SOCK_DGRAM, 0);
    setsockopt(confd, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(struct timeval));

    // 2. 初始化服务器地址
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    inet_pton(AF_INET, server_ip, &serveraddr.sin_addr.s_addr);
    serveraddr.sin_port = htons(server_port);
    printf("sendto: %s:%d :%s\n", server_ip, server_port, str);

    // 3. 向服务器发送数据
    sendto(confd, str, strlen(str), 0, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    reclen = recvfrom(confd, buff, sizeof(buff), 0, NULL, 0);
    if (reclen > 0){
        buff[reclen] = '\0';
        printf("rec(%d):%s\n", reclen, buff);
    } else {
        printf("rec err(%d)", reclen);
    }

    // 5. 关闭socket
    close(confd);
}

