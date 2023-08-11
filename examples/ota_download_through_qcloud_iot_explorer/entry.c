#include "tos_k.h"

/* 用户根据自己的底层通信链路来配置此宏
 * 如果是基于以太网 lwip 的链路，这里应该定义 USE_LWIP
 * 如果是基于模组的通信链路，这里应该定义相应的模组宏，如使用 ESP8266 则定义 USE_ESP8266
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

#include "ota_download.h"

extern ota_flash_drv_t stm32l4_norflash_onchip_drv_ota;
extern ota_flash_prop_t stm32l4_norflash_onchip_prop_ota;

void application_entry(void *arg)
{
#ifdef USE_LWIP
    dns_init();
    MX_LWIP_Init();
#endif

#ifdef USE_ESP8266x
    extern int esp8266_sal_init(hal_uart_port_t uart_port);
    extern int esp8266_join_ap(const char *ssid, const char *pwd);
    esp8266_sal_init(HAL_UART_PORT_0);
    esp8266_join_ap("SheldonDai", "srnr6x9xbhmb0");
#endif

#ifdef USE_NB_BC35
    extern int bc35_28_95_sal_init(hal_uart_port_t uart_port);
    bc35_28_95_sal_init(HAL_UART_PORT_0);
#endif

    uint32_t partition_addr = 0x08024800;

    if (ota_env_init(OTA_UPDATE_IN_POSITION, partition_addr, &stm32l4_norflash_onchip_drv_ota, &stm32l4_norflash_onchip_prop_ota) < 0) {
        printf("ota env init failed!\n");
        return;
    }

    ota_download_through_explorer();

    while (1) {
        printf("This is a qcloud explorer sdk ota demo!\r\n");
        tos_task_delay(1000);
    }
}

