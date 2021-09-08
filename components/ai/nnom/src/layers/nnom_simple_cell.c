/*
 * Copyright (c) 2018-2020
 * Jianjia Ma
 * majianjia@live.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-08-21     Jianjia Ma   The first version
 */

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "nnom.h"
#include "nnom_local.h"
#include "nnom_layers.h"
#include "layers/nnom_simple_cell.h"

#ifdef NNOM_USING_CMSIS_NN
#include "arm_math.h"
#include "arm_nnfunctions.h"
#endif

// Simple RNN
// unit = output shape
// type of activation
nnom_rnn_cell_t *simple_cell_s(const nnom_simple_cell_config_t* config)
{
	nnom_simple_cell_t *cell;
	cell = nnom_mem(sizeof(nnom_simple_cell_t));
	if (cell == NULL)
		return NULL;
	// set methods
	cell->super.run = simple_cell_run;
	cell->super.build = simple_cell_build;
	cell->super.free = simple_cell_free;
	cell->super.config = (void*) config;
	cell->super.units = config->units;
	cell->super.type = NNOM_SIMPLE_CELL;

	// set parameters 
	cell->bias = config->bias;
	cell->weights = config->weights;
	cell->recurrent_weights = config->recurrent_weights;
	cell->act_type = config->act_type; 
	// q format for intermediate products
	cell->q_dec_iw = config->q_dec_iw;
	cell->q_dec_hw = config->q_dec_hw;
	cell->q_dec_h = config->q_dec_h;
	
	return (nnom_rnn_cell_t *)cell;
}

nnom_status_t simple_cell_free(nnom_rnn_cell_t* cell)
{
	return NN_SUCCESS;
}

// the state buffer and computational buffer shape of the cell
nnom_status_t simple_cell_build(nnom_rnn_cell_t* cell)
{
	nnom_layer_t *layer = cell->layer; 
	nnom_simple_cell_t *c = (nnom_simple_cell_t *)cell;
	nnom_simple_cell_config_t *config = (nnom_simple_cell_config_t *)cell->config;
	int q_hw_iw;
	
	// activation, check if activation is supported 
	if(config->act_type != ACT_SIGMOID && config->act_type != ACT_TANH)
		return NN_ARGUMENT_ERROR;

	// calculate output shift for the 2 calculation. 
	// hw = the product of hidden x weight, iw = the product of input x weight
	// due to the addition of them, they must have same q format.
	q_hw_iw = MIN(c->q_dec_hw, c->q_dec_iw);  

	// for the 2 dot in cell: output shift = input_dec + weight_dec - output_dec
	c->oshift_hw = c->q_dec_h + c->recurrent_weights->q_dec[0] - q_hw_iw;
	c->oshift_iw = layer->in->tensor->q_dec[0] + c->weights->q_dec[0] - q_hw_iw;

	// bias shift =  bias_dec - out_dec
	c->bias_shift = layer->in->tensor->q_dec[0] + c->weights->q_dec[0] - c->bias->q_dec[0];

	// state size = one timestamp output size. 
	cell->state_size = cell->units;

	// comp buffer size: not required
	cell->comp_buf_size = 0; 

	// finally, calculate the MAC for info
	cell->macc = cell->feature_size * cell->units // input: feature  * state
				+ cell->units * cell->units;      // recurrent, state *  output_unit

	return NN_SUCCESS;
}

// This Simple Cell replicate the Keras's SimpleCell as blow 
/*
 def call(self, inputs, states, training=None):
    prev_output = states[0] if nest.is_sequence(states) else states

	h = K.dot(inputs, self.kernel)
	h = K.bias_add(h, self.bias)

	h2 = K.dot(prev_output, self.recurrent_kernel)
    output = h + H2
    output = self.activation(output)

    new_state = [output] if nest.is_sequence(states) else output
    return output, new_state
*/

nnom_status_t simple_cell_run(nnom_rnn_cell_t* cell)
{
	nnom_simple_cell_t* c = (nnom_simple_cell_t*) cell;
	int act_int_bit = 7 - MIN(c->q_dec_hw, c->q_dec_iw);

	// in_state x recurrent_weight -> h2 (output buf)
	local_dot_q7_opt(cell->in_state, c->recurrent_weights->p_data, cell->units, cell->units, c->oshift_hw, cell->out_data);
	// (input x weight) + bias -> h (in_state buf)
	local_fully_connected_q7_opt(cell->in_data, c->weights->p_data, 
				cell->feature_size, cell->units, c->bias_shift, c->oshift_iw, c->bias->p_data, cell->in_state, NULL);
	// h + h2 -> (out_state buf)
	local_add_q7(cell->in_state, cell->out_data, cell->out_state, 0, cell->units);

	// active(out_state buf)
	if(c->act_type == ACT_TANH)
		local_tanh_q7(cell->out_state, cell->units, act_int_bit);
		//local_hard_tanh_q7(cell->out_state, cell->units, act_int_bit);
	else
		local_sigmoid_q7(cell->out_state, cell->units, act_int_bit);
		//local_hard_sigmoid_q7(cell->out_state, cell->units, act_int_bit);

	// (out_state buf) --copy--> (output buf)
	nnom_memcpy(cell->out_data, cell->out_state, cell->units);

	return NN_SUCCESS;
}


