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

#include <errno.h>
#include <pthread.h>

#include "os/os.h"
#include "nimble/nimble_npl.h"

ble_npl_error_t
ble_npl_mutex_init(struct ble_npl_mutex *mu)
{
    if (!mu) {
        return BLE_NPL_INVALID_PARAM;
    }

    pthread_mutexattr_init(&mu->attr);
    pthread_mutexattr_settype(&mu->attr, PTHREAD_MUTEX_RECURSIVE_NP);
    pthread_mutex_init(&mu->lock, &mu->attr);

    return BLE_NPL_OK;
}

ble_npl_error_t
ble_npl_mutex_release(struct ble_npl_mutex *mu)
{
    if (!mu) {
        return BLE_NPL_INVALID_PARAM;
    }

    if (pthread_mutex_unlock(&mu->lock)) {
        return BLE_NPL_BAD_MUTEX;
    }

    return BLE_NPL_OK;
}

ble_npl_error_t
ble_npl_mutex_pend(struct ble_npl_mutex *mu, uint32_t timeout)
{
    int err;

    if (!mu) {
        return BLE_NPL_INVALID_PARAM;
    }

    if (timeout == BLE_NPL_WAIT_FOREVER) {
        err = pthread_mutex_lock(&mu->lock);
    } else {
        err = clock_gettime(CLOCK_REALTIME, &mu->wait);
        if (err) {
            return BLE_NPL_ERROR;
        }

        mu->wait.tv_sec  += timeout / 1000;
        mu->wait.tv_nsec += (timeout % 1000) * 1000000;

        err = pthread_mutex_timedlock(&mu->lock, &mu->wait);
        if (err == ETIMEDOUT) {
            return BLE_NPL_TIMEOUT;
        }
    }

    return (err) ? BLE_NPL_ERROR : BLE_NPL_OK;
}
