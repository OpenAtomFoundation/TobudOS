/* main.c - Application main entry point */

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

/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * This application is specific to the Nordic nRF52840-PDK board.
 *
 * It supports the 4 buttons and 4 LEDs as mesh clients and servers.
 *
 * Prior to provisioning, a button inverts the state of the
 * corresponding LED.
 *
 * Button and LED 1 are in the root node.
 * The 3 remaining button/LED pairs are in element 1 through 3.
 * Assuming the provisioner assigns 0x100 to the root node,
 * the secondary elements will appear at 0x101, 0x102 and 0x103.
 *
 * It's anticipated that after provisioning, the button clients would
 * be configured to publish and the LED servers to subscribe.
 *
 * If a LED server is provided with a publish address, it will
 * also publish its status on a state change.
 *
 * Messages from a button to its corresponding LED are ignored as
 * the LED's state has already been changed locally by the button client.
 *
 * The buttons are debounced at a nominal 250ms. That value can be
 * changed as needed.
 *
 */

#include "bsp/bsp.h"
#include "console/console.h"
#include "host/ble_hs.h"
#include "mesh/glue.h"
#include "mesh/mesh.h"

///////////////////////
// stub-s

/*
 * The "pull" of the gpio. This is either an input or an output.
 */
enum hal_gpio_pull {
    /** Pull-up/down not enabled */
    HAL_GPIO_PULL_NONE = 0,
    /** Pull-up enabled */
    HAL_GPIO_PULL_UP = 1,
    /** Pull-down enabled */
    HAL_GPIO_PULL_DOWN = 2
};
typedef enum hal_gpio_pull hal_gpio_pull_t;

/*
 * IRQ trigger type.
 */
enum hal_gpio_irq_trigger {
    HAL_GPIO_TRIG_NONE = 0,
    /** IRQ occurs on rising edge */
    HAL_GPIO_TRIG_RISING = 1,
    /** IRQ occurs on falling edge */
    HAL_GPIO_TRIG_FALLING = 2,
    /** IRQ occurs on either edge */
    HAL_GPIO_TRIG_BOTH = 3,
    /** IRQ occurs when line is low */
    HAL_GPIO_TRIG_LOW = 4,
    /** IRQ occurs when line is high */
    HAL_GPIO_TRIG_HIGH = 5
};
typedef enum hal_gpio_irq_trigger hal_gpio_irq_trig_t;


/* Function proto for GPIO irq handler functions */
typedef void (*hal_gpio_irq_handler_t)(void *arg);

int hal_gpio_irq_init(int pin, hal_gpio_irq_handler_t handler, void *arg,
                      hal_gpio_irq_trig_t trig, hal_gpio_pull_t pull)
{
    // printf("gpio irq init\n");
    return 0;
}

void hal_gpio_irq_enable(int pin)
{
    // printf("gpio irq enable\n");
}

void hal_gpio_init_out(int led, int s)
{
    // printf("gpio init out\n");
}

void hal_gpio_write(int led, int s)
{
    // printf("led[%d]: %s\n", s == 0 ? "OFF" : "ON");
}

int __atomic_load_4(int *p)
{
    return *p;
}

int __atomic_fetch_and_4(int *p, int v)
{
    return *p & v;
}

int __atomic_fetch_or_4(int *p, int v)
{
    return *p | v;
}

int __atomic_exchange_4(int *p, int v)
{
    int old = *p;
    *p = v;
    return old;
}

void unreachable(void)
{
    while (1) {
        ;
    }
}

///////////////////////


#define CID_RUNTIME 0x05C3

/* Model Operation Codes */
#define BT_MESH_MODEL_OP_GEN_ONOFF_GET		BT_MESH_MODEL_OP_2(0x82, 0x01)
#define BT_MESH_MODEL_OP_GEN_ONOFF_SET		BT_MESH_MODEL_OP_2(0x82, 0x02)
#define BT_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK	BT_MESH_MODEL_OP_2(0x82, 0x03)
#define BT_MESH_MODEL_OP_GEN_ONOFF_STATUS	BT_MESH_MODEL_OP_2(0x82, 0x04)

