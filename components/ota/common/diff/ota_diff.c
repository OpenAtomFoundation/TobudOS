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

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "bsdiff.h"
#include "wstream.h"
#include "proc_bar.h"
#include "graph.h"
#include "stack.h"
#include "topo_sorting.h"
#include "segment_tree.h"
#include "lzma_compress.h"
#include "lzma_uncompress.h"

#include "ota_diff.h"

old_blk_info_t *old_blk_infos = NULL;
new_blk_info_t *new_blk_infos = NULL;

summary_t summary = {
    .ring_cnt                       = 0,
    .ring_size_max                  = 0,
    .ring_size_min                  = INT_MAX,

    .patch_size                     = 0,

    .block_len                      = 0,
    .block_cnt                      = 0,

    .zipped_patch4block_size_max    = 0,
    .zipped_patch4block_size_min    = INT_MAX,

    .patch4block_size_max           = 0,
    .patch4block_size_min           = INT_MAX,

    .safe_block_cnt                 = 0,
    .is_safe_block_ignored          = 0,
};

static int32_t offtin_u32(uint8_t *buf)
{
    int32_t y;

    y = buf[3] & 0x7F;
    y = y * 256; y += buf[2];
    y = y * 256; y += buf[1];
    y = y * 256; y += buf[0];

    if (buf[3] & 0x80) {
        y = -y;
    }

    return y;
}

static uint16_t offtin_u16(uint8_t *buf)
{
    int32_t y;

    y = buf[1];
    y = y * 256; y += buf[0];

    return y;
}

static void offtout_u32(int32_t x, uint8_t *buf)
{
    int32_t y;

    if (x < 0) {
        y = -x;
    } else {
        y = x;
    }

    buf[0] = y % 256; y -= buf[0];
    y = y / 256; buf[1] = y % 256; y -= buf[1];
    y = y / 256; buf[2] = y % 256; y -= buf[2];
    y = y / 256; buf[3] = y % 256;

    if (x < 0) {
        buf[3] |= 0x80;
    }
}

static void offtout_u16(uint16_t y, uint8_t *buf)
{
    buf[0] = y % 256; y -= buf[0];
    y = y / 256; buf[1] = y % 256;
}

static void relied_insert_by_new_idx_from(relied_detail_t *detail, list_t *list)
{
    relied_detail_t *iter;

    LIST_FOR_EACH_ENTRY(iter, relied_detail_t, list, list) {
        if (iter->new_blk_idx > detail->new_blk_idx) {
            break;
        }

        if (iter->new_blk_idx == detail->new_blk_idx &&
            iter->new_from > detail->new_from) {
            break;
        }
    }
    list_add_tail(&detail->list, &iter->list);
}

static int old_relied_add(int32_t old_blk_idx, int32_t new_blk_idx, int32_t new_from, int32_t old_from, int32_t diff_from, int32_t step)
{
    relied_detail_t *detail;
    old_blk_info_t *old_blk_info;

    if ((detail = malloc(sizeof(relied_detail_t))) == NULL) {
        printf("malloc failed");
        return -1;
    }

    old_blk_info = &old_blk_infos[old_blk_idx];

    list_init(&detail->list);
    detail->new_blk_idx = new_blk_idx;
    detail->new_from    = new_from;
    detail->old_from    = old_from;
    detail->diff_from   = diff_from;
    detail->step        = step;

    relied_insert_by_new_idx_from(detail, &old_blk_info->relied_list);
    ++old_blk_info->relied_cnt;

    return 0;
}

static void cmd_insert_by_new_from(cmd_t *cmd, list_t *list)
{
    cmd_t *iter;

    LIST_FOR_EACH_ENTRY(iter, cmd_t, list, list) {
        if (iter->new_from > cmd->new_from) {
            break;
        }
    }
    list_add_tail(&cmd->list, &iter->list);
}

static int extra_cmd_add(int32_t new_blk_idx, int32_t new_from, int32_t extra_from, int32_t step)
{
    cmd_t *cmd;
    new_blk_info_t *new_blk_info;

    if ((cmd = malloc(sizeof(cmd_t))) == NULL) {
        printf("malloc failed");
        return -1;
    }

    new_blk_info = &new_blk_infos[new_blk_idx];

    cmd->type                           = CMD_TYPE_EXTRA;
    cmd->new_from                       = new_from;
    cmd->step                           = step;

    cmd->detail.extra_detail.extra_from = extra_from;

    list_init(&cmd->list);
    cmd_insert_by_new_from(cmd, &new_blk_info->cmd_list);

    ++new_blk_info->cmd_cnt;

    return 0;
}

static int diff_cmd_add(int32_t new_blk_idx, int32_t new_from, int32_t old_from, int32_t diff_from, int32_t step)
{
    cmd_t *cmd;
    new_blk_info_t *new_blk_info;

    if ((cmd = malloc(sizeof(cmd_t))) == NULL) {
        printf("malloc failed");
        return -1;
    }

    new_blk_info = &new_blk_infos[new_blk_idx];

    cmd->type                           = CMD_TYPE_DIFF;
    cmd->new_from                       = new_from;
    cmd->step                           = step;

    cmd->detail.diff_detail.diff_from   = diff_from;
    cmd->detail.diff_detail.old_from    = old_from;

    list_init(&cmd->list);
    cmd_insert_by_new_from(cmd, &new_blk_info->cmd_list);

    ++new_blk_info->cmd_cnt;

    return 0;
}

