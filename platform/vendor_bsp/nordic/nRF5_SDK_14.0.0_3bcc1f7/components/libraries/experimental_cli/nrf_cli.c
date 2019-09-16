/**
 * Copyright (c) 2016 - 2017, Nordic Semiconductor ASA
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
#include "sdk_common.h"
#if NRF_MODULE_ENABLED(NRF_CLI)
#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>
#include "nrf_cli.h"
#include "nrf_cli_vt100.h"
#include "app_error.h"
#include "nrf_assert.h"
#include "nrf_delay.h"
#include "nrf_pwr_mgmt.h"

#if NRF_MODULE_ENABLED(NRF_CLI_HISTORY)
    #if NRF_CLI_HISTORY_ELEMENT_SIZE * NRF_CLI_HISTORY_ELEMENT_COUNT == 0
        #error Not proper memory size allocated for NRF_CLI_HISTORY
    #endif
#endif

/* 2 == 1 char for cmd + 1 char for '\0' */
#if NRF_CLI_CMD_BUFF_SIZE < 2
    #error too small NRF_CLI_CMD_BUFF_SIZE
#endif

#if NRF_CLI_PRINTF_BUFF_SIZE < 1
    #error too small NRF_CLI_PRINTF_BUFF_SIZE
#endif

/*lint -save -esym(526,cli_command*) -esym(526,cli_sorted_cmd_ptrs*)*/
NRF_SECTION_DEF(cli_command, nrf_cli_cmd_entry_t);
#define CLI_DATA_SECTION_ITEM_GET(i) NRF_SECTION_ITEM_GET(cli_command, nrf_cli_cmd_entry_t, (i))
#define CLI_DATA_SECTION_ITEM_COUNT  NRF_SECTION_ITEM_COUNT(cli_command, nrf_cli_cmd_entry_t)

NRF_SECTION_DEF(cli_sorted_cmd_ptrs, const char *);
/*lint -restore*/
#define CLI_SORTED_CMD_PTRS_ITEM_GET(i) NRF_SECTION_ITEM_GET(cli_sorted_cmd_ptrs, const char *, (i))
#define CLI_SORTED_CMD_PTRS_START_ADDR_GET NRF_SECTION_START_ADDR(cli_sorted_cmd_ptrs)

#if defined(NRF_CLI_LOG_BACKEND) && NRF_CLI_LOG_BACKEND
#include "nrf_log_str_formatter.h"
#include "nrf_log_internal.h"
#endif

#define NRF_CLI_IO_BUFFER_AUTO_FLUSH_ENABLE(_p_cli)     (_p_cli->p_fprintf_ctx->auto_flush = true)
#define NRF_CLI_IO_BUFFER_AUTO_FLUSH_DISABLE(_p_cli)    (_p_cli->p_fprintf_ctx->auto_flush = false)
#define NRF_CLI_IO_BUFFER_FLUSH(_p_cli)     nrf_fprintf_buffer_flush(_p_cli->p_fprintf_ctx)

#define NRF_CLI_HELP_FLAG_SET(_p_cli)   (_p_cli->p_ctx->show_help = true)
#define NRF_CLI_HELP_FLAG_CLEAR(_p_cli) (_p_cli->p_ctx->show_help = false)

/**
 * @brief Maximum commands per line after tab key press.
 * */
#define NRF_CLI_RECIEVE_STATE_NEXT(_p_cli, _state)  (_p_cli->p_ctx->receive_state = _state)
#define NRF_CLI_INIT_OPTION_PRINTER     (NULL)

#define NRF_CLI_MAX_TERMINAL_SIZE       (250u)
#define NRF_CLI_CURSOR_POSITION_BUFFER  (10u)   /* 10 == {esc, [, 2, 5, 0, ;, 2, 5, 0, '\0'} */
#define NRF_CLI_DEFAULT_TERMINAL_WIDTH  (80u)
#define NRF_CLI_DEFAULT_TERMINAL_HEIGHT (24u)
#define NRF_CLI_INITIAL_CURS_POS        (1u)    /* initial cursor position is (1, 1) */

#define NRF_CLI_CMD_ROOT_LVL            (0u)

/* Macro to send VT100 commands */
#define NRF_CLI_VT100_CMD(_p_cli_, _cmd_)   {       \
    ASSERT(_p_cli_);                                \
    ASSERT(_p_cli_->p_fprintf_ctx);                 \
    static char const cmd[] = _cmd_;                \
    nrf_fprintf(_p_cli_->p_fprintf_ctx, "%s", cmd); \
}

#if NRF_MODULE_ENABLED(NRF_CLI_BUILD_IN_CMDS)
static char const m_nrf_cli_bad_param_count[] = " bad parameter count\r\n";
static char const m_nrf_cli_uknown_parameter[] = " unknown parameter: ";
#endif
static char const m_nrf_cli_command_not_found[] = ": command not found";

static bool cli_log_entry_process(nrf_cli_t const * p_cli, bool skip);
static void cli_state_execute(nrf_cli_t const * p_cli);

static inline size_t cli_strlen(char const * str)
{
    return str == NULL ? 0 : strlen(str);
}

/* function returns true if cursor is at beginning of an empty line */
static inline bool cursor_in_empty_line(nrf_cli_t const * p_cli)
{
    return ( (p_cli->p_ctx->cmd_buff_pos + cli_strlen(p_cli->p_name)) %
              p_cli->p_ctx->vt100_ctx.cons.terminal_wid == 0);
}

/* function returns true if command length is equal to multiplicity of terminal width */
static inline bool full_line_cmd(nrf_cli_t const * p_cli)
{
    return ((p_cli->p_ctx->cmd_buff_len + cli_strlen(p_cli->p_name)) %
            p_cli->p_ctx->vt100_ctx.cons.terminal_wid == 0);
}

/* function sends data stream to cli instance. Each time before this function is call fprintf
 * IO buffer needs to be flushed to avoid synchronization issues. For that purpose macro:
 * NRF_CLI_IO_BUFFER_FLUSH shall be used */
static void cli_write(nrf_cli_t const * p_cli,
                      void const *      p_data,
                      size_t            length,
                      size_t *          p_cnt)
{
    ASSERT(p_cli && p_data);
    ASSERT(p_cli->p_iface->p_api);
    size_t offset = 0;
    size_t cnt;
    while (length)
    {
        ret_code_t ret = p_cli->p_iface->p_api->write(p_cli->p_iface,
                                                      &((uint8_t const *)p_data)[offset],
                                                      length,
                                                      &cnt);
        UNUSED_VARIABLE(ret);
        ASSERT(ret == NRF_SUCCESS);
        ASSERT(length >= cnt);
        offset += cnt;
        length -= cnt;
        if (cnt == 0 && (p_cli->p_ctx->state != NRF_CLI_STATE_PANIC_MODE_ACTIVE))
        {
#if NRF_MODULE_ENABLED(NRF_CLI_USES_TASK_MANAGER)
            (void)task_events_wait(NRF_CLI_TRANSPORT_TX_RDY_TASK_EVT);
#else
            while (p_cli->p_ctx->tx_rdy == false)
            {
                ;
            }
            p_cli->p_ctx->tx_rdy = false;
#endif
        }
    }

    if (p_cnt)
    {
        *p_cnt = cnt;
    }
}

/* function sends 1 char to cli instance */
static inline void cli_putc(nrf_cli_t const * p_cli, char ch)
{
    nrf_fprintf(p_cli->p_fprintf_ctx, "%c", ch);
}

/* function reads data from cli instance */
static void cli_read(nrf_cli_t const * p_cli,
                     void *            p_data,
                     size_t            length,
                     size_t *          p_cnt)
{
    ASSERT(p_cli && p_data);
    ASSERT(p_cli->p_iface);

    ret_code_t ret = p_cli->p_iface->p_api->read(p_cli->p_iface, p_data, length, p_cnt);
    UNUSED_VARIABLE(ret);
}

/* Function cmd_get shall be used to search commands. It moves pointer pp_entry to command
 * of static command structure. If command cannot be found function will set pp_entry to NULL.
 *  p_command   - pointer to command which will be processed (no matter for root command)
 *  lvl         - level of requested command
 *  idx         - index of requested command
 *  pp_entry    - pointer which will point to subcommand[idx] after function execution
 *  p_st_entry  - pointer to structure where dynamic entry data can be stored
 */
static void cmd_get(nrf_cli_cmd_entry_t const *     p_command,
                    size_t                          lvl,
                    size_t                          idx,
                    nrf_cli_static_entry_t const ** pp_entry,
                    nrf_cli_static_entry_t *        p_st_entry)
{
    ASSERT (pp_entry != NULL);
    ASSERT (p_st_entry != NULL);

    if (lvl == NRF_CLI_CMD_ROOT_LVL)
    {
        if (idx < CLI_DATA_SECTION_ITEM_COUNT)
        {
            nrf_cli_cmd_entry_t const * p_cmd = NULL;
            char const * * pp_sorted_cmds = (char const * *)CLI_SORTED_CMD_PTRS_START_ADDR_GET;
            for (size_t i = 0; i < CLI_DATA_SECTION_ITEM_COUNT; i++)
            {
                p_cmd = CLI_DATA_SECTION_ITEM_GET(i);
                if (!strcmp(pp_sorted_cmds[idx], p_cmd->u.p_static->p_syntax))
                {
                    *pp_entry = p_cmd->u.p_static;
                    return;
                }
            }
        }
        *pp_entry = NULL;
        return;
    }

    if (p_command == NULL)
    {
        *pp_entry = NULL;
        return;
    }

    if (p_command->is_dynamic)
    {
        p_command->u.p_dynamic_get(idx, p_st_entry);
        if (p_st_entry->p_syntax == NULL)
        {
            *pp_entry = NULL;
        }
        else
        {
            *pp_entry = p_st_entry;
        }
    }
    else
    {
        if (p_command->u.p_static[idx].p_syntax != NULL)
        {
            *pp_entry = &p_command->u.p_static[idx];
        }
        else
        {
            *pp_entry = NULL;
        }
    }
}

/* function multiline_console_data_check checks current cursor position (x, y) on terminal screen
 * basing on: command length, console name length and terminal width.
 * Example 1:
 * || - cursor
 *  ----------------------------
 * |console_name $: ||          |
 *  ----------------------------
 * => coordinates are:  cur_x = 17, cur_x_end = 17,
 *                      cur_y = 1,  cur_y_end = 1
 * Example 2:
 *  ----------------------------
 * |console_name $: test command|
 * |showing |e|xample           |
 *  ----------------------------
 * => coordinates are:  cur_x = 9, cur_x_end = 18 (cursor can be one column after 'e')
 * =>                   cur_y = 2, cur_y_end = 2
 * Example 3:
 *  ----------------------------
 * |console_name $: test command|
 * |showing e|x|ample with more |
 * |parameters                  |
 *  ----------------------------
 * => coordinates are:  cur_x = 10, cur_x_end = 11 (cursor can be one column after 's')
 * =>                   cur_y = 2, cur_y_end = 3
 */
static nrf_cli_multiline_cons_t const * multiline_console_data_check(nrf_cli_t const * p_cli)
{
    nrf_cli_ctx_t * p_ctx = p_cli->p_ctx;
    nrf_cli_multiline_cons_t * cons = &p_cli->p_ctx->vt100_ctx.cons;

    cons->name_len = cli_strlen(p_cli->p_name);

    /* current cursor position in command
     * +1 -> because home position is (1, 1) */
    cons->cur_x = (p_ctx->cmd_buff_pos + cons->name_len) % cons->terminal_wid + 1;
    cons->cur_y = (p_ctx->cmd_buff_pos + cons->name_len) / cons->terminal_wid + 1;

    /* extreme postion when cursor is at the end of command */
    cons->cur_y_end = (p_ctx->cmd_buff_len + cons->name_len) / cons->terminal_wid + 1;
    cons->cur_x_end = (p_ctx->cmd_buff_len + cons->name_len) % cons->terminal_wid + 1;

    return cons;
}

/* function sends VT100 command to clear the screen from cursor position to end of the screen */
static inline void cli_clear_eos(nrf_cli_t const * p_cli)
{
    NRF_CLI_VT100_CMD(p_cli, NRF_CLI_VT100_CLEAREOS);
}

/* function sends VT100 command to save cursor position */
static inline void cli_cursor_save(nrf_cli_t const * p_cli)
{
    NRF_CLI_VT100_CMD(p_cli, NRF_CLI_VT100_SAVECURSOR);
}

/* function sends VT100 command to restore saved cursor position */
static inline void cli_cursor_restore(nrf_cli_t const * p_cli)
{
    NRF_CLI_VT100_CMD(p_cli, NRF_CLI_VT100_RESTORECURSOR);
}

