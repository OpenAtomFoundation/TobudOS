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
 * Title:        arm_avgpool_s8.c
 * Description:  Pooling function implementations
 *
 * $Date:        May 29,2020
 * $Revision:    V.2.0.1
 *
 * Target Processor:  Cortex-M CPUs
 *
 * -------------------------------------------------------------------- */

#include "cmsis/CMSIS/DSP/Include/arm_math.h"
#include "cmsis/CMSIS/NN/Include/arm_nnfunctions.h"


#if defined (ARM_MATH_DSP) && !defined (ARM_MATH_MVEI)

static void buffer_scale_back_q15_to_q7_and_clamp(q15_t *buffer, q7_t *target, uint16_t length,
                                                  uint16_t count,const int act_min, const int act_max)
{
  int i;
  int sum;

  for (i = 0; i < length; i++)
  {
    sum = buffer[i] > 0 ? (buffer[i] + count / 2) / count : (buffer[i] - count / 2) / count;

    sum = MAX(sum, act_min);
    sum = MIN(sum, act_max);

    target[i] = (q7_t) (sum);
  }
}
#endif

/**
 *  @ingroup groupNN
 */

/**
 * @addtogroup Pooling
 * @{
 */

/*
 * s8 average pooling function
 *
 * Refer to header file for details.
 *
 */

#if defined(ARM_MATH_MVEI)

