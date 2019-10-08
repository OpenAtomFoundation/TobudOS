/**
 * Copyright (c) 2018 - 2019, Nordic Semiconductor ASA
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
#include <ctype.h>
#include "nrf_cli.h"
#include "nrf_log.h"
#include "sdk_common.h"
#include "nrf_stack_guard.h"

#define CLI_EXAMPLE_MAX_CMD_CNT (20u)
#define CLI_EXAMPLE_MAX_CMD_LEN (33u)
#define CLI_EXAMPLE_VALUE_BIGGER_THAN_STACK     (20000u)

/* buffer holding dynamicly created user commands */
static char m_dynamic_cmd_buffer[CLI_EXAMPLE_MAX_CMD_CNT][CLI_EXAMPLE_MAX_CMD_LEN];
/* commands counter */
static uint8_t m_dynamic_cmd_cnt;

uint32_t m_counter;
bool     m_counter_active = false;

/* Command handlers */
static void cmd_print_param(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    for (size_t i = 1; i < argc; i++)
    {
        nrf_cli_print(p_cli, "argv[%d] = %s", i, argv[i]);
    }
}

static void cmd_print_all(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    for (size_t i = 1; i < argc; i++)
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "%s ", argv[i]);
    }
    nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "\n");
}

static void cmd_print(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    ASSERT(p_cli);
    ASSERT(p_cli->p_ctx && p_cli->p_iface && p_cli->p_name);

    if ((argc == 1) || nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }

    if (argc != 2)
    {
        nrf_cli_error(p_cli, "%s: bad parameter count", argv[0]);
        return;
    }

    nrf_cli_error(p_cli, "%s: unknown parameter: %s", argv[0], argv[1]);
}

static void cmd_python(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    UNUSED_PARAMETER(argc);
    UNUSED_PARAMETER(argv);

    nrf_cli_error(p_cli, "Nice joke ;)");
}

static void cmd_dynamic(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    if ((argc == 1) || nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }

    if (argc > 2)
    {
        nrf_cli_error(p_cli, "%s: bad parameter count", argv[0]);
    }
    else
    {
        nrf_cli_error(p_cli, "%s: please specify subcommand", argv[0]);
    }
}

/* function required by qsort */
static int string_cmp(const void * p_a, const void * p_b)
{
    ASSERT(p_a);
    ASSERT(p_b);
    return strcmp((const char *)p_a, (const char *)p_b);
}

static void cmd_dynamic_add(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    if (nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }

    if (argc != 2)
    {
        nrf_cli_error(p_cli, "%s: bad parameter count", argv[0]);
        return;
    }

    if (m_dynamic_cmd_cnt >= CLI_EXAMPLE_MAX_CMD_CNT)
    {
        nrf_cli_error(p_cli, "command limit reached");
        return;
    }

    uint8_t idx;
    nrf_cli_cmd_len_t cmd_len = strlen(argv[1]);

    if (cmd_len >= CLI_EXAMPLE_MAX_CMD_LEN)
    {
        nrf_cli_error(p_cli, "too long command");
        return;
    }

    for (idx = 0; idx < cmd_len; idx++)
    {
        if (!isalnum((int)(argv[1][idx])))
        {
            nrf_cli_error(p_cli, "bad command name - please use only alphanumerical characters");
            return;
        }
    }

    for (idx = 0; idx < CLI_EXAMPLE_MAX_CMD_CNT; idx++)
    {
        if (!strcmp(m_dynamic_cmd_buffer[idx], argv[1]))
        {
            nrf_cli_error(p_cli, "duplicated command");
            return;
        }
    }

    sprintf(m_dynamic_cmd_buffer[m_dynamic_cmd_cnt++], "%s", argv[1]);

    qsort(m_dynamic_cmd_buffer,
          m_dynamic_cmd_cnt,
          sizeof (m_dynamic_cmd_buffer[0]),
          string_cmp);

    nrf_cli_print(p_cli, "command added successfully");
}

static void cmd_dynamic_show(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    if (nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }

    if (argc != 1)
    {
        nrf_cli_error(p_cli, "%s: bad parameter count", argv[0]);
        return;
    }

    if (m_dynamic_cmd_cnt == 0)
    {
        nrf_cli_warn(p_cli, "Please add some commands first.");
        return;
    }
    nrf_cli_print(p_cli, "Dynamic command list:");
    for (uint8_t i = 0; i < m_dynamic_cmd_cnt; i++)
    {
        nrf_cli_print(p_cli, "[%3d] %s", i, m_dynamic_cmd_buffer[i]);
    }
}

static void cmd_dynamic_execute(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    if (nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }

    if (argc != 2)
    {
        nrf_cli_error(p_cli, "%s: bad parameter count", argv[0]);
        return;
    }

    for (uint8_t idx = 0; idx <  m_dynamic_cmd_cnt; idx++)
    {
        if (!strcmp(m_dynamic_cmd_buffer[idx], argv[1]))
        {
            nrf_cli_print(p_cli, "dynamic command: %s", argv[1]);
            return;
        }
    }
    nrf_cli_error(p_cli, "%s: uknown parameter: %s", argv[0], argv[1]);
}

