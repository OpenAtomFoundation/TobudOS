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

#include "stdio.h"
#include "string.h"
#include "proc_bar.h"

int proc_bar_init(proc_bar_t *proc_bar, int max)
{
    const char *stat = { "-\\|/" };

    if (!proc_bar || max <= 0) {
        return -1;
    }

    memset(proc_bar->buf, 0, sizeof(proc_bar->buf));
    proc_bar->cursor    = 0;
    proc_bar->max       = max;

    printf("[%-101s][%%%d]%c\r", proc_bar->buf, 0, stat[proc_bar->cursor % 4]);
    fflush(stdout);

    return 0;
}

int proc_bar_update(proc_bar_t *proc_bar)
{
    int percentage, i = 0;
    const char *stat = { "-\\|/" };

    if (!proc_bar || proc_bar->cursor >= proc_bar->max) {
        return -1;
    }

    percentage = (++proc_bar->cursor) * 100 / proc_bar->max;

    for (i = 0; i <= percentage; ++i) {
        proc_bar->buf[i] = '#';
    }

    printf("[%-101s][%%%d]%c\r", proc_bar->buf, percentage, stat[proc_bar->cursor % 4]);
    fflush(stdout);

    return 0;
}

