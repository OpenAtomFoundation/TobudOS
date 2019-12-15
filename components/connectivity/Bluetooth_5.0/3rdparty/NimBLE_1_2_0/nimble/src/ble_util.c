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

#include "os/os.h"
#include "nimble/ble.h"

/**
 * Converts an OS error code to its equivalent BLE_ERR code.
 *
 * @param os_err                The OS error code to convert.
 *
 * @return                      The equivalent BLE_ERR code.
 */
int
ble_err_from_os(int os_err)
{
    switch (os_err) {
    case 0:
        return 0;

    case OS_ENOMEM:
        return BLE_ERR_MEM_CAPACITY;

    default:
        return BLE_ERR_UNSPECIFIED;
    }
}
