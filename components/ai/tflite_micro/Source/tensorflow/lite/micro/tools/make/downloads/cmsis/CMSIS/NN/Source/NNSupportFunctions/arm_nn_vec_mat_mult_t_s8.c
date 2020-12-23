/*
 * Copyright (C) 2020 Arm Limited or its affiliates. All rights reserved.
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
 * Title:        arm_nn_vec_mat_mult_t_s8
 * Description:  s8 vector by matrix (transposed) multiplication
 *
 * $Date:        April 2, 2020
 * $Revision:    V.1.5.0
 *
 * Target Processor:  Cortex-M
 *
 * -------------------------------------------------------------------- */

#include "cmsis/CMSIS/DSP/Include/arm_math.h"
#include "cmsis/CMSIS/NN/Include/arm_nnfunctions.h"
#include "cmsis/CMSIS/NN/Include/arm_nnsupportfunctions.h"

/**
 * @ingroup groupSupport
 */

/**
 * @addtogroup NNBasicMath
 * @{
 */

/*
 * s8 vector(lhs) by matrix (transposed) multiplication
   *
   * Refer header file for details.
   *
   */
arm_status arm_nn_vec_mat_mult_t_s8(const q7_t *lhs,
                                    const q7_t *rhs,
                                    const q31_t *bias,
                                    q7_t *dst,
                                    const int32_t lhs_offset,
                                    const int32_t rhs_offset,
                                    const int32_t dst_offset,
                                    const int32_t dst_multiplier,
                                    const int32_t dst_shift,
                                    const int32_t rhs_cols,
                                    const int32_t rhs_rows,
                                    const int32_t activation_min,
                                    const int32_t activation_max)
{
#if defined(ARM_MATH_MVEI)

    const int16x8_t rhs_offset_vec = vdupq_n_s16((int16_t)rhs_offset);
    const int16x8_t lhs_offset_vec = vdupq_n_s16((int16_t)lhs_offset);

    int32_t row_loop_cnt = rhs_rows / 4;

    for (int i_row_loop_cnt = 0; i_row_loop_cnt < row_loop_cnt; i_row_loop_cnt++)
    {
        int32_t acc1 = bias[0];
        int32_t acc2 = bias[1];
        int32_t acc3 = bias[2];
        int32_t acc4 = bias[3];
        bias += 4;

        int32x4_t acc;
        const int32_t col_loop_cnt = (rhs_cols + 7) / 8;

        const int8_t *vec = lhs;
        const int8_t *rhs_0 = rhs;
        const int8_t *rhs_1 = rhs + rhs_cols;
        const int8_t *rhs_2 = rhs + 2 * rhs_cols;
        const int8_t *rhs_3 = rhs + 3 * rhs_cols;

        uint32_t col_cnt = (uint32_t)rhs_cols;

        for (int i = 0; i < col_loop_cnt; i++)
        {
            mve_pred16_t p = vctp16q(col_cnt);
            col_cnt -= 8;
            const int16x8_t tmp_b = vaddq_m_s16(vuninitializedq_s16(),
                                                vldrbq_z_s16(vec, p), lhs_offset_vec, p);

            const int16x8_t tmp_a0 = vaddq_m_s16(vuninitializedq_s16(),
                                                 vldrbq_z_s16(rhs_0, p), rhs_offset_vec, p);
            acc1 = vmladavaq_p_s16(acc1, tmp_a0, tmp_b, p);

            const int16x8_t tmp_a1 = vaddq_m_s16(vuninitializedq_s16(),
                                                 vldrbq_z_s16(rhs_1, p), rhs_offset_vec, p);
            acc2 = vmladavaq_p_s16(acc2, tmp_a1, tmp_b, p);

            const int16x8_t tmp_a2 = vaddq_m_s16(vuninitializedq_s16(),
                                                 vldrbq_z_s16(rhs_2, p), rhs_offset_vec, p);
            acc3 = vmladavaq_p_s16(acc3, tmp_a2, tmp_b, p);

            const int16x8_t tmp_a3 = vaddq_m_s16(vuninitializedq_s16(),
                                                 vldrbq_z_s16(rhs_3, p), rhs_offset_vec, p);
            acc4 = vmladavaq_p_s16(acc4, tmp_a3, tmp_b, p);

            vec += 8;
            rhs_0 += 8;
            rhs_1 += 8;
            rhs_2 += 8;
            rhs_3 += 8;
        }
        rhs += 4 * rhs_cols;

        acc[0] = acc1;
        acc[1] = acc2;
        acc[2] = acc3;
        acc[3] = acc4;

        acc = arm_requantize_mve(acc, dst_multiplier, dst_shift);
        acc = vaddq_s32(acc, vdupq_n_s32(dst_offset));
        acc = vmaxq_s32(acc, vdupq_n_s32(activation_min));
        acc = vminq_s32(acc, vdupq_n_s32(activation_max));

        vstrbq_s32(dst, acc);
        dst += 4;
    }

    row_loop_cnt = rhs_rows & 3;

    for (int i_row_loop_cnt = 0; i_row_loop_cnt < row_loop_cnt;
         i_row_loop_cnt++)
    {
        int32_t acc = *bias++;
        const int32_t col_loop_cnt = (rhs_cols + 7) / 8;
        const int8_t *vec = lhs;
        const int8_t *kernel_cur = rhs;

        uint32_t col_cnt = (uint32_t)rhs_cols;

        for (int i = 0; i < col_loop_cnt; i++)
        {
            mve_pred16_t p = vctp16q(col_cnt);
            col_cnt -= 8;
            const int16x8_t tmp_b = vaddq_m_s16(vuninitializedq_s16(),
                                                vldrbq_z_s16(vec, p), lhs_offset_vec, p);

            const int16x8_t tmp_a = vaddq_m_s16(vuninitializedq_s16(),
                                                vldrbq_z_s16(kernel_cur, p), rhs_offset_vec, p);
            acc = vmladavaq_p_s16(acc, tmp_a, tmp_b, p);
            vec += 8;
            kernel_cur += 8;
        }
        rhs += rhs_cols;

        acc = arm_nn_requantize(acc, dst_multiplier, dst_shift);
        acc += dst_offset;

        acc = MAX(acc, activation_min);
        acc = MIN(acc, activation_max);
        *dst++ = (int8_t)(acc);
    }

#elif defined(ARM_MATH_DSP)
    const int32_t off0 = rhs_cols - 4;
    const int16_t lhs_offset_s16 = lhs_offset;
    const int16_t rhs_offset_s16 = rhs_offset;

    const uint32_t lhs_offset_s16x2 = __PKHBT(lhs_offset_s16, lhs_offset_s16, 16);
    const uint32_t rhs_offset_s16x2 = __PKHBT(rhs_offset_s16, rhs_offset_s16, 16);

    for (int32_t rhs_rows_idx = 0; rhs_rows_idx <= (rhs_rows - 2); rhs_rows_idx += 2)
    {
        const q7_t *lhs_ptr = &lhs[0];
        const q7_t *rhs_ptr = &rhs[0];

        q31_t res00 = *bias++;
        q31_t res01 = *bias++;

        int32_t rhs_cols_idx = 0;

        q31_t val0, val1, val2, val3, val4, val5;
        for (; rhs_cols_idx <= (rhs_cols - 16); rhs_cols_idx += 16)
        {
            // Read 4 x int8 values from the RHS matrix
            val0 = arm_nn_read_q7x4_ia((const q7_t **)&rhs_ptr);
            val2 = __SXTAB16(rhs_offset_s16x2, val0);
            // Read 4 x int8 values from the LHS vector
            val1 = arm_nn_read_q7x4_ia((const q7_t **)&lhs_ptr);
            val0 = __SXTAB16(rhs_offset_s16x2, __ROR(val0, 8));
            val3 = __SXTAB16(lhs_offset_s16x2, val1);
            // Read 4 x int8 values from the RHS matrix
            val4 = arm_nn_read_q7x4((const q7_t *)rhs_ptr + off0);
            val1 = __SXTAB16(lhs_offset_s16x2, __ROR(val1, 8));

            // Perform the accumulations
            res00 = __SMLAD(val3, val2, res00);
            val5  = __SXTAB16(rhs_offset_s16x2, val4);
            res00 = __SMLAD(val1, val0, res00);
            val4  = __SXTAB16(rhs_offset_s16x2, __ROR(val4, 8));
            // Read 4 x int8 values from the RHS matrix
            val0 = arm_nn_read_q7x4_ia((const q7_t **)&rhs_ptr);
            res01 = __SMLAD(val3, val5, res01);
            res01 = __SMLAD(val1, val4, res01);

            val2 = __SXTAB16(rhs_offset_s16x2, val0);
            // Read 4 x int8 values from the LHS vector
            val1 = arm_nn_read_q7x4_ia((const q7_t **)&lhs_ptr);
            val0 = __SXTAB16(rhs_offset_s16x2, __ROR(val0, 8));
            val3 = __SXTAB16(lhs_offset_s16x2, val1);
            // Read 4 x int8 values from the RHS matrix
            val4 = arm_nn_read_q7x4((const q7_t *)rhs_ptr + off0);
            val1 = __SXTAB16(lhs_offset_s16x2, __ROR(val1, 8));

            // Perform the accumulations
            res00 = __SMLAD(val3, val2, res00);
            val5  = __SXTAB16(rhs_offset_s16x2, val4);
            res00 = __SMLAD(val1, val0, res00);
            val4  = __SXTAB16(rhs_offset_s16x2, __ROR(val4, 8));
            // Read 4 x int8 values from the RHS matrix
            val0 = arm_nn_read_q7x4_ia((const q7_t **)&rhs_ptr);
            res01 = __SMLAD(val3, val5, res01);
            res01 = __SMLAD(val1, val4, res01);

            val2 = __SXTAB16(rhs_offset_s16x2, val0);
            // Read 4 x int8 values from the LHS vector
            val1 = arm_nn_read_q7x4_ia((const q7_t **)&lhs_ptr);
            val0 = __SXTAB16(rhs_offset_s16x2, __ROR(val0, 8));
            val3 = __SXTAB16(lhs_offset_s16x2, val1);
            // Read 4 x int8 values from the RHS matrix
            val4 = arm_nn_read_q7x4((const q7_t *)rhs_ptr + off0);
            val1 = __SXTAB16(lhs_offset_s16x2, __ROR(val1, 8));

            // Perform the accumulations
            res00 = __SMLAD(val3, val2, res00);
            val5  = __SXTAB16(rhs_offset_s16x2, val4);
            res00 = __SMLAD(val1, val0, res00);
            val4  = __SXTAB16(rhs_offset_s16x2, __ROR(val4, 8));
            // Read 4 x int8 values from the RHS matrix
            val0 = arm_nn_read_q7x4_ia((const q7_t **)&rhs_ptr);
            res01 = __SMLAD(val3, val5, res01);
            res01 = __SMLAD(val1, val4, res01);

            val2 = __SXTAB16(rhs_offset_s16x2, val0);
            // Read 4 x int8 values from the LHS vector
            val1 = arm_nn_read_q7x4_ia((const q7_t **)&lhs_ptr);
            val0 = __SXTAB16(rhs_offset_s16x2, __ROR(val0, 8));
            val3 = __SXTAB16(lhs_offset_s16x2, val1);
            // Read 4 x int8 values from the RHS matrix
            val4 = arm_nn_read_q7x4((const q7_t *)rhs_ptr + off0);
            val1 = __SXTAB16(lhs_offset_s16x2, __ROR(val1, 8));

            // Perform the accumulations
            res00 = __SMLAD(val3, val2, res00);
            val5  = __SXTAB16(rhs_offset_s16x2, val4);
            res00 = __SMLAD(val1, val0, res00);
            val4  = __SXTAB16(rhs_offset_s16x2, __ROR(val4, 8));
            res01 = __SMLAD(val3, val5, res01);
            res01 = __SMLAD(val1, val4, res01);
        }

        for (; rhs_cols_idx < rhs_cols; ++rhs_cols_idx)
        {
            q31_t rhs_value0 = rhs_ptr[0] + rhs_offset;
            q31_t rhs_value1 = rhs_ptr[rhs_cols] + rhs_offset;
            q31_t lhs_value  = lhs_ptr[0] + lhs_offset;

            res00 += lhs_value * rhs_value0;
            res01 += lhs_value * rhs_value1;

            ++rhs_ptr;
            ++lhs_ptr;
        }

        // Quantize down
        res00 = arm_nn_requantize(res00, dst_multiplier, dst_shift);
        res01 = arm_nn_requantize(res01, dst_multiplier, dst_shift);

        // Add offset
        res00 += dst_offset;
        res01 += dst_offset;

        // Clamp the result
        res00 = MAX(res00, activation_min);
        res00 = MIN(res00, activation_max);
        res01 = MAX(res01, activation_min);
        res01 = MIN(res01, activation_max);

        *dst++ = (q7_t)res00;
        *dst++ = (q7_t)res01;

        rhs += 2 * rhs_cols;
    }

    if (rhs_rows % 2)
    {
        const q7_t *lhs_ptr = &lhs[0];
        const q7_t *rhs_ptr = &rhs[0];

        q31_t res00 = *bias++;

        int32_t rhs_cols_idx = 0;

        q31_t val0, val1, val2, val3;
        for (; rhs_cols_idx <= (rhs_cols - 16); rhs_cols_idx += 16)
        {
            val0 = arm_nn_read_q7x4_ia((const q7_t **)&rhs_ptr);
            val1 = __SXTAB16(rhs_offset_s16x2, val0);
            val2 = arm_nn_read_q7x4_ia((const q7_t **)&lhs_ptr);
            val0 = __SXTAB16(rhs_offset_s16x2, __ROR(val0, 8));
            val3 = __SXTAB16(lhs_offset_s16x2, val2);
            val2 = __SXTAB16(lhs_offset_s16x2, __ROR(val2, 8));

            // Partial accumulations
            res00 = __SMLAD(val3, val1, res00);
            res00 = __SMLAD(val2, val0, res00);

            val0 = arm_nn_read_q7x4_ia((const q7_t **)&rhs_ptr);
            val1 = __SXTAB16(rhs_offset_s16x2, val0);
            val2 = arm_nn_read_q7x4_ia((const q7_t **)&lhs_ptr);
            val0 = __SXTAB16(rhs_offset_s16x2, __ROR(val0, 8));
            val3 = __SXTAB16(lhs_offset_s16x2, val2);
            val2 = __SXTAB16(lhs_offset_s16x2, __ROR(val2, 8));

            // Partial accumulations
            res00 = __SMLAD(val3, val1, res00);
            res00 = __SMLAD(val2, val0, res00);

            val0 = arm_nn_read_q7x4_ia((const q7_t **)&rhs_ptr);
            val1 = __SXTAB16(rhs_offset_s16x2, val0);
            val2 = arm_nn_read_q7x4_ia((const q7_t **)&lhs_ptr);
            val0 = __SXTAB16(rhs_offset_s16x2, __ROR(val0, 8));
            val3 = __SXTAB16(lhs_offset_s16x2, val2);
            val2 = __SXTAB16(lhs_offset_s16x2, __ROR(val2, 8));

            // Partial accumulations
            res00 = __SMLAD(val3, val1, res00);
            res00 = __SMLAD(val2, val0, res00);

            val0 = arm_nn_read_q7x4_ia((const q7_t **)&rhs_ptr);
            val1 = __SXTAB16(rhs_offset_s16x2, val0);
            val2 = arm_nn_read_q7x4_ia((const q7_t **)&lhs_ptr);
            val0 = __SXTAB16(rhs_offset_s16x2, __ROR(val0, 8));
            val3 = __SXTAB16(lhs_offset_s16x2, val2);
            val2 = __SXTAB16(lhs_offset_s16x2, __ROR(val2, 8));

            // Partial accumulations
            res00 = __SMLAD(val3, val1, res00);
            res00 = __SMLAD(val2, val0, res00);
        }

        for (; rhs_cols_idx < rhs_cols; ++rhs_cols_idx)
        {
            q31_t rhs_value0 = rhs_ptr[0] + rhs_offset;
            q31_t lhs_value  = lhs_ptr[0] + lhs_offset;

            res00 += lhs_value * rhs_value0;

            ++rhs_ptr;
            ++lhs_ptr;
        }

        // Quantize down
        res00 = arm_nn_requantize(res00, dst_multiplier, dst_shift);

        // Add offset
        res00 += dst_offset;

        // Clamp the result
        res00 = MAX(res00, activation_min);
        res00 = MIN(res00, activation_max);

        *dst = (q7_t)res00;
    }

#else

    for (int32_t rhs_rows_idx = 0; rhs_rows_idx <= (rhs_rows - 2); rhs_rows_idx += 2)
    {
        const q7_t *lhs_ptr = &lhs[0];
        const q7_t *rhs_ptr = &rhs[0];

        q31_t res00 = *bias++;
        q31_t res01 = *bias++;

        for (int32_t rhs_cols_idx = 0; rhs_cols_idx < rhs_cols; ++rhs_cols_idx)
        {
            q31_t rhs_value0 = rhs_ptr[0] + rhs_offset;
            q31_t rhs_value1 = rhs_ptr[rhs_cols] + rhs_offset;
            q31_t lhs_value  = lhs_ptr[0] + lhs_offset;

            res00 += lhs_value * rhs_value0;
            res01 += lhs_value * rhs_value1;

            ++rhs_ptr;
            ++lhs_ptr;
        }

        // Quantize down
        res00 = arm_nn_requantize(res00, dst_multiplier, dst_shift);
        res01 = arm_nn_requantize(res01, dst_multiplier, dst_shift);

        // Add offset
        res00 += dst_offset;
        res01 += dst_offset;

        // Clamp the result
        res00 = MAX(res00, activation_min);
        res00 = MIN(res00, activation_max);
        res01 = MAX(res01, activation_min);
        res01 = MIN(res01, activation_max);

        *dst++ = (q7_t)res00;
        *dst++ = (q7_t)res01;

        rhs += 2 * rhs_cols;
    }

    if (rhs_rows % 2)
    {
        const q7_t *lhs_ptr = &lhs[0];
        const q7_t *rhs_ptr = &rhs[0];

        q31_t res00 = *bias++;

        for (int32_t rhs_cols_idx = 0; rhs_cols_idx < rhs_cols; ++rhs_cols_idx)
        {
            q31_t rhs_value0 = rhs_ptr[0] + rhs_offset;
            q31_t lhs_value  = lhs_ptr[0] + lhs_offset;

            res00 += lhs_value * rhs_value0;

            ++rhs_ptr;
            ++lhs_ptr;
        }

        // Quantize down
        res00 = arm_nn_requantize(res00, dst_multiplier, dst_shift);

        // Add offset
        res00 += dst_offset;

        // Clamp the result
        res00 = MAX(res00, activation_min);
        res00 = MIN(res00, activation_max);

        *dst = (q7_t)res00;
    }
#endif

    return ARM_MATH_SUCCESS;
}

/**
 * @} end of NNBasicMath group
 */
