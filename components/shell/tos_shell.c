/*----------------------------------------------------------------------------
 * Tencent is pleased to support the open source community by making TencentOS
 * available.
 *
 * Copyright (C) 2019 THL A29 Limited, a Tencent company. All rights reserved.
 * If you have downloaded a copy of the TencentOS binary from Tencent, please
 * note that the TencentOS binary is licensed under the BSD 3-Clause License.
 *
 * If you have downloaded a copy of the TencentOS source code from Tencent,
 * please note that TencentOS source code is licensed under the BSD 3-Clause
 * License, except for the third-party components listed below which are
 * subject to different license terms. Your integration of TencentOS into your
 * own projects may require compliance with the BSD 3-Clause License, as well
 * as the other licenses applicable to the third-party components included
 * within TencentOS.
 *---------------------------------------------------------------------------*/

#include "tos_shell.h"

__STATIC__ shell_ctl_t shell_ctl;

__STATIC__ k_stack_t shell_parser_task_stack[SHELL_PARSER_TASK_STACK_SIZE];

__STATIC__ int shell_getchar(void)
{
    uint8_t chr;
    k_err_t err;

    if (tos_sem_pend(&SHELL_CTL->shell_rx_sem, TOS_TIME_FOREVER) != K_ERR_NONE) {
        return -1;
    }

    err = tos_chr_fifo_pop(&SHELL_CTL->shell_rx_fifo, &chr);

    return err == K_ERR_NONE ? chr : -1;
}

__STATIC__ void shell_putchar(const char ch)
{
    SHELL_CTL->output(ch);
}

__STATIC__ void shell_puts(const char *s)
{
    while (*s) {
        SHELL_CTL->output(*s++);
    }
}

__STATIC__ int shell_readline(void)
{
    int chr;
    char *buf = SHELL_CTL->cmd_buffer;
    uint16_t line_length = 0;

    while (K_TRUE) {
        if (buf - SHELL_CTL->cmd_buffer >= (SHELL_CTL->cmd_buffer_size - 1)) {
            return -1;
        }

        chr = shell_getchar();
        if (chr < 0) {
            return -1;
        }

        if (chr == '\n' || chr == '\r') {
            *buf = '\0';
            tos_shell_printf("\r\n");
            break;
        } else if (chr == '\t'){
            *buf = '\0';
            tos_shell_printf("\r\n");
            cmd_help(0, NULL);
            break;
        }
        
        shell_putchar(chr);
        
        *buf++ = chr;
        line_length++;
    }
    return line_length;
}

__STATIC__ void shell_cmd_do_process(int argc, char *argv[])
{
    const shell_cmd_t *cmd;
    static const char *cmd_not_found = "command not found\r\n";

    cmd = shell_cmd_find(argv[0]);
    if (!cmd) {
        tos_shell_printf(cmd_not_found);
    } else {
        cmd->handler(argc, argv);
    }
}

__STATIC__ void shell_cmd_process(void)
{
    int argc = 0;
    static char *argv[SHELL_CMD_ARGV_MAX];
    char *pos = SHELL_CTL->cmd_buffer;

    // left strip
    while (*pos == ' ' || *pos == '\t') {
        ++pos;
    }

    if (!*pos) {
        return;
    }

    /* cmd_demo arg0 arg1 arg2 */
    /* simple, ugly, but works */
    while (*pos) {
        argv[argc] = pos;
        while (*pos != ' ' && *pos != '\t' && *pos) {
            ++pos;
        }

        while (*pos == ' ' || *pos == '\t') {
            *pos++ = '\0';
        }

        ++argc;
    }

    shell_cmd_do_process(argc, argv);
}

__STATIC__ void shell_prompt(void)
{
    tos_shell_printf("tshell>");
}

__STATIC__ void shell_parser(void *arg)
{
    int rc;

    shell_prompt();

    while (K_TRUE) {
        rc = shell_readline();

        if (rc > 0) {
            shell_cmd_process();
        }

        shell_prompt();
    }
}

__API__ int tos_shell_init(char *cmd_buf, int cmd_buf_size, shell_output_t output, shell_cmd_set_t *custom_cmd_set)
{
    void *buffer = K_NULL;

    TOS_PTR_SANITY_CHECK_RC(cmd_buf, -1);
    TOS_PTR_SANITY_CHECK_RC(output, -1);

    memset(SHELL_CTL, 0, sizeof(shell_ctl_t));

    SHELL_CTL->cmd_buffer       = cmd_buf;
    SHELL_CTL->cmd_buffer_size  = cmd_buf_size;
    SHELL_CTL->output           = output;

    shell_cmd_init(custom_cmd_set);

    buffer = tos_mmheap_alloc(cmd_buf_size * 2);
    if (!buffer) {
        return -1;
    }

    SHELL_CTL->shell_rx_fifo_buffer = (uint8_t *)buffer;
    tos_chr_fifo_create(&SHELL_CTL->shell_rx_fifo, buffer, cmd_buf_size * 2);

    if (tos_sem_create(&SHELL_CTL->shell_rx_sem, (k_sem_cnt_t)0u) != K_ERR_NONE) {
        goto errout0;
    }

    if (tos_task_create(&SHELL_CTL->parser, "shell_parser", shell_parser,
                        K_NULL, SHELL_PARSER_TASK_PRIO, shell_parser_task_stack,
                        SHELL_PARSER_TASK_STACK_SIZE, 0) != K_ERR_NONE) {
        goto errout1;
    }

    return 0;

errout1:
    tos_sem_destroy(&SHELL_CTL->shell_rx_sem);

errout0:
    tos_mmheap_free(SHELL_CTL->shell_rx_fifo_buffer);
    SHELL_CTL->shell_rx_fifo_buffer = K_NULL;
    tos_chr_fifo_destroy(&SHELL_CTL->shell_rx_fifo);

    return -1;
}

__API__ void tos_shell_deinit(void)
{
    tos_task_destroy(&SHELL_CTL->parser);
    tos_sem_destroy(&SHELL_CTL->shell_rx_sem);

    tos_mmheap_free(SHELL_CTL->shell_rx_fifo_buffer);
    SHELL_CTL->shell_rx_fifo_buffer = K_NULL;
    tos_chr_fifo_destroy(&SHELL_CTL->shell_rx_fifo);
}

__API__ int tos_shell_cmd_set_regiser(const shell_cmd_set_t *cmd_set)
{
    return shell_cmd_set_regiser(cmd_set);
}

__API__ int tos_shell_cmd_set_unregiser(const shell_cmd_set_t *cmd_set)
{
    return shell_cmd_set_unregiser(cmd_set);
}

__API__ void tos_shell_printf(const char *format, ...)
{
    va_list args;
    static char buffer[SHELL_OUTPUT_MAX];

    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    shell_puts(buffer);
}

__API__ void tos_shell_printfln(const char *format, ...)
{
    va_list args;
    static char buffer[SHELL_OUTPUT_MAX];

    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    shell_puts(buffer);
    shell_puts("\r\n");
}

__API__ void tos_shell_input_byte(uint8_t data)
{
    if (tos_chr_fifo_push(&SHELL_CTL->shell_rx_fifo, data) == K_ERR_NONE) {
        tos_sem_post(&SHELL_CTL->shell_rx_sem);
    }
}

