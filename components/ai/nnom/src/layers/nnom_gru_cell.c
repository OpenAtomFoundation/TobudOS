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
#include "layers/nnom_gru_cell.h"

#ifdef NNOM_USING_CMSIS_NN
#include "arm_math.h"
#include "arm_nnfunctions.h"
#endif

nnom_rnn_cell_t *gru_cell_s(const nnom_gru_cell_config_t* config)
{
	nnom_gru_cell_t *cell;
	cell = nnom_mem(sizeof(nnom_gru_cell_t));
	if (cell == NULL)
		return NULL;
	// set methods
	cell->super.run = gru_cell_run;
	cell->super.build = gru_cell_build;
	cell->super.free = gru_cell_free;
	cell->super.config = (void*) config;
	cell->super.units = config->units;
    cell->super.type = NNOM_GRU_CELL;

	// set parameters 
	cell->bias = config->bias;
	cell->weights = config->weights;
	cell->recurrent_weights = config->recurrent_weights;

    // q format for intermediate calculation
    cell->q_dec_h = config->q_dec_h;
    cell->q_dec_z = config->q_dec_z;

	return (nnom_rnn_cell_t *)cell;
}

nnom_status_t gru_cell_free(nnom_rnn_cell_t* cell)
{
	return NN_SUCCESS;
}

// the state buffer and computational buffer shape of the cell
nnom_status_t gru_cell_build(nnom_rnn_cell_t* cell)
{
	nnom_layer_t *layer = cell->layer; 
	nnom_gru_cell_t *c = (nnom_gru_cell_t *)cell;

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
	cell->state_size = cell->units * 2; // Q15

	// comp buffer size: not required
	cell->comp_buf_size = cell->units * (3*3) * 2 + cell->feature_size * 2; //q15 + input q7->q15 buffer.  

	// finally, calculate the MAC for info for each timestamp
	cell->macc = cell->feature_size * cell->units *3 // input: feature * state * 3 gates
				+ cell->units * cell->units *8 // recurrent, state *  output_unit * (5 gate + 3 mult)
				+ cell->units * (3 + 3 + 5);  // 3 gates, 3 mult, 5  addition

	return NN_SUCCESS;
}


// keras implementation as below. 
/*
  def step(cell_inputs, cell_states):
    """Step function that will be used by Keras RNN backend."""
    h_tm1 = cell_states[0]

    # inputs projected by all gate matrices at once
    matrix_x = K.dot(cell_inputs, kernel)
    matrix_x = K.bias_add(matrix_x, input_bias)

    x_z, x_r, x_h = array_ops.split(matrix_x, 3, axis=1)

    # hidden state projected by all gate matrices at once
    matrix_inner = K.dot(h_tm1, recurrent_kernel)
    matrix_inner = K.bias_add(matrix_inner, recurrent_bias)

    recurrent_z, recurrent_r, recurrent_h = array_ops.split(matrix_inner, 3,
                                                            axis=1)
    z = nn.sigmoid(x_z + recurrent_z)
    r = nn.sigmoid(x_r + recurrent_r)
    hh = nn.tanh(x_h + r * recurrent_h)

    # previous and candidate state mixed by update gate
    h = z * h_tm1 + (1 - z) * hh
    return h, [h]
*/

