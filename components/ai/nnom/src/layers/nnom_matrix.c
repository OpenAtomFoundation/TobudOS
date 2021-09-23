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
#include "layers/nnom_matrix.h"

// TODO, completely change this file to local version
#ifdef NNOM_USING_CMSIS_NN
#include "arm_math.h"
#include "arm_nnfunctions.h"
#endif

nnom_status_t matrix_build(nnom_layer_t *layer);

nnom_layer_t *add_s(const nnom_matrix_config_t * config)
{
	nnom_matrix_layer_t *cl = (nnom_matrix_layer_t *) Add(config->output_shift);
	if(cl)
		cl->super.config = (void*) config;
	return (nnom_layer_t *)cl;
}

nnom_layer_t *sub_s(const nnom_matrix_config_t * config)
{
	nnom_matrix_layer_t *cl = (nnom_matrix_layer_t *) Sub(config->output_shift);
	if(cl)
		cl->super.config = (void*) config;
	return (nnom_layer_t *)cl;
}

nnom_layer_t *mult_s(const nnom_matrix_config_t * config)
{
	nnom_matrix_layer_t *cl = (nnom_matrix_layer_t *) Mult(config->output_shift);
	if(cl)
		cl->super.config = (void*) config;
	return (nnom_layer_t *)cl;
}

nnom_layer_t *Add(int16_t oshift)
{
	nnom_matrix_layer_t *cl = (nnom_matrix_layer_t *)_same_shape_matrix_layer();
	if (cl == NULL)
		return NULL;
	// set type in layer parent
	cl->super.type = NNOM_ADD;
	cl->super.run = add_run;
	cl->oshift = oshift;
	return (nnom_layer_t *)cl;
}

nnom_layer_t *Sub(int16_t oshift)
{
	nnom_matrix_layer_t *cl = (nnom_matrix_layer_t *)_same_shape_matrix_layer();
	if (cl == NULL)
		return NULL;
	// set type in layer parent
	cl->super.type = NNOM_SUB;
	cl->super.run = sub_run;
	cl->oshift = oshift;
	return (nnom_layer_t *)cl;
}

nnom_layer_t *Mult(int16_t oshift)
{
	nnom_matrix_layer_t *cl = (nnom_matrix_layer_t *)_same_shape_matrix_layer();
	if (cl == NULL)
		return NULL;
	// set type in layer parent
	cl->super.type = NNOM_MULT;
	cl->super.run = mult_run;
	cl->oshift = oshift;
	return (nnom_layer_t *)cl;
}

// init a base layer instance with same shape 1 in 1 out. More IO can be added later
// mainly used by matrix calculation (add, mult, sub)
nnom_layer_t *_same_shape_matrix_layer()
{
	nnom_matrix_layer_t *layer;
	nnom_layer_io_t *in, *out;
	//nnom_buf_t *comp;
	size_t mem_size;

	// apply a block memory for all the sub handles.
	mem_size = sizeof(nnom_matrix_layer_t) + sizeof(nnom_layer_io_t) * 2;
	layer = nnom_mem(mem_size);
	if (layer == NULL)
		return NULL;

	// distribut the memory to sub handles.
	in = (void *)((uint8_t*)layer + sizeof(nnom_matrix_layer_t));
	out = (void *)((uint8_t*)in + sizeof(nnom_layer_io_t));
	//comp = (void *)((uint8_t*)out + sizeof(nnom_layer_io_t));

	// set type in layer parent
	layer->super.build = matrix_build;
	// set buf state
	in->type = NNOM_TENSOR_BUF_TEMP;
	out->type = NNOM_TENSOR_BUF_TEMP;
	//comp->type = NNOM_TENSOR_BUF_TEMP;
	// put in & out on the layer.
	layer->super.in = io_init(layer, in);
	layer->super.out = io_init(layer, out);
	//layer->super.comp = comp;
	return (nnom_layer_t*)layer;
}