static void gen_onoff_set(struct bt_mesh_model *model,
                          struct bt_mesh_msg_ctx *ctx,
                          struct os_mbuf *buf);

static void gen_onoff_set_unack(struct bt_mesh_model *model,
                                struct bt_mesh_msg_ctx *ctx,
                                struct os_mbuf *buf);

static void gen_onoff_get(struct bt_mesh_model *model,
                          struct bt_mesh_msg_ctx *ctx,
                          struct os_mbuf *buf);

static void gen_onoff_status(struct bt_mesh_model *model,
                             struct bt_mesh_msg_ctx *ctx,
                             struct os_mbuf *buf);

/*
 * Server Configuration Declaration
 */

static struct bt_mesh_cfg_srv cfg_srv = {
        .relay = BT_MESH_RELAY_DISABLED,
        .beacon = BT_MESH_BEACON_ENABLED,
#if defined(CONFIG_BT_MESH_FRIEND)
        .frnd = BT_MESH_FRIEND_ENABLED,
#else
        .frnd = BT_MESH_FRIEND_NOT_SUPPORTED,
#endif
#if defined(CONFIG_BT_MESH_GATT_PROXY)
        .gatt_proxy = BT_MESH_GATT_PROXY_ENABLED,
#else
        .gatt_proxy = BT_MESH_GATT_PROXY_NOT_SUPPORTED,
#endif
        .default_ttl = 7,

        /* 3 transmissions with 20ms interval */
        .net_transmit = BT_MESH_TRANSMIT(2, 20),
        .relay_retransmit = BT_MESH_TRANSMIT(2, 20),
};

/*
 * Client Configuration Declaration
 */

static struct bt_mesh_cfg_cli cfg_cli = {
};

/*
 * Health Server Declaration
 */

static struct bt_mesh_health_srv health_srv = {
};

/*
 * Publication Declarations
 *
 * The publication messages are initialized to the
 * the size of the opcode + content
 *
 * For publication, the message must be in static or global as
 * it is re-transmitted several times. This occurs
 * after the function that called bt_mesh_model_publish() has
 * exited and the stack is no longer valid.
 *
 * Note that the additional 4 bytes for the AppMIC is not needed
 * because it is added to a stack variable at the time a
 * transmission occurs.
 *
 */

static struct bt_mesh_model_pub health_pub;
static struct bt_mesh_model_pub gen_onoff_pub_srv;
static struct bt_mesh_model_pub gen_onoff_pub_cli;
static struct bt_mesh_model_pub gen_onoff_pub_srv_s_0;
static struct bt_mesh_model_pub gen_onoff_pub_cli_s_0;
static struct bt_mesh_model_pub gen_onoff_pub_srv_s_1;
static struct bt_mesh_model_pub gen_onoff_pub_cli_s_1;
static struct bt_mesh_model_pub gen_onoff_pub_srv_s_2;
static struct bt_mesh_model_pub gen_onoff_pub_cli_s_2;

static struct os_mbuf *bt_mesh_pub_msg_health_pub;
static struct os_mbuf *bt_mesh_pub_msg_gen_onoff_pub_srv;
static struct os_mbuf *bt_mesh_pub_msg_gen_onoff_pub_cli;
static struct os_mbuf *bt_mesh_pub_msg_gen_onoff_pub_srv_s_0;
static struct os_mbuf *bt_mesh_pub_msg_gen_onoff_pub_cli_s_0;
static struct os_mbuf *bt_mesh_pub_msg_gen_onoff_pub_srv_s_1;
static struct os_mbuf *bt_mesh_pub_msg_gen_onoff_pub_cli_s_1;
static struct os_mbuf *bt_mesh_pub_msg_gen_onoff_pub_srv_s_2;
static struct os_mbuf *bt_mesh_pub_msg_gen_onoff_pub_cli_s_2;

