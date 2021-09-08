/*
 * Copyright (c) 2018-2020
 * Jianjia Ma
 * majianjia@live.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-08-24     Jianjia Ma   The first version
 */

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "nnom.h"
#include "nnom_local.h"
#include "nnom_layers.h"
#include "layers/nnom_lstm_cell.h"

#ifdef NNOM_USING_CMSIS_NN
#include "arm_math.h"
#include "arm_nnfunctions.h"
#endif

// LSTM RNN
// unit = output shape
// type of activation
nnom_rnn_cell_t *lstm_cell_s(const nnom_lstm_cell_config_t* config)
{
	nnom_lstm_cell_t *cell;
	cell = nnom_mem(sizeof(nnom_lstm_cell_t));
	if (cell == NULL)
		return NULL;
	// set methods
	cell->super.run = lstm_cell_q7_q15_run;
	cell->super.build = lstm_cell_q7_q15_build;
	cell->super.free = lstm_cell_free;
	cell->super.config = (void*) config;
	cell->super.units = config->units;
    cell->super.type = NNOM_LSTM_CELL;

	// set parameters 
	cell->bias = config->bias;
	cell->weights = config->weights;
	cell->recurrent_weights = config->recurrent_weights;

    // q format for intermediate calculation
    cell->q_dec_c = config->q_dec_c;
    cell->q_dec_h = config->q_dec_h;
    cell->q_dec_z = config->q_dec_z;

	return (nnom_rnn_cell_t *)cell;
}

nnom_status_t lstm_cell_free(nnom_rnn_cell_t* cell)
{
	return NN_SUCCESS;
}

// keras implementation as below. 
/*
  def step(cell_inputs, cell_states):
    """Step function that will be used by Keras RNN backend."""
    h_tm1 = cell_states[0]  # previous memory state
    c_tm1 = cell_states[1]  # previous carry state

    z = K.dot(cell_inputs, kernel)          -> q_iw
    z += K.dot(h_tm1, recurrent_kernel)     -> q_hw
    z = K.bias_add(z, bias)                 

    z0, z1, z2, z3 = array_ops.split(z, 4, axis=1)

    i = nn.sigmoid(z0)
    f = nn.sigmoid(z1)
    c = f * c_tm1 + i * nn.tanh(z2)
    o = nn.sigmoid(z3)

    h = o * nn.tanh(c)
    return h, [h, c]
*/



// the state buffer and computational buffer shape of the cell
nnom_status_t lstm_cell_q7_q15_build(nnom_rnn_cell_t* cell)
{
	nnom_layer_t *layer = cell->layer; 
	nnom_lstm_cell_t *c = (nnom_lstm_cell_t *)cell;

	// calculate output shift for the 2 calculation. 
	// hw = the product of hidden x weight, iw = the product of input x weight
	// due to the addition of them, they must have same q format.
    // that is -> c->q_dec_z; 

	// for the dots in cell: output shift = input_dec + weight_dec - output_dec
	c->oshift_hw = c->q_dec_h + c->recurrent_weights->q_dec[0] - c->q_dec_z;
	c->oshift_iw = layer->in->tensor->q_dec[0] + c->weights->q_dec[0] - c->q_dec_z; 

	// bias shift =  bias_dec - out_dec
	c->bias_shift = layer->in->tensor->q_dec[0] + c->weights->q_dec[0] - c->bias->q_dec[0];

	// state size = one timestamp output size. 
	cell->state_size = cell->units * 2 * 2; // Q15

	// // comp buffer size: not required
	cell->comp_buf_size = cell->units * 12 * 2 + cell->feature_size * 2; //q15 + input q7->q15 buffer.  

	// finally, calculate the MAC for info (for each timestamp)
	cell->macc = cell->feature_size * cell->units *4    // input: feature *  state * 4 gates
				+ cell->units * cell->units *4		    // recurrent, state
				+ cell->units  *10;                     // output_unit * (5 gate + 3 mult + 2 addition)

	return NN_SUCCESS;
}

