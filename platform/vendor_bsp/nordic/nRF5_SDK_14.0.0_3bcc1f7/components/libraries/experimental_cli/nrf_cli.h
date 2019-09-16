/**
 * Copyright (c) 2017 - 2017, Nordic Semiconductor ASA
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
#ifndef NRF_CLI_H__
#define NRF_CLI_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "sdk_common.h"
#include "nrf_cli_types.h"
#include "nrf_section.h"
#include "nrf_log_backend_interface.h"
#include "nrf_queue.h"
#include "nrf_log_ctrl.h"
#include "app_util_platform.h"

#if NRF_MODULE_ENABLED(NRF_CLI_USES_TASK_MANAGER)
#include "task_manager.h"
#endif

#include "nrf_fprintf.h"
#include "nrf_fprintf_format.h"

#define NRF_CLI_RX_BUFF_SIZE 16

/* CLI reserves top task manager flags, bits 0...18 are available for application */
#define NRF_CLI_TRANSPORT_TX_RDY_TASK_EVT      (1UL << 19)
#define NRF_CLI_TRANSPORT_RX_RDY_TASK_EVT      (1UL << 20)
#define NRF_CLI_LOG_PENDING_TASK_EVT           (1UL << 21)
#define NRF_CLI_CMD_EXECUTE_EVT                (1UL << 22)
#define NRF_CLI_KILL_TASK_EVT                  (1UL << 23)

#define NRF_CLI_TASK_EVTS (NRF_CLI_TRANSPORT_TX_RDY_TASK_EVT | \
                           NRF_CLI_TRANSPORT_RX_RDY_TASK_EVT | \
                           NRF_CLI_LOG_PENDING_TASK_EVT      | \
                           NRF_CLI_CMD_EXECUTE_EVT           | \
                           NRF_CLI_KILL_TASK_EVT)
/**
 * @defgroup nrf_cli Command Line Interface
 * @ingroup app_common
 *
 * @brief Module for unified command line handling.
 *
 * @{
 */

/**
 * @brief   Aliases to: @ref nrf_cli, @ref nrf_cli_cmd_entry and @ref nrf_cli_static_entry.
 *          Must be created here to satisfy module declaration order dependencies.
 */
typedef struct nrf_cli nrf_cli_t;
typedef struct nrf_cli_cmd_entry nrf_cli_cmd_entry_t;
typedef struct nrf_cli_static_entry nrf_cli_static_entry_t;

/**
 * @brief CLI dynamic command descriptor.
 *
 * @details Function shall fill received @ref nrf_cli_static_entry structure with requested (idx)
 *          dynamic subcommand data. If there is more than one dynamic subcommand available
 *          function shall ensure that returned commands: p_static->p_syntax are sorted in
 *          alphabetical order. If idx exceed available dynamic subcommands function must write
 *          to p_static->p_syntax NULL value. This will be the indication for CLI module that
 *          there are no more dynamic commands to read.
 **/
typedef void (*nrf_cli_dynamic_get)(size_t idx, nrf_cli_static_entry_t * p_static);

/**
 * @brief CLI command descriptor.
 **/
struct nrf_cli_cmd_entry
{
    bool is_dynamic;
    union
    {
        nrf_cli_dynamic_get p_dynamic_get;  //!< Pointer to function returning dynamic commands
        nrf_cli_static_entry_t const *  p_static;   //!< Pointer to array of static commands
    } u;
};

/**
 * @brief CLI command handler prototype.
 **/
typedef void (*nrf_cli_cmd_handler)(nrf_cli_t const * p_cli, size_t argc, char **argv);

/**
 * @brief CLI static command descriptor.
 **/
struct nrf_cli_static_entry
{
    char const * p_syntax;  //!< Command syntax strings.
    char const * p_help;    //!< Command help string.

    nrf_cli_cmd_entry_t const * p_subcmd;   //!< Pointer to subcommand

    nrf_cli_cmd_handler handler; //!< Command handler.
};

/**
 * @brief Macro for defining and adding root command (level 0).
 *
 * @note Each root command shall have unique syntax.
 *
 * @param[in] p_syntax  Command syntax (for example: history).
 * @param[in] p_subcmd  Poninter to subcommands array.
 * @param[in] p_help    Pointer to command help string.
 * @param[in] p_handler Pointer to function handler.
 * */
