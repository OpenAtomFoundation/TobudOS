/**
 * @file    wm_wl_mbox.h
 *
 * @brief   mailbox (mbox) APIs
 *
 * @author  dave
 *
 * Copyright (c) 2015 Winner Microelectronics Co., Ltd.
 */
#ifndef __TLS_WL_MBOX_H__
#define __TLS_WL_MBOX_H__

#include "wm_type_def.h"
#include "wm_osal.h"

/** max value of time out */
#define SYS_ARCH_TIMEOUT 0xffffffffUL

/** pointer to the mailbox */
typedef tls_os_queue_t * tls_mbox_t;

/**
 * @defgroup System_APIs System APIs
 * @brief System APIs
 */

/**
 * @addtogroup System_APIs
 * @{
 */

/**
 * @defgroup MBOX_APIs MBOX APIs
 * @brief Software mailbox APIs
 */

/**
 * @addtogroup MBOX_APIs
 * @{
 */

/**
 * @brief          Create a malibox
 *
 * @param[out]     *mbox              pointer to the mailbox
 * @param[in]      size               size of mailbox
 *
 * @retval         TLS_OS_SUCCESS     success
 * @retval         TLS_OS_ERROR       failed
 *
 * @note           None
 */
s8 tls_mbox_new(tls_mbox_t *mbox, int size);

/**
 * @brief          Check if an mbox is valid/allocated
 *
 * @param[in]      mbox    pointer to the mailbox
 *
 * @retval         0       invalid
 * @retval         1       valid
 *
 * @note           None
 */
int tls_mbox_valid(tls_mbox_t mbox);

/**
 * @brief          Sends a message to a mailbox
 *
 * @param[in]      mbox    pointer to the mailbox
 * @param[in]      *msg    pointer to the message to be post
 *
 * @return         None
 *
 * @note           None
 */
void tls_mbox_post(tls_mbox_t mbox, void *msg);

/**
 * @brief          Posts the msg to the mailbox.
 *
 * @param[in]      mbox               pointer to the mailbox
 * @param[in]      *msg               pointer to the message to be post
 *
 * @retval         TLS_OS_SUCCESS     success
 * @retval         TLS_OS_ERROR       failed
 *
 * @note           this function have to block until the "msg" is really posted.
 */
s8 tls_mbox_trypost(tls_mbox_t mbox, void *msg);

/**
 * @brief          Waits for a message within specified time
 *
 * @param[in]      mbox         pointer to the mailbox
 * @param[out]     **msg        pointer to the message to be received
 * @param[in]      timeout      the specified time
 *
 * @retval         SYS_ARCH_TIMEOUT     time out
 * @retval         other                time of elapsed
 *
 * @note           None
 */
u32 tls_arch_mbox_fetch(tls_mbox_t mbox, void **msg, u32 timeout);

/**
 * @}
 */

/**
 * @}
 */

#endif

