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

#include "testutil/testutil.h"
#include "ble_hs_test.h"
#include "ble_hs_test_util.h"

static struct ble_store_status_event ble_store_test_status_event;

static void
ble_store_test_util_verify_peer_deleted(const ble_addr_t *addr)
{
    union ble_store_value value;
    union ble_store_key key;
    ble_addr_t addrs[64];
    int num_addrs;
    int rc;
    int i;

    memset(&key, 0, sizeof key);
    key.sec.peer_addr = *addr;
    rc = ble_store_read(BLE_STORE_OBJ_TYPE_OUR_SEC, &key, &value);
    TEST_ASSERT(rc == BLE_HS_ENOENT);
    rc = ble_store_read(BLE_STORE_OBJ_TYPE_PEER_SEC, &key, &value);
    TEST_ASSERT(rc == BLE_HS_ENOENT);

    memset(&key, 0, sizeof key);
    key.cccd.peer_addr = *addr;
    rc = ble_store_read(BLE_STORE_OBJ_TYPE_CCCD, &key, &value);
    TEST_ASSERT(rc == BLE_HS_ENOENT);

    rc = ble_store_util_bonded_peers(addrs, &num_addrs,
                                     sizeof addrs / sizeof addrs[0]);
    TEST_ASSERT_FATAL(rc == 0);
    for (i = 0; i < num_addrs; i++) {
        TEST_ASSERT(ble_addr_cmp(addr, addrs + i) != 0);
    }
}

static int
ble_store_test_util_status_overflow(struct ble_store_status_event *event,
                                    void *arg)
{
    int *status;

    status = arg;

    ble_store_test_status_event = *event;
    return *status;
}

static void
ble_store_test_util_overflow_sec(int is_our_sec)
{
    union ble_store_value val;
    int obj_type;
    int status;
    int rc;
    int i;

    ble_hs_test_util_init();

    ble_hs_cfg.store_status_cb = ble_store_test_util_status_overflow;
    ble_hs_cfg.store_status_arg = &status;

    if (is_our_sec) {
        obj_type = BLE_STORE_OBJ_TYPE_OUR_SEC;
    } else {
        obj_type = BLE_STORE_OBJ_TYPE_PEER_SEC;
    }

    memset(&ble_store_test_status_event, 0,
           sizeof ble_store_test_status_event);
    memset(&val, 0, sizeof val);

    val.sec.peer_addr =
        (ble_addr_t){ BLE_ADDR_PUBLIC, { 1, 2, 3, 4, 5, 6 } };
    val.sec.ltk_present = 1,

    status = BLE_HS_ESTORE_CAP;
    for (i = 0; ; i++) {
        rc = ble_store_write(obj_type, &val);
        if (i < MYNEWT_VAL(BLE_STORE_MAX_BONDS)) {
            TEST_ASSERT_FATAL(rc == 0);
        } else {
            /* This record should have caused an overflow. */
            TEST_ASSERT(rc == BLE_HS_ESTORE_CAP);
            TEST_ASSERT(ble_store_test_status_event.event_code ==
                        BLE_STORE_EVENT_OVERFLOW);
            TEST_ASSERT(ble_store_test_status_event.overflow.obj_type ==
                        obj_type);
            TEST_ASSERT(ble_store_test_status_event.overflow.value == &val);
            break;
        }

        val.sec.peer_addr.val[0]++;
    }
}

static int
ble_store_test_util_count(int obj_type)
{
    int count;
    int rc;

    rc = ble_store_util_count(obj_type, &count);
    TEST_ASSERT_FATAL(rc == 0);

    return count;
}