void init_pub(void)
{
    bt_mesh_pub_msg_health_pub              = NET_BUF_SIMPLE(1 + 3 + 0);
    bt_mesh_pub_msg_gen_onoff_pub_srv       = NET_BUF_SIMPLE(2 + 2);
    bt_mesh_pub_msg_gen_onoff_pub_cli       = NET_BUF_SIMPLE(2 + 2);
    bt_mesh_pub_msg_gen_onoff_pub_srv_s_0   = NET_BUF_SIMPLE(2 + 2);
    bt_mesh_pub_msg_gen_onoff_pub_cli_s_0   = NET_BUF_SIMPLE(2 + 2);
    bt_mesh_pub_msg_gen_onoff_pub_srv_s_1   = NET_BUF_SIMPLE(2 + 2);
    bt_mesh_pub_msg_gen_onoff_pub_cli_s_1   = NET_BUF_SIMPLE(2 + 2);
    bt_mesh_pub_msg_gen_onoff_pub_srv_s_2   = NET_BUF_SIMPLE(2 + 2);
    bt_mesh_pub_msg_gen_onoff_pub_cli_s_2   = NET_BUF_SIMPLE(2 + 2);

    health_pub.msg =                 bt_mesh_pub_msg_health_pub;
    gen_onoff_pub_srv.msg =          bt_mesh_pub_msg_gen_onoff_pub_srv;
    gen_onoff_pub_cli.msg =          bt_mesh_pub_msg_gen_onoff_pub_cli;
    gen_onoff_pub_srv_s_0.msg =      bt_mesh_pub_msg_gen_onoff_pub_srv_s_0;
    gen_onoff_pub_cli_s_0.msg =      bt_mesh_pub_msg_gen_onoff_pub_cli_s_0;
    gen_onoff_pub_srv_s_1.msg =      bt_mesh_pub_msg_gen_onoff_pub_srv_s_1;
    gen_onoff_pub_cli_s_1.msg =      bt_mesh_pub_msg_gen_onoff_pub_cli_s_1;
    gen_onoff_pub_srv_s_2.msg =      bt_mesh_pub_msg_gen_onoff_pub_srv_s_2;
    gen_onoff_pub_cli_s_2.msg =      bt_mesh_pub_msg_gen_onoff_pub_cli_s_2;
}

/*
 * Models in an element must have unique op codes.
 *
 * The mesh stack dispatches a message to the first model in an element
 * that is also bound to an app key and supports the op code in the
 * received message.
 *
 */

/*
 * OnOff Model Server Op Dispatch Table
 *
 */

static const struct bt_mesh_model_op gen_onoff_srv_op[] = {
        { BT_MESH_MODEL_OP_GEN_ONOFF_GET, 0, gen_onoff_get },
        { BT_MESH_MODEL_OP_GEN_ONOFF_SET, 2, gen_onoff_set },
        { BT_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK, 2, gen_onoff_set_unack },
        BT_MESH_MODEL_OP_END,
};

/*
 * OnOff Model Client Op Dispatch Table
 */

static const struct bt_mesh_model_op gen_onoff_cli_op[] = {
        { BT_MESH_MODEL_OP_GEN_ONOFF_STATUS, 1, gen_onoff_status },
        BT_MESH_MODEL_OP_END,
};

struct onoff_state {
    u8_t current;
    u8_t previous;
    u8_t led_gpio_pin;
};

/*
 * Declare and Initialize Element Contexts
 * Change to select different GPIO output pins
 */

static struct onoff_state onoff_state_arr[] = {
        { .led_gpio_pin = LED_1 },
        { .led_gpio_pin = LED_2 },
        { .led_gpio_pin = LED_3 },
        { .led_gpio_pin = LED_4 },
};

/*
 *
 * Element Model Declarations
 *
 * Element 0 Root Models
 */

