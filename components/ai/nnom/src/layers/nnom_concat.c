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
#include "layers/nnom_concat.h"

nnom_layer_t *concat_s(const nnom_concat_config_t *config)
{
	nnom_layer_t* layer = Concat(config->axis);
	if(layer)
		layer->config = (void*) config;
	return layer;
}

// concate method
// concate requires more than one input module. aux input will be allocated in model.merge()
nnom_layer_t *Concat(int8_t axis)
{
	nnom_concat_layer_t *layer;
	nnom_layer_io_t *in, *out;
	size_t mem_size;

	// apply a block memory for all the sub handles.
	mem_size = sizeof(nnom_concat_layer_t) + sizeof(nnom_layer_io_t) * 2;
	layer = nnom_mem(mem_size);
	if (layer == NULL)
		return NULL;

	// distribut the memory to sub handles.
	in = (void *)((uint8_t*)layer + sizeof(nnom_concat_layer_t));
	out = (void *)((uint8_t*)in + sizeof(nnom_layer_io_t));

	// set type in layer parent
	layer->super.type = NNOM_CONCAT;
	layer->super.run = concat_run;
	layer->super.build = concat_build;
	// set buf state
	in->type = NNOM_TENSOR_BUF_TEMP;
	out->type = NNOM_TENSOR_BUF_TEMP;
	// put in & out on the layer.
	layer->super.in = io_init(layer, in);
	layer->super.out = io_init(layer, out);
	
	// axis
	layer->axis = axis; 

	return (nnom_layer_t *)layer;
}


nnom_status_t concat_build(nnom_layer_t *layer)
{
	nnom_concat_layer_t *cl = (nnom_concat_layer_t *)layer;
	nnom_layer_io_t *in;
	uint32_t in_num = 0;
	int32_t num_dim;

	// for each input module, copy the shape from the output of last layer
	in = layer->in;
	while (in != NULL)
	{
		//get the last layer's output as input shape
		in->tensor = in->hook.io->tensor;
		in = in->aux;
		in_num++;
	}
	
	// allocate new tensor for output, keep the same dimension lenght
	layer->out->tensor = new_tensor(NNOM_QTYPE_PER_TENSOR, layer->in->tensor->num_dim, tensor_get_num_channel(layer->in->tensor));
	tensor_cpy_attr(layer->out->tensor, layer->in->tensor);
	
	// convert the axis. 
	if (cl->axis < 0)
		cl->axis = (layer->in->tensor->num_dim + cl->axis);
	else if (cl->axis >0)
		cl->axis = cl->axis -1; // keras use axis start from 1. we are using 0, 1, 2 (check?)

	// find out the concated axis
	num_dim = layer->in->tensor->num_dim;
	for (uint32_t i = 0; i < num_dim; i ++)
	{
		// exclue the concat axies
		if (i == cl->axis)
		{
			layer->out->tensor->dim[i] = 0;

			// add the same axis from all input up. 
			in = layer->in;
			while (in != NULL)
			{
				layer->out->tensor->dim[i] += in->tensor->dim[i];
				in = in->aux;
			}
			continue;
		}

		// check others, all other must be same shape
		in = layer->in;
		while (in != NULL && in->aux != NULL)
		{
			if (in->tensor->dim[i] != in->aux->tensor->dim[i])
				return NN_ARGUMENT_ERROR;
			in = in->aux;
		}

		// now set other axis
		layer->out->tensor->dim[i] = layer->in->tensor->dim[i];
	}

	return NN_SUCCESS;
}


#ifdef NNOM_USING_CHW
// axis index converter between HWC and CHW
static inline int chw_i(int hwc, int num_dim)
{
    num_dim = num_dim -1;
	hwc = hwc + 1;			
	if(hwc>num_dim) 
		hwc = 0;
	return hwc;
}
static inline int hwc_i(int chw, int num_dim)
{
    num_dim = num_dim -1;
	chw = chw - 1;			
	if(chw<num_dim) 
		chw = num_dim;
	return chw;
}
#endif

nnom_status_t concat_run(nnom_layer_t *layer)
{
	// by default, concat layer has mutiple (>=2) input and 1 output.
	nnom_concat_layer_t *cl = (nnom_concat_layer_t *)layer;
	nnom_layer_io_t *in;
    uint32_t dwidth = layer->in->tensor->bitwidth/8; // data width in byte

#ifdef NNOM_USING_CHW
	// Concatenate for HWC	
	uint8_t *pin;
	uint8_t *pout = layer->out->tensor->p_data;
	uint32_t block_size;
	uint32_t n_block;
	uint8_t num_dim = layer->in->tensor->num_dim;
	
	// calcualte number of block to concat. the other shapes before the concat axis
	n_block = 1;
	for(int i= 0; i< chw_i(cl->axis, num_dim); i++)
	{
		n_block *= layer->in->tensor->dim[hwc_i(i, num_dim)];
	}
	
	// concat all input layers
	for(int i=0; i<n_block; i++)
	{
		in = layer->in;
		while (in != NULL)
		{
			// the block size of concat data in this layer
			block_size = dwidth;
			for(int j= num_dim-1; j >= chw_i(cl->axis, num_dim); j--)
				block_size *= in->tensor->dim[hwc_i(j, num_dim)];
			// concat		
			pin = (uint8_t *)in->tensor->p_data + i * block_size;
			nnom_memcpy(pout, pin, block_size);
			pout += block_size;
			in = in->aux;
		}
	}
	
#else // end of CHW concate

	// Concatenate for HWC	
	uint8_t* pin;
	uint8_t* pout = layer->out->tensor->p_data;
	uint32_t block_size;
	uint32_t n_block;
	uint8_t num_dim = layer->in->tensor->num_dim;

	// calcualte the number of block to concat. (the other shapes before the concat axis)
	n_block = 1;
	for (int i = 0; i < cl->axis; i++)
		n_block *= layer->in->tensor->dim[i];

	// concat all input layers
	for (int i = 0; i < n_block; i++)
	{
		in = layer->in;
		while (in != NULL)
		{
			// the block size of concat data in this layer
			block_size = dwidth;
			for (int j = cl->axis; j < num_dim; j++)
				block_size *= in->tensor->dim[j];
			// concat		
			pin = (uint8_t*)in->tensor->p_data + i * block_size;
			nnom_memcpy(pout, pin, block_size);
			pout += block_size;
			in = in->aux;
		}
	}
#endif
	return NN_SUCCESS;
}

