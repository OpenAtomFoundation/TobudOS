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

#include "nrfx.h"

#if NIMBLE_CFG_CONTROLLER

static void (*radio_isr_addr)(void);
static void (*rng_isr_addr)(void);
static void (*rtc0_isr_addr)(void);

void RADIO_IRQHandler(void)
{
    radio_isr_addr();
}

void RNG_IRQHandler(void)
{
    rng_isr_addr();
}

void RTC0_IRQHandler(void)
{
    rtc0_isr_addr();
}

void
ble_npl_hw_set_isr(int irqn, void (*addr)(void))
{
    switch (irqn) {
    case RADIO_IRQn:
        radio_isr_addr = addr;
        break;

    case RNG_IRQn:
        rng_isr_addr = addr;
        break;

    case RTC0_IRQn:
        rtc0_isr_addr = addr;
        break;
    }
}

#endif