static struct bt_mesh_model root_models[] = {
        BT_MESH_MODEL_CFG_SRV(&cfg_srv),
        BT_MESH_MODEL_CFG_CLI(&cfg_cli),
        BT_MESH_MODEL_HEALTH_SRV(&health_srv, &health_pub),
        BT_MESH_MODEL(BT_MESH_MODEL_ID_GEN_ONOFF_SRV, gen_onoff_srv_op,
                      &gen_onoff_pub_srv, &onoff_state_arr[0]),
        BT_MESH_MODEL(BT_MESH_MODEL_ID_GEN_ONOFF_CLI, gen_onoff_cli_op,
                      &gen_onoff_pub_cli, &onoff_state_arr[0]),
};

/*
 * Element 1 Models
 */

static struct bt_mesh_model secondary_0_models[] = {
        BT_MESH_MODEL(BT_MESH_MODEL_ID_GEN_ONOFF_SRV, gen_onoff_srv_op,
                      &gen_onoff_pub_srv_s_0, &onoff_state_arr[1]),
        BT_MESH_MODEL(BT_MESH_MODEL_ID_GEN_ONOFF_CLI, gen_onoff_cli_op,
                      &gen_onoff_pub_cli_s_0, &onoff_state_arr[1]),
};

/*
 * Element 2 Models
 */

static struct bt_mesh_model secondary_1_models[] = {
        BT_MESH_MODEL(BT_MESH_MODEL_ID_GEN_ONOFF_SRV, gen_onoff_srv_op,
                      &gen_onoff_pub_srv_s_1, &onoff_state_arr[2]),
        BT_MESH_MODEL(BT_MESH_MODEL_ID_GEN_ONOFF_CLI, gen_onoff_cli_op,
                      &gen_onoff_pub_cli_s_1, &onoff_state_arr[2]),
};

/*
 * Element 3 Models
 */

static struct bt_mesh_model secondary_2_models[] = {
        BT_MESH_MODEL(BT_MESH_MODEL_ID_GEN_ONOFF_SRV, gen_onoff_srv_op,
                      &gen_onoff_pub_srv_s_2, &onoff_state_arr[3]),
        BT_MESH_MODEL(BT_MESH_MODEL_ID_GEN_ONOFF_CLI, gen_onoff_cli_op,
                      &gen_onoff_pub_cli_s_2, &onoff_state_arr[3]),
};

/*
 * Button to Client Model Assignments
 */

struct bt_mesh_model *mod_cli_sw[] = {
        &root_models[4],
        &secondary_0_models[1],
        &secondary_1_models[1],
        &secondary_2_models[1],
};

/*
 * LED to Server Model Assigmnents
 */

struct bt_mesh_model *mod_srv_sw[] = {
        &root_models[3],
        &secondary_0_models[0],
        &secondary_1_models[0],
        &secondary_2_models[0],
};

/*
 * Root and Secondary Element Declarations
 */

static struct bt_mesh_elem elements[] = {
        BT_MESH_ELEM(0, root_models, BT_MESH_MODEL_NONE),
        BT_MESH_ELEM(0, secondary_0_models, BT_MESH_MODEL_NONE),
        BT_MESH_ELEM(0, secondary_1_models, BT_MESH_MODEL_NONE),
        BT_MESH_ELEM(0, secondary_2_models, BT_MESH_MODEL_NONE),
};

static const struct bt_mesh_comp comp = {
        .cid = CID_RUNTIME,
        .elem = elements,
        .elem_count = ARRAY_SIZE(elements),
};

struct sw {
    u8_t sw_num;
    u8_t onoff_state;
    struct ble_npl_callout button_work;
    struct ble_npl_callout button_timer;
};


static u8_t button_press_cnt;
static struct sw sw;

static u8_t trans_id;
static u32_t time, last_time;
static u16_t primary_addr;
static u16_t primary_net_idx;

