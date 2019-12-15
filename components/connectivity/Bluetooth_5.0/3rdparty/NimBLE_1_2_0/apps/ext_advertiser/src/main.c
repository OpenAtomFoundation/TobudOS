/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include "os/mynewt.h"
#include "console/console.h"
#include "config/config.h"
#include "nimble/ble.h"
#include "host/ble_hs.h"
#include "host/util/util.h"

#include"patterns.h"

static uint8_t id_addr_type;

static void start_legacy_duration(uint8_t pattern, bool configure);
static void start_ext_max_events(uint8_t pattern, bool configure);

static int
start_ext_max_events_gap_event(struct ble_gap_event *event, void *arg)
{
    static uint8_t pattern = 1;

    switch (event->type) {
    case BLE_GAP_EVENT_ADV_COMPLETE:
        break;
    default:
        assert(0);
        return 0;
    }

    assert(event->adv_complete.instance == 4);
    assert(event->adv_complete.reason == BLE_HS_ETIMEOUT);
    assert(event->adv_complete.num_ext_adv_events == 10);

    console_printf("instance %u terminated\n", event->adv_complete.instance);

    pattern++;

    start_ext_max_events(pattern, false);

    return 0;
}

/* Starts advertising instance with 100 max events and changing adv data pattern
 * and SID.
 */
static void
start_ext_max_events(uint8_t pattern, bool configure)
{
    struct ble_gap_ext_adv_params params;
    static uint8_t adv_data[600];
    struct os_mbuf *data;
    uint8_t instance = 4;
    ble_addr_t addr;
    int events = 10;
    int rc;

    if (configure) {
        /* use defaults for non-set params */
        memset (&params, 0, sizeof(params));

        /* advertise using random addr */
        params.own_addr_type = BLE_OWN_ADDR_RANDOM;

        params.primary_phy = BLE_HCI_LE_PHY_1M;
        params.secondary_phy = BLE_HCI_LE_PHY_1M;
        params.tx_power = 127;
        params.sid = pattern % 16;

        /* allow larger interval, 400 * 0.625ms with 100 events will give up to
         * ~2.5 seconds for instance
         */
        params.itvl_min = BLE_GAP_ADV_FAST_INTERVAL1_MIN;
        params.itvl_max = 400;

        /* configure instance 0 */
        rc = ble_gap_ext_adv_configure(instance, &params, NULL,
                                        start_ext_max_events_gap_event, NULL);
        assert (rc == 0);

        /* set random (NRPA) address for instance */
        rc = ble_hs_id_gen_rnd(1, &addr);
        assert (rc == 0);

        rc = ble_gap_ext_adv_set_addr(instance, &addr );
        assert (rc == 0);
    }

    /* in this case both advertising data and scan response is allowed, but
     * both are limited to 31 bytes each
     */

    /* get mbuf for adv data */
    data = os_msys_get_pkthdr(600, 0);
    assert(data);

    memset(adv_data, pattern, sizeof(adv_data));

    /* fill mbuf with adv data */
    rc = os_mbuf_append(data, adv_data, 600);
    assert(rc == 0);

   rc = ble_gap_ext_adv_set_data(instance, data);
   assert (rc == 0);

   /* start advertising */
    rc = ble_gap_ext_adv_start(instance, 0, events);
    assert (rc == 0);

    console_printf("instance %u started (PDUs with max events %d)\n",
                                                            instance, events);
}

static int
start_legacy_duration_gap_event(struct ble_gap_event *event, void *arg)
{
    static uint8_t pattern = 1;

    switch (event->type) {
    case BLE_GAP_EVENT_ADV_COMPLETE:
        break;
    default:
        assert(0);
        return 0;
    }

    assert(event->adv_complete.instance == 3);
    assert(event->adv_complete.reason == BLE_HS_ETIMEOUT);

    console_printf("instance %u terminated\n", event->adv_complete.instance);

    pattern++;

    start_legacy_duration(pattern, false);

    return 0;
}

/* Starts advertising instance with 5sec timeout and changing adv data pattern
 * and SID.
 */