#define NRF_CLI_CMD_REGISTER(p_syntax, p_subcmd, p_help, p_handler)     \
    nrf_cli_static_entry_t const CONCAT_3(nrf_cli_, p_syntax, _raw) =   \
        NRF_CLI_CMD(p_syntax, p_subcmd, p_help, p_handler);             \
    NRF_SECTION_ITEM_REGISTER(cli_command,                              \
                              nrf_cli_cmd_entry_t const CONCAT_3(nrf_cli_, p_syntax, _const)) = { \
                                .is_dynamic = false,                    \
                                .u.p_static = &CONCAT_3(nrf_cli_, p_syntax, _raw) \
    }; \
    NRF_SECTION_ITEM_REGISTER(cli_sorted_cmd_ptrs, char const * CONCAT_2(p_syntax, _str_ptr))

/**
 * @brief Macro for creating a subcommand set. It must be used outside of any function body.
 *
 * @param[in] name  Name of the subcommand set.
 * */
#define NRF_CLI_CREATE_STATIC_SUBCMD_SET(name)                  \
    /*lint -save -e85 -e31*/                                    \
    static nrf_cli_static_entry_t const CONCAT_2(name, _raw)[]; \
    static nrf_cli_cmd_entry_t const name = {                   \
        .is_dynamic = false,                                    \
        .u.p_static = CONCAT_2(name, _raw)                      \
    };                                                          \
    static nrf_cli_static_entry_t const CONCAT_2(name, _raw)[] = /*lint -restore*/

/**
 * @brief Define ending subcommands set.
 *
 * */
#define NRF_CLI_SUBCMD_SET_END {NULL}

/**
 * @brief Macro for creating a dynamic entry.
 *
 * @param[in] name  Name of the dynamic entry.
 * @param[in] p_get Pointer to function returning dynamic commands array @ref nrf_cli_dynamic_get.
 */
#define NRF_CLI_CREATE_DYNAMIC_CMD(name, p_get) \
    /*lint -save -e19*/                         \
    static nrf_cli_cmd_entry_t const name = {   \
        .is_dynamic  = true,                    \
        .u.p_dynamic_get = p_get                \
}; /*lint -restore*/


/**
 * @brief Initializes CLI command (@ref nrf_cli_static_entry).
 *
 * @param[in] _p_syntax  Command syntax (for example: history).
 * @param[in] _p_subcmd  Poninter to subcommands array.
 * @param[in] _p_help    Pointer to command help string.
 * @param[in] _p_handler Pointer to function handler.
 **/
#define NRF_CLI_CMD(_p_syntax, _p_subcmd, _p_help, _p_handler) { \
    .p_syntax = (const char *)  STRINGIFY(_p_syntax), \
    .p_subcmd =                 _p_subcmd,  \
    .p_help  = (const char *)   _p_help,    \
    .handler =                  _p_handler  \
}

/**
 * @brief Internal CLI state in reposnse to data received from terminal.
 * */
typedef enum
{
    NRF_CLI_RECEIVE_DEFAULT,
    NRF_CLI_RECEIVE_ESC,
    NRF_CLI_RECEIVE_ESC_SEQ
} nrf_cli_receive_t;


/**
 * @brief Internal CLI state.
 * */
typedef enum
{
    NRF_CLI_STATE_UNINITIALIZED,      //!< State uninitialized.
    NRF_CLI_STATE_INITIALIZED,        //!< State initialized.
    NRF_CLI_STATE_COLLECT,            //!< State collect.
    NRF_CLI_STATE_EXECUTE,            //!< State execute.
    NRF_CLI_STATE_PANIC_MODE_ACTIVE,  //!< State panic mode activated.
    NRF_CLI_STATE_PANIC_MODE_INACTIVE //!< State panic mode requested but not supported.
} nrf_cli_state_t;

/**
 * @brief Event type from CLI transport.
 * */
typedef enum
{
    NRF_CLI_TRANSPORT_EVT_RX_RDY,
    NRF_CLI_TRANSPORT_EVT_TX_RDY
} nrf_cli_transport_evt_t;

typedef void (*nrf_cli_transport_handler_t)(nrf_cli_transport_evt_t evt_type, void * p_context);

typedef struct nrf_cli_transport_s nrf_cli_transport_t;

/**
 * @brief Unified CLI transport interface.
 * */