// Q7 input output 
// Q7 weights
// Q15 states and intermediate buffer
nnom_status_t lstm_cell_q7_q15_run(nnom_rnn_cell_t* cell)
{
	nnom_layer_t *layer = cell->layer;
	nnom_lstm_cell_t* c = (nnom_lstm_cell_t*) cell;
    int act_int_bit = 7 - c->q_dec_z;

    // state buffer
    // low |-- hidden --|-- carry --| high
    q15_t* h_tm1 = (q15_t*)cell->in_state;
    q15_t* c_tm1 = (q15_t*)cell->in_state + cell->units;
    q15_t* o_state[2];
    o_state[0] = (q15_t*)cell->out_state;
    o_state[1] = (q15_t*)cell->out_state + cell->units;

    // computing buffer
    // low |-- buf0 --|-- buf1 --|-- buf2 --|-- input q15 --|
    q15_t* z[4];
    q15_t *buf0, *buf1, *buf2, *in_q15_buf;
    buf0 = (q15_t*)layer->comp->mem->blk;
    buf1 = (q15_t*)layer->comp->mem->blk + cell->units*4;
    buf2 = (q15_t*)layer->comp->mem->blk + cell->units*8;
    in_q15_buf = (q15_t*)layer->comp->mem->blk + cell->units*12;

    // input q7 -> q15
    local_q7_to_q15(cell->in_data, in_q15_buf, cell->feature_size);

    // z1 = K.dot(cell_inputs, kernel) + bias -> buf1
	#ifdef NNOM_USING_CMSIS_NN
		arm_fully_connected_mat_q7_vec_q15_opt
	#else
		local_fully_connected_mat_q7_vec_q15_opt
	#endif 
		(in_q15_buf, c->weights->p_data, cell->feature_size, cell->units*4, c->bias_shift + 8, c->oshift_iw, c->bias->p_data, buf1, NULL);

    // z2 = K.dot(h_tm1, recurrent_kernel)  -> buf2
	// --- arm version must use bias, so we have to use local implementation
    local_fully_connected_mat_q7_vec_q15_opt(h_tm1, c->recurrent_weights->p_data, 
            cell->units, cell->units*4, 0, c->oshift_hw, NULL, buf2, NULL); 

    // z = z1 + z2  -> buf0
    local_add_q15(buf1, buf2, buf0, 0, cell->units*4);

    // split the data to each gate
    z[0] = buf0;
    z[1] = buf0 + cell->units;
    z[2] = buf0 + cell->units*2;
    z[3] = buf0 + cell->units*3;

    // i = nn.sigmoid(z0)
    local_sigmoid_q15(z[0], cell->units, act_int_bit);
    // f = nn.sigmoid(z1)
    local_sigmoid_q15(z[1], cell->units, act_int_bit);
    // o = nn.sigmoid(z3)
    local_sigmoid_q15(z[3], cell->units, act_int_bit);

    /* c = f * c_tm1 + i * nn.tanh(z2) for the step 1-3. */
    // 1. i * tanh(z2) -> buf1
    local_tanh_q15(z[2], cell->units, act_int_bit);
    local_mult_q15(z[0], z[2], buf1, 30 - (c->q_dec_c+8), cell->units); 
    // 2. f * c_tm1 -> o_state[0] 
    local_mult_q15(z[1], c_tm1, o_state[0], 15, cell->units);
    // 3. c = i*tanh + f*c_tm1 -> o_state[1]   ** fill the upper state (carry)
    local_add_q15(buf1, o_state[0], o_state[1], 0, cell->units);

    /* h = o * nn.tanh(c) -> o_state[0] for the step 1-2 */
    // 1. tanh(c) -> buf2  --- first copy then activate. 
    nnom_memcpy(buf2, o_state[1], cell->units*2);
	local_tanh_q15(buf2, cell->units, 7 - c->q_dec_c); //  this int bit is under 8bit
    // 2. h = o*tanh(c) -> o_state[0]    ** fill the lower state (memory, hidden)
    local_mult_q15(z[3], buf2, o_state[0], 15, cell->units);

    // copy and shift q15 to q7 ** (copy hidden to output)
    local_q15_to_q7(o_state[0], cell->out_data, 8, cell->units);
	return NN_SUCCESS;
}


// researve for debugging, printing the intermediate products and variables
#if 0
static void print_variable(q7_t* data,char*name, int dec_bit, int size)
{
	printf("\n");
	printf("%s\n", name);
	for(int i = 0; i < size; i++)
	{
		if(i%8==0)
			printf("\n");
		printf("%f\t", (float) data[i] / (1 << dec_bit));
	}
	printf("\n");
}

static void print_variable_q15(q15_t *data,char*name, int dec_bit, int size)
{
	printf("\n\n");
	printf("%s", name);
	for(int i = 0; i < size; i++)
	{
		if(i%8==0)
			printf("\n");
		printf("%f\t", (float) data[i] / (1 << dec_bit));
	}
	printf("\n");
}


