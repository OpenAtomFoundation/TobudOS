#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nimble/nimble_port_tencentos_tiny.h"

#include "host/ble_hs.h"
#include "host/util/util.h"
#include "host/ble_gatt.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

#include "blehr_sens.h"

static const char device_name[] = "Nimble Gatt";

static uint8_t ble_addr_type;

static void start_advertise(void);

static int blegatt_gap_event(struct ble_gap_event *event, void *arg)
{
    switch (event->type) {
        case BLE_GAP_EVENT_CONNECT:
            if (event->connect.status) {
                start_advertise();
            }
            break;

        case BLE_GAP_EVENT_DISCONNECT:
            start_advertise();
            break;
    }

    return 0;
}

static void start_advertise(void)
{
    int rc;
    struct ble_gap_adv_params advp;

    memset(&advp, 0, sizeof advp);
    advp.conn_mode = BLE_GAP_CONN_MODE_UND;
    advp.disc_mode = BLE_GAP_DISC_MODE_GEN;
    rc = ble_gap_adv_start(ble_addr_type, NULL, BLE_HS_FOREVER,
                           &advp, blegatt_gap_event, NULL);
    assert(rc == 0);
}

static void
put_ad(uint8_t ad_type, uint8_t ad_len, const void *ad, uint8_t *buf,
       uint8_t *len)
{
    buf[(*len)++] = ad_len + 1;
    buf[(*len)++] = ad_type;

    memcpy(&buf[*len], ad, ad_len);

    *len += ad_len;
}

static void
update_ad(void)
{
    uint8_t ad[BLE_HS_ADV_MAX_SZ];
    uint8_t ad_len = 0;
    uint8_t ad_flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;

    put_ad(BLE_HS_ADV_TYPE_FLAGS, 1, &ad_flags, ad, &ad_len);
    put_ad(BLE_HS_ADV_TYPE_COMP_NAME, sizeof(device_name), device_name, ad, &ad_len);

    ble_gap_adv_set_data(ad, ad_len);
}

#if 0

#if 1
/* UUID = 1bce38b3-d137-48ff-a13e-033e14c7a335 */
static const ble_uuid128_t gatt_svr_svc_rw_demo_uuid
        = BLE_UUID128_INIT(0x35, 0xa3, 0xc7, 0x14, 0x3e, 0x03, 0x3e, 0xa1, 0xff,
                0x48, 0x37, 0xd1, 0xb3, 0x38, 0xce, 0x1b);

static ble_uuid128_t svc_adv_uuids_128[] =
{
    { BLE_UUID_TYPE_128, { 0x35, 0xa3, 0xc7, 0x14, 0x3e, 0x03, 0x3e, 0xa1, 0xff,
            0x48, 0x37, 0xd1, 0xb3, 0x38, 0xce, 0x1b } },
};

#endif

static ble_uuid16_t svc_adv_uuids[] =
{
    { BLE_UUID_TYPE_16, GATT_DEVICE_INFO_UUID },
};

void ble_set_fields(void)
{
    struct ble_gap_adv_params adv_params;
    struct ble_hs_adv_fields fields;
    int rc;

    /*
     *  Set the advertisement data included in our advertisements:
     *     o Flags (indicates advertisement type and other general info)
     *     o Advertising tx power
     *     o Device name
     */
    memset(&fields, 0, sizeof(fields));

    /*
     * Advertise two flags:
     *      o Discoverability in forthcoming advertisement (general)
     *      o BLE-only (BR/EDR unsupported)
     */
    fields.flags = BLE_HS_ADV_F_DISC_GEN |
                    BLE_HS_ADV_F_BREDR_UNSUP;

    /*
     * Indicate that the TX power level field should be included; have the
     * stack fill this value automatically.  This is done by assigning the
     * special value BLE_HS_ADV_TX_PWR_LVL_AUTO.
     */
    fields.tx_pwr_lvl_is_present = 1;
    fields.tx_pwr_lvl = BLE_HS_ADV_TX_PWR_LVL_AUTO;

    fields.name = (uint8_t *)device_name;
    fields.name_len = strlen(device_name);
    fields.name_is_complete = 1;

    /*** 0x02,0x03 - 16-bit service class UUIDs. */
#if 1
    fields.num_uuids16 = 1;
    fields.uuids16 = svc_adv_uuids;
    fields.uuids16_is_complete = 1;
#endif

#if 0
    fields.num_uuids128 = 1;
    fields.uuids128 = svc_adv_uuids_128;
    fields.uuids128_is_complete = 1;
#endif

    rc = ble_gap_adv_set_fields(&fields);
    if (rc != 0) {
        MODLOG_DFLT(ERROR, "error setting advertisement data; rc=%d\n", rc);
        return;
    }

    /*** 0xff - Manufacturer specific data. */
    memset(&fields, 0, sizeof(fields));
    fields.mfg_data = (uint8_t*)"GZH:Bluetooth-BLE,177341833";
    fields.mfg_data_len = 27;

    rc = ble_gap_adv_rsp_set_fields(&fields);
    if (rc != 0) {
        MODLOG_DFLT(ERROR, "error setting advertisement data; rc=%d\n", rc);
        return;
    }
}

#endif

int ble_boot(void)
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

    /* set the device name */
    rc = ble_svc_gap_device_name_set(device_name);
    assert(rc == 0);

    /* reload the GATT server to link our added services */
    ble_gatts_start();

    update_ad();

#if 0
    ble_set_fields();
#endif

    start_advertise();

    return 0;
}

k_task_t ble_boot_task;
k_stack_t ble_boot_stack[2048];

int main(void)
{
    board_init();

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

