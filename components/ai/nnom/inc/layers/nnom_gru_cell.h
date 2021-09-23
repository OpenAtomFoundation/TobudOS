/*
 * Copyright (c) 2018-2020
 * Jianjia Ma
 * majianjia@live.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-08-27     Jianjia Ma   The first version
 */

#ifndef __NNOM_GRU_CELL_H__
#define __NNOM_GRU_CELL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "nnom_rnn.h"
#include "nnom_activation.h"

typedef struct _nnom_gru_cell_config_t
{
	nnom_layer_config_t super;
	nnom_tensor_t *weights;
	nnom_tensor_t* recurrent_weights;
	nnom_tensor_t *bias;
	nnom_qformat_param_t q_dec_z, q_dec_h; // z, r, h
	uint16_t units;
} nnom_gru_cell_config_t;


typedef struct _nnom_gru_cell_t
{
	nnom_rnn_cell_t super;

	nnom_tensor_t* weights;
	nnom_tensor_t* recurrent_weights;
	nnom_tensor_t* bias;

    // decide later. 
    // z, r, h
	nnom_qformat_param_t q_dec_z, q_dec_h;
	nnom_qformat_param_t oshift_iw, oshift_hw, bias_shift;

} nnom_gru_cell_t;

// gru
nnom_rnn_cell_t *gru_cell_s(const nnom_gru_cell_config_t* config);

nnom_status_t gru_cell_free(nnom_rnn_cell_t* cell);
nnom_status_t gru_cell_build(nnom_rnn_cell_t* cell);
nnom_status_t gru_cell_run(nnom_rnn_cell_t* cell);

#ifdef __cplusplus
}
#endif

#endif /* __NNOM_GRU_CELL_H__ */
