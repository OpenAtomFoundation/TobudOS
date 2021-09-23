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

#ifndef __NNOM_UPSAMPLE_H__
#define __NNOM_UPSAMPLE_H__

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

// Up Sampling layer (UnPooling)
typedef struct _nnom_upsample_layer_t
{
	nnom_layer_t super;
	nnom_3d_shape_t kernel;
} nnom_upsample_layer_t;

typedef struct _nnom_upsample_config_t
{
	nnom_layer_config_t super;
	nnom_shape_data_t kernel[2];
} nnom_upsample_config_t;

// API
nnom_layer_t *upsample_s(const nnom_upsample_config_t *config);
nnom_layer_t *UpSample(nnom_3d_shape_t kernel);

// Methods
nnom_status_t upsample_build(nnom_layer_t *layer);
nnom_status_t upsample_run(nnom_layer_t *layer);

#ifdef __cplusplus
}
#endif

#endif /* __NNOM_UPSAMPLE_H__ */
