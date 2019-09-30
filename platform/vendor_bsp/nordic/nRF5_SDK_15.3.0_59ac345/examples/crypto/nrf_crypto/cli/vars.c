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

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "sdk_config.h"
#include "nrf_cli.h"
#include "cli_utils.h"
#include "raw.h"

#include "vars.h"


/* =============================================================================================
 *                         Configuration and definitions
 * =============================================================================================
 */


#ifndef APP_VAR_MEMORY_SIZE
#define APP_VAR_MEMORY_SIZE (16 * 1024)  ///< @brief Define default size of the variable memory space.
#endif

#define TEMP_PREFIX     "$_TMP@"  ///< @brief Variable name prefix for temporary variables.
#define TEMP_PREFIX_LEN 6         ///< @brief Length of @ref TEMP_PREFIX.


/* =============================================================================================
 *                         Variable memory buffer definition
 * =============================================================================================
 */


#define VAR_MEMORY_BEGIN ((uint8_t *)var_memory)                       ///< @brief Returns pointer to the first byte of the variable memory space.
#define VAR_MEMORY_END   ((uint8_t *)var_memory + APP_VAR_MEMORY_SIZE) ///< @brief Returns pointer to the next byte after the last byte of the variable memory space.

static uint32_t var_memory[(APP_VAR_MEMORY_SIZE + 3) / 4];             ///< @brief Array holding variable memory space. It is uint32_t to force correct alignment.


/* =============================================================================================
 *                   Dynamically allocated memory block type definition
 * =============================================================================================
 */


/** @brief Callback function to prevent user from deleting a dynamically allocated block. @see var_free_fn_t.
 */
static bool alloc_user_free_prevent(var_t * p_var)
{
    nrf_cli_fprintf(cli_get(), NRF_CLI_WARNING, "Cannot delete dynamically allocated memory.\n");
    return false;
}


/** @brief Variable type that contains the dynamically allocated block.
 */
static var_type_t var_alloc =
{
    .p_description = "Dynamically allocated memory buffer",
    .free_fn = alloc_user_free_prevent,
    .print_fn = NULL,
    .p_tag = NULL,
};



/* =============================================================================================
 *                                Block access functions
 * =============================================================================================
 */


/** @brief Returns the first block of the variable memory space, including free and dynamically allocated blocks.
 */
static var_header_t * block_begin()
{
    return (var_header_t *)VAR_MEMORY_BEGIN;
}


/** @brief Returns the next block after the one provided.
 */
static var_header_t * block_next(var_header_t * current)
{
    uint8_t * p_this = (uint8_t *)current;
    uint8_t * p_next = p_this + current->total_size;
    if (p_next < VAR_MEMORY_END)
    {
        return (var_header_t *)p_next;
    }
    else
    {
        return NULL;
    }
}


/** @brief Returns true if block is a variable, meaning it is not free and it is not a dynamically allocated block.
 */
static bool block_is_var(var_header_t * p_block)
{
    return (p_block->p_type != NULL) && (p_block->p_type != &var_alloc);
}


/** @brief Combines all neighboring free blocks, starting with the block provided in the parameter.
 */
static void block_free_space_consolidate(var_header_t * p_block)
{
    var_header_t * p_next_block;
    if ((p_block == NULL) || (p_block->p_type != NULL))
    {
        return;
    }
    p_next_block = block_next(p_block);
    while ((p_next_block != NULL) && (p_next_block->p_type == NULL))
    {
        p_block->total_size += p_next_block->total_size;
        p_next_block = block_next(p_block);
    }
}


/** @brief Allocates a block inside the variable memory space.
 *
 *  Only total size of the block is set after this call. The rest of data (including content) is zeroed.
 *  @param size     Requested total size of a block, including header.
 *  @param verbose  Prints messages in case of problems if true.
 *  @return         Pointer to the newly allocated block, or NULL if there is no space available.
 */
