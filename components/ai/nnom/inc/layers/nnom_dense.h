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

#ifndef __NNOM_DENSE_H__
#define __NNOM_DENSE_H__

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

typedef struct _nnom_dense_layer_t
{
	nnom_layer_t super;
	size_t output_unit;
	nnom_tensor_t *weight;
	nnom_tensor_t *bias;
	nnom_qformat_param_t *output_rshift;			
	nnom_qformat_param_t *bias_lshift;
} nnom_dense_layer_t;

// a machine interface for configuration
typedef struct _nnom_dense_config_t
{
	nnom_layer_config_t super;
	nnom_qtype_t qtype; 	//quantisation type(per channel or per layer)
	nnom_tensor_t *weight;
	nnom_tensor_t *bias;
	nnom_qformat_param_t *output_shift;			
	nnom_qformat_param_t *bias_shift;
} nnom_dense_config_t;

// method
nnom_status_t dense_free(nnom_layer_t *layer);
nnom_status_t dense_build(nnom_layer_t *layer);
nnom_status_t dense_run(nnom_layer_t *layer);

// API
nnom_layer_t *dense_s(const nnom_dense_config_t *config);
nnom_layer_t *Dense(size_t output_unit, const nnom_weight_t *w, const nnom_bias_t *b);

#ifdef __cplusplus
}
#endif

#endif /* __NNOM_DENSE_H__ */
