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

#include "stdlib.h"
#include "string.h"
#include "segment_tree.h"

#define ROOT                1
#define NODES_N(len)        (3 * len)

static void segtree_build(stree_node_t *nodes, int left, int right, int root)
{
    stree_node_t *node;

    node                = &nodes[root];
    node->left          = left;
    node->right         = right;
    node->mid           = (left + right) / 2;
    node->is_covered    = 0;

    if (left + 1 != right) {
        segtree_build(nodes, left, node->mid, 2 * root);
        segtree_build(nodes, node->mid, right, 2 * root + 1);
    }
}

int segtree_create(stree_t *stree, int left, int right)
{
    int length;
    stree_node_t *nodes;

    if (!stree) {
        return -1;
    }

    length = right - left;
    if (length <= 0) {
        return -1;
    }

    memset(stree, 0, sizeof(stree_t));

    if ((nodes = (stree_node_t *)malloc(NODES_N(length) * sizeof(stree_node_t))) == NULL) {
        return -1;
    }

    memset(nodes, 0, NODES_N(length) * sizeof(stree_node_t));

    segtree_build(nodes, left, right, ROOT);

    stree->nodes        = nodes;
    stree->length       = length;
    stree->left         = left;
    stree->right        = right;

    return 0;
}

int segtree_destroy(stree_t *stree)
{
    if (!stree || !stree->nodes || !stree->length) {
        return -1;
    }

    free(stree->nodes);
    return 0;
}

int segtree_reset(stree_t *stree)
{
    if (!stree || !stree->nodes || !stree->length) {
        return -1;
    }

    memset(stree->nodes, 0, NODES_N(stree->length) * sizeof(stree_node_t));

    segtree_build(stree->nodes, stree->left, stree->right, ROOT);

    return 0;
}

static void segtree_do_insert(stree_node_t *nodes, int left, int right, int root)
{
    stree_node_t *node;

    node = &nodes[root];

    if (node->left == left && node->right == right) {
        node->is_covered = 1;
        return;
    }

    if (right <= node->mid) {
        segtree_do_insert(nodes, left, right, 2 * root);
    } else if (left >= node->mid) {
        segtree_do_insert(nodes, left, right, 2 * root + 1);
    } else {
        segtree_do_insert(nodes, left, node->mid, 2 * root);
        segtree_do_insert(nodes, node->mid, right, 2 * root + 1);
    }
}

int segtree_insert(stree_t *stree, int left, int right)
{
    if (!stree || !stree->nodes) {
        return -1;
    }

    if (right - left < 0) {
        return -1;
    }

    segtree_do_insert(stree->nodes, left, right, ROOT);

    return 0;
}

static int segtree_do_delete(stree_node_t *nodes, int left, int right, int root)
{
    int is_covered = 0;
    stree_node_t *node;

    node = &nodes[root];

    if (node->left + 1 == node->right) {
        is_covered = node->is_covered;
        node->is_covered = 0;
        return is_covered;
    }

    if (node->is_covered) {
        node->is_covered                = 0;
        nodes[2 * root].is_covered      = 1;
        nodes[2 * root + 1].is_covered  = 1;
    }

    if (right <= node->mid) {
        return segtree_do_delete(nodes, left, right, 2 * root);
    } else if (left >= node->mid) {
        return segtree_do_delete(nodes, left, right, 2 * root + 1);
    } else {
        return segtree_do_delete(nodes, left, node->mid, 2 * root) &&
                segtree_do_delete(nodes, node->mid, right, 2 * root + 1);
    }
}

int segtree_delete(stree_t *stree, int left, int right)
{
    if (!stree || !stree->nodes) {
        return -1;
    }

    if (right - left < 0) {
        return -1;
    }

    return segtree_do_delete(stree->nodes, left, right, ROOT);
}

static int segtree_do_query(stree_node_t *nodes, int left, int right, int root)
{
    stree_node_t *node;

    node = &nodes[root];

    if (node->left + 1 == node->right) {
        return node->is_covered;
    }

    if (right <= node->mid) {
        return segtree_do_query(nodes, left, right, 2 * root);
    } else if (left >= node->mid) {
        return segtree_do_query(nodes, left, right, 2 * root + 1);
    } else {
        return segtree_do_query(nodes, left, node->mid, 2 * root) &&
                segtree_do_query(nodes, node->mid, right, 2 * root + 1);
    }

}

int segtree_query(stree_t *stree, int left, int right)
{
    if (!stree || !stree->nodes) {
        return -1;
    }

    if (right - left < 0) {
        return -1;
    }

    return segtree_do_query(stree->nodes, left, right, ROOT);
}

static int segtree_do_cal(stree_node_t *nodes, int root)
{
    stree_node_t *node;

    node = &nodes[root];

    if (node->is_covered) {
        return node->right - node->left;
    }

    if (node->left + 1 == node->right) {
        return 0;
    }

    return segtree_do_cal(nodes, 2 * root ) + segtree_do_cal(nodes, 2 * root + 1);
}

int segtree_cal(stree_t *stree)
{
    if (!stree || !stree->nodes) {
        return -1;
    }

    return segtree_do_cal(stree->nodes, ROOT);
}

