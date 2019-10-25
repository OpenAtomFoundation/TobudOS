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

#include <string.h>
#include <errno.h>
#include "testutil/testutil.h"
#include "nimble/ble.h"
#include "host/ble_uuid.h"
#include "ble_hs_test.h"
#include "ble_hs_test_util.h"

#define BLE_GATTS_REG_TEST_MAX_ENTRIES  256

struct ble_gatts_reg_test_entry {
    uint8_t op;
    ble_uuid_any_t uuid;
    uint16_t handle;
    uint16_t val_handle; /* If a characteristic. */

    const struct ble_gatt_svc_def *svc;
    const struct ble_gatt_chr_def *chr;
    const struct ble_gatt_dsc_def *dsc;
};

static struct ble_gatts_reg_test_entry
ble_gatts_reg_test_entries[BLE_GATTS_REG_TEST_MAX_ENTRIES];

static int ble_gatts_reg_test_num_entries;

static void
ble_gatts_reg_test_init(void)
{
    ble_hs_test_util_init();
    ble_gatts_reg_test_num_entries = 0;
}

static void
ble_gatts_reg_test_misc_reg_cb(struct ble_gatt_register_ctxt *ctxt, void *arg)
{
    struct ble_gatts_reg_test_entry *entry;

    TEST_ASSERT_FATAL(ble_gatts_reg_test_num_entries <
                      BLE_GATTS_REG_TEST_MAX_ENTRIES);

    entry = ble_gatts_reg_test_entries + ble_gatts_reg_test_num_entries++;
    memset(entry, 0, sizeof *entry);

    entry->op = ctxt->op;
    switch (ctxt->op) {
    case BLE_GATT_REGISTER_OP_SVC:
        ble_uuid_to_any(ctxt->svc.svc_def->uuid, &entry->uuid);
        entry->handle = ctxt->svc.handle;
        entry->svc = ctxt->svc.svc_def;
        break;

    case BLE_GATT_REGISTER_OP_CHR:
        ble_uuid_to_any(ctxt->chr.chr_def->uuid, &entry->uuid);
        entry->handle = ctxt->chr.def_handle;
        entry->val_handle = ctxt->chr.val_handle;
        entry->svc = ctxt->chr.svc_def;
        entry->chr = ctxt->chr.chr_def;
        break;

    case BLE_GATT_REGISTER_OP_DSC:
        ble_uuid_to_any(ctxt->dsc.dsc_def->uuid, &entry->uuid);
        entry->handle = ctxt->dsc.handle;
        entry->svc = ctxt->dsc.svc_def;
        entry->chr = ctxt->dsc.chr_def;
        entry->dsc = ctxt->dsc.dsc_def;
        break;

    default:
        TEST_ASSERT(0);
        break;
    }
}

static void
ble_gatts_reg_test_misc_lookup_good(struct ble_gatts_reg_test_entry *entry)
{
    uint16_t chr_def_handle;
    uint16_t chr_val_handle;
    uint16_t svc_handle;
    uint16_t dsc_handle;
    int rc;

    switch (entry->op) {
    case BLE_GATT_REGISTER_OP_SVC:
        rc = ble_gatts_find_svc(&entry->uuid.u, &svc_handle);
        TEST_ASSERT_FATAL(rc == 0);
        TEST_ASSERT(svc_handle == entry->handle);
        break;

    case BLE_GATT_REGISTER_OP_CHR:
        rc = ble_gatts_find_chr(entry->svc->uuid, entry->chr->uuid,
                                &chr_def_handle, &chr_val_handle);
        TEST_ASSERT_FATAL(rc == 0);
        TEST_ASSERT(chr_def_handle == entry->handle);
        TEST_ASSERT(chr_val_handle == entry->val_handle);
        break;

    case BLE_GATT_REGISTER_OP_DSC:
        rc = ble_gatts_find_dsc(entry->svc->uuid, entry->chr->uuid,
                                entry->dsc->uuid, &dsc_handle);
        break;

    default:
        TEST_ASSERT(0);
        break;
    }
}

