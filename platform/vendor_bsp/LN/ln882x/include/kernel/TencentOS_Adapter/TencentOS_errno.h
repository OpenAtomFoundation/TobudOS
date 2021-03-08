/**
 * @file   TencentOS_errno.h
 * @author LightningSemi WLAN Team
 * Copyright (C) 2018 LightningSemi Technology Co., Ltd. All rights reserved.
 */

#ifndef _KERNEL_OS_TENCENT_OS_ERRNO_H_
#define _KERNEL_OS_TENCENT_OS_ERRNO_H_

#include "./TencentOS/adapter_common.h"

#ifdef __cplusplus
extern "C" {
#endif



/**
 * @brief Get error number of the current thread
 * @return Error number of the current thread
 */
static __always_inline int OS_GetErrno(void)
{

}

/**
 * @brief Set error number of the current thread
 * @param[in] Error number to be set
 * @return None
 */
static __always_inline void OS_SetErrno(int err)
{

}


#ifdef __cplusplus
}
#endif

#endif /* _KERNEL_OS_TENCENT_OS_ERRNO_H_ */
