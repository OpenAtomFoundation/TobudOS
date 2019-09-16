#ifndef _TOS_FIFO_H_
#define  _TOS_FIFO_H_

typedef struct k_fifo_st {
#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    knl_obj_t   knl_obj;
#endif

    int         beg;
    int         end;
    size_t      cnt;
    uint8_t    *buf;
    size_t      siz;
} k_fifo_t;

/**
 * @brief Create a fifo.
 * Create a fifo.
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
__API__ k_err_t tos_fifo_create(k_fifo_t *fifo, uint8_t *buffer, size_t size);

/**
 * @brief Destroy a fifo.
 * Destroy a fifo.
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
__API__ k_err_t tos_fifo_destroy(k_fifo_t *fifo);

/**
 * @brief Push data into fifo.
 * Push one single data into the fifo.
 *
 * @attention None
 *
 * @param[in]   fifo        pointer to the handler of the fifo.
 * @param[in]   data        data to push into the fifo.
 *
 * @return  errno
 * @retval  #K_ERR_FIFO_FULL  the fifo is full.
 * @retval  #K_ERR_NONE       return successfully.
 */
__API__ k_err_t tos_fifo_push(k_fifo_t *fifo, uint8_t data);

/**
 * @brief Push stream into fifo.
 * Push a stream data into the fifo.
 *
 * @attention None
 *
 * @param[in]   fifo        pointer to the handler of the fifo.
 * @param[IN]   stream      stream to be pushed into the fifo.
 * @param[OUT]  size        size of the stream.
 *
 * @return  the actual number of the data pushed into the fifo.
 */
__API__ int     tos_fifo_push_stream(k_fifo_t *fifo, uint8_t *stream, size_t size);

/**
 * @brief Pop data from fifo.
 * Pop one single data from the fifo.
 *
 * @attention None
 *
 * @param[in]   fifo        pointer to the handler of the fifo.
 * @param[OUT]  out         one signle buffer to hold the data poped from the fifo.
 *
 * @return  errno
 * @retval  #K_ERR_FIFO_EMPTY   the fifo is empty.
 * @retval  #K_ERR_NONE         return successfully.
 */
__API__ k_err_t tos_fifo_pop(k_fifo_t *fifo, uint8_t *out);

/**
 * @brief Pop stream from fifo.
 * Pop a stream data from the fifo.
 *
 * @attention None
 *
 * @param[in]   fifo        pointer to the handler of the fifo.
 * @param[OUT]  buffer      pointer to the buffer to receive the stream poped.
 * @param[OUT]  size        size of the buffer.
 *
 * @return  the actual number of the data poped from the fifo.
 */
__API__ int     tos_fifo_pop_stream(k_fifo_t *fifo, uint8_t *buffer, size_t size);

/**
 * @brief Flush fifo.
 * Flush/reset the fifo.
 *
 * @attention None
 *
 * @param[in]   fifo        pointer to the handler of the fifo.
 *
 * @return  None.
 */
__API__ void    tos_fifo_flush(k_fifo_t *fifo);

/**
 * @brief Whether the fifo is empty.
 * Whether the fifo is empty.
 *
 * @attention None
 *
 * @param[in]   fifo        pointer to the handler of the fifo.
 *
 * @return  whether the fifo is emtpy.
 * @retval  #0      the fifo is not empty.
 * @retval  #Not 0  the fifo is empty.
 */
__API__ int     tos_fifo_is_empty(k_fifo_t *fifo);

/**
 * @brief Whether the fifo is full.
 * Whether the fifo is full.
 *
 * @attention None
 *
 * @param[in]   fifo        pointer to the handler of the fifo.
 *
 * @return  whether the fifo is full.
 * @retval  #0      the fifo is not full.
 * @retval  #Not 0  the fifo is full.
 */
__API__ int     tos_fifo_is_full(k_fifo_t *fifo);

#endif // _TOS_FIFO_H_

