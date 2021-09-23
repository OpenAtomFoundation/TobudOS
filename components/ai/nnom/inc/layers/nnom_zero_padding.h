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

#ifndef __NNOM_ZERO_PADDING_H__
#define __NNOM_ZERO_PADDING_H__

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

typedef struct _nnom_zero_padding_config_t
{
	nnom_layer_config_t super;
	nnom_border_t pad;
} nnom_zero_padding_config_t;

// zero padding
typedef struct _nnom_zero_padding_layer_t
{
	nnom_layer_t super;
	nnom_border_t pad;
} nnom_zero_padding_layer_t;

// API
nnom_layer_t *zeropadding_s(const nnom_zero_padding_config_t* config);
nnom_layer_t *ZeroPadding(nnom_border_t pad);

// method
nnom_status_t zero_padding_build(nnom_layer_t *layer);
nnom_status_t zero_padding_run(nnom_layer_t *layer);

#ifdef __cplusplus
}
#endif

#endif /* __NNOM_ZERO_PADDING_H__ */
