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
#include "layers/nnom_dense.h"

#ifdef NNOM_USING_CMSIS_NN
#include "arm_math.h"
#include "arm_nnfunctions.h"
#endif

nnom_layer_t *dense_s(const nnom_dense_config_t *config)
{
	nnom_dense_layer_t *layer;
	nnom_buf_t *comp;
	nnom_layer_io_t *in, *out;

	// apply a block memory for all the sub handles.
	size_t mem_size = sizeof(nnom_dense_layer_t) + sizeof(nnom_layer_io_t) * 2 + sizeof(nnom_buf_t);
	layer = nnom_mem(mem_size);
	if (layer == NULL)
		return NULL;

	// distribut the memory to sub handles.
	in = (void *)((uint8_t*)layer + sizeof(nnom_dense_layer_t));
	out = (void *)((uint8_t*)in + sizeof(nnom_layer_io_t));
	comp = (void *)((uint8_t*)out + sizeof(nnom_layer_io_t));

	// set type in layer parent
	layer->super.type = NNOM_DENSE;
	// set buf state
	in->type = NNOM_TENSOR_BUF_TEMP;
	out->type = NNOM_TENSOR_BUF_TEMP;
	comp->type = NNOM_TENSOR_BUF_TEMP;
	// put in & out on the layer.
	layer->super.in = io_init(layer, in);
	layer->super.out = io_init(layer, out);
	layer->super.comp = comp;
	// set run and outshape methods
	layer->super.run = dense_run;
	layer->super.build = dense_build;
	layer->super.free = dense_free;

	// set parameters
	layer->output_unit = tensor_get_num_channel(config->weight); 
	layer->bias = config->bias;
	layer->weight = config->weight;
	// set shifts
	layer->output_rshift = (nnom_qformat_param_t *)config->output_shift;
	layer->bias_lshift = (nnom_qformat_param_t *)config->bias_shift;
	// set config
	layer->super.config = (void*) config;

	return (nnom_layer_t *)layer;
}

nnom_layer_t *Dense(size_t output_unit, const nnom_weight_t *w, const nnom_bias_t *b)
{
	nnom_dense_layer_t *layer;
	nnom_buf_t *comp;
	nnom_layer_io_t *in, *out;

	// apply a block memory for all the sub handles.
	size_t mem_size = sizeof(nnom_dense_layer_t) + sizeof(nnom_layer_io_t) * 2 + sizeof(nnom_buf_t);
	layer = nnom_mem(mem_size);
	if (layer == NULL)
		return NULL;

	// distribut the memory to sub handles.
	in = (void *)((uint8_t*)layer + sizeof(nnom_dense_layer_t));
	out = (void *)((uint8_t*)in + sizeof(nnom_layer_io_t));
	comp = (void *)((uint8_t*)out + sizeof(nnom_layer_io_t));

	// set type in layer parent
	layer->super.type = NNOM_DENSE;
	// set buf state
	in->type = NNOM_TENSOR_BUF_TEMP;
	out->type = NNOM_TENSOR_BUF_TEMP;
	comp->type = NNOM_TENSOR_BUF_TEMP;
	// put in & out on the layer.
	layer->super.in = io_init(layer, in);
	layer->super.out = io_init(layer, out);
	layer->super.comp = comp;
	// set run and outshape methods
	layer->super.run = dense_run;
	layer->super.build = dense_build;

	// set parameters
	layer->output_unit = output_unit; // this is no longer needed. the information is contained in the weight tensor. 

	layer->weight = new_tensor(NNOM_QTYPE_PER_TENSOR, 2, output_unit);
	layer->bias = new_tensor(NNOM_QTYPE_PER_TENSOR, 1, output_unit);

	// configure weight tensor manually to support new tensor-based backends. 
	// needs to be very careful
	{
		// config weight 
		nnom_shape_data_t dim[2] = {0, output_unit}; // the first dim doesnt matter here. will be file in later. 
		*(layer->weight->q_offset) = 0;			// we have no support of offset here
		*(layer->weight->q_dec) = 0;		// this is not even correct
		layer->weight->p_data = (void*)w->p_value;
		layer->weight->bitwidth = 8;
		layer->weight->qtype = NNOM_QTYPE_PER_TENSOR;
		nnom_memcpy(layer->weight->dim, dim, layer->weight->num_dim * sizeof(nnom_shape_data_t));

		// config bias 
		dim[0] = output_unit;
		*(layer->bias->q_offset) = 0;			// we have no support of offset here
		*(layer->bias->q_dec) = 0;		// this is not even correct
		layer->bias->p_data = (void*)b->p_value;
		layer->bias->bitwidth = 8;
		layer->weight->qtype = NNOM_QTYPE_PER_TENSOR;
		nnom_memcpy(layer->bias->dim, dim, layer->bias->num_dim * sizeof(nnom_shape_data_t));
	}

	// set output shifts
	layer->output_rshift = (nnom_qformat_param_t *)&w->shift;
	layer->bias_lshift = (nnom_qformat_param_t *)&b->shift;

	return (nnom_layer_t *)layer;
}

