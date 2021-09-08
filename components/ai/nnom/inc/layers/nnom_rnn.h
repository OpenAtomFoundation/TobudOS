/*
 * Copyright (c) 2018-2020
 * Jianjia Ma
 * majianjia@live.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-05-03     Jianjia Ma   The first version
 */

#ifndef __NNOM_RNN_H__
#define __NNOM_RNN_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "nnom.h"
#include "nnom_layers.h"
#include "nnom_local.h"
#include "nnom_tensor.h"

// a machine interface for configuration
typedef struct _nnom_rnn_config_t
{
	nnom_layer_config_t super;
	bool return_sequence;
	bool stateful;
	bool go_backwards;
} nnom_rnn_config_t;

// RNN cell base type
typedef struct _nnom_rnn_cell_t
{
	nnom_status_t (*run)(struct _nnom_rnn_cell_t* cell); // cell runner
	nnom_status_t (*build)(struct _nnom_rnn_cell_t* cell); // cell builder, calculate buffer size, output data size
	nnom_status_t (*free)(struct _nnom_rnn_cell_t* cell); // 
	nnom_layer_t *layer;				// pointer to its layer holder
	nnom_layer_config_t *config;		// config for the cell event it is a layer type		
	nnom_rnn_cell_type_t type;	

	void *in_data;						// input data
	void *out_data;						// output data
	void *in_state;					// input state data (or hidden state)
	void *out_state;				// output state data

	size_t comp_buf_size;			// the size of temporary buffer. 
	size_t state_size; 				// the size of hidden state
	uint16_t units;					// the output units 
	uint16_t feature_size;			// the input feature size (vector size)

	size_t macc; // stat of MAC count. 
} nnom_rnn_cell_t;

typedef struct _nnom_rnn_layer_t
{
	nnom_layer_t super;
	nnom_rnn_cell_t *cell;
	void *state_buf;		// memory allocated to store state, size = 2 x size of state required by cell. 

	uint16_t timestamp_size;// size of timestamp
	bool return_sequence; 	// whether to return the output for each unit (sequence)
	bool stateful;			// whether the states are kept after one inteference
	bool go_backwards;		// whether go backwards timestamping
} nnom_rnn_layer_t;


// rnn layer 
nnom_layer_t *rnn_s(nnom_rnn_cell_t *cell, const nnom_rnn_config_t* config);

nnom_status_t rnn_run(nnom_layer_t* layer);
nnom_status_t rnn_build(nnom_layer_t* layer);
nnom_status_t rnn_free(nnom_layer_t* layer);

#ifdef __cplusplus
}
#endif

#endif /* __NNOM_RNN_H__ */
