#include "esp8266.h"
#include "mcu_init.h"
#include "cmsis_os.h"
#include "tos_at_socket.h"

#define TCP_TEST_TASK_STK_SIZE         4096
void tcp_test(void);
osThreadDef(tcp_test, osPriorityNormal, 1, TCP_TEST_TASK_STK_SIZE);

#define RECV_LEN            1024
uint8_t recv_data[RECV_LEN];

void tcp_test(void)
{
    int recv_len = -1;
    int fd, rc, cnt = 0;

    struct sockaddr_in addr;

    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("39.108.190.129");
    addr.sin_port = htons(8080);

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        printf("socket failed\n");
        return;
    }

    rc = connect(fd, (struct sockaddr *)&addr, sizeof(addr));
    if (rc < 0) {
        printf("connect failed\n");
        close(fd);
        return;
    }

    while (1) {
        send(fd, (const void*)"This is TCP Test!\r\n", strlen("This is TCP Test!\r\n"), 0);

        recv_len = recv(fd, recv_data, sizeof(recv_data), 0);
        if (recv_len < 0) {
            printf("task receive error\n");
        } else if (recv_len == 0) {
            printf("task receive none\n");
        } else {
            recv_data[recv_len] = 0;
            printf("task: receive len: %d\nmsg from remote: %s\n", recv_len, recv_data);
        }

        if (++cnt >= 20) {
            break;
        }

        tos_sleep_ms(2000);
    }

    close(fd);
}

#define USE_ESP8266

void application_entry(void *arg)
{
#ifdef USE_ESP8266
    esp8266_sal_init(HAL_UART_PORT_0);
    esp8266_join_ap("SheldonDai", "srnr6x9xbhmb0");
#endif

#ifdef USE_SIM800A
    sim800a_power_on();
    sim800a_sal_init(HAL_UART_PORT_2);
#endif

    osThreadCreate(osThread(tcp_test), NULL);
}

