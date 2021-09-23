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
#include "layers/nnom_rnn.h"

nnom_status_t rnn_build(nnom_layer_t *layer);
nnom_status_t rnn_run(nnom_layer_t *layer);
nnom_status_t rnn_free(nnom_layer_t* layer);

// RNN
nnom_layer_t *rnn_s(nnom_rnn_cell_t *cell, const nnom_rnn_config_t* config)
{
	nnom_rnn_layer_t *layer;
	nnom_buf_t *comp;
	nnom_layer_io_t *in, *out;

	// apply a block memory for all the sub handles.
	size_t mem_size = sizeof(nnom_rnn_layer_t) + sizeof(nnom_layer_io_t) * 2 + sizeof(nnom_buf_t);
	layer = nnom_mem(mem_size);
	if (layer == NULL)
		return NULL;

	// distribut the memory to sub handles.
	in = (void *)((uint8_t*)layer + sizeof(nnom_rnn_layer_t));
	out = (void *)((uint8_t*)in + sizeof(nnom_layer_io_t));
	comp = (void *)((uint8_t*)out + sizeof(nnom_layer_io_t));

	// set type in layer parent
	layer->super.type = NNOM_RNN;
	// set buf state
	in->type = NNOM_TENSOR_BUF_TEMP;
	out->type = NNOM_TENSOR_BUF_TEMP;
	comp->type = NNOM_TENSOR_BUF_TEMP;
	// put in & out on the layer.
	layer->super.in = io_init(layer, in);
	layer->super.out = io_init(layer, out);
	layer->super.comp = comp;
	// set run and outshape methods
	layer->super.run = rnn_run;
	layer->super.build = rnn_build;
	layer->super.free = rnn_free;

	// rnn parameters.
	layer->return_sequence = config->return_sequence;
	layer->stateful = config->stateful;
	layer->go_backwards = config->go_backwards;
	layer->super.config = (void*)config;
	layer->cell = cell;

	// set this layer to the cell
	layer->cell->layer = (nnom_layer_t *)layer;

	return (nnom_layer_t *)layer;
}

nnom_status_t rnn_free(nnom_layer_t* layer)
{
	nnom_rnn_layer_t* cl = (nnom_rnn_layer_t*)layer;
	// free cell
	if(cl->cell->free)
		cl->cell->free(cl->cell);

	// free state buffer
	nnom_free(cl->state_buf);

	return NN_SUCCESS;
}

nnom_status_t rnn_build(nnom_layer_t* layer)
{
	nnom_rnn_layer_t *cl = (nnom_rnn_layer_t *)layer;

	// get the tensor from last layer's output
	layer->in->tensor = layer->in->hook.io->tensor;
	
	// timestamp size
	cl->timestamp_size = layer->in->tensor->num_dim > 2 ? layer->in->tensor->dim[1] : layer->in->tensor->dim[0];

	if(cl->return_sequence)
	{
		// create new tensor for the output
		layer->out->tensor = new_tensor(NNOM_QTYPE_PER_TENSOR, 2, 0);
		// shape: timestamp, units
		layer->out->tensor->dim[0] = cl->timestamp_size;
		layer->out->tensor->dim[1] = cl->cell->units;
	}
	else
	{
		// create new tensor for the output
		layer->out->tensor = new_tensor(NNOM_QTYPE_PER_TENSOR, 1, 0);
		// shape: timestamp, units
		layer->out->tensor->dim[0] = cl->cell->units;
	}

	// output q format - the output of the available activations are both q0.7.  
	layer->out->tensor->q_dec[0] = layer->in->tensor->bitwidth==16? 15: 7;
	layer->out->tensor->bitwidth = layer->in->tensor->bitwidth;
	// see if the activation will change the q format
	if(layer->actail) 
		layer->out->tensor->q_dec[0] = act_get_dec_bit(layer->actail->type, layer->out->tensor->q_dec[0]);

	// get feature size from input tensor
	cl->cell->feature_size = tensor_get_num_channel(layer->in->tensor); // vector (feature) size

	// call cell builder to build the cell
	cl->cell->build(cl->cell);

	// get the size of computation buffer?
	cl->super.comp->size = cl->cell->comp_buf_size; 	// size of intermediate buffer required by the cell. 
	cl->state_buf = nnom_mem(cl->cell->state_size * 2); // allocate state buf for upper/lower state buffer. 
	if(!cl->state_buf)
		return NN_NO_MEMORY;
	
	// get the computational cost provided by Cell
	layer->stat.macc = cl->cell->macc * cl->timestamp_size;
	return NN_SUCCESS;
}

nnom_status_t rnn_run(nnom_layer_t* layer)
{
	nnom_status_t result;
	nnom_rnn_layer_t* cl = (nnom_rnn_layer_t*)(layer);
	size_t timestamps_size = layer->in->tensor->dim[layer->in->tensor->num_dim-2];
	size_t feature_size = tensor_get_num_channel(layer->in->tensor); // feature size = last dimension. 
	size_t state_size = cl->cell->state_size;
	size_t output_growth;
	void* upper_state = (q7_t*)cl->state_buf + state_size;
	void* lower_state = (q7_t*)cl->state_buf;

	// reset state buffer if not in stateful
	if (!cl->stateful)
		nnom_memset(cl->state_buf, 0, state_size * 2);

	// set output data
	output_growth = cl->return_sequence ? cl->cell->units : 0;

	// run timestamp by timestamp
	for (uint32_t round = 0; round < timestamps_size; round++)
	{
		if(cl->go_backwards)
		{
			// set input data
			cl->cell->in_data = (q7_t*)layer->in->tensor->p_data + feature_size*(timestamps_size - 1 - round);
			// set output data
			cl->cell->out_data = (q7_t*)layer->out->tensor->p_data + output_growth*(timestamps_size - 1 - round);
		}
		else
		{
			// set input data
			cl->cell->in_data = (q7_t*)layer->in->tensor->p_data + feature_size*round;
			// set output data
			cl->cell->out_data = (q7_t*)layer->out->tensor->p_data + output_growth*round;
		}
		
		// switch upper/lower state buffer
		if(cl->cell->in_state != lower_state)
		{
			cl->cell->in_state = lower_state;
			cl->cell->out_state = upper_state;
		}
		else
		{
			cl->cell->in_state = upper_state;
			cl->cell->out_state = lower_state;
		}

		// run it
		result = cl->cell->run(cl->cell);
		if(result != NN_SUCCESS)
			return result;
	}
	
	return NN_SUCCESS;
}

