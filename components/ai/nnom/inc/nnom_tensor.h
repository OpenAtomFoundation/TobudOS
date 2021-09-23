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
 * 2019-02-10     Jianjia Ma   Compiler supports dense net connection
 */

#ifndef __NNOM_TENSOR_H__
#define __NNOM_TENSOR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "nnom.h"


void delete_tensor(nnom_tensor_t* t);
nnom_tensor_t* new_tensor(nnom_qtype_t type, uint32_t num_dim, uint32_t num_channel);
// set tensor by value
// for tensor with quantized type NNOM_QTYPE_PER_TENSOR
nnom_tensor_t* tensor_set_attr_v(nnom_tensor_t* t, 
		nnom_qformat_param_t dec_bit, nnom_qformat_param_t offset, nnom_shape_data_t* dim, uint32_t num_dim, uint8_t bitwidth);
nnom_tensor_t* tensor_set_attr(nnom_tensor_t* t, 
		nnom_qformat_param_t*dec_bit, nnom_qformat_param_t *offset, nnom_shape_data_t* dim, uint32_t num_dim, uint8_t bitwidth);
nnom_tensor_t* tensor_cpy_attr(nnom_tensor_t* des, nnom_tensor_t* src);
size_t tensor_get_num_channel(nnom_tensor_t* t);
size_t tensor_size(nnom_tensor_t* t);
size_t tensor_size_byte(nnom_tensor_t* t);

// only support 3d tensor
// change format from CHW to HWC
// the shape of the data, input data, output data
void tensor_hwc2chw_q7(nnom_tensor_t* des, nnom_tensor_t* src);

// change format from CHW to HWC
// the shape of the data, input data, output data
void tensor_chw2hwc_q7(nnom_tensor_t* des, nnom_tensor_t* src);

// deprecated. 
void hwc2chw_q7(nnom_3d_shape_t shape, q7_t* p_in, q7_t* p_out);
void chw2hwc_q7(nnom_3d_shape_t shape, q7_t* p_in, q7_t* p_out);

#ifdef __cplusplus
}
#endif

#endif /*__NNOM_TENSOR_H__ */
