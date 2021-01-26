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
 * Title:        arm_nn_mat_mul_core_4x_s8.c
 * Description:  General matrix multiplication function for MVE extension
 *
 * $Date:        January 20, 2020
 * $Revision:    V.2.0.0
 *
 * Target Processor:  Cortex-M cores
 * -------------------------------------------------------------------- */

#include "cmsis/CMSIS/DSP/Include/arm_math.h"
#include "cmsis/CMSIS/NN/Include/arm_nnfunctions.h"

/**
 * @ingroup groupSupport
 */

/**
 * @addtogroup NNBasicMath
 * @{
 */

/*
   * s8 matrix multiplication to process 4 rows and one column
   *
   * Refer header file for details.
   *
   */
arm_status arm_nn_mat_mul_core_4x_s8(const int32_t row_elements,
                                     const int32_t offset,
                                     const int8_t *row_base,
                                     const int8_t *col_base,
                                     int32_t *const sum_col,
                                     int32_t *const output)
{
    int32_t acc_n0 = 0;
    int32_t acc_n1 = 0;
    int32_t acc_n2 = 0;
    int32_t acc_n3 = 0;

    const int8_t *ip_row_0 = row_base;
    const int8_t *ip_row_1 = row_base + offset;
    const int8_t *ip_row_2 = row_base + (2 * offset);
    const int8_t *ip_row_3 = row_base + (3 * offset);
    int32_t sum_tmp = 0;

#if defined(ARM_MATH_MVEI) && !defined(ARM_MATH_AUTOVECTORIZE)
    __asm volatile(
        "   vldrb.8         q0, [%[col]], 16     \n"
        "   wlstp.8         lr, %[cnt], 1f       \n"
        "2:                                      \n"
        "   vaddva.s8      %[sum], q0            \n"
        "   vldrb.8         q1, [%[row0]], 16    \n"
        "   vmladava.s8    %[out0], q0, q1       \n"
        "   vldrb.8         q2, [%[row1]], 16    \n"
        "   vmladava.s8     %[out1], q0, q2      \n"
        "   vldrb.8         q3, [%[row2]], 16    \n"
        "   vmladava.s8     %[out2], q0, q3      \n"
        "   vldrb.8         q4, [%[row3]], 16    \n"
        "   vmladava.s8     %[out3], q0, q4      \n"
        "   vldrb.8         q0, [%[col]], 16     \n"
        "   letp            lr, 2b               \n"
        "1:                                      \n"
        :[col] "+r"(col_base)
        ,[sum] "+Te"(sum_tmp)
        ,[row0] "+r"(ip_row_0)
        ,[row1] "+r"(ip_row_1)
        ,[row2] "+r"(ip_row_2)
        ,[row3] "+r"(ip_row_3)
        ,[out0] "+Te"(acc_n0)
        ,[out1] "+Te"(acc_n1)
        ,[out2] "+Te"(acc_n2)
        ,[out3] "+Te"(acc_n3)
        : [cnt] "r"(row_elements)
        : "q0", "q1", "q2", "q3", "q4", "memory", "r14");
#else
    for (int i = 0; i < row_elements; i++)
    {
        int32_t col = col_base[i];
        sum_tmp += col;
        acc_n0 += ip_row_0[i] * col;
        acc_n1 += ip_row_1[i] * col;
        acc_n2 += ip_row_2[i] * col;
        acc_n3 += ip_row_3[i] * col;
    }
#endif
    output[0] = acc_n0;
    output[1] = acc_n1;
    output[2] = acc_n2;
    output[3] = acc_n3;

    *sum_col = sum_tmp;

    return ARM_MATH_SUCCESS;
}

/**
 * @} end of NNBasicMath group
 */
