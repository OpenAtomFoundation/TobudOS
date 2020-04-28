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

#ifndef _TOS_CHAR_FIFO_H_
#define  _TOS_CHAR_FIFO_H_

__CDECLS_BEGIN

typedef struct k_char_fifo_st {
    knl_obj_t   knl_obj;

    k_ring_q_t  ring_q;
} k_chr_fifo_t;

/**
 * @brief Create a character fifo.
 * Create a character fifo.
 *
 * @attention None
 *
 * @param[in]   fifo        pointer to the handler of the fifo.
 * @param[in]   buffer      memory buffer provided to be as the inner buffer.
 * @param[in]   size        size of the memory buffer.
 *
 * @return  errno
 * @retval  #K_ERR_OBJ_PTR_NULL fifo is a NULL pointer.
 * @retval  #K_ERR_NONE         return successfully.
 */
__API__ k_err_t tos_chr_fifo_create(k_chr_fifo_t *chr_fifo, void *buffer, size_t size);

/**
 * @brief Destroy a character fifo.
 * Destroy a character fifo.
 *
 * @attention None
 *
 * @param[in]   fifo        pointer to the handler of the fifo.
 *
 * @return  errno
 * @retval  #K_ERR_OBJ_PTR_NULL fifo is a NULL pointer.
 * @retval  #K_ERR_OBJ_INVALID  not a valid pointer to a fifo.
 * @retval  #K_ERR_NONE         return successfully.
 */
__API__ k_err_t tos_chr_fifo_destroy(k_chr_fifo_t *chr_fifo);

#if TOS_CFG_MMHEAP_EN > 0u

/**
 * @brief Create a character fifo with a dynamic allocated buffer.
 * Create a character fifo with a dynamic allocated buffer.
 *
 * @attention the buffer is dynamic allocated(tos_mmheap_alloc)
 *
 * @param[in]   fifo        pointer to the handler of the fifo.
 * @param[in]   fifo_size   size of the fifo.
 *
 * @return  errno
 * @retval  #K_ERR_OBJ_PTR_NULL fifo is a NULL pointer.
 * @retval  #K_ERR_NONE         return successfully.
 */
__API__ k_err_t tos_chr_fifo_create_dyn(k_chr_fifo_t *chr_fifo, size_t fifo_size);

/**
 * @brief Destroy a character fifo with a dynamic allocated buffer.
 * Destroy a character fifo with a dynamic allocated buffer.
 *
 * @attention None
 *
 * @param[in]   fifo        pointer to the handler of the fifo.
 *
 * @return  errno
 * @retval  #K_ERR_OBJ_PTR_NULL fifo is a NULL pointer.
 * @retval  #K_ERR_OBJ_INVALID  not a valid pointer to a fifo.
 * @retval  #K_ERR_NONE         return successfully.
 */
__API__ k_err_t tos_chr_fifo_destroy_dyn(k_chr_fifo_t *chr_fifo);

#endif

/**
 * @brief Push data into character fifo.
 * Push one single data into the character fifo.
 *
 * @attention None
 *
 * @param[in]   fifo        pointer to the handler of the character fifo.
 * @param[in]   data        data to push into the fifo.
 *
 * @return  errno
 * @retval  #K_ERR_RING_Q_FULL  the character fifo is full.
 * @retval  #K_ERR_NONE         return successfully.
 */
__API__ k_err_t tos_chr_fifo_push(k_chr_fifo_t *chr_fifo, uint8_t data);

/**
 * @brief Push stream into character fifo.
 * Push a stream data into the character fifo.
 *
 * @attention None
 *
 * @param[in]   fifo        pointer to the handler of the character fifo.
 * @param[IN]   stream      stream to be pushed into the character fifo.
 * @param[OUT]  size        size of the stream.
 *
 * @return  the actual number of the data pushed into the character fifo.
 */
__API__ int     tos_chr_fifo_push_stream(k_chr_fifo_t *chr_fifo, uint8_t *stream, size_t size);

/**
 * @brief Pop data from character fifo.
 * Pop one single data from the character fifo.
 *
 * @attention None
 *
 * @param[in]   fifo        pointer to the handler of the character fifo.
 * @param[OUT]  out         one signle buffer to hold the data poped from the character fifo.
 *
 * @return  errno
 * @retval  #K_ERR_FIFO_EMPTY   the character fifo is empty.
 * @retval  #K_ERR_NONE         return successfully.
 */
__API__ k_err_t tos_chr_fifo_pop(k_chr_fifo_t *chr_fifo, uint8_t *out);

/**
 * @brief Pop stream from character fifo.
 * Pop a stream data from the character fifo.
 *
 * @attention None
 *
 * @param[in]   fifo        pointer to the handler of the character fifo.
 * @param[OUT]  buffer      pointer to the buffer to receive the stream poped.
 * @param[OUT]  size        size of the buffer.
 *
 * @return  the actual number of the data poped from the character fifo.
 */
__API__ int     tos_chr_fifo_pop_stream(k_chr_fifo_t *chr_fifo, uint8_t *buffer, size_t size);

/**
 * @brief Flush character fifo.
 * Flush/reset the character fifo.
 *
 * @attention None
 *
 * @param[in]   fifo        pointer to the handler of the character fifo.
 *
 * @return  None.
 */
__API__ k_err_t tos_chr_fifo_flush(k_chr_fifo_t *chr_fifo);

/**
 * @brief Whether the character fifo is empty.
 * Whether the character fifo is empty.
 *
 * @attention None
 *
 * @param[in]   fifo        pointer to the handler of the character fifo.
 *
 * @return  whether the character fifo is emtpy.
 * @retval  #0      the character fifo is not empty.
 * @retval  #Not 0  the character fifo is empty.
 */
__API__ int     tos_chr_fifo_is_empty(k_chr_fifo_t *chr_fifo);

/**
 * @brief Whether the character fifo is full.
 * Whether the character fifo is full.
 *
 * @attention None
 *
 * @param[in]   fifo        pointer to the handler of the character fifo.
 *
 * @return  whether the character fifo is full.
 * @retval  #0      the character fifo is not full.
 * @retval  #Not 0  the character fifo is full.
 */
__API__ int     tos_chr_fifo_is_full(k_chr_fifo_t *chr_fifo);

__CDECLS_END

#endif /* _TOS_CHAR_FIFO_H_ */

