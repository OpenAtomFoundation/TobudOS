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
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "graph.h"

int graph_create(graph_t *graph, int vertexs_n)
{
    int i = 0;
    vertex_t *vertexs;

    if (!graph || vertexs_n <= 0) {
        return -1;
    }

    memset(graph, 0, sizeof(graph_t));

    if ((vertexs = malloc(vertexs_n * sizeof(vertex_t))) == NULL) {
        return -1;
    }

    memset(vertexs, 0, vertexs_n * sizeof(vertex_t));

    for (i = 0; i < vertexs_n; ++i) {
        list_init(&vertexs[i].first_in);
        list_init(&vertexs[i].first_out);
    }

    graph->edges_n      = 0;
    graph->vertexs_n    = vertexs_n;
    graph->vertexs      = vertexs;

    return 0;
}

int graph_destroy(graph_t *graph)
{
    int i = 0;
    vertex_t *vertex;
    edge_t *edge, *tmp;

    if (!graph || !graph->vertexs || !graph->vertexs_n) {
        return -1;
    }

    for (i = 0; i < graph->vertexs_n; ++i) {
        vertex = &graph->vertexs[i];
        LIST_FOR_EACH_ENTRY_SAFE(edge, tmp, edge_t, tail_list, &vertex->first_out) {
            list_del(&edge->tail_list);
            free(edge);
        }
    }

    free(graph->vertexs);

    graph->edges_n      = 0;
    graph->vertexs_n    = 0;
    graph->vertexs      = NULL;

    return 0;
}

int graph_edge_add(graph_t *graph, int tail_vertex, int head_vertex)
{
    edge_t *edge;
    vertex_t *tail_v, *head_v;

    if (!graph || !graph->vertexs || !graph->vertexs_n) {
        return -1;
    }

    if (tail_vertex >= graph->vertexs_n ||
        head_vertex >= graph->vertexs_n ||
        tail_vertex < 0 ||
        head_vertex < 0) {
        return -1;
    }

    if ((edge = malloc(sizeof(edge_t))) == NULL) {
        return -1;
    }

    edge->head_vertex = head_vertex;
    edge->tail_vertex = tail_vertex;
    list_init(&edge->head_list);
    list_init(&edge->tail_list);

    tail_v = &graph->vertexs[tail_vertex];
    head_v = &graph->vertexs[head_vertex];

    list_add(&edge->tail_list, &tail_v->first_out);
    ++tail_v->out_degree;

    list_add(&edge->head_list, &head_v->first_in);
    ++head_v->in_degree;

    ++graph->edges_n;

    return 0;
}

int graph_edge_rmv(graph_t *graph, int tail_vertex, int head_vertex)
{
    edge_t *edge, *tmp;
    vertex_t *tail_v, *head_v;

    if (!graph || !graph->vertexs || !graph->vertexs_n) {
        return -1;
    }

    if (tail_vertex >= graph->vertexs_n ||
        head_vertex >= graph->vertexs_n ||
        tail_vertex < 0 ||
        head_vertex < 0) {
        return -1;
    }

    tail_v = &graph->vertexs[tail_vertex];
    head_v = &graph->vertexs[head_vertex];

    LIST_FOR_EACH_ENTRY_SAFE(edge, tmp, edge_t, tail_list, &tail_v->first_out) {
        if (edge->tail_vertex == tail_vertex &&
            edge->head_vertex == head_vertex) {
            list_del(&edge->tail_list);
            --tail_v->out_degree;

            list_del(&edge->head_list);
            --head_v->in_degree;

            free(edge);
            --graph->edges_n;

            return 0;
        }
    }

    return -1;
}

int graph_edge_rmv_by_tail(graph_t *graph, int tail_vertex, graph_edge_delete_cb del_cb, void *cb_arg)
{
    edge_t *edge, *tmp;
    vertex_t *tail_v, *head_v;

    if (!graph || !graph->vertexs || !graph->vertexs_n) {
        return -1;
    }

    if (tail_vertex >= graph->vertexs_n || tail_vertex < 0) {
        return -1;
    }

    tail_v = &graph->vertexs[tail_vertex];

    LIST_FOR_EACH_ENTRY_SAFE(edge, tmp, edge_t, tail_list, &tail_v->first_out) {
        if (edge->tail_vertex == tail_vertex) {
            list_del(&edge->tail_list);
            --tail_v->out_degree;

            list_del(&edge->head_list);
            head_v = &graph->vertexs[edge->head_vertex];
            --head_v->in_degree;

            if (del_cb) {
                del_cb(edge->tail_vertex, edge->head_vertex, cb_arg);
            }

            free(edge);
            --graph->edges_n;
        }
    }

    return 0;
}

