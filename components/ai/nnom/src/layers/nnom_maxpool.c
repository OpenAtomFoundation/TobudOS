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
#include "layers/nnom_maxpool.h"

#ifdef NNOM_USING_CMSIS_NN
#include "arm_math.h"
#include "arm_nnfunctions.h"
#endif

nnom_layer_t *maxpool_s(const nnom_pool_config_t * config)
{
	nnom_layer_t *layer;
	
	// test, to accomodate 1d and 2d input
	if(config->num_dim == 1)
	{
		layer = MaxPool(kernel(1, config->kernel_size[0]), 
						stride(1, config->stride_size[0]), 
						config->padding_type);
	}
	else
	{
		layer = MaxPool(kernel(config->kernel_size[0], config->kernel_size[1]), 
						stride(config->stride_size[0], config->stride_size[1]),
						config->padding_type);
	}
	
	if(layer)
		layer->config = (void*) config;
	return layer;
}

nnom_layer_t *MaxPool(nnom_3d_shape_t k, nnom_3d_shape_t s, nnom_padding_t pad_type)
{
	nnom_maxpool_layer_t *layer;
	nnom_buf_t *comp;
	nnom_layer_io_t *in, *out;

	// apply a block memory for all the sub handles.
	size_t mem_size = sizeof(nnom_maxpool_layer_t) + sizeof(nnom_layer_io_t) * 2 + sizeof(nnom_buf_t);
	layer = nnom_mem(mem_size);
	if (layer == NULL)
		return NULL;

	// distribut the memory to sub handles.
	in = (void *)((uint8_t*)layer + sizeof(nnom_maxpool_layer_t));
	out = (void *)((uint8_t*)in + sizeof(nnom_layer_io_t));
	comp = (void *)((uint8_t*)out + sizeof(nnom_layer_io_t));

	// set type in layer parent
	layer->super.type = NNOM_MAXPOOL;
	layer->super.run = maxpool_run;
	layer->super.build = maxpool_build;
	// set buf state
	in->type = NNOM_TENSOR_BUF_TEMP;
	out->type = NNOM_TENSOR_BUF_TEMP;
	comp->type = NNOM_TENSOR_BUF_TEMP;
	// put in & out on the layer.
	layer->super.in = io_init(layer, in);
	layer->super.out = io_init(layer, out);
	layer->super.comp = comp;

	// set parameters
	layer->kernel = k;
	layer->stride = s;
	layer->padding_type = pad_type;

	// padding
	if (layer->padding_type == PADDING_SAME)
	{
		layer->pad.h = (k.h - 1) / 2;
		layer->pad.w = (k.w - 1) / 2;
		layer->pad.c = 1; // no meaning
	}
	else
	{
		layer->pad.h = 0;
		layer->pad.w = 0;
		layer->pad.c = 0;
	}
	return (nnom_layer_t *)layer;
}

nnom_status_t maxpool_build(nnom_layer_t *layer)
{
	nnom_maxpool_layer_t *cl = (nnom_maxpool_layer_t *)layer;

	// get the tensor from last layer's output
	layer->in->tensor = layer->in->hook.io->tensor;

	// create new tensor for output
	layer->out->tensor = new_tensor(NNOM_QTYPE_PER_TENSOR, layer->in->tensor->num_dim, tensor_get_num_channel(layer->in->tensor));
	// copy then change later. 
	tensor_cpy_attr(layer->out->tensor, layer->in->tensor);
	
	// see if the activation will change the q format
	if(layer->actail) 
		layer->out->tensor->q_dec[0] = act_get_dec_bit(layer->actail->type, layer->out->tensor->q_dec[0]);

	// now we set up the tensor shape, always HWC format
	if (cl->padding_type == PADDING_SAME)
	{
		layer->out->tensor->dim[0] = NN_CEILIF(layer->in->tensor->dim[0], cl->stride.h);
		layer->out->tensor->dim[1] = NN_CEILIF(layer->in->tensor->dim[1], cl->stride.w);
		layer->out->tensor->dim[2] = layer->in->tensor->dim[2]; // channel stays the same
	}
	else
	{
		layer->out->tensor->dim[0] = NN_CEILIF(layer->in->tensor->dim[0] - cl->kernel.h + 1, cl->stride.h);
		layer->out->tensor->dim[1] = NN_CEILIF(layer->in->tensor->dim[1] - cl->kernel.w + 1, cl->stride.w);
		layer->out->tensor->dim[2] = layer->in->tensor->dim[2];
	}

	return NN_SUCCESS;
}

nnom_status_t maxpool_run(nnom_layer_t *layer)
{
	nnom_maxpool_layer_t *cl = (nnom_maxpool_layer_t *)(layer);

	uint16_t out_x, out_y;

	// if global pooling
	if(layer->out->tensor->num_dim == 1)
	{
		out_x = 1; out_y = 1;
	}
	else // normal pooling. 
	{
		out_x = layer->out->tensor->dim[1]; //W
		out_y = layer->out->tensor->dim[0]; //h
	}
	
#ifdef NNOM_USING_CHW
    local_maxpool_q7_CHW(layer->in->tensor->p_data, 				
			layer->in->tensor->dim[1], layer->in->tensor->dim[0], layer->in->tensor->dim[2],
			cl->kernel.w, cl->kernel.h, 
			cl->pad.w, cl->pad.h,
			cl->stride.w, cl->stride.h,
			out_x, out_y,
			NULL,
			layer->out->tensor->p_data);
#else //end of CHW
	// HWC
	#ifdef NNOM_USING_CMSIS_NN
	// 2D, square
	if (layer->in->tensor->dim[1] == layer->in->tensor->dim[0] &&
		layer->out->tensor->dim[1] == layer->out->tensor->dim[0])
	{
		arm_maxpool_q7_HWC(
			layer->in->tensor->p_data,
			layer->in->tensor->dim[1], layer->in->tensor->dim[2],
			cl->kernel.w, cl->pad.w, cl->stride.w,
			layer->out->tensor->dim[1],
			NULL,
			layer->out->tensor->p_data);
	}
	// none square 2D, or 1D
	else
	#endif
	{
		// CMSIS-NN does not support none-square pooling, we have to use local implementation
		local_maxpool_q7_HWC(layer->in->tensor->p_data, 				
				layer->in->tensor->dim[1], layer->in->tensor->dim[0], layer->in->tensor->dim[2],
				cl->kernel.w, cl->kernel.h, 
				cl->pad.w, cl->pad.h,
				cl->stride.w, cl->stride.h,
				out_x, out_y,
				NULL,
				layer->out->tensor->p_data);
	}
#endif // CHW/HWC
	return NN_SUCCESS;
}