/*
 * Generic OnOff Model Server Message Handlers
 *
 * Mesh Model Specification 3.1.1
 *
 */

static void gen_onoff_get(struct bt_mesh_model *model,
                          struct bt_mesh_msg_ctx *ctx,
                          struct os_mbuf *buf)
{
    struct os_mbuf *msg = NET_BUF_SIMPLE(2 + 1 + 4);
    struct onoff_state *state = model->user_data;

    BT_INFO("addr 0x%04x onoff 0x%02x",
                bt_mesh_model_elem(model)->addr, state->current);
    bt_mesh_model_msg_init(msg, BT_MESH_MODEL_OP_GEN_ONOFF_STATUS);
    net_buf_simple_add_u8(msg, state->current);

    if (bt_mesh_model_send(model, ctx, msg, NULL, NULL)) {
        BT_ERR("Unable to send On Off Status response");
    }

    os_mbuf_free_chain(msg);
}

static void gen_onoff_set_unack(struct bt_mesh_model *model,
                                struct bt_mesh_msg_ctx *ctx,
                                struct os_mbuf *buf)
{
    struct os_mbuf *msg = model->pub->msg;
    struct onoff_state *state = model->user_data;
    int err;

    state->current = net_buf_simple_pull_u8(buf);
    BT_INFO("addr 0x%02x state 0x%02x",
                bt_mesh_model_elem(model)->addr, state->current);

    /* Pin set low turns on LED's on the nrf52840-pca10056 board */
    hal_gpio_write(state->led_gpio_pin,
                   state->current ? 0 : 1);

    /*
     * If a server has a publish address, it is required to
     * publish status on a state change
     *
     * See Mesh Profile Specification 3.7.6.1.2
     *
     * Only publish if there is an assigned address
     */

    if (state->previous != state->current &&
        model->pub->addr != BT_MESH_ADDR_UNASSIGNED) {
        BT_INFO("publish last 0x%02x cur 0x%02x",
                    state->previous,
                    state->current);
        state->previous = state->current;
        bt_mesh_model_msg_init(msg,
                               BT_MESH_MODEL_OP_GEN_ONOFF_STATUS);
        net_buf_simple_add_u8(msg, state->current);
        err = bt_mesh_model_publish(model);
        if (err) {
            BT_ERR("bt_mesh_model_publish err %d", err);
        }
    }
}

static void gen_onoff_set(struct bt_mesh_model *model,
                          struct bt_mesh_msg_ctx *ctx,
                          struct os_mbuf *buf)
{
    BT_INFO("");

    gen_onoff_set_unack(model, ctx, buf);
    gen_onoff_get(model, ctx, buf);
}

static void gen_onoff_status(struct bt_mesh_model *model,
                             struct bt_mesh_msg_ctx *ctx,
                             struct os_mbuf *buf)
{
    u8_t	state;

    state = net_buf_simple_pull_u8(buf);

    BT_INFO("Node 0x%04x OnOff status from 0x%04x with state 0x%02x",
                bt_mesh_model_elem(model)->addr, ctx->addr, state);
}

static int output_number(bt_mesh_output_action_t action, u32_t number)
{
    BT_INFO("OOB Number %u", number);
    return 0;
}

static int output_string(const char *str)
{
    BT_INFO("OOB String %s", str);
    return 0;
}

static void prov_complete(u16_t net_idx, u16_t addr)
{
    BT_INFO("provisioning complete for net_idx 0x%04x addr 0x%04x",
                net_idx, addr);
    primary_addr = addr;
    primary_net_idx = net_idx;
}

static void prov_reset(void)
{
    bt_mesh_prov_enable(BT_MESH_PROV_ADV | BT_MESH_PROV_GATT);
}

static u8_t dev_uuid[16] = MYNEWT_VAL(BLE_MESH_DEV_UUID);

#define BUTTON_DEBOUNCE_DELAY_MS 250