static void
start_legacy_duration(uint8_t pattern, bool configure)
{
    struct ble_gap_ext_adv_params params;
    uint8_t adv_data[31];
    struct os_mbuf *data;
    uint8_t instance = 3;
    ble_addr_t addr;
    int duration = 500; /* 5seconds, 10ms units */
    int rc;

    if (configure) {
        /* use defaults for non-set params */
        memset (&params, 0, sizeof(params));

        /* enable advertising using legacy PDUs */
        params.legacy_pdu = 1;

        /* advertise using random addr */
        params.own_addr_type = BLE_OWN_ADDR_RANDOM;

        params.primary_phy = BLE_HCI_LE_PHY_1M;
        params.secondary_phy = BLE_HCI_LE_PHY_1M;
        params.tx_power = 127;
        params.sid = pattern % 16;

        /* configure instance 0 */
        rc = ble_gap_ext_adv_configure(instance, &params, NULL,
                                        start_legacy_duration_gap_event, NULL);
        assert (rc == 0);

        /* set random (NRPA) address for instance */
        rc = ble_hs_id_gen_rnd(1, &addr);
        assert (rc == 0);

        rc = ble_gap_ext_adv_set_addr(instance, &addr );
        assert (rc == 0);
    }

    /* in this case both advertising data and scan response is allowed, but
     * both are limited to 31 bytes each
     */

    /* get mbuf for adv data */
    data = os_msys_get_pkthdr(31, 0);
    assert(data);

    memset(adv_data, pattern, sizeof(adv_data));

    /* fill mbuf with adv data */
    rc = os_mbuf_append(data, adv_data, 31);
    assert(rc == 0);

   rc = ble_gap_ext_adv_set_data(instance, data);
   assert (rc == 0);

   /* start advertising */
    rc = ble_gap_ext_adv_start(instance, duration, 0);
    assert (rc == 0);

    console_printf("instance %u started (legacy PDUs with duration %d)\n",
                                                            instance, duration);
}

/* this is simple non-connectable scannable instance using legacy PUDs that
 * runs forever
 */
static void
start_scannable_legacy_ext(void)
{
    struct ble_gap_ext_adv_params params;
    struct os_mbuf *data;
    uint8_t instance = 2;
    ble_addr_t addr;
    int rc;

    /* use defaults for non-set params */
    memset (&params, 0, sizeof(params));

    /* enable scannable advertising using legacy PDUs */
    params.scannable = 1;
    params.legacy_pdu = 1;

    /* advertise using random addr */
    params.own_addr_type = BLE_OWN_ADDR_RANDOM;

    params.primary_phy = BLE_HCI_LE_PHY_1M;
    params.secondary_phy = BLE_HCI_LE_PHY_1M;
    params.tx_power = 127;
    params.sid = 2;

    /* configure instance 0 */
    rc = ble_gap_ext_adv_configure(instance, &params, NULL, NULL, NULL);
    assert (rc == 0);

    /* set random (NRPA) address for instance */
    rc = ble_hs_id_gen_rnd(1, &addr);
    assert (rc == 0);

    rc = ble_gap_ext_adv_set_addr(instance, &addr );
    assert (rc == 0);

    /* in this case both advertising data and scan response is allowed, but
     * both are limited to 31 bytes each
     */

    /* get mbuf for adv data */
    data = os_msys_get_pkthdr(31, 0);
    assert(data);

    /* fill mbuf with adv data */
    rc = os_mbuf_append(data, ext_adv_pattern_1, 31);
    assert(rc == 0);

   rc = ble_gap_ext_adv_set_data(instance, data);
   assert (rc == 0);

    /* get mbuf for scan rsp data */
    data = os_msys_get_pkthdr(31, 0);
    assert(data);

    /* fill mbuf with scan rsp data */
    rc = os_mbuf_append(data, ext_adv_pattern_1 + 31, 31);
    assert(rc == 0);

   rc = ble_gap_ext_adv_rsp_set_data(instance, data);
   assert (rc == 0);

   /* start advertising */
    rc = ble_gap_ext_adv_start(instance, 0, 0);
    assert (rc == 0);

    console_printf("instance %u started (scannable legacy PDUs)\n", instance);
}

static int
scannable_ext_gap_event(struct ble_gap_event *event, void *arg)
{
    switch (event->type) {
    default:
        break;
    }

    return 0;
}

/* this is simple scannable instance that runs forever
 * TODO Get scan request notifications.
 */