static int blk_cmd_verify(size_t oldsize, size_t newsize, size_t blk_len)
{
    int rc = 0, i = 0;
    cmd_t *cmd;
    stree_t stree;
    new_blk_info_t *new_blk_info;
    int32_t new_blk_cnt = BLK_CNT(newsize, blk_len);
    int32_t lower_bound;
    size_t cmd_newsize = 0;

    if (segtree_create(&stree, 0, blk_len) != 0) {
        ERROR("segment tree create failed!\n");
    }

    for (i = 0; i < new_blk_cnt; ++i) {
        new_blk_info    = &new_blk_infos[i];
        lower_bound     = BLK_LOWER_BOUND(i, blk_len);

        LIST_FOR_EACH_ENTRY(cmd, cmd_t, list, &new_blk_info->cmd_list) {
            if (segtree_query(&stree, cmd->new_from - lower_bound, cmd->new_from + cmd->step - lower_bound)) {
                ERROR("corrupt blk cmd!\n");
            }
            segtree_insert(&stree, cmd->new_from - lower_bound, cmd->new_from + cmd->step - lower_bound);
        }

        if (i != new_blk_cnt - 1 &&
            segtree_cal(&stree) != blk_len) {
            ERROR("incompleted cmd!\n");
        }

        if (i == new_blk_cnt - 1 &&
            segtree_cal(&stree) != BLK_EXTRA(newsize, blk_len)) {
            ERROR("incompleted cmd!\n");
        }
        cmd_newsize += segtree_cal(&stree);

        segtree_reset(&stree);
    }

    if (cmd_newsize != newsize) {
        ERROR("incompleted cmd for newsize!\n");
    }

OUT:
    segtree_destroy(&stree);

    return rc;
}

static int old_blk_safe_refresh(uint8_t *old, size_t oldsize, uint8_t *new, size_t newsize, size_t blk_len, uint8_t *diff_blk, uint8_t *extra_blk)
{
    int rc = 0, i = 0, j = 0;
    int32_t old_blk_cnt = BLK_CNT(oldsize, blk_len);
    int32_t new_blk_cnt = BLK_CNT(newsize, blk_len);
    int32_t lower_bound;

    cmd_t *cmd;
    old_blk_info_t *old_blk_info;
    new_blk_info_t *new_blk_info;
    uint8_t *buf = NULL;
    size_t size2cmp;

    if ((buf = malloc(blk_len)) == NULL) {
        ERROR("malloc failed!\n");
    }

    for (i = 0; i < MIN(old_blk_cnt, new_blk_cnt); ++i) {
        old_blk_info = &old_blk_infos[i];
        new_blk_info = &new_blk_infos[i];

        memset(buf, 0, blk_len);

        LIST_FOR_EACH_ENTRY(cmd, cmd_t, list, &new_blk_info->cmd_list) {
            lower_bound = BLK_LOWER_BOUND(i, blk_len);

            if (cmd->type == CMD_TYPE_DIFF) {
                for (j = 0; j < cmd->step; ++j) {
                    (buf + cmd->new_from - lower_bound)[j] = (old + cmd->detail.diff_detail.old_from)[j] + ((int8_t *)diff_blk + cmd->detail.diff_detail.diff_from)[j];
                }
            } else if (cmd->type == CMD_TYPE_EXTRA) {
                memcpy(buf + cmd->new_from - lower_bound, extra_blk + cmd->detail.extra_detail.extra_from, cmd->step);
            }
        }

        size2cmp = (i == new_blk_cnt - 1 ? BLK_EXTRA(newsize, blk_len) : blk_len);

        if (memcmp(buf, THE_BUF(new, i, blk_len), size2cmp) != 0) {
            ERROR("wrong bspatch block cmd!\n");
        }

        if (memcmp(buf, THE_BUF(old, i, blk_len), size2cmp) == 0) {
            /* this block is all the same after patch */
            old_blk_info->is_safe = 1;
        }
    }

OUT:
    FREE(buf);

    return rc;
}

static int blk_info_build(int32_t X, int32_t Y, int32_t new_from, int32_t old_from, int32_t diff_from, int32_t extra_from, size_t blk_len)
{
    /*
    "add x bytes
        from oldfile to x bytes from the diff block; copy y bytes from the
        extra block; seek forwards in oldfile by z bytes".
    */
    int rc = 0;

    int32_t step;

    int32_t old_blk_idx             = BLK_IDX(old_from, blk_len);
    int32_t old_blk_upper_bound     = BLK_UPPER_BOUND(old_from, blk_len);
    int32_t old_blk_remaining       = old_blk_upper_bound - old_from;

    int32_t new_blk_idx             = BLK_IDX(new_from, blk_len);
    int32_t new_blk_upper_bound     = BLK_UPPER_BOUND(new_from, blk_len);
    int32_t new_blk_remaining       = new_blk_upper_bound - new_from;

    while (X) {
        step = MIN_TRIPLE(new_blk_remaining, old_blk_remaining, X);

        new_blk_remaining   -= step;
        old_blk_remaining   -= step;
        X                   -= step;

        if (diff_cmd_add(new_blk_idx, new_from, old_from, diff_from, step) != 0) {
            ERROR("diff cmd add failed");
        }

        if (old_relied_add(old_blk_idx, new_blk_idx, new_from, old_from, diff_from, step) != 0) {
            ERROR("old relied add failed");
        }

        new_from            += step;
        old_from            += step;
        diff_from           += step;

        if (new_blk_remaining == 0) {
            new_blk_idx         = BLK_IDX(new_from, blk_len);
            new_blk_upper_bound = BLK_UPPER_BOUND(new_from, blk_len);
            new_blk_remaining   = new_blk_upper_bound - new_from;
        }

        if (old_blk_remaining == 0) {
            old_blk_idx         = BLK_IDX(old_from, blk_len);
            old_blk_upper_bound = BLK_UPPER_BOUND(old_from, blk_len);
            old_blk_remaining   = old_blk_upper_bound - old_from;
        }
    }

    while (Y) {
        step = MIN(new_blk_remaining, Y);

        new_blk_remaining   -= step;
        Y                   -= step;

        if (extra_cmd_add(new_blk_idx, new_from, extra_from, step) != 0) {
            ERROR("extra cmd add failed");
        }

        new_from            += step;
        extra_from          += step;

        if (new_blk_remaining == 0) {
            new_blk_idx         = BLK_IDX(new_from, blk_len);
            new_blk_upper_bound = BLK_UPPER_BOUND(new_from, blk_len);
            new_blk_remaining   = new_blk_upper_bound - new_from;
        }
    }

OUT:
    return rc;
}

