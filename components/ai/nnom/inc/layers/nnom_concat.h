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

#ifndef __NNOM_CONCAT_H__
#define __NNOM_CONCAT_H__

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

// concatenate layer
typedef struct _nnom_concat_layer
{
	nnom_layer_t super;
	int8_t axis;
} nnom_concat_layer_t;

typedef struct _nnom_concat_config_t
{
	nnom_layer_config_t super;
	int8_t axis;
} nnom_concat_config_t;

// method
nnom_status_t concat_build(nnom_layer_t *layer);
nnom_status_t concat_run(nnom_layer_t *layer);

// API
nnom_layer_t *concat_s(const nnom_concat_config_t *config);
nnom_layer_t *Concat(int8_t axis);


#ifdef __cplusplus
}
#endif

#endif /* __NNOM_CONCAT_H__ */
