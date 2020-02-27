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

#include "private/posix_config.h"

#include "errno.h"
#include "mqueue.h"
#include "private/time.h"
#include "private/mqueue.h"

#if POSIX_CFG_MQUEUE_EN > 0u

__API__ int mq_close(mqd_t mqdes)
{
    k_err_t kerr;
    mqueue_ctl_t *the_ctl;

    the_ctl = mqueue_by_id(mqdes);
    if (!the_ctl) {
        return EINVAL;
    }

    kerr = tos_prio_mail_q_destroy_dyn((k_prio_mail_q_t *)&the_ctl->kprio_mail_q);
    mqueue_id_free(mqdes);
    tos_mmheap_free(the_ctl);

    if (kerr == K_ERR_NONE) {
        return 0;
    }

    return EINVAL;
}

__NOTSUPP__ int mq_getattr(mqd_t mqdes, struct mq_attr *mqstat)
{
    return EOPNOTSUPP;
}

__NOTSUPP__ int mq_notify(mqd_t mqdes, const struct sigevent *notification)
{
    return EOPNOTSUPP;
}

__API__ mqd_t mq_open(const char *name, int oflag, ...)
{
    mqd_t id;
    k_err_t kerr;
    mode_t mode;
    va_list arg;
    mqueue_ctl_t *the_ctl;
    struct mq_attr *attr;

    id = mqueue_id_alloc();
    if (id == -1) {
        return -1;
    }

    va_start(arg, oflag);
    mode = va_arg(arg, mode_t);
    mode = mode; /* make compiler happy */
    attr = va_arg(arg, struct mq_attr *);
    va_end(arg);

    if (attr->mq_maxmsg > MQUEUE_MSG_MAX ||
        attr->mq_msgsize > MQUEUE_MSG_SIZE_MAX) {
        return EINVAL;
    }

    the_ctl = (mqueue_ctl_t *)tos_mmheap_alloc(sizeof(mqueue_ctl_t));
    if (!the_ctl) {
        return -1;
    }

    kerr = tos_prio_mail_q_create_dyn((k_prio_mail_q_t *)&the_ctl->kprio_mail_q,
                                        attr->mq_maxmsg, attr->mq_msgsize);
    if (kerr != K_ERR_NONE) {
        tos_mmheap_free(the_ctl);
        return -1;
    }

    the_ctl->id = id;
    mqueue_id_add(id, the_ctl);

    return id;
}

__API__ ssize_t mq_receive(mqd_t mqdes, char *msg_ptr, size_t msg_len, unsigned *msg_prio)
{
    k_err_t kerr;
    mqueue_ctl_t *the_ctl;

    the_ctl = mqueue_by_id(mqdes);
    if (!the_ctl) {
        return EINVAL;
    }

    kerr = tos_prio_mail_q_pend((k_prio_mail_q_t *)&the_ctl->kprio_mail_q, msg_ptr, &msg_len, TOS_TIME_FOREVER);
    if (kerr == K_ERR_NONE) {
        return 0;
    }

    return msg_len;
}

__API__ int mq_send(mqd_t mqdes, const char *msg_ptr, size_t msg_len, unsigned msg_prio)
{
    k_err_t kerr;
    mqueue_ctl_t *the_ctl;

    the_ctl = mqueue_by_id(mqdes);
    if (!the_ctl) {
        return EINVAL;
    }

    kerr = tos_prio_mail_q_post((k_prio_mail_q_t *)&the_ctl->kprio_mail_q, (void *)msg_ptr, msg_len, msg_prio);
    if (kerr == K_ERR_NONE) {
        return 0;
    }

    return EINVAL;
}

__NOTSUPP__ int mq_setattr(mqd_t mqdes, const struct mq_attr *mqstat, struct mq_attr *omqstat)
{
    return EOPNOTSUPP;
}

__API__ ssize_t mq_timedreceive(mqd_t mqdes, char *msg_ptr, size_t msg_len, unsigned *msg_prio, const struct timespec *abstime)
{
    k_err_t kerr;
    k_tick_t ktick;
    mqueue_ctl_t *the_ctl;

    the_ctl = mqueue_by_id(mqdes);
    if (!the_ctl) {
        return EINVAL;
    }

    ktick   = timespec_to_ktick(abstime);
    kerr    = tos_prio_mail_q_pend((k_prio_mail_q_t *)&the_ctl->kprio_mail_q, (void *)msg_ptr, &msg_len, ktick);
    if (kerr == K_ERR_NONE) {
        return 0;
    }

    return msg_len;
}

__NOTSUPP__ int mq_timedsend(mqd_t mqdes, const char *msg_ptr, size_t msg_len, unsigned msg_prio, const struct timespec *abstime)
{
    return EOPNOTSUPP;
}

__NOTSUPP__ int mq_unlink(const char *name)
{
    return EOPNOTSUPP;
}

#endif /* POSIX_CFG_MQUEUE_EN */

