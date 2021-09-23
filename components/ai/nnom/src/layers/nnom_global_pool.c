
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
#include "layers/nnom_global_pool.h"

nnom_layer_t * global_maxpool_s(const nnom_global_pool_config_t *config)
{
	nnom_maxpool_layer_t * cl = (nnom_maxpool_layer_t *)GlobalMaxPool();
	if(cl)
	{
		cl->super.config = (void*) config;
		cl->output_shift = config->output_shift;
	}
	return (nnom_layer_t *)cl;
}
nnom_layer_t * global_avgpool_s(const nnom_global_pool_config_t *config)
{
	nnom_maxpool_layer_t * cl = (nnom_maxpool_layer_t *)GlobalAvgPool();
	if(cl)
	{
		cl->super.config = (void*) config;
		cl->output_shift = config->output_shift;
	}
	return (nnom_layer_t *)cl;
}

nnom_layer_t * global_sumpool_s(const nnom_global_pool_config_t *config)
{
	nnom_maxpool_layer_t * cl = (nnom_maxpool_layer_t *)GlobalSumPool();
	if(cl)
	{
		cl->super.config = (void*) config;
		cl->output_shift = config->output_shift;
	}
	return (nnom_layer_t *)cl;
}


nnom_layer_t *GlobalMaxPool(void)
{
	// create the normal pooling layer, the parameters are left empty to fill in later.
	// parameters will be filled in in global_pooling_build()
	nnom_layer_t *layer = MaxPool(kernel(0, 0), stride(0, 0), PADDING_VALID);

	// change to global max pool
	if (layer != NULL)
	{
		layer->type = NNOM_GLOBAL_MAXPOOL;
		layer->build = global_pool_build;
	}

	return (nnom_layer_t *)layer;
}

nnom_layer_t *GlobalAvgPool(void)
{
	// create the normal pooling layer, the parameters are left empty to fill in later.
	// parameters will be filled in global_pooling_build() remotely
	nnom_layer_t *layer = MaxPool(kernel(0, 0), stride(0, 0), PADDING_VALID);

	// change some parameters to be recognised as avg pooling
	if (layer != NULL)
	{
		layer->type = NNOM_GLOBAL_AVGPOOL;
		layer->run = avgpool_run; // global and basic pooling share the same runner
		layer->build = global_pool_build;
	}

	return (nnom_layer_t *)layer;
}

nnom_layer_t *GlobalSumPool(void)
{
	// create the normal pooling layer, the parameters are left empty to fill in later.
	// parameters will be filled in global_pooling_build() remotely
	nnom_layer_t *layer = MaxPool(kernel(0, 0), stride(0, 0), PADDING_VALID);

	// change some parameters to be recognised as avg pooling
	if (layer != NULL)
	{
		layer->type = NNOM_GLOBAL_SUMPOOL;
		layer->run = sumpool_run; // global and basic pooling share the same runner
		layer->build = global_pool_build;
	}

	return (nnom_layer_t *)layer;
}

nnom_status_t global_pool_build(nnom_layer_t *layer)
{
	nnom_maxpool_layer_t *cl = (nnom_maxpool_layer_t *)layer;

	// get the tensor from last layer's output
	layer->in->tensor = layer->in->hook.io->tensor;

	// create new tensor for output
	layer->out->tensor = new_tensor(NNOM_QTYPE_PER_TENSOR, 1, tensor_get_num_channel(layer->in->tensor));

	nnom_shape_data_t dim[1] = {tensor_get_num_channel(layer->in->tensor)}; // fill the first 2 dim later
	tensor_set_attr_v(layer->out->tensor, layer->in->tensor->q_dec[0], 0, dim, sizeof(dim)/sizeof(nnom_shape_data_t), 8); 
	
	// see if the activation will change the q format
	if(layer->actail) 
		layer->out->tensor->q_dec[0] = act_get_dec_bit(layer->actail->type, layer->out->tensor->q_dec[0]);

	// different from other *_build(), the kernel..padding left by layer API needs to be set in here
	// due to the *_run() methods of global pooling are using the normall pooling's.
	// fill in the parameters left by layer APIs (GlobalAvgPool and MaxAvgPool)
	cl->kernel = shape(layer->in->tensor->dim[0], layer->in->tensor->dim[1], 1);
	cl->stride = shape(1, 1, 1);
	cl->pad = shape(0, 0, 0);
	cl->padding_type = PADDING_VALID;

	// additionally, avg pooling require computational buffer, which is  2*dim_im_out*ch_im_in
	if (layer->type == NNOM_AVGPOOL || layer->type == NNOM_GLOBAL_AVGPOOL)
	{
		//  bufferA size:  2*dim_im_out*ch_im_in
		layer->comp->size = 2 * layer->out->tensor->dim[0] * layer->in->tensor->dim[2];
	}
	
	// additional for sumpool
	if (layer->type == NNOM_SUMPOOL || layer->type == NNOM_GLOBAL_SUMPOOL)
		layer->comp->size = 4 * tensor_size(layer->out->tensor);

	return NN_SUCCESS;
}


