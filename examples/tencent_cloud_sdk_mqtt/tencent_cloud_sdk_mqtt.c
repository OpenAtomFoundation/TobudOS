#include "tos.h"

/* 用户根据自己的底层通信链路来配置此宏
 * 如果是基于以太网lwip的链路，这里应该定义 USE_LWIP
 * 如果是基于模组的通信链路，这里应该定义相应的模组宏，如使用ESP8266则定义 USE_ESP8266
 * */
#define USE_ESP8266

#ifdef USE_LWIP
#include "lwip/api.h"
#include "lwip/sockets.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#endif

#ifdef USE_ESP8266
#include "esp8266.h"
#endif

void application_entry(void *arg)
{
    extern void mqtt_basic_thread(void);

#ifdef USE_LWIP
    dns_init();
    MX_LWIP_Init();
#endif

#ifdef USE_ESP8266
    extern int esp8266_sal_init(hal_uart_port_t uart_port);
    extern int esp8266_join_ap(const char *ssid, const char *pwd);
    esp8266_sal_init(HAL_UART_PORT_0);
    esp8266_join_ap("SheldonDai", "srnr6x9xbhmb0");
#endif

#ifdef USE_NB_BC35
    extern int bc35_28_95_sal_init(hal_uart_port_t uart_port);
    bc35_28_95_sal_init(HAL_UART_PORT_0);
#endif

    mqtt_basic_thread();

    while (1) {
        printf("This is a mqtt demo!\r\n");
        tos_task_delay(1000);
    }
}