/* function forcing new line - cannot be replaced with function cursor_down_move */
static inline void cursor_next_line_move(nrf_cli_t const * p_cli)
{
    NRF_CLI_VT100_CMD(p_cli, NRF_CLI_VT100_NEXTLINE);
}

/* function moves cursor left by n positions */
static inline void cursor_left_move(nrf_cli_t const * p_cli, nrf_cli_cmd_len_t n)
{
    if (n > 0)
    {
        nrf_fprintf(p_cli->p_fprintf_ctx, "\033[%dD", n);
    }
}

/* function moves cursor right by n positions */
static inline void cursor_right_move(nrf_cli_t const * p_cli, nrf_cli_cmd_len_t n)
{
    if (n > 0)
    {
        nrf_fprintf(p_cli->p_fprintf_ctx, "\033[%dC", n);
    }
}

/* function moves cursor up by n positions */
static inline void cursor_up_move(nrf_cli_t const * p_cli, nrf_cli_cmd_len_t n)
{
    if (n > 0)
    {
        nrf_fprintf(p_cli->p_fprintf_ctx, "\033[%dA", n);
    }
}

/* function moves cursor down by n positions but it will bring no effect if cursor is in the last
 * line of terminal screen. In such case cursor_next_line_move function shall be invoked. */
static inline void cursor_down_move(nrf_cli_t const * p_cli, nrf_cli_cmd_len_t n)
{
    if (n > 0)
    {
         nrf_fprintf(p_cli->p_fprintf_ctx, "\033[%dB", n);
    }
}

/* function increments cursor position (if possible) and moves cursor to new line if necessary */
static void cursor_position_increment(nrf_cli_t const * p_cli)
{
    if (p_cli->p_ctx->cmd_buff_pos >= p_cli->p_ctx->cmd_buff_len)
    {
        return; /* incrementation not possible */
    }

    nrf_cli_multiline_cons_t const * cons = multiline_console_data_check(p_cli);
    nrf_cli_cmd_len_t cur_y = cons->cur_y;
    ++p_cli->p_ctx->cmd_buff_pos;
    cons = multiline_console_data_check(p_cli);

    if (cur_y == cons->cur_y)
    {
        cursor_right_move(p_cli, 1);
    }
    else
    {
        cursor_next_line_move(p_cli);
    }
}

/* function will move cursor back to position == cmd_buff_pos. Example usage is when cursor needs
 * to be moved back after printing some text. This function cannot be used to move cursor to new
 * location by manual change of cmd_buff_pos.*/
static void cursor_position_synchronize(nrf_cli_t const * p_cli)
{
    nrf_cli_multiline_cons_t const * cons = multiline_console_data_check(p_cli);
    bool last_line = cons->cur_y == cons->cur_y_end ? true : false;

    /* This cmd is needed in case cursor reaches bottom line of terminal to scroll one line down */
    if (cursor_in_empty_line(p_cli) || full_line_cmd(p_cli))
    {
        cursor_next_line_move(p_cli);
    }

    if (last_line)
    {
        cursor_left_move(p_cli, cons->cur_x_end - cons->cur_x);
    }
    else
    {
        cursor_up_move(p_cli, cons->cur_y_end - cons->cur_y);
        if (cons->cur_x > cons->cur_x_end)
        {
            cursor_right_move(p_cli, cons->cur_x - cons->cur_x_end);
        }
        else
        {
            cursor_left_move(p_cli, cons->cur_x_end - cons->cur_x);
        }
    }
}

/* function moves cursor to begin of command position, just after console name */
static void cursor_home_position_move(nrf_cli_t const * p_cli)
{
    nrf_cli_multiline_cons_t const * cons = multiline_console_data_check(p_cli);

    if ((cons->cur_x == cons->name_len + NRF_CLI_INITIAL_CURS_POS) &&
        (cons->cur_y == NRF_CLI_INITIAL_CURS_POS))
    {
        return; /* nothing to handle because cursor is in start position */
    }

    if (cons->cur_y > NRF_CLI_INITIAL_CURS_POS)
    {
        cursor_up_move(p_cli, cons->cur_y - NRF_CLI_INITIAL_CURS_POS);
    }

    if (cons->cur_x > cons->name_len)
    {
        cursor_left_move(p_cli, cons->cur_x - NRF_CLI_INITIAL_CURS_POS - cons->name_len);
    }
    else
    {
        cursor_right_move(p_cli, cons->name_len + NRF_CLI_INITIAL_CURS_POS - cons->cur_x);
    }
    /* allign data buffer pointer with cursor postion */
    p_cli->p_ctx->cmd_buff_pos = 0;
}

/* function moves cursor to end of command */
static void cursor_end_position_move(nrf_cli_t const * p_cli)
{
    nrf_cli_multiline_cons_t const * cons = multiline_console_data_check(p_cli);

    if ((cons->cur_x == cons->cur_x_end) && (cons->cur_y == cons->cur_y_end))
    {
        return; /* nothing to handle because cursor is in end position */
    }

    if (cons->cur_y_end > cons->cur_y)
    {
        cursor_down_move(p_cli, cons->cur_y_end - cons->cur_y);
    }

    if (cons->cur_x > cons->cur_x_end)
    {
        cursor_left_move(p_cli, cons->cur_x - cons->cur_x_end);
    }
    else
    {
        cursor_right_move(p_cli, cons->cur_x_end - cons->cur_x);
    }
    /* allign data buffer pointer with cursor postion */
    p_cli->p_ctx->cmd_buff_pos = p_cli->p_ctx->cmd_buff_len;
}

#if NRF_MODULE_ENABLED(NRF_CLI_BUILD_IN_CMDS)
/* function reads cursor position from terminal. */
static ret_code_t cursor_position_get(nrf_cli_t const * p_cli)
{
    size_t cnt;
    uint16_t x = 0; /* horizontal position */
    uint16_t y = 0; /* vertical position */
    uint8_t buff_idx = 0;
    char c = 0;

    /* clear temp buffer */
    memset(p_cli->p_ctx->temp_buff, 0, sizeof(p_cli->p_ctx->temp_buff));

    /* escape code asking terminal about its size */
    static char const cmd_get_terminal_size[] = "\033[6n";

    nrf_fprintf(p_cli->p_fprintf_ctx, cmd_get_terminal_size);
    /* fprintf buffer needs to be flushed to start sending prepared escape code to the terminal */
    NRF_CLI_IO_BUFFER_FLUSH(p_cli);

    /* timeout for terminal response = ~1s */
    for (uint16_t i = 0; i < 1000; i++)
    {
        do
        {
            cli_read(p_cli, &c, sizeof(c), &cnt);
            if (cnt == 0)
            {
                nrf_delay_us(999);
                continue;
            }
            if ((c != NRF_CLI_VT100_ASCII_ESC) &&
                (p_cli->p_ctx->temp_buff[0] != NRF_CLI_VT100_ASCII_ESC))
            {
                continue;
            }

            if (c == 'R') /* end of response from the terminal */
            {
                p_cli->p_ctx->temp_buff[buff_idx] = '\0';
                if (p_cli->p_ctx->temp_buff[1] != '[')
                {
                    p_cli->p_ctx->temp_buff[0] = 0;
                    return NRF_ERROR_INVALID_DATA;
                }
                buff_idx = 2;  /* index start position in the buffer where 'y' is stored */
                while (p_cli->p_ctx->temp_buff[buff_idx] != ';')
                {
                    y = y * 10 + (p_cli->p_ctx->temp_buff[buff_idx++] - '0');
                    if (buff_idx >= NRF_CLI_CMD_BUFF_SIZE)
                    {
                        return NRF_ERROR_DATA_SIZE;
                    }
                }
                if (++buff_idx >= NRF_CLI_CMD_BUFF_SIZE)
                {
                    return NRF_ERROR_DATA_SIZE;
                }
                while (p_cli->p_ctx->temp_buff[buff_idx] != '\0')
                {
                    x = x * 10 + (p_cli->p_ctx->temp_buff[buff_idx++] - '0');
                    if (buff_idx >= NRF_CLI_CMD_BUFF_SIZE)
                    {
                        return NRF_ERROR_DATA_SIZE;
                    }
                }
                /* horizontal cursor position */
                if (x > NRF_CLI_MAX_TERMINAL_SIZE)
                {
                    p_cli->p_ctx->vt100_ctx.cons.cur_x = NRF_CLI_MAX_TERMINAL_SIZE;
                }
                else
                {
                    /* horizontal cursor position */
                    p_cli->p_ctx->vt100_ctx.cons.cur_x = (nrf_cli_cmd_len_t)x;
                }
                /* vertical cursor position */
                if (y > NRF_CLI_MAX_TERMINAL_SIZE)
                {
                    p_cli->p_ctx->vt100_ctx.cons.cur_y = NRF_CLI_MAX_TERMINAL_SIZE;
                }
                else
                {
                    p_cli->p_ctx->vt100_ctx.cons.cur_y = (nrf_cli_cmd_len_t)y;
                }
                p_cli->p_ctx->temp_buff[0] = 0;
                return NRF_SUCCESS;
            }
            else
            {
                p_cli->p_ctx->temp_buff[buff_idx] = c;
            }

            if (++buff_idx > NRF_CLI_CURSOR_POSITION_BUFFER - 1)
            {
                p_cli->p_ctx->temp_buff[0] = 0;
                /* data_buf[NRF_CLI_CURSOR_POSITION_BUFFER - 1] is reserved for '\0' */
                return NRF_ERROR_NO_MEM;
            }

        } while (cnt > 0);
    }
    return NRF_ERROR_TIMEOUT;
}

/* function gets terminal width and height */
static ret_code_t terminal_size_get(nrf_cli_t const *   p_cli,
                                    nrf_cli_cmd_len_t * p_length,
                                    nrf_cli_cmd_len_t * p_height)
{
    ASSERT(p_length);
    ASSERT(p_height);

    uint16_t x;
    uint16_t y;

    if (cursor_position_get(p_cli) == NRF_SUCCESS)
    {
        x = p_cli->p_ctx->vt100_ctx.cons.cur_x;
        y = p_cli->p_ctx->vt100_ctx.cons.cur_y;
        /* assumption: terminal widht and height < 999 */
        cursor_right_move(p_cli, NRF_CLI_MAX_TERMINAL_SIZE); /* move to last column */
        cursor_down_move(p_cli,  NRF_CLI_MAX_TERMINAL_SIZE); /* move to last row */
    }
    else
    {
        return NRF_ERROR_NOT_SUPPORTED;
    }

    if (cursor_position_get(p_cli) == NRF_SUCCESS)
    {
        *p_length = p_cli->p_ctx->vt100_ctx.cons.cur_x;
        *p_height = p_cli->p_ctx->vt100_ctx.cons.cur_y;
        cursor_left_move(p_cli, *p_length - x);
        cursor_up_move(p_cli, *p_height - y);

        return NRF_SUCCESS;
    }
    return NRF_ERROR_NOT_SUPPORTED;
}
#endif // NRF_MODULE_ENABLED(NRF_CLI_BUILD_IN_CMDS)

#if NRF_MODULE_ENABLED(NRF_CLI_VT100_COLORS)
static void vt100_color_set(nrf_cli_t const * p_cli, nrf_cli_vt100_color_t color)
{
    if (color != NRF_CLI_DEFAULT)
    {
        if (p_cli->p_ctx->vt100_ctx.col.col == color)
        {
            return;
        }

        uint8_t cmd[] = NRF_CLI_VT100_COLOR(color - 1);

        p_cli->p_ctx->vt100_ctx.col.col = color;
        nrf_fprintf(p_cli->p_fprintf_ctx, "%s", cmd);
    }
    else
    {
        static uint8_t const cmd[] = NRF_CLI_VT100_MODESOFF;

        p_cli->p_ctx->vt100_ctx.col.col = color;
        nrf_fprintf(p_cli->p_fprintf_ctx, "%s", cmd);
    }
}

static void vt100_bgcolor_set(nrf_cli_t const * p_cli, nrf_cli_vt100_color_t bgcolor)
{
    if (bgcolor != NRF_CLI_DEFAULT)
    {
        if (p_cli->p_ctx->vt100_ctx.col.bgcol == bgcolor)
        {
            return;
        }
         /* -1 because default value is first in enum */
        uint8_t cmd[] = NRF_CLI_VT100_BGCOLOR(bgcolor - 1);

        p_cli->p_ctx->vt100_ctx.col.bgcol = bgcolor;
        nrf_fprintf(p_cli->p_fprintf_ctx, "%s", cmd);
    }
}

