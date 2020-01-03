#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nimble/nimble_port_tencentos_tiny.h"

#include "nimble/ble.h"
#include "host/ble_gap.h"

#define BLE_RAW_ADV_DATA_LEN        (31U)

typedef enum nimble_scan_status_en {
    NIMBLE_SCAN_STATUS_STOPPED,
    NIMBLE_SCAN_STATUS_SCANNING,
} nimble_scan_status_t;

typedef struct nimble_scan_item_st {
    k_list_t        list;
    ble_addr_t      addr;   /* ble address */
    uint8_t         adv_data[BLE_RAW_ADV_DATA_LEN]; /* advertising data received */
    uint8_t         adv_data_len;   /* length of the advertising data */
    int8_t          rssi;   /* last RSSI of a scanned node */
    uint8_t         type;   /* advertising packet type */
} nimble_scan_item_t;

/*
type: type of advertising packet, e.g BLE_HCI_ADV_TYPE_ADV_IND
disc_addr: advertising address of the source node
disc_rssi: RSSI value for the received packet
disc_adv_data: received advertising data
disc_adv_data_len: length of disc_adv_data in bytes
*/
typedef void(*nimble_scan_cb_t)(uint8_t type,
                                 const ble_addr_t *disc_addr, int8_t disc_rssi,
                                 const uint8_t *disc_adv_data, size_t disc_adv_data_len);

static uint8_t ble_addr_type;
static nimble_scan_cb_t scan_disc_cb  = NULL;
static struct ble_gap_disc_params scan_params = { 0 };

static nimble_scan_item_t entry_pool[200];
TOS_LIST_DEFINE(free_entry);
TOS_LIST_DEFINE(scan_entry);

void nimble_scan_info_list_init(void)
{
    int i = 0;

    for (i = 0; i < sizeof(entry_pool) / sizeof(entry_pool[0]); ++i) {
        tos_list_init(&entry_pool[i].list);
        tos_list_add(&entry_pool[i].list, &free_entry);
    }
}

void nimble_scan_item_addr_display(ble_addr_t *addr)
{
    int i = 0;

    printf("%02x", (int)addr->val[5]);

    for (i = 4; i >=0; --i) {
        printf(":%02x", addr->val[i]);
    }

    switch (addr->type) {
        case BLE_ADDR_PUBLIC:
            printf(" (PUBLIC)");
            break;

        case BLE_ADDR_RANDOM:
            printf(" (RANDOM)");
            break;

        case BLE_ADDR_PUBLIC_ID:
            printf(" (PUB_ID)");
            break;

        case BLE_ADDR_RANDOM_ID:
            printf(" (RAND_ID)");
            break;

        default:
            printf(" (UNKNOWN)");
            break;
    }
}

void nimble_scan_item_type_display(uint8_t type)
{
    switch (type) {
        case BLE_HCI_ADV_TYPE_ADV_IND:
            printf(" [IND]");
            break;

        case BLE_HCI_ADV_TYPE_ADV_DIRECT_IND_HD:
            printf(" [DIRECT_IND_HD]");
            break;

        case BLE_HCI_ADV_TYPE_ADV_SCAN_IND:
            printf(" [SCAN_IND]");
            break;

        case BLE_HCI_ADV_TYPE_ADV_NONCONN_IND:
            printf(" [NONCONN_IND]");
            break;

        case BLE_HCI_ADV_TYPE_ADV_DIRECT_IND_LD:
            printf(" [DIRECT_IND_LD]");
            break;

        default:
            printf(" [INVALID]");
            break;
    }
}

void nimble_scan_item_display(nimble_scan_item_t *item)
{
    nimble_scan_item_addr_display(&item->addr);
    nimble_scan_item_type_display(item->type);
    printf(", rssi: %d\n", item->rssi);
    printf("\n");
}

void nimble_scan_info_list_display(void)
{
    nimble_scan_item_t *iter;

    TOS_LIST_FOR_EACH_ENTRY(iter, nimble_scan_item_t, list, &scan_entry) {
        nimble_scan_item_display(iter);
    }
}

