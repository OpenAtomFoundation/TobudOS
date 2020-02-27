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

#ifndef _TOS_EVENT_H_
#define _TOS_EVENT_H_

__CDECLS_BEGIN

#if TOS_CFG_EVENT_EN > 0

// if we are pending an event, for any flag we expect is set is ok, this flag should be passed to tos_event_pend
#define TOS_OPT_EVENT_PEND_ANY          (k_opt_t)0x0001

// if we are pending an event, must all the flag we expect is set is ok, this flag should be passed to tos_event_pend
#define TOS_OPT_EVENT_PEND_ALL          (k_opt_t)0x0002

// if we are pending an event, and we wanna clear the event's flag after we read, this flag should be passed to tos_event_pend
/* ATTENTION:
    we can pass both TOS_OPT_EVENT_PEND_CLR and TOS_OPT_EVENT_PEND_ANY, or TOS_OPT_EVENT_PEND_CLR and TOS_OPT_EVENT_PEND_ALL
    to tos_event_pend, if we wanna do this, a (TOS_OPT_EVENT_PEND_CLR | TOS_OPT_EVENT_PEND_ANY) or
    (TOS_OPT_EVENT_PEND_CLR | TOS_OPT_EVENT_PEND_ALL) should be passed.
    but, (TOS_OPT_EVENT_PEND_ANY | TOS_OPT_EVENT_PEND_ALL) is invalid, we cannot both wanna any and all flag is set.
 */
#define TOS_OPT_EVENT_PEND_CLR          (k_opt_t)0x0004

typedef enum opt_event_post_en {
    OPT_EVENT_POST_KEP,
    OPT_EVENT_POST_CLR,
} opt_event_post_t;

typedef struct k_event_st {
#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    knl_obj_t       knl_obj;
#endif

    pend_obj_t      pend_obj;
    k_event_flag_t  flag;
} k_event_t;

/**
 * @brief Create an event.
 * create an event.
 *
 * @attention None
 *
 * @param[in]   event       pointer to the handler of the event.
 * @param[in]   init_flag   initial flag of the event.
 *
 * @return  errcode
 * @retval  #K_ERR_NONE                   return successfully.
 */
__API__ k_err_t tos_event_create(k_event_t *event, k_event_flag_t init_flag);

/**
 * @brief Destroy an event.
 * destroy an event.
 *
 * @attention None
 *
 * @param[in]   event       pointer to the handler of the event.
 *
 * @return  errcode
 * @retval  #K_ERR_NONE                   return successfully.
 */
__API__ k_err_t tos_event_destroy(k_event_t *event);

/**
 * @brief Pend an event.
 * pend an event.
 *
 * @attention if opt is TOS_OPT_EVENT_PEND_ANY, any of the flag_expect is set is ok;
 *            if opt is TOS_OPT_EVENT_PEND_ALL£¬ must all the flag_expect is set is ok.
 *
 * @param[in]   event       pointer to the handler of the event.
 * @param[in]   flag_expect the flag we expect from the event.
 * @param[OUT]  flag_match  if we get the flag we expect, what exactly they are?
 * @param[in]   timeout     how much time(in k_tick_t) we would like to wait.
 * @param[in]   opt         option for pend.
 *
 * @return  errcode
 * @retval  #K_ERR_EVENT_PEND_OPT_INVALID     opt is invalid
 * @retval  #K_ERR_PEND_NOWAIT                we get nothing, and we don't wanna wait.
 * @retval  #K_ERR_PEND_SCHED_LOCKED          we can wait, but scheduler is locked.
 * @retval  #K_ERR_PEND_TIMEOUT               the time we wait is up, we get nothing.
 * @retval  #K_ERR_PEND_DESTROY               the event we are pending is destroyed.
 * @retval  #K_ERR_NONE                       return successfully.
 */
__API__ k_err_t tos_event_pend(k_event_t *event, k_event_flag_t flag_expect, k_event_flag_t *flag_match, k_tick_t timeout, k_opt_t opt);

/**
 * @brief Post an event.
 * post an event.
 *
 * @attention if you are posting an event in tos_event_post, event's own flag will be overwrited by the flag we post.
 *            eg. if an event's own flag is 0x0001, and we are posting a flag 0x0030, after the post, the event's flag
 *            will be overwrited to 0x0030.
 *
 * @param[in]   event       pointer to the handler of the event.
 * @param[in]   flag        the flag we post.
 *
 * @return  errcode
 * @retval  #K_ERR_NONE                       return successfully.
 */
__API__ k_err_t tos_event_post(k_event_t *event, k_event_flag_t flag);

/**
 * @brief Post an event.
 * post an event, and keep the original own flag of the event.
 *
 * @attention the original own flag of the event will be keeped.
 *            eg.if an event's own flag is 0x0001, and we are posting a flag 0x0030, after the post, the event's flag
 *            will be changed to 0x0031(0x0030 | 0x0001), which means the event's original flag is keeped.
 *
 * @param[in]   event       pointer to the handler of the event.
 * @param[in]   flag        the flag we post.
 *
 * @return  errcode
 * @retval  #K_ERR_NONE                       return successfully.
 */
__API__ k_err_t tos_event_post_keep(k_event_t *event, k_event_flag_t flag);

#endif

__CDECLS_END

#endif /* _TOS_EVENT_H_ */