static var_header_t * block_allocate(size_t size, bool verbose)
{
    var_header_t * p_block;
    var_header_t * p_next_block;
    uint32_t       allocate_size = (size + 3) & ~3;

    // Iterate over all blocks
    for (p_block = block_begin(); p_block != NULL; p_block = block_next(p_block))
    {
        // Skip used blocks
        if (p_block->p_type != NULL)
        {
            continue;
        }
        // If after this unused block there are also unused blocks, merge them all.
        block_free_space_consolidate(p_block);
        // If this unused block is big enough to hold requested size, use it.
        if (p_block->total_size >= allocate_size)
        {
            size_t old_size = p_block->total_size;
            memset(p_block, 0x00, p_block->total_size);
            if (old_size - allocate_size > sizeof(var_header_t))
            {
                // This block is bigger than requested, so divide it into two blocks.
                p_block->total_size = allocate_size;
                p_next_block = block_next(p_block);
                p_next_block->p_type = NULL;
                p_next_block->total_size = old_size - allocate_size;
            }
            else
            {
                p_block->total_size = old_size;
            }
            return p_block;
        }
    }

    if (verbose)
    {
        nrf_cli_fprintf(cli_get(), NRF_CLI_WARNING, "No more space in variable memory\n");
    }

    return NULL;
}


/* =============================================================================================
 *                              Variable access functions
 * =============================================================================================
 */


void var_init(void)
{
    var_header_t * initial_block = block_begin();
    initial_block->total_size = APP_VAR_MEMORY_SIZE;
    initial_block->p_type = NULL;
}


var_t * var_begin(void)
{
    var_header_t * p_start = block_begin();
    if (block_is_var(p_start))
    {
        return (var_t *)p_start;
    }
    else
    {
        return var_next((var_t *)p_start);
    }
}


var_t * var_next(var_t * current_var)
{
    var_header_t * p_next = block_next((var_header_t *)current_var);
    while ((p_next != NULL) && !block_is_var(p_next))
    {
        p_next = block_next(p_next);
    }
    return (var_t *)p_next;
}


var_t * var_get(const char * p_name, bool verbose)
{
    var_t * p_var;

    // Iterate over all objects
    for (p_var = var_begin(); p_var != NULL; p_var = var_next(p_var))
    {
        if (strcmp(p_var->name, p_name) == 0)
        {
            return p_var;
        }
    }

    if (verbose)
    {
        nrf_cli_fprintf(cli_get(), NRF_CLI_WARNING, "Variable '%s' not found\n", p_name);
    }

    return NULL;
}


/** @brief Checks whether name of the variable is valid.
 * @param p_name   Name to check.
 * @param verbose  Print messages if name is invalid.
 * @return         true is name is valid.
 */
static bool var_name_validate(const char * p_name, bool verbose)
{
    size_t i;
    size_t len = strlen(p_name);
    if (p_name[0] != '$')
    {
        if (verbose)
        {
            nrf_cli_fprintf(cli_get(), NRF_CLI_WARNING, "Variable name must start with '$' sign\n");
        }
        return false;
    }
    else if (len <= 1)
    {
        if (verbose)
        {
            nrf_cli_fprintf(cli_get(), NRF_CLI_WARNING, "Variable name empty\n");
        }
        return false;
    }
    else if (len > VAR_NAME_LENGTH_MAX)
    {
        if (verbose)
        {
            nrf_cli_fprintf(cli_get(),
                            NRF_CLI_WARNING,
                            "Variable name too long, maximum %d, actual %d\n",
                            VAR_NAME_LENGTH_MAX,
                            len);
        }
        return false;
    }
    for (i = 0; i < len; i++)
    {
        if ((p_name[i] <= ' ') || (p_name[i] >= '\x7F'))
        {
            if (verbose)
            {
                nrf_cli_fprintf(cli_get(),
                                NRF_CLI_WARNING,
                                "Invalid character in variable name\n");
            }
            return false;
        }
    }
    return true;
}


