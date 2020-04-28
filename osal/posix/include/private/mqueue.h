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

#ifndef _POSIX_PRIVATE_MQUEUE_H_
#define  _POSIX_PRIVATE_MQUEUE_H_

#include "tos_k.h"

#include "private/posix_config.h"

__CDECLS_BEGIN

#define MQUEUE_MAX                  (POSIX_CFG_MQUEUE_MAX)

#define MQUEUE_MSG_MAX              20
#define MQUEUE_MSG_SIZE_MAX         50

typedef struct mqueue_control_st {
    mqd_t           id;
    k_prio_mail_q_t kprio_mail_q;
} mqueue_ctl_t;

__KNL__ int mqueue_id_add(mqd_t id, mqueue_ctl_t *mqueue_ctl);

__KNL__ mqd_t mqueue_id_alloc(void);

__KNL__ int mqueue_id_free(mqd_t id);

__KNL__ mqueue_ctl_t *mqueue_by_id(mqd_t id);

__CDECLS_END

#endif /* _POSIX_PRIVATE_TIMER_H_*/