static void
start_scannable_ext(void)
{
    struct ble_gap_ext_adv_params params;
    struct os_mbuf *data;
    uint8_t instance = 1;
    ble_addr_t addr;
    int rc;

    /* use defaults for non-set params */
    memset (&params, 0, sizeof(params));

    /* enable scannable advertising */
    params.scannable = 1;

    /* enable scan request notification */
    params.scan_req_notif = 1;

    /* advertise using random addr */
    params.own_addr_type = BLE_OWN_ADDR_RANDOM;

    params.primary_phy = BLE_HCI_LE_PHY_1M;
    params.secondary_phy = BLE_HCI_LE_PHY_1M;
    params.tx_power = 127;
    params.sid = 1;

    /* configure instance 0 */
    rc = ble_gap_ext_adv_configure(instance, &params, NULL,
                                   scannable_ext_gap_event, NULL);
    assert (rc == 0);

    /* set random (NRPA) address for instance */
    rc = ble_hs_id_gen_rnd(1, &addr);
    assert (rc == 0);

    rc = ble_gap_ext_adv_set_addr(instance, &addr );
    assert (rc == 0);

    /* in this case only scan response is allowed */

    /* get mbuf for scan rsp data */
    data = os_msys_get_pkthdr(sizeof(ext_adv_pattern_1), 0);
    assert(data);

    /* fill mbuf with scan rsp data */
    rc = os_mbuf_append(data, ext_adv_pattern_1, sizeof(ext_adv_pattern_1));
    assert(rc == 0);

   rc = ble_gap_ext_adv_rsp_set_data(instance, data);
   assert (rc == 0);

   /* start advertising */
    rc = ble_gap_ext_adv_start(instance, 0, 0);
    assert (rc == 0);

    console_printf("instance %u started (scannable)\n", instance);
}

/* this is simple non-connectable instance that runs forever */
static void
start_non_connectable_ext(void)
{
    struct ble_gap_ext_adv_params params;
    struct os_mbuf *data;
    uint8_t instance = 0;
    int rc;

    /* use defaults for non-set params */
    memset (&params, 0, sizeof(params));

    /* advertise using ID addr */
    params.own_addr_type = id_addr_type;

    params.primary_phy = BLE_HCI_LE_PHY_1M;
    params.secondary_phy = BLE_HCI_LE_PHY_1M;
    params.tx_power = 127;
    params.sid = 0;

    /* configure instance */
    rc = ble_gap_ext_adv_configure(instance, &params, NULL, NULL, NULL);
    assert (rc == 0);

    /* in this case only advertisign data is allowed */

    /* get mbuf for adv data */
    data = os_msys_get_pkthdr(sizeof(ext_adv_pattern_1), 0);
    assert(data);

    /* fill mbuf with adv data */
    rc = os_mbuf_append(data, ext_adv_pattern_1, sizeof(ext_adv_pattern_1));
    assert(rc == 0);

   rc = ble_gap_ext_adv_set_data(instance, data);
   assert (rc == 0);

   /* start advertising */
    rc = ble_gap_ext_adv_start(instance, 0, 0);
    assert (rc == 0);

    console_printf("instance %u started (non-con non-scan)\n", instance);
}

static void
on_sync(void)
{
    int rc;

    console_printf("Synced, starting advertising\n");

    /* Make sure we have proper identity address set (public preferred) */
    rc = ble_hs_util_ensure_addr(0);
    assert(rc == 0);

    /* configure global address */
    rc = ble_hs_id_infer_auto(0, &id_addr_type);
    assert(rc == 0);

    start_non_connectable_ext();

    start_scannable_ext();

    start_scannable_legacy_ext();

    start_legacy_duration(0, true);

    start_ext_max_events(0, true);
}

/*
 * main
 *
 * The main task for the project. This function initializes the packages,
 * then starts serving events from default event queue.
 *
 * @return int NOTE: this function should never return!
 */
int
main(void)
{
    /* Initialize OS */
    sysinit();

    console_printf("Extended Advertising sample application\n");

    /* Set sync callback */
    ble_hs_cfg.sync_cb = on_sync;

    /* As the last thing, process events from default event queue */
    while (1) {
        os_eventq_run(os_eventq_dflt_get());
    }
    return 0;
}
