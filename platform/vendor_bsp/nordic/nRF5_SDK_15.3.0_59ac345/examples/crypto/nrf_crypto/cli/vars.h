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
#ifndef VARS_H__
#define VARS_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "nrf_cli.h"


#define VAR_NAME_LENGTH_MAX 15  ///< Maximum length of CLI variable including '$' character and not including NULL character


/** @brief Create dynamic command listing all existing CLI variables.
 *  @param name  Name of the dynamic command.
 *  @param next  Next subcommand after this command or NULL.
 */
#define VAR_REGISTER_SUB_COMMANDS(name, next) \
    static void _##name##_get(size_t idx, nrf_cli_static_entry_t * p_static) \
    { \
        var_dynamic_cmd((next), idx, p_static); \
    } \
    NRF_CLI_CREATE_DYNAMIC_CMD(name, _##name##_get)


/** @brief Contains header of the variable.
 *
 *  Some blocks inside variable memory space contain only headers, e.g. dynamically allocated memory
 *  by @ref var_malloc. Data size and name are skipped to save memory.
 */
typedef struct var_header_s
{
    size_t                    total_size;  ///< Total size of the block that contains this variable including this header.
    struct var_type_s const * p_type;      ///< Pointer to type definition of this variable.
} var_header_t;


/** @brief Structure holding variable.
 *
 *  It has to by located at the variables memory space. Immediately after this structure content
 *  data is located and it can be retried by @ref VAR_DATA macro.
 */
typedef struct var_s
{
    var_header_t header;                        ///< Variable header.
    size_t       data_size;                     ///< Actual content size that can be smaller than allocated space.
    char         name[VAR_NAME_LENGTH_MAX + 1]; ///< Name of the variable.
} var_t;


/** @brief Macro to get data of the CLI variable
 */
#define VAR_DATA(p_var) ((uint8_t *)(&((var_t *)(p_var))[1]))


/** @brief Callback function printing content of the variable of the specific type.
 *  @param p_var       Variable to print.
 *  @param short_info  true if printed text must be short (brief); no new lines. The text appears after the variable description.
 *                     false if printed text can be long (full content); multiple lines are allowed.
 */
typedef void (*var_print_fn_t)(var_t * p_var, bool short_info);


/** @brief Callback function for deallocating variables of the specific type.
 *  @param p_var  Variable to deallocate.
 *  @return       false to prevent variable to be removed, true otherwise.
 */
typedef bool (*var_free_fn_t)(var_t * p_var);


/** @brief Structure holding information regarding the type of variable.
 */
typedef struct var_type_s
{
    char           const * p_description; ///< Short description of the type.
    var_free_fn_t          free_fn;       ///< Callback function that is to be called when variable is about to be deleted.
    var_print_fn_t         print_fn;      ///< Callback function that is to be called to print information about content.
    void *                 p_tag;         ///< Transparent pointer that can be used by callback functions.
} var_type_t;


/** @brief Initialize CLI variable memory space.
 */
void var_init(void);

/** @brief Starts iterator over all CLI variables.
 *  @return  Pointer to the first variable, or NULL if there are no variables.
 */
var_t * var_begin(void);

/** @brief Go to next defined CLI variable.
 *  @param current_var  Pointer to the current CLI variable.
 *  @return             Pointer to the next variable, or NULL if there are no more variables.
 */
var_t * var_next(var_t * current_var);

/** @brief Finds a variable of the specified name in the CLI variable memory space.
 *  @param p_name   Name of the variable to search for (including '$' character).
 *  @param verbose  Print messages in case of problems if true.
 *  @return         Pointer to variable or NULL if not found.
 */
var_t * var_get(const char * p_name, bool verbose);

/** @brief Deletes a variable of the specified name from the CLI variable memory space.
 *  @param p_name   Name of the variable to search for (including '$' character).
 *  @param verbose  Print messages in case of problems if true.
 *  @return         true if variable was deleted successfully, false otherwise.
 */
bool var_delete(const char * p_name, bool verbose);

/** @brief Renames a variable of the specified name.
 *  @param p_name     Name of the variable to search for (including '$' character).
 *  @param p_new_name New name of the variable (including '$' character).
 *  @param verbose    Print messages in case of problems if true.
 *  @return           true if variable was renamed successfully, false otherwise.
 */
bool var_rename(const char * p_name, const char * p_new_name, bool verbose);

/** @brief Creates new variable.
 *  @param p_name   Name of the new variable.
 *  @param p_type   Pointer to structure thet defines the type of the variable.
 *  @param size     Initial content data size. It is an initial value and it can be changed later to
 *                  a smaller one, but not bigger than the initial value.
 *  @param verbose  Print messages in case of problems if true.
 *  @return         Pointer to the newly created variable with zeroed content.
 */
var_t * var_create(const char * p_name, var_type_t const * p_type, size_t size, bool verbose);

/** @brief Creates a new temporal variable.
 *
 *  Name of the variable is automatically generated and is indicated as temporary.
 *  Normally, this kind of variables are deleted after exit from command by @ref  var_all_temp_delete.
 *  It is possible to rename variable to make it not temporary.
 *
 *  @param p_type   Pointer to structure that defines the type of the variable.
 *  @param size     Initial content data size. It is an initial value and it can be changed later to
 *                  a smaller one, but not bigger than the initial value.
 *  @param verbose  Print messages in case of problems if true.
 *  @return         Pointer to newly created variable with zeroed content.
 */
var_t * var_temp_create(var_type_t const * p_type, size_t size, bool verbose);

/** @brief Deletes all variables with the name indicated as temporary.
 */
void var_all_temp_delete(void);

/** @brief Checks if variable is temporary.
 *  @param p_var  Variable to check.
 *  @return       true if variable is temporary.
 */
bool var_is_temp(var_t const * p_var);

/** @brief Prints variable on terminal.
 *  @param p_var  Variable to print.
 */
void var_show(var_t * p_var);


/** @brief Allocates memory block inside the variable memory space.
 *
 *  The variable memory space can be also used to handle dynamic memory allocation. In this case, the memory blocks
 *  are reduced to save memory (@see var_header_t), and they only have total size and type.
 *
 *  @param size  Number of bytes to allocate.
 *  @return      Pointer to the newly allocated data, or NULL if there is no free block big enough to
 *               fit requested size.
 */
void * var_malloc(size_t size);

/** @brief Chage size of the allocated block.
 *  @param p_old     Pointer to the old memory block.
 *  @param new_size  New size.
 *  @return          Pointer to the new memory block.
 */
void * var_realloc(void * p_old, size_t new_size);

/** @brief Memory allocation in calloc style. @see var_malloc.
 */
void * var_calloc(size_t count, size_t size);

/** @brief Dynamic memory free.
 *  @param p_data  Pointer to free.
 */
void var_free(void* p_data);

/** @brief Shortcut to create a temporary variable just to use it as a temporary buffer.
 *
 *  Returned pointer does not have to be deallocated, because this will be done by @ref var_all_temp_delete.
 *  @param size  Number of bytes to allocate.
 */
void * var_temp_alloc(size_t size);


/** @brief Function that generates dynamic command that lists all variables used by the @ref VAR_REGISTER_SUB_COMMANDS macro.
 */
void var_dynamic_cmd(nrf_cli_cmd_entry_t const * p_subcmd, size_t idx, nrf_cli_static_entry_t * p_static);


#endif // VARS_H__
