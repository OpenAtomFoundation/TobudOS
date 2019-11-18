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

#ifndef _TOS_COMPLETION_H_
#define  _TOS_COMPLETION_H_

#if TOS_CFG_COMPLETION_EN > 0u

typedef uint16_t    completion_done_t;

typedef struct k_completion_st {
#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    knl_obj_t           knl_obj;
#endif

    pend_obj_t          pend_obj;
    completion_done_t   done;
} k_completion_t;

/**
 * @brief Create a completion.
 * create a completion.
 *
 * @attention None
 *
 * @param[in]   completion      pointer to the handler of the completion.
 *
 * @return  errcode
 * @retval  #K_ERR_NONE                   return successfully.
 */
__API__ k_err_t tos_completion_create(k_completion_t *completion);

/**
 * @brief Destroy a completion.
 * destroy a completion.
 *
 * @attention None
 *
 * @param[in]   completion      pointer to the handler of the completion.
 *
 * @return  errcode
 * @retval  #K_ERR_OBJ_INVALID          completion is not a valid pointer to completion
 * @retval  #K_ERR_NONE                 return successfully.
 */
__API__ k_err_t tos_completion_destroy(k_completion_t *completion);

/**
 * @brief Pend a completion.
 * pend a completion.
 *
 * @attention None
 *
 * @param[in]   completion  pointer to the handler of the completion.
 * @param[in]   timeout     how much time(in k_tick_t) we would like to wait.
 *
 * @return  errcode
 * @retval  #K_ERR_PEND_NOWAIT                we get nothing, and we don't wanna wait.
 * @retval  #K_ERR_PEND_SCHED_LOCKED          we can wait, but scheduler is locked.
 * @retval  #K_ERR_PEND_TIMEOUT               the time we wait is up, we get nothing.
 * @retval  #K_ERR_PEND_DESTROY               the completion we are pending is destroyed.
 * @retval  #K_ERR_NONE                       return successfully.
 */
__API__ k_err_t tos_completion_pend_timed(k_completion_t *completion, k_tick_t timeout);

/**
 * @brief Pend a completion.
 * pend a completion.
 *
 * @attention None
 *
 * @param[in]   completion  pointer to the handler of the completion.
 * @param[in]   timeout     how much time(in k_tick_t) we would like to wait.
 *
 * @return  errcode
 * @retval  #K_ERR_PEND_SCHED_LOCKED          we can wait, but scheduler is locked.
 * @retval  #K_ERR_PEND_DESTROY               the completion we are pending is destroyed.
 * @retval  #K_ERR_NONE                       return successfully.
 */
__API__ k_err_t tos_completion_pend(k_completion_t *completion);

/**
 * @brief Post a completion.
 * post a completion and wakeup one pending task.
 *
 * @attention when tos_completion_post return successfully, only one task who are waitting for the completion will be woken up.
 *
 * @param[in]   completion  pointer to the handler of the completion.
 *
 * @return  errcode
 * @retval  #K_ERR_COMPLETION_OVERFLOW  we are nesting post a completion too much.
 * @retval  #K_ERR_NONE                 return successfully.
 */
__API__ k_err_t tos_completion_post(k_completion_t *completion);

/**
 * @brief Post a completion.
 * post a completion and wakeup all the pending task.
 *
 * @attention when tos_completion_post_all return successfully, all of the tasks who are waitting for the completion will be woken up.
 *
 * @param[in]   completion  pointer to the handler of the completion.
 *
 * @return  errcode
 * @retval  #K_ERR_COMPLETION_OVERFLOW      we are nesting post a completion too much.
 * @retval  #K_ERR_NONE                     return successfully.
 */
__API__ k_err_t tos_completion_post_all(k_completion_t *completion);

/**
 * @brief Reset a completion.
 * reset a completion to un-done.
 *
 * @attention None.
 *
 * @param[in]   completion  pointer to the handler of the completion.
 *
 * @return  errcode
 * @retval  #K_ERR_NONE                     return successfully.
 */
__API__ k_err_t tos_completion_reset(k_completion_t *completion);

/**
 * @brief Test whether a completion is done.
 * test whether a completion is done.
 *
 * @attention None
 *
 * @param[in]   completion  pointer to the handler of the completion.
 *
 * @return  whether a completion is done
 * @retval  K_TRUE          the completion is done.
 * @retval  K_FALSE         the completion is not done.
 */
__API__ int tos_completion_is_done(k_completion_t *completion);

#endif

#endif