var_t * var_create(const char * p_name, var_type_t const * p_type, size_t size, bool verbose)
{
    var_t * p_var;

    if (!var_name_validate(p_name, verbose))
    {
        return NULL;
    }

    p_var = var_get(p_name, false);
    if (p_var != NULL)
    {
        if (verbose)
        {
            nrf_cli_fprintf(cli_get(),
                            NRF_CLI_INFO,
                            "Variable '%s' already exists, overriding.\n",
                            p_name);
        }
        if (!var_delete(p_var->name, verbose))
        {
            return NULL;
        }
    }

    p_var = (var_t *)block_allocate(sizeof(var_t) + size, verbose);
    if (p_var == NULL)
    {
        return NULL;
    }
    p_var->data_size = size;
    strcpy(p_var->name, p_name);
    p_var->header.p_type = p_type;

    return p_var;
}


/** @brief Deletes variable by using pointer to the variable.
 *  @param p_var  Pointer to variable to delete.
 *  @return       false if variable free function prevents from its deletion.
 */
static bool var_delete_by_ptr(var_t * p_var)
{
    if (p_var->header.p_type->free_fn != NULL)
    {
        if (!p_var->header.p_type->free_fn(p_var))
        {
            return false;
        }
    }
    // Mark matching object as unused block
    p_var->header.p_type = NULL;
    return true;
}


bool var_delete(const char * p_name, bool verbose)
{
    var_t * p_var = var_get(p_name, verbose);
    if (p_var != NULL)
    {
        if (!var_delete_by_ptr(p_var))
        {
            if (verbose)
            {
                nrf_cli_fprintf(cli_get(), NRF_CLI_WARNING, "Cannot delete '%s'\n", p_name);
            }
            return false;
        }
        return true;
    }
    return false;
}


bool var_rename(const char * p_name, const char * p_new_name, bool verbose)
{
    var_t * p_dest_var;
    var_t * p_var = var_get(p_name, verbose);
    if (p_var == NULL)
    {
        return false;
    }
    if (!var_name_validate(p_new_name, verbose))
    {
        return false;
    }
    p_dest_var = var_get(p_new_name, false);
    if (p_dest_var != NULL)
    {
        if (verbose)
        {
            nrf_cli_fprintf(cli_get(),
                            NRF_CLI_WARNING,
                            "Variable '%s' already exists, overriding.\n",
                            p_new_name);
        }
        if (!var_delete(p_dest_var->name, verbose))
        {
            return false;
        }
    }
    strcpy(p_var->name, p_new_name);
    return true;
}


void var_show(var_t * p_var)
{
    nrf_cli_fprintf(cli_get(), NRF_CLI_NORMAL, "Name:            %s\n", p_var->name);
    nrf_cli_fprintf(cli_get(), NRF_CLI_NORMAL, "Description:     %s", p_var->header.p_type->p_description);
    if (p_var->header.p_type->print_fn)
    {
        nrf_cli_fprintf(cli_get(), NRF_CLI_NORMAL, ". ");
        p_var->header.p_type->print_fn(p_var, true);
    }
    nrf_cli_fprintf(cli_get(), NRF_CLI_NORMAL, "\n");
    nrf_cli_fprintf(cli_get(), NRF_CLI_NORMAL, "Allocated bytes: %d\n", p_var->header.total_size);
    nrf_cli_fprintf(cli_get(), NRF_CLI_NORMAL, "Used bytes:      %d\n", p_var->data_size);

    if (p_var->header.p_type->print_fn != NULL)
    {
        p_var->header.p_type->print_fn(p_var, false);
    }
}


void var_dynamic_cmd(nrf_cli_cmd_entry_t const * p_subcmd, size_t idx, nrf_cli_static_entry_t * p_static)
{
    var_t * p_var;

    // Iterate over all objects
    for (p_var = var_begin(); p_var != NULL; p_var = var_next(p_var))
    {
        if (idx == 0)
        {
            p_static->p_syntax = p_var->name;
            p_static->handler  = NULL;
            p_static->p_subcmd = p_subcmd;
            p_static->p_help   = NULL;
            return;
        }
        idx--;
    }

    p_static->p_syntax = NULL;
}