int graph_edge_rmv_one_by_tail(graph_t *graph, int tail_vertex)
{
    edge_t *edge, *tmp;
    vertex_t *tail_v, *head_v;

    if (!graph || !graph->vertexs || !graph->vertexs_n) {
        return -1;
    }

    if (tail_vertex >= graph->vertexs_n || tail_vertex < 0) {
        return -1;
    }

    tail_v = &graph->vertexs[tail_vertex];

    LIST_FOR_EACH_ENTRY_SAFE(edge, tmp, edge_t, tail_list, &tail_v->first_out) {
        if (edge->tail_vertex == tail_vertex) {
            list_del(&edge->tail_list);
            --tail_v->out_degree;

            list_del(&edge->head_list);
            head_v = &graph->vertexs[edge->head_vertex];
            --head_v->in_degree;

            free(edge);
            --graph->edges_n;

            return 0;
        }
    }

    return -1;
}

int graph_edge_rmv_by_head(graph_t *graph, int head_vertex, graph_edge_delete_cb del_cb, void *cb_arg)
{
    edge_t *edge, *tmp;
    vertex_t *tail_v, *head_v;

    if (!graph || !graph->vertexs || !graph->vertexs_n) {
        return -1;
    }

    if (head_vertex >= graph->vertexs_n || head_vertex < 0) {
        return -1;
    }

    head_v = &graph->vertexs[head_vertex];

    LIST_FOR_EACH_ENTRY_SAFE(edge, tmp, edge_t, head_list, &head_v->first_in) {
        if (edge->head_vertex == head_vertex) {
            list_del(&edge->tail_list);
            tail_v = &graph->vertexs[edge->tail_vertex];
            --tail_v->out_degree;

            list_del(&edge->head_list);
            --head_v->in_degree;

            if (del_cb) {
                del_cb(edge->tail_vertex, edge->head_vertex, cb_arg);
            }

            free(edge);
            --graph->edges_n;
        }
    }

    return 0;
}

int graph_edge_rmv_one_by_head(graph_t *graph, int head_vertex)
{
    edge_t *edge, *tmp;
    vertex_t *tail_v, *head_v;

    if (!graph || !graph->vertexs || !graph->vertexs_n) {
        return -1;
    }

    if (head_vertex >= graph->vertexs_n || head_vertex < 0) {
        return -1;
    }

    head_v = &graph->vertexs[head_vertex];

    LIST_FOR_EACH_ENTRY_SAFE(edge, tmp, edge_t, head_list, &head_v->first_in) {
        if (edge->head_vertex == head_vertex) {
            list_del(&edge->tail_list);
            tail_v = &graph->vertexs[edge->tail_vertex];
            --tail_v->out_degree;

            list_del(&edge->head_list);
            --head_v->in_degree;

            free(edge);
            --graph->edges_n;

            return 0;
        }
    }

    return -1;
}

int graph_edgesn_get(graph_t *graph)
{
    if (!graph || !graph->vertexs || !graph->vertexs_n) {
        return -1;
    }

    return graph->edges_n;
}

int graph_vertex_max(graph_t *graph)
{
    if (!graph || !graph->vertexs || !graph->vertexs_n) {
        return -1;
    }

    return graph->vertexs_n;
}

int graph_indegree_get(graph_t *graph, int vertex)
{
    if (!graph || !graph->vertexs || !graph->vertexs_n) {
        return -1;
    }

    if (vertex >= graph->vertexs_n || vertex < 0) {
        return -1;
    }

    return graph->vertexs[vertex].in_degree;
}

int graph_outdegree_get(graph_t *graph, int vertex)
{
    if (!graph || !graph->vertexs || !graph->vertexs_n) {
        return -1;
    }

    if (vertex >= graph->vertexs_n || vertex < 0) {
        return -1;
    }

    return graph->vertexs[vertex].out_degree;
}

int graph_tag_set(graph_t *graph, int vertex, v_tag_t tag)
{
    if (!graph || !graph->vertexs || !graph->vertexs_n) {
        return -1;
    }

    if (vertex >= graph->vertexs_n || vertex < 0) {
        return -1;
    }

    graph->vertexs[vertex].tag = tag;
    return 0;
}

int graph_tag_reset(graph_t *graph, int vertex)
{
    if (!graph || !graph->vertexs || !graph->vertexs_n) {
        return -1;
    }

    if (vertex >= graph->vertexs_n || vertex < 0) {
        return -1;
    }

    graph->vertexs[vertex].tag = 0;
    return 0;
}

v_tag_t graph_tag_get(graph_t *graph, int vertex)
{
    if (!graph || !graph->vertexs || !graph->vertexs_n) {
        return (v_tag_t)-1;
    }

    if (vertex >= graph->vertexs_n || vertex < 0) {
        return (v_tag_t)-1;
    }

    return graph->vertexs[vertex].tag;
}