static inline void vt100_colors_store(nrf_cli_t const *        p_cli,
                                      nrf_cli_vt100_colors_t * p_color)
{
    memcpy(p_color, &p_cli->p_ctx->vt100_ctx.col, sizeof(nrf_cli_vt100_colors_t));
}

static void vt100_colors_restore(nrf_cli_t const *              p_cli,
                                 nrf_cli_vt100_colors_t const * p_color)
{
    vt100_color_set(p_cli, p_color->col);
    vt100_bgcolor_set(p_cli, p_color->bgcol);
}
#endif // NRF_MODULE_ENABLED(NRF_CLI_VT100_COLORS)

static void left_arrow_handle(nrf_cli_t const * p_cli)
{
    nrf_cli_multiline_cons_t const * cons = multiline_console_data_check(p_cli);

    if ((cons->cur_x == cons->name_len + NRF_CLI_INITIAL_CURS_POS) &&
        (cons->cur_y == NRF_CLI_INITIAL_CURS_POS))
    {
        return; /* nothing to handle because cursor is in start position */
    }

    if (cons->cur_x == NRF_CLI_INITIAL_CURS_POS)
    {   /* go to previous line */
        cursor_up_move(p_cli, 1);
        cursor_right_move(p_cli, cons->terminal_wid);
        --p_cli->p_ctx->cmd_buff_pos;
    }
    else
    {
        cursor_left_move(p_cli, 1);
        --p_cli->p_ctx->cmd_buff_pos;
    }
}

static void right_arrow_handle(nrf_cli_t const * p_cli)
{
    nrf_cli_multiline_cons_t const * cons = multiline_console_data_check(p_cli);

    if ((cons->cur_x == cons->cur_x_end) &&
        (cons->cur_y == cons->cur_y_end))
    {
        return; /* nothing to handle because cursor is in start position */
    }

    if (cons->cur_x == cons->terminal_wid) /* go to next line */
    {
        cursor_down_move(p_cli, 1);
        cursor_left_move(p_cli, cons->terminal_wid);
        ++p_cli->p_ctx->cmd_buff_pos;
    }
    else
    {
        cursor_right_move(p_cli, 1);
        ++p_cli->p_ctx->cmd_buff_pos;
    }
}

static void char_insert(nrf_cli_t const * p_cli, char data)
{
    nrf_cli_cmd_len_t diff;
    bool ins_mode = p_cli->p_ctx->insert_mode;

    diff = p_cli->p_ctx->cmd_buff_len - p_cli->p_ctx->cmd_buff_pos;

    if (!ins_mode)
    {
        if (p_cli->p_ctx->cmd_buff_len >= (NRF_CLI_CMD_BUFF_SIZE - 1))
        {
            return;
        }
        if (diff > 0)
        {
            memmove(&p_cli->p_ctx->cmd_buff[p_cli->p_ctx->cmd_buff_pos + 1],
                    &p_cli->p_ctx->cmd_buff[p_cli->p_ctx->cmd_buff_pos],
                    diff);
        }
    }
    else
    {
        if ((p_cli->p_ctx->cmd_buff_len >= (NRF_CLI_CMD_BUFF_SIZE - 1)) &&
            (diff == 0))
        {
            /* If cmd buffer is full it is possible to replace chars but adding new
               is not allowed */
            return;
        }
    }

    p_cli->p_ctx->cmd_buff[p_cli->p_ctx->cmd_buff_pos] = data;

    if (!ins_mode)
    {
        p_cli->p_ctx->cmd_buff[++p_cli->p_ctx->cmd_buff_len] = '\0';
    }

    if (diff > 0)
    {
        nrf_cli_multiline_cons_t const * cons = multiline_console_data_check(p_cli);
        bool last_line = cons->cur_y == cons->cur_y_end ? true : false;

        /* below if else statement is to mininalize esc codes transmition */
        if (last_line)
        {
            nrf_cli_fprintf(p_cli,
                            NRF_CLI_NORMAL,
                            "%s",
                            &p_cli->p_ctx->cmd_buff[p_cli->p_ctx->cmd_buff_pos]);
            cursor_left_move(p_cli, diff - ins_mode); /* move cursor 1 char left less for ins mode */
        }
        else
        {
            /* save current cursor position in order to get back after fprintf function */
            cli_cursor_save(p_cli);
            nrf_cli_fprintf(p_cli,
                            NRF_CLI_NORMAL,
                            "%s",
                            &p_cli->p_ctx->cmd_buff[p_cli->p_ctx->cmd_buff_pos]);
            cli_cursor_restore(p_cli);
            cursor_right_move(p_cli, 1); /* move right by 1 to edit next character */
        }
    }
    else
    {
        /* new char appended at the end of buffer */
        if (ins_mode)
        {
            p_cli->p_ctx->cmd_buff[++p_cli->p_ctx->cmd_buff_len] = '\0';
        }
        cli_putc(p_cli, data);
    }

    /* incrementation needs to be executed before calling cursor_in_empty_line function */
    ++p_cli->p_ctx->cmd_buff_pos;

    /* forcing terminal to switch to new line if cmd is too long */
    if (cursor_in_empty_line(p_cli))
    {
        cursor_next_line_move(p_cli);
        return;
    }

    if (full_line_cmd(p_cli))
    {
        nrf_cli_multiline_cons_t const * cons = multiline_console_data_check(p_cli);

        /* below code will force terminal to scroll one line down when currently entered command
         * reaches lower right corner of terminal screen */
        cursor_down_move(p_cli, cons->cur_y_end - cons->cur_y - 1);
        cursor_next_line_move(p_cli);
        cursor_up_move(p_cli, cons->cur_y_end - cons->cur_y);
        cursor_right_move(p_cli, cons->cur_x - 1);
        return;
    }
}

static void char_backspace(nrf_cli_t const * p_cli)
{
    nrf_cli_cmd_len_t diff;

    if ((p_cli->p_ctx->cmd_buff_len == 0) || (p_cli->p_ctx->cmd_buff_pos == 0))
    {
        return;
    }

    diff = p_cli->p_ctx->cmd_buff_len - p_cli->p_ctx->cmd_buff_pos;

    memmove(&p_cli->p_ctx->cmd_buff[p_cli->p_ctx->cmd_buff_pos - 1],
            &p_cli->p_ctx->cmd_buff[p_cli->p_ctx->cmd_buff_pos],
            diff + 1);

    --p_cli->p_ctx->cmd_buff_pos;
    --p_cli->p_ctx->cmd_buff_len;

    if (diff > 0)
    {
        cli_putc(p_cli, NRF_CLI_VT100_ASCII_BSPACE);

        nrf_cli_multiline_cons_t const * cons = multiline_console_data_check(p_cli);
        bool last_line = cons->cur_y == cons->cur_y_end ? true : false;

        if (last_line)
        {
            nrf_cli_fprintf(p_cli,
                            NRF_CLI_NORMAL,
                            "%s",
                            &p_cli->p_ctx->cmd_buff[p_cli->p_ctx->cmd_buff_pos]);
            cli_clear_eos(p_cli);
            cursor_left_move(p_cli, diff);
        }
        else
        {
            /* if cursor is not in last cmd line its position needs to be saved by
             * VT100 command */
            cli_cursor_save(p_cli);
            cli_clear_eos(p_cli);
            nrf_cli_fprintf(p_cli,
                            NRF_CLI_NORMAL,
                            "%s",
                            &p_cli->p_ctx->cmd_buff[p_cli->p_ctx->cmd_buff_pos]);
            cli_cursor_restore(p_cli);
        }
    }
    else
    {
        static char const cmd_bspace[] = {
            NRF_CLI_VT100_ASCII_BSPACE, ' ', NRF_CLI_VT100_ASCII_BSPACE, '\0'};
        nrf_fprintf(p_cli->p_fprintf_ctx, "%s", cmd_bspace);
    }
}

static void char_delete(nrf_cli_t const * p_cli)
{
    nrf_cli_cmd_len_t diff;

    diff = p_cli->p_ctx->cmd_buff_len - p_cli->p_ctx->cmd_buff_pos;

    if (diff == 0)
    {
        return;
    }

    memmove(&p_cli->p_ctx->cmd_buff[p_cli->p_ctx->cmd_buff_pos],
            &p_cli->p_ctx->cmd_buff[p_cli->p_ctx->cmd_buff_pos + 1],
            diff);

    --p_cli->p_ctx->cmd_buff_len;

    nrf_cli_multiline_cons_t const * cons = multiline_console_data_check(p_cli);
    bool last_line = cons->cur_y == cons->cur_y_end ? true : false;

    /* if last line of cmd is edited there is no need for saving cursor position */
    if (last_line)
    {
        nrf_cli_fprintf(p_cli,
                        NRF_CLI_NORMAL,
                        "%s",
                        &p_cli->p_ctx->cmd_buff[p_cli->p_ctx->cmd_buff_pos]);
        NRF_CLI_VT100_CMD(p_cli, NRF_CLI_VT100_CLEAREOL);
        cursor_left_move(p_cli, --diff);
    }
    else
    {
        cli_cursor_save(p_cli);
        cli_clear_eos(p_cli);
        nrf_cli_fprintf(p_cli,
                        NRF_CLI_NORMAL,
                        "%s",
                        &p_cli->p_ctx->cmd_buff[p_cli->p_ctx->cmd_buff_pos]);
        cli_cursor_restore(p_cli);
    }
}

static char make_argv(size_t * p_argc, char ** pp_argv, char * p_cmd, uint8_t maxargc)
{
    char c;
    char quote = 0;

    *p_argc = 0;
    do
    {
        c = *p_cmd;
        if (c == '\0')
        {
            break;
        }

        if (isspace((int)c))
        {
            *p_cmd++ = '\0';
            continue;
        }

        pp_argv[(*p_argc)++] = p_cmd;
        quote = 0;

        while (1)
        {
            c = *p_cmd;

            if (c == '\0')
            {
                break;
            }

            if (!quote)
            {
                switch (c)
                {
                    case '\\':
                        memmove(p_cmd, p_cmd + 1, cli_strlen(p_cmd));
                        p_cmd += 1;
                        continue;

                    case '\'':
                    case '\"':
                        memmove(p_cmd, p_cmd + 1, cli_strlen(p_cmd));
                        quote = c;
                        continue;
                    default:
                        break;
                }
            }

            if (quote == c)
            {
                memmove(p_cmd, p_cmd + 1, cli_strlen(p_cmd));
                quote = 0;
                continue;
            }

            if (quote && c == '\\')
            {
                char t = *(p_cmd + 1);

                if (t == quote)
                {
                    memmove(p_cmd, p_cmd + 1, cli_strlen(p_cmd));
                    p_cmd += 1;
                    continue;
                }

                if (t == '0')
                {
                    uint8_t i;
                    uint8_t v = 0;

                    for (i = 2; i < (2 + 3); i++)
                    {
                        t = *(p_cmd + i);

                        if (t >= '0' && t <= '7')
                        {
                            v = (v << 3) | (t - '0');
                        }
                        else
                        {
                            break;
                        }
                    }

                    if (i > 2)
                    {
                        memmove(p_cmd, p_cmd + (i - 1), cli_strlen(p_cmd) - (i - 2));
                        *p_cmd++ = v;
                        continue;
                    }
                }

                if (t == 'x')
                {
                    uint8_t i;
                    uint8_t v = 0;

                    for (i = 2; i < (2 + 2); i++)
                    {
                        t = *(p_cmd + i);

                        if (t >= '0' && t <= '9')
                        {
                            v = (v << 4) | (t - '0');
                        }
                        else if (t >= 'a' && t <= 'f')
                        {
                            v = (v << 4) | (t - 'a' + 10);
                        }
                        else if (t >= 'A' && t <= 'F')
                        {
                            v = (v << 4) | (t - 'A' + 10);
                        }
                        else
                        {
                            break;
                        }
                    }

                    if (i > 2)
                    {
                        memmove(p_cmd, p_cmd + (i - 1), cli_strlen(p_cmd) - (i - 2));
                        *p_cmd++ = v;
                        continue;
                    }
                }
            }
            if (!quote && isspace((int)c))
            {
                break;
            }

            p_cmd += 1;
        }
    } while (*p_argc < maxargc);

    ASSERT(*p_argc <= NRF_CLI_ARGC_MAX);
    pp_argv[*p_argc] = 0;

    return quote;
}

static void cli_state_change(nrf_cli_t const * p_cli, nrf_cli_state_t state)
{
    p_cli->p_ctx->state = state;
    switch (state)
    {
        case NRF_CLI_STATE_COLLECT:
            p_cli->p_ctx->cmd_buff[0] = '\0';  /* clear command buffer */
            p_cli->p_ctx->cmd_buff_pos = 0;
            p_cli->p_ctx->cmd_buff_len = 0;
            nrf_cli_fprintf(p_cli, NRF_CLI_INFO, "%s", p_cli->p_name);
            break;
        default:
            break;
    }
}