static int bspatch_parse(size_t oldsize, size_t newsize, size_t blk_len, uint8_t *patch, size_t patchsize, uint8_t **the_diff_blk, uint8_t **the_extra_blk)
{
    int rc = 0;

    int32_t X, Y, Z;
    int32_t new_from = 0, old_from = 0;

    int32_t ctl_len, diff_len, new_len;
    int32_t ctl_from = 0, diff_from = 0, extra_from = 0;
    uint8_t *ctl_blk, *diff_blk, *extra_blk;

    ctl_len         = offtin_u32(patch);
    patch          += sizeof(int32_t);
    diff_len        = offtin_u32(patch);
    patch          += sizeof(int32_t);
    new_len         = offtin_u32(patch);
    patch          += sizeof(int32_t);

    if (new_len != newsize) {
        ERROR("invalid bspatch");
    }

    ctl_blk         = patch;
    diff_blk        = patch + ctl_len;
    extra_blk       = patch + ctl_len + diff_len;

    while (ctl_from < ctl_len) {
        /* read the original X, Y, Z from ctl block */
        X               = offtin_u32(ctl_blk + ctl_from);
        ctl_from       += sizeof(int32_t);

        Y               = offtin_u32(ctl_blk + ctl_from);
        ctl_from       += sizeof(int32_t);

        Z               = offtin_u32(ctl_blk + ctl_from);
        ctl_from       += sizeof(int32_t);

        blk_info_build(X, Y, new_from, old_from, diff_from, extra_from, blk_len);

        new_from    += X + Y;
        old_from    += X + Z;
        diff_from   += X;
        extra_from  += Y;
    }

    *the_diff_blk   = diff_blk;
    *the_extra_blk  = extra_blk;

OUT:
    return rc;
}

static int blk_info_create(size_t oldsize, size_t newsize, size_t blk_len)
{
    int rc = 0, i = 0;

    int32_t old_blk_cnt = BLK_CNT(oldsize, blk_len);
    int32_t new_blk_cnt = BLK_CNT(newsize, blk_len);

    if ((old_blk_infos = malloc(old_blk_cnt * sizeof(old_blk_info_t))) == NULL) {
        ERROR("malloc failed");
    }

    if ((new_blk_infos = malloc(new_blk_cnt * sizeof(new_blk_info_t))) == NULL) {
        ERROR("malloc failed");
    }

    memset(old_blk_infos, 0, old_blk_cnt * sizeof(old_blk_info_t));
    memset(new_blk_infos, 0, new_blk_cnt * sizeof(new_blk_info_t));

    for (i = 0; i < old_blk_cnt; ++i) {
        list_init(&old_blk_infos[i].relied_list);
        list_init(&old_blk_infos[i].simplified_relied_list);
    }

    for (i = 0; i < new_blk_cnt; ++i) {
        list_init(&new_blk_infos[i].cmd_list);
    }

    return 0;

OUT:
    if (old_blk_infos) {
        free(old_blk_infos);
    }

    if (new_blk_infos) {
        free(new_blk_infos);
    }

    return rc;
}

static void blk_info_destroy(size_t oldsize, size_t newsize, size_t blk_len)
{
    int i = 0;
    cmd_t *cmd, *c_tmp;
    relied_detail_t *detail, *d_tmp;
    smpl_relied_detail_t *smpl_detail, *sd_tmp;

    int32_t old_blk_cnt = BLK_CNT(oldsize, blk_len);
    int32_t new_blk_cnt = BLK_CNT(newsize, blk_len);

    old_blk_info_t *old_blk_info;
    new_blk_info_t *new_blk_info;

    if (old_blk_infos) {
        for (i = 0; i < old_blk_cnt; ++i) {
            old_blk_info = &old_blk_infos[i];

            LIST_FOR_EACH_ENTRY_SAFE(detail, d_tmp, relied_detail_t, list, &old_blk_info->relied_list) {
                list_del(&detail->list);
                free(detail);
            }

            LIST_FOR_EACH_ENTRY_SAFE(smpl_detail, sd_tmp, smpl_relied_detail_t, list, &old_blk_info->simplified_relied_list) {
                list_del(&smpl_detail->list);
                free(smpl_detail);
            }
        }

        free(old_blk_infos);
    }

    if (new_blk_infos) {
        for (i = 0; i < new_blk_cnt; ++i) {
            new_blk_info = &new_blk_infos[i];

            LIST_FOR_EACH_ENTRY_SAFE(cmd, c_tmp, cmd_t, list, &new_blk_info->cmd_list) {
                list_del(&cmd->list);
                free(cmd);
            }
        }

        free(new_blk_infos);
    }

    old_blk_infos = NULL;
    new_blk_infos = NULL;
}

