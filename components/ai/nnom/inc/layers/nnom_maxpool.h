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

#ifndef __NNOM_MAXPOOL_H__
#define __NNOM_MAXPOOL_H__

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

// Max Pooling
typedef struct _nnom_maxpool_layer_t
{
	nnom_layer_t super;
	nnom_3d_shape_t kernel;
	nnom_3d_shape_t stride;
	nnom_3d_shape_t pad;
	nnom_padding_t padding_type;
	int16_t output_shift;			// reserve
} nnom_maxpool_layer_t;

// a machine interface for configuration
typedef struct _nnom_pool_config_t
{
	nnom_layer_config_t super;
	nnom_padding_t padding_type;
	int16_t output_shift;
	int8_t kernel_size[2];
	int8_t stride_size[2];
	int8_t num_dim;
} nnom_pool_config_t;

// method
nnom_status_t maxpool_build(nnom_layer_t *layer);
nnom_status_t maxpool_run(nnom_layer_t *layer);

// API
nnom_layer_t *maxpool_s(const nnom_pool_config_t * config);
nnom_layer_t *MaxPool(nnom_3d_shape_t k, nnom_3d_shape_t s, nnom_padding_t pad_type);

#ifdef __cplusplus
}
#endif

#endif /* __NNOM_MATRIX_H__ */
