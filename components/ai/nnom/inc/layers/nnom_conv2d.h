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

#ifndef __NNOM_CONV2D_H__
#define __NNOM_CONV2D_H__

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

// child layers parameters
typedef struct _nnom_conv2d_layer_t
{
	nnom_layer_t super;
	nnom_3d_shape_t kernel;
	nnom_3d_shape_t stride;
	nnom_3d_shape_t pad;
	nnom_3d_shape_t dilation;
	nnom_padding_t padding_type;
	uint32_t filter_mult; 							// filter size (for conv) or multilplier (for depthwise)

	nnom_tensor_t *weight; 
	nnom_tensor_t *bias;

	// test
	nnom_qformat_param_t * output_rshift;			
	nnom_qformat_param_t * bias_lshift;
} nnom_conv2d_layer_t;

// a machine interface for configuration
typedef struct _nnom_conv2d_config_t
{
	nnom_layer_config_t super;
	nnom_qtype_t qtype; 	//quantisation type(per channel or per layer)
	nnom_tensor_t *weight;
	nnom_tensor_t *bias;
	nnom_qformat_param_t *output_shift;   
	nnom_qformat_param_t *bias_shift;   
	uint32_t filter_size;  
	int8_t kernel_size[2];
	int8_t stride_size[2];
	int8_t padding_size[2];
	int8_t dilation_size[2];
	nnom_padding_t padding_type;
} nnom_conv2d_config_t;

// method
nnom_status_t conv2d_run(nnom_layer_t *layer);
nnom_status_t conv2d_build(nnom_layer_t *layer);
nnom_status_t conv2d_free(nnom_layer_t *layer);

// utils
uint32_t conv_output_length(uint32_t input_length, uint32_t filter_size, nnom_padding_t padding, uint32_t stride, uint32_t dilation);

// API
nnom_layer_t *conv2d_s(const nnom_conv2d_config_t *config);
nnom_layer_t *Conv2D(uint32_t filters, nnom_3d_shape_t k, nnom_3d_shape_t s, nnom_3d_shape_t d,  nnom_padding_t pad_type,
					 const nnom_weight_t *w, const nnom_bias_t *b);

#ifdef __cplusplus
}
#endif

#endif /* __NNOM_CONV2D_H__ */