#if NRF_MODULE_ENABLED(NRF_CLI_HISTORY)
static inline void history_mode_exit(nrf_cli_t const * p_cli)
{
    p_cli->p_ctx->p_cmd_list_element = NULL;
}

static void history_handle(nrf_cli_t const * p_cli, bool up)
{
    nrf_cli_memobj_header_t header = {
        .p_prev = NULL,
        .p_next = NULL,
        .cmd_len = 0
    };
    nrf_cli_cmd_len_t current_cmd_len;
    bool skip = false;

    if (!up) /* button down */
    {
        if (p_cli->p_ctx->p_cmd_list_element == NULL)
        {
            return;
        }
        cursor_home_position_move(p_cli);

        nrf_memobj_read(p_cli->p_ctx->p_cmd_list_element,
                        &header,
                        NRF_CLI_HISTORY_HEADER_SIZE,
                        (uint32_t)0);

        p_cli->p_ctx->p_cmd_list_element = header.p_next;
        current_cmd_len = p_cli->p_ctx->cmd_buff_len;

        if (p_cli->p_ctx->p_cmd_list_element == NULL)
        {
            if (cli_strlen(p_cli->p_ctx->temp_buff) > 0)
            {
                strcpy(p_cli->p_ctx->cmd_buff, p_cli->p_ctx->temp_buff);
            }
            else
            {
                p_cli->p_ctx->cmd_buff[0] = '\0';
            }
            header.cmd_len = cli_strlen(p_cli->p_ctx->cmd_buff);
            skip = true;
        }
    }
    else /* button up */
    {
        if ((p_cli->p_ctx->p_cmd_list_element == p_cli->p_ctx->p_cmd_list_tail) ||
            (p_cli->p_ctx->p_cmd_list_head == NULL))
        {
            /* nothing to display */
            return;
        }
        cursor_home_position_move(p_cli);

        if (p_cli->p_ctx->p_cmd_list_element == NULL)
        {
            current_cmd_len = cli_strlen(p_cli->p_ctx->cmd_buff);

            p_cli->p_ctx->p_cmd_list_element = p_cli->p_ctx->p_cmd_list_head;
            /* save currently entered and not executed command */
            if (current_cmd_len > 0)
            {
                strcpy(p_cli->p_ctx->temp_buff, p_cli->p_ctx->cmd_buff);
            }
            else
            {
                p_cli->p_ctx->temp_buff[0] = '\0';
            }
        }
        else
        {
            nrf_memobj_read(p_cli->p_ctx->p_cmd_list_element,
                            &header,
                            NRF_CLI_HISTORY_HEADER_SIZE,
                            (uint32_t)0);
            current_cmd_len = header.cmd_len;
            p_cli->p_ctx->p_cmd_list_element = header.p_prev;
        }
    }
    if (!skip)
    {
        nrf_memobj_read(p_cli->p_ctx->p_cmd_list_element,
                        &header,
                        NRF_CLI_HISTORY_HEADER_SIZE,
                        (uint32_t)0);

        nrf_memobj_read(p_cli->p_ctx->p_cmd_list_element,
                        p_cli->p_ctx->cmd_buff,
                        header.cmd_len + 1, /* +1 for '\0' */
                        (uint32_t)NRF_CLI_HISTORY_HEADER_SIZE);
    }

    p_cli->p_ctx->cmd_buff_pos = header.cmd_len;
    p_cli->p_ctx->cmd_buff_len = header.cmd_len;

    if (current_cmd_len > header.cmd_len)
    {
        cli_clear_eos(p_cli);
    }

    nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "%s", p_cli->p_ctx->cmd_buff);
    if (cursor_in_empty_line(p_cli) || full_line_cmd(p_cli))
    {
        cursor_next_line_move(p_cli);
    }
}

static void history_list_element_add(nrf_cli_t const * p_cli, nrf_memobj_t * p_memobj)
{
    ASSERT(p_memobj != NULL);

    nrf_cli_memobj_header_t header;

    if (p_cli->p_ctx->p_cmd_list_head == NULL)
    {
        p_cli->p_ctx->p_cmd_list_head = p_memobj;
        p_cli->p_ctx->p_cmd_list_tail = p_memobj;
        header.p_prev = NULL;
        header.p_next = NULL;
        header.cmd_len = p_cli->p_ctx->cmd_buff_len;
    }
    else
    {
        nrf_memobj_read(p_cli->p_ctx->p_cmd_list_head,
                        &header,
                        NRF_CLI_HISTORY_HEADER_SIZE,
                        (uint32_t)0);

        header.p_next = p_memobj;

        nrf_memobj_write(p_cli->p_ctx->p_cmd_list_head,
                         &header,
                         NRF_CLI_HISTORY_HEADER_SIZE,
                         (uint32_t)0);

        header.p_next = NULL;
        header.p_prev = p_cli->p_ctx->p_cmd_list_head;
        header.cmd_len = p_cli->p_ctx->cmd_buff_len;

        p_cli->p_ctx->p_cmd_list_head = p_memobj;
    }

    nrf_memobj_write(p_memobj,
                     &header,
                     NRF_CLI_HISTORY_HEADER_SIZE,
                     (uint32_t)0);

    nrf_memobj_write(p_memobj,
                     p_cli->p_ctx->cmd_buff,
                     p_cli->p_ctx->cmd_buff_len + 1, /* +1 for '\0' */
                     (uint32_t)NRF_CLI_HISTORY_HEADER_SIZE);
}

static void history_list_element_oldest_remove(nrf_cli_t const * p_cli)
{
    if (p_cli->p_ctx->p_cmd_list_tail == NULL)
    {
        return; // nothing to do
    }

    nrf_cli_memobj_header_t header;
    nrf_memobj_t * p_memobj = p_cli->p_ctx->p_cmd_list_tail;

    nrf_memobj_read(p_memobj,
                    &header,
                    NRF_CLI_HISTORY_HEADER_SIZE,
                    (uint32_t)0);

    p_cli->p_ctx->p_cmd_list_tail = header.p_next;
    memset(&header, 0, sizeof(nrf_cli_memobj_header_t));
    nrf_memobj_write(p_memobj, &header, NRF_CLI_HISTORY_HEADER_SIZE, 0);
    nrf_memobj_free(p_memobj);

    if (p_cli->p_ctx->p_cmd_list_tail == NULL) /* removed all memory objects */
    {
        p_cli->p_ctx->p_cmd_list_head = NULL;
        return;
    }

    nrf_memobj_read(p_cli->p_ctx->p_cmd_list_tail,
                    &header,
                    NRF_CLI_HISTORY_HEADER_SIZE,
                    (uint32_t)0);

    header.p_prev = NULL;
    nrf_memobj_write(p_cli->p_ctx->p_cmd_list_tail, &header, NRF_CLI_HISTORY_HEADER_SIZE, 0);
}

static void history_list_free_memory(nrf_cli_t const * p_cli)
{
    while (p_cli->p_ctx->p_cmd_list_tail != NULL)
    {
        history_list_element_oldest_remove(p_cli);
    }
}

static void history_save(nrf_cli_t const * p_cli)
{
    nrf_cli_cmd_len_t cmd_new_len = cli_strlen(p_cli->p_ctx->cmd_buff);

    history_mode_exit(p_cli);

    if (cmd_new_len == 0)
    {
        return;
    }

    /* checking if newly entered command is not duplicated with previous one */
    if (p_cli->p_ctx->p_cmd_list_head != NULL)
    {
        nrf_cli_memobj_header_t header;

        nrf_memobj_read(p_cli->p_ctx->p_cmd_list_head,
                        &header,
                        NRF_CLI_HISTORY_HEADER_SIZE,
                        (uint32_t)0);
        if (cmd_new_len == header.cmd_len)
        {
            nrf_memobj_read(p_cli->p_ctx->p_cmd_list_head,
                            p_cli->p_ctx->temp_buff,
                            header.cmd_len + 1, /* +1 for '\0' */
                            (uint32_t)NRF_CLI_HISTORY_HEADER_SIZE);

            if (strcmp(p_cli->p_ctx->cmd_buff, p_cli->p_ctx->temp_buff) == 0)
            {
                p_cli->p_ctx->temp_buff[0] = '\0';
                return; /* duplicated command, nothing to save */
            }
            p_cli->p_ctx->temp_buff[0] = '\0';
        }
    }

    for (size_t idx = 0; idx < NRF_CLI_HISTORY_ELEMENT_COUNT; idx++)
    {
        nrf_memobj_t * p_memobj;

        p_memobj = nrf_memobj_alloc(p_cli->p_cmd_hist_mempool,
                                    ((size_t)NRF_CLI_HISTORY_HEADER_SIZE + cmd_new_len + 1));
        if (p_memobj != NULL)
        {
            history_list_element_add(p_cli, p_memobj);
            return;
        }
        else
        {
            history_list_element_oldest_remove(p_cli);
        }
    }
    return;
}
#endif // NRF_MODULE_ENABLED(NRF_CLI_HISTORY)

/* function checks how many similar chars in row two strings have starting from first char */
static nrf_cli_cmd_len_t str_similarity_check(char const * str_a, char const * str_b)
{
    nrf_cli_cmd_len_t cnt = 0;

    while (str_a[cnt] != '\0')
    {
        if (str_a[cnt] != str_b[cnt])
        {
            return cnt;
        }

        if (++cnt == 0)
        {
            return --cnt; /* too long strings */
        }
    }
    return cnt;
}

static void completion_insert(nrf_cli_t const * p_cli,
                              char const *      p_compl,
                              nrf_cli_cmd_len_t compl_len)
{
    ASSERT (p_compl);

    nrf_cli_cmd_len_t diff = p_cli->p_ctx->cmd_buff_len - p_cli->p_ctx->cmd_buff_pos;

    if ((p_cli->p_ctx->cmd_buff_len + compl_len > NRF_CLI_CMD_BUFF_SIZE - 1) ||
        (compl_len == 0))
    {
        return;
    }

    /* make space for completion */
    memmove(&p_cli->p_ctx->cmd_buff[p_cli->p_ctx->cmd_buff_pos + compl_len],
            &p_cli->p_ctx->cmd_buff[p_cli->p_ctx->cmd_buff_pos],
            diff + 1); /* + 1 for '\0' */

    /* insert completion */
    memmove(&p_cli->p_ctx->cmd_buff[p_cli->p_ctx->cmd_buff_pos],
            p_compl,
            compl_len);

    p_cli->p_ctx->cmd_buff_len = cli_strlen(p_cli->p_ctx->cmd_buff);
    nrf_cli_fprintf(p_cli,
                    NRF_CLI_NORMAL,
                    "%s",
                    &p_cli->p_ctx->cmd_buff[p_cli->p_ctx->cmd_buff_pos]);
    p_cli->p_ctx->cmd_buff_pos += compl_len;

    if (cursor_in_empty_line(p_cli) || full_line_cmd(p_cli))
    {
        cursor_next_line_move(p_cli);
    }

    if (diff > 0)
    {
        cursor_position_synchronize(p_cli);
    }
}

static void option_print(nrf_cli_t const * p_cli,
                         char const *      p_option,
                         nrf_cli_cmd_len_t longest_option)
{
    static char const * tab = "  ";

    if (p_option == NULL)
    {
        /* reset function */
        p_cli->p_ctx->vt100_ctx.printed_cmd = 0;
        return;
    }
    longest_option += cli_strlen(tab);

    nrf_cli_cmd_len_t columns =
        (p_cli->p_ctx->vt100_ctx.cons.terminal_wid - cli_strlen(tab)) / longest_option;
    nrf_cli_cmd_len_t diff = longest_option - cli_strlen(p_option);

    if (p_cli->p_ctx->vt100_ctx.printed_cmd++ % columns == 0)
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_OPTION, "\r\n%s%s", tab, p_option);
    }
    else
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_OPTION, "%s", p_option);
    }
    cursor_right_move(p_cli, diff);
}