static void
ble_gatts_reg_test_misc_lookup_bad(struct ble_gatts_reg_test_entry *entry)
{
    struct ble_gatts_reg_test_entry *cur;
    ble_uuid_any_t wrong_uuid;
    int rc;
    int i;

    switch (entry->op) {
    case BLE_GATT_REGISTER_OP_SVC:
        /* Wrong service UUID. */
        ble_uuid_to_any(entry->svc->uuid, &wrong_uuid);
        wrong_uuid.u16.value++;
        rc = ble_gatts_find_svc(&wrong_uuid.u, NULL);
        TEST_ASSERT(rc == BLE_HS_ENOENT);
        break;

    case BLE_GATT_REGISTER_OP_CHR:
        /* Correct service UUID, wrong characteristic UUID. */
        ble_uuid_to_any(entry->chr->uuid, &wrong_uuid);
        wrong_uuid.u16.value++;
        rc = ble_gatts_find_chr(entry->svc->uuid, &wrong_uuid.u, NULL, NULL);
        TEST_ASSERT(rc == BLE_HS_ENOENT);

        /* Incorrect service UUID, correct characteristic UUID. */
        ble_uuid_to_any(entry->svc->uuid, &wrong_uuid);
        wrong_uuid.u16.value++;
        rc = ble_gatts_find_chr(&wrong_uuid.u, entry->chr->uuid, NULL, NULL);
        TEST_ASSERT(rc == BLE_HS_ENOENT);

        /* Existing (but wrong) service, correct characteristic UUID. */
        for (i = 0; i < ble_gatts_reg_test_num_entries; i++) {
            cur = ble_gatts_reg_test_entries + i;
            switch (cur->op) {
            case BLE_GATT_REGISTER_OP_SVC:
                if (cur->svc != entry->svc) {
                    rc = ble_gatts_find_chr(cur->svc->uuid,
                                            entry->chr->uuid,
                                            NULL, NULL);
                    TEST_ASSERT(rc == BLE_HS_ENOENT);
                }
                break;

            case BLE_GATT_REGISTER_OP_CHR:
                /* Characteristic that isn't in this service. */
                if (cur->svc != entry->svc) {
                    rc = ble_gatts_find_chr(entry->svc->uuid,
                                            cur->chr->uuid,
                                            NULL, NULL);
                    TEST_ASSERT(rc == BLE_HS_ENOENT);
                }
                break;

            case BLE_GATT_REGISTER_OP_DSC:
                /* Use descriptor UUID instead of characteristic UUID. */
                rc = ble_gatts_find_chr(entry->svc->uuid,
                                        cur->dsc->uuid,
                                        NULL, NULL);
                TEST_ASSERT(rc == BLE_HS_ENOENT);
                break;

            default:
                TEST_ASSERT(0);
                break;
            }
        }
        break;

    case BLE_GATT_REGISTER_OP_DSC:
        /* Correct svc/chr UUID, wrong dsc UUID. */
        ble_uuid_to_any(entry->dsc->uuid, &wrong_uuid);
        wrong_uuid.u128.value[15]++;
        rc = ble_gatts_find_dsc(entry->svc->uuid, entry->chr->uuid,
                                &wrong_uuid.u, NULL);
        TEST_ASSERT(rc == BLE_HS_ENOENT);

        /* Incorrect svc UUID, correct chr/dsc UUID. */
        ble_uuid_to_any(entry->svc->uuid, &wrong_uuid);
        wrong_uuid.u128.value[15]++;
        rc = ble_gatts_find_dsc(&wrong_uuid.u, entry->chr->uuid,
                                entry->dsc->uuid, NULL);
        TEST_ASSERT(rc == BLE_HS_ENOENT);

        for (i = 0; i < ble_gatts_reg_test_num_entries; i++) {
            cur = ble_gatts_reg_test_entries + i;
            switch (cur->op) {
            case BLE_GATT_REGISTER_OP_SVC:
                /* Existing (but wrong) svc, correct chr/dsc UUID. */
                if (cur->svc != entry->svc) {
                    rc = ble_gatts_find_dsc(cur->svc->uuid,
                                            entry->chr->uuid,
                                            entry->dsc->uuid,
                                            NULL);
                    TEST_ASSERT(rc == BLE_HS_ENOENT);
                }
                break;

            case BLE_GATT_REGISTER_OP_CHR:
                /* Existing (but wrong) svc/chr, correct dsc UUID. */
                if (cur->chr != entry->chr) {
                    rc = ble_gatts_find_dsc(cur->svc->uuid,
                                            cur->chr->uuid,
                                            entry->dsc->uuid,
                                            NULL);
                    TEST_ASSERT(rc == BLE_HS_ENOENT);
                }
                break;

            case BLE_GATT_REGISTER_OP_DSC:
                /* Descriptor that isn't in this characteristic. */
                if (cur->chr != entry->chr) {
                    rc = ble_gatts_find_dsc(cur->svc->uuid,
                                            cur->chr->uuid,
                                            entry->dsc->uuid,
                                            NULL);
                    TEST_ASSERT(rc == BLE_HS_ENOENT);
                }
                break;

            default:
                TEST_ASSERT(0);
                break;
            }
        }
        break;

    default:
        TEST_ASSERT(0);
        break;
    }
}