//
nnom_status_t gru_cell_run(nnom_rnn_cell_t* cell)
{
	nnom_layer_t *layer = cell->layer;
	nnom_gru_cell_t* c = (nnom_gru_cell_t*) cell;
    int act_int_bit = 7 - c->q_dec_z;
	// gate data    
    q15_t* x_z, *x_r, *x_h;
    q15_t* recurrent_z, *recurrent_r, *recurrent_h;
	q15_t* temp[3];

    // bias
    q7_t* bias = (q7_t*)c->bias->p_data;
    q7_t* recurrent_bias = (q7_t*)c->bias->p_data + cell->units*3;

    // state buffer
    q15_t* h_tm1 = (q15_t*)cell->in_state;
    q15_t* h_t = (q15_t*)cell->out_state;

    // computing buffer
    // low |-- buf0 --|-- buf1 --|-- buf2 --|-- input_q15 --|
    q15_t *buf[3];
    buf[0] = (q15_t*)layer->comp->mem->blk;
    buf[1] = (q15_t*)layer->comp->mem->blk + cell->units*3;
    buf[2] = (q15_t*)layer->comp->mem->blk + cell->units*6;
    q15_t *in_q15_buf = (q15_t*)layer->comp->mem->blk + cell->units*9;

    // input q7 cast to q15
    local_q7_to_q15(cell->in_data, in_q15_buf, cell->feature_size);

    // matrix_x = K.dot(cell_inputs, kernel) + bias  --> buf0
	#ifdef NNOM_USING_CMSIS_NN
		arm_fully_connected_mat_q7_vec_q15_opt
	#else
		local_fully_connected_mat_q7_vec_q15_opt
	#endif 
		(in_q15_buf, c->weights->p_data, cell->feature_size, 
			cell->units*3, c->bias_shift + 8, c->oshift_iw, bias, buf[0], NULL);

    // matrix_intter = K.dot(h_tm1, recurrent_kernel) + bias -> buf1
    #ifdef NNOM_USING_CMSIS_NN
		arm_fully_connected_mat_q7_vec_q15_opt
	#else
		local_fully_connected_mat_q7_vec_q15_opt
	#endif 
		(h_tm1, c->recurrent_weights->p_data, cell->units, 
			cell->units*3,  c->bias_shift + 8, c->oshift_hw, recurrent_bias, buf[1], NULL); 

	// split to each gate
    x_z = buf[0];
    x_r = buf[0] + cell->units;
    x_h = buf[0] + cell->units*2;
    recurrent_z = buf[1];
    recurrent_r = buf[1] + cell->units;
    recurrent_h = buf[1] + cell->units*2;
	// buffers
    temp[0] = buf[2];
    temp[1] = buf[2] + cell->units;
    temp[2] = buf[2] + cell->units*2;

    /* z = nn.sigmoid(x_z + recurrent_z) */
    // 1.  z1 = x_z + recurrent_z    --->  temp[0]
    local_add_q15(x_z, recurrent_z, temp[0], 0, cell->units);
    // 2.  z = sigmoid(z1)
    local_sigmoid_q15(temp[0], cell->units, act_int_bit);

    /* r = nn.sigmoid(x_r + recurrent_r) */
    // 1.  r1 = x_r + recurrent_r    --->  temp[1]
    local_add_q15(x_r, recurrent_r, temp[1], 0, cell->units);
    // 2.  r = sigmoid(r1)
    local_sigmoid_q15(temp[1], cell->units, act_int_bit);

    /* hh = nn.tanh(x_h + r * recurrent_h) */
    // 1.  hh1 = r * recurrent_h     ---> temp[2]
    local_mult_q15(temp[1], recurrent_h, temp[2], 15, cell->units);
    // 2.  hh2 = x_h + hh1            ---> temp[1]
    local_add_q15(x_h, temp[2], temp[1], 0, cell->units);
    // 3.  hh = tanh(h2)           ---> temp[1]
    local_tanh_q15(temp[1], cell->units, act_int_bit);

    /* h = z * h_tm1 + (1 - z) * hh  */
    // 1. h1 = z*h_tm1   ---> temp[2]
    local_mult_q15(temp[0], h_tm1, temp[2], 15, cell->units);
    // 2. h2 = 1 - z            ---> h_t state buff
    local_1_minor_z_q15(temp[0], h_t, 15, cell->units);
    // 3. h3 = h2 * hh          ---> temp[0]
    local_mult_q15(h_t, temp[1],  temp[0], 15, cell->units);
    // h = h1 + h3
    local_add_q15(temp[2], temp[0], h_t, 0, cell->units);

    // finally, copy and convert state to output
    local_q15_to_q7(h_t, cell->out_data, 8, cell->units);
	return NN_SUCCESS;
}


// Researve for debugging, printing the intermediate variables/data.
#if 0
// delete after testing completed
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

