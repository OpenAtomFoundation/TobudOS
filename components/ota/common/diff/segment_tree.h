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

#ifndef _SEGMENT_TREE_H_
#define  _SEGMENT_TREE_H_

typedef struct segment_tree_node_st {
    int     left;
    int     right;
    int     mid;
    int     is_covered;
} stree_node_t;

typedef struct segment_tree_st {
    stree_node_t   *nodes;
    int             length;
    int             left;
    int             right;
} stree_t;

int segtree_create(stree_t *stree, int left, int right);

int segtree_destroy(stree_t *stree);

int segtree_reset(stree_t *stree);

int segtree_insert(stree_t *stree, int left, int right);

int segtree_delete(stree_t *stree, int left, int right);

int segtree_query(stree_t *stree, int left, int right);

int segtree_cal(stree_t *stree);

#endif