arm_status arm_avgpool_s8(const cmsis_nn_context *ctx,
                          const cmsis_nn_pool_params *pool_params,
                          const cmsis_nn_dims *input_dims,
                          const q7_t *src,
                          const cmsis_nn_dims *filter_dims,
                          const cmsis_nn_dims *output_dims,
                          q7_t *dst)
{
  (void)ctx;
  const int32_t dim_src_height = input_dims->h;
  const int32_t dim_src_width = input_dims->w;
  const int32_t dim_dst_height = output_dims->h;
  const int32_t dim_dst_width = output_dims->w;
  const int32_t stride_height = pool_params->stride.h;
  const int32_t stride_width = pool_params->stride.w;
  const int32_t dim_kernel_height = filter_dims->h;
  const int32_t dim_kernel_width = filter_dims->w;
  const int32_t padding_height = pool_params->padding.h;
  const int32_t padding_width = pool_params->padding.w;
  const int32_t act_min = pool_params->activation.min;
  const int32_t act_max = pool_params->activation.max;
  const int32_t ch_src = input_dims->c;

  int32_t   i_x, i_y;
  int32_t   k_x, k_y;

  for (i_y = 0; i_y < dim_dst_height; i_y++)
  {
    for (i_x = 0; i_x < dim_dst_width; i_x++)
    {

      int32_t   k_y_start,k_y_end;
      int32_t   k_x_start,k_x_end;
      int32_t   chCnt;
      const int8_t    *pTmp, *pTmpInner;
      int8_t    *pDst;

      k_y_start = MAX(0, i_y * stride_height - padding_height);
      k_y_end = MIN(i_y * stride_height - padding_height + dim_kernel_height,dim_src_height);

      k_x_start = MAX(0,i_x * stride_width - padding_width);
      k_x_end = MIN(i_x * stride_width - padding_width + dim_kernel_width, dim_src_width);


      pTmp = src;
      pDst = &dst[ch_src * (i_x + i_y * dim_dst_width)];

      chCnt = ch_src >> 4;
      while(chCnt > 0)
      {
        int32x4_t sumV1,sumV2,sumV3,sumV4;

        int8x16_t tempV;
        int16x8_t tempVLO, tempVHI;
        int32x4_t tempVLOLO, tempVLOHI, tempVHILO, tempVHIHI;
        int32_t   count = 0;

        sumV1 = vdupq_n_s32(0);
        sumV2 = vdupq_n_s32(0);
        sumV3 = vdupq_n_s32(0);
        sumV4 = vdupq_n_s32(0);

        for (k_y = k_y_start; k_y < k_y_end; k_y++)
        {
          for (k_x = k_x_start; k_x < k_x_end; k_x++)
          {
            pTmpInner = pTmp + (ch_src * (k_x + k_y * dim_src_width));
            tempV = vldrbq_s8 (pTmpInner);

            tempVLO = vmovlbq_s8(tempV);
            tempVHI = vmovltq_s8(tempV);

            tempVLOLO = vmovlbq_s16(tempVLO);
            tempVLOHI = vmovltq_s16(tempVLO);

            tempVHILO = vmovlbq_s16(tempVHI);
            tempVHIHI = vmovltq_s16(tempVHI);

            sumV1 = vaddq_s32(sumV1,tempVLOLO);
            sumV2 = vaddq_s32(sumV2,tempVLOHI);
            sumV3 = vaddq_s32(sumV3,tempVHILO);
            sumV4 = vaddq_s32(sumV4,tempVHIHI);

            count++;
          }
        }


        sumV1[0] = sumV1[0] > 0 ? (sumV1[0] + count / 2) / count : (sumV1[0] - count / 2) / count;
        sumV1[1] = sumV1[1] > 0 ? (sumV1[1] + count / 2) / count : (sumV1[1] - count / 2) / count;
        sumV1[2] = sumV1[2] > 0 ? (sumV1[2] + count / 2) / count : (sumV1[2] - count / 2) / count;
        sumV1[3] = sumV1[3] > 0 ? (sumV1[3] + count / 2) / count : (sumV1[3] - count / 2) / count;

        sumV2[0] = sumV2[0] > 0 ? (sumV2[0] + count / 2) / count : (sumV2[0] - count / 2) / count;
        sumV2[1] = sumV2[1] > 0 ? (sumV2[1] + count / 2) / count : (sumV2[1] - count / 2) / count;
        sumV2[2] = sumV2[2] > 0 ? (sumV2[2] + count / 2) / count : (sumV2[2] - count / 2) / count;
        sumV2[3] = sumV2[3] > 0 ? (sumV2[3] + count / 2) / count : (sumV2[3] - count / 2) / count;

        sumV3[0] = sumV3[0] > 0 ? (sumV3[0] + count / 2) / count : (sumV3[0] - count / 2) / count;
        sumV3[1] = sumV3[1] > 0 ? (sumV3[1] + count / 2) / count : (sumV3[1] - count / 2) / count;
        sumV3[2] = sumV3[2] > 0 ? (sumV3[2] + count / 2) / count : (sumV3[2] - count / 2) / count;
        sumV3[3] = sumV3[3] > 0 ? (sumV3[3] + count / 2) / count : (sumV3[3] - count / 2) / count;

        sumV4[0] = sumV4[0] > 0 ? (sumV4[0] + count / 2) / count : (sumV4[0] - count / 2) / count;
        sumV4[1] = sumV4[1] > 0 ? (sumV4[1] + count / 2) / count : (sumV4[1] - count / 2) / count;
        sumV4[2] = sumV4[2] > 0 ? (sumV4[2] + count / 2) / count : (sumV4[2] - count / 2) / count;
        sumV4[3] = sumV4[3] > 0 ? (sumV4[3] + count / 2) / count : (sumV4[3] - count / 2) / count;

        sumV1 = vmaxq_s32(sumV1, vdupq_n_s32(act_min));
        sumV1 = vminq_s32(sumV1, vdupq_n_s32(act_max));

        sumV2 = vmaxq_s32(sumV2, vdupq_n_s32(act_min));
        sumV2 = vminq_s32(sumV2, vdupq_n_s32(act_max));

        sumV3 = vmaxq_s32(sumV3, vdupq_n_s32(act_min));
        sumV3 = vminq_s32(sumV3, vdupq_n_s32(act_max));

        sumV4 = vmaxq_s32(sumV4, vdupq_n_s32(act_min));
        sumV4 = vminq_s32(sumV4, vdupq_n_s32(act_max));

        tempVLO = vmovnbq_s32(tempVLO,sumV1);
        tempVLO = vmovntq_s32(tempVLO,sumV2);

        tempVHI = vmovnbq_s32(tempVHI,sumV3);
        tempVHI = vmovntq_s32(tempVHI,sumV4);


        tempV = vmovnbq_s16(tempV,tempVLO);
        tempV = vmovntq_s16(tempV,tempVHI);

        vstrbq_s8(pDst,tempV);
        pDst += 16;

        chCnt --;
        pTmp += 16;
      }

      chCnt = ch_src & 0xF;
      while(chCnt > 0)
      {
        int32_t       sum = 0;
        int32_t       count = 0;

        for (k_y = k_y_start; k_y < k_y_end; k_y++)
        {
          for (k_x = k_x_start; k_x < k_x_end; k_x++)
          {
            sum += pTmp[ch_src * (k_x + k_y * dim_src_width)];
            count++;
          }
        }
        sum = sum > 0 ? (sum + count / 2) / count : (sum - count / 2) / count;
        sum = MAX(sum, act_min);
        sum = MIN(sum, act_max);

        *pDst++ = sum;

        chCnt --;
        pTmp++;
      }
    }
  }
  return ARM_MATH_SUCCESS;
}

