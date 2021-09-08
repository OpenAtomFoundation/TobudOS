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

#ifndef __NNOM_OUTPUT_H__
#define __NNOM_OUTPUT_H__

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

#include "layers/nnom_input.h"

// method
nnom_status_t output_build(nnom_layer_t *layer);
nnom_status_t output_run(nnom_layer_t *layer);

// API
nnom_layer_t *output_s(const nnom_io_config_t* config);
nnom_layer_t *Output(nnom_3d_shape_t output_shape, void *p_buf);

#ifdef __cplusplus
}
#endif

#endif /* __NNOM_OUTPUT_H__ */
