/*
 * Copyright (c) 2018-2020
 * Jianjia Ma
 * majianjia@live.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-08-20     Jianjia Ma   The first version
 */

#ifndef __NNOM_SIMPLE_CELL_H__
#define __NNOM_SIMPLE_CELL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "nnom_rnn.h"
#include "nnom_activation.h"


// This Simple Cell replicate the Keras's SimpleCell as blow 
/*
 def call(self, inputs, states, training=None):
    prev_output = states[0] if nest.is_sequence(states) else states

	h = K.dot(inputs, self.kernel)
	h = K.bias_add(h, self.bias)

    output = h + K.dot(prev_output, self.recurrent_kernel)
    output = self.activation(output)

    new_state = [output] if nest.is_sequence(states) else output
    return output, new_state
*/

// a machine interface for configuration
typedef struct _nnom_simple_cell_config_t
{
	nnom_layer_config_t super;
	nnom_tensor_t *weights;
	nnom_tensor_t* recurrent_weights;
	nnom_tensor_t *bias;
	nnom_qformat_param_t q_dec_iw, q_dec_hw, q_dec_h;
	nnom_activation_type_t act_type;		// type of the activation
	uint16_t units;
} nnom_simple_cell_config_t;


typedef struct _nnom_simple_cell_t
{
	nnom_rnn_cell_t super;
	nnom_activation_type_t act_type;

	nnom_tensor_t* weights;
	nnom_tensor_t* recurrent_weights;
	nnom_tensor_t* bias;

	// experimental, 
	// iw: input x weight
	// hw: hidden state x recurrent weight
	// h: hidden state
	nnom_qformat_param_t q_dec_iw, q_dec_hw, q_dec_h;
	nnom_qformat_param_t oshift_iw, oshift_hw, bias_shift;
	
} nnom_simple_cell_t;


// RNN cells
// The shape for RNN input is (batch, timestamp, feature), where batch is always 1. 
//
// SimpleCell
nnom_rnn_cell_t *simple_cell_s(const nnom_simple_cell_config_t* config);

nnom_status_t simple_cell_free(nnom_rnn_cell_t* cell);
nnom_status_t simple_cell_build(nnom_rnn_cell_t* cell);
nnom_status_t simple_cell_run(nnom_rnn_cell_t* cell);


#ifdef __cplusplus
}
#endif

#endif /* __NNOM_SIMPLE_CELL_H__ */