static void cli_tab_handle(nrf_cli_t const * p_cli)
{
    size_t cmd_idx;
    size_t cmd_last = 0;
    size_t cmd_first = 0;

    size_t argc;
    char * argv[NRF_CLI_ARGC_MAX + 1]; /* +1 reserved for NULL in function make_argv */

    nrf_cli_cmd_len_t cmd_lvl = NRF_CLI_CMD_ROOT_LVL;
    nrf_cli_cmd_len_t cmd_longest = 0; /* longest matching command */

    /* longest possible completion length, -1 for '\0' */
    nrf_cli_cmd_len_t compl_len = (NRF_CLI_CMD_BUFF_SIZE - 1) - p_cli->p_ctx->cmd_buff_len;

    if (compl_len == 0)
    {
        return;
    }

    memcpy(p_cli->p_ctx->temp_buff,
           p_cli->p_ctx->cmd_buff,
           p_cli->p_ctx->cmd_buff_pos); /* copy command to the cursor position */

    p_cli->p_ctx->temp_buff[p_cli->p_ctx->cmd_buff_pos] = '\0';

    /* check if current cursor position is space */
    bool space = isspace((int)p_cli->p_ctx->cmd_buff[p_cli->p_ctx->cmd_buff_pos - 1]);

#if NRF_MODULE_ENABLED(NRF_CLI_HISTORY)
    /* tab execution during history mode must terminate history mode because both functions
       share temp_buff */
    history_mode_exit(p_cli);
#endif

    /* create argument list */
    (void)make_argv(&argc,
                    &argv[0],
                    p_cli->p_ctx->temp_buff,
                    NRF_CLI_ARGC_MAX);

    nrf_cli_cmd_len_t arg_len = cli_strlen(argv[cmd_lvl]);

    /* reserved memory for static_entry to handle dynamic commands */
    nrf_cli_static_entry_t static_entry;

    nrf_cli_cmd_entry_t const * p_cmd = NULL;
    nrf_cli_static_entry_t const * p_st_cmd = NULL;
    nrf_cli_static_entry_t const * p_st_cmd_last = NULL;

    do
    {
        if ((argc == 0) ||
            (cmd_lvl >= argc - 1 + space))
        {
            if (space)
            {
                arg_len = 0;
            }
            else
            {
                arg_len = cli_strlen(argv[cmd_lvl]);
            }

            cmd_idx = 0;

            while (1)
            {
                cmd_get(p_cmd, cmd_lvl, cmd_idx++, &p_st_cmd, &static_entry);

                if (p_st_cmd == NULL)
                {
                    break; /* no more commands */
                }

                if (strncmp(argv[cmd_lvl], p_st_cmd->p_syntax, arg_len) != 0)
                {
                    if (p_st_cmd_last != NULL)
                    { /* no more match will be found as commands are sorted alphabetically */
                        break;
                    }
                    continue;
                }
                if (p_st_cmd_last == NULL)
                {
                    cmd_first = cmd_idx - 1;
                    cmd_longest = cli_strlen(p_st_cmd->p_syntax);
                    if (compl_len > (cmd_longest - arg_len))
                    {
                        compl_len = cmd_longest - arg_len;
                    }
                }
                else
                {
                    nrf_cli_cmd_len_t len = cli_strlen(p_st_cmd->p_syntax);
                    if (len > cmd_longest)
                    {
                        cmd_longest = len;
                    }

                    if (compl_len > 0)  /* checking if partial completion possible */
                    {
                        nrf_cli_static_entry_t last_entry;
                        cmd_get(p_cmd, cmd_lvl, cmd_last, &p_st_cmd_last, &last_entry);

                        len = str_similarity_check(p_st_cmd->p_syntax + arg_len,
                                                   p_st_cmd_last->p_syntax + arg_len);
                        if (compl_len > len)
                        {
                            compl_len = len; /* determining longest possible completion */
                        }
                    }
                }
                cmd_last = cmd_idx - 1;
                p_st_cmd_last = p_st_cmd;

                if (cmd_idx == 0)   /* Too many possibilities */
                {
                    nrf_cli_fprintf(p_cli, NRF_CLI_WARNING,
                                    "Tab function: commands counter overflow\r\n");
                    break;
                }
            }
        }
        else
        {
            cmd_idx = 0;

            while (1)
            {
                cmd_get(p_cmd, cmd_lvl, cmd_idx++, &p_st_cmd, &static_entry);

                if (cmd_idx == 0)
                {
                    return; /* no match found, no more commands available */
                }

                if (p_st_cmd == NULL)
                {
                    return; /* no more commands available */
                }
                /* strcmp used because exact match is requested */
                if (strcmp(argv[cmd_lvl], p_st_cmd->p_syntax) == 0)
                {
                    p_cmd = p_st_cmd->p_subcmd;
                    break;
                }
            }
        }

        if ((p_cmd == NULL) || (p_st_cmd == NULL))
        {
            break;
        }

    } while (++cmd_lvl < argc + space);

    if (p_st_cmd_last == NULL)
    {
        return; /* no match found */
    }

    if (cmd_first == cmd_last) /* only 1 match found */
    {
        if (p_cmd->is_dynamic)
        {
            /* In case of dynamic entry, function cmd_get shall be called again for matching
             * command index (cmd_last). It is because static_entry is most likely appended by
             * not valid data.
             */
            cmd_get(p_cmd, cmd_lvl, cmd_last, &p_st_cmd_last, &static_entry);
        }
        if (cli_strlen(p_st_cmd_last->p_syntax) != arg_len) /* not excact match found */
        {
            completion_insert(p_cli, p_st_cmd_last->p_syntax + arg_len, compl_len);
        }

        /* next char in buff is not space */
        if (!isspace((int)p_cli->p_ctx->cmd_buff[p_cli->p_ctx->cmd_buff_pos]))
        {
            if (p_cli->p_ctx->insert_mode)
            {
                p_cli->p_ctx->insert_mode = false;
                char_insert(p_cli, ' ');
                p_cli->p_ctx->insert_mode = true;
            }
            else
            {
                char_insert(p_cli, ' ');
            }
        }
        else
        {
            /*  case:
                | | -> cursor
                cons_name $: valid_cmd valid_sub_cmd| |argument  <tab>
             */
            cursor_position_increment(p_cli);
            /* result:
                cons_name $: valid_cmd valid_sub_cmd |a|rgument
             */
        }
        return;
    }

    /* printing options - all matching commands */
    option_print(p_cli, NRF_CLI_INIT_OPTION_PRINTER, cmd_longest);
    for (cmd_idx = cmd_first; cmd_idx <= cmd_last; cmd_idx++)
    {
        cmd_get(p_cmd, cmd_lvl, cmd_idx, &p_st_cmd, &static_entry);
        option_print(p_cli, p_st_cmd->p_syntax, cmd_longest);
    }

    nrf_cli_fprintf(p_cli, NRF_CLI_INFO, "\r\n%s", p_cli->p_name);
    nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "%s", p_cli->p_ctx->cmd_buff);

    cursor_position_synchronize(p_cli);
    completion_insert(p_cli, p_st_cmd_last->p_syntax + arg_len, compl_len);
}

#define NRF_CLI_ASCII_MAX_CHAR (127u)
static inline ret_code_t ascii_filter(char const data)
{
    return (uint8_t)data > NRF_CLI_ASCII_MAX_CHAR ? NRF_ERROR_INVALID_DATA : NRF_SUCCESS;
}

static void cli_state_collect(nrf_cli_t const * p_cli)
{
    size_t count = 0;
    char data;

    while (1)
    {
        cli_read(p_cli, &data, sizeof(data), &count);
        if (count == 0)
        {
            return;
        }

        if (ascii_filter(data) != NRF_SUCCESS)
        {
            continue;
        }

#if NRF_MODULE_ENABLED(NRF_PWR_MGMT)
        nrf_pwr_mgmt_feed();
#endif

        switch (p_cli->p_ctx->receive_state)
        {
            case NRF_CLI_RECEIVE_DEFAULT:
                if (data == p_cli->newline_char)
                {
                    if (p_cli->p_ctx->cmd_buff_len == 0)
                    {
                        cursor_next_line_move(p_cli);
                        cli_state_change(p_cli, NRF_CLI_STATE_COLLECT);
                    }
                    else
                    {
                        cli_state_change(p_cli, NRF_CLI_STATE_EXECUTE);
#if NRF_MODULE_ENABLED(NRF_CLI_USES_TASK_MANAGER)
                        task_events_set(task_id_get(), NRF_CLI_CMD_EXECUTE_EVT);
#endif
                    }
                    return;
                }
                switch (data)
                {
                    case NRF_CLI_VT100_ASCII_ESC:       /* ESCAPE */
                        NRF_CLI_RECIEVE_STATE_NEXT(p_cli, NRF_CLI_RECEIVE_ESC);
                        break;
                    case '\0':
                        break;
                    case '\t':                          /* TAB */
                        cli_tab_handle(p_cli);
                        break;
                    case NRF_CLI_VT100_ASCII_BSPACE:    /* BACKSPACE */
                        char_backspace(p_cli);
                        break;
                    case NRF_CLI_VT100_ASCII_DEL:       /* DELETE */
                        char_delete(p_cli);
                        break;
                    default:
                        if (!isprint((int)data))
                        {
                            break;
                        }
                        char_insert(p_cli, data);
                        break;
                }
                break;
            case NRF_CLI_RECEIVE_ESC:
                if (data == '[')
                {
                    NRF_CLI_RECIEVE_STATE_NEXT(p_cli, NRF_CLI_RECEIVE_ESC_SEQ);
                }
                else
                {
                    NRF_CLI_RECIEVE_STATE_NEXT(p_cli, NRF_CLI_RECEIVE_DEFAULT);
                }
                break;
            case NRF_CLI_RECEIVE_ESC_SEQ:
                switch (data)
                {
#if NRF_MODULE_ENABLED(NRF_CLI_HISTORY)
                    case 'A': /* UP arrow */
                        history_handle(p_cli, true);
                        break;
                    case 'B': /* DOWN arrow */
                        history_handle(p_cli, false);
                        break;
#endif
                    case 'C': /* RIGHT arrow */
                        right_arrow_handle(p_cli);
                        break;
                    case 'D': /* LEFT arrow */
                        left_arrow_handle(p_cli);
                        break;
                    case 'F': /* END Button */
                        cursor_end_position_move(p_cli);
                        break;
                    case 'H': /* HOME Button */
                        cursor_home_position_move(p_cli);
                        break;
                    case 'L': /* INSERT Button */
                        p_cli->p_ctx->insert_mode = !p_cli->p_ctx->insert_mode;
                        break;
                    default:
                        break;
                }
                NRF_CLI_RECIEVE_STATE_NEXT(p_cli, NRF_CLI_RECEIVE_DEFAULT);
                break;
            default:
                NRF_CLI_RECIEVE_STATE_NEXT(p_cli, NRF_CLI_RECEIVE_DEFAULT);
                break;
        }
    }
}

/* function removes white chars from beginning and end of command buffer */
static void cmd_trim(nrf_cli_t const * p_cli)
{
    nrf_cli_cmd_len_t i = 0;

    if (p_cli->p_ctx->cmd_buff[0] == '\0') /* no command in the buffer */
    {
        return;
    }

    /* counting head white chars */
    while (isspace((int)p_cli->p_ctx->cmd_buff[i++]))
    {
        if (i == 0)
        {
            p_cli->p_ctx->cmd_buff[0] = '\0';
            return;
        }
    }

    /* removing head white chars */
    if (--i > 0)
    {
        memmove(p_cli->p_ctx->cmd_buff,
                p_cli->p_ctx->cmd_buff + i,
                (p_cli->p_ctx->cmd_buff_len + 1) - i); /* +1 for '\0' */
        p_cli->p_ctx->cmd_buff_len = p_cli->p_ctx->cmd_buff_len - i;
        p_cli->p_ctx->cmd_buff_pos = p_cli->p_ctx->cmd_buff_len;
    }

    /* counting tail white chars */
    char * p_end = &p_cli->p_ctx->cmd_buff[p_cli->p_ctx->cmd_buff_len - 1];
    i = 0;
    while (isspace((int)*p_end))
    {
        ++i;
        --p_end;
    }

    /* removing tail white chars */
    if (p_end != &p_cli->p_ctx->cmd_buff[p_cli->p_ctx->cmd_buff_len - 1])
    {
        p_cli->p_ctx->cmd_buff[p_cli->p_ctx->cmd_buff_len - i] = '\0';
        p_cli->p_ctx->cmd_buff_len = p_cli->p_ctx->cmd_buff_len - i;
        p_cli->p_ctx->cmd_buff_pos = p_cli->p_ctx->cmd_buff_len;
    }
}

/* function is analyzing command buffer to find matching commands. Next it invokes last recognized
 * command which has handler and passes rest of command buffer as arguments. */