typedef struct
{
    /**
     * @brief Initializes CLI transport interface.
     *
     * @param[in] p_transport   Pointer to transfer instance.
     * @param[in] p_config      Pointer to instance configuration.
     * @param[in] evt_handler   Event handler.
     * @param[in] p_context     Pointer to context passed to event handler.
     *
     * @return Standard error code.
     * */
    ret_code_t (*init)(nrf_cli_transport_t const * p_transport,
                       void const *                p_config,
                       nrf_cli_transport_handler_t evt_handler,
                       void *                      p_context);

    /**
     * @brief Uninitialize CLI transport interface.
     *
     * @param[in] p_transport  Pointer to transfer instance.
     *
     * @return Standard error code.
     * */
    ret_code_t (*uninit)(nrf_cli_transport_t const * p_transport);

    /**
     * @brief Reconfigures transport to work in blocking mode.
     *
     * @param p_transport  Pointer to transfer instance.
     * @param blocking     If true transport is enabled in blocking mode.
     *
     * @return NRF_SUCCESS on successful enabling, error otherwise (also if not supported).
     */
    ret_code_t (*enable)(nrf_cli_transport_t const * p_transport,
                         bool                        blocking);

    /**
     * @brief Writes data to transport interface.
     *
     * @param[in] p_transport  Pointer to transfer instance.
     * @param[in] p_data       Pointer to source buffer.
     * @param[in] length       Source buffer length.
     * @param[in] p_cnt        Pointer to sent bytes counter.
     *
     * @return Standard error code.
     * */
    ret_code_t (*write)(nrf_cli_transport_t const * p_transport,
                        const void *                p_data,
                        size_t                      length,
                        size_t *                    p_cnt);

    /**
     * @brief Reads data from transport interface
     *
     * @param[in] p_transport  Pointer to transfer instance.
     * @param[in] p_data       Pointer to destination buffer.
     * @param[in] length       Destination buffer length.
     * @param[in] p_cnt        Pointer to received bytes counter.
     *
     * @return Standard error code.
     * */
    ret_code_t (*read)(nrf_cli_transport_t const * p_transport,
                       void *                      p_data,
                       size_t                      length,
                       size_t *                    p_cnt);

} nrf_cli_transport_api_t;

struct nrf_cli_transport_s
{
    nrf_cli_transport_api_t const * p_api;
};

/**
 * @brief CLI history object header
 * */
typedef PACKED_STRUCT
{
    nrf_memobj_t * p_prev;  //!< pointer to next object
    nrf_memobj_t * p_next;  //!< pointer to previous object
    nrf_cli_cmd_len_t cmd_len; //!< command length
} nrf_cli_memobj_header_t;

/**
 * @brief CLI instance context.
 * */
typedef struct
{
    nrf_cli_state_t   state;            //!< Internal module state.
    nrf_cli_receive_t receive_state;    //!< Escape sequence indicator.

    nrf_cli_static_entry_t const * p_current_stcmd; //!< Currently executed command.

    nrf_cli_vt100_ctx_t vt100_ctx;          //!< VT100 color and cursor position, terminal width.

    nrf_cli_cmd_len_t cmd_buff_len;         //!< Command length.
    nrf_cli_cmd_len_t cmd_buff_pos;         //!< Command buffer cursor position.

    char cmd_buff[NRF_CLI_CMD_BUFF_SIZE];       //!< Command input buffer.
    char temp_buff[NRF_CLI_CMD_BUFF_SIZE];      //!< Temporary buffer used by various functions.
    char printf_buff[NRF_CLI_PRINTF_BUFF_SIZE]; //!< Printf buffer size.

#if NRF_MODULE_ENABLED(NRF_CLI_USES_TASK_MANAGER)
    task_id_t     task_id;
#endif

#if NRF_MODULE_ENABLED(NRF_CLI_HISTORY)
    nrf_memobj_t * p_cmd_list_head;     //!< Pointer to head of history list
    nrf_memobj_t * p_cmd_list_tail;     //!< Pointer to tail of history list
    nrf_memobj_t * p_cmd_list_element;  //!< Pointer to element of history list
#endif

    volatile bool tx_rdy;

    uint8_t insert_mode : 1;    //!< Enables / disables console insert mode for text introduction
    uint8_t show_help   : 1;    //!< Shows help if command was called with -h or --help parameter
#if NRF_MODULE_ENABLED(NRF_CLI_VT100_COLORS)
    uint8_t use_colors  : 1;    //!< Enables / disables colored syntax
    uint8_t unused      : 5;
#else
    uint8_t unused      : 6;
#endif

} nrf_cli_ctx_t;

extern const nrf_log_backend_api_t nrf_log_backend_cli_api;

typedef struct
{
    nrf_log_backend_t   backend;
    nrf_queue_t const * p_queue;
    void *              p_context;
    nrf_cli_t const *   p_cli;
} nrf_cli_log_backend_t;

