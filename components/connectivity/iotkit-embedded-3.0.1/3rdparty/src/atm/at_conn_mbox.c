/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "infra_types.h"
#include "at_wrapper.h"

#include "at_conn_mbox.h"

typedef struct
{
    void    *buffer;
    uint32_t length;
    uint32_t head;
    uint32_t tail;
    uint8_t  full;
} at_ringbuf_t;

#ifndef PLATFORM_HAS_DYNMEM
static at_ringbuf_t ringbufs[AT_CONN_NUM] = {{NULL, 0, 0, 0, 0}};
#endif

static at_ringbuf_t *alloc_ringbuf(void)
{
#ifdef PLATFORM_HAS_DYNMEM
    return HAL_Malloc(sizeof(at_ringbuf_t));
#else
    int i;

    for (i = 0; i < AT_CONN_NUM; i++) {
        if (NULL == ringbufs[i].buffer) {
            return &ringbufs[i];
        }
    }

    return NULL;
#endif
}

static void free_ringbuf(at_ringbuf_t *ringbuf)
{
    if (ringbuf) {
#ifdef PLATFORM_HAS_DYNMEM
        HAL_Free(ringbuf);
#else
        memset(ringbuf, 0, sizeof(at_ringbuf_t));
#endif
   }
}

static int at_ringbuf_available_read_space(at_ringbuf_t *ringbuf)
{
    if (ringbuf->full)
        return ringbuf->length;

    if (ringbuf->head == ringbuf->tail) {
        return 0;
    } else if (ringbuf->head < ringbuf->tail) {
        return ringbuf->tail - ringbuf->head;
    } else {
        return ringbuf->length - (ringbuf->head - ringbuf->tail);
    }
}

static int at_ringbuf_full(at_ringbuf_t *ringbuf)
{
    return ringbuf->full;
}

static int at_ringbuf_empty(at_ringbuf_t *ringbuf)
{
    return (at_ringbuf_available_read_space(ringbuf) == 0);
}

static at_ringbuf_t *at_ringbuf_create(int length, void *buf)
{
    at_ringbuf_t *ringbuf = NULL;

    if (length < 2 || NULL == buf) {
        HAL_Printf("Error: ringbuf len MUST exceed one!");
        return NULL;
    }

    ringbuf = alloc_ringbuf();
    if (ringbuf == NULL) {
        return NULL;
    }
    memset(ringbuf, 0, sizeof(at_ringbuf_t));

    ringbuf->length = length;
    ringbuf->buffer = buf;

    return ringbuf;
}

static void at_ringbuf_clear_all(at_ringbuf_t *ringbuf)
{
    ringbuf->head = ringbuf->tail = 0;
}

static void at_ringbuf_destroy(at_ringbuf_t *ringbuf)
{
    if (ringbuf) {
        if (ringbuf->buffer) {
            at_ringbuf_clear_all(ringbuf);

            ringbuf->buffer = NULL;
        }
        free_ringbuf(ringbuf);
    }
}

static int at_ringbuf_write(at_ringbuf_t *ringbuf, void *data, int size)
{
    uint32_t next;

    if (ringbuf == NULL || data == NULL) {
        return -1;
    }

    if (at_ringbuf_full(ringbuf)) {
        HAL_Printf("ringbuf full!");
        return -1;
    }

    memcpy(&(((void **) ringbuf->buffer)[ringbuf->tail]), data, size);
    next = (ringbuf->tail + 1) % (ringbuf->length);
    if (next == ringbuf->head) {
        ringbuf->full = 1;
    } else {
        ringbuf->tail = next;
    }

    return 0;
}

static int at_ringbuf_read(at_ringbuf_t *ringbuf, void *target,
                            unsigned int ms, unsigned int *size)
{
    *size = 0;

    if (ringbuf == NULL || target == NULL) {
        return -1;
    }

    /* TODO: timeout handle */
    if (at_ringbuf_empty(ringbuf)) {
        return -1;
    }

    memcpy(((void **)target), &((void **)ringbuf->buffer)[ringbuf->head], sizeof(void *));
    ((void **)ringbuf->buffer)[ringbuf->head] = NULL;
    *size = sizeof(void *);
    ringbuf->head = (ringbuf->head + 1) % (ringbuf->length);

    if (ringbuf->full) {
        ringbuf->full = 0;
        ringbuf->tail = (ringbuf->tail + 1) % (ringbuf->length);
    }

    return 0;
}

/**********************public interface***********************/
int at_mbox_new(at_mbox_t *mb, int size, void *buf)
{
    void *hdl = NULL;

    if (NULL == mb || NULL == buf) {
        return  -1;
    }

    hdl = at_ringbuf_create(size, buf);
    if (hdl == NULL) {
        return -1;
    }
    mb->hdl = hdl;

    return 0;
}

void at_mbox_free(at_mbox_t *mb)
{
    if ((mb != NULL)) {
        at_ringbuf_destroy((at_ringbuf_t *)mb->hdl);
    }
}

void at_mbox_post(at_mbox_t *mb, void *msg)
{
    at_ringbuf_write((at_ringbuf_t *)mb->hdl, &msg, sizeof(void *));
}

int at_mbox_trypost(at_mbox_t *mb, void *msg)
{
    if (at_ringbuf_write((at_ringbuf_t *)mb->hdl,
                         &msg, sizeof(void *)) != 0) {
        return -1;
    } else {
        return 0;
    }
}

int at_mbox_valid(at_mbox_t *mbox)
{
    if (mbox == NULL) {
        return 0;
    }

    if (mbox->hdl == NULL) {
        return 0;
    }

    return 1;
}

uint32_t at_mbox_fetch(at_mbox_t *mb, void **msg, uint32_t timeout)
{
    uint32_t begin_ms, end_ms, elapsed_ms;
    uint32_t len;
    uint32_t ret;

    if (mb == NULL) {
        return AT_MBOX_TIMEOUT;
    }

    begin_ms = HAL_UptimeMs();

    if (timeout != 0UL) {
        if (at_ringbuf_read((at_ringbuf_t *)mb->hdl, msg, timeout, &len) == 0) {
            end_ms = HAL_UptimeMs();
            elapsed_ms = end_ms - begin_ms;
            ret = elapsed_ms;
        } else {
            ret = AT_MBOX_TIMEOUT;
        }
    } else {
        while (at_ringbuf_read((at_ringbuf_t *)mb->hdl, msg, AT_MBOX_TIMEOUT, &len) != 0);
        end_ms = HAL_UptimeMs();
        elapsed_ms = end_ms - begin_ms;

        if (elapsed_ms == 0UL) {
            elapsed_ms = 1UL;
        }

        ret = elapsed_ms;
    }

    return ret;
}

uint32_t at_mbox_tryfetch(at_mbox_t *mb, void **msg)
{
    uint32_t len;

    if (mb == NULL) {
        return AT_MBOX_EMPTY;
    }

    if (at_ringbuf_read((at_ringbuf_t *)mb->hdl, msg, 0u, &len) != 0) {
        return AT_MBOX_EMPTY;
    } else {
        return 0;
    }
}

int at_mbox_empty(at_mbox_t *mb)
{
    if (mb == NULL) {
        return -1;
    }

    return at_ringbuf_empty((at_ringbuf_t *)mb->hdl);
}

void at_mbox_set_invalid(at_mbox_t *mb) 
{ 
    if (mb != NULL) { 
        mb->hdl = NULL; 
    }
}