static void verbose_print(graph_t *graph, size_t oldsize, size_t newsize, size_t blk_len)
{
    int i = 0;
    old_blk_info_t *old_blk_info;
    int32_t old_blk_cnt = BLK_CNT(oldsize, blk_len);

    smpl_relied_detail_t *smpl_detail;

#if 0
    int is_safe;
    cmd_t *cmd;
    new_blk_info_t *new_blk_info;
    int32_t new_blk_cnt = BLK_CNT(newsize, blk_len);
    relied_detail_t *detail;

    printf("====== patch cmd list ======\n");
    for (i = 0; i < new_blk_cnt; ++i) {
        is_safe = 0;

        new_blk_info = &new_blk_infos[i];

        if (i < old_blk_cnt) {
            is_safe = old_blk_infos[i].is_safe;
        }
        printf("\nnew block idx: %-4d%s\n", i, is_safe ? "(SAFE)" : "");

        LIST_FOR_EACH_ENTRY(cmd, cmd_t, list, &new_blk_info->cmd_list) {
            printf(" - (%s)new_from: %-8d  step: %-5d", cmd->type == CMD_TYPE_DIFF ? "DIFF " : "EXTRA", cmd->new_from, cmd->step);
            if (cmd->type == CMD_TYPE_DIFF) {
                printf(" old_from: %-8d(%-4d)  diff_from: %-8d\n",
                                            cmd->detail.diff_detail.old_from,
                                            BLK_IDX(cmd->detail.diff_detail.old_from, blk_len),
                                            cmd->detail.diff_detail.diff_from);
            } else {
                printf(" extra_from: %-8d\n", cmd->detail.extra_detail.extra_from);
            }
        }
    }

    printf("\n\n====== relied relationship list ======\n");
    for (i = 0; i < old_blk_cnt; ++i) {
        old_blk_info = &old_blk_infos[i];
        printf("\nold block idx: %-4d%s\n", i, old_blk_info->is_safe ? "(SAFE)" : "");

        LIST_FOR_EACH_ENTRY(detail, relied_detail_t, list, &old_blk_info->relied_list) {
            printf(" - new block idx: %-4d  new_from: %-8d old_from: %-8d  step: %-5d  diff_from: %-8d\n", detail->new_blk_idx, detail->new_from, detail->old_from, detail->step, detail->diff_from);
        }
    }
#endif

    printf("\n\n====== simplified relied relationship list ======\n");
    for (i = 0; i < old_blk_cnt; ++i) {
        old_blk_info = &old_blk_infos[i];
        if (old_blk_info->simplified_relied_cnt == 0) {
            continue;
        }

        printf("\nold block idx: %-4d%s\n", i, old_blk_info->is_safe ? "(SAFE)" : "");

        LIST_FOR_EACH_ENTRY(smpl_detail, smpl_relied_detail_t, list, &old_blk_info->simplified_relied_list) {
            printf(" - new block idx: %-4d\n", smpl_detail->new_blk_idx);
        }
    }
    printf("\n==================================================\n\n");

#if 0
    printf("\n\n====== graph of relied relationship ======\n");
    graph_in_print(graph);
#endif
}

static void summary_print(void)
{
    printf("\n============= summary =============\n");
    printf("RINGs in graph          : %8d\n", summary.ring_cnt);
    if (summary.ring_cnt != 0) {
        printf("RING size MAX           : %8d\n", summary.ring_size_max);
        printf("RING size MIN           : %8d\n", summary.ring_size_min);
    }

    printf("\n");
    printf("PATCH size              : %8d\n", summary.patch_size);

    printf("\n");
    printf("BLOCK size              : %8d\n", summary.block_len);
    printf("BLOCK count             : %8d\n", summary.block_cnt);

    printf("\n");
    printf("ZIPPED patch4block MAX  : %8d\n", summary.zipped_patch4block_size_max);
    printf("ZIPPED patch4block MIN  : %8d\n", summary.zipped_patch4block_size_min);

    printf("\n");
    printf("PATCH4BLOCK MAX         : %8d\n", summary.patch4block_size_max);
    printf("PATCH4BLOCK MIN         : %8d\n", summary.patch4block_size_min);

    printf("\n");
    printf("SAFE block count        : %8d%s\n", summary.safe_block_cnt, summary.is_safe_block_ignored ? "(IGNORED)" : "");
    printf("===================================\n");
}

static int relied_simplify(size_t oldsize, size_t blk_len, graph_t *graph)
{
    int i = 0;
    int32_t last_new_blk_idx = -1;
    relied_detail_t *detail, *d_tmp;
    old_blk_info_t *old_blk_info;
    smpl_relied_detail_t *smpl_detail;
    int32_t old_blk_cnt = BLK_CNT(oldsize, blk_len);

    for (i = 0; i < old_blk_cnt; ++i) {
        old_blk_info = &old_blk_infos[i];

        last_new_blk_idx = -1;
        LIST_FOR_EACH_ENTRY_SAFE(detail, d_tmp, relied_detail_t, list, &old_blk_info->relied_list) {
            if (detail->new_blk_idx == last_new_blk_idx) {
                continue;
            }

            if (detail->new_blk_idx == i) {
                last_new_blk_idx = detail->new_blk_idx;
                continue;
            }

            last_new_blk_idx = detail->new_blk_idx;
            if ((smpl_detail = malloc(sizeof(smpl_relied_detail_t))) == NULL) {
                return -1;
            }

            smpl_detail->new_blk_idx = detail->new_blk_idx;
            list_init(&smpl_detail->list);

            list_add(&smpl_detail->list, &old_blk_info->simplified_relied_list);

            ++old_blk_info->simplified_relied_cnt;

            /* we use a graph to record the dependency(relied) relationship */
            /* use topological sorting to find the order sequence to restore the new block */
            if (i < graph_vertex_max(graph)) {
                /* point from tail(rely-ing) to head(relied) */
                /* detail->new_blk_idx(tail) relied on old block i(head)*/
                graph_edge_add(graph, detail->new_blk_idx, i);
            }
        }
    }

    return 0;
}

int analysing_dependency(graph_t *graph)
{
    dfs_t dfs;
    int ring_size;

    if (graph_dfs_create(&dfs, graph) != 0) {
        return -1;
    }

    while (graph_dfs_has_next(&dfs)) {
        if (graph_dfs_ring_detect(&dfs, &ring_size) == 0) {
            // printf("$$$  %d\n", ring_size);
            if (ring_size < summary.ring_size_min) {
                summary.ring_size_min = ring_size;
            }
            if (ring_size > summary.ring_size_max) {
                summary.ring_size_max = ring_size;
            }
            ++summary.ring_cnt;
        }
    }

    graph_dfs_destroy(&dfs);

    return 0;
}

static int is_all_zeros(uint8_t *buf, size_t size)
{
    int i = 0;

    for (i = 0; i < size; ++i) {
        if (buf[i] != 0) {
            return 0;
        }
    }

    return 1;
}

