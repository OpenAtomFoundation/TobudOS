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
#include "layers/nnom_flatten.h"

nnom_layer_t *flatten_s(const nnom_flatten_config_t *config)
{
	nnom_layer_t *layer = Flatten();
	if(layer)
		layer->config = (void*) config;
	return layer;
}

nnom_layer_t *Flatten(void)
{
	nnom_layer_t *layer;
	nnom_layer_io_t *in, *out;

	// apply a block memory for all the sub handles.
	size_t mem_size = sizeof(nnom_layer_t) + sizeof(nnom_layer_io_t) * 2;
	layer = nnom_mem(mem_size);
	if (layer == NULL)
		return NULL;

	// distribut the memory to sub handles.
	in = (void *)((uint8_t*)layer + sizeof(nnom_layer_t));
	out = (void *)((uint8_t*)in + sizeof(nnom_layer_io_t));

	// set type in layer parent
	layer->type = NNOM_FLATTEN;
	layer->run = flatten_run;
	layer->build = flatten_build;
	// set buf state
	in->type = NNOM_TENSOR_BUF_TEMP;
	#ifdef NNOM_USING_CHW
		out->type = NNOM_TENSOR_BUF_TEMP; // test for CHW format
	#else
		out->type = NNOM_TENSOR_BUF_NULL; 
	#endif
	// put in & out on the layer.
	layer->in = io_init(layer, in);
	layer->out = io_init(layer, out);

	return layer;
}

nnom_status_t flatten_build(nnom_layer_t *layer)
{ 
	// get the tensor from last layer's output
	layer->in->tensor = layer->in->hook.io->tensor;

	// create new tensor for output
	layer->out->tensor = new_tensor(NNOM_QTYPE_PER_TENSOR, layer->in->tensor->num_dim, tensor_get_num_channel(layer->in->tensor));
	// setup new tensor
	nnom_shape_data_t dim[1] = {tensor_size(layer->in->tensor)};
	tensor_set_attr(layer->out->tensor, layer->in->tensor->q_dec, layer->in->tensor->q_offset, dim, 1, 8);

	return NN_SUCCESS;
}

nnom_status_t flatten_run(nnom_layer_t *layer)
{
	#ifdef NNOM_USING_CHW
	// CHW format must reorder to HWC for dense layer and all other 1D layer (?)
	tensor_chw2hwc_q7(layer->out->tensor, layer->in->tensor);
	#endif
	return NN_SUCCESS;
}