static void cli_state_execute(nrf_cli_t const * p_cli)
{
    char quote;
    size_t argc;
    char * argv[NRF_CLI_ARGC_MAX + 1]; /* +1 reserved for NULL added by function make_argv */

    size_t cmd_idx;             /* currently analyzed command in cmd_level */
    size_t cmd_lvl = NRF_CLI_CMD_ROOT_LVL; /* currently analyzed command level */
    size_t cmd_handler_lvl = 0; /* last command level for witch handler has been found */
    size_t cmd_handler_idx = 0; /* last command index for witch handler has been found */

    nrf_cli_cmd_entry_t const * p_cmd = NULL;

    cmd_trim(p_cli);
#if NRF_MODULE_ENABLED(NRF_CLI_HISTORY)
    history_save(p_cli);
#endif

    /* create argument list */
    quote = make_argv(&argc,
                      &argv[0],
                      p_cli->p_ctx->cmd_buff,
                      NRF_CLI_ARGC_MAX);

    if (!argc)
    {
        cursor_next_line_move(p_cli);
        return;
    }

    if (quote != 0)
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "\r\nnot terminated: %c\r\n", quote);
        return;
    }

    /*  try match command syntax for root cmd. */
    for (cmd_idx = 0; cmd_idx <= CLI_DATA_SECTION_ITEM_COUNT; ++cmd_idx)
    {
        if (cmd_idx >= CLI_DATA_SECTION_ITEM_COUNT)
        {
            if (cursor_in_empty_line(p_cli))
            {
                nrf_cli_fprintf(p_cli,
                                NRF_CLI_ERROR,
                                "%s%s\r\n",
                                argv[0],
                                m_nrf_cli_command_not_found);
            }
            else
            {
                nrf_cli_fprintf(p_cli,
                                NRF_CLI_ERROR,
                                "\r\n%s%s\r\n",
                                argv[0],
                                m_nrf_cli_command_not_found);
            }
            return;
        }

        p_cmd = CLI_DATA_SECTION_ITEM_GET(cmd_idx);
        if (strcmp(argv[cmd_lvl], p_cmd->u.p_static->p_syntax) != 0)
        {
            continue;
        }
        break;
    }

    /* root command shall be static */
    ASSERT(p_cmd->is_dynamic == false);

    /* pointer to cmd level where handler has been found */
    nrf_cli_cmd_entry_t const * p_cmd_low_level_entry = NULL;

    /* memory reserved for dynamic commands */
    nrf_cli_static_entry_t static_entry;

    nrf_cli_cmd_handler handler_cmd_lvl_0 = p_cmd->u.p_static->handler;
    if (handler_cmd_lvl_0 != NULL)
    {
        p_cli->p_ctx->p_current_stcmd = p_cmd->u.p_static;
    }

    p_cmd = p_cmd->u.p_static->p_subcmd;
    cmd_lvl++;
    cmd_idx = 0;

    while (1)
    {
        if (cmd_lvl >= argc)
        {
            break;
        }

        if (!strcmp(argv[cmd_lvl], "-h") || !strcmp(argv[cmd_lvl], "--help"))
        {
            /* command called with help option so it makes no sense to go deeper in commands tree */
            NRF_CLI_HELP_FLAG_SET(p_cli);
            break;
        }

        cmd_get(p_cmd,
                cmd_lvl,
                cmd_idx++,
                &p_cli->p_ctx->p_current_stcmd,
                &static_entry);

        if ((cmd_idx == 0) || (p_cli->p_ctx->p_current_stcmd == NULL))
        {
            break;
        }

        if (strcmp(argv[cmd_lvl], p_cli->p_ctx->p_current_stcmd->p_syntax) == 0)
        {
            if (p_cli->p_ctx->p_current_stcmd->handler != NULL)
            {
                /* storing p_st_cmd->handler is not feasable because in case of dynamic
                 * commands data will be invalid in next loop */
                cmd_handler_lvl = cmd_lvl;
                cmd_handler_idx = cmd_idx - 1;
                p_cmd_low_level_entry = p_cmd;
            }
            cmd_lvl++;
            cmd_idx = 0;
            p_cmd = p_cli->p_ctx->p_current_stcmd->p_subcmd;
        }
    }

    cursor_end_position_move(p_cli);
    if (!cursor_in_empty_line(p_cli))
    {
        cursor_next_line_move(p_cli);
    }

    /* Executing deepest found handler */
    if (p_cmd_low_level_entry != NULL)
    {
        cmd_get(p_cmd_low_level_entry,
                cmd_handler_lvl,
                cmd_handler_idx,
                &p_cli->p_ctx->p_current_stcmd,
                &static_entry);

        p_cli->p_ctx->p_current_stcmd->handler(p_cli,
                                               argc - cmd_handler_lvl,
                                               &argv[cmd_handler_lvl]);
    }
    else if (handler_cmd_lvl_0 != NULL)
    {
        handler_cmd_lvl_0(p_cli, argc, &argv[0]);
    }
    else
    {
        p_cli->p_ctx->show_help = false;

        nrf_cli_fprintf(p_cli,
                        NRF_CLI_ERROR,
                        "%s\r\n",
                        "please specify subcommand");
    }
    NRF_CLI_HELP_FLAG_CLEAR(p_cli);
}

/* function required by qsort */
static int string_cmp(void const * pp_a, void const * pp_b)
{
    ASSERT(pp_a);
    ASSERT(pp_b);

    char const ** pp_str_a = (char const **)pp_a;
    char const ** pp_str_b = (char const **)pp_b;

    return strcmp(*pp_str_a, *pp_str_b);
}

static void cli_transport_evt_handler(nrf_cli_transport_evt_t evt_type, void * p_context)
{
    nrf_cli_t * p_cli = (nrf_cli_t *)p_context;
    ASSERT(p_cli);
#if NRF_MODULE_ENABLED(NRF_CLI_USES_TASK_MANAGER)
    task_events_set(p_cli->p_ctx->task_id, evt_type == NRF_CLI_TRANSPORT_EVT_RX_RDY ?
            NRF_CLI_TRANSPORT_RX_RDY_TASK_EVT : NRF_CLI_TRANSPORT_TX_RDY_TASK_EVT);
#else

    if (evt_type == NRF_CLI_TRANSPORT_EVT_RX_RDY)
    {

    }
    else
    {
        /* wr done evt */
        p_cli->p_ctx->tx_rdy = true;
    }
#endif
}

static ret_code_t nrf_cli_instance_init(nrf_cli_t const * p_cli,
                                        void const *      p_config,
                                        bool              use_colors)
{
    ASSERT(p_cli);
    ASSERT(p_cli->p_ctx && p_cli->p_iface && p_cli->p_name);
    ASSERT((p_cli->newline_char == '\n') || (p_cli->newline_char == '\r'));

#if defined(NRF_CLI_LOG_BACKEND) && NRF_CLI_LOG_BACKEND
    p_cli->p_log_backend->p_cli = p_cli;
#endif
    ret_code_t ret = p_cli->p_iface->p_api->init(p_cli->p_iface,
                                                 p_config,
                                                 cli_transport_evt_handler,
                                                 (void *)p_cli);
    if (ret != NRF_SUCCESS)
    {
        return ret;
    }

#if NRF_MODULE_ENABLED(NRF_CLI_HISTORY)
    ASSERT(p_cli->p_cmd_hist_mempool);
    ret = nrf_memobj_pool_init(p_cli->p_cmd_hist_mempool);
    if (ret != NRF_SUCCESS)
    {
        return ret;
    }
    p_cli->p_ctx->p_cmd_list_head = NULL;
    p_cli->p_ctx->p_cmd_list_tail = NULL;
#endif

    memset(p_cli->p_ctx, 0, sizeof(nrf_cli_ctx_t));
    p_cli->p_ctx->tx_rdy = true;

#if NRF_MODULE_ENABLED(NRF_CLI_VT100_COLORS)
    p_cli->p_ctx->use_colors = use_colors;
#endif
    p_cli->p_ctx->state = NRF_CLI_STATE_INITIALIZED;
    p_cli->p_ctx->vt100_ctx.cons.terminal_wid = NRF_CLI_DEFAULT_TERMINAL_WIDTH;
    p_cli->p_ctx->vt100_ctx.cons.terminal_hei = NRF_CLI_DEFAULT_TERMINAL_HEIGHT;

    const char * * pp_sorted_cmds = (const char * *)CLI_SORTED_CMD_PTRS_START_ADDR_GET;
    for (size_t i = 0; i < CLI_DATA_SECTION_ITEM_COUNT; i++)
    {
        const nrf_cli_cmd_entry_t * cmd;
        cmd = CLI_DATA_SECTION_ITEM_GET(i);

        /* NULL syntax commands not allowed */
        ASSERT(cmd);
        ASSERT(cmd->u.p_static->p_syntax);

        pp_sorted_cmds[i] = cmd->u.p_static->p_syntax;
    }

    if (CLI_DATA_SECTION_ITEM_COUNT > 0)
    {
        qsort(pp_sorted_cmds,
              CLI_DATA_SECTION_ITEM_COUNT,
              sizeof (char *),
              string_cmp);
    }

    return NRF_SUCCESS;
}

#if NRF_MODULE_ENABLED(NRF_CLI_USES_TASK_MANAGER)
static ret_code_t nrf_cli_instance_uninit(nrf_cli_t const * p_cli);
void console_task(void * p_context)
{
    nrf_cli_t * p_cli = (nrf_cli_t *)p_context;

    ret_code_t ret = nrf_cli_start(p_cli);
    APP_ERROR_CHECK(ret);

    while (1)
    {
        uint32_t evts = task_events_wait(NRF_CLI_TASK_EVTS);

        if (evts & NRF_CLI_KILL_TASK_EVT)
        {
            (void)nrf_cli_instance_uninit(p_cli);
            task_exit();
        }
        else
        {
            nrf_cli_process(p_cli);
        }
    }
}
#endif

ret_code_t nrf_cli_init(nrf_cli_t const *  p_cli,
                        void const *       p_config,
                        bool               use_colors,
                        bool               log_backend,
                        nrf_log_severity_t init_lvl)
{
    ASSERT(p_cli);

    ret_code_t err_code = nrf_cli_instance_init(p_cli, p_config, use_colors);

#if NRF_CLI_LOG_BACKEND && NRF_MODULE_ENABLED(NRF_LOG)
    if ((err_code == NRF_SUCCESS) && log_backend && NRF_CLI_LOG_BACKEND)
    {
        int32_t id = nrf_log_backend_add(&p_cli->p_log_backend->backend, init_lvl);
        if (id < 0)
        {
            return NRF_ERROR_NO_MEM;
        }

        nrf_log_backend_enable(&p_cli->p_log_backend->backend);
    }
#endif
    return err_code;
}

ret_code_t nrf_cli_task_create(nrf_cli_t const * p_cli)
{
#if NRF_MODULE_ENABLED(NRF_CLI_USES_TASK_MANAGER)
    p_cli->p_ctx->task_id = task_create(console_task, p_cli->p_name,(void *)p_cli);
    if (p_cli->p_ctx->task_id == TASK_ID_INVALID)
    {
        return NRF_ERROR_NO_MEM;
    }
    else
    {
        return NRF_SUCCESS;
    }
#else
    return NRF_ERROR_NOT_SUPPORTED;
#endif
}

static ret_code_t nrf_cli_instance_uninit(nrf_cli_t const * p_cli)
{
    ASSERT(p_cli);
    ASSERT(p_cli->p_ctx && p_cli->p_iface && p_cli->p_name);

#if NRF_CLI_LOG_BACKEND && NRF_MODULE_ENABLED(NRF_LOG)
    if (p_cli->p_log_backend != NULL)
    {
        nrf_log_backend_disable(&p_cli->p_log_backend->backend);
        nrf_log_backend_remove(&p_cli->p_log_backend->backend);
    }
#endif

    ret_code_t ret = p_cli->p_iface->p_api->uninit(p_cli->p_iface);
    if (ret != NRF_SUCCESS)
    {
        return ret;
    }

#if NRF_MODULE_ENABLED(NRF_CLI_HISTORY)
    history_list_free_memory(p_cli);
#endif

    memset(p_cli->p_ctx, 0, sizeof(nrf_cli_ctx_t));
    p_cli->p_ctx->state = NRF_CLI_STATE_UNINITIALIZED;

    return NRF_SUCCESS;
}

ret_code_t nrf_cli_uninit(nrf_cli_t const * p_cli)
{
#if NRF_MODULE_ENABLED(NRF_CLI_USES_TASK_MANAGER)
    task_events_set(p_cli->p_ctx->task_id, NRF_CLI_KILL_TASK_EVT);
    return NRF_SUCCESS;
#else
    return nrf_cli_instance_uninit(p_cli);
#endif
}