static int patch4block_assemble(int new_blk_idx, size_t blk_len, uint8_t *diff_blk, uint8_t *extra_blk, uint8_t *patch4blk, size_t patch4blk_size, size_t *the_patch4blk_size)
{
    int rc = 0, i = 0;
    int32_t len, the_len;
    size_t the_patchsize;

    cmd_t *cmd, *tmp;
    wstream_t ws;
    new_blk_info_t *new_blk_info = &new_blk_infos[new_blk_idx];

    uint8_t *diff = NULL, *extra = NULL;
    uint32_t diff_len = 0, extra_len = 0;
    /*
     format of patch for one block:
         0          2   I block index of the patch
         2          2   N count of cmd
         4          2   C sizeof(control block)
         6          2   D sizeof(diff block)

         8          C   control block
         8 + C      D   diff block
         8 + C + D  ?   extra block
     */
    /* control block:
        with a leading 8bit domain, indicating the cmd type, 0 for DIFF, 1 for EXTRA

        if current is a CMD_COPY(X, Y, Z) cmd:
            X is 0(COPY)
            locate old to Z, copy Y bytes from oldfile

        if current is a CMD_DIFF(X, Y, Z) cmd:
            X is 1(DIFF)
            locate old to Z, add Y bytes from oldfile to Y bytes from the diff block

        if current is a CMD_EXTRA(X, Y) cmd:
            X is 2(EXTRA)
            copy Y bytes from the extra block
     */
    uint8_t X;
    int is_diff_all_zeros = 0;
    uint32_t Z;
    uint16_t I = (uint16_t)new_blk_idx, N = (uint16_t)new_blk_info->cmd_cnt, Y;
    uint8_t header[FOLD_N(sizeof(uint16_t) / sizeof(uint8_t), 4)];
    uint8_t buf[FOLD_N(sizeof(uint32_t) / sizeof(uint8_t), 1)];

    if ((diff = malloc(blk_len)) == NULL) {
        ERROR("malloc failed!\n");
    }

    if ((extra = malloc(blk_len)) == NULL) {
        ERROR("malloc failed!\n");
    }

    wstream_create(&ws, patch4blk, patch4blk_size);
    /* reserve space for C & D */
    if (wstream_write_stream(&ws, header, sizeof(header)) != 0) {
        ERROR("wstream full!\n");
    }

    LIST_FOR_EACH_ENTRY_SAFE(cmd, tmp, cmd_t, list, &new_blk_info->cmd_list) {
        if (cmd->type == CMD_TYPE_DIFF) {
            Y = cmd->step;
            Z = cmd->detail.diff_detail.old_from;

            is_diff_all_zeros = is_all_zeros(&diff_blk[cmd->detail.diff_detail.diff_from], Y);
            X = is_diff_all_zeros ? CMD_COPY : CMD_DIFF;

            /* write control */
            if (wstream_write_byte(&ws, X) != 0) {
                ERROR("wstream full!\n");
            }
            offtout_u16(Y, buf);
            if (wstream_write_stream(&ws, buf, sizeof(uint16_t)) != 0) {
                ERROR("wstream full!\n");
            }
            offtout_u32(Z, buf);
            if (wstream_write_stream(&ws, buf, sizeof(uint32_t)) != 0) {
                ERROR("wstream full!\n");
            }

            if (is_diff_all_zeros) {
                continue;
            }

            for (i = 0; i < Y; ++i) {
                diff[diff_len++] = diff_blk[cmd->detail.diff_detail.diff_from + i];
            }
        } else if (cmd->type == CMD_TYPE_EXTRA) {
            X = CMD_EXTRA;
            Y = cmd->step;

            /* write control */
            if (wstream_write_byte(&ws, X) != 0) {
                ERROR("wstream full!\n");
            }
            offtout_u16(Y, buf);
            if (wstream_write_stream(&ws, buf, sizeof(uint16_t)) != 0) {
                ERROR("wstream full!\n");
            }

            for (i = 0; i < Y; ++i) {
                extra[extra_len++] = extra_blk[cmd->detail.extra_detail.extra_from + i];
            }
        }

        list_del(&cmd->list);
        free(cmd);
    }

    /* compute size of ctrl data */
    len = wstream_length_get(&ws);
    if (len == -1) {
        ERROR("wstream invalid!\n");
    }

    /* write block index */
    offtout_u16(I, header + FOLD_N(sizeof(uint16_t), 0));

    /* write cmd count */
    offtout_u16(N, header + FOLD_N(sizeof(uint16_t), 1));

    /* write size of ctrl data */
    offtout_u16(len - sizeof(header), header + FOLD_N(sizeof(uint16_t), 2));

    /* write diff data */
    if (wstream_write_stream(&ws, diff, diff_len) != 0) {
        ERROR("wstream full!\n");
    }

    /* compute size of diff data */
    the_len = wstream_length_get(&ws);
    if (the_len == -1) {
        ERROR("wstream invalid!\n");
    }

    /* write size of diff data */
    offtout_u16(the_len - len, header + FOLD_N(sizeof(uint16_t), 3));

    /* write extra data */
    if (wstream_write_stream(&ws, extra, extra_len) != 0) {
        ERROR("wstream full!\n");
    }

    /* write header */
    if (wstream_write_stream_at(&ws, 0, header, sizeof(header)) != 0) {
        ERROR("wstream full!\n");
    }

    the_patchsize = wstream_length_get(&ws);
    if (the_patchsize == -1) {
        ERROR("wstream invalid!\n");
    }

    *the_patch4blk_size = the_patchsize;

OUT:
    FREE(diff);
    FREE(extra);

    wstream_destroy(&ws);

    return rc;
}