nnom_status_t dense_build(nnom_layer_t *layer)
{
	nnom_dense_layer_t *cl = (nnom_dense_layer_t *)layer;

	// get the tensor from last layer's output
	layer->in->tensor = layer->in->hook.io->tensor;

	// create new tensor for output
	layer->out->tensor = new_tensor(NNOM_QTYPE_PER_TENSOR, 1, tensor_get_num_channel(layer->in->tensor));
	// setup new tensor
	nnom_shape_data_t dim[1] = {cl->output_unit};
	tensor_set_attr(layer->out->tensor, cl->weight->q_dec, cl->weight->q_offset, dim, 1, 8); // test, this is not correct

	// calculate the output tensor q format, only support per tensor quantise now
	layer->out->tensor->q_dec[0] = layer->in->tensor->q_dec[0] + cl->weight->q_dec[0] - cl->output_rshift[0];
	// see if the activation will change the q format
	if(layer->actail) 
		layer->out->tensor->q_dec[0] = act_get_dec_bit(layer->actail->type, layer->out->tensor->q_dec[0]);
	
	// vec_buffer size: dim_vec (*2, q7->q15) ? I am not sure this is right
	layer->comp->size = tensor_size(layer->in->tensor)*2;

	// computational cost: In * out
	layer->stat.macc = tensor_size(layer->in->tensor) * tensor_size(layer->out->tensor);
	return NN_SUCCESS;
}

nnom_status_t dense_free(nnom_layer_t *layer)
{
	// free weight and bias tensor when we are not initialised from structured configuration. 
	if(!layer->config)
	{
		nnom_dense_layer_t* cl = (nnom_dense_layer_t*)layer;
		delete_tensor(cl->weight);
		delete_tensor(cl->bias);
	}

	return NN_SUCCESS;
}

nnom_status_t dense_run(nnom_layer_t *layer)
{
	nnom_status_t result = NN_SUCCESS;
	nnom_dense_layer_t *cl = (nnom_dense_layer_t *)(layer);
	nnom_qformat_param_t bias_shift = cl->bias_lshift[0];			// this is not correct but a temporary fix solution for backward compatibility.
	nnom_qformat_param_t output_shift = cl->output_rshift[0];


#if !(DENSE_WEIGHT_OPT)
	#ifdef NNOM_USING_CMSIS_NN
		result = (nnom_status_t)arm_fully_connected_q7(
	#else
		local_fully_connected_q7(
	#endif
#else
	#ifdef NNOM_USING_CMSIS_NN
		result = (nnom_status_t)arm_fully_connected_q7_opt(
	#else
		local_fully_connected_q7_opt(
	#endif
#endif
			layer->in->tensor->p_data,
			cl->weight->p_data,
			tensor_size(layer->in->tensor), layer->out->tensor->dim[0],
			bias_shift, output_shift,
			cl->bias->p_data,
			layer->out->tensor->p_data, (q15_t *)(layer->comp->mem->blk));
	return result;
}

