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

#ifndef _POSIX_SIGNAL_H_
#define  _POSIX_SIGNAL_H_

#include "sys/types.h"

union sigval {
    int     sival_int;  /* integer signal value */
    void   *sival_ptr;  /* pointer signal value */
};

struct sigevent {
    int             sigev_notify;   /* notification type */
    int             sigev_signo;    /* signal number */
    union sigval    sigev_value;    /* signal value */
    void            (*sigev_notify_function)(union sigval);      /* notification function */
    pthread_attr_t  *sigev_notify_attributes;   /* notification attributes */
};

#endif /* _POSIX_SIGNAL_H_ */