static int make_patch(graph_t *graph, size_t oldsize, size_t newsize, size_t blk_len, uint8_t *diff_blk, uint8_t *extra_blk, int is_safe_ignored, uint8_t **patch, size_t *patchsize)
{
    int rc = 0;
    int new_blk_idx;
    lzma_err_t err;

    wstream_t ws;
    topo_sorting_t topo_sorting;
    proc_bar_t proc_bar;

    uint8_t *final_patch = NULL, *patch4blk = NULL, *zipped_patch4blk = NULL;
    size_t final_patch_size = FOLD_N(newsize, 3);
    size_t patch4blk_size = FOLD_N(blk_len, 3);
    size_t zippped_patch4blk_size = FOLD_N(blk_len, 3);
    size_t the_patch4blk_size, the_zipped_patch4blk_size;

    uint8_t patch_hdr[FOLD_N(sizeof(uint16_t) / sizeof(uint8_t), 2)];
    uint8_t patch4blk_hdr[FOLD_N(sizeof(uint16_t) / sizeof(uint8_t), 2)];

    uint16_t blk_cnt = 0;

    /* how many rings are there in the graph */
    uint16_t ring_cnt = 0;

    /*
      format of patch:
      patch_hdr + (patch4blk_hdr + patch4blk) * N

      patch_hdr:
        0   2   length of one block
        2   2   total count of blocks in the patch

      patch4blk_hdr:
        0   2   original length of the patch
        2   2   zipped length of the patch
     */

    if ((final_patch = malloc(final_patch_size * sizeof(uint8_t))) == NULL) {
        ERROR("malloc failed!\n");
    }

    if ((patch4blk = malloc(patch4blk_size * sizeof(uint8_t))) == NULL) {
        ERROR("malloc failed!\n");
    }

    if ((zipped_patch4blk = malloc(zippped_patch4blk_size * sizeof(uint8_t))) == NULL) {
        ERROR("malloc failed!\n");
    }

    wstream_create(&ws, final_patch, final_patch_size);
    /* reserve space for patch_hdr */
    if (wstream_write_stream(&ws, patch_hdr, sizeof(patch_hdr)) != 0) {
        ERROR("wstream full!\n");
    }

    if (topo_sorting_create(&topo_sorting, graph) != 0) {
        ERROR("malloc failed!\n");
    }

    proc_bar_init(&proc_bar, BLK_CNT(newsize, blk_len));

MAKE_PATCH:
    while (topo_sorting_has_next(&topo_sorting)) {
        new_blk_idx = topo_sorting_next(&topo_sorting);

        if (new_blk_idx < BLK_CNT(oldsize, blk_len) &&
            old_blk_infos[new_blk_idx].is_safe) {
            /* this is a safe block, and we wanna ignore it */
            ++summary.safe_block_cnt;

            if (is_safe_ignored) {
                summary.is_safe_block_ignored = 1;
                proc_bar_update(&proc_bar);
                continue;
            }
        }

        /* assemble the army! */
        if (patch4block_assemble(new_blk_idx,
                                    blk_len,
                                    diff_blk,
                                    extra_blk,
                                    patch4blk,
                                    patch4blk_size,
                                    &the_patch4blk_size) != 0) {
            ERROR("patch4block_assemble failed!\n");
        }

        the_zipped_patch4blk_size = zippped_patch4blk_size;
        err = lzma_compress(zipped_patch4blk, &the_zipped_patch4blk_size, patch4blk, the_patch4blk_size);
        if (err != LZMA_ERR_OK) {
            ERROR("lzma compress failed");
        }

        offtout_u16((uint16_t)the_patch4blk_size,           patch4blk_hdr);
        offtout_u16((uint16_t)the_zipped_patch4blk_size,    patch4blk_hdr + sizeof(uint16_t));

        if (wstream_write_stream(&ws, patch4blk_hdr, sizeof(patch4blk_hdr)) != 0) {
            ERROR("wstream full!\n");
        }

        if (wstream_write_stream(&ws, zipped_patch4blk, the_zipped_patch4blk_size) != 0) {
            ERROR("wstream full!\n");
        }

        proc_bar_update(&proc_bar);

        if (the_patch4blk_size > summary.patch4block_size_max) {
            summary.patch4block_size_max = the_patch4blk_size;
        }
        if (the_patch4blk_size < summary.patch4block_size_min) {
            summary.patch4block_size_min = the_patch4blk_size;
        }
        if (the_zipped_patch4blk_size > summary.zipped_patch4block_size_max) {
            summary.zipped_patch4block_size_max = the_zipped_patch4blk_size;
        }
        if (the_zipped_patch4blk_size < summary.zipped_patch4block_size_min) {
            summary.zipped_patch4block_size_min = the_zipped_patch4blk_size;
        }

        ++blk_cnt;
    }

    /* still edges left? there's at least one ring in the graph */
    if (topo_has_ring(&topo_sorting)) {
        ++ring_cnt;
        topo_ring_break(&topo_sorting);
        goto MAKE_PATCH;
    }

    offtout_u16((uint16_t)blk_len,      patch_hdr);
    offtout_u16((uint16_t)blk_cnt,      patch_hdr + sizeof(uint16_t));
    if (wstream_write_stream_at(&ws, 0, patch_hdr, sizeof(patch_hdr)) != 0) {
        ERROR("wstream full!\n");
    }

    *patchsize  = wstream_length_get(&ws);
    *patch      = final_patch;

    summary.block_cnt = blk_cnt;
    summary.block_len  = blk_len;

    summary.patch_size = *patchsize;

OUT:
    FREE(patch4blk);
    FREE(zipped_patch4blk);

    topo_sorting_destroy(&topo_sorting);

    return rc;
}

static int backup_map_create(bkup_map_t *bkup_map, uint8_t *backup_buf, const size_t backup_n, uint16_t blk_len)
{
    uint16_t i = 0;
    uint16_t *cache = NULL;

    memset(bkup_map, 0, sizeof(bkup_map_t));

    if ((cache = malloc(backup_n * sizeof(uint16_t))) == NULL) {
        return -1;
    }

    for (i = 0; i < backup_n; ++i) {
        cache[i] = (uint16_t)-1;
    }

    bkup_map->buf           = backup_buf;
    bkup_map->blk_len       = blk_len;
    bkup_map->cache         = cache;
    bkup_map->backup_n      = backup_n;
    bkup_map->cursor        = 0;

    return 0;
}