static void
ble_gatts_reg_test_misc_verify_entry(uint8_t op, const ble_uuid_t *uuid)
{
    struct ble_gatts_reg_test_entry *entry;
    int i;

    for (i = 0; i < ble_gatts_reg_test_num_entries; i++) {
        entry = ble_gatts_reg_test_entries + i;
        if (entry->op == op && ble_uuid_cmp(&entry->uuid.u, uuid) == 0) {
            break;
        }
    }
    TEST_ASSERT_FATAL(entry != NULL);

    /* Verify that characteristic value handle was properly assigned at
     * registration.
     */
    if (op == BLE_GATT_REGISTER_OP_CHR) {
        TEST_ASSERT(*entry->chr->val_handle == entry->val_handle);
    }

    /* Verify that the entry can be looked up. */
    ble_gatts_reg_test_misc_lookup_good(entry);

    /* Verify that "barely incorrect" UUID information doesn't retrieve any
     * handles.
     */
    ble_gatts_reg_test_misc_lookup_bad(entry);
}

static int
ble_gatts_reg_test_misc_dummy_access(uint16_t conn_handle,
                                     uint16_t attr_handle,
                                     struct ble_gatt_access_ctxt *ctxt,
                                     void *arg)
{
    return 0;
}

TEST_CASE_SELF(ble_gatts_reg_test_svc_return)
{
    int rc;

    /*** Missing UUID. */
    ble_gatts_reg_test_init();
    struct ble_gatt_svc_def svcs_no_uuid[] = { {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
    }, {
        0
    } };

    rc = ble_gatts_register_svcs(svcs_no_uuid, NULL, NULL);
    TEST_ASSERT(rc == BLE_HS_EINVAL);

    /*** Circular dependency. */
    ble_gatts_reg_test_init();
    struct ble_gatt_svc_def svcs_circ[] = { {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(0x1234),
        .includes = (const struct ble_gatt_svc_def*[]) { svcs_circ + 1, NULL },
    }, {
        .type = BLE_GATT_SVC_TYPE_SECONDARY,
        .uuid = BLE_UUID16_DECLARE(0x1234),
        .includes = (const struct ble_gatt_svc_def*[]) { svcs_circ + 0, NULL },
    }, {
        0
    } };

    rc = ble_gatts_register_svcs(svcs_circ, NULL, NULL);
    TEST_ASSERT(rc == BLE_HS_EINVAL);

    /*** Success. */
    ble_gatts_reg_test_init();
    struct ble_gatt_svc_def svcs_good[] = { {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(0x1234),
        .includes = (const struct ble_gatt_svc_def*[]) { svcs_good + 1, NULL },
    }, {
        .type = BLE_GATT_SVC_TYPE_SECONDARY,
        .uuid = BLE_UUID16_DECLARE(0x1234),
    }, {
        0
    } };

    rc = ble_gatts_register_svcs(svcs_good, NULL, NULL);
    TEST_ASSERT(rc == 0);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_gatts_reg_test_chr_return)
{
    int rc;

    /*** Missing callback. */
    ble_gatts_reg_test_init();
    struct ble_gatt_svc_def svcs_no_chr_cb[] = { {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(0x1234),
        .characteristics = (struct ble_gatt_chr_def[]) { {
            .uuid = BLE_UUID16_DECLARE(0x1111),
            .flags = BLE_GATT_CHR_F_READ,
        }, {
            0
        } },
    }, {
        0
    } };

    rc = ble_gatts_register_svcs(svcs_no_chr_cb, NULL, NULL);
    TEST_ASSERT(rc == BLE_HS_EINVAL);

    /*** Success. */
    ble_gatts_reg_test_init();
    struct ble_gatt_svc_def svcs_good[] = { {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(0x1234),
        .characteristics = (struct ble_gatt_chr_def[]) { {
            .uuid = BLE_UUID16_DECLARE(0x1111),
            .access_cb = ble_gatts_reg_test_misc_dummy_access,
            .flags = BLE_GATT_CHR_F_READ,
        }, {
            0
        } },
    }, {
        0
    } };

    rc = ble_gatts_register_svcs(svcs_good, NULL, NULL);
    TEST_ASSERT(rc == 0);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_gatts_reg_test_dsc_return)
{
    int rc;

    /*** Missing callback. */
    ble_gatts_reg_test_init();
    struct ble_gatt_svc_def svcs_no_dsc_cb[] = { {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(0x1234),
        .characteristics = (struct ble_gatt_chr_def[]) { {
            .uuid = BLE_UUID16_DECLARE(0x1111),
            .access_cb = ble_gatts_reg_test_misc_dummy_access,
            .flags = BLE_GATT_CHR_F_READ,
            .descriptors = (struct ble_gatt_dsc_def[]) { {
                .uuid = BLE_UUID16_DECLARE(0x8888),
                .att_flags = 5,
            }, {
                0
            } },
        }, {
            0
        } },
    }, {
        0
    } };

    rc = ble_gatts_register_svcs(svcs_no_dsc_cb, NULL, NULL);
    TEST_ASSERT(rc == BLE_HS_EINVAL);

    /*** Success. */
    ble_gatts_reg_test_init();
    struct ble_gatt_svc_def svcs_good[] = { {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(0x1234),
        .characteristics = (struct ble_gatt_chr_def[]) { {
            .uuid = BLE_UUID16_DECLARE(0x1111),
            .access_cb = ble_gatts_reg_test_misc_dummy_access,
            .flags = BLE_GATT_CHR_F_READ,
            .descriptors = (struct ble_gatt_dsc_def[]) { {
                .uuid = BLE_UUID16_DECLARE(0x8888),
                .access_cb = ble_gatts_reg_test_misc_dummy_access,
                .att_flags = 5,
            }, {
                0
            } },
        }, {
            0
        } },
    }, {
        0
    } };

    rc = ble_gatts_register_svcs(svcs_good, NULL, NULL);
    TEST_ASSERT(rc == 0);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

static void
ble_gatts_reg_test_misc_svcs(struct ble_gatt_svc_def *svcs)
{
    const struct ble_gatt_svc_def *svc;
    const struct ble_gatt_chr_def *chr;
    const struct ble_gatt_dsc_def *dsc;
    int rc;

    ble_gatts_reg_test_init();

    /* Register all the attributes. */
    rc = ble_gatts_register_svcs(svcs, ble_gatts_reg_test_misc_reg_cb,
                                 NULL);
    TEST_ASSERT_FATAL(rc == 0);

    /* Verify that the appropriate callbacks were executed. */
    for (svc = svcs; svc->type != BLE_GATT_SVC_TYPE_END; svc++) {
        ble_gatts_reg_test_misc_verify_entry(BLE_GATT_REGISTER_OP_SVC,
                                             svc->uuid);

        if (svc->characteristics != NULL) {
            for (chr = svc->characteristics; chr->uuid != NULL; chr++) {
                ble_gatts_reg_test_misc_verify_entry(BLE_GATT_REGISTER_OP_CHR,
                                                     chr->uuid);

                if (chr->descriptors != NULL) {
                    for (dsc = chr->descriptors; dsc->uuid != NULL; dsc++) {
                        ble_gatts_reg_test_misc_verify_entry(
                            BLE_GATT_REGISTER_OP_DSC, dsc->uuid);
                    }
                }
            }
        }
    }
}

TEST_CASE_SELF(ble_gatts_reg_test_svc_cb)
{
    /*** 1 primary. */
    ble_gatts_reg_test_misc_svcs((struct ble_gatt_svc_def[]) { {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(0x1234),
    }, {
        0
    } });

    /*** 3 primary. */
    ble_gatts_reg_test_misc_svcs((struct ble_gatt_svc_def[]) { {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(0x1234),
    }, {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(0x2234),
    }, {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(0x3234),
    }, {
        0
    } });

    /*** 1 primary, 1 secondary. */
    ble_gatts_reg_test_misc_svcs((struct ble_gatt_svc_def[]) { {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(0x1234),
    }, {
        .type = BLE_GATT_SVC_TYPE_SECONDARY,
        .uuid = BLE_UUID16_DECLARE(0x2222),
    }, {
        0
    } });

    /*** 1 primary, 1 secondary, 1 include. */
    struct ble_gatt_svc_def svcs[] = {
        [0] = {
            .type = BLE_GATT_SVC_TYPE_PRIMARY,
            .uuid = BLE_UUID16_DECLARE(0x1234),
            .includes = (const struct ble_gatt_svc_def*[]) { svcs + 1, NULL, },
        },
        [1] = {
            .type = BLE_GATT_SVC_TYPE_SECONDARY,
            .uuid = BLE_UUID16_DECLARE(0x2222),
        }, {
            0
        }
    };
    ble_gatts_reg_test_misc_svcs(svcs);

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_gatts_reg_test_chr_cb)
{
    uint16_t val_handles[16];

    /*** 1 characteristic. */
    ble_gatts_reg_test_misc_svcs((struct ble_gatt_svc_def[]) { {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(0x1234),
        .characteristics = (struct ble_gatt_chr_def[]) { {
            .uuid = BLE_UUID16_DECLARE(0x1111),
            .access_cb = ble_gatts_reg_test_misc_dummy_access,
            .flags = BLE_GATT_CHR_F_READ,
            .val_handle = val_handles + 0,
        }, {
            0
        } },
    }, {
        0
    } });

    /*** 3 characteristics. */
    ble_gatts_reg_test_misc_svcs((struct ble_gatt_svc_def[]) { {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(0x1234),
        .characteristics = (struct ble_gatt_chr_def[]) { {
            .uuid = BLE_UUID16_DECLARE(0x1111),
            .access_cb = ble_gatts_reg_test_misc_dummy_access,
            .flags = BLE_GATT_CHR_F_READ,
            .val_handle = val_handles + 0,
        }, {
            .uuid = BLE_UUID16_DECLARE(0x2222),
            .access_cb = ble_gatts_reg_test_misc_dummy_access,
            .flags = BLE_GATT_CHR_F_WRITE,
            .val_handle = val_handles + 1,
        }, {
            0
        } },
    }, {
        .type = BLE_GATT_SVC_TYPE_SECONDARY,
        .uuid = BLE_UUID16_DECLARE(0x5678),
        .characteristics = (struct ble_gatt_chr_def[]) { {
            .uuid = BLE_UUID16_DECLARE(0x3333),
            .access_cb = ble_gatts_reg_test_misc_dummy_access,
            .flags = BLE_GATT_CHR_F_READ,
            .val_handle = val_handles + 2,
        }, {
            0
        } },
    }, {
        0
    } });

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_CASE_SELF(ble_gatts_reg_test_dsc_cb)
{
    uint16_t val_handles[16];

    /*** 1 descriptor. */
    ble_gatts_reg_test_misc_svcs((struct ble_gatt_svc_def[]) { {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(0x1234),
        .characteristics = (struct ble_gatt_chr_def[]) { {
            .uuid = BLE_UUID16_DECLARE(0x1111),
            .access_cb = ble_gatts_reg_test_misc_dummy_access,
            .flags = BLE_GATT_CHR_F_READ,
            .val_handle = val_handles + 0,
            .descriptors = (struct ble_gatt_dsc_def[]) { {
                .uuid = BLE_UUID16_DECLARE(0x111a),
                .att_flags = 5,
                .access_cb = ble_gatts_reg_test_misc_dummy_access,
            }, {
                0
            } },
        }, {
            0
        } },
    }, {
        0
    } });

    /*** 5+ descriptors. */
    ble_gatts_reg_test_misc_svcs((struct ble_gatt_svc_def[]) { {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(0x1234),
        .characteristics = (struct ble_gatt_chr_def[]) { {
            .uuid = BLE_UUID16_DECLARE(0x1111),
            .access_cb = ble_gatts_reg_test_misc_dummy_access,
            .flags = BLE_GATT_CHR_F_READ,
            .val_handle = val_handles + 0,
            .descriptors = (struct ble_gatt_dsc_def[]) { {
                .uuid = BLE_UUID16_DECLARE(0x111a),
                .att_flags = 5,
                .access_cb = ble_gatts_reg_test_misc_dummy_access,
            }, {
                0
            } },
        }, {
            .uuid = BLE_UUID16_DECLARE(0x2222),
            .access_cb = ble_gatts_reg_test_misc_dummy_access,
            .flags = BLE_GATT_CHR_F_WRITE,
            .val_handle = val_handles + 1,
        }, {
            0
        } },
    }, {
        .type = BLE_GATT_SVC_TYPE_SECONDARY,
        .uuid = BLE_UUID16_DECLARE(0x5678),
        .characteristics = (struct ble_gatt_chr_def[]) { {
            .uuid = BLE_UUID16_DECLARE(0x3333),
            .access_cb = ble_gatts_reg_test_misc_dummy_access,
            .flags = BLE_GATT_CHR_F_READ,
            .val_handle = val_handles + 2,
            .descriptors = (struct ble_gatt_dsc_def[]) { {
                .uuid = BLE_UUID16_DECLARE(0x333a),
                .att_flags = 5,
                .access_cb = ble_gatts_reg_test_misc_dummy_access,
            }, {
                .uuid = BLE_UUID16_DECLARE(0x333b),
                .att_flags = 5,
                .access_cb = ble_gatts_reg_test_misc_dummy_access,
            }, {
                .uuid = BLE_UUID16_DECLARE(0x333c),
                .att_flags = 5,
                .access_cb = ble_gatts_reg_test_misc_dummy_access,
            }, {
                .uuid = BLE_UUID16_DECLARE(0x333e),
                .att_flags = 5,
                .access_cb = ble_gatts_reg_test_misc_dummy_access,
            }, {
                0
            } },
        }, {
            .uuid = BLE_UUID16_DECLARE(0x4444),
            .access_cb = ble_gatts_reg_test_misc_dummy_access,
            .flags = BLE_GATT_CHR_F_READ,
            .val_handle = val_handles + 3,
            .descriptors = (struct ble_gatt_dsc_def[]) { {
                .uuid = BLE_UUID16_DECLARE(0x444a),
                .att_flags = 5,
                .access_cb = ble_gatts_reg_test_misc_dummy_access,
            }, {
                .uuid = BLE_UUID16_DECLARE(0x444b),
                .att_flags = 5,
                .access_cb = ble_gatts_reg_test_misc_dummy_access,
            }, {
                .uuid = BLE_UUID16_DECLARE(0x444c),
                .att_flags = 5,
                .access_cb = ble_gatts_reg_test_misc_dummy_access,
            }, {
                .uuid = BLE_UUID16_DECLARE(0x444e),
                .att_flags = 5,
                .access_cb = ble_gatts_reg_test_misc_dummy_access,
            }, {
                0
            } },
        }, {
            0
        } },
    }, {
        0
    } });

    ble_hs_test_util_assert_mbufs_freed(NULL);
}

TEST_SUITE(ble_gatts_reg_suite)
{
    ble_gatts_reg_test_svc_return();
    ble_gatts_reg_test_chr_return();
    ble_gatts_reg_test_dsc_return();

    ble_gatts_reg_test_svc_cb();
    ble_gatts_reg_test_chr_cb();
    ble_gatts_reg_test_dsc_cb();
}
