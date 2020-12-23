/*
 * Copyright (C) 2010-2020 Arm Limited or its affiliates. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* ----------------------------------------------------------------------
 * Project:      CMSIS NN Library
 * Title:        arm_convolve_1x1_s8_fast.c
 * Description:  Fast q7 version of 1x1 convolution (non-square shape)
 *
 * $Date:        May 29, 2020
 * $Revision:    V.2.0.1
 *
 * Target Processor:  Cortex-M cores
 *
 * -------------------------------------------------------------------- */

#include "cmsis/CMSIS/NN/Include/arm_nnfunctions.h"
#include "cmsis/CMSIS/NN/Include/arm_nn_types.h"

#define DIM_KER_X (1U)
#define DIM_KER_Y (1U)

/**
 *  @ingroup groupNN
 */

/**
 * @addtogroup NNConv
 * @{
 */

/*
   * Fast s8 version for 1x1 convolution (non-square shape)
   *
   * Refer header file for details.
   *
   */

arm_status arm_convolve_1x1_s8_fast(const cmsis_nn_context *ctx,
                                    const cmsis_nn_conv_params *conv_params,
                                    const cmsis_nn_per_channel_quant_params *quant_params,
                                    const cmsis_nn_dims *input_dims,
                                    const q7_t *input_data,
                                    const cmsis_nn_dims *filter_dims,
                                    const q7_t *filter_data,
                                    const cmsis_nn_dims *bias_dims,
                                    const int32_t *bias_data,
                                    const cmsis_nn_dims *output_dims,
                                    q7_t *output_data)
{
    if (input_dims->c % 4 != 0 ||
        conv_params->padding.w != 0 || conv_params->padding.h != 0 ||
        conv_params->stride.w != 1 || conv_params->stride.h != 1)
    {
        return ARM_MATH_SIZE_MISMATCH;
    }

    (void)ctx;
    (void)filter_dims;
    (void)bias_dims;

#if defined(ARM_MATH_MVEI)

    const int32_t col_len       = input_dims->w * input_dims->h * input_dims->n;
    const int32_t output_ch     = output_dims->c;
    const int32_t input_ch      = input_dims->c;
    const int32_t input_offset  = conv_params->input_offset;
    const int32_t out_offset    = conv_params->output_offset;
    const int32_t out_activation_min = conv_params->activation.min;
    const int32_t out_activation_max = conv_params->activation.max;
    int32_t *output_mult        = quant_params->multiplier;
    int32_t *output_shift       = quant_params->shift;

    for (int i_items = 0; i_items <= (col_len - 4); i_items += 4)
    {
        for (int i_out_ch = 0; i_out_ch < output_ch; i_out_ch++)
        {
            int32_t sum_row = 0;
            int32_t temp_out[4];

            (void)arm_nn_mat_mul_core_4x_s8(input_ch,
                                            input_ch,
                                            input_data + i_items * input_ch,
                                            filter_data + i_out_ch * input_ch,
                                            &sum_row,
                                            temp_out);
            int32x4_t res = vldrwq_s32(temp_out);

            res = vaddq_n_s32(res, bias_data[i_out_ch]);
            sum_row = sum_row * input_offset;
            res = vaddq_n_s32(res, sum_row);
            res = arm_requantize_mve(res, output_mult[i_out_ch], output_shift[i_out_ch]);
            res = vaddq_n_s32(res, out_offset);

            res = vmaxq_s32(res, vdupq_n_s32(out_activation_min));
            res = vminq_s32(res, vdupq_n_s32(out_activation_max));

            const uint32x4_t scatter_offset = {0, (uint32_t)output_ch,
                                               (uint32_t)output_ch * 2,
                                               (uint32_t)output_ch * 3};
            vstrbq_scatter_offset_s32(output_data, scatter_offset, res);
            output_data++;
        }
        output_data += (3 * output_ch);
    }

    /* Handle left over elements */
    for (int i_items = (col_len & ~0x3); i_items < col_len; i_items++)
    {
        for (int i_out_ch = 0; i_out_ch < output_ch; i_out_ch++)
        {
            int32_t sum_row = 0;

            int32_t acc;
            (void)arm_nn_mat_mul_core_1x_s8(input_ch,
                                            input_data + i_items * input_ch,
                                            filter_data + i_out_ch * input_ch,
                                            &sum_row,
                                            &acc);

            acc += bias_data[i_out_ch];
            sum_row = (sum_row * input_offset);
            acc += sum_row;
            acc = arm_nn_requantize(acc, output_mult[i_out_ch], output_shift[i_out_ch]);
            acc += out_offset;

            acc = MAX(acc, out_activation_min);
            acc = MIN(acc, out_activation_max);
            *output_data++ = acc;
        }
    }

#else
    /* Run the following code as reference implementation for Cortex-M processors with or without DSP extension */

    const int32_t lhs_rows = input_dims->w * input_dims->h * input_dims->n;
    const int32_t rhs_rows = output_dims->c;
    const int32_t rhs_cols = input_dims->c;

    arm_nn_mat_mult_nt_t_s8(input_data,
                            filter_data,
                            bias_data,
                            output_data,
                            quant_params->multiplier,
                            quant_params->shift,
                            lhs_rows,
                            rhs_rows,
                            rhs_cols,
                            conv_params->input_offset,
                            conv_params->output_offset,
                            conv_params->activation.min,
                            conv_params->activation.max);

#endif

    /* Return to application */
    return ARM_MATH_SUCCESS;
}

int32_t arm_convolve_1x1_s8_fast_get_buffer_size(const cmsis_nn_dims *input_dims)
{
    (void)input_dims;
    return 0;
}

/**
 * @} end of NNConv group
 */
