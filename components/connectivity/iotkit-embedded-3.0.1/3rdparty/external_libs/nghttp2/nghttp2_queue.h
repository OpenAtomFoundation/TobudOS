/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */


#ifndef NGHTTP2_QUEUE_H
#define NGHTTP2_QUEUE_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "nghttp2.h"

typedef struct nghttp2_queue_cell {
  void *data;
  struct nghttp2_queue_cell *next;
} nghttp2_queue_cell;

typedef struct {
  nghttp2_queue_cell *front, *back;
} nghttp2_queue;

void nghttp2_queue_init(nghttp2_queue *queue);
void nghttp2_queue_free(nghttp2_queue *queue);
int nghttp2_queue_push(nghttp2_queue *queue, void *data);
void nghttp2_queue_pop(nghttp2_queue *queue);
void *nghttp2_queue_front(nghttp2_queue *queue);
void *nghttp2_queue_back(nghttp2_queue *queue);
int nghttp2_queue_empty(nghttp2_queue *queue);

#endif /* NGHTTP2_QUEUE_H */
