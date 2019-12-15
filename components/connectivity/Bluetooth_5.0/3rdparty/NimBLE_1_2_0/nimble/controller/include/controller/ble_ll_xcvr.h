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

#ifndef H_BLE_LL_XCVR_
#define H_BLE_LL_XCVR_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef BLE_XCVR_RFCLK

/* RF clock states */
#define BLE_RFCLK_STATE_OFF     (0)
#define BLE_RFCLK_STATE_ON      (1)
#define BLE_RFCLK_STATE_SETTLED (2)

int ble_ll_xcvr_rfclk_state(void);
void ble_ll_xcvr_rfclk_start_now(uint32_t now);
void ble_ll_xcvr_rfclk_stop(void);
void ble_ll_xcvr_rfclk_enable(void);
void ble_ll_xcvr_rfclk_disable(void);
uint32_t ble_ll_xcvr_rfclk_time_till_settled(void);
void ble_ll_xcvr_rfclk_timer_exp(void *arg);
void ble_ll_xcvr_rfclk_timer_start(uint32_t cputime);
#endif

#ifdef __cplusplus
}
#endif

#endif /* H_LL_ */