nimble_scan_item_t *nimble_scan_item_alloc(void)
{
    return TOS_LIST_FIRST_ENTRY_OR_NULL(&free_entry, nimble_scan_item_t, list);
}

int nimble_scan_item_is_exist(ble_addr_t *addr)
{
    nimble_scan_item_t *iter;

    TOS_LIST_FOR_EACH_ENTRY(iter, nimble_scan_item_t, list, &free_entry) {
        if (ble_addr_cmp(&addr, &iter->addr) == 0) {
            return K_TRUE;
        }
    }
    return K_FALSE;
}

void nimble_scan_info_save(uint8_t type, const ble_addr_t *disc_addr, int8_t disc_rssi,
                            const uint8_t *disc_adv_data, size_t disc_adv_data_len)
{
    int i = 0;
    nimble_scan_item_t *item;

    if (nimble_scan_item_is_exist(disc_addr)) {
        return;
    }

    item = nimble_scan_item_alloc();
    if (!item) {
        return;
    }

    memcpy(&item->addr, disc_addr, sizeof(ble_addr_t));
    if (disc_adv_data) {
        memcpy(&item->adv_data, disc_adv_data, disc_adv_data_len);
    }
    item->adv_data_len = disc_adv_data_len;
    item->rssi = disc_rssi;
    item->type = type;

    tos_list_del(&item->list);
    tos_list_add(&item->list, &scan_entry);
}

static int blescan_gap_event(struct ble_gap_event *event, void *arg)
{
    /* only interested in the DISC event */
    switch (event->type) {
    case BLE_GAP_EVENT_DISC:
        scan_disc_cb(event->disc.event_type, &event->disc.addr, event->disc.rssi,
                        event->disc.data, (size_t)event->disc.length_data);
        break;

    default:
        /* should never happen */
        MODLOG_DFLT(DEBUG, "unexpected event: (%d)\n", (int)event->type);
        assert(0);
        break;
    }

    return 0;
}

int nimble_scan_start(void)
{
    int rc;

    if (ble_gap_disc_active() != 0) {
        return 0;
    }

    rc = ble_gap_disc(ble_addr_type, BLE_HS_FOREVER,
                           &scan_params, blescan_gap_event, NULL);
    if (rc != 0) {
        MODLOG_DFLT(DEBUG, "[scanner] err: start failed (%i)\n", rc);
        return -1;
    }

    return 0;
}

void nimble_scan_stop(void)
{
    int rc;

    if (ble_gap_disc_active() != 1) {
        return;
    }

    rc = ble_gap_disc_cancel();
    assert(rc == 0);
}

nimble_scan_status_t nimble_scan_status(void)
{
    return ble_gap_disc_active() == 1 ? NIMBLE_SCAN_STATUS_SCANNING : NIMBLE_SCAN_STATUS_STOPPED;
}

int nimble_scan_init(const struct ble_gap_disc_params *params, nimble_scan_cb_t disc_cb)
{
    assert(disc_cb);

    if (params) {
        memcpy(&scan_params, params, sizeof(scan_params));
    } else {
        memset(&scan_params, 0, sizeof(scan_params));
    }
    scan_disc_cb = disc_cb;

    return 0;
}

int ble_boot(void)
{
    int rc;

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

    /* do a 'full time' scan, set the window equal the interval */
    struct ble_gap_disc_params scan_params = {
        .itvl = BLE_GAP_LIM_DISC_SCAN_INT,
        .window = BLE_GAP_LIM_DISC_SCAN_WINDOW,
        .filter_policy = 0,                         /* dummy */
        .limited = 0,                               /* no limited discovery */
        .passive = 0,                               /* no passive scanning */
        .filter_duplicates = 0,                     /* no duplicate filtering */
    };

    nimble_scan_info_list_init();

    /* initialize the nimble scanner */
    nimble_scan_init(&scan_params, nimble_scan_info_save);

    rc = nimble_scan_start();
    assert(rc == 0);

    tos_task_delay(2000);

    nimble_scan_stop();

    nimble_scan_info_list_display();

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
    tos_task_create(&ble_boot_task, "ble_boot", ble_boot, NULL,
                        6,
                        ble_boot_stack, sizeof(ble_boot_stack),
                        0);
    tos_knl_start();
}

