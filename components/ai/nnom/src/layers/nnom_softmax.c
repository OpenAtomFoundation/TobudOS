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
#include "layers/nnom_softmax.h"

#ifdef NNOM_USING_CMSIS_NN
#include "arm_math.h"
#include "arm_nnfunctions.h"
#endif

nnom_layer_t *softmax_s(const nnom_softmax_config_t * config)
{
	nnom_layer_t * layer = Softmax();
	if(layer)
		layer->config = (void*) config;
	return layer;
}

nnom_layer_t *Softmax(void)
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
	layer->type = NNOM_SOFTMAX;
	layer->run = softmax_run;
	layer->build = softmax_build;
	// set buf state
	in->type = NNOM_TENSOR_BUF_TEMP;
	out->type = NNOM_TENSOR_BUF_TEMP;
	// put in & out on the layer.
	layer->in = io_init(layer, in);
	layer->out = io_init(layer, out);

	return layer;
}

nnom_status_t softmax_build(nnom_layer_t *layer)
{
	// get the last layer's output as input shape
	layer->in->tensor = layer->in->hook.io->tensor;
	// output tensor
	layer->out->tensor = new_tensor(NNOM_QTYPE_PER_TENSOR, layer->in->tensor->num_dim, tensor_get_num_channel(layer->in->tensor));
	tensor_cpy_attr(layer->out->tensor, layer->in->tensor);
	// softmax has fixed output dec bit
	layer->out->tensor->q_dec[0] = 7;
	return NN_SUCCESS;
}

nnom_status_t softmax_run(nnom_layer_t *layer)
{
	// looks like the new version cause accuracy drop quite a lot. 
//	#ifdef NNOM_USING_CMSIS_NN
//	// temporary fixed for mutiple dimension input. 
//	arm_softmax_q7(layer->in->tensor->p_data, tensor_size(layer->out->tensor), layer->out->tensor->p_data);
//	#else
	local_softmax_q7(layer->in->tensor->p_data, tensor_size(layer->out->tensor), layer->out->tensor->p_data);
	//#endif
	return NN_SUCCESS;
}