TEST_CASE_SELF(ble_store_test_peers)
{
    struct ble_store_value_sec secs[3] = {
        {
            .peer_addr = { BLE_ADDR_PUBLIC,     { 1, 2, 3, 4, 5, 6 } },
            .ltk_present = 1,
        },
        {
            /* Address value is a duplicate of above, but type differs. */
            .peer_addr = { BLE_ADDR_RANDOM,     { 1, 2, 3, 4, 5, 6 } },
            .ltk_present = 1,
        },
        {
            .peer_addr = { BLE_ADDR_PUBLIC,     { 2, 3, 4, 5, 6, 7 } },
            .ltk_present = 1,
        },
    };
    ble_addr_t peer_addrs[3];
    int num_addrs;
    int rc;
    int i;

    ble_hs_test_util_init();

    for (i = 0; i < sizeof secs / sizeof secs[0]; i++) {
        rc = ble_store_write_our_sec(secs + i);
        TEST_ASSERT_FATAL(rc == 0);
        rc = ble_store_write_peer_sec(secs + i);
        TEST_ASSERT_FATAL(rc == 0);
    }

    rc = ble_store_util_bonded_peers(peer_addrs, &num_addrs,
                                     sizeof peer_addrs / sizeof peer_addrs[0]);
    TEST_ASSERT_FATAL(rc == 0);

    TEST_ASSERT(num_addrs == sizeof secs / sizeof secs[0]);
    for (i = 0; i < num_addrs; i++) {
        TEST_ASSERT(ble_addr_cmp(&peer_addrs[i], &secs[i].peer_addr) == 0);
    }

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_store_test_delete_peer)
{
    struct ble_store_value_sec secs[2] = {
        {
            .peer_addr = { BLE_ADDR_PUBLIC,     { 1, 2, 3, 4, 5, 6 } },
            .ltk_present = 1,
        },
        {
            /* Address value is a duplicate of above, but type differs. */
            .peer_addr = { BLE_ADDR_RANDOM,     { 1, 2, 3, 4, 5, 6 } },
            .ltk_present = 1,
        },
    };
    struct ble_store_value_cccd cccds[3] = {
        /* First two belong to first peer. */
        {
            .peer_addr = secs[0].peer_addr,
            .chr_val_handle = 5,
        },
        {
            .peer_addr = secs[0].peer_addr,
            .chr_val_handle = 8,
        },

        /* Last belongs to second peer. */
        {
            .peer_addr = secs[1].peer_addr,
            .chr_val_handle = 5,
        },
    };
    union ble_store_value value;
    union ble_store_key key;
    int count;
    int rc;
    int i;

    ble_hs_test_util_init();

    for (i = 0; i < sizeof secs / sizeof secs[0]; i++) {
        rc = ble_store_write_our_sec(secs + i);
        TEST_ASSERT_FATAL(rc == 0);
        rc = ble_store_write_peer_sec(secs + i);
        TEST_ASSERT_FATAL(rc == 0);
    }

    for (i = 0; i < sizeof cccds / sizeof cccds[0]; i++) {
        rc = ble_store_write_cccd(cccds + i);
        TEST_ASSERT_FATAL(rc == 0);
    }

    /* Delete first peer. */
    rc = ble_store_util_delete_peer(&secs[0].peer_addr);
    TEST_ASSERT_FATAL(rc == 0);

    /* Ensure all traces of first peer have been removed. */
    ble_store_test_util_verify_peer_deleted(&secs[0].peer_addr);

    /* Ensure second peer data is still intact. */
    ble_store_key_from_value_sec(&key.sec, secs + 1);

    rc = ble_store_util_count(BLE_STORE_OBJ_TYPE_OUR_SEC, &count);
    TEST_ASSERT_FATAL(rc == 0);
    TEST_ASSERT(count == 1);

    rc = ble_store_read_our_sec(&key.sec, &value.sec);
    TEST_ASSERT_FATAL(rc == 0);
    TEST_ASSERT(memcmp(&value.sec, secs + 1, sizeof value.sec) == 0);

    rc = ble_store_util_count(BLE_STORE_OBJ_TYPE_PEER_SEC, &count);
    TEST_ASSERT_FATAL(rc == 0);
    TEST_ASSERT(count == 1);

    rc = ble_store_read_peer_sec(&key.sec, &value.sec);
    TEST_ASSERT_FATAL(rc == 0);
    TEST_ASSERT(memcmp(&value.sec, secs + 1, sizeof value.sec) == 0);

    ble_store_key_from_value_cccd(&key.cccd, cccds + 2);

    rc = ble_store_util_count(BLE_STORE_OBJ_TYPE_CCCD, &count);
    TEST_ASSERT_FATAL(rc == 0);
    TEST_ASSERT(count == 1);

    rc = ble_store_read_cccd(&key.cccd, &value.cccd);
    TEST_ASSERT_FATAL(rc == 0);
    TEST_ASSERT(memcmp(&value.cccd, cccds + 2, sizeof value.cccd) == 0);

    /* Delete second peer. */
    rc = ble_store_util_delete_peer(&secs[1].peer_addr);
    TEST_ASSERT_FATAL(rc == 0);

    /* Ensure all traces of first peer have been removed. */
    ble_store_test_util_verify_peer_deleted(&secs[1].peer_addr);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_store_test_count)
{
    struct ble_store_value_sec secs[4] = {
        {
            .peer_addr = { BLE_ADDR_PUBLIC,     { 1, 2, 3, 4, 5, 6 } },
            .ltk_present = 1,
        },
        {
            .peer_addr = { BLE_ADDR_RANDOM,     { 1, 2, 3, 4, 5, 6 } },
            .ltk_present = 1,
        },
        {
            .peer_addr = { BLE_ADDR_PUBLIC,     { 2, 3, 4, 5, 6, 7 } },
            .ltk_present = 1,
        },
        {
            .peer_addr = { BLE_ADDR_RANDOM,     { 3, 4, 5, 6, 7, 8 } },
            .ltk_present = 1,
        },
    };
    struct ble_store_value_cccd cccds[2] = {
        {
            .peer_addr = secs[0].peer_addr,
            .chr_val_handle = 5,
        },
        {
            .peer_addr = secs[0].peer_addr,
            .chr_val_handle = 8,
        },
    };
    int count;
    int rc;
    int i;

    ble_hs_test_util_init();

    /*** Verify initial counts are 0. */

    rc = ble_store_util_count(BLE_STORE_OBJ_TYPE_OUR_SEC, &count);
    TEST_ASSERT_FATAL(rc == 0);
    TEST_ASSERT(count == 0);

    rc = ble_store_util_count(BLE_STORE_OBJ_TYPE_PEER_SEC, &count);
    TEST_ASSERT_FATAL(rc == 0);
    TEST_ASSERT(count == 0);

    rc = ble_store_util_count(BLE_STORE_OBJ_TYPE_CCCD, &count);
    TEST_ASSERT_FATAL(rc == 0);
    TEST_ASSERT(count == 0);

    /* Write some test data. */

    for (i = 0; i < 3; i++) {
        rc = ble_store_write_our_sec(secs + i);
        TEST_ASSERT_FATAL(rc == 0);
    }
    for (i = 0; i < 2; i++) {
        rc = ble_store_write_peer_sec(secs + i);
        TEST_ASSERT_FATAL(rc == 0);
    }
    for (i = 0; i < 1; i++) {
        rc = ble_store_write_cccd(cccds + i);
        TEST_ASSERT_FATAL(rc == 0);
    }

    /*** Verify counts after populating store. */
    rc = ble_store_util_count(BLE_STORE_OBJ_TYPE_OUR_SEC, &count);
    TEST_ASSERT_FATAL(rc == 0);
    TEST_ASSERT(count == 3);

    rc = ble_store_util_count(BLE_STORE_OBJ_TYPE_PEER_SEC, &count);
    TEST_ASSERT_FATAL(rc == 0);
    TEST_ASSERT(count == 2);

    rc = ble_store_util_count(BLE_STORE_OBJ_TYPE_CCCD, &count);
    TEST_ASSERT_FATAL(rc == 0);
    TEST_ASSERT(count == 1);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_store_test_overflow)
{
    ble_store_test_util_overflow_sec(0);
    ble_store_test_util_overflow_sec(1);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_store_test_clear)
{
    const struct ble_store_value_sec secs[2] = {
        {
            .peer_addr = { BLE_ADDR_PUBLIC,     { 1, 2, 3, 4, 5, 6 } },
            .ltk_present = 1,
        },
        {
            /* Address value is a duplicate of above, but type differs. */
            .peer_addr = { BLE_ADDR_RANDOM,     { 1, 2, 3, 4, 5, 6 } },
            .ltk_present = 1,
        },
    };
    const struct ble_store_value_cccd cccds[3] = {
        /* First two belong to first peer. */
        {
            .peer_addr = secs[0].peer_addr,
            .chr_val_handle = 5,
        },
        {
            .peer_addr = secs[0].peer_addr,
            .chr_val_handle = 8,
        },

        /* Last belongs to second peer. */
        {
            .peer_addr = secs[1].peer_addr,
            .chr_val_handle = 5,
        },
    };
    int rc;
    int i;

    ble_hs_test_util_init();

    for (i = 0; i < sizeof secs / sizeof secs[0]; i++) {
        rc = ble_store_write_our_sec(secs + i);
        TEST_ASSERT_FATAL(rc == 0);
        rc = ble_store_write_peer_sec(secs + i);
        TEST_ASSERT_FATAL(rc == 0);
    }

    for (i = 0; i < sizeof cccds / sizeof cccds[0]; i++) {
        rc = ble_store_write_cccd(cccds + i);
        TEST_ASSERT_FATAL(rc == 0);
    }

    /* Sanity check. */
    TEST_ASSERT_FATAL(
        ble_store_test_util_count(BLE_STORE_OBJ_TYPE_OUR_SEC) == 2);
    TEST_ASSERT_FATAL(
        ble_store_test_util_count(BLE_STORE_OBJ_TYPE_PEER_SEC) == 2);
    TEST_ASSERT_FATAL(
        ble_store_test_util_count(BLE_STORE_OBJ_TYPE_CCCD) == 3);

    /* Ensure store is empty after clear gets called. */
    rc = ble_store_clear();
    TEST_ASSERT_FATAL(rc == 0);
    TEST_ASSERT(ble_store_test_util_count(BLE_STORE_OBJ_TYPE_OUR_SEC) == 0);
    TEST_ASSERT(ble_store_test_util_count(BLE_STORE_OBJ_TYPE_PEER_SEC) == 0);
    TEST_ASSERT(ble_store_test_util_count(BLE_STORE_OBJ_TYPE_CCCD) == 0);

    /* Ensure second clear succeeds with no effect. */
    rc = ble_store_clear();
    TEST_ASSERT_FATAL(rc == 0);
    TEST_ASSERT(ble_store_test_util_count(BLE_STORE_OBJ_TYPE_OUR_SEC) == 0);
    TEST_ASSERT(ble_store_test_util_count(BLE_STORE_OBJ_TYPE_PEER_SEC) == 0);
    TEST_ASSERT(ble_store_test_util_count(BLE_STORE_OBJ_TYPE_CCCD) == 0);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_SUITE(ble_store_suite)
{
    ble_store_test_peers();
    ble_store_test_delete_peer();
    ble_store_test_count();
    ble_store_test_overflow();
    ble_store_test_clear();
}