// Q7 input output 
// Q7 weights
// Q15 states and intermediate buffer
nnom_status_t lstm_cell_q7_q15_run(nnom_rnn_cell_t* cell)
{
	nnom_layer_t *layer = cell->layer;
	nnom_rnn_layer_t* cl = (nnom_rnn_layer_t *) layer;
	nnom_lstm_cell_t* c = (nnom_lstm_cell_t*) cell;
    int act_int_bit = 7 - c->q_dec_z;
	
				// test
				//nnom_memset(cell->in_data, 32, cell->feature_size); 

    // state buffer
    // low |-- hidden --|-- carry --| high
    q15_t* h_tm1 = (q15_t*)cell->in_state;
    q15_t* c_tm1 = (q15_t*)cell->in_state + cell->units;
    q15_t* o_state[2];
    o_state[0] = (q15_t*)cell->out_state;
    o_state[1] = (q15_t*)cell->out_state + cell->units;

    // computing buffer
    // low |-- buf0 --|-- buf1 --|-- buf2 --|-- input q15 --|
    q15_t* z[4];
    q15_t *buf0, *buf1, *buf2, *in_q15_buf;
    buf0 = (q15_t*)layer->comp->mem->blk;
    buf1 = (q15_t*)layer->comp->mem->blk + cell->units*4;
    buf2 = (q15_t*)layer->comp->mem->blk + cell->units*8;
    in_q15_buf = (q15_t*)layer->comp->mem->blk + cell->units*12;

    // input q7 -> q15
    //local_q7_to_q15_no_shift(cell->in_data, in_q15_buf, cell->feature_size);
    local_q7_to_q15(cell->in_data, in_q15_buf, cell->feature_size);
			print_variable_q15(in_q15_buf, "input", layer->in->tensor->q_dec[0] + 8, cell->feature_size);
			print_variable_q15(h_tm1, "h_tml", 15, cell->units);
			print_variable_q15(c_tm1, "c_tml", c->q_dec_c + 8, cell->units);

    // z1 = K.dot(cell_inputs, kernel) + bias -> buf1
	#ifdef NNOM_USING_CMSIS_NN
		arm_fully_connected_mat_q7_vec_q15_opt
	#else
		local_fully_connected_mat_q7_vec_q15_opt
	#endif 
		(in_q15_buf, c->weights->p_data, cell->feature_size, cell->units*4, c->bias_shift + 8, c->oshift_iw, c->bias->p_data, buf1, NULL);

    // z2 = K.dot(h_tm1, recurrent_kernel)  -> buf2
	// arm version must use bias, so we have to use local implementation
    local_fully_connected_mat_q7_vec_q15_opt(h_tm1, c->recurrent_weights->p_data, 
            cell->units, cell->units*4, 0, c->oshift_hw, NULL, buf2, NULL); 

    // z = z1 + z2  -> buf0
    local_add_q15(buf1, buf2, buf0, 0, cell->units*4);
	
			print_variable_q15(buf0, "z", c->q_dec_z + 8, cell->units*4);
			print_variable_q15(buf1, "z1", c->q_dec_z + 8, cell->units*4);
			print_variable_q15(buf2, "z2", c->q_dec_z + 8, cell->units*4);

    // split the data to each gate
    z[0] = buf0;
    z[1] = buf0 + cell->units;
    z[2] = buf0 + cell->units*2;
    z[3] = buf0 + cell->units*3;

    // i = nn.sigmoid(z0)
    local_sigmoid_q15(z[0], cell->units, act_int_bit);
    // f = nn.sigmoid(z1)
    local_sigmoid_q15(z[1], cell->units, act_int_bit);
    // o = nn.sigmoid(z3)
    local_sigmoid_q15(z[3], cell->units, act_int_bit);
	
			print_variable_q15(z[0], "z[0] - i", 15, cell->units);
			print_variable_q15(z[1], "z[1] - f", 15, cell->units);
			print_variable_q15(z[3], "z[3] - o", 15, cell->units);

    /* c = f * c_tm1 + i * nn.tanh(z2) for the step 1-3. */
    // 1. i * tanh(z2) -> buf1
    local_tanh_q15(z[2], cell->units, act_int_bit);
			print_variable_q15(z[2], "z[2] - ?", 15, cell->units);
	
    local_mult_q15(z[0], z[2], buf1, 30 - (c->q_dec_c+8), cell->units); //q0.15 * q0.15 >> (shift) = (q_c + 8) // i am not very sure
			print_variable_q15(buf1, "c2: i * tanh(z2) ", c->q_dec_c+8, cell->units);

    // 2. f * c_tm1 -> o_state[0] 
    local_mult_q15(z[1], c_tm1, o_state[0], 15, cell->units);
			print_variable_q15(o_state[0], "c1: f * c_tm1", c->q_dec_c+8, cell->units);

    // 3. c = i*tanh + f*c_tm1 -> o_state[1]   ** fill the upper state (carry)
    local_add_q15(buf1, o_state[0], o_state[1], 0, cell->units);
			print_variable_q15(o_state[1], "c = c1+c2", c->q_dec_c+8, cell->units);

    /* h = o * nn.tanh(c) -> o_state[0] for the step 1-2 */
    // 1. tanh(c) -> buf2  --- first copy then activate. 
    nnom_memcpy(buf2, o_state[1], cell->units*2);
	local_tanh_q15(buf2, cell->units, 7 - c->q_dec_c); //  this int bit is under 8bit
			print_variable_q15(buf2, "tanh(c)", 15, cell->units);

    // 2. h = o*tanh(c) -> o_state[0]    ** fill the lower state (memory, hidden)
    local_mult_q15(z[3], buf2, o_state[0], 15, cell->units);
			print_variable_q15(o_state[0], "h = o*tanh(c)", 15, cell->units);

    // copy and shift q15 to q7 ** (copy hidden to output)
    local_q15_to_q7(o_state[0], cell->out_data, 8, cell->units);
	
			print_variable(cell->out_data, "q7 output)", 7, cell->units);

	return NN_SUCCESS;
}
#endif