ret_code_t nrf_cli_start(nrf_cli_t const * p_cli)
{
    ASSERT(p_cli);
    ASSERT(p_cli->p_ctx && p_cli->p_iface && p_cli->p_name);

    if (p_cli->p_ctx->state != NRF_CLI_STATE_INITIALIZED)
    {
        return NRF_ERROR_INVALID_STATE;
    }

#if NRF_MODULE_ENABLED(NRF_CLI_USES_TASK_MANAGER)
    void * p_context = (void *)((uint32_t)task_id_get());
    p_cli->p_log_backend->p_context = p_context;
#endif

    ret_code_t err_code = p_cli->p_iface->p_api->enable(p_cli->p_iface, false);

#if NRF_MODULE_ENABLED(NRF_CLI_USES_TASK_MANAGER)
    task_events_set(task_id_get(), NRF_CLI_TRANSPORT_RX_RDY_TASK_EVT);
#endif

    if (err_code == NRF_SUCCESS)
    {
#if NRF_CLI_VT100_COLORS_ENABLED
        vt100_color_set(p_cli, NRF_CLI_NORMAL);
        vt100_bgcolor_set(p_cli, NRF_CLI_VT100_COLOR_BLACK);
#endif
        nrf_fprintf(p_cli->p_fprintf_ctx, "\r\n\n");
        cli_state_change(p_cli, NRF_CLI_STATE_COLLECT);
    }

    return err_code;
}

ret_code_t nrf_cli_stop(nrf_cli_t const * p_cli)
{
    ASSERT(p_cli);
    ASSERT(p_cli->p_ctx && p_cli->p_iface && p_cli->p_name);

    if (p_cli->p_ctx->state == NRF_CLI_STATE_INITIALIZED ||
        p_cli->p_ctx->state == NRF_CLI_STATE_UNINITIALIZED)
    {
        return NRF_ERROR_INVALID_STATE;
    }

    cli_state_change(p_cli, NRF_CLI_STATE_INITIALIZED);
    return NRF_SUCCESS;
}

void nrf_cli_process(nrf_cli_t const * p_cli)
{
    ASSERT(p_cli);
    ASSERT(p_cli->p_ctx && p_cli->p_iface && p_cli->p_name);

    switch (p_cli->p_ctx->state)
    {
        case NRF_CLI_STATE_UNINITIALIZED:
        case NRF_CLI_STATE_INITIALIZED: /* console initialized but in stop state */
            /* do nothing here */
            break;
        case NRF_CLI_STATE_COLLECT:
        {
            cli_state_collect(p_cli);
            bool log_processed = cli_log_entry_process(p_cli, false);
            if (log_processed)
            {
                nrf_cli_fprintf(p_cli, NRF_CLI_INFO,   "%s", p_cli->p_name);
                nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "%s", p_cli->p_ctx->cmd_buff);
                cursor_position_synchronize(p_cli);
            }
            break;
        }
        case NRF_CLI_STATE_EXECUTE:
            cli_state_execute(p_cli);
            cli_state_change(p_cli, NRF_CLI_STATE_COLLECT);
            break;
        default:
            break;
    }
    NRF_CLI_IO_BUFFER_FLUSH(p_cli);
}

/* this function shall be only used by nrf_fprintf module */
void nrf_cli_print_stream(void const * p_user_ctx, char const * p_data, size_t data_len)
{
    cli_write((nrf_cli_t const *)p_user_ctx,
              p_data,
              data_len,
              NULL);
}

void nrf_cli_fprintf(nrf_cli_t const *      p_cli,
                     nrf_cli_vt100_color_t  color,
                     char const *           p_fmt,
                                            ...)
{
    ASSERT(p_fmt);
    ASSERT(p_cli);
    ASSERT(p_cli->p_ctx && p_cli->p_iface && p_cli->p_name);

    va_list args = {0};
    va_start(args, p_fmt);

#if NRF_MODULE_ENABLED(NRF_CLI_VT100_COLORS)
    if ((p_cli->p_ctx->use_colors) && (color != p_cli->p_ctx->vt100_ctx.col.col))
    {
        nrf_cli_vt100_colors_t col;

        vt100_colors_store(p_cli, &col);
        vt100_color_set(p_cli, color);

        nrf_fprintf_fmt(p_cli->p_fprintf_ctx, p_fmt, &args);

        vt100_colors_restore(p_cli, &col);
    }
    else
#endif // NRF_MODULE_ENABLED(NRF_CLI_VT100_COLORS)
    {
        nrf_fprintf_fmt(p_cli->p_fprintf_ctx, p_fmt, &args);
    }

    va_end(args);
}

/* Function prints string on terminal screen with requested margin.
 * It also takes care to not devide words.
 * p_cli:               Pointer to CLI instance.
 * str:                 String to be printed.
 * terminal_offset:     Requested left margin.
 * offset_first_line:   Add margin to first printed line.
 */
static void format_offset_string_print(nrf_cli_t const * p_cli,
                                       char const *      str,
                                       size_t            terminal_offset,
                                       bool              offset_first_line)
{
    if (str == NULL)
    {
        return;
    }

    if (offset_first_line)
    {
        cursor_right_move(p_cli, terminal_offset);
    }

    size_t length;
    size_t offset = 0;

    /* skip whitespaces */
    while (isspace((int)*(str + offset)))
    {
       ++offset;
    }

    while (1)
    {
        size_t idx = 0;
        length = cli_strlen(str) - offset;

        if (length <= p_cli->p_ctx->vt100_ctx.cons.terminal_wid - terminal_offset)
        {
            for (idx = 0; idx < length; idx++)
            {
                if (*(str + offset + idx) == '\n')
                {
                    NRF_CLI_IO_BUFFER_FLUSH(p_cli);
                    cli_write(p_cli, str + offset, idx, NULL);
                    offset += idx + 1;
                    cursor_next_line_move(p_cli);
                    cursor_right_move(p_cli, terminal_offset);
                    break;
                }
            }
            /* string will fit in one line */
            nrf_fprintf(p_cli->p_fprintf_ctx, str + offset);
            break;
        }
        else
        {
            /* in case some words are longer than line length they will be devided
               at the end of line. */
            length = p_cli->p_ctx->vt100_ctx.cons.terminal_wid - terminal_offset;

            while (1)
            {
                /* determining line break */
                if (isspace((int)(*(str + offset + idx))))
                {
                    length = idx;
                    if (*(str + offset + idx) == '\n')
                    {
                        break;
                    }
                }
                if ((idx + terminal_offset) >= p_cli->p_ctx->vt100_ctx.cons.terminal_wid)
                {
                    /* end of line reached */
                    break;
                }
                ++idx;
            }

            /* writing one line */
            NRF_CLI_IO_BUFFER_FLUSH(p_cli);
            cli_write(p_cli, str + offset, length, NULL);
            offset += length;

            /* calculating text offset to ensure that next line will not begin with a space */
            while (isspace((int)(*(str + offset))))
            {
                ++offset;
            }
            cursor_next_line_move(p_cli);
            cursor_right_move(p_cli, terminal_offset);
        }
    }
    cursor_next_line_move(p_cli);
}

void nrf_cli_help_print(nrf_cli_t const *               p_cli,
                        nrf_cli_getopt_option_t const * p_opt,
                        size_t                          opt_len)
{
    ASSERT(p_cli);
    ASSERT(p_cli->p_ctx && p_cli->p_iface && p_cli->p_name);

    static uint8_t const tab_len = 2;
    static char const opt_sep[] =", "; /* options separator */
    static char const help[] = "-h, --help";
    static char const cmd_sep[] = " - "; /* command separator */
    uint16_t field_width = 0;
    uint16_t longest_string = cli_strlen(help) - cli_strlen(opt_sep);

    /* Printing command help string */
    nrf_cli_fprintf(p_cli,
                    NRF_CLI_NORMAL,
                    "%s%s", p_cli->p_ctx->p_current_stcmd->p_syntax, cmd_sep);

    field_width = cli_strlen(p_cli->p_ctx->p_current_stcmd->p_syntax) + cli_strlen(cmd_sep);
    format_offset_string_print(p_cli, p_cli->p_ctx->p_current_stcmd->p_help, field_width, false);

    /* Printing options */
    nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "Options:\r\n");

    /* looking for the longest option */
    if ((opt_len > 0) && (p_opt != NULL))
    {
        for (size_t i = 0; i < opt_len; ++i)
        {
            if (cli_strlen(p_opt[i].p_optname_short) + cli_strlen(p_opt[i].p_optname)
                    > longest_string)
            {
                longest_string = cli_strlen(p_opt[i].p_optname_short)
                    + cli_strlen(p_opt[i].p_optname);
            }
        }
    }
    longest_string += cli_strlen(opt_sep) + tab_len;

    nrf_cli_fprintf(p_cli,
                    NRF_CLI_NORMAL,
                    "  %-*s:",
                    longest_string,
                    help);

    /* print help string for options: -h and --help */
    field_width = longest_string + tab_len + 1; /* tab_len + 1 == "  " and ':' from: "  %-*s:" */
    format_offset_string_print(p_cli, "Show command help.", field_width, false);

    /* formating and printing all available options (except -h, --help) */
    if (p_opt != NULL)
    {
        for (size_t i = 0; i < opt_len; ++i)
        {
            if ((p_opt[i].p_optname_short != NULL) && (p_opt[i].p_optname != NULL))
            {
                nrf_cli_fprintf(p_cli,
                                NRF_CLI_NORMAL,
                                "  %s%s%s",
                                p_opt[i].p_optname_short,
                                opt_sep,
                                p_opt[i].p_optname);
                field_width = longest_string + tab_len;
                cursor_right_move(p_cli,
                                  field_width - ( cli_strlen(p_opt[i].p_optname_short)
                                                + cli_strlen(p_opt[i].p_optname)
                                                + tab_len
                                                + cli_strlen(opt_sep)));
                cli_putc(p_cli, ':');
                ++field_width;  /* incrementing because char ':' was printed above */
            }
            else if (p_opt[i].p_optname_short != NULL)
            {
                nrf_cli_fprintf(p_cli,
                                NRF_CLI_NORMAL,
                                "  %-*s:",
                                longest_string,
                                p_opt[i].p_optname_short);
                /* tab_len + 1 == "  " and ':' from: "  %-*s:" */
                field_width = longest_string + tab_len + 1;
            }
            else if (p_opt[i].p_optname != NULL)
            {
                nrf_cli_fprintf(p_cli,
                                NRF_CLI_NORMAL,
                                "  %-*s:",
                                longest_string,
                                p_opt[i].p_optname);
                /* tab_len + 1 == "  " and ':' from: "  %-*s:" */
                field_width = longest_string + tab_len + 1;
            }
            else
            {
                /* do nothing */
            }

            if (p_opt[i].p_optname_help != NULL)
            {
                format_offset_string_print(p_cli, p_opt[i].p_optname_help, field_width, false);
            }
            else
            {
                cursor_next_line_move(p_cli);
            }
        }
    }

    /* check if there are subcommands */
    if (p_cli->p_ctx->p_current_stcmd->p_subcmd == NULL)
    {
        return;
    }

    /* Print formated subcommands help - only 1 level */
    nrf_cli_static_entry_t static_entry;
    nrf_cli_cmd_entry_t const * p_cmd = p_cli->p_ctx->p_current_stcmd->p_subcmd;
    nrf_cli_static_entry_t const * p_st_cmd = NULL;

    field_width = 0;
    longest_string = 0;

    size_t cmd_idx = 0;

    /* looking for the longest subcommand to print */
    while (1)
    {
        cmd_get(p_cmd, !NRF_CLI_CMD_ROOT_LVL, cmd_idx++, &p_st_cmd, &static_entry);

        if (p_st_cmd == NULL)
        {
            break;
        }
        if (cli_strlen(p_st_cmd->p_syntax) > longest_string)
        {
            longest_string = cli_strlen(p_st_cmd->p_syntax);
        }
    }

    /* check if there are dynamic subcommands */
    if (cmd_idx == 1)
    {
        return;
    }

    nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "Subcommands:\r\n");

    /* printing subcommands and help string if exists */
    cmd_idx = 0;
    while (1)
    {
        cmd_get(p_cmd, !NRF_CLI_CMD_ROOT_LVL, cmd_idx++, &p_st_cmd, &static_entry);

        if (p_st_cmd == NULL)
        {
            break;
        }

        field_width = longest_string + tab_len;
        nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL,"  %-*s:", field_width, p_st_cmd->p_syntax);
        field_width += tab_len + 1; /* tab_len + 1 == "  " and ':' from: "  %-*s:" */

        if (p_st_cmd->p_help != NULL)
        {
            format_offset_string_print(p_cli, p_st_cmd->p_help, field_width, false);
        }
        else
        {
            cursor_next_line_move(p_cli);
        }
    }
}


