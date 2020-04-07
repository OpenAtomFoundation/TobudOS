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

#include "tos_k.h"
#include "tos_at_socket_prv.h"
#include "sal_module_wrapper.h"

static socket_ctl_t socket_ctl_table[SOCKET_CTL_MAX] = { 0 };

int socket_id_sanity_check(int id)
{
    if (id < 0 ||
        id >= TOS_COUNT_OF(socket_ctl_table) ||
        !socket_ctl_table[id].is_taken) {
        return K_FALSE;
    }

    return K_TRUE;
}

int socket_id_set_sanity_check(int id)
{
    if (!socket_id_sanity_check(id)) {
        return K_FALSE;
    }

    if (socket_ctl_table[id].fd == -1) {
        return K_FALSE;
    }

    return K_TRUE;
}

int socket_id_alloc(void)
{
    int i = 0;
    socket_ctl_t *the_ctl;

    for (i = 0; i < TOS_COUNT_OF(socket_ctl_table); ++i) {
        the_ctl = &socket_ctl_table[i];
        if (!the_ctl->is_taken) {
            the_ctl->is_taken   = K_TRUE;
            the_ctl->fd         = -1;
            return i;
        }
    }

    return -1;
}

void socket_id_free(int id)
{
    socket_ctl_table[id].is_taken   = K_FALSE;
    socket_ctl_table[id].fd         = -1;
}

void socket_set_protocol(int id, sal_proto_t protocol)
{
    socket_ctl_table[id].protocol = protocol;
}

sal_proto_t socket_get_protocol(int id)
{
    return socket_ctl_table[id].protocol;
}

void socket_set_fd(int id, int fd)
{
    socket_ctl_table[id].fd = fd;
}

int socket_get_fd(int id)
{
    return socket_ctl_table[id].fd;
}

