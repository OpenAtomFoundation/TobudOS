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

#ifndef _GRAPH_H_
#define  _GRAPH_H_

#include "list.h"
#include "stack.h"

// visited
#define DFS_VISITED     (1 << 0)

// ring node
#define DFS_R_NODE      (1 << 1)

// terminal node
#define DFS_T_NODE      (1 << 2)

#define IS_VISITED(tag)     ((tag) & DFS_VISITED)

#define IS_T_NODE(tag)      ((tag) & DFS_R_NODE)

#define IS_R_NODE(tag)      ((tag) & DFS_T_NODE)

#define SET_VISITED(tag)    ((tag) | DFS_VISITED)

#define SET_R_NODE(tag)     ((tag) | DFS_R_NODE)

#define SET_T_NODE(tag)     ((tag) | DFS_T_NODE)

typedef uint8_t         vertex_tag_t;
typedef vertex_tag_t    v_tag_t;

/*
    head
    ^
    |
    |
    |
    tail
 */
typedef struct edge_st {
    int         tail_vertex;
    int         head_vertex;

    list_t      tail_list;  /* all the egdes with the same tail with us */
    list_t      head_list; /* with same head */
} edge_t;

typedef struct vertex_st {
    list_t      first_in;
    list_t      first_out;

    int         in_degree;
    int         out_degree;

    v_tag_t     tag;
} vertex_t;

typedef struct graph_st {
    vertex_t   *vertexs;

    int         vertexs_n;
    int         edges_n;
} graph_t;

typedef struct depth_first_search_st {
    stack_t     stack;
    graph_t    *graph;
} dfs_t;

typedef void (*graph_edge_delete_cb)(int tail_vertex, int head_vertex, void *cb_arg);

int graph_create(graph_t *graph, int vertexs_n);

int graph_destroy(graph_t *graph);

int graph_edge_add(graph_t *graph, int tail_vertex, int head_vertex);

int graph_edge_rmv(graph_t *graph, int tail_vertex, int head_vertex);

int graph_edge_rmv_by_tail(graph_t *graph, int tail_vertex, graph_edge_delete_cb del_cb, void *cb_arg);

int graph_edge_rmv_one_by_tail(graph_t *graph, int tail_vertex);

int graph_edge_rmv_by_head(graph_t *graph, int head_vertex, graph_edge_delete_cb del_cb, void *cb_arg);

int graph_edge_rmv_one_by_head(graph_t *graph, int head_vertex);

int graph_edgesn_get(graph_t *graph);

int graph_vertex_max(graph_t *graph);

int graph_indegree_get(graph_t *graph, int vertex);

int graph_outdegree_get(graph_t *graph, int vertex);

int graph_tag_set(graph_t *graph, int vertex, v_tag_t tag);

int graph_tag_reset(graph_t *graph, int vertex);

v_tag_t graph_tag_get(graph_t *graph, int vertex);

int graph_dfs_create(dfs_t *dfs, graph_t *graph);

int graph_dfs_destroy(dfs_t *dfs);

int graph_dfs_has_next(dfs_t *dfs);

int graph_dfs_ring_detect(dfs_t *dfs, int *ring_size);

int graph_out_print(graph_t *graph);

int graph_in_print(graph_t *graph);

static inline void graph_vertex_set_visited(graph_t *graph, int vertex)
{
    graph_tag_set(graph, vertex, SET_VISITED(graph_tag_get(graph, vertex)));
}

static inline void graph_vertex_set_rnode(graph_t *graph, int vertex)
{
    graph_tag_set(graph, vertex, SET_R_NODE(graph_tag_get(graph, vertex)));
}

static inline void graph_vertex_set_tnode(graph_t *graph, int vertex)
{
    graph_tag_set(graph, vertex, SET_T_NODE(graph_tag_get(graph, vertex)));
}

static inline int graph_vertex_is_visited(graph_t *graph, int vertex)
{
    return IS_VISITED(graph_tag_get(graph, vertex));
}

static inline int graph_vertex_is_rnode(graph_t *graph, int vertex)
{
    return IS_R_NODE(graph_tag_get(graph, vertex));
}

static inline int graph_vertex_is_tnode(graph_t *graph, int vertex)
{
    return IS_T_NODE(graph_tag_get(graph, vertex));
}

#endif

