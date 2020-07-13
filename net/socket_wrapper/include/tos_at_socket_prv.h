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

#ifndef  _TOS_AT_SOCKET_PRV_H_
#define  _TOS_AT_SOCKET_PRV_H_

#include "sal_module_wrapper.h"

#define SOCKET_CTL_MAX      6 /* should consistent with AT_DATA_CHANNEL_NUM */

typedef struct socket_control_st {
    int             fd;         /* the real channel id allocated by at framework.
                                    for some module like NBIot(bc35), you only know the channel id until you really do the
                                    connect to the server */
    int             is_taken;   /* is this control usable?(if is_taken is FALSE then it's not usable) */
    sal_proto_t     protocol;
} socket_ctl_t;

int socket_id_sanity_check(int id);

int socket_id_set_sanity_check(int id);

int socket_id_alloc(void);

void socket_id_free(int id);

void socket_set_protocol(int id, sal_proto_t protocol);

sal_proto_t socket_get_protocol(int id);

void socket_set_fd(int id, int fd);

int socket_get_fd(int id);

#define SOCKET_ID_SANITY_CHECK(id)          \
    if (!socket_id_sanity_check(id)) {      \
        return -1;                          \
    }

#define SOCKET_ID_SET_SANITY_CHECK(id)          \
    if (!socket_id_set_sanity_check(id)) {      \
        return -1;                              \
    }

#endif /* _TOS_AT_SOCKET_PRV_H_ */

