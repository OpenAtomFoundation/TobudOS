/**
 * Copyright (c) 2017 - 2019, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "ble_gap.h"
#include "ble_gatt.h"
#include "nrf_cli.h"
#include "nrf_strerror.h"

extern void gatt_mtu_set(uint16_t att_mtu);
extern void preferred_phy_set(ble_gap_phys_t * p_phy);
extern void conn_evt_len_ext_set(bool status);
extern void data_len_set(uint8_t value);
extern void connection_interval_set(uint16_t value);
extern void current_config_print(nrf_cli_t const * p_cli);

extern char const * phy_str(ble_gap_phys_t);
extern bool is_tester_board(void);
extern void test_begin(void);


static void att_mtu_size_set(nrf_cli_t const * p_cli, uint16_t value)
{
    if (value < BLE_GATT_ATT_MTU_DEFAULT || value > NRF_SDH_BLE_GATT_MAX_MTU_SIZE)
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "Invalid setting: %d.\n", value);
        return;
    }

    gatt_mtu_set(value);
    nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "ATT MTU size set to %d bytes.\n", value);
}


static void conn_interval_set(nrf_cli_t const * p_cli, uint16_t value)
{
    connection_interval_set(value);
    nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "Connection interval set to %d units.\n", value);
}


static void dl_set(nrf_cli_t const * p_cli, uint32_t value)
{
    // These are the stack default values.
    if (value < 27 || value > 251)
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "Invalid setting: %d.\n", value);
        return;
    }

    data_len_set(value);
    nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "Data length set to %d.\n", value);
}


static void cele_set(nrf_cli_t const * p_cli, bool enable)
{
    nrf_cli_fprintf(
        p_cli, NRF_CLI_NORMAL, "Connection event length extension %s.\n", enable ? "on" : "off");
    conn_evt_len_ext_set(enable);
}


static void phy_set(nrf_cli_t const * p_cli, uint8_t value)
{
    ble_gap_phys_t phy =
    {
        .rx_phys = value,
        .tx_phys = value,
    };

    preferred_phy_set(&phy);
    nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "Preferred PHY set to %s.\n", phy_str(phy));
}


static void cmd_conn_interval_7_5_set(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    conn_interval_set(p_cli, (uint16_t)(MSEC_TO_UNITS(7.5, UNIT_1_25_MS)));
}


static void cmd_conn_interval_50_set(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    conn_interval_set(p_cli, (uint16_t)(MSEC_TO_UNITS(50, UNIT_1_25_MS)));
}


static void cmd_conn_interval_400_set(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    conn_interval_set(p_cli, (uint16_t)(MSEC_TO_UNITS(400, UNIT_1_25_MS)));
}


NRF_CLI_CREATE_STATIC_SUBCMD_SET(m_conn_interval_cmds)
{
    NRF_CLI_CMD(7.5, NULL, "Use a 7.5ms connection interval", cmd_conn_interval_7_5_set),
    NRF_CLI_CMD(50,  NULL, "use a 50ms connection interval",  cmd_conn_interval_50_set),
    NRF_CLI_CMD(400, NULL, "Use a 400ms connection interval", cmd_conn_interval_400_set),
    NRF_CLI_SUBCMD_SET_END
};


static void cmd_cele_on_set(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    cele_set(p_cli, true);
}


static void cmd_cele_off_set(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    cele_set(p_cli, false);
}


NRF_CLI_CREATE_STATIC_SUBCMD_SET(m_cele_cmd)
{
    NRF_CLI_CMD(on,  NULL, "Turn Connection Event Length Extension on",  cmd_cele_on_set),
    NRF_CLI_CMD(off, NULL, "Turn Connection Event Length Extension off", cmd_cele_off_set),
    NRF_CLI_SUBCMD_SET_END
};


static void cmd_phy_1m_set(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    phy_set(p_cli, BLE_GAP_PHY_1MBPS);
}


static void cmd_phy_2m_set(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    phy_set(p_cli, BLE_GAP_PHY_2MBPS);
}


#ifdef NRF52840_XXAA
static void cmd_phy_coded_set(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    phy_set(p_cli, BLE_GAP_PHY_CODED);
}
#endif


NRF_CLI_CREATE_STATIC_SUBCMD_SET(m_prephy_cmds)
{
    NRF_CLI_CMD(1M,    NULL, "Set preferred PHY to 1Mbps", cmd_phy_1m_set),
    NRF_CLI_CMD(2M,    NULL, "Set preferred PHY to 2Mbps", cmd_phy_2m_set),
#ifdef NRF52840_XXAA
    NRF_CLI_CMD(coded, NULL, "Set preferred PHY to Coded", cmd_phy_coded_set),
#endif
    NRF_CLI_SUBCMD_SET_END
};


static void att_mtu_cmd(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    if ((argc == 1) || nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
    }
    else if (argc == 2)
    {
        att_mtu_size_set(p_cli, strtol(argv[1], NULL, 10));
    }
    else
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s:%s\n", argv[0], " wrong parameter list");
    }
}


static void data_len_cmd(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    if ((argc == 1) || nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
    }
    else if (argc == 2)
    {
        dl_set(p_cli, strtol(argv[1], NULL, 10));
    }
    else
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s:%s\n", argv[0], " wrong parameter list");
    }
}


static void config_print_cmd(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    current_config_print(p_cli);
}


static void gap_evt_len_cmd(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "This value cannot be set at runtime.\n"
                                           "Modify NRF_SDH_BLE_GAP_EVENT_LENGTH in sdk_config.h "
                                           "and recompile the application.\n");
}


static void default_cmd(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    if ((argc == 1) || nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
    }
    else
    {
        nrf_cli_fprintf(p_cli,
                        NRF_CLI_ERROR,
                        "%s:%s%s\n",
                        argv[0],
                        " unknown parameter: ",
                        argv[1]);
    }
}


NRF_CLI_CREATE_STATIC_SUBCMD_SET(m_test_config_cmds)
{
    NRF_CLI_CMD(att_mtu,            NULL,                  "Configure ATT MTU size",                  att_mtu_cmd),
    NRF_CLI_CMD(data_length,        NULL,                  "Configure data length",                   data_len_cmd),
    NRF_CLI_CMD(conn_evt_len_ext,   &m_cele_cmd,           "Enable or disable Data Length Extension", default_cmd),
    NRF_CLI_CMD(conn_interval,      &m_conn_interval_cmds, "Configure GAP connection interval",       default_cmd),
    NRF_CLI_CMD(print,              NULL,                  "Print current configuration",             config_print_cmd),
    NRF_CLI_CMD(phy,                &m_prephy_cmds,        "Configure preferred PHY",                 default_cmd),
    NRF_CLI_CMD(gap_evt_len,        NULL,                  "Configure GAP event length",              gap_evt_len_cmd),
    NRF_CLI_SUBCMD_SET_END
};

static void cmd_test_run(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    if (is_tester_board())
    {
        test_begin();
    }
    else
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "Wrong board setup!\n");
    }
}


NRF_CLI_CMD_REGISTER(config, &m_test_config_cmds, "Configure the example", default_cmd);
NRF_CLI_CMD_REGISTER(run,    NULL,                "Run the test",          cmd_test_run);
