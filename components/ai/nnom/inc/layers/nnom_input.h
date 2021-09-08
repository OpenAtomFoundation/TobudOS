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

#ifndef __NNOM_INPUT_H__
#define __NNOM_INPUT_H__

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

// IO layer
typedef struct _nnom_io_layer
{
	nnom_layer_t super;
	nnom_3d_shape_t shape;
	nnom_qformat_param_t dec_bit;
	void *buf; //input or output
} nnom_io_layer_t;

typedef struct _nnom_io_config_t
{
	nnom_layer_config_t super;
	nnom_tensor_t *tensor; 
}nnom_io_config_t;

// method
nnom_status_t input_build(nnom_layer_t *layer);
nnom_status_t input_run(nnom_layer_t *layer);

// API
nnom_layer_t *input_s(const nnom_io_config_t* config);
nnom_layer_t *Input(nnom_3d_shape_t input_shape, void *p_buf);


#ifdef __cplusplus
}
#endif

#endif /* __NNOM_INPUT_H__ */
