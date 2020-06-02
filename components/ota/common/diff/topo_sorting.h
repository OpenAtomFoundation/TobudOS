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

#ifndef _TOPO_SORTING_H_
#define  _TOPO_SORTING_H_

#include "stack.h"
#include "graph.h"

#define TOPO_VISITED         1

typedef struct topo_sorting_st {
    stack_t     stack;
    graph_t    *graph;
} topo_sorting_t;

int topo_sorting_create(topo_sorting_t *topo_sorting, graph_t *graph);

int topo_sorting_destroy(topo_sorting_t *topo_sorting);

int topo_sorting_has_next(topo_sorting_t *topo_sorting);

int topo_sorting_next(topo_sorting_t *topo_sorting);

int topo_has_ring(topo_sorting_t *topo_sorting);

int topo_ring_break(topo_sorting_t *topo_sorting);

#endif