#if NRF_CLI_LOG_BACKEND && NRF_MODULE_ENABLED(NRF_LOG)
static bool cli_log_entry_process(nrf_cli_t const * p_cli, bool skip)
{
    nrf_log_entry_t entry;

    if (nrf_queue_pop(p_cli->p_log_backend->p_queue, &entry) != NRF_SUCCESS)
    {
        return false;
    }

    if (skip)
    {
        nrf_memobj_put(entry);
        return true;
    }
    {
        /* errasing currently displayed command and console name */
        nrf_cli_multiline_cons_t const * cons = multiline_console_data_check(p_cli);

        if (cons->cur_y > NRF_CLI_INITIAL_CURS_POS)
        {
            cursor_up_move(p_cli, cons->cur_y - NRF_CLI_INITIAL_CURS_POS);
        }

        if (cons->cur_x > NRF_CLI_INITIAL_CURS_POS)
        {
            cursor_left_move(p_cli, cons->cur_x - NRF_CLI_INITIAL_CURS_POS);
        }
        cli_clear_eos(p_cli);
    }

    /* printing logs from queue */
    do
    {
        nrf_log_header_t header;
        uint32_t         memobj_offset = 0;
        nrf_log_str_formatter_entry_params_t params;

        nrf_memobj_read(entry, &header, HEADER_SIZE*sizeof(uint32_t), memobj_offset);
        memobj_offset = HEADER_SIZE * sizeof(uint32_t);

        params.timestamp  = header.timestamp;
        params.module_id  = header.module_id;
        params.use_colors = NRF_LOG_USES_COLORS; /* color will be provided by console application */

        if (header.base.generic.type == HEADER_TYPE_STD)
        {
            char const * p_log_str = (char const *)((uint32_t)header.base.std.addr);
            params.severity  = (nrf_log_severity_t)header.base.std.severity;
            params.raw        = header.base.std.raw;
            uint32_t nargs = header.base.std.nargs;
            uint32_t args[6];

            nrf_memobj_read(entry, args, nargs*sizeof(uint32_t), memobj_offset);
            nrf_log_std_entry_process(p_log_str,
                                      args,
                                      nargs,
                                      &params,
                                      p_cli->p_fprintf_ctx);
        }
        else if (header.base.generic.type == HEADER_TYPE_HEXDUMP)
        {
            uint32_t data_len;
            uint8_t data_buf[8];
            uint32_t chunk_len;

            data_len        = header.base.hexdump.len;
            params.severity = (nrf_log_severity_t)header.base.hexdump.severity;
            params.raw      = header.base.hexdump.raw;

            do
            {
                chunk_len = sizeof(data_buf) > data_len ? data_len : sizeof(data_buf);
                nrf_memobj_read(entry, data_buf, chunk_len, memobj_offset);
                memobj_offset += chunk_len;
                data_len      -= chunk_len;
                nrf_log_hexdump_entry_process(data_buf, chunk_len, &params, p_cli->p_fprintf_ctx);
            } while (data_len > 0);
        }

        nrf_memobj_put(entry);
    } while (nrf_queue_pop(p_cli->p_log_backend->p_queue, &entry) == NRF_SUCCESS);
    return true;
}

static void nrf_log_backend_cli_put(nrf_log_backend_t const * p_backend, nrf_log_entry_t * p_msg)
{
    nrf_cli_log_backend_t * p_backend_cli = CONTAINER_OF(p_backend, nrf_cli_log_backend_t, backend);
    nrf_cli_t const * p_cli = p_backend_cli->p_cli;

    //If panic mode cannot be handled stop handling new requests.
    if (p_cli->p_ctx->state != NRF_CLI_STATE_PANIC_MODE_INACTIVE)
    {
        bool panic_mode = (p_cli->p_ctx->state == NRF_CLI_STATE_PANIC_MODE_ACTIVE);
        //If there is no place for new log entry remove the oldest one
        ret_code_t err_code = nrf_queue_push(p_backend_cli->p_queue, &p_msg);
        while (err_code != NRF_SUCCESS)
        {
            (void)cli_log_entry_process(p_cli, panic_mode ? false : true);

            err_code = nrf_queue_push(p_backend_cli->p_queue, &p_msg);
        }
        nrf_memobj_get(p_msg);

        if (panic_mode)
        {
            (void)cli_log_entry_process(p_cli, false);
        }
#if NRF_MODULE_ENABLED(NRF_CLI_USES_TASK_MANAGER)
        else
        {
            task_events_set((task_id_t)((uint32_t)p_backend_cli->p_context & 0x000000FF),
                            NRF_CLI_LOG_PENDING_TASK_EVT);
        }
#endif
    }
}

static void nrf_log_backend_cli_flush(nrf_log_backend_t const * p_backend)
{
    UNUSED_PARAMETER(p_backend);
}

static void nrf_log_backend_cli_panic_set(nrf_log_backend_t const * p_backend)
{
    nrf_cli_log_backend_t * p_backend_cli = CONTAINER_OF(p_backend, nrf_cli_log_backend_t, backend);
    nrf_cli_t const * p_cli = p_backend_cli->p_cli;

    if (p_cli->p_iface->p_api->enable(p_cli->p_iface, true) == NRF_SUCCESS)
    {
        p_cli->p_ctx->state = NRF_CLI_STATE_PANIC_MODE_ACTIVE;
    }
    else
    {
        p_cli->p_ctx->state = NRF_CLI_STATE_PANIC_MODE_INACTIVE;
    }
}

const nrf_log_backend_api_t nrf_log_backend_cli_api = {
        .put       = nrf_log_backend_cli_put,
        .flush     = nrf_log_backend_cli_flush,
        .panic_set = nrf_log_backend_cli_panic_set,
};
#else
static bool cli_log_entry_process(nrf_cli_t const * p_cli, bool skip)
{
    UNUSED_PARAMETER(p_cli);
    UNUSED_PARAMETER(skip);
    return false;
}
#endif // NRF_CLI_LOG_BACKEND

/* ============ build-in commands ============ */
#if NRF_MODULE_ENABLED(NRF_CLI_BUILD_IN_CMDS)
static void nrf_cli_cmd_clear(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    ASSERT(p_cli);
    ASSERT(p_cli->p_ctx && p_cli->p_iface && p_cli->p_name);
    UNUSED_PARAMETER(argv);

    if ((argc == 2) && (nrf_cli_help_requested(p_cli)))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }
    NRF_CLI_VT100_CMD(p_cli, NRF_CLI_VT100_CURSORHOME);
    NRF_CLI_VT100_CMD(p_cli, NRF_CLI_VT100_CLEARSCREEN);
}

#if NRF_MODULE_ENABLED(NRF_CLI_VT100_COLORS)
static void nrf_cli_cmd_colors_off(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    if (nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }

    if (argc != 1)
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s:%s", argv[0], m_nrf_cli_bad_param_count);
        return;
    }
    p_cli->p_ctx->use_colors = false;
}

static void nrf_cli_cmd_colors_on(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    if (nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }

    if (argc != 1)
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s:%s", argv[0], m_nrf_cli_bad_param_count);
        return;
    }
    p_cli->p_ctx->use_colors = true;
}

static void nrf_cli_cmd_colors(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    ASSERT(p_cli);
    ASSERT(p_cli->p_ctx && p_cli->p_iface && p_cli->p_name);

    if ((argc == 1) || (nrf_cli_help_requested(p_cli)))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }

    if (argc != 2)
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s:%s", argv[0], m_nrf_cli_bad_param_count);
        return;
    }

    nrf_cli_fprintf(p_cli,
                    NRF_CLI_ERROR,
                    "%s:%s%s\r\n",
                    argv[0],
                    m_nrf_cli_uknown_parameter,
                    argv[1]);
}
#endif // NRF_MODULE_ENABLED(NRF_CLI_VT100_COLORS)

#if NRF_MODULE_ENABLED(NRF_CLI_HISTORY)
static void nrf_cli_cmd_history(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    ASSERT(p_cli);
    ASSERT(p_cli->p_ctx && p_cli->p_iface && p_cli->p_name);

    if (nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }

    if (argc > 1)
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s:%s", argv[0], m_nrf_cli_bad_param_count);
        return;
    }

    size_t i = 0;
    nrf_memobj_t const * p_cmd_list = p_cli->p_ctx->p_cmd_list_tail;
    nrf_cli_memobj_header_t header;

    while (1)
    {
        if ((p_cmd_list == NULL) || (i >= NRF_CLI_HISTORY_ELEMENT_COUNT))
        {
            break;
        }
        nrf_memobj_read((nrf_memobj_t * )p_cmd_list,
                        &header,
                        NRF_CLI_HISTORY_HEADER_SIZE,
                        (uint32_t)0);
        nrf_memobj_read((nrf_memobj_t * )p_cmd_list,
                        p_cli->p_ctx->temp_buff,
                        header.cmd_len + 1,
                        (uint32_t)NRF_CLI_HISTORY_HEADER_SIZE);
        p_cmd_list = header.p_next;
        nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "[%3d] %s\r\n", i++, p_cli->p_ctx->temp_buff);
    }
    p_cli->p_ctx->temp_buff[0] = '\0';
}
#endif // NRF_MODULE_ENABLED(NRF_CLI_HISTORY)

static void nrf_cli_cmd_resize_default(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    if (nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }

    if (argc != 1)
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s:%s", argv[0], m_nrf_cli_bad_param_count);
        return;
    }

    NRF_CLI_VT100_CMD(p_cli, NRF_CLI_VT100_SETCOL_80);
    p_cli->p_ctx->vt100_ctx.cons.terminal_wid = NRF_CLI_DEFAULT_TERMINAL_WIDTH;
    p_cli->p_ctx->vt100_ctx.cons.terminal_hei = NRF_CLI_DEFAULT_TERMINAL_HEIGHT;
}

static void nrf_cli_cmd_resize(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    ASSERT(p_cli);
    ASSERT(p_cli->p_ctx && p_cli->p_iface && p_cli->p_name);

    if (argc > 2)
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "%s:%s", argv[0], m_nrf_cli_bad_param_count);
        return;
    }

    if (argc == 1)
    {
        if (terminal_size_get(p_cli,
                              &p_cli->p_ctx->vt100_ctx.cons.terminal_wid,
                              &p_cli->p_ctx->vt100_ctx.cons.terminal_hei) != NRF_SUCCESS)
        {
            p_cli->p_ctx->vt100_ctx.cons.terminal_wid = NRF_CLI_DEFAULT_TERMINAL_WIDTH;
            p_cli->p_ctx->vt100_ctx.cons.terminal_hei = NRF_CLI_DEFAULT_TERMINAL_HEIGHT;
            nrf_cli_fprintf(p_cli,
                            NRF_CLI_WARNING,
                            "no response from terminal, assumed 80x24 chars screen size\r\n");
        }
        return;
    }

    if (nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
    }
    else
    {
        nrf_cli_fprintf(p_cli,
                        NRF_CLI_ERROR,
                        "%s:%s%s\r\n",
                        argv[0],
                        m_nrf_cli_uknown_parameter,
                        argv[1]);
    }
}

#if NRF_MODULE_ENABLED(NRF_CLI_VT100_COLORS)
NRF_CLI_CREATE_STATIC_SUBCMD_SET(m_sub_colors)
{
    NRF_CLI_CMD(off, NULL, "Disable colored syntax.", nrf_cli_cmd_colors_off),
    NRF_CLI_CMD(on,  NULL, "Enable colored syntax.", nrf_cli_cmd_colors_on),
    NRF_CLI_SUBCMD_SET_END
};
NRF_CLI_CMD_REGISTER(colors, &m_sub_colors, "Toggle colored syntax.", nrf_cli_cmd_colors);
#endif

NRF_CLI_CMD_REGISTER(clear, NULL, "Clear screen.", nrf_cli_cmd_clear);

#if NRF_MODULE_ENABLED(NRF_CLI_HISTORY)
NRF_CLI_CMD_REGISTER(history, NULL, "Command history.", nrf_cli_cmd_history);
#endif

NRF_CLI_CREATE_STATIC_SUBCMD_SET(m_sub_resize)
{
    NRF_CLI_CMD(default,
                NULL,
                "Assume 80 chars screen width and send this settings to the terminal.",
                nrf_cli_cmd_resize_default),
    NRF_CLI_SUBCMD_SET_END
};
NRF_CLI_CMD_REGISTER(resize,
                     &m_sub_resize,
                     "Console gets terminal screen size or assumes 80 in case readout fails. "
                     "It must be executed after each terminal width change to ensure correct "
                     "text display.",
                     nrf_cli_cmd_resize);

#endif // NRF_MODULE_ENABLED(NRF_CLI_BUILD_IN_CMDS)

#endif // NRF_MODULE_ENABLED(NRF_CLI)
