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

#ifndef __NNOM_DW_CONV2D_H__
#define __NNOM_DW_CONV2D_H__

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

// method
nnom_status_t dw_conv2d_build(nnom_layer_t *layer);
nnom_status_t dw_conv2d_run(nnom_layer_t *layer);

//API
nnom_layer_t *dw_conv2d_s(const nnom_conv2d_config_t *config);
nnom_layer_t *DW_Conv2D(uint32_t multiplier, nnom_3d_shape_t k, nnom_3d_shape_t s, nnom_3d_shape_t d, nnom_padding_t pad_type,
						const nnom_weight_t *w, const nnom_bias_t *b);

#ifdef __cplusplus
}
#endif

#endif /* __NNOM_DW_CONV2D_H__ */
