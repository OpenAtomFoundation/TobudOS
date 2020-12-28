#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nimble/nimble_port_tencentos_tiny.h"

#include "host/ble_hs.h"
#include "host/util/util.h"
#include "host/ble_gatt.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

#include "boards.h"
#include "ble_qiot_config.h"
#include "flash_storage.h"

#include "ble_qiot_export.h"
#include "ble_qiot_import.h"


extern int gatt_svr_init(void);
extern void board_init(void);
extern void nimble_port_init(void);

static const char device_name[] = "TOS LLSync";

static uint8_t ble_addr_type;

void ble_boot(void *arg)
{
    int rc = 0;

    extern void nimble_port_run(void);
    nimble_port_tencentos_tiny_init(nimble_port_run);

    /* make sure synchronization of host and controller is done */
    while (!ble_hs_synced()) {
        ;
    }

    rc = ble_hs_util_ensure_addr(0);
    assert(rc == 0);
    rc = ble_hs_id_infer_auto(0, &ble_addr_type);
    assert(rc == 0);

    ble_svc_gap_init();
    ble_svc_gatt_init();

    /* verify and add our custom services */
    rc = gatt_svr_init();
    assert(rc == 0);

    /* init llsync, add llsync services */
    ble_qiot_explorer_init();

    /* set the device name */
    rc = ble_svc_gap_device_name_set(device_name);
    assert(rc == 0);

    /* reload the GATT server to link our added services */
    ble_gatts_start();

    /* llsync start advertising */
    ble_qiot_advertising_start();

    return;
}

static void leds_init(void)
{
    bsp_board_init(BSP_INIT_LEDS);
}

void property_power_switch(const char *data, uint16_t len)
{
    if (data[0]) {
        bsp_board_led_on(BSP_BOARD_LED_0);
        printf("Received LED ON!");
    } else {
        bsp_board_led_off(BSP_BOARD_LED_0);
        printf("Received LED OFF!");
    }
    return;
}

void action_led_blink(int ms)
{
    bsp_board_led_on(BSP_BOARD_LED_1);
    tos_sleep_ms(ms);
    bsp_board_led_off(BSP_BOARD_LED_1);
    tos_sleep_ms(ms);
    bsp_board_led_on(BSP_BOARD_LED_1);
    tos_sleep_ms(ms);
    bsp_board_led_off(BSP_BOARD_LED_1);
    tos_sleep_ms(ms);
    bsp_board_led_on(BSP_BOARD_LED_1);
    tos_sleep_ms(ms);
    bsp_board_led_off(BSP_BOARD_LED_1);
}

void report_reply_blink(void)
{
    bsp_board_led_on(BSP_BOARD_LED_2);
    tos_sleep_ms(200);
    bsp_board_led_off(BSP_BOARD_LED_2);
    tos_sleep_ms(200);
    bsp_board_led_on(BSP_BOARD_LED_2);
    tos_sleep_ms(200);
    bsp_board_led_off(BSP_BOARD_LED_2);
}


k_task_t ble_boot_task;
k_stack_t ble_boot_stack[2048];

int main(void)
{
    board_init();

    // other devices init
    leds_init();
    
    /* init flash that llsync need to save some device info */
    fstorage_init();

    /* Initialize OS */
    tos_knl_init();

    nimble_port_init();

    /*
        in this case, ble_boot_task's priority must lower than MYNEWT_VAL_BLE_HOST_TASK_PRIORITY && MYNEWT_VAL_BLE_LL_TASK_PRIORITY,
        numerically bigger
        to give the host and ll task a chance to run first just after the nimble_port_tencentos_tiny_init.
     */
    tos_task_create(&ble_boot_task, "boot", ble_boot, NULL,
                        6,
                        ble_boot_stack, sizeof(ble_boot_stack),
                        0);
    tos_knl_start();
}

