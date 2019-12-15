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

#ifndef _TOS_EVTDRV_MSG_H_
#define  _TOS_EVTDRV_MSG_H_

#if TOS_CFG_EVENT_DRIVEN_EN > 0u

typedef uint16_t    evtdrv_msg_len_t;
typedef uint8_t    *evtdrv_msg_body_t;

typedef struct evtdrv_message_header_st {
    k_list_t            list;
    evtdrv_task_id_t    dst_task_id;    // destination task id
    evtdrv_msg_len_t    len;
} evtdrv_msg_hdr_t;

#define EVTDRV_MSG_BODY2HDR(msg_body)       ((evtdrv_msg_hdr_t *)((uint8_t *)msg_body - sizeof(evtdrv_msg_hdr_t)))
#define EVTDRV_MSG_LEN(msg_len)             (sizeof(evtdrv_msg_hdr_t) + msg_len)
#define EVTDRV_MSG_HDR2BODY(msg_hdr)        ((evtdrv_msg_body_t)((evtdrv_msg_hdr_t *)msg_hdr + 1))

/**
 * @brief Allocate a message body.
 * Allocate a message body as a payload to hold the message content.
 *
 * @attention None
 *
 * @param[in]   msg_len     the length of the message payload.
 *
 * @return  the message body allocated.
 * @retval  #NULL       allocate failed.
 * @retval  #NO-NULL    allocate successfully.
 */
__API__ evtdrv_msg_body_t tos_evtdrv_msg_alloc(evtdrv_msg_len_t msg_len);

/**
 * @brief Free a message body.
 *
 * @attention if you wanna free a message body, the message must be "dequeued"(already received by the target task)
 *
 * @param[in]   msg_body    message body to free.
 *
 * @return  errcode
 * @retval  #EVTDRV_ERR_PTR_NULL  message body is NULL.
 * @retval  #EVTDRV_ERR_MSG_BUSY  message is still in the message queue(not received).
 * @retval  #EVTDRV_ERR_NONE      free successfully.
 */
__API__ evtdrv_err_t tos_evtdrv_msg_free(evtdrv_msg_body_t msg_body);

/**
 * @brief Send a message to the target task.
 *
 * @attention next time the target task should be "wakeup" by the event of TOS_EVTDRV_SYS_EVENT_MSG.
 *
 * @param[in]   task_id     id of the target task.
 * @param[in]   msg_body    message body to send.
 *
 * @return  errcode
 * @retval  #EVTDRV_ERR_PTR_NULL      message body is NULL.
 * @retval  #EVTDRV_ERR_TASK_INVALID  target task id is invalid.
 * @retval  #EVTDRV_ERR_NONE          send successfully.
 */
__API__ evtdrv_err_t tos_evtdrv_msg_send(evtdrv_task_id_t dst_task_id, evtdrv_msg_body_t msg_body);

__API__ evtdrv_msg_body_t tos_evtdrv_msg_recv(void);

__KERNEL__ void evtdrv_msg_init(void);

#endif /* TOS_CFG_EVENT_DRIVEN_EN */

#endif /* _TOS_EVTDRV_MSG_H_ */

