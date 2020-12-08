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
 * Title:        arm_nn_mat_mult_kernel_s8_s16.c
 * Description:  Matrix-multiplication function for convolution
 *
 * $Date:        May 29, 2020
 * $Revision:    V.1.0.2
 *
 * Target Processor:  Cortex-M cores
 * -------------------------------------------------------------------- */

#include "cmsis/CMSIS/DSP/Include/arm_math.h"
#include "cmsis/CMSIS/NN/Include/arm_nnfunctions.h"

/*
   * Matrix-multiplication function for convolution with per-channel requantization.
   *
   * Refer header file for details.
   *
   */

q7_t *arm_nn_mat_mult_kernel_s8_s16(const q7_t *input_a,
                                    const q15_t *input_b,
                                    const uint16_t output_ch,
                                    const int32_t *out_shift,
                                    const int32_t *out_mult,
                                    const int32_t out_offset,
                                    const int16_t activation_min,
                                    const int16_t activation_max,
                                    const uint16_t num_col_a,
                                    const int32_t *const output_bias,
                                    q7_t *out_0)
{
#if defined(ARM_MATH_MVEI)
#define ROW_PER_LOOP (4)
#define COL_PER_LOOP (8)

    const q7_t *ip_a0_s8 = input_a;
    q7_t *out_1 = out_0 + output_ch;

    const int32_t *bias = output_bias;

    int32_t row_count = output_ch / ROW_PER_LOOP;

    while (row_count)
    {
        const q15_t *ip_b0_s16 = input_b;
        const q15_t *ip_b1_s16 = input_b + num_col_a;

        const q7_t *ip_a1_s8 = ip_a0_s8 + num_col_a;
        const q7_t *ip_a2_s8 = ip_a0_s8 + num_col_a * 2;
        const q7_t *ip_a3_s8 = ip_a0_s8 + num_col_a * 3;

        q31_t ch_0_out_n = bias[0];
        q31_t ch_1_out_n = bias[1];
        q31_t ch_2_out_n = bias[2];
        q31_t ch_3_out_n = bias[3];

        q31_t ch_0_out_n1 = ch_0_out_n;
        q31_t ch_1_out_n1 = ch_1_out_n;
        q31_t ch_2_out_n1 = ch_2_out_n;
        q31_t ch_3_out_n1 = ch_3_out_n;
        bias += 4;

        int32_t col_count = num_col_a / COL_PER_LOOP;

        while (col_count)
        {
            // Load inputs
            const int16x8_t ip_b0 = vld1q_s16(ip_b0_s16);
            ip_b0_s16 += COL_PER_LOOP;
            const int16x8_t ip_b1 = vld1q_s16(ip_b1_s16);
            ip_b1_s16 += COL_PER_LOOP;

            // Load filters
            const int16x8_t ip_a0 = vldrbq_s16(ip_a0_s8);
            ip_a0_s8 += COL_PER_LOOP;
            const int16x8_t ip_a1 = vldrbq_s16(ip_a1_s8);
            ip_a1_s8 += COL_PER_LOOP;
            const int16x8_t ip_a2 = vldrbq_s16(ip_a2_s8);
            ip_a2_s8 += COL_PER_LOOP;
            const int16x8_t ip_a3 = vldrbq_s16(ip_a3_s8);
            ip_a3_s8 += COL_PER_LOOP;

            // MAC
            ch_0_out_n += vmladavq_s16(ip_b0, ip_a0);
            ch_1_out_n += vmladavq_s16(ip_b0, ip_a1);
            ch_2_out_n += vmladavq_s16(ip_b0, ip_a2);
            ch_3_out_n += vmladavq_s16(ip_b0, ip_a3);
            ch_0_out_n1 += vmladavq_s16(ip_b1, ip_a0);
            ch_1_out_n1 += vmladavq_s16(ip_b1, ip_a1);
            ch_2_out_n1 += vmladavq_s16(ip_b1, ip_a2);
            ch_3_out_n1 += vmladavq_s16(ip_b1, ip_a3);

            col_count--;
        }

        /* Handle tail */
        col_count = (num_col_a & (COL_PER_LOOP - 1)) - 1;
        while (col_count >= 0)
        {
            const int32_t b0 = ip_b0_s16[col_count];
            const int32_t b1 = ip_b1_s16[col_count];

            ch_0_out_n += b0 * ip_a0_s8[col_count];
            ch_1_out_n += b0 * ip_a1_s8[col_count];
            ch_2_out_n += b0 * ip_a2_s8[col_count];
            ch_3_out_n += b0 * ip_a3_s8[col_count];

            ch_0_out_n1 += b1 * ip_a0_s8[col_count];
            ch_1_out_n1 += b1 * ip_a1_s8[col_count];
            ch_2_out_n1 += b1 * ip_a2_s8[col_count];
            ch_3_out_n1 += b1 * ip_a3_s8[col_count];
            col_count--;
        }
        ip_a0_s8 += (num_col_a & (COL_PER_LOOP - 1));

        int32x4_t out_vec_0;
        int32x4_t out_vec_1;
        out_vec_0[0] = ch_0_out_n;
        out_vec_0[1] = ch_1_out_n;
        out_vec_0[2] = ch_2_out_n;
        out_vec_0[3] = ch_3_out_n;

        out_vec_1[0] = ch_0_out_n1;
        out_vec_1[1] = ch_1_out_n1;
        out_vec_1[2] = ch_2_out_n1;
        out_vec_1[3] = ch_3_out_n1;

        int32x4_t mult = vldrwq_s32(out_mult);
        int32x4_t shift = vldrwq_s32(out_shift);
        out_mult += ROW_PER_LOOP;
        out_shift += ROW_PER_LOOP;

        out_vec_0 = arm_requantize_mve_32x4(out_vec_0, mult, shift);
        out_vec_1 = arm_requantize_mve_32x4(out_vec_1, mult, shift);

        out_vec_0 = vaddq_n_s32(out_vec_0, out_offset);
        out_vec_0 = vmaxq_s32(out_vec_0, vdupq_n_s32(activation_min));
        out_vec_0 = vminq_s32(out_vec_0, vdupq_n_s32(activation_max));
        vstrbq_s32(out_0, out_vec_0);
        out_0 += ROW_PER_LOOP;

        out_vec_1 = vaddq_n_s32(out_vec_1, out_offset);
        out_vec_1 = vmaxq_s32(out_vec_1, vdupq_n_s32(activation_min));
        out_vec_1 = vminq_s32(out_vec_1, vdupq_n_s32(activation_max));
        vstrbq_s32(out_1, out_vec_1);
        out_1 += ROW_PER_LOOP;
        row_count--;
        ip_a0_s8 += (num_col_a * 3);
    }

    row_count = output_ch & (ROW_PER_LOOP - 1);

    if (row_count)
    {
        ip_a0_s8 = input_a + num_col_a * (output_ch & ~3);
        const mve_pred16_t p = vctp32q((uint32_t)row_count);
        int32x4_t out_vec_0 = vdupq_n_s32(0);
        int32x4_t out_vec_1 = vdupq_n_s32(0);
        int32x4_t mult_tail;
        int32x4_t shift_tail;

        for (int i_ch = 0; i_ch < row_count; i_ch++)
        {
            int32_t output_0 = bias[i_ch];
            int32_t output_1 = bias[i_ch];
            const q15_t *ip_b0_s16 = input_b;
            const q15_t *ip_b1_s16 = input_b + num_col_a;

            for (int i_idx = 0; i_idx < num_col_a; i_idx++)
            {
                output_0 += ip_b0_s16[i_idx] * ip_a0_s8[i_idx];
                output_1 += ip_b1_s16[i_idx] * ip_a0_s8[i_idx];
            }

            ip_a0_s8 += num_col_a;
            out_vec_0[i_ch] = output_0;
            out_vec_1[i_ch] = output_1;
            mult_tail[i_ch] = out_mult[i_ch];
            shift_tail[i_ch] = out_shift[i_ch];
        }
        out_vec_0 = arm_requantize_mve_32x4(out_vec_0, mult_tail, shift_tail);
        out_vec_1 = arm_requantize_mve_32x4(out_vec_1, mult_tail, shift_tail);

        out_vec_0 = vaddq_n_s32(out_vec_0, out_offset);
        out_vec_0 = vmaxq_s32(out_vec_0, vdupq_n_s32(activation_min));
        out_vec_0 = vminq_s32(out_vec_0, vdupq_n_s32(activation_max));
        vstrbq_p_s32(out_0, out_vec_0, p);

        out_vec_1 = vaddq_n_s32(out_vec_1, out_offset);
        out_vec_1 = vmaxq_s32(out_vec_1, vdupq_n_s32(activation_min));
        out_vec_1 = vminq_s32(out_vec_1, vdupq_n_s32(activation_max));

        vstrbq_p_s32(out_1, out_vec_1, p);
        out_1 += row_count;
    }

    return out_1;

#elif defined(ARM_MATH_DSP)
    /* set up the second output pointers */
    q7_t *out_1 = out_0 + output_ch;
    const int32_t *bias = output_bias;

    uint16_t row_count = output_ch / 2;
    const q7_t *ip_a0 = input_a;
    /* this loop over rows in A */
    while (row_count)
    {
        /* setup pointers for B */
        const q15_t *ip_b0 = input_b;
        const q15_t *ip_b1 = ip_b0 + num_col_a;

        /* align the second pointer for A */
        const q7_t *ip_a1 = ip_a0 + num_col_a;

        /* Init accumulator with bias for channel N and N + 1 */
        q31_t ch_0_out_0 = *bias;
        q31_t ch_0_out_1 = *bias++;
        q31_t ch_1_out_0 = *bias;
        q31_t ch_1_out_1 = *bias++;

        uint16_t col_count = num_col_a / 4;
        /* accumulate over the vector */
        while (col_count)
        {
            q31_t a01, a02, a11, a12;
            q31_t b0 = arm_nn_read_q15x2_ia(&ip_b0);
            q31_t b1 = arm_nn_read_q15x2_ia(&ip_b1);

            ip_a0 = read_and_pad(ip_a0, &a01, &a02);
            ip_a1 = read_and_pad(ip_a1, &a11, &a12);

            ch_0_out_0 = __SMLAD(a01, b0, ch_0_out_0);
            ch_0_out_1 = __SMLAD(a01, b1, ch_0_out_1);
            ch_1_out_0 = __SMLAD(a11, b0, ch_1_out_0);
            ch_1_out_1 = __SMLAD(a11, b1, ch_1_out_1);

            b0 = arm_nn_read_q15x2_ia(&ip_b0);
            b1 = arm_nn_read_q15x2_ia(&ip_b1);

            ch_0_out_0 = __SMLAD(a02, b0, ch_0_out_0);
            ch_0_out_1 = __SMLAD(a02, b1, ch_0_out_1);
            ch_1_out_0 = __SMLAD(a12, b0, ch_1_out_0);
            ch_1_out_1 = __SMLAD(a12, b1, ch_1_out_1);

            col_count--;
        } /* while over col_count */
        col_count = num_col_a & 0x3;
        while (col_count)
        {
            q7_t a0 = *ip_a0++;
            q15_t b0 = *ip_b0++;
            q7_t a1 = *ip_a1++;
            q15_t b1 = *ip_b1++;

            ch_0_out_0 += a0 * b0;
            ch_0_out_1 += a0 * b1;
            ch_1_out_0 += a1 * b0;
            ch_1_out_1 += a1 * b1;
            col_count--;
        } /* while over col_count */

        ch_0_out_0 = arm_nn_requantize(ch_0_out_0, *out_mult, *out_shift);
        ch_0_out_0 += out_offset;
        ch_0_out_0 = MAX(ch_0_out_0, activation_min);
        ch_0_out_0 = MIN(ch_0_out_0, activation_max);
        *out_0++ = (q7_t)ch_0_out_0;

        ch_0_out_1 = arm_nn_requantize(ch_0_out_1, *out_mult, *out_shift);
        ch_0_out_1 += out_offset;
        ch_0_out_1 = MAX(ch_0_out_1, activation_min);
        ch_0_out_1 = MIN(ch_0_out_1, activation_max);
        *out_1++ = (q7_t)ch_0_out_1;
        out_mult++;
        out_shift++;

        ch_1_out_0 = arm_nn_requantize(ch_1_out_0, *out_mult, *out_shift);
        ch_1_out_0 += out_offset;
        ch_1_out_0 = MAX(ch_1_out_0, activation_min);
        ch_1_out_0 = MIN(ch_1_out_0, activation_max);
        *out_0++ = (q7_t)ch_1_out_0;

        ch_1_out_1 = arm_nn_requantize(ch_1_out_1, *out_mult, *out_shift);
        ch_1_out_1 += out_offset;
        ch_1_out_1 = MAX(ch_1_out_1, activation_min);
        ch_1_out_1 = MIN(ch_1_out_1, activation_max);
        *out_1++ = (q7_t)ch_1_out_1;
        out_mult++;
        out_shift++;

        /* skip row */
        ip_a0 += num_col_a;
        row_count--;
    }

    /* compute the last odd numbered row if any */
    if (output_ch & 0x1)
    {
        /* setup pointers for B */
        const q15_t *ip_b0 = input_b;
        const q15_t *ip_b1 = ip_b0 + num_col_a;

        /* load the bias */
        q31_t ch_0_out_0 = *bias;
        q31_t ch_0_out_1 = *bias++;

        uint16_t col_count = num_col_a >> 2;
        while (col_count)
        {
            q31_t a01, a02;
            q31_t b0 = arm_nn_read_q15x2_ia(&ip_b0);
            q31_t b1 = arm_nn_read_q15x2_ia(&ip_b1);

            ip_a0 = read_and_pad(ip_a0, &a01, &a02);

            ch_0_out_0 = __SMLAD(a01, b0, ch_0_out_0);
            ch_0_out_1 = __SMLAD(a01, b1, ch_0_out_1);

            b0 = arm_nn_read_q15x2_ia(&ip_b0);
            b1 = arm_nn_read_q15x2_ia(&ip_b1);
            ch_0_out_0 = __SMLAD(a02, b0, ch_0_out_0);
            ch_0_out_1 = __SMLAD(a02, b1, ch_0_out_1);

            col_count--;
        }
        col_count = num_col_a & 0x3;
        while (col_count)
        {
            q7_t a0 = *ip_a0++;
            q15_t b0 = *ip_b0++;
            q15_t b1 = *ip_b1++;

            ch_0_out_0 += a0 * b0;
            ch_0_out_1 += a0 * b1;
            col_count--;
        }
        ch_0_out_0 = arm_nn_requantize(ch_0_out_0, *out_mult, *out_shift);
        ch_0_out_0 += out_offset;
        ch_0_out_0 = MAX(ch_0_out_0, activation_min);
        ch_0_out_0 = MIN(ch_0_out_0, activation_max);
        *out_0++ = (q7_t)ch_0_out_0;

        ch_0_out_1 = arm_nn_requantize(ch_0_out_1, *out_mult, *out_shift);
        ch_0_out_1 += out_offset;
        ch_0_out_1 = MAX(ch_0_out_1, activation_min);
        ch_0_out_1 = MIN(ch_0_out_1, activation_max);
        *out_1++ = (q7_t)ch_0_out_1;
        out_mult++;
        out_shift++;
    }

    out_0 += output_ch;

    /* return the new output pointer with offset */
    return out_0;
#else
    (void)input_a;
    (void)input_b;
    (void)output_ch;
    (void)out_shift;
    (void)out_mult;
    (void)out_offset;
    (void)activation_min;
    (void)activation_max;
    (void)num_col_a;
    (void)output_bias;
    (void)out_0;
    /* To be completed */
    return NULL;
#endif
}