#if NRF_CLI_LOG_BACKEND && NRF_MODULE_ENABLED(NRF_LOG)
#define NRF_LOG_BACKEND_CLI_DEF(_name_, _queue_size_)                                       \
        NRF_QUEUE_DEF(nrf_log_entry_t,                                                      \
                      CONCAT_2(_name_, _queue),_queue_size_, NRF_QUEUE_MODE_NO_OVERFLOW);   \
        static nrf_cli_log_backend_t _name_ = {                                             \
                .backend = {.p_api = &nrf_log_backend_cli_api},                             \
                .p_queue = &CONCAT_2(_name_, _queue),                                       \
        }

#define NRF_CLI_BACKEND_PTR(_name_) &CONCAT_2(_name_, _log_backend)
#else
#define NRF_LOG_BACKEND_CLI_DEF(_name_, _queue_sz_)
#define NRF_CLI_BACKEND_PTR(_name_) NULL
#endif

#if NRF_MODULE_ENABLED(NRF_CLI_HISTORY)
/* Header consists memory for cmd length and pointer to: prev and next element */
#define NRF_CLI_HISTORY_HEADER_SIZE (sizeof(nrf_cli_memobj_header_t))

#define NRF_CLI_HISTORY_MEM_OBJ(name)                       \
    NRF_MEMOBJ_POOL_DEF(CONCAT_2(name, _cmd_hist_memobj),   \
                    NRF_CLI_HISTORY_HEADER_SIZE +           \
                    NRF_CLI_HISTORY_ELEMENT_SIZE,           \
                    NRF_CLI_HISTORY_ELEMENT_COUNT)

#define NRF_CLI_MEMOBJ_PTR(_name_) &CONCAT_2(_name_, _cmd_hist_memobj)
#else
#define NRF_CLI_MEMOBJ_PTR(_name_) NULL
#define NRF_CLI_HISTORY_MEM_OBJ(name)
#endif

/**
 * @brief CLI instance internals.
 *
 * @ref nrf_cli_t
 * */
struct nrf_cli
{
    char const * const p_name; //!< Terminal name.

    nrf_cli_transport_t const * p_iface;        //!< Transport interface.
    nrf_cli_ctx_t *             p_ctx;          //!< Internal context.
    nrf_cli_log_backend_t *     p_log_backend;  //!< Logger backend.
    nrf_fprintf_ctx_t *         p_fprintf_ctx;  //!< fprintf context.
    nrf_memobj_pool_t const *   p_cmd_hist_mempool; //!< Memory reserved for commands history.
    char const newline_char;   //!< New line character, only allowed values: \\n and \\r.
};

/**
 * @brief Macro for defining command line interface instance.
 *
 * @param[in] name              Instance name.
 * @param[in] cli_prefix        CLI prefix string.
 * @param[in] p_transport_iface Pointer to transport interface.
 * @param[in] newline_ch        New line character - only allowed values are '\\n' or '\\r'.
 * @param[in] log_queue_size    Logger processing queue size.
 * */
#define NRF_CLI_DEF(name, cli_prefix, p_transport_iface, newline_ch, log_queue_size)    \
        static nrf_cli_t const name;                                            \
        static nrf_cli_ctx_t CONCAT_2(name, _ctx);                              \
        NRF_FPRINTF_DEF(CONCAT_2(name, _fprintf_ctx),                           \
                        &name,                                                  \
                        CONCAT_2(name, _ctx).printf_buff,                       \
                        NRF_CLI_PRINTF_BUFF_SIZE,                               \
                        false,                                                  \
                        nrf_cli_print_stream);                                  \
        NRF_LOG_BACKEND_CLI_DEF(CONCAT_2(name, _log_backend), log_queue_size);  \
        NRF_CLI_HISTORY_MEM_OBJ(name);                                          \
        /*lint -save -e31*/                                                     \
        static nrf_cli_t const name = {                                         \
            .p_name = cli_prefix,                                               \
            .p_iface = p_transport_iface,                                       \
            .p_ctx = &CONCAT_2(name, _ctx),                                     \
            .p_log_backend = NRF_CLI_BACKEND_PTR(name),                         \
            .p_fprintf_ctx = &CONCAT_2(name, _fprintf_ctx),                     \
            .p_cmd_hist_mempool = NRF_CLI_MEMOBJ_PTR(name),                     \
            .newline_char = newline_ch                                          \
        } /*lint -restore*/

/**
 * @brief Initializes a transport layer and internal CLI state.
 *
 * @param[in] p_cli                 Pointer to CLI instance.
 * @param[in] p_transport_config    Configuration forwarded to the transport during initialization.
 * @param[in] use_colors            Enables colored prints.
 * @param[in] log_backend           If true console will be used as logger backend.
 * @param[in] init_lvl              Default severity level for the logger.
 *
 * @return Standard error code.
 * */