/* =============================================================================================
 *                         Temporary variable handling functions
 * =============================================================================================
 */


/** @brief Generates new name for the temporary variable.
 *  @return Pointer to the static buffer that contains the new name. It is valid until the next
 *          var_temp_name_generate call.
 */
static char * var_temp_name_generate()
{
    int i;
    static char name[] = TEMP_PREFIX "AAAAAAAAA";
    for (i = TEMP_PREFIX_LEN + 8; i > 0; i--)
    {
        if (name[i] == 'Z')
        {
            name[i] = 'A';
        }
        else
        {
            name[i]++;
            break;
        }
    }
    return name;
}


var_t * var_temp_create(var_type_t const * p_type, size_t size, bool verbose)
{
    return var_create(var_temp_name_generate(), p_type, size, verbose);
}


bool var_is_temp(var_t const * p_var)
{
    return strncmp(p_var->name, TEMP_PREFIX, TEMP_PREFIX_LEN) == 0;
}


void var_all_temp_delete(void)
{
    var_t * p_var;
    for (p_var = var_begin(); p_var != NULL; p_var = var_next(p_var))
    {
        if (strncmp(p_var->name, TEMP_PREFIX, TEMP_PREFIX_LEN) == 0)
        {
            UNUSED_RETURN_VALUE(var_delete(p_var->name, false));
        }
    }
}


/* =============================================================================================
 *                         Memory allocation inside variables memory
 * =============================================================================================
 */


void * var_malloc(size_t size)
{
    var_header_t * p_block = block_allocate(sizeof(var_header_t) + size, true);
    if (p_block != NULL)
    {
        p_block->p_type = &var_alloc;
    }
    return (void*)(&p_block[1]);

}


void * var_calloc(size_t count, size_t size)
{
    return var_malloc(count * size);
}


void var_free(void* p_data)
{
    var_header_t * p_block = &((var_header_t *)p_data)[-1];
    if (p_data != NULL)
    {
        p_block->p_type = NULL;
    }
}


void * var_realloc(void * p_old, size_t new_size)
{
    void * p_dest = var_malloc(new_size);
    var_free(p_old);
    if ((p_old != NULL) && (p_dest != NULL))
    {
        var_header_t * p_src_block = &((var_header_t *)p_old)[-1];
        size_t copy_bytes = p_src_block->total_size - sizeof(var_header_t);
        if (copy_bytes < new_size)
        {
            copy_bytes = new_size;
        }
        memcpy(p_dest, p_old, copy_bytes);
    }
    return p_dest;
}


void * var_temp_alloc(size_t size)
{
    var_t * p_temp_var;
    p_temp_var = var_temp_create(&g_raw_type, size, true);
    if (p_temp_var == NULL)
    {
        return NULL;
    }
    return VAR_DATA(p_temp_var);
}


/* =============================================================================================
 *                              Variable handling commands
 * =============================================================================================
 */


VAR_REGISTER_SUB_COMMANDS(m_subcmd_var, NULL);


/** @brief Command to list all variables. See command help for details.
 */
static bool cmd_ls(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    var_t * p_var;

    UNUSED_PARAMETER(argv);

    CLI_ARGS_CHECK(argc == 1);

    nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "%-16s %6s   %s\n", "Name", "Size", "Description");
    nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "%-16s %6s   %s\n", "----", "----", "-----------");

    // Iterate over all variables
    for (p_var = var_begin(); p_var != NULL; p_var = var_next(p_var))
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "%-16s %6d   %s. ", p_var->name,
            p_var->data_size, p_var->header.p_type->p_description);
        if (p_var->header.p_type->print_fn)
        {
            p_var->header.p_type->print_fn(p_var, true);
        }
        nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "\n");
    }

    return true;
}


CLI_CMD_REGISTER(ls, NULL, cmd_ls,
    "List all defined variables.\n"
    "Usage:\n"
    "  ls\n"
    );


/** @brief Command to remove a variable. See command help for details.
 */
