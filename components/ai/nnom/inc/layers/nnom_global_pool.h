/*
 * Copyright (c) 2018-2020
 * Jianjia Ma
 * majianjia@live.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-05-03     Jianjia Ma   The first version
 */

#ifndef __NNOM_GLOBAL_POOL_H__
#define __NNOM_GLOBAL_POOL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "nnom.h"
#include "nnom_layers.h"
#include "nnom_local.h"
#include "nnom_tensor.h"

#include "layers/nnom_maxpool.h"

typedef struct _nnom_global_pool_config_t
{
    nnom_layer_config_t super;
    int16_t output_shift;
}nnom_global_pool_config_t;

// method
nnom_status_t global_pool_build(nnom_layer_t *layer);

// API
nnom_layer_t * global_maxpool_s(const nnom_global_pool_config_t *config);
nnom_layer_t * global_avgpool_s(const nnom_global_pool_config_t *config);
nnom_layer_t * global_sumpool_s(const nnom_global_pool_config_t *config);

nnom_layer_t *GlobalMaxPool(void);
nnom_layer_t *GlobalAvgPool(void);
nnom_layer_t *GlobalSumPool(void);


#ifdef __cplusplus
}
#endif

#endif /* __NNOM_GLOBAL_POOL_H__ */
