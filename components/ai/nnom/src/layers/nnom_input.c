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
#include "layers/nnom_input.h"

nnom_layer_t *input_s(const nnom_io_config_t* config)
{
	nnom_io_layer_t *layer;
	nnom_layer_io_t *in, *out;
	// apply a block memory for all the sub handles.
	layer = nnom_mem(sizeof(nnom_io_layer_t) + sizeof(nnom_layer_io_t) * 2);
	if (layer == NULL)
		return NULL;

	// distribut the memory to sub handles.
	in = (void *)((uint8_t*)layer + sizeof(nnom_io_layer_t));
	out = (void *)((uint8_t*)in + sizeof(nnom_layer_io_t));

	// set type in layer parent
	layer->super.type = NNOM_INPUT;
	layer->super.run = input_run;
	layer->super.build = input_build;
	// set buf state
	in->type = NNOM_TENSOR_BUF_TEMP;
	out->type = NNOM_TENSOR_BUF_NULL;
	// put in & out on the layer.
	layer->super.in = io_init(layer, in);
	layer->super.out = io_init(layer, out);

	/*
	// some other layers (Conv, pooling) are not supporting 12 d input, we still expand the 1,2 dimension to 3
    // test -> native support 1,2,3 D input. 
	layer->super.in->tensor = new_tensor(NNOM_QTYPE_PER_TENSOR, config->tensor->num_dim, tensor_get_num_channel(config->tensor));
	tensor_cpy_attr(layer->super.in->tensor, config->tensor);
	layer->buf = config->tensor->p_data;
	layer->dec_bit = config->tensor->q_dec[0];
	*/

	// set parameters
    if(config->tensor->num_dim == 1) // test for 1d input, expend h = 1
        layer->shape = shape(1, 1, config->tensor->dim[0]);
    else if (config->tensor->num_dim == 2) // test for 1d input, expend h = 1
		layer->shape = shape(1, config->tensor->dim[0], config->tensor->dim[1]);
	else
		layer->shape = shape(config->tensor->dim[0], config->tensor->dim[1], config->tensor->dim[2]);
	layer->buf = config->tensor->p_data;
	layer->dec_bit = config->tensor->q_dec[0];

	// experimental: fixed input dim to 3
	// input normally dont have a tensor, so we create one to store the initial data. 
	nnom_shape_data_t dim[3] = {layer->shape.h, layer->shape.w, layer->shape.c};
	layer->super.in->tensor = new_tensor(NNOM_QTYPE_PER_TENSOR, 3, tensor_get_num_channel(config->tensor));
	tensor_set_attr_v(layer->super.in->tensor, layer->dec_bit, 0, dim, sizeof(dim)/sizeof(nnom_shape_data_t), 8);
	return (nnom_layer_t *)layer;
}

nnom_layer_t *Input(nnom_3d_shape_t input_shape, void *p_buf)
{
	nnom_io_layer_t *layer;
	nnom_layer_io_t *in, *out;

	// apply a block memory for all the sub handles.
	layer = nnom_mem(sizeof(nnom_io_layer_t) + sizeof(nnom_layer_io_t) * 2);
	if (layer == NULL)
		return NULL;

	// distribut the memory to sub handles.
	in = (void *)((uint8_t*)layer + sizeof(nnom_io_layer_t));
	out = (void *)((uint8_t*)in + sizeof(nnom_layer_io_t));

	// set type in layer parent
	layer->super.type = NNOM_INPUT;
	layer->super.run = input_run;
	layer->super.build = input_build;
	// set buf state
	in->type = NNOM_TENSOR_BUF_TEMP;
	out->type = NNOM_TENSOR_BUF_NULL;
	// put in & out on the layer.
	layer->super.in = io_init(layer, in);
	layer->super.out = io_init(layer, out);

	// set parameters
	layer->shape = input_shape;
	layer->buf = p_buf;
	layer->dec_bit = 7;

	// experimental: fixed input dim to 3
	// input normally dont have a tensor, so we create one to store the initial data. 
	nnom_shape_data_t dim[3] = { input_shape.h, input_shape.w, input_shape.c };
	layer->super.in->tensor = new_tensor(NNOM_QTYPE_PER_TENSOR, 3, input_shape.c);
	tensor_set_attr_v(layer->super.in->tensor, layer->dec_bit, 0, dim, sizeof(dim)/sizeof(nnom_shape_data_t), 8);
	return (nnom_layer_t *)layer;
}

nnom_status_t input_build(nnom_layer_t* layer)
{
	// the input tensor of inputlayer has assigned previously 

	// output tensor
	// 1. allocate a new tensor for output
	// 2. set the same dim, qfmt to the new tensor.
	layer->out->tensor = new_tensor(NNOM_QTYPE_PER_TENSOR, layer->in->tensor->num_dim, tensor_get_num_channel(layer->in->tensor));
	tensor_cpy_attr(layer->out->tensor, layer->in->tensor);

	// now this build has passed the input tensors (shapes, formats) to the new tensors. 
	return NN_SUCCESS;
}


nnom_status_t input_run(nnom_layer_t *layer)
{
	nnom_io_layer_t *cl = (nnom_io_layer_t *)layer;
#ifdef NNOM_USING_CHW
	if(layer->in->tensor->num_dim == 3)
    {
        nnom_3d_shape_t shape = {layer->in->tensor->dim[0], layer->in->tensor->dim[1], layer->in->tensor->dim[2]};
        hwc2chw_q7(shape, cl->buf, layer->in->tensor->p_data);
    }
    else if (layer->in->tensor->num_dim == 2)
    {
        nnom_3d_shape_t shape = {1, layer->in->tensor->dim[0], layer->in->tensor->dim[1]};
        hwc2chw_q7(shape, cl->buf, layer->in->tensor->p_data);
    }
    else
#endif
	nnom_memcpy(layer->in->tensor->p_data, cl->buf, tensor_size(layer->in->tensor));

	return NN_SUCCESS;
}
