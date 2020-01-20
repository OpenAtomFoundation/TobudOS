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

#ifndef _TOS_SHELL_COMMAND_H_
#define  _TOS_SHELL_COMMAND_H_

#include "tos_k.h"

typedef int (*shell_cmd_handler_t)(int argc, char *argv[]);

typedef struct shell_command_st {
    const char         *name;
    shell_cmd_handler_t handler;
    const char         *help;
} shell_cmd_t;

typedef struct shell_command_set_st {
    k_slist_t                   list;
    const shell_cmd_t *const    commands;
} shell_cmd_set_t;

__KERNEL__ int shell_cmd_set_regiser(const shell_cmd_set_t *cmd_set);

__KERNEL__ int shell_cmd_set_unregiser(const shell_cmd_set_t *cmd_set);

__KERNEL__ const shell_cmd_t *shell_cmd_find(const char *name);

__KERNEL__ void shell_cmd_init(shell_cmd_set_t *custom_cmd_set);

#endif /* _TOS_SHELL_COMMAND_H_ */

