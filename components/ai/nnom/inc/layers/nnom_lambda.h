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

#ifndef __NNOM_LAMBDA_H__
#define __NNOM_LAMBDA_H__

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

// lambda layer
typedef struct _nnom_lambda_layer_t
{
	nnom_layer_t super;
	void *parameters;							  // parameters for lambda
} nnom_lambda_layer_t;

// lambda layer
typedef struct _nnom_lambda_config_t
{
	nnom_layer_config_t super;
	nnom_status_t (*run_func_name)(nnom_layer_t *layer);	// run method. required
	nnom_status_t (*build_func_name)(nnom_layer_t *layer);// compute output buffer shape. can be left null, will call default_build()
	nnom_status_t (*free_func_name)(nnom_layer_t *layer);	// a callback to free private resources (comp buf not included) can be left null
	void *parameters;							// parameters for lambda
} nnom_lambda_config_t;



#ifdef __cplusplus
}
#endif

#endif /* __NNOM_LAMBDA_H__ */
