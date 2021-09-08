/*
 * Copyright (c) 2018-2020
 * Jianjia Ma
 * majianjia@live.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-07-23     Jianjia Ma   The first version
 */

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "nnom.h"
#include "nnom_local.h"
#include "nnom_layers.h"
#include "layers/nnom_lambda.h"

nnom_layer_t *lambda_s(const nnom_lambda_config_t * config)
{
	nnom_lambda_layer_t *cl = (nnom_lambda_layer_t *)Lambda(
		config->run_func_name, 
		config->build_func_name,
		config->free_func_name,
		config->parameters);
	if(cl)
		cl->super.config = (void*) config;
	return (nnom_layer_t *)cl;
}

// TODO: extended to multiple IO layer
nnom_layer_t *Lambda(nnom_status_t (*run)(nnom_layer_t *),
					 nnom_status_t (*build)(nnom_layer_t *),
					 nnom_status_t (*free)(nnom_layer_t *),
					 void *parameters)
{
	nnom_lambda_layer_t *layer;
	nnom_layer_io_t *in, *out;

	// apply a block memory for all the sub handles.
	size_t mem_size = sizeof(nnom_io_layer_t) + sizeof(nnom_layer_io_t) * 2;
	layer = nnom_mem(mem_size);
	if (layer == NULL)
		return NULL;

	// distribut the memory to sub handles.
	in = (void *)((uint8_t*)layer + sizeof(nnom_lambda_layer_t));
	out = (void *)((uint8_t*)in + sizeof(nnom_layer_io_t));

	// set buf type.
	in->type = NNOM_TENSOR_BUF_TEMP;
	out->type = NNOM_TENSOR_BUF_TEMP;

	// set io modules to the layer
	layer->super.in = io_init(layer, in);
	layer->super.out = io_init(layer, out);
	// layer type
	layer->super.type = NNOM_LAMBDA;

	// user parameters
	layer->parameters = parameters;

	// free method
	layer->super.free = free;

	// output shape method. pass NULL in will use the default outshape method, which set the output shape same as input shape.
	if (build == NULL)
		layer->super.build = default_build;
	else
		layer->super.build = build;
	// run method. default_run() will simply copy data from input tensor to output tensor. 
	if(run == NULL)
		layer->super.run = default_run;
	else
		layer->super.run = run;

	return (nnom_layer_t *)layer;
}