static int backup_map_destroy(bkup_map_t *bkup_map)
{
    if (bkup_map->cache) {
        free(bkup_map->cache);
    }
    return 0;
}

static int backup_map_add(bkup_map_t *bkup_map, uint16_t blk_idx, uint8_t *backup_ed_addr)
{
    uint16_t slot = bkup_map->cursor;
    uint16_t blk_len = bkup_map->blk_len;
    uint8_t *backup_ee_addr = THE_BUF(bkup_map->buf, slot, bkup_map->blk_len);

    memcpy(backup_ee_addr, backup_ed_addr, blk_len);

    bkup_map->cache[slot] = blk_idx;
    bkup_map->cursor = (slot + 1) % bkup_map->backup_n;
    return 0;
}

static uint8_t *backup_map_query(bkup_map_t *bkup_map, uint16_t blk_idx)
{
    uint16_t i = 0;

    for (i = 0; i < bkup_map->backup_n; ++i) {
        if (bkup_map->cache[i] == blk_idx) {
            return THE_BUF(bkup_map->buf, i, bkup_map->blk_len);
        }
    }

    return NULL;
}

static int patch_test(uint8_t *old, size_t oldsize, uint8_t *new, size_t newsize, size_t the_blk_len, uint8_t *patch, size_t patchsize)
{
    int rc = 0, i = 0;
    uint8_t *the_new = NULL, *the_old = NULL;

    uint8_t *backup = NULL;
    const uint16_t backup_n = 3;
    bkup_map_t bkup_map;
    uint8_t *backup_buf;

    uint8_t *patch4blk = NULL;
    uint16_t blk_len, blk_cnt;
    uint16_t unzipped_len, zipped_len;
    size_t the_unzipped_len;
    size_t the_zipped_len;

    /*
      format of patch:
      patch_hdr + (patch4blk_hdr + zipped(patch4blk)) * N

      patch_hdr:
        0   2   length of one block
        4   2   total count of blocks in the patch

      patch4blk_hdr:
        4   2   original length of the patch
        8   2   zipped length of the patch
     */
    blk_len     = offtin_u16(patch);
    patch      += sizeof(uint16_t);
    blk_cnt     = offtin_u16(patch);
    patch      += sizeof(uint16_t);

    if (blk_len != the_blk_len) {
        ERROR("wrong patch!\n");
    }

    if ((the_new = malloc(ALIGN_UP(newsize, the_blk_len))) == NULL) {
        ERROR("malloc failed\n");
    }

    if ((the_old = malloc(ALIGN_UP(oldsize, the_blk_len))) == NULL) {
        ERROR("malloc failed\n");
    }

    memcpy(the_new,     old,    MIN(oldsize, newsize));
    memcpy(the_old,     old,    oldsize);

    if ((patch4blk = malloc(FOLD_N(blk_len, 3))) == NULL) {
        ERROR("malloc failed\n");
    }

    /* simulation of ota partition for backup */
    if ((backup = malloc(FOLD_N(blk_len, backup_n))) == NULL) {
        ERROR("malloc failed\n");
    }

    if (backup_map_create(&bkup_map, backup, backup_n, blk_len) != 0) {
        ERROR("backup_map_create failed\n");
    }

    while (blk_cnt--) {
        unzipped_len        = offtin_u16(patch);
        patch              += sizeof(uint16_t);
        zipped_len          = offtin_u16(patch);
        patch              += sizeof(uint16_t);

        the_unzipped_len    = unzipped_len;
        the_zipped_len      = zipped_len;
        if (lzma_uncompress(patch4blk, &the_unzipped_len, patch, &the_zipped_len) != LZMA_ERR_OK) {
            ERROR("uncompress failed");
        }

        patch              += zipped_len;

        /*
         format of patch for one block:
             0          2   I block index of the patch
             2          2   N count of cmd
             4          2   C sizeof(control block)
             6          2   D sizeof(diff block)

             6          C   control block
             6 + C      D   diff block
             6 + C + D  ?   extra block
         */

        /* control block:
            with a leading 8bit domain, indicating the cmd type, 0 for DIFF, 1 for EXTRA

            if current is a CMD_COPY(X, Y, Z) cmd:
                X is 0(COPY)
                locate old to Z, copy Y bytes from oldfile

            if current is a CMD_DIFF(X, Y, Z) cmd:
                X is 1(DIFF)
                locate old to Z, add Y bytes from oldfile to Y bytes from the diff block

            if current is a CMD_EXTRA(X, Y) cmd:
                X is 2(EXTRA)
                copy Y bytes from the extra block
         */
        uint8_t X;
        uint32_t Z;
        uint16_t I, N, Y, C, D;
        uint8_t *ctrl_blk, *diff_blk, *extra_blk;
        uint16_t cursor = 0, size2cmp = 0;
        uint16_t old_idx, old_offset;

        I           = offtin_u16(patch4blk + FOLD_N(sizeof(uint16_t), 0));
        N           = offtin_u16(patch4blk + FOLD_N(sizeof(uint16_t), 1));
        C           = offtin_u16(patch4blk + FOLD_N(sizeof(uint16_t), 2));
        D           = offtin_u16(patch4blk + FOLD_N(sizeof(uint16_t), 3));

        ctrl_blk    = patch4blk + FOLD_N(sizeof(uint16_t), 4);
        diff_blk    = patch4blk + FOLD_N(sizeof(uint16_t), 4) + C;
        extra_blk   = patch4blk + FOLD_N(sizeof(uint16_t), 4) + C + D;

        memset(THE_BUF(the_new, I, blk_len), 0, blk_len);

        /* backup the old block I first */
        backup_map_add(&bkup_map, I, THE_BUF(the_old, I, blk_len));

        while (N--) {
            X               = *(uint8_t *)ctrl_blk;
            ctrl_blk       += 1;

            if (X == CMD_COPY || X == CMD_DIFF) { // a COPY or DIFF cmd
                Y           = offtin_u16(ctrl_blk);
                ctrl_blk   += sizeof(uint16_t);

                Z           = offtin_u32(ctrl_blk);
                ctrl_blk   += sizeof(uint32_t);

                old_idx     = BLK_IDX(Z, blk_len);
                old_offset  = Z - BLK_LOWER_BOUND(old_idx, blk_len);

                /* sanity check */
                if (BLK_IDX(cursor + Y - 1, blk_len) != BLK_IDX(cursor, blk_len)) {
                    ERROR("wrong patch!");
                }

                backup_buf  = backup_map_query(&bkup_map, old_idx);
                if (!backup_buf) {
                    memcpy(THE_BUF(the_new, I, blk_len) + cursor, the_old + Z, Y);
                } else {
                    memcpy(THE_BUF(the_new, I, blk_len) + cursor, backup_buf + old_offset, Y);
                }

                if (X == CMD_DIFF) {
                    for (i = 0; i < Y; ++i) {
                        (THE_BUF(the_new, I, blk_len) + cursor)[i] += *(int8_t *)(diff_blk + i);
                    }
                    diff_blk += Y;
                }
            } else  if (X == CMD_EXTRA){  // a EXTRA cmd
                Y           = offtin_u16(ctrl_blk);
                ctrl_blk   += sizeof(uint16_t);

                /* sanity check */
                if (BLK_IDX(cursor + Y - 1, blk_len) != BLK_IDX(cursor, blk_len)) {
                    ERROR("wrong patch!");
                }

                memcpy(THE_BUF(the_new, I, blk_len) + cursor, extra_blk, Y);
                extra_blk  += Y;
            } else {
                ERROR("wrong patch!");
            }

            cursor         += Y;
            size2cmp       += Y;

            /* sanity check */
            if (cursor > blk_len) {
                ERROR("wrong patch!");
            }
        }

        /* simulation of earse of the flash and write of the new firmware block */
        if (I < BLK_CNT(oldsize, blk_len)) {
            memcpy(THE_BUF(the_old, I, blk_len), THE_BUF(the_new, I, blk_len), size2cmp);
        }

        if (memcmp(THE_BUF(the_new, I, blk_len), THE_BUF(new, I, blk_len), size2cmp) != 0) {
            ERROR("wrong patch!");
        }
    }

    if (memcmp(the_new, new, newsize) != 0) {
        ERROR("wrong patch!");
    }

OUT:
    FREE(the_new);
    FREE(the_old);
    FREE(patch4blk);
    FREE(backup);

    backup_map_destroy(&bkup_map);

    return rc;
}