#else
arm_status arm_avgpool_s8(const cmsis_nn_context *ctx,
                          const cmsis_nn_pool_params *pool_params,
                          const cmsis_nn_dims *input_dims,
                          const q7_t *src,
                          const cmsis_nn_dims *filter_dims,
                          const cmsis_nn_dims *output_dims,
                          q7_t *dst)
{
  const int32_t dim_src_height = input_dims->h;
  const int32_t dim_src_width = input_dims->w;
  const int32_t dim_dst_height = output_dims->h;
  const int32_t dim_dst_width = output_dims->w;
  const int32_t stride_height = pool_params->stride.h;
  const int32_t stride_width = pool_params->stride.w;
  const int32_t dim_kernel_height = filter_dims->h;
  const int32_t dim_kernel_width = filter_dims->w;
  const int32_t padding_height = pool_params->padding.h;
  const int32_t padding_width = pool_params->padding.w;
  const int32_t act_min = pool_params->activation.min;
  const int32_t act_max = pool_params->activation.max;
  const int32_t ch_src = input_dims->c;
  q15_t *bufferA = (q15_t *)ctx->buf;

#if defined (ARM_MATH_DSP)

  /* Run the following code for Cortex-M4 and Cortex-M7
   */
  int32_t k_x, k_y, i_x, i_y;

  for (i_y = 0; i_y < dim_dst_height; i_y++)
  {
    for (i_x = 0; i_x < dim_dst_width; i_x++)
    {
      /* Condition for kernel start dimension: (base_idx_<x,y> + kernel_<x,y>_start) >= 0 */
      const int32_t base_idx_y = (i_y * stride_height) - padding_height;
      const int32_t base_idx_x = (i_x * stride_width) - padding_width;
      const int32_t kernel_y_start = MAX(0, -base_idx_y);
      const int32_t kernel_x_start = MAX(0, -base_idx_x);

      /* Condition for kernel end dimension: (base_idx_<x,y> + kernel_<x,y>_end) < dim_src_<width,height> */
      const int32_t kernel_y_end = MIN(dim_kernel_height, dim_src_height - base_idx_y);
      const int32_t kernel_x_end = MIN(dim_kernel_width, dim_src_width - base_idx_x);

      int count = 0;

      for (k_y = kernel_y_start; k_y < kernel_y_end; k_y++)
      {
        for (k_x = kernel_x_start; k_x < kernel_x_end; k_x++)
        {
          const q7_t *start = src + ch_src * (k_x + base_idx_x + (k_y + base_idx_y) * dim_src_width);

          if (count == 0)
          {
            arm_q7_to_q15_no_shift(start, bufferA, ch_src);
          }
          else
          {
            arm_nn_accumulate_q7_to_q15(bufferA, start, ch_src);
          }
          count++;
        }
      }
      buffer_scale_back_q15_to_q7_and_clamp(bufferA, dst, ch_src, count, act_min, act_max);
      dst += ch_src;
    }
  }
#else

  /* Reference C code adapted from CMSIS-NN arm_avepool_q7_HWC.
   */
  (void)bufferA;
  int16_t i_ch_in, i_x, i_y;
  int16_t k_x, k_y;

  for (i_y = 0; i_y < dim_dst_height; i_y++)
  {
    for (i_x = 0; i_x < dim_dst_width; i_x++)
    {
      for (i_ch_in = 0; i_ch_in < ch_src; i_ch_in++)
      {
        int sum = 0;
        int count = 0;
        for (k_y = i_y * stride_height - padding_height; k_y < i_y * stride_height - padding_height + dim_kernel_height; k_y++)
        {
          for (k_x = i_x * stride_width - padding_width; k_x < i_x * stride_width - padding_width + dim_kernel_width; k_x++)
          {
            if (k_y >= 0 && k_x >= 0 && k_y < dim_src_height && k_x < dim_src_width)
            {
              sum += src[i_ch_in + ch_src * (k_x + k_y * dim_src_width)];
              count++;
            }
          }
        }
        sum = sum > 0 ? (sum + count / 2) / count : (sum - count / 2) / count;
        sum = MAX(sum, act_min);
        sum = MIN(sum, act_max);

        dst[i_ch_in + ch_src * (i_x + i_y * dim_dst_width)] = sum;
      }
    }
  }

#endif
  return ARM_MATH_SUCCESS;
}

#endif /* ARM_MATH_MVEI */

int32_t arm_avgpool_s8_get_buffer_size(const int dim_dst_width,
                                       const int ch_src)
{
  (void)dim_dst_width;

#if defined(ARM_MATH_DSP) && !defined(ARM_MATH_MVEI)
  return (ch_src * sizeof(int16_t));
#else
  (void)ch_src;
  return 0;
#endif
}
/**
 * @} end of Pooling group
 */