ret_code_t nrf_cli_init(nrf_cli_t const *   p_cli,
                        void const *        p_transport_config,
                        bool                use_colors,
                        bool                log_backend,
                        nrf_log_severity_t  init_lvl);

ret_code_t nrf_cli_task_create(nrf_cli_t const * p_cli);

/**
 * @brief Uninitializes a transport layer and internal CLI state.
 *
 * @param p_cli Pointer to CLI instance.
 *
 * @return Standard error code.
 * */
ret_code_t nrf_cli_uninit(nrf_cli_t const * p_cli);

/**
 * @brief Start CLI processing.
 *
 * @param p_cli Pointer to CLI instance.
 *
 * @return Standard error code.
 * */
ret_code_t nrf_cli_start(nrf_cli_t const * p_cli);

/**
 * @brief Stop CLI processing.
 *
 * @param p_cli Pointer to CLI instance.
 *
 * @return Standard error code.
 * */
ret_code_t nrf_cli_stop(nrf_cli_t const * p_cli);

#define NRF_CLI_DEFAULT  NRF_CLI_VT100_COLOR_DEFAULT    /**< Turn off character attributes  */
#define NRF_CLI_NORMAL   NRF_CLI_VT100_COLOR_WHITE      /**< Normal color printf.           */
#define NRF_CLI_INFO     NRF_CLI_VT100_COLOR_GREEN      /**< Info color printf.             */
#define NRF_CLI_OPTION   NRF_CLI_VT100_COLOR_CYAN       /**< Option color printf.           */
#define NRF_CLI_WARNING  NRF_CLI_VT100_COLOR_YELLOW     /**< Warning color printf.          */
#define NRF_CLI_ERROR    NRF_CLI_VT100_COLOR_RED        /**< Error color printf.            */

/**
 * @brief   Printf like function, which send formated data stream to the CLI.
 *          This function shall not be used outside of CLI module or CLI command context.
 *
 * @param[in] p_cli Pointer to CLI instance.
 * @param[in] color Printf color.
 * @param[in] p_fmt Format string.
 * @param[in] ...   List of parameters to print.
 * */
void nrf_cli_fprintf(nrf_cli_t const *      p_cli,
                     nrf_cli_vt100_color_t  color,
                     char const *           p_fmt,
                                            ...);

/**
 * @brief Process function, which should be executed when data is ready
 *        in transport interface.
 *
 * @param[in] p_cli Pointer to CLI instance.
 * */
void nrf_cli_process(nrf_cli_t const * p_cli);


/**
 * @brief Option descriptor.
 * */
typedef struct nrf_cli_getopt_option
{
    char const * p_optname;         //!< Option long name.
    char const * p_optname_short;   //!< Option short name.
    char const * p_optname_help;    //!< Option help string.
} nrf_cli_getopt_option_t;


/**
 * @brief Option structure initializer @ref nrf_cli_getopt_option.
 *
 * @param[in] _p_optname    Option name long.
 * @param[in] _p_shortname  Option name short.
 * @param[in] _p_help       Option help string.
 * */
#define NRF_CLI_OPT(_p_optname, _p_shortname, _p_help) { \
        .p_optname       = _p_optname,   \
        .p_optname_short = _p_shortname, \
        .p_optname_help  = _p_help,      \
}

/**
 * @brief Informs that command has been called with -h or --help option.
 *
 * @param[in] p_cli Pointer to CLI instance.
 *
 * @return          True if help has been requested.
 * */
__STATIC_INLINE bool nrf_cli_help_requested(nrf_cli_t const * p_cli)
{
    return p_cli->p_ctx->show_help;
}

/**
 * @brief       Prints actual command help if user called it with -h or --help option.
 * @details     Function will print help string of: currently entered command, its options
 *              and subcommands (if exists).
 *
 * @param[in] p_cli     Pointer to CLI instance.
 * @param[in] p_opt     Pointer to optional option array.
 * @param[in] opt_len   Option array size.
 * */
void nrf_cli_help_print(nrf_cli_t const *               p_cli,
                        nrf_cli_getopt_option_t const * p_opt,
                        size_t                          opt_len);

/**
 * @brief Function shall not be used directly, it is required by nrf_fprintf module.
 *
 * @param[in] p_user_ctx    Pointer to context for CLI instance.
 * @param[in] p_data        Pointer to data buffer.
 * @param[in] data_len      Data buffer size.
 * */
void nrf_cli_print_stream(void const * p_user_ctx, char const * p_data, size_t data_len);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* NRF_CLI_H__ */
