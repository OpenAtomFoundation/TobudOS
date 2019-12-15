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

#include <stdbool.h>

#include "nrf_cli.h"
#include "nrf_crypto_rng.h"
#include "nordic_common.h"

#include "raw.h"
#include "vars.h"
#include "cli_utils.h"


/** @brief Command to generate random vector. See command help for details.
 */
static bool cmd_rng_vector_generate(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    ret_code_t   ret;
    char       * p_arg_end;
    size_t       output_size;
    var_t      * p_vector_var;

    UNUSED_PARAMETER(p_cli);

    CLI_ARGS_CHECK(argc == 3);

    p_arg_end = NULL;
    output_size = (size_t)strtol(argv[1], &p_arg_end, 0);
    if ((strcmp(p_arg_end, "bits") == 0) || (strcmp(p_arg_end, "bit") == 0))
    {
        if ((output_size % 8) != 0)
        {
            nrf_cli_fprintf(cli_get(), NRF_CLI_ERROR, "Number of bits must be multiple of 8.\n");
            return false;
        }
        output_size = output_size / 8;
    }
    else
    {
        CLI_ARGS_CHECK(p_arg_end[0] == '\0');
    }

    p_vector_var = cli_output_arg_get(2, &g_raw_type, output_size);
    CLI_CHECK(p_vector_var);

    ret = nrf_crypto_rng_vector_generate(VAR_DATA(p_vector_var), output_size);

    CLI_ERROR_CHECK(ret);

    return true;
}


CLI_CMD_REGISTER(rng_vector_generate, NULL, cmd_rng_vector_generate,
    "Generate a random vector.\n"
    "Usage:\n"
    "  rng_vector_generate size vector\n"
    "Arguments:\n"
    "  IN  size    Requested size of the vector in bytes\n"
    "  OUT vector  Resulting data\n"
    );


/** @brief Command to generate random vector in specific range. See command help for details.
 */
static bool cmd_rng_vector_generate_in_range(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    ret_code_t   ret;
    uint8_t    * p_min;
    var_t      * p_min_var;
    var_t      * p_max_var;
    var_t      * p_vector_var;

    UNUSED_PARAMETER(p_cli);
    UNUSED_PARAMETER(argv);

    CLI_ARGS_CHECK(argc == 4);

    p_min_var = cli_input_arg_get(1, &g_raw_type);
    CLI_CHECK(p_min_var);

    p_max_var = cli_input_arg_get(2, &g_raw_type);
    CLI_CHECK(p_max_var);

    p_vector_var = cli_output_arg_get(3, &g_raw_type, p_max_var->data_size);
    CLI_CHECK(p_vector_var);

    CLI_ARGS_CHECK(p_min_var->data_size <= p_max_var->data_size);

    if (p_min_var->data_size < p_max_var->data_size)
    {
        p_min = var_temp_alloc(p_max_var->data_size);
        CLI_CHECK(p_min != NULL);
        memset(p_min, 0, p_max_var->data_size);
        memcpy(&p_min[p_max_var->data_size - p_min_var->data_size], VAR_DATA(p_min_var), p_min_var->data_size);
    }
    else
    {
        p_min = VAR_DATA(p_min_var);
    }

    ret = nrf_crypto_rng_vector_generate_in_range(VAR_DATA(p_vector_var), p_min, VAR_DATA(p_max_var), p_max_var->data_size);

    CLI_ERROR_CHECK(ret);

    return true;
}


CLI_CMD_REGISTER(rng_vector_generate_in_range, NULL, cmd_rng_vector_generate_in_range,
    "Generate a random vector in specific range.\n"
    "Usage:\n"
    "  rng_vector_generate_in_range min max vector\n"
    "Arguments:\n"
    "  IN  min     Minimum value\n"
    "  IN  max     Maximum value\n"
    "  OUT vector  Resulting data\n"
    );


/** @brief Command to reseed RNG. See command help for details.
 */
static bool cmd_rng_reseed(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    ret_code_t   ret;
    uint8_t    * p_data;
    size_t       data_size;
    var_t      * p_data_var;

    UNUSED_PARAMETER(p_cli);
    UNUSED_PARAMETER(argv);

    CLI_ARGS_CHECK((argc == 1) || (argc == 2));

    if (argc == 2)
    {
        p_data_var = cli_input_arg_get(1, &g_raw_type);
        CLI_CHECK(p_data_var);
        p_data = VAR_DATA(p_data_var);
        data_size = p_data_var->data_size;
    }
    else
    {
        p_data = NULL;
        data_size = 0;
    }

    ret = nrf_crypto_rng_reseed(NULL, p_data, data_size);
    CLI_ERROR_CHECK(ret);

    return true;
}


CLI_CMD_REGISTER(rng_reseed, NULL, cmd_rng_reseed,
    "Reseed the generator.\n"
    "Usage:\n"
    "  rng_reseed [data]\n"
    "Arguments:\n"
    "  IN  data    Seed data used to reseed\n"
    );