int graph_dfs_create(dfs_t *dfs, graph_t *graph)
{
    if (!dfs || !graph) {
        return -1;
    }

    if (graph_vertex_max(graph) == 0) {
        return -1;
    }

    memset(dfs, 0, sizeof(dfs_t));

    if (stack_create(&dfs->stack, graph_vertex_max(graph)) != 0) {
        return 0;
    }

    if (stack_push(&dfs->stack, 0) != 0) {
        // push the first vertex of the graph into the stack
        return -1;
    }

    graph_vertex_set_visited(graph, 0);

    dfs->graph = graph;

    return 0;
}

int graph_dfs_destroy(dfs_t *dfs)
{
    int i = 0;

    if (!dfs || !dfs->graph) {
        return -1;
    }

    for (i = 0; i < graph_vertex_max(dfs->graph); ++i) {
        graph_tag_reset(dfs->graph, i);
    }

    stack_destroy(&dfs->stack);

    dfs->graph = NULL;

    return 0;
}

int graph_dfs_has_next(dfs_t *dfs)
{
    if (!dfs || !dfs->graph) {
        return 0;
    }

    return !stack_is_empty(&dfs->stack);
}

int graph_dfs_ring_detect(dfs_t *dfs, int *ring_size)
{
    int i = 0;
    edge_t *edge;
    graph_t *graph;
    vertex_t *vertex;
    int vertex_idx, the_ring_size = 0, is_found = 0;

    assert(sizeof(int) == sizeof(element_type_t));

    if (!dfs || !dfs->graph || !ring_size) {
        return -1;
    }

    graph = dfs->graph;

    while (!stack_is_empty(&dfs->stack)) {
        vertex_idx  = stack_top(&dfs->stack);
        vertex      = &graph->vertexs[vertex_idx];

        is_found    = 0;

        LIST_FOR_EACH_ENTRY(edge, edge_t, tail_list, &vertex->first_out) {
            if (!graph_vertex_is_visited(graph, edge->head_vertex)) {
                // still fresh neighbors here
                stack_push(&dfs->stack, edge->head_vertex);
                graph_vertex_set_visited(graph, edge->head_vertex);
                is_found = 1;
                break;
            }

            if (graph_vertex_is_tnode(graph, edge->head_vertex) ||
                graph_vertex_is_rnode(graph, edge->head_vertex)) {
                // already detected as a ring or a terminal node
                continue;
            }

            // set DFS_RING_NODE tag
            graph_vertex_set_rnode(graph, edge->head_vertex);

            // if reach here, a ring is detected
            stack_peek_init(&dfs->stack);
            while ((vertex_idx = stack_peek(&dfs->stack)) != -1) {
                ++the_ring_size;
                if (vertex_idx == edge->head_vertex) {
                    *ring_size = the_ring_size;
                    return 0;
                }
            }
        }

        // no fresh neighbors anymore
        if (!is_found) {
            stack_pop(&dfs->stack);
            graph_vertex_set_tnode(graph, vertex_idx);
        }
    }

    /* if here, graph may still has isolated nodes there: multi-connected-component in the graph */
    for (i = 0; i < graph_vertex_max(graph); ++i) {
        if (!graph_vertex_is_visited(graph, i)) {
            stack_push(&dfs->stack, i);
            graph_vertex_set_visited(graph, i);
            break;
        }
    }

    return -1;
}

int graph_out_print(graph_t *graph)
{
    int i = 0;
    edge_t *edge;
    vertex_t *vertex;

    if (!graph || !graph->vertexs || !graph->vertexs_n) {
        return -1;
    }

    for (i = 0; i < graph->vertexs_n; ++i) {
        vertex = &graph->vertexs[i];

        if (vertex->out_degree == 0) {
            continue;
        }

        printf("\nvertex: %d\n", i);

        LIST_FOR_EACH_ENTRY(edge, edge_t, tail_list, &vertex->first_out) {
            printf(" - %d -> %d\n", edge->tail_vertex, edge->head_vertex);
        }
    }

    return 0;
}

int graph_in_print(graph_t *graph)
{
    int i = 0;
    edge_t *edge;
    vertex_t *vertex;

    if (!graph || !graph->vertexs || !graph->vertexs_n) {
        return -1;
    }

    for (i = 0; i < graph->vertexs_n; ++i) {
        vertex = &graph->vertexs[i];

        if (vertex->in_degree == 0) {
            continue;
        }

        printf("\nvertex: %d\n", i);

        LIST_FOR_EACH_ENTRY(edge, edge_t, head_list, &vertex->first_in) {
            printf(" - %d -> %d\n", edge->tail_vertex, edge->head_vertex);
        }
    }

    return 0;
}

