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
#include "layers/nnom_cropping.h"

nnom_layer_t * cropping_s(const nnom_cropping_config_t *config)
{
	nnom_layer_t *layer = Cropping(config->pad);
	if(layer)
		layer->config = (void*) config;
	return layer;
}

// Cropping layer
nnom_layer_t *Cropping(nnom_border_t pad)
{
	nnom_layer_t *layer;
	// most setting are the same as zero padding
	layer = ZeroPadding(pad);
	
	// now change to cropping
	layer->type = NNOM_CROPPING;
	layer->run = cropping_run;
	layer->build = cropping_build;

	return layer;
}

nnom_status_t cropping_build(nnom_layer_t* layer)
{
	nnom_cropping_layer_t *cl = (nnom_cropping_layer_t *)layer;

	// get the tensor from last layer's output
	layer->in->tensor = layer->in->hook.io->tensor;

	// create new tensor for output
	layer->out->tensor = new_tensor(NNOM_QTYPE_PER_TENSOR, layer->in->tensor->num_dim, tensor_get_num_channel(layer->in->tensor));
	// copy then change later. 
	tensor_cpy_attr(layer->out->tensor, layer->in->tensor);
	
	// output shape
	if(layer->in->tensor->dim[1] <= (cl->pad.left + cl->pad.right) || 
		layer->in->tensor->dim[0] <= (cl->pad.top + cl->pad.bottom))
		return NN_ARGUMENT_ERROR;
	
	layer->out->tensor->dim[0] = layer->in->tensor->dim[0] - (cl->pad.top + cl->pad.bottom);
	layer->out->tensor->dim[1] = layer->in->tensor->dim[1] - (cl->pad.left + cl->pad.right);
	layer->out->tensor->dim[2] = layer->in->tensor->dim[2];
	return NN_SUCCESS;
}


nnom_status_t cropping_run(nnom_layer_t * layer)
{
	nnom_cropping_layer_t *cl = (nnom_cropping_layer_t*)layer;
	
#ifdef NNOM_USING_CHW
	local_cropping_CHW_q7(
#else
	local_cropping_HWC_q7(
#endif	
						layer->in->tensor->p_data, 
						layer->in->tensor->dim[1], layer->in->tensor->dim[0], layer->in->tensor->dim[2],
						cl->pad.top,
						cl->pad.bottom,
						cl->pad.left,
						cl->pad.right,
						layer->out->tensor->p_data,
						layer->out->tensor->dim[1], layer->out->tensor->dim[0]);

	return NN_SUCCESS;
}
