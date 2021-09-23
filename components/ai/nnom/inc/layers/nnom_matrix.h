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

#ifndef __NNOM_MATRIX_H__
#define __NNOM_MATRIX_H__

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

// the maximum input layer hooked to this layer
#define MAX_INPUT_LAYER 8

// matrix layer
typedef struct _nnom_matrix_layer_t
{
	nnom_layer_t super;
	int16_t oshift;		// output right shift
} nnom_matrix_layer_t;

typedef struct _nnom_matrix_config_t
{
	nnom_layer_config_t super;
	int16_t output_shift;		// output right shift
} nnom_matrix_config_t;

// methods
nnom_layer_t* _same_shape_matrix_layer(void);
nnom_status_t add_run(nnom_layer_t *layer);
nnom_status_t sub_run(nnom_layer_t *layer);
nnom_status_t mult_run(nnom_layer_t *layer);

// API
nnom_layer_t *add_s(const nnom_matrix_config_t * config);
nnom_layer_t *sub_s(const nnom_matrix_config_t * config);
nnom_layer_t *mult_s(const nnom_matrix_config_t * config);
nnom_layer_t *Add(int16_t oshift);
nnom_layer_t *Sub(int16_t oshift);
nnom_layer_t *Mult(int16_t oshift);

#ifdef __cplusplus
}
#endif

#endif /* __NNOM_MATRIX_H__ */