static void cmd_dynamic_remove(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    if ((argc == 1) || nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }

    if (argc != 2)
    {
        nrf_cli_error(p_cli, "%s: bad parameter count", argv[0]);
        return;
    }

    for (uint8_t idx = 0; idx <  m_dynamic_cmd_cnt; idx++)
    {
        if (!strcmp(m_dynamic_cmd_buffer[idx], argv[1]))
        {
            if (idx == CLI_EXAMPLE_MAX_CMD_CNT - 1)
            {
                m_dynamic_cmd_buffer[idx][0] = '\0';
            }
            else
            {
                memmove(m_dynamic_cmd_buffer[idx],
                        m_dynamic_cmd_buffer[idx + 1],
                        sizeof(m_dynamic_cmd_buffer[idx]) * (m_dynamic_cmd_cnt - idx));
            }
            --m_dynamic_cmd_cnt;
            nrf_cli_print(p_cli, "command removed successfully");
            return;
        }
    }
    nrf_cli_error(p_cli, "did not find command: %s", argv[1]);
}

static void cmd_counter_start(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    if (argc != 1)
    {
        nrf_cli_error(p_cli, "%s: bad parameter count", argv[0]);
        return;
    }

    m_counter_active = true;
}

static void cmd_counter_stop(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    if (argc != 1)
    {
        nrf_cli_error(p_cli, "%s: bad parameter count", argv[0]);
        return;
    }

    m_counter_active = false;
}

static void cmd_counter_reset(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    if (argc != 1)
    {
        nrf_cli_error(p_cli, "%s: bad parameter count", argv[0]);
        return;
    }

    m_counter = 0;
}

static void cmd_counter(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    ASSERT(p_cli);
    ASSERT(p_cli->p_ctx && p_cli->p_iface && p_cli->p_name);

    /* Extra defined dummy option */
    static const nrf_cli_getopt_option_t opt[] = {
        NRF_CLI_OPT(
            "--test",
            "-t",
            "dummy option help string"
        )
    };

    if ((argc == 1) || nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, opt, ARRAY_SIZE(opt));
        return;
    }

    if (argc != 2)
    {
        nrf_cli_error(p_cli, "%s: bad parameter count", argv[0]);
        return;
    }

    if (!strcmp(argv[1], "-t") || !strcmp(argv[1], "--test"))
    {
        nrf_cli_print(p_cli, "Dummy test option.");
        return;
    }

    /* subcommands have their own handlers and they are not processed here */
    nrf_cli_error(p_cli, "%s: unknown parameter: %s", argv[0], argv[1]);
}

static void cmd_nordic(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    UNUSED_PARAMETER(argc);
    UNUSED_PARAMETER(argv);

    if (nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }

    nrf_cli_fprintf(p_cli, NRF_CLI_OPTION,
                    "\n"
                    "            .co:.                   'xo,          \n"
                    "         .,collllc,.             'ckOOo::,..      \n"
                    "      .:ooooollllllll:'.     .;dOOOOOOo:::;;;'.   \n"
                    "   'okxddoooollllllllllll;'ckOOOOOOOOOo:::;;;,,,' \n"
                    "   OOOkxdoooolllllllllllllllldxOOOOOOOo:::;;;,,,'.\n"
                    "   OOOOOOkdoolllllllllllllllllllldxOOOo:::;;;,,,'.\n"
                    "   OOOOOOOOOkxollllllllllllllllllcccldl:::;;;,,,'.\n"
                    "   OOOOOOOOOOOOOxdollllllllllllllccccc::::;;;,,,'.\n"
                    "   OOOOOOOOOOOOOOOOkxdlllllllllllccccc::::;;;,,,'.\n"
                    "   kOOOOOOOOOOOOOOOOOOOkdolllllllccccc::::;;;,,,'.\n"
                    "   kOOOOOOOOOOOOOOOOOOOOOOOxdllllccccc::::;;;,,,'.\n"
                    "   kOOOOOOOOOOOOOOOOOOOOOOOOOOkxolcccc::::;;;,,,'.\n"
                    "   kOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOkdlc::::;;;,,,'.\n"
                    "   xOOOOOOOOOOOxdkOOOOOOOOOOOOOOOOOOOOxoc:;;;,,,'.\n"
                    "   xOOOOOOOOOOOdc::ldkOOOOOOOOOOOOOOOOOOOkdc;,,,''\n"
                    "   xOOOOOOOOOOOdc::;;,;cdkOOOOOOOOOOOOOOOOOOOxl;''\n"
                    "   .lkOOOOOOOOOdc::;;,,''..;oOOOOOOOOOOOOOOOOOOOx'\n"
                    "      .;oOOOOOOdc::;;,.       .:xOOOOOOOOOOOOd;.  \n"
                    "          .:xOOdc:,.              'ckOOOOkl'      \n"
                    "             .od'                    'xk,         \n"
                    "\n");

    nrf_cli_print(p_cli, "                Nordic Semiconductor              \n");
}

