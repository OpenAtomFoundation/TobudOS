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

#ifndef __NNOM_BASELAYER_H__
#define __NNOM_BASELAYER_H__

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
nnom_status_t default_build(nnom_layer_t *layer);
nnom_status_t default_run(nnom_layer_t *layer);

// API
nnom_layer_t *baselayer_s(const nnom_layer_config_t * config);
nnom_layer_t *BaseLayer(void);

#ifdef __cplusplus
}
#endif

#endif /* __NNOM_BASELAYER_H__ */
