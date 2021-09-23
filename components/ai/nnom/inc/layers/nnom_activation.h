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

#ifndef __NNOM_ACTIVATION_H__
#define __NNOM_ACTIVATION_H__

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


// activation layer
typedef struct _nnom_activation_layer_t
{
	nnom_layer_t super;
	nnom_activation_t *act; 
} nnom_activation_layer_t;


// activation with fixed q format (tanh and sigmoid)
typedef struct _nnom_activation_fixed_q_t
{
	nnom_activation_t super;
	uint8_t dec_bit;
} nnom_activation_fixed_q_t;

// leaky relu
typedef struct _nnom_activation_leaky_relu_t
{
	nnom_activation_t super;
	q7_t alpha;					// alpha is present by q0.7 format. (-128 = -1) 
} nnom_activation_leaky_relu_t;

// advance relu (full ReLU)
typedef struct _nnom_activation_adv_relu_t
{
	nnom_activation_t super;
	q7_t negative_slope;			// negative_slope is present by q0.7 format. (-128 = -1) 
	float max;						// cap of the max value
	float threshold;				// threshold
} nnom_activation_adv_relu_t;

// method
nnom_status_t activation_run(nnom_layer_t* layer);
nnom_status_t activation_free(nnom_layer_t *layer);

// activation delete
void act_delete(nnom_activation_t* act);

// a direct api on tensor
nnom_status_t act_tensor_run(nnom_activation_t* act, nnom_tensor_t* tensor);


// Layer API
nnom_layer_t *Activation(nnom_activation_t *act);
nnom_layer_t *ReLU(void);
nnom_layer_t *LeakyReLU(float alpha);
nnom_layer_t *AdvReLU(float alpha, float max, float threshold);
nnom_layer_t *Sigmoid(int32_t dec_bit);
nnom_layer_t *TanH(int32_t dec_bit);

// Activation API. 
nnom_activation_t* act_relu(void);
nnom_activation_t* act_leaky_relu(float alpha);
nnom_activation_t* act_adv_relu(float negative_slope, float max, float threshold);
nnom_activation_t* act_tanh(int32_t dec_bit);
nnom_activation_t* act_sigmoid(int32_t dec_bit);
nnom_activation_t* act_hard_tanh(int32_t dec_bit);
nnom_activation_t* act_hard_sigmoid(int32_t dec_bit);

// utils
int32_t act_get_dec_bit(nnom_activation_type_t type, int32_t dec_bit);

#ifdef __cplusplus
}
#endif

#endif /* __NNOM_ACTIVATION_H__ */
