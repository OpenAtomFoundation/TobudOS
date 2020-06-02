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

#include "assert.h"
#include "string.h"
#include "topo_sorting.h"

int topo_sorting_create(topo_sorting_t *topo_sorting, graph_t *graph)
{
    int i = 0;

    if (!topo_sorting || !graph) {
        return -1;
    }

    memset(topo_sorting, 0, sizeof(topo_sorting_t));

    if (stack_create(&topo_sorting->stack, graph_vertex_max(graph)) != 0) {
        return 0;
    }

    for (i = 0; i < graph_vertex_max(graph); ++i) {
        if (graph_indegree_get(graph, i) == 0 &&
            graph_tag_get(graph, i) != TOPO_VISITED) {
            stack_push(&topo_sorting->stack, i);
            graph_tag_set(graph, i, TOPO_VISITED);
        }
    }

    topo_sorting->graph = graph;

    return 0;
}

int topo_sorting_destroy(topo_sorting_t *topo_sorting)
{
    int i = 0;

    if (!topo_sorting || !topo_sorting->graph) {
        return -1;
    }

    for (i = 0; i < graph_vertex_max(topo_sorting->graph); ++i) {
        graph_tag_reset(topo_sorting->graph, i);
    }

    stack_destroy(&topo_sorting->stack);

    topo_sorting->graph = NULL;

    return 0;
}

int topo_sorting_has_next(topo_sorting_t *topo_sorting)
{
    if (!topo_sorting || !topo_sorting->graph) {
        return 0;
    }

    return !stack_is_empty(&topo_sorting->stack);
}

static void push2stack(int tail_vertex, int head_vertex, void *arg)
{
    topo_sorting_t *topo_sorting = (topo_sorting_t *)arg;

    if (graph_indegree_get(topo_sorting->graph, head_vertex) == 0) {
        stack_push(&topo_sorting->stack, head_vertex);
        graph_tag_set(topo_sorting->graph, head_vertex, TOPO_VISITED);
    }
}

int topo_sorting_next(topo_sorting_t *topo_sorting)
{
    int vertex;

    assert(sizeof(e_type_t) == sizeof(int));

    if (!topo_sorting || !topo_sorting->graph) {
        return -1;
    }

    if (stack_is_empty(&topo_sorting->stack)) {
        return -1;
    }

    vertex = stack_pop(&topo_sorting->stack);

    graph_edge_rmv_by_tail(topo_sorting->graph, vertex, push2stack, (void *)topo_sorting);

    return vertex;
}

int topo_has_ring(topo_sorting_t *topo_sorting)
{
    if (!topo_sorting || !topo_sorting->graph) {
        return -1;
    }

    return graph_edgesn_get(topo_sorting->graph) != 0;
}

int topo_ring_break(topo_sorting_t *topo_sorting)
{
    int i = 0;
    graph_t *graph;

    if (!topo_sorting || !topo_sorting->graph) {
        return -1;
    }

    graph = topo_sorting->graph;

    for (i = 0; i < graph_vertex_max(graph); ++i) {
        if (graph_indegree_get(graph, i) != 0 &&
            graph_outdegree_get(graph, i) != 0 &&
            graph_tag_get(graph, i) != TOPO_VISITED) {
            /* remove one edge from the graph */
            graph_edge_rmv_one_by_head(graph, i);
            break;
        }
    }

    for (i = 0; i < graph_vertex_max(graph); ++i) {
        if (graph_indegree_get(graph, i) == 0 &&
            graph_tag_get(graph, i) != TOPO_VISITED) {
            stack_push(&topo_sorting->stack, i);
            graph_tag_set(graph, i, TOPO_VISITED);
        }
    }

    return 0;
}