/*
 * Map GPIO pins to button number
 * Change to select different GPIO input pins
 */

static uint8_t pin_to_sw(int pin_pos)
{
    switch (pin_pos) {
        case BUTTON_1: return 0;
        case BUTTON_2: return 1;
        case BUTTON_3: return 2;
        case BUTTON_4: return 3;
        default:break;
    }

    BT_ERR("No match for GPIO pin 0x%08x", pin_pos);
    return 0;
}

static void button_pressed(struct ble_npl_event *ev)
{
    int pin_pos = (int ) ev->arg;
    /*
     * One button press within a 1 second interval sends an on message
     * More than one button press sends an off message
     */

    time = k_uptime_get_32();

    /* debounce the switch */
    if (time < last_time + BUTTON_DEBOUNCE_DELAY_MS) {
        last_time = time;
        return;
    }

    if (button_press_cnt == 0) {
        ble_npl_callout_reset(&sw.button_timer, ble_npl_time_ms_to_ticks32(K_SECONDS(1)));
    }

    BT_INFO("button_press_cnt 0x%02x", button_press_cnt);
    button_press_cnt++;

    /* The variable pin_pos is the pin position in the GPIO register,
     * not the pin number. It's assumed that only one bit is set.
     */

    sw.sw_num = pin_to_sw(pin_pos);
    last_time = time;
}

/*
 * Button Count Timer Worker
 */

static void button_cnt_timer(struct ble_npl_event *work)
{
    struct sw *button_sw = work->arg;

    button_sw->onoff_state = button_press_cnt == 1 ? 1 : 0;
    BT_INFO("button_press_cnt 0x%02x onoff_state 0x%02x",
                button_press_cnt, button_sw->onoff_state);
    button_press_cnt = 0;
    ble_npl_callout_reset(&sw.button_work, 0);
}

/*
 * Button Pressed Worker Task
 */

static void button_pressed_worker(struct ble_npl_event *work)
{
    struct os_mbuf *msg = NET_BUF_SIMPLE(1);
    struct bt_mesh_model *mod_cli, *mod_srv;
    struct bt_mesh_model_pub *pub_cli, *pub_srv;
    struct sw *sw = work->arg;
    u8_t sw_idx = sw->sw_num;
    int err;

    mod_cli = mod_cli_sw[sw_idx];
    pub_cli = mod_cli->pub;

    mod_srv = mod_srv_sw[sw_idx];
    pub_srv = mod_srv->pub;
    (void)pub_srv;

    /* If unprovisioned, just call the set function.
     * The intent is to have switch-like behavior
     * prior to provisioning. Once provisioned,
     * the button and its corresponding led are no longer
     * associated and act independently. So, if a button is to
     * control its associated led after provisioning, the button
     * must be configured to either publish to the led's unicast
     * address or a group to which the led is subscribed.
     */

    if (primary_addr == BT_MESH_ADDR_UNASSIGNED) {
        struct bt_mesh_msg_ctx ctx = {
                .addr = sw_idx + primary_addr,
        };

        /* This is a dummy message sufficient
	 * for the led server
	 */

        net_buf_simple_add_u8(msg, sw->onoff_state);
        gen_onoff_set_unack(mod_srv, &ctx, msg);
        goto done;
    }

    if (pub_cli->addr == BT_MESH_ADDR_UNASSIGNED) {
        goto done;
    }

    BT_INFO("publish to 0x%04x onoff 0x%04x sw_idx 0x%04x",
                pub_cli->addr, sw->onoff_state, sw_idx);
    bt_mesh_model_msg_init(pub_cli->msg,
                           BT_MESH_MODEL_OP_GEN_ONOFF_SET);
    net_buf_simple_add_u8(pub_cli->msg, sw->onoff_state);
    net_buf_simple_add_u8(pub_cli->msg, trans_id++);
    err = bt_mesh_model_publish(mod_cli);
    if (err) {
        BT_ERR("bt_mesh_model_publish err %d", err);
    }

done:
    os_mbuf_free_chain(msg);
}

