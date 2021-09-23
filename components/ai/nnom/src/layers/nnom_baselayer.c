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
#include "layers/nnom_baselayer.h"

// this layer copys the input to the output

nnom_layer_t *baselayer_s(const nnom_layer_config_t * config)
{
	nnom_layer_t *layer = BaseLayer();
	if(layer)
		layer->config = (void*) config;
	return layer;
}

nnom_layer_t *BaseLayer()
{
	nnom_io_layer_t *layer;
	nnom_layer_io_t *in, *out;

	// apply a block memory for all the sub handles.
	size_t mem_size = sizeof(nnom_io_layer_t) + sizeof(nnom_layer_io_t) * 2;
	layer = nnom_mem(mem_size);
	if (layer == NULL)
		return NULL;

	// distribut the memory to sub handles.
	in = (void *)((uint8_t*)layer + sizeof(nnom_io_layer_t));
	out = (void *)((uint8_t*)in + sizeof(nnom_layer_io_t));

	// set type in layer parent
	layer->super.type = NNOM_BASE;
	layer->super.run = default_run;
	layer->super.build = default_build;
	// set buf state
	in->type = NNOM_TENSOR_BUF_TEMP;
	out->type = NNOM_TENSOR_BUF_NULL;
	// put in & out on the layer.
	layer->super.in = io_init(layer, in);
	layer->super.out = io_init(layer, out);

	return (nnom_layer_t *)layer;
}

// this is call while output shape is not defined.
// this will set the output shape same as input shape, and it set only the primary IO
// this cannot be used as first layer, of course...
nnom_status_t default_build(nnom_layer_t *layer)
{
	// get the last layer's output as input shape
	layer->in->tensor = layer->in->hook.io->tensor;
	// output tensor
	// 1. allocate a new tensor for output
	// 2. set the same dim, qfmt to the new tensor.
	layer->out->tensor = new_tensor(NNOM_QTYPE_PER_TENSOR,layer->in->tensor->num_dim, tensor_get_num_channel(layer->in->tensor));
	tensor_cpy_attr(layer->out->tensor, layer->in->tensor);
	
	// see if the activation will change the q format
	if(layer->actail) 
		layer->out->tensor->q_dec[0] = act_get_dec_bit(layer->actail->type, layer->out->tensor->q_dec[0]);

	// now this build has passed the input tensors (shapes, formats) to the new tensors. 
	return NN_SUCCESS;
}

// simply copy input to output
nnom_status_t default_run(nnom_layer_t *layer)
{
	if(layer->out->type != NNOM_TENSOR_BUF_NULL)
    {
		nnom_memcpy(layer->out->tensor->p_data, layer->in->tensor->p_data, tensor_size_byte(layer->in->tensor)); 
    }
	return NN_SUCCESS;
}