nnom_status_t matrix_build(nnom_layer_t *layer)
{
	// get the last layer's output as input shape (if more than one)
	nnom_layer_io_t *in = layer->in;
	while(in)
	{
		in->tensor = in->hook.io->tensor;
		in = in->aux;
	}
	// output tensor
	layer->out->tensor = new_tensor(NNOM_QTYPE_PER_TENSOR,layer->in->tensor->num_dim, tensor_get_num_channel(layer->in->tensor));
	tensor_cpy_attr(layer->out->tensor, layer->in->tensor);
	
	// see if the activation will change the q format
	if(layer->actail) 
		layer->out->tensor->q_dec[0] = act_get_dec_bit(layer->actail->type, layer->out->tensor->q_dec[0]);

	// now this build has passed the input tensors (shapes, formats) to the new tensors. 
	return NN_SUCCESS;
}


nnom_status_t add_run(nnom_layer_t *layer)
{
	nnom_matrix_layer_t* cl = (nnom_matrix_layer_t*)layer;
	nnom_layer_io_t *in = layer->in;;
	size_t t_size = tensor_size(layer->out->tensor);
	int32_t oshift = cl->oshift;
	size_t num_input = nnom_io_length(layer->in);
	q7_t *input_mem_blk[MAX_INPUT_LAYER];

	// if there is only 2 matrix
	if(num_input == 2)
	{
		#ifdef NNOM_USING_CMSIS_NN
		if(oshift == 0)
			arm_add_q7(layer->in->tensor->p_data, layer->in->aux->tensor->p_data, layer->out->tensor->p_data, t_size);
		else
		#endif
			local_add_q7(layer->in->tensor->p_data, layer->in->aux->tensor->p_data, layer->out->tensor->p_data, oshift, t_size);
	}
	else
	{	
		for(int i = 0; i < num_input; i++)
		{
			input_mem_blk[i] = in->tensor->p_data;
			in = in->aux;
		}
		local_multiple_add_q7(layer->out->tensor->p_data, oshift, t_size, num_input, input_mem_blk);
	}

	return NN_SUCCESS;
}

nnom_status_t sub_run(nnom_layer_t *layer)
{
	nnom_matrix_layer_t* cl = (nnom_matrix_layer_t*)layer;
	nnom_layer_io_t *in = layer->in;
	size_t t_size = tensor_size(layer->out->tensor);
	int32_t oshift = cl->oshift;
	size_t num_input = nnom_io_length(layer->in);
	q7_t *input_mem_blk[MAX_INPUT_LAYER];

	// if there is only 2 matrix
	if(num_input == 2)
	{
		// the first 2 matrix
		#ifdef NNOM_USING_CMSIS_NN
		if(oshift == 0)
			arm_sub_q7(layer->in->tensor->p_data, layer->in->aux->tensor->p_data, layer->out->tensor->p_data, t_size);
		else
		#endif
			local_sub_q7(layer->in->tensor->p_data, layer->in->aux->tensor->p_data, layer->out->tensor->p_data, oshift, t_size);
	}
	else
	{	
		for(int i = 0; i < num_input; i++)
		{
			input_mem_blk[i] = in->tensor->p_data;
			in = in->aux;
		}
		local_multiple_sub_q7(layer->out->tensor->p_data, oshift, t_size, num_input, input_mem_blk);
	}
	return NN_SUCCESS;
}

nnom_status_t mult_run(nnom_layer_t *layer)
{
	nnom_matrix_layer_t* cl = (nnom_matrix_layer_t*)layer;
	nnom_layer_io_t *in = layer->in;
	size_t t_size = tensor_size(layer->out->tensor);
	int32_t oshift = cl->oshift;
	size_t num_input = nnom_io_length(layer->in);
	q7_t *input_mem_blk[MAX_INPUT_LAYER];

	// if there is only 2 matrix
	if(num_input == 2)
	{
		// the first 2 matrix
		#ifdef NNOM_USING_CMSIS_NN
		if(oshift == 0)
			arm_mult_q7(layer->in->tensor->p_data, layer->in->aux->tensor->p_data, layer->out->tensor->p_data, t_size);
		else
		#endif
			local_mult_q7(layer->in->tensor->p_data, layer->in->aux->tensor->p_data, layer->out->tensor->p_data, oshift, t_size);
	}
	else
	{	
		for(int i = 0; i < num_input; i++)
		{
			input_mem_blk[i] = in->tensor->p_data;
			in = in->aux;
		}
		local_multiple_mult_q7(layer->out->tensor->p_data, oshift, t_size, num_input, input_mem_blk);
	}
	return NN_SUCCESS;
}