/* Disable OOB security for SILabs Android app */

static const struct bt_mesh_prov prov = {
    .uuid = dev_uuid,
#if 1
    .output_size = 6,
    .output_actions = (BT_MESH_DISPLAY_NUMBER | BT_MESH_DISPLAY_STRING),
    .output_number = output_number,
    .output_string = output_string,
#else
    .output_size = 0,
    .output_actions = 0,
    .output_number = 0,
#endif
    .complete = prov_complete,
    .reset = prov_reset,
};

void init_led(u8_t dev)
{
    hal_gpio_init_out(onoff_state_arr[dev].led_gpio_pin, 1);
}

static struct ble_npl_event button_event;

static void
gpio_irq_handler(void *arg)
{
    button_event.arg = arg;
    ble_npl_eventq_put(nimble_port_get_dflt_eventq(), &button_event);
}

void init_button(int button)
{
    button_event.fn = button_pressed;

    hal_gpio_irq_init(button, gpio_irq_handler, (void *)button,
                      HAL_GPIO_TRIG_FALLING, HAL_GPIO_PULL_UP);
    hal_gpio_irq_enable(button);
}

static void
blemesh_on_reset(int reason)
{
    BLE_HS_LOG(ERROR, "Resetting state; reason=%d\n", reason);
}

static void
blemesh_on_sync(void)
{
    int err;
    ble_addr_t addr;

    console_printf("Bluetooth initialized\n");

    /* Use NRPA */
    err = ble_hs_id_gen_rnd(1, &addr);
    assert(err == 0);
    err = ble_hs_id_set_rnd(addr.val);
    assert(err == 0);

    err = bt_mesh_init(addr.type, &prov, &comp);
    if (err) {
        console_printf("Initializing mesh failed (err %d)\n", err);
        return;
    }

    if (IS_ENABLED(CONFIG_SETTINGS)) {
        settings_load();
    }

    if (bt_mesh_is_provisioned()) {
        console_printf("Mesh network restored from flash\n");
    }

    bt_mesh_prov_enable(BT_MESH_PROV_GATT | BT_MESH_PROV_ADV);

    console_printf("Mesh initialized\n");
}

int ble_boot(void)
{
#ifdef ARCH_sim
    mcu_sim_parse_args(argc, argv);
#endif

    BT_INFO("Initializing...");

    /* Initialize the button debouncer */
    last_time = k_uptime_get_32();

    /* Initialize button worker task*/
    ble_npl_callout_init(&sw.button_work, nimble_port_get_dflt_eventq(),
                    button_pressed_worker, &sw);

    /* Initialize button count timer */
    ble_npl_callout_init(&sw.button_timer, nimble_port_get_dflt_eventq(),
                    button_cnt_timer, &sw);

    /* Initialize LED's */
    init_led(0);
    init_led(1);
    init_led(2);
    init_led(3);

    init_button(BUTTON_1);
    init_button(BUTTON_2);
    init_button(BUTTON_3);
    init_button(BUTTON_4);

    init_pub();

    /* Initialize the NimBLE host configuration. */
    ble_hs_cfg.reset_cb = blemesh_on_reset;
    ble_hs_cfg.sync_cb = blemesh_on_sync;
    ble_hs_cfg.store_status_cb = ble_store_util_status_rr;

    bt_mesh_register_gatt();

    extern void nimble_port_run(void);
    nimble_port_tencentos_tiny_init(nimble_port_run);

    return 0;
}

k_task_t ble_boot_task;
k_stack_t ble_boot_stack[512];

int main(void)
{
    board_init();

    /* Initialize OS */
    tos_knl_init();

    nimble_port_init();

    tos_task_create(&ble_boot_task, "boot", ble_boot, NULL,
                        4,
                        ble_boot_stack, sizeof(ble_boot_stack),
                        0);
    tos_knl_start();
}

