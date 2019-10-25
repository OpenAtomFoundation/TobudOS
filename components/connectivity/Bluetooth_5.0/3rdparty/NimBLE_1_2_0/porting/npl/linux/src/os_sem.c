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
#include <errno.h>
#include <semaphore.h>

#include "os/os.h"
#include "nimble/nimble_npl.h"

ble_npl_error_t
ble_npl_sem_init(struct ble_npl_sem *sem, uint16_t tokens)
{
    if (!sem) {
        return BLE_NPL_INVALID_PARAM;
    }

    sem_init(&sem->lock, 0, tokens);

    return BLE_NPL_OK;
}

ble_npl_error_t
ble_npl_sem_release(struct ble_npl_sem *sem)
{
    int err;

    if (!sem) {
        return BLE_NPL_INVALID_PARAM;
    }

    err = sem_post(&sem->lock);

    return (err) ? BLE_NPL_ERROR : BLE_NPL_OK;
}

ble_npl_error_t
ble_npl_sem_pend(struct ble_npl_sem *sem, uint32_t timeout)
{
    int err = 0;
    struct timespec wait;

    if (!sem) {
        return BLE_NPL_INVALID_PARAM;
    }

    if (timeout == BLE_NPL_WAIT_FOREVER) {
        err = sem_wait(&sem->lock);
    } else {
        err = clock_gettime(CLOCK_REALTIME, &wait);
        if (err) {
            return BLE_NPL_ERROR;
        }

        wait.tv_sec  += timeout / 1000;
        wait.tv_nsec += (timeout % 1000) * 1000000;

        err = sem_timedwait(&sem->lock, &wait);
        if (err && errno == ETIMEDOUT) {
            return BLE_NPL_TIMEOUT;
        }
    }

    return (err) ? BLE_NPL_ERROR : BLE_NPL_OK;
}

uint16_t
ble_npl_sem_get_count(struct ble_npl_sem *sem)
{
    int count;

    assert(sem);
    assert(&sem->lock);
    sem_getvalue(&sem->lock, &count);

    return count;
}