//
nnom_status_t gru_cell_run(nnom_rnn_cell_t* cell)
{
	nnom_layer_t *layer = cell->layer;
	nnom_gru_cell_t* c = (nnom_gru_cell_t*) cell;
    int act_int_bit = 7 - c->q_dec_z;
	// gate data    
    q15_t* x_z, *x_r, *x_h;
    q15_t* recurrent_z, *recurrent_r, *recurrent_h;
	q15_t* temp[3];
	
    	 		// test
	 			//nnom_memset(cell->in_data, 5 * (1<<layer->in->tensor->q_dec[0]), cell->feature_size); 

    // bias
    q7_t* bias = (q7_t*)c->bias->p_data;
    q7_t* recurrent_bias = (q7_t*)c->bias->p_data + cell->units*3;

    // state buffer
    q15_t* h_tm1 = (q15_t*)cell->in_state;
    q15_t* h_t = (q15_t*)cell->out_state;

    // computing buffer
    // low |-- buf0 --|-- buf1 --|-- buf2 --|-- input_q15 --|
    q15_t *buf[3];
    buf[0] = (q15_t*)layer->comp->mem->blk;
    buf[1] = (q15_t*)layer->comp->mem->blk + cell->units*3;
    buf[2] = (q15_t*)layer->comp->mem->blk + cell->units*6;
    q15_t *in_q15_buf = (q15_t*)layer->comp->mem->blk + cell->units*9;

    // input q7 cast to q15
    local_q7_to_q15(cell->in_data, in_q15_buf, cell->feature_size);

    // matrix_x = K.dot(cell_inputs, kernel) + bias  --> buf0
	#ifdef NNOM_USING_CMSIS_NN
		arm_fully_connected_mat_q7_vec_q15_opt
	#else
		local_fully_connected_mat_q7_vec_q15_opt
	#endif 
		(in_q15_buf, c->weights->p_data, cell->feature_size, 
			cell->units*3, c->bias_shift + 8, c->oshift_iw, bias, buf[0], NULL);

    // matrix_intter = K.dot(h_tm1, recurrent_kernel) + bias -> buf1
    #ifdef NNOM_USING_CMSIS_NN
		arm_fully_connected_mat_q7_vec_q15_opt
	#else
		local_fully_connected_mat_q7_vec_q15_opt
	#endif 
		(h_tm1, c->recurrent_weights->p_data, cell->units, 
			cell->units*3,  c->bias_shift + 8, c->oshift_hw, recurrent_bias, buf[1], NULL); 
			
			print_variable_q15(in_q15_buf, "input", layer->in->tensor->q_dec[0]+8, cell->feature_size);
			print_variable_q15(buf[0], "matrix_x", c->q_dec_z+8, cell->units*3);
			print_variable_q15(buf[1], "matrix_recurrent", 	c->q_dec_z+8, cell->units*3);

	// split to each gate
    x_z = buf[0];
    x_r = buf[0] + cell->units;
    x_h = buf[0] + cell->units*2;
    recurrent_z = buf[1];
    recurrent_r = buf[1] + cell->units;
    recurrent_h = buf[1] + cell->units*2;
	// buffers
    temp[0] = buf[2];
    temp[1] = buf[2] + cell->units;
    temp[2] = buf[2] + cell->units*2;

    // z = nn.sigmoid(x_z + recurrent_z) 
    // 1.  z1 = x_z + recurrent_z    --->  temp[0]
    local_add_q15(x_z, recurrent_z, temp[0], 0, cell->units);
    // 2.  z = sigmoid(z1)
    local_sigmoid_q15(temp[0], cell->units, act_int_bit);
		print_variable_q15(temp[0], "z", 15, cell->units);

    // r = nn.sigmoid(x_r + recurrent_r) 
    // 1.  r1 = x_r + recurrent_r    --->  temp[1]
    local_add_q15(x_r, recurrent_r, temp[1], 0, cell->units);
    // 2.  r = sigmoid(r1)
    local_sigmoid_q15(temp[1], cell->units, act_int_bit);
		print_variable_q15(temp[1], "r", 15, cell->units);

    // hh = nn.tanh(x_h + r * recurrent_h) 
    // 1.  hh1 = r * recurrent_h     ---> temp[2]
    local_mult_q15(temp[1], recurrent_h, temp[2], 15, cell->units);
    // 2.  hh2 = x_h + h1            ---> temp[1]
    local_add_q15(x_h, temp[2], temp[1], 0, cell->units);
    // 3.  hh = tanh(h2)           ---> temp[1]
    local_tanh_q15(temp[1], cell->units, act_int_bit);
		print_variable_q15(temp[1], "hh", 15, cell->units);

    // h = z * h_tm1 + (1 - z) * hh  
    // 1. h1 = z*h_tm1   ---> temp[2]
    local_mult_q15(temp[0], h_tm1, temp[2], 15, cell->units);
		print_variable_q15( temp[2], "h1", 15, cell->units);
    // 2. h2 = 1 - z            ---> h_t state buff
    local_1_minor_z_q15(temp[0], h_t, 15, cell->units);
		print_variable_q15( h_t, "h2", 15, cell->units);
    // 3. h3 = h2 * hh          ---> temp[0]
    local_mult_q15(h_t, temp[1],  temp[0], 15, cell->units);
		print_variable_q15( temp[0], "h3", 15, cell->units);
    // h = h1 + h3
    local_add_q15(temp[2], temp[0], h_t, 0, cell->units);
		print_variable_q15(h_t, "h", 15, cell->units);

    // finally, copy and convert state to output
    local_q15_to_q7(h_t, cell->out_data, 8, cell->units);
	return NN_SUCCESS;
}
#endif
