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

#ifndef __NNOM_CROPPING_H__
#define __NNOM_CROPPING_H__

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

#include "layers/nnom_zero_padding.h"

// Cropping, same as zeropadding
typedef nnom_zero_padding_layer_t nnom_cropping_layer_t;

typedef nnom_zero_padding_config_t nnom_cropping_config_t;

// method
nnom_status_t cropping_build(nnom_layer_t *layer);
nnom_status_t cropping_run(nnom_layer_t *layer);

// API
nnom_layer_t * cropping_s(const nnom_cropping_config_t *config);
nnom_layer_t *Cropping(nnom_border_t pad);

#ifdef __cplusplus
}
#endif

#endif /* __NNOM_CROPPING_H__ */
