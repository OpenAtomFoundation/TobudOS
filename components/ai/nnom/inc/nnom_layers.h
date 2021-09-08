/*
 * Copyright (c) 2018-2020
 * Jianjia Ma
 * majianjia@live.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-02-05     Jianjia Ma   The first version
 */

#ifndef __NNOM_LAYERS_H__
#define __NNOM_LAYERS_H__

#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "nnom.h"

// properties
nnom_3d_shape_t shape(size_t h, size_t w, size_t c);
nnom_3d_shape_t kernel(size_t h, size_t w);
nnom_3d_shape_t stride(size_t h, size_t w);
nnom_3d_shape_t dilation(size_t h, size_t w);
nnom_border_t border(size_t top, size_t bottom, size_t left, size_t right);
//nnom_qformat_t qformat(int8_t m, int8_t n);
size_t shape_size(nnom_3d_shape_t* s);

// this function is to add a new IO to current inited IO
// input, the targeted IO that the new IO will be added to
// output , the new IO
nnom_layer_io_t* io_add_aux(nnom_layer_io_t* targeted_io);
nnom_layer_io_t *io_init(void *owner_layer, nnom_layer_io_t *io);

#define NN_CEILIF(x,y) ((x+y-1)/y)

#include "layers/nnom_activation.h"
#include "layers/nnom_concat.h"
#include "layers/nnom_conv2d.h"
#include "layers/nnom_cropping.h"
#include "layers/nnom_conv2d_trans.h"
#include "layers/nnom_dense.h"
#include "layers/nnom_dw_conv2d.h"
#include "layers/nnom_flatten.h"
#include "layers/nnom_global_pool.h"
#include "layers/nnom_input.h"
#include "layers/nnom_lambda.h"
#include "layers/nnom_matrix.h"
#include "layers/nnom_maxpool.h"
#include "layers/nnom_avgpool.h"
#include "layers/nnom_output.h"
#include "layers/nnom_rnn.h"
#include "layers/nnom_softmax.h"
#include "layers/nnom_sumpool.h"
#include "layers/nnom_upsample.h"
#include "layers/nnom_zero_padding.h"
#include "layers/nnom_rnn.h"
#include "layers/nnom_simple_cell.h"
#include "layers/nnom_lstm_cell.h"
#include "layers/nnom_gru_cell.h"

// Layer APIs ******
// (a summary for each individual layer's files)

// input/output
nnom_layer_t *Input(nnom_3d_shape_t input_shape, void *p_buf);
nnom_layer_t *Output(nnom_3d_shape_t output_shape, void *p_buf);

// Pooling
nnom_layer_t *MaxPool(nnom_3d_shape_t k, nnom_3d_shape_t s, nnom_padding_t pad);
nnom_layer_t *AvgPool(nnom_3d_shape_t k, nnom_3d_shape_t s, nnom_padding_t pad);
nnom_layer_t *SumPool(nnom_3d_shape_t k, nnom_3d_shape_t s, nnom_padding_t pad);
nnom_layer_t *GlobalMaxPool(void);
nnom_layer_t *GlobalAvgPool(void);
nnom_layer_t *GlobalSumPool(void);

// padding, cropping, upsample
nnom_layer_t *UpSample(nnom_3d_shape_t kernel);	
nnom_layer_t *ZeroPadding(nnom_border_t pad);
nnom_layer_t *Cropping(nnom_border_t pad);

// Activation
nnom_layer_t *Activation(nnom_activation_t *act);
nnom_layer_t *ReLU(void);
nnom_layer_t *LeakyReLU(float alpha);
nnom_layer_t *Softmax(void);
nnom_layer_t *Sigmoid(int32_t dec_bit);  // input dec bit
nnom_layer_t *TanH(int32_t dec_bit);     // input dec bit 

// Matrix
nnom_layer_t *Add(int16_t oshift);       // output shift
nnom_layer_t *Sub(int16_t oshift);       // output shift			
nnom_layer_t *Mult(int16_t oshift);      // output shift

nnom_layer_t *Flatten(void);
nnom_layer_t *Concat(int8_t axis);
// -- NN Constructers --
// conv2d
nnom_layer_t *Conv2D(uint32_t filters, nnom_3d_shape_t k, nnom_3d_shape_t s, nnom_3d_shape_t d, nnom_padding_t pad,
					 const nnom_weight_t *w, const nnom_bias_t *b);