static bool cmd_rm(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    UNUSED_PARAMETER(p_cli);

    CLI_ARGS_CHECK(argc == 2);

    if (strcmp(argv[1], "*") == 0)
    {
        var_t * p_next_var;
        var_t * p_var = var_begin();
        bool ok = true;

        while (p_var != NULL)
        {
            p_next_var = var_next(p_var);
            ok = var_delete(p_var->name, true) && ok;
            p_var = p_next_var;
        }

        return ok;
    }
    else
    {
        return var_delete(argv[1], true);
    }
}


CLI_CMD_REGISTER(rm, &m_subcmd_var, cmd_rm,
    "Remove a variable.\n"
    "Usage:\n"
    "  rm variable_name\n"
    "Arguments:\n"
    "  IN  variable_name   Name of the variable to remove or \"*\" to delete all.\n"
    );


/** @brief Command to rename a variable. See command help for details.
 */
static bool cmd_mv(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    var_t * p_var;

    UNUSED_PARAMETER(p_cli);

    CLI_ARGS_CHECK(argc == 3);

    p_var = cli_input_arg_get(1, NULL);
    CLI_CHECK(p_var != NULL);

    return var_rename(p_var->name, argv[2], true);
}


CLI_CMD_REGISTER(mv, &m_subcmd_var, cmd_mv,
    "Rename a variable.\n"
    "Usage:\n"
    "  mv old_name new_name\n"
    "Arguments:\n"
    "  IN  old_name   Name of the variable to rename.\n"
    "  IN  new_name   New name of the variable.\n"
    );


/** @brief Command to show a variable. See command help for details.
 */
static bool cmd_show(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    var_t * p_var;

    UNUSED_PARAMETER(p_cli);
    UNUSED_PARAMETER(argv);

    CLI_ARGS_CHECK(argc == 2);

    p_var = cli_input_arg_get(1, NULL);
    CLI_CHECK(p_var != NULL);

    var_show(p_var);

    return true;
}


CLI_CMD_REGISTER(show, &m_subcmd_var, cmd_show,
    "Show a content of a variable.\n"
    "Usage:\n"
    "  show variable_name\n"
    "Arguments:\n"
    "  IN  variable_name   Name of the variable to show.\n"
    );


/** @brief Command to show full content of variable memory space. See command help for details.
 */
static bool cmd_varmap(nrf_cli_t const * p_cli, size_t argc, char ** argv)
{
    var_header_t * p_block;

    UNUSED_PARAMETER(p_cli);

    CLI_ARGS_CHECK(argc == 1);

    UNUSED_RETURN_VALUE(block_allocate(APP_VAR_MEMORY_SIZE + 1, false)); // Force free block consolidation

    nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "%6s %6s %-16s %s\n", "Size", "Used", "Name",
        "Description");
    nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "%6s %6s %-16s %s\n", "----", "----", "----",
        "-----------");

    // Iterate over all blocks
    for (p_block = block_begin(); p_block != NULL; p_block = block_next(p_block))
    {
        if (p_block->p_type != NULL)
        {
            char const * name;
            size_t data_size;
            if (p_block->p_type == &var_alloc)
            {
                name = "[dynamic]";
                data_size = p_block->total_size - sizeof(var_header_t);
            }
            else
            {
                name = ((var_t *)p_block)->name;
                data_size = ((var_t *)p_block)->data_size;
            }
            nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "%6d %6d %-16s %s. ", p_block->total_size,
                data_size, name, p_block->p_type->p_description);
            if (p_block->p_type->print_fn)
            {
                p_block->p_type->print_fn((var_t *)p_block, true);
            }
            nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "\n");
        }
        else
        {
            nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "%6d %6s %-16s %s\n", p_block->total_size,
                "", "[FREE]", "Unused memory block");
        }
    }

    return true;
}


CLI_CMD_REGISTER(varmap, NULL, cmd_varmap,
    "Print full content of the variable memory.\n"
    "Usage:\n"
    "  varmap\n"
    );

