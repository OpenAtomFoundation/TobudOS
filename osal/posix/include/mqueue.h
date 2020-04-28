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

#ifndef _POSIX_MQUEUE_H_
#define  _POSIX_MQUEUE_H_

#include "tos_k.h"

#include "signal.h"
#include "time.h"

__CDECLS_BEGIN

typedef int         mqd_t;

struct mq_attr {
    long    mq_flags;       /* message queue flags */
    long    mq_maxmsg;      /* maximum number of messages */
    long    mq_msgsize;     /* maximum message size */
    long    mq_curmsgs;     /* number of messages currently queued */
};

#define __NOTSUPP__

__API__     int     mq_close(mqd_t mqdes);
__NOTSUPP__ int     mq_getattr(mqd_t mqdes, struct mq_attr *mqstat);
__NOTSUPP__ int     mq_notify(mqd_t mqdes, const struct sigevent *notification);
__API__     mqd_t   mq_open(const char *name, int oflag, ...);
__API__     ssize_t mq_receive(mqd_t mqdes, char *msg_ptr, size_t msg_len, unsigned *msg_prio);
__API__     int     mq_send(mqd_t mqdes, const char *msg_ptr, size_t msg_len, unsigned msg_prio);
__NOTSUPP__ int     mq_setattr(mqd_t mqdes,     const struct mq_attr *mqstat, struct mq_attr *omqstat);
__API__     ssize_t mq_timedreceive(mqd_t mqdes, char *msg_ptr, size_t msg_len, unsigned *msg_prio, const struct timespec *abstime);
__NOTSUPP__ int     mq_timedsend(mqd_t mqdes, const char *msg_ptr, size_t msg_len, unsigned msg_prio, const struct timespec *abstime);
__NOTSUPP__ int     mq_unlink(const char *name);

__CDECLS_END

#endif /* _POSIX_MQUEUE_H_ */

