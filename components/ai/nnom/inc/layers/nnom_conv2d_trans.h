/*
 * Copyright (c) 2018-2020
 * Jianjia Ma
 * majianjia@live.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-05-30     Jianjia Ma   The first version
 */

#ifndef __NNOM_DECONV2D_H__
#define __NNOM_DECONV2D_H__

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
#include "layers/nnom_conv2d.h"

// child layers parameters
typedef nnom_conv2d_layer_t nnom_conv2d_trans_layer_t;

typedef nnom_conv2d_config_t nnom_conv2d_trans_config_t;

// method
nnom_status_t conv2d_trans_run(nnom_layer_t *layer);
nnom_status_t conv2d_trans_build(nnom_layer_t *layer);

// utils
uint32_t conv_trans_output_length(uint32_t input_length, uint32_t filter_size, nnom_padding_t padding, uint32_t stride, uint32_t dilation);

// API
nnom_layer_t *conv2d_trans_s(const nnom_conv2d_config_t *config);
nnom_layer_t *Conv2DTrans(uint32_t filters, nnom_3d_shape_t k, nnom_3d_shape_t s, nnom_3d_shape_t d,  nnom_padding_t pad_type,
					 const nnom_weight_t *w, const nnom_bias_t *b);

#ifdef __cplusplus
}
#endif

#endif /* __NNOM_DECONV2D_H__ */