// deconv2d
nnom_layer_t *Conv2DTrans(uint32_t filters, nnom_3d_shape_t k, nnom_3d_shape_t s, nnom_3d_shape_t d, nnom_padding_t pad,
					 const nnom_weight_t *w, const nnom_bias_t *b);

// depthwise_convolution
nnom_layer_t *DW_Conv2D(uint32_t multiplier, nnom_3d_shape_t k, nnom_3d_shape_t s, nnom_3d_shape_t d, nnom_padding_t pad,
						const nnom_weight_t *w, const nnom_bias_t *b);

// fully connected, dense
nnom_layer_t *Dense(size_t output_unit, const nnom_weight_t *w, const nnom_bias_t *b);


// Lambda Layers
nnom_layer_t *Lambda(nnom_status_t (*run)(nnom_layer_t *),	// run method, required
					 nnom_status_t (*build)(nnom_layer_t *), // optional, call default_build() if left null
					 nnom_status_t (*free)(nnom_layer_t *),   // not required if no resources needs to be deleted, can be left null.
					 void *parameters);						  // user private parameters for run method, left null if not needed.

// building methods
nnom_status_t default_build(nnom_layer_t* layer);
nnom_status_t input_build(nnom_layer_t* layer);

nnom_status_t conv2d_build(nnom_layer_t* layer);
nnom_status_t dw_conv2d_build(nnom_layer_t* layer);
nnom_status_t conv2d_trans_build(nnom_layer_t* layer);
nnom_status_t dense_build(nnom_layer_t* layer);
nnom_status_t rnn_build(nnom_layer_t* layer);

nnom_status_t upsample_build(nnom_layer_t* layer);
nnom_status_t zero_padding_build(nnom_layer_t* layer);
nnom_status_t cropping_build(nnom_layer_t* layer);

nnom_status_t maxpool_build(nnom_layer_t* layer);
nnom_status_t avgpool_build(nnom_layer_t* layer);
nnom_status_t sumpool_build(nnom_layer_t* layer);
nnom_status_t global_pool_build(nnom_layer_t* layer);

nnom_status_t flatten_build(nnom_layer_t* layer);
nnom_status_t concat_build(nnom_layer_t* layer);

// run
nnom_status_t input_run(nnom_layer_t* layer);
nnom_status_t output_run(nnom_layer_t* layer);
nnom_status_t flatten_run(nnom_layer_t* layer);
nnom_status_t default_run(nnom_layer_t* layer);  // simply copy data from input to output

nnom_status_t dw_conv2d_run(nnom_layer_t* layer);
nnom_status_t conv2d_run(nnom_layer_t* layer);
nnom_status_t conv2d_trans_run(nnom_layer_t* layer);
nnom_status_t dense_run(nnom_layer_t* layer);
nnom_status_t rnn_run(nnom_layer_t* layer);

nnom_status_t upsample_run(nnom_layer_t* layer);
nnom_status_t zero_padding_run(nnom_layer_t* layer);
nnom_status_t cropping_run(nnom_layer_t* layer);

nnom_status_t activation_run(nnom_layer_t* layer);
nnom_status_t softmax_run(nnom_layer_t* layer);

nnom_status_t maxpool_run(nnom_layer_t* layer);
nnom_status_t avgpool_run(nnom_layer_t* layer);
nnom_status_t sumpool_run(nnom_layer_t* layer);

nnom_status_t concat_run(nnom_layer_t* layer);
nnom_status_t add_run(nnom_layer_t* layer);
nnom_status_t sub_run(nnom_layer_t* layer);
nnom_status_t mult_run(nnom_layer_t* layer);

// Activation APIs
// Softmax is not considered as activation in NNoM, Softmax is in layer API.
nnom_activation_t* act_relu(void);
nnom_activation_t* act_leaky_relu(float alpha);
nnom_activation_t* act_sigmoid(int32_t dec_bit);
nnom_activation_t* act_tanh(int32_t dec_bit);

// direct API
nnom_status_t act_tensor_run(nnom_activation_t* act, nnom_tensor_t* tensor);

#ifdef __cplusplus
}
#endif

#endif /* __NNOM_LAYERS_H__ */
