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

#include "os/mynewt.h"
#include "bsp/bsp.h"
#include "hal/hal_gpio.h"
#include "host/ble_gap.h"
#include "bleprph.h"

#if MYNEWT_VAL(BLEPRPH_LE_PHY_SUPPORT)

static const int button_gpio[4] = MYNEWT_VAL(BLEPRPH_LE_PHY_BUTTON_GPIO);
static const int led_gpio[3] = MYNEWT_VAL(BLEPRPH_LE_PHY_LED_GPIO);

#define PHY_TO_PTR(_mask, _opts) (void *)(((_opts) << 16) | ((_mask)))
#define PTR_TO_PHY_MASK(_ptr) (uint8_t)(((int)_ptr) & 0x0ff)
#define PTR_TO_PHY_OPTS(_ptr) (uint8_t)(((int)_ptr) >> 16)

static struct os_event gpio_event;

static uint16_t conn_handle = CONN_HANDLE_INVALID;

static void
gpio_irq_handler(void *arg)
{
    gpio_event.ev_arg = arg;
    os_eventq_put(os_eventq_dflt_get(), &gpio_event);
}

static void
gpio_event_handler(struct os_event *ev)
{
    uint8_t phy_mask;
    uint8_t phy_opts;
    int sr;

    OS_ENTER_CRITICAL(sr);
    phy_mask = PTR_TO_PHY_MASK(ev->ev_arg);
    phy_opts = PTR_TO_PHY_OPTS(ev->ev_arg);
    OS_EXIT_CRITICAL(sr);

    if (conn_handle != CONN_HANDLE_INVALID) {
        ble_gap_set_prefered_le_phy(conn_handle, phy_mask, phy_mask, phy_opts);
    }
}

static void
setup_button_gpio(int button, uint8_t phy_mask, uint8_t phy_opts)
{
    if (button < 0) {
        return;
    }

    hal_gpio_irq_init(button, gpio_irq_handler, PHY_TO_PTR(phy_mask, phy_opts),
                      HAL_GPIO_TRIG_FALLING, HAL_GPIO_PULL_UP);
    hal_gpio_irq_enable(button);
}

void
phy_init(void)
{
    gpio_event.ev_cb = gpio_event_handler;

    /*
     * XXX: we could make this configurable, but for now assume all pins are
     * valid, buttons gpio pins are pulled-up and LEDs are active-low - this
     * is valid for nRF52840 PDK.
     */
    setup_button_gpio(button_gpio[0], BLE_GAP_LE_PHY_1M_MASK,
                      BLE_GAP_LE_PHY_CODED_ANY);
    setup_button_gpio(button_gpio[1], BLE_GAP_LE_PHY_2M_MASK,
                      BLE_GAP_LE_PHY_CODED_ANY);
    setup_button_gpio(button_gpio[2], BLE_GAP_LE_PHY_CODED_MASK,
                      BLE_GAP_LE_PHY_CODED_S2);
    setup_button_gpio(button_gpio[3], BLE_GAP_LE_PHY_CODED_MASK,
                      BLE_GAP_LE_PHY_CODED_S8);

    hal_gpio_init_out(led_gpio[0], 1);
    hal_gpio_init_out(led_gpio[1], 1);
    hal_gpio_init_out(led_gpio[2], 1);
}

void
phy_conn_changed(uint16_t handle)
{
    uint8_t phy = 0;

    conn_handle = handle;

    if (handle != CONN_HANDLE_INVALID) {
        /* XXX: assume symmetric phy for now */
        ble_gap_read_le_phy(handle, &phy, &phy);
    }

    phy_update(phy);
}

void
phy_update(uint8_t phy)
{
    if (conn_handle == CONN_HANDLE_INVALID) {
        hal_gpio_write(led_gpio[0], 1);
        hal_gpio_write(led_gpio[1], 1);
        hal_gpio_write(led_gpio[2], 1);
    } else {
        hal_gpio_write(led_gpio[0], !(phy == BLE_GAP_LE_PHY_1M));
        hal_gpio_write(led_gpio[1], !(phy == BLE_GAP_LE_PHY_2M));
        hal_gpio_write(led_gpio[2], !(phy == BLE_GAP_LE_PHY_CODED));
    }
}

#endif
