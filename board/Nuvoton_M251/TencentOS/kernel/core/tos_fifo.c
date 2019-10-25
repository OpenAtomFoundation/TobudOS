#include "tos.h"

__STATIC_INLINE__ int fifo_next(k_fifo_t *fifo, int index)
{
    return (index + 1) % fifo->siz;
}

__API__ k_err_t tos_fifo_create(k_fifo_t *fifo, uint8_t *buffer, size_t size)
{
    TOS_PTR_SANITY_CHECK(fifo);

#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    knl_object_init(&fifo->knl_obj, KNL_OBJ_TYPE_FIFO);
#endif

    fifo->beg = 0;
    fifo->end = 0;
    fifo->cnt = 0;
    fifo->buf = buffer;
    fifo->siz = size;

    return K_ERR_NONE;
}

__API__ k_err_t tos_fifo_destroy(k_fifo_t *fifo)
{
    TOS_PTR_SANITY_CHECK(fifo);

#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    if (!knl_object_verify(&fifo->knl_obj, KNL_OBJ_TYPE_FIFO)) {
        return K_ERR_OBJ_INVALID;
    }
#endif

    fifo->beg = 0;
    fifo->end = 0;
    fifo->cnt = 0;
    fifo->buf = K_NULL;
    fifo->siz = 0;

#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    knl_object_deinit(&fifo->knl_obj);
#endif

    return K_ERR_NONE;
}

__API__ k_err_t tos_fifo_push(k_fifo_t *fifo, uint8_t data)
{
    TOS_CPU_CPSR_ALLOC();

#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    if (!knl_object_verify(&fifo->knl_obj, KNL_OBJ_TYPE_FIFO)) {
        return K_ERR_OBJ_INVALID;
    }
#endif

    TOS_CPU_INT_DISABLE();

    if (tos_fifo_is_full(fifo)) {
        TOS_CPU_INT_ENABLE();
        return K_ERR_FIFO_FULL;
    }

    fifo->buf[fifo->end] = data;
    fifo->end = fifo_next(fifo, fifo->end);
    ++fifo->cnt;

    TOS_CPU_INT_ENABLE();
    return K_ERR_NONE;
}

__API__ int tos_fifo_push_stream(k_fifo_t *fifo, uint8_t *stream, size_t size)
{
    TOS_CPU_CPSR_ALLOC();
    int i = 0;

#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    if (!knl_object_verify(&fifo->knl_obj, KNL_OBJ_TYPE_FIFO)) {
        return K_ERR_OBJ_INVALID;
    }
#endif

    TOS_CPU_INT_DISABLE();
    while (!tos_fifo_is_full(fifo) && i < size) {
        if (tos_fifo_push(fifo, stream[i]) != K_ERR_NONE) {
            TOS_CPU_INT_ENABLE();
            return i;
        }
        ++i;
    }
    TOS_CPU_INT_ENABLE();
    return i;
}

__API__ k_err_t tos_fifo_pop(k_fifo_t *fifo, uint8_t *out)
{
    TOS_CPU_CPSR_ALLOC();
    uint8_t data;

#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    if (!knl_object_verify(&fifo->knl_obj, KNL_OBJ_TYPE_FIFO)) {
        return K_ERR_OBJ_INVALID;
    }
#endif

    TOS_CPU_INT_DISABLE();

    if (tos_fifo_is_empty(fifo)) {
        TOS_CPU_INT_ENABLE();
        return K_ERR_FIFO_EMPTY;
    }

    data = fifo->buf[fifo->beg];
    fifo->beg = fifo_next(fifo, fifo->beg);
    --fifo->cnt;

    TOS_CPU_INT_ENABLE();

    *out = data;
    return K_ERR_NONE;
}

__API__ int tos_fifo_pop_stream(k_fifo_t *fifo, uint8_t *buffer, size_t size)
{
    TOS_CPU_CPSR_ALLOC();
    int i = 0;
    uint8_t data;

#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    if (!knl_object_verify(&fifo->knl_obj, KNL_OBJ_TYPE_FIFO)) {
        return K_ERR_OBJ_INVALID;
    }
#endif

    TOS_CPU_INT_DISABLE();
    while (!tos_fifo_is_empty(fifo) && i < size) {
        if (tos_fifo_pop(fifo, &data) != K_ERR_NONE) {
            TOS_CPU_INT_ENABLE();
            return i;
        }
        buffer[i++] = data;
    }
    TOS_CPU_INT_ENABLE();
    return i;
}

__API__ void tos_fifo_flush(k_fifo_t *fifo)
{
#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    if (!knl_object_verify(&fifo->knl_obj, KNL_OBJ_TYPE_FIFO)) {
        return;
    }
#endif

    fifo->beg = 0;
    fifo->end = 0;
    fifo->cnt = 0;
}

__API__ int tos_fifo_is_empty(k_fifo_t *fifo)
{
    TOS_CPU_CPSR_ALLOC();
    int is_empty = 0;

#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    if (!knl_object_verify(&fifo->knl_obj, KNL_OBJ_TYPE_FIFO)) {
        return K_ERR_OBJ_INVALID;
    }
#endif

    TOS_CPU_INT_DISABLE();
    is_empty = (fifo->cnt == 0);
    TOS_CPU_INT_ENABLE();

    return is_empty;
}

__API__ int tos_fifo_is_full(k_fifo_t *fifo)
{
    TOS_CPU_CPSR_ALLOC();
    int is_full = 0;

#if TOS_CFG_OBJECT_VERIFY_EN > 0u
    if (!knl_object_verify(&fifo->knl_obj, KNL_OBJ_TYPE_FIFO)) {
        return K_ERR_OBJ_INVALID;
    }
#endif

    TOS_CPU_INT_DISABLE();
    is_full = (fifo->cnt == fifo->siz);
    TOS_CPU_INT_ENABLE();

    return is_full;
}

