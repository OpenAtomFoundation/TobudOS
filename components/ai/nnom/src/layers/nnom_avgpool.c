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
#include "layers/nnom_avgpool.h"

#ifdef NNOM_USING_CMSIS_NN
#include "arm_math.h"
#include "arm_nnfunctions.h"
#endif

nnom_layer_t *avgpool_s(const nnom_pool_config_t * config)
{
	nnom_avgpool_layer_t *cl;
	
	if(config->num_dim == 1)
	{
		cl = (nnom_avgpool_layer_t *)AvgPool(kernel(1, config->kernel_size[0]), 
						stride(1, config->stride_size[0]),
						config->padding_type);
	}
	else
	{
		cl = (nnom_avgpool_layer_t *)AvgPool(kernel(config->kernel_size[0], config->kernel_size[1]), 
						stride(config->stride_size[0], config->stride_size[1]),
						config->padding_type);
	}
	
	if(cl)
	{
		cl->super.config = (void*) config;
		cl->output_shift = config->output_shift; // no idea if we need it
	}
	return (nnom_layer_t *)cl;
}

nnom_layer_t *AvgPool(nnom_3d_shape_t k, nnom_3d_shape_t s, nnom_padding_t pad_type)
{
	nnom_layer_t *layer = MaxPool(k, s, pad_type);

	if (layer != NULL)
	{
		layer->type = NNOM_AVGPOOL;
		layer->run = avgpool_run;
		layer->build = avgpool_build;
	}
	return (nnom_layer_t *)layer;
}

nnom_status_t avgpool_build(nnom_layer_t *layer)
{
	uint32_t size;
	// avg pooling share the same output shape, stride, padding setting.
	maxpool_build(layer);

	#ifdef NNOM_USING_CMSIS_NN
	// however, avg pooling require a computational buffer.
	//  bufferA size:  2*dim_im_out*ch_im_in
	size = layer->out->tensor->dim[1] > layer->out->tensor->dim[0] ?
						layer->out->tensor->dim[1] : layer->out->tensor->dim[0];
	layer->comp->size = 2 * size * layer->in->tensor->dim[2];
	#endif

	return NN_SUCCESS;
}

nnom_status_t avgpool_run(nnom_layer_t *layer)
{
	nnom_avgpool_layer_t *cl = (nnom_avgpool_layer_t *)(layer);
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

    // 16 bit
    if(layer->in->tensor->bitwidth == 16)
    {
#ifdef NNOM_USING_CHW
	local_avepool_q15_CHW(layer->in->tensor->p_data, 				
			layer->in->tensor->dim[1], layer->in->tensor->dim[0], layer->in->tensor->dim[2],
			cl->kernel.w, cl->kernel.h, 
			cl->pad.w, cl->pad.h,
			cl->stride.w, cl->stride.h,
			out_x, out_y,
			cl->output_shift,
			NULL,
			layer->out->tensor->p_data);
#else
    local_avepool_q15_HWC(layer->in->tensor->p_data, 				
            layer->in->tensor->dim[1], layer->in->tensor->dim[0], layer->in->tensor->dim[2],
            cl->kernel.w, cl->kernel.h, 
            cl->pad.w, cl->pad.h,
            cl->stride.w, cl->stride.h,
            out_x, out_y,
            cl->output_shift,
            NULL,
            layer->out->tensor->p_data);
#endif
    }
    // 8bit
	else{
#ifdef NNOM_USING_CHW
	local_avepool_q7_CHW(layer->in->tensor->p_data, 				
			layer->in->tensor->dim[1], layer->in->tensor->dim[0], layer->in->tensor->dim[2],
			cl->kernel.w, cl->kernel.h, 
			cl->pad.w, cl->pad.h,
			cl->stride.w, cl->stride.h,
			out_x, out_y,
			cl->output_shift,
			NULL,
			layer->out->tensor->p_data);
#else //end of CHW
	#ifdef NNOM_USING_CMSIS_NN
	// 2D, square
	if (layer->in->tensor->dim[1] == layer->in->tensor->dim[0] &&
		layer->out->tensor->dim[1] == layer->out->tensor->dim[0] &&
		cl->output_shift == 0)
	{
		arm_avepool_q7_HWC(
			layer->in->tensor->p_data,
			layer->in->tensor->dim[1], layer->in->tensor->dim[2],
			cl->kernel.w, cl->pad.w, cl->stride.w,
			layer->out->tensor->dim[1],
			layer->comp->mem->blk,
			layer->out->tensor->p_data);
	}
	// none square 2D, or 1D
	else
	#endif
	{
		// CMSIS-NN does not support none-square pooling, we have to use local implementation
		local_avepool_q7_HWC(layer->in->tensor->p_data, 				
				layer->in->tensor->dim[1], layer->in->tensor->dim[0], layer->in->tensor->dim[2],
				cl->kernel.w, cl->kernel.h, 
				cl->pad.w, cl->pad.h,
				cl->stride.w, cl->stride.h,
				out_x, out_y,
				cl->output_shift,
				NULL,
				layer->out->tensor->p_data);
	}
#endif
    }
	return NN_SUCCESS;
}