/* This function cannot be static otherwise it can be inlined. As a result, variable:
   tab[CLI_EXAMPLE_VALUE_BIGGER_THAN_STACK] will be always created on stack. This will block
   possiblity to call functions: nrf_cli_help_requested and nrf_cli_help_print within
   cmd_stack_overflow, because stack guard will be triggered. */
void cli_example_stack_overflow_force(void)
{
    char tab[CLI_EXAMPLE_VALUE_BIGGER_THAN_STACK];
    volatile char * p_tab = tab;

    /* This function accesses stack area protected by nrf_stack_guard. As a result
       MPU (memory protection unit) triggers an exception (hardfault). Hardfault handler will log
       exception reason.*/
    for (size_t idx = 0; idx < STACK_SIZE; idx++)
    {
        *(p_tab + idx) = (uint8_t)idx;
    }

}

static void cmd_stack_overflow(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    UNUSED_PARAMETER(argc);
    UNUSED_PARAMETER(argv);

    if (nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }

    cli_example_stack_overflow_force();
}


/**
 * @brief Command set array
 * */
NRF_CLI_CMD_REGISTER(nordic, NULL, "Print Nordic Semiconductor logo.", cmd_nordic);

NRF_CLI_CREATE_STATIC_SUBCMD_SET(m_sub_print)
{
    NRF_CLI_CMD(all,   NULL, "Print all entered parameters.", cmd_print_all),
    NRF_CLI_CMD(param, NULL, "Print each parameter in new line.", cmd_print_param),
    NRF_CLI_SUBCMD_SET_END
};
NRF_CLI_CMD_REGISTER(print, &m_sub_print, "print", cmd_print);

NRF_CLI_CMD_REGISTER(python, NULL, "python", cmd_python);

NRF_CLI_CREATE_STATIC_SUBCMD_SET(m_sub_counter)
{
    NRF_CLI_CMD(reset,  NULL, "Reset seconds counter.",  cmd_counter_reset),
    NRF_CLI_CMD(start,  NULL, "Start seconds counter.",  cmd_counter_start),
    NRF_CLI_CMD(stop,   NULL, "Stop seconds counter.",   cmd_counter_stop),
    NRF_CLI_SUBCMD_SET_END
};
NRF_CLI_CMD_REGISTER(counter,
                     &m_sub_counter,
                     "Display seconds on terminal screen",
                     cmd_counter);

NRF_CLI_CMD_REGISTER(stack_overflow,
                     NULL,
                     "Command tests nrf_stack_guard module. Upon command call stack will be "
                     "overflowed and microcontroller shall log proper reset reason. \n\rTo observe "
                     "stack_guard execution, stack shall be set to value lower than 20000 bytes.",
                     cmd_stack_overflow);


/* dynamic command creation */
static void dynamic_cmd_get(size_t idx, nrf_cli_static_entry_t * p_static)
{
    ASSERT(p_static);

    if (idx < m_dynamic_cmd_cnt)
    {
        /* m_dynamic_cmd_buffer must be sorted alphabetically to ensure correct CLI completion */
        p_static->p_syntax = m_dynamic_cmd_buffer[idx];
        p_static->handler  = NULL;
        p_static->p_subcmd = NULL;
        p_static->p_help = "Show dynamic command name.";
    }
    else
    {
        /* if there are no more dynamic commands available p_syntax must be set to NULL */
        p_static->p_syntax = NULL;
    }
}

NRF_CLI_CREATE_DYNAMIC_CMD(m_sub_dynamic_set, dynamic_cmd_get);
NRF_CLI_CREATE_STATIC_SUBCMD_SET(m_sub_dynamic)
{
    NRF_CLI_CMD(add, NULL,
        "Add a new dynamic command.\nExample usage: [ dynamic add test ] will add "
        "a dynamic command 'test'.\nIn this example, command name length is limited to 32 chars. "
        "You can add up to 20 commands. Commands are automatically sorted to ensure correct "
        "CLI completion.",
        cmd_dynamic_add),
    NRF_CLI_CMD(execute, &m_sub_dynamic_set, "Execute a command.", cmd_dynamic_execute),
    NRF_CLI_CMD(remove, &m_sub_dynamic_set, "Remove a command.", cmd_dynamic_remove),
    NRF_CLI_CMD(show, NULL, "Show all added dynamic commands.", cmd_dynamic_show),
    NRF_CLI_SUBCMD_SET_END
};
NRF_CLI_CMD_REGISTER(dynamic,
                     &m_sub_dynamic,
                     "Demonstrate dynamic command usage.",
                     cmd_dynamic);
