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

#ifndef _TOS_SHELL_H_
#define  _TOS_SHELL_H_

#include "tos_k.h"
#include "tos_shell_command.h"

#define SHELL_CMD_ARGV_MAX                      16
#define SHELL_OUTPUT_MAX                        256
#define SHELL_PARSER_TASK_STACK_SIZE            1024
#define SHELL_PARSER_TASK_PRIO                  3

typedef void (*shell_output_t)(const char *str);

typedef struct shell_control_st {
    k_task_t            parser;

    shell_output_t      output;

    char               *cmd_buffer;
    int                 cmd_buffer_size;

    k_sem_t             shell_rx_sem;
    k_chr_fifo_t        shell_rx_fifo;
    uint8_t            *shell_rx_fifo_buffer;
} shell_ctl_t;

#define SHELL_CTL   ((shell_ctl_t *)(&shell_ctl))

__API__ int tos_shell_init(char *cmd_buf, int cmd_buf_size, shell_output_t output, shell_cmd_set_t *custom_cmd_set);

__API__ void tos_shell_deinit(void);

__API__ int tos_shell_cmd_set_regiser(const shell_cmd_set_t *cmd_set);

__API__ int tos_shell_cmd_set_unregiser(const shell_cmd_set_t *cmd_set);

__API__ void tos_shell_printf(const char *format, ...);

__API__ void tos_shell_input_byte(uint8_t data);

#endif /* _TOS_SHELL_H_ */