int ota_diff(uint8_t *old, size_t oldsize, uint8_t *new, size_t newsize, size_t blk_len, int is_safe_ignored, int is_verbose, uint8_t **patch, size_t *patchsize)
{
    int rc = 0;
    graph_t graph;

    uint8_t *bs_patch = NULL;
    size_t bs_patchsize;
    uint8_t *diff_blk, *extra_blk;

    /* build a graph */
    if (graph_create(&graph, BLK_CNT(newsize, blk_len)) != 0) {
        ERROR("graph build failed!");
    }

    printf("RUN bsdiff algorithm ...\n");
    /* the bsdiff algorithm */
    if (bsdiff(old, oldsize, new, newsize, &bs_patch, &bs_patchsize) != 0) {
        ERROR("generate bspatch failed!");
    }

    /* some malloc */
    if (blk_info_create(oldsize, newsize, blk_len) != 0) {
        ERROR("block info create failed!");
    }

    /* divide the bsdiff patch into cmds by block */
    printf("PARSING bspatch ...\n");
    if (bspatch_parse(oldsize, newsize, blk_len, bs_patch, bs_patchsize, &diff_blk, &extra_blk) != 0) {
        ERROR("bspatch parse failed!");
    }

    /* check whether cmds of each block can assemble an integral block */
    printf("VERIFY cmd integrity ...\n");
    if (blk_cmd_verify(oldsize, newsize, blk_len) != 0) {
        ERROR("blk cmd check failed!");
    }

    /* check whether cmds can assemble each blocks correctly and which block[s] are safe */
    /* safe means all the same after do the patch, that means nothing should patched to the old block */
    printf("SEARCHING safe block ...\n");
    if (old_blk_safe_refresh(old, oldsize, new, newsize, blk_len, diff_blk, extra_blk) != 0) {
        ERROR("old blk safe refresh failed!");
    }

    /* make relied relationship simple */
    printf("BUILD dependency graph ...\n");
    if (relied_simplify(oldsize, blk_len, &graph) != 0) {
        ERROR("relied simplify failed!");
    }

    printf("ANALYSING dependency graph ...\n");
    if (analysing_dependency(&graph)) {
        ERROR("analysing_dependency failed!");
    }

    /* verbose*/
    if (is_verbose) {
        verbose_print(&graph, oldsize, newsize, blk_len);
    }

    /* make the real patch */
    printf("MAKING final patch ...\n");
    if (make_patch(&graph, oldsize, newsize, blk_len, diff_blk, extra_blk, is_safe_ignored, patch, patchsize) != 0) {
        ERROR("make patch failed!");
    }

    /* test the patch if is correct */
    printf("\nTESTING final patch ...\n");
    if (patch_test(old, oldsize, new, newsize, blk_len, *patch, *patchsize) != 0) {
        ERROR("patch test failed!");
    }

    printf("DONE!\n");
    if (is_verbose) {
        summary_print();
    }

OUT:
    FREE(bs_patch);

    graph_destroy(&graph);
    blk_info_destroy(oldsize, newsize, blk_len);

    return rc;
}

