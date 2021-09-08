/*
 * Copyright (c) 2018-2020
 * Jianjia Ma
 * majianjia@live.com
 *
 * SPDX-License-Identifier: Apache-2.0
 * 
 * Notice: 
 * Code in this file inlcudes derivative works from CMSIS
 * Please check the LICENSE file for detial.
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-10-05     Jianjia Ma   The first version
 */

#include "nnom.h"
#include "nnom_local.h"

// modified from CMSIS-NN test_ref
void local_avepool_q15_HWC(const q15_t *Im_in,           // input image
	const uint16_t dim_im_in_x,  // input image dimension x or W
	const uint16_t dim_im_in_y,  // input image dimension y or H
	const uint16_t ch_im_in,     // number of input image channels
	const uint16_t dim_kernel_x, // window kernel size
	const uint16_t dim_kernel_y, // window kernel size
	const uint16_t padding_x,    // padding sizes
	const uint16_t padding_y,    // padding sizes
	const uint16_t stride_x,     // stride
	const uint16_t stride_y,     // stride
	const uint16_t dim_im_out_x, // output image dimension x or W
	const uint16_t dim_im_out_y, // output image dimension y or H
	const uint16_t output_shift, // output right shift
	q7_t *bufferA,               // a buffer for local storage, NULL by now
	q15_t *Im_out)
{
    int16_t i_ch_in, i_x, i_y;
    int16_t k_x, k_y;

    for (i_ch_in = 0; i_ch_in < ch_im_in; i_ch_in++)
    {
        for (i_y = 0; i_y < dim_im_out_y; i_y++)
        {
            for (i_x = 0; i_x < dim_im_out_x; i_x++)
            {
                int sum = 0;
                int count = 0;
                for (k_y = i_y * stride_y - padding_y; k_y < i_y * stride_y - padding_y + dim_kernel_y; k_y++)
                {
                    for (k_x = i_x * stride_x - padding_x; k_x < i_x * stride_x - padding_x + dim_kernel_x; k_x++)
                    {
                        if (k_y >= 0 && k_x >= 0 && k_y < dim_im_in_y && k_x < dim_im_in_x)
                        {
                            sum += Im_in[i_ch_in + ch_im_in * (k_x + k_y * dim_im_in_x)];
                            count++;
                        }
                    }
                }
                Im_out[i_ch_in + ch_im_in * (i_x + i_y * dim_im_out_x)] = sum / (count>>output_shift);
            }
        }
    }
}

void local_avepool_q15_CHW(const q15_t *Im_in,           // input image
	const uint16_t dim_im_in_x,  // input image dimension x or W
	const uint16_t dim_im_in_y,  // input image dimension y or H
	const uint16_t ch_im_in,     // number of input image channels
	const uint16_t dim_kernel_x, // window kernel size
	const uint16_t dim_kernel_y, // window kernel size
	const uint16_t padding_x,    // padding sizes
	const uint16_t padding_y,    // padding sizes
	const uint16_t stride_x,     // stride
	const uint16_t stride_y,     // stride
	const uint16_t dim_im_out_x, // output image dimension x or W
	const uint16_t dim_im_out_y, // output image dimension y or H
	const uint16_t output_shift, // output right shift
	q7_t *bufferA,               // a buffer for local storage, NULL by now
	q15_t *Im_out)
{
    int16_t i_ch_in, i_x, i_y;
    int16_t k_x, k_y;
	int32_t ch_offset;

    for (i_ch_in = 0; i_ch_in < ch_im_in; i_ch_in++)
    {
		ch_offset = i_ch_in*dim_im_in_x*dim_im_in_y;
        for (i_y = 0; i_y < dim_im_out_y; i_y++)
        {
            for (i_x = 0; i_x < dim_im_out_x; i_x++)
            {
                int sum = 0;
                int count = 0;
                for (k_y = i_y * stride_y - padding_y; k_y < i_y * stride_y - padding_y + dim_kernel_y; k_y++)
                {
                    for (k_x = i_x * stride_x - padding_x; k_x < i_x * stride_x - padding_x + dim_kernel_x; k_x++)
                    {
                        if (k_y >= 0 && k_x >= 0 && k_y < dim_im_in_y && k_x < dim_im_in_x)
                        {
                            sum += Im_in[ch_offset + (k_x + k_y * dim_im_in_x)];
                            count++;
                        }
                    }
                }
                Im_out[i_ch_in*dim_im_out_x*dim_im_out_y + (i_x + i_y * dim_im_out_x)] = sum / (count>>output_shift);
            }
        }
    }
}

// modified from CMSIS-NN test_ref
void local_maxpool_q15_HWC(const q15_t *Im_in,           // input image
	const uint16_t dim_im_in_x,  // input image dimension x or W
	const uint16_t dim_im_in_y,  // input image dimension y or H
	const uint16_t ch_im_in,     // number of input image channels
	const uint16_t dim_kernel_x, // window kernel size
	const uint16_t dim_kernel_y, // window kernel size
	const uint16_t padding_x,    // padding sizes
	const uint16_t padding_y,    // padding sizes
	const uint16_t stride_x,     // stride
	const uint16_t stride_y,     // stride
	const uint16_t dim_im_out_x, // output image dimension x or W
	const uint16_t dim_im_out_y, // output image dimension y or H
	q7_t *bufferA,               // a buffer for local storage, NULL by now
	q15_t *Im_out)
{
    int16_t i_ch_in, i_x, i_y;
    int16_t k_x, k_y;

    for (i_ch_in = 0; i_ch_in < ch_im_in; i_ch_in++)
    {
        for (i_y = 0; i_y < dim_im_out_y; i_y++)
        {
            for (i_x = 0; i_x < dim_im_out_x; i_x++)
            {
                int max = -32768;
                for (k_y = i_y * stride_y - padding_y; k_y < i_y * stride_y - padding_y + dim_kernel_y; k_y++)
                {
                    for (k_x = i_x * stride_x - padding_x; k_x < i_x * stride_x - padding_x + dim_kernel_x; k_x++)
                    {
                        if (k_y >= 0 && k_x >= 0 && k_y < dim_im_in_y && k_x < dim_im_in_x)
                        {
                            if (Im_in[i_ch_in + ch_im_in * (k_x + k_y * dim_im_in_x)] > max)
                            {
                                max = Im_in[i_ch_in + ch_im_in * (k_x + k_y * dim_im_in_x)];
                            }
                        }
                    }
                }
                Im_out[i_ch_in + ch_im_in * (i_x + i_y * dim_im_out_x)] = max;
            }
        }
    }
}

void local_maxpool_q15_CHW(const q15_t *Im_in,           // input image
	const uint16_t dim_im_in_x,  // input image dimension x or W
	const uint16_t dim_im_in_y,  // input image dimension y or H
	const uint16_t ch_im_in,     // number of input image channels
	const uint16_t dim_kernel_x, // window kernel size
	const uint16_t dim_kernel_y, // window kernel size
	const uint16_t padding_x,    // padding sizes
	const uint16_t padding_y,    // padding sizes
	const uint16_t stride_x,     // stride
	const uint16_t stride_y,     // stride
	const uint16_t dim_im_out_x, // output image dimension x or W
	const uint16_t dim_im_out_y, // output image dimension y or H
	q7_t *bufferA,               // a buffer for local storage, NULL by now
	q15_t *Im_out)
{
    int16_t i_ch_in, i_x, i_y;
    int16_t k_x, k_y;
	int32_t ch_offset;

    for (i_ch_in = 0; i_ch_in < ch_im_in; i_ch_in++)
    {
		ch_offset = i_ch_in * dim_im_out_x * dim_im_out_y; 
        for (i_y = 0; i_y < dim_im_out_y; i_y++)
        {
            for (i_x = 0; i_x < dim_im_out_x; i_x++)
            {
                int max = -32768;
                for (k_y = i_y * stride_y - padding_y; k_y < i_y * stride_y - padding_y + dim_kernel_y; k_y++)
                {
                    for (k_x = i_x * stride_x - padding_x; k_x < i_x * stride_x - padding_x + dim_kernel_x; k_x++)
                    {
                        if (k_y >= 0 && k_x >= 0 && k_y < dim_im_in_y && k_x < dim_im_in_x)
                        {
                            if (Im_in[i_ch_in * dim_im_in_x * dim_im_in_y + (k_x + k_y * dim_im_in_x)] > max)
                            {
                                max = Im_in[i_ch_in * dim_im_in_x * dim_im_in_y + (k_x + k_y * dim_im_in_x)];
                            }
                        }
                    }
                }
                Im_out[ch_offset+(i_x + i_y * dim_im_out_x)] = max;
            }
        }
    }
}

// shift according to the maximum
void local_sumpool_q15_HWC(const q15_t *Im_in,           // input image
	const uint16_t dim_im_in_x,  // input image dimension x or W
	const uint16_t dim_im_in_y,  // input image dimension y or H
	const uint16_t ch_im_in,     // number of input image channels
	const uint16_t dim_kernel_x, // window kernel size
	const uint16_t dim_kernel_y, // window kernel size
	const uint16_t padding_x,    // padding sizes
	const uint16_t padding_y,    // padding sizes
	const uint16_t stride_x,     // stride
	const uint16_t stride_y,     // stride
	const uint16_t dim_im_out_x, // output image dimension x or W
	const uint16_t dim_im_out_y, // output image dimension y or H
    const uint16_t output_shift, // output right shift
	q7_t *bufferA,               // a buffer for local storage, size = 4*output_size
	q15_t *Im_out)
{
    int16_t i_ch_in, i_x, i_y;
    int16_t k_x, k_y;
    int32_t *buf = (int32_t *)bufferA;
	// stage2
    // int32_t max_abs = 0;
    // int32_t output_shift;
    // size_t output_size = dim_im_out_x * dim_im_out_x * ch_im_in;

    // save in 32bit
    for (i_ch_in = 0; i_ch_in < ch_im_in; i_ch_in++)
    {
        for (i_y = 0; i_y < dim_im_out_y; i_y++)
        {
            for (i_x = 0; i_x < dim_im_out_x; i_x++)
            {
                int sum = 0;
                for (k_y = i_y * stride_y - padding_y; k_y < i_y * stride_y - padding_y + dim_kernel_y; k_y++)
                {
                    for (k_x = i_x * stride_x - padding_x; k_x < i_x * stride_x - padding_x + dim_kernel_x; k_x++)
                    {
                        if (k_y >= 0 && k_x >= 0 && k_y < dim_im_in_y && k_x < dim_im_in_x)
                        {
                            sum += Im_in[i_ch_in + ch_im_in * (k_x + k_y * dim_im_in_x)];
                        }
                    }
                }
                // 32bit
                buf[i_ch_in + ch_im_in * (i_x + i_y * dim_im_out_x)] = (q15_t)__NNOM_SSAT((sum >> output_shift), 16);
            }
        }
    }

    // // find max amount results
    // for (int i = 0; i < output_size; i++)
    // {
    //     int32_t val = buf[i];
    //     if (val < 0)
    //         val = -val;
    //     if (val > max_abs)
    //         max_abs = val;
    // }
    // // find best shift to cover the max
    // for (output_shift = 0;; output_shift++)
    // {
    //     if (127 * (1 + output_shift) >= max_abs)
    //         break;
    // }

    // // shift the results
    // for (int i = 0; i < output_size; i++)
    // {
    //     Im_out[i] = buf[i] >> output_shift;
    // }
    //return output_shift;
}

// temporary for the thesis
// shift according to the maximum
void local_sumpool_q15_CHW(const q15_t *Im_in,           // input image
	const uint16_t dim_im_in_x,  // input image dimension x or W
	const uint16_t dim_im_in_y,  // input image dimension y or H
	const uint16_t ch_im_in,     // number of input image channels
	const uint16_t dim_kernel_x, // window kernel size
	const uint16_t dim_kernel_y, // window kernel size
	const uint16_t padding_x,    // padding sizes
	const uint16_t padding_y,    // padding sizes
	const uint16_t stride_x,     // stride
	const uint16_t stride_y,     // stride
	const uint16_t dim_im_out_x, // output image dimension x or W
	const uint16_t dim_im_out_y, // output image dimension y or H
	const uint16_t output_shift, // output right shift
	q7_t *bufferA,               // a buffer for local storage, size = 4*output_size
	q15_t *Im_out)
{
    int16_t i_ch_in, i_x, i_y;
    int16_t k_x, k_y;
    int32_t *buf = (int32_t *)bufferA;
	int32_t i_ch_offset, o_ch_offset;
	// // stage2
    // int32_t max_abs = 0;
    // int32_t output_shift;
    // size_t output_size = dim_im_out_x * dim_im_out_x * ch_im_in;

    // save in 32bit
    for (i_ch_in = 0; i_ch_in < ch_im_in; i_ch_in++)
    {
		i_ch_offset = i_ch_in*dim_im_in_x*dim_im_in_y;
		o_ch_offset = i_ch_in*dim_im_out_x*dim_im_out_y;
		
        for (i_y = 0; i_y < dim_im_out_y; i_y++)
        {
            for (i_x = 0; i_x < dim_im_out_x; i_x++)
            {
                int sum = 0;
                for (k_y = i_y * stride_y - padding_y; k_y < i_y * stride_y - padding_y + dim_kernel_y; k_y++)
                {
                    for (k_x = i_x * stride_x - padding_x; k_x < i_x * stride_x - padding_x + dim_kernel_x; k_x++)
                    {
                        if (k_y >= 0 && k_x >= 0 && k_y < dim_im_in_y && k_x < dim_im_in_x)
                        {
                            sum += Im_in[i_ch_offset + (k_x + k_y * dim_im_in_x)];
                        }
                    }
                }
                // 32bit
                buf[o_ch_offset + (i_x + i_y * dim_im_out_x)] = (q15_t)__NNOM_SSAT((sum >> output_shift), 16);
            }
        }
    }

    // // find max amount results
    // for (int i = 0; i < output_size; i++)
    // {
    //     int32_t val = buf[i];
    //     if (val < 0)
    //         val = -val;
    //     if (val > max_abs)
    //         max_abs = val;
    // }
    // // find best shift to cover the max
    // for (output_shift = 0;; output_shift++)
    // {
    //     if (127 * (1 + output_shift) >= max_abs)
    //         break;
    // }

    // // shift the results
    // for (int i = 0; i < output_size; i++)
    // {
    //     Im_out[i] = buf[i] >> output_shift;
    // }
    //return output_shift;
}

// customised up sample pooling
void local_up_sampling_q15_HWC(const q15_t *Im_in,       // input image
	const uint16_t dim_im_in_x,  // input image dimension x or W
	const uint16_t dim_im_in_y,  // input image dimension y or H
	const uint16_t ch_im_in,     // number of input image channels
	const uint16_t dim_kernel_x, // window kernel size
	const uint16_t dim_kernel_y, // window kernel size
	const uint16_t dim_im_out_x, // output image dimension x or W
	const uint16_t dim_im_out_y, // output image dimension y or H
	q7_t *bufferA,               // a buffer for local storage, NULL by now
	q15_t *Im_out)
{
    int16_t i_x, i_y;
	
    // for loop for each pixel in input image.
    for (i_y = 0; i_y < dim_im_in_y; i_y++)
    {
        for (i_x = 0; i_x < dim_im_in_x; i_x++)
        {
            // copy all the channels together. 
            const q15_t *p_in = Im_in + (i_y * dim_im_in_x + i_x ) * ch_im_in;
            q15_t *pout = Im_out + (i_y * dim_im_in_x * dim_kernel_x * dim_kernel_y + i_x * dim_kernel_y) * ch_im_in;

            // copy along x axis
            for(int i = 0; i<dim_kernel_x; i++)
                nnom_memcpy(pout + i * ch_im_in, p_in, ch_im_in * sizeof(q15_t));
            // duplicate the copied x data into y axis. 
            for(int i = 1; i<dim_kernel_y; i++)
                nnom_memcpy(pout + i * ch_im_in * dim_im_in_x * dim_kernel_x, pout, ch_im_in * dim_kernel_x * sizeof(q15_t));
        }
    }
}

void local_up_sampling_q15_CHW(const q15_t *Im_in,       // input image
	const uint16_t dim_im_in_x,  // input image dimension x or W
	const uint16_t dim_im_in_y,  // input image dimension y or H
	const uint16_t ch_im_in,     // number of input image channels
	const uint16_t dim_kernel_x, // window kernel size
	const uint16_t dim_kernel_y, // window kernel size
	const uint16_t dim_im_out_x, // output image dimension x or W
	const uint16_t dim_im_out_y, // output image dimension y or H
	q7_t *bufferA,               // a buffer for local storage, NULL by now
	q15_t *Im_out)
{
    int16_t i_x, i_y, ch;
	// for loop for channel
	for(ch=0; ch<ch_im_in; ch++)
	{
		// for loop for each pixel in input image.
		for (i_y = 0; i_y < dim_im_in_y; i_y++)
		{
			for (i_x = 0; i_x < dim_im_in_x; i_x++)
			{
				const q15_t *p_in = Im_in + ch * dim_im_in_x * dim_im_in_y + (i_y * dim_im_in_x + i_x);
				q15_t *pout = Im_out + ch * dim_im_out_x * dim_im_out_y + (i_y * dim_im_in_x * dim_kernel_x * dim_kernel_y + i_x * dim_kernel_y);

				// cpy along x axis
				for(int i = 0; i<dim_kernel_x; i++)
					*(pout + i) =  *p_in;
				// duplicate the copied x data into y axis. 
				for(int i = 1; i<dim_kernel_y; i++)
					nnom_memcpy(pout + i * dim_im_in_x * dim_kernel_x, pout, dim_kernel_x * sizeof(q15_t));
			}
		}
	}
}

void local_convolve_HWC_q15_nonsquare(const q15_t *Im_in,                // input image
	const uint16_t dim_im_in_x,                                        // input image dimention x
	const uint16_t dim_im_in_y,                                        // input image dimention y
	const uint16_t ch_im_in,                                           // number of input image channels
	const q7_t *wt,                                                    // kernel weights
	const uint16_t ch_im_out,                                          // number of filters, i.e., output image channels
	const uint16_t dim_kernel_x,                                       // filter kernel size x
	const uint16_t dim_kernel_y,                                       // filter kernel size y
	const uint16_t padding_x,                                          // padding sizes x
	const uint16_t padding_y,                                          // padding sizes y
	const uint16_t stride_x,                                           // stride x
	const uint16_t stride_y,                                           // stride y
    const uint16_t dilation_x,                                         // dilation x
	const uint16_t dilation_y,                                         // dilation y
	const q7_t *bias,                                                  // bias
	const nnom_qformat_param_t *bias_shift,                                        // bias shifts
    const nnom_qformat_param_t *out_shift,                                         // output shift
    const nnom_qtype_t q_type,                                         // per channel or per tensor
    q15_t *Im_out,                                                      // output image
	const uint16_t dim_im_out_x,                                       // output image dimension x
	const uint16_t dim_im_out_y,                                       // output image dimension y
	q15_t *bufferA,                                                    //buffer space for input
	q7_t *bufferB                                                      //buffer space for output
)
{
    int i, j, k, l, m, n;
    int64_t conv_out;
    int in_row, in_col;
    int in_pix_loc, wt_loc;
    int shift_idx, shift_steps;
    if(q_type == NNOM_QTYPE_PER_AXIS)
        shift_steps = 1;
    else
        shift_steps = 0;

    for (i = 0, shift_idx = 0; i < ch_im_out; i++, shift_idx += shift_steps)
    {
        for (j = 0; j < dim_im_out_y; j++)
        {
            int32_t base_idx_y = stride_y * j - padding_y;
            for (k = 0; k < dim_im_out_x; k++)
            {
				int32_t base_idx_x = stride_x * k - padding_x;
                int32_t ker_y_start = MAX(0, -(base_idx_y-(dilation_y-1))/dilation_y);
                int32_t ker_x_start = MAX(0, -(base_idx_x-(dilation_x-1))/dilation_x);
                int32_t ker_y_end = MIN(dim_kernel_y, (dim_im_in_y - base_idx_y + (dilation_y-1))/dilation_y);
                int32_t ker_x_end = MIN(dim_kernel_x, (dim_im_in_x - base_idx_x + (dilation_x-1))/dilation_x);

                if(bias)
                    conv_out = ((q31_t)(bias[i]) << bias_shift[shift_idx]) + NNOM_ROUND(out_shift[shift_idx]);
                else
                    conv_out = (q31_t)NNOM_ROUND(out_shift[shift_idx]);
                
                for (m = ker_y_start; m < ker_y_end; m++)
                {
                    for (n = ker_x_start; n < ker_x_end; n++)
                    {
                        // if-for implementation
                        in_row = stride_y * j + m * dilation_y - padding_y;
                        in_col = stride_x * k + n * dilation_x - padding_x;

                        // pre-calculate the pixel location and weight location to improve the performance.
                        in_pix_loc = (in_row * dim_im_in_x + in_col) * ch_im_in;
                        wt_loc = i * ch_im_in * dim_kernel_y * dim_kernel_x + (m * dim_kernel_x + n) * ch_im_in;
                        
                        for (l = 0; l < ch_im_in; l++)
                        {    
                            conv_out += Im_in[in_pix_loc + l] * wt[wt_loc + l];
                        } 
                    }
                }
                Im_out[i + (j * dim_im_out_x + k) * ch_im_out] = (q15_t)__NNOM_SSAT((conv_out >> out_shift[shift_idx]), 16);
            }
        }
    }
}

void local_convolve_CHW_q15_nonsquare(const q15_t *Im_in,                // input image
	const uint16_t dim_im_in_x,                                        // input image dimention x
	const uint16_t dim_im_in_y,                                        // input image dimention y
	const uint16_t ch_im_in,                                           // number of input image channels
	const q7_t *wt,                                                    // kernel weights
	const uint16_t ch_im_out,                                          // number of filters, i.e., output image channels
	const uint16_t dim_kernel_x,                                       // filter kernel size x
	const uint16_t dim_kernel_y,                                       // filter kernel size y
	const uint16_t padding_x,                                          // padding sizes x
	const uint16_t padding_y,                                          // padding sizes y
	const uint16_t stride_x,                                           // stride x
	const uint16_t stride_y,                                           // stride y
    const uint16_t dilation_x,                                         // dilation x
	const uint16_t dilation_y,                                         // dilation y
	const q7_t *bias,                                                  // bias
	const nnom_qformat_param_t *bias_shift,                                        // bias shifts
    const nnom_qformat_param_t *out_shift,                                         // output shift
    const nnom_qtype_t q_type,                                         // per channel or per tensor
    q15_t *Im_out,                                                      // output image
	const uint16_t dim_im_out_x,                                       // output image dimension x
	const uint16_t dim_im_out_y,                                       // output image dimension y
	q15_t *bufferA,                                                    //buffer space for input
	q7_t *bufferB                                                      //buffer space for output
)
{
    int i, j, k, l, m, n;
    int64_t conv_out;
    int in_row, in_col;
    int shift_idx, shift_steps;
    if(q_type == NNOM_QTYPE_PER_AXIS)
        shift_steps = 1;
    else
        shift_steps = 0;

    for(i = 0, shift_idx = 0; i < ch_im_out; i++, shift_idx += shift_steps)
    {
        for (j = 0; j < dim_im_out_y; j++)
        {
            for (k = 0; k < dim_im_out_x; k++)
            {
                if(bias)
                    conv_out = ((q31_t)(bias[i]) << bias_shift[shift_idx]) + NNOM_ROUND(out_shift[shift_idx]);
                else
                    conv_out = (q31_t)NNOM_ROUND(out_shift[shift_idx]);
				for (m = 0; m < dim_kernel_y; m++)
				{
					for (n = 0; n < dim_kernel_x; n++)
					{
						// if-for implementation
						in_row = stride_y * j + m * dilation_y - padding_y;
						in_col = stride_x * k + n * dilation_x - padding_x;
						if (in_row >= 0 && in_col >= 0 && in_row < dim_im_in_y && in_col < dim_im_in_x)
						{
							for (l = 0; l < ch_im_in; l++)
							{
								conv_out += Im_in[(in_row * dim_im_in_x + in_col) + l * dim_im_in_x * dim_im_in_y] *
									wt[(m * dim_kernel_x + n) * ch_im_in * ch_im_out + l * ch_im_out + i];
							}
						}
					}
				}
                Im_out[i * dim_im_out_x * dim_im_out_y + (j * dim_im_out_x + k)] = (q15_t)__NNOM_SSAT((conv_out >> out_shift[shift_idx]), 16);
            }
        }
    }
}

#define FALSE 0
#define TRUE 1

static int alg_deconv2d_calculate_position(
		int pos,
		int stride,
		int padding,
		int dim_kernel,
		int dim_in,
		int* in_start,
		int* kernel_start,
		int* kernel_end)
{
	int is_zero = FALSE;
	int of, adj;
	is_zero = FALSE;
	*in_start = pos/stride;
	of = pos%stride;
	*kernel_start = padding - of;
	if(*kernel_start >= 0) {
		adj = MIN(*in_start, *kernel_start/stride);
		*kernel_start -= adj*stride;
		*in_start -= adj;
	} else {
		adj = -*kernel_start + dim_kernel;
		if(adj<=stride) {
			is_zero = TRUE;
		} else {
			adj = MIN(dim_in-1-*in_start, adj/stride);
			*kernel_start += adj*stride;
			*in_start += adj;
		}
	}
	of = dim_kernel - 1 - *kernel_start;
	adj = MIN(dim_in-1-*in_start, of/stride);
	*kernel_end = *kernel_start + adj*stride;

	return is_zero;
}

void local_conv_trans_HWC_q15_nonsquare(const int8_t * Im_in,
	const uint16_t dim_im_in_x,                                        // input image dimention x
	const uint16_t dim_im_in_y,                                        // input image dimention y
	const uint16_t ch_im_in,                                           // number of input image channels
	const q7_t *wt,                                                    // kernel weights
	const uint16_t ch_im_out,                                          // number of filters, i.e., output image channels
	const uint16_t dim_kernel_x,                                       // filter kernel size x
	const uint16_t dim_kernel_y,                                       // filter kernel size y
	const uint16_t padding_x,                                          // padding sizes x
	const uint16_t padding_y,                                          // padding sizes y
	const uint16_t stride_x,                                           // stride x
	const uint16_t stride_y,                                           // stride y
    const uint16_t dilation_x,                                         // dilation x
	const uint16_t dilation_y,                                         // dilation y
	const q7_t *bias,                                                  // bias
	const uint16_t bias_shift, const uint16_t out_shift, q15_t *Im_out, // output image
	const uint16_t dim_im_out_x,                                       // output image dimension x
	const uint16_t dim_im_out_y,                                       // output image dimension y
	q15_t *bufferA,                                                    //buffer space for input
	q7_t *bufferB                                                      //buffer space for output
)
// {
// 	int ox, oy, oc, ky, kx, kc, ix, iy;
// 	int conv_out;
// 	int in_pix_loc, wt_loc;

//     (void)dilation_y;
//     (void)dilation_x;

//     // padding and stride are applied to output 
//  	for (oc = 0; oc < ch_im_out; oc++) 
//     {
//  		for (oy = 0; oy < dim_im_out_y; oy++) 
//         {
//  			for (ox = 0; ox < dim_im_out_x; ox++)
//             {
//  				conv_out = ((q31_t)(bias[oc]) << bias_shift) + NNOM_ROUND(out_shift);

//                 for (ky = 0; ky < dim_kernel_y; ky++)
//                 {
//                     for (kx = 0; kx < dim_kernel_x; kx++)
//                     {
//                         // input y, input x location
//                         iy = oy / stride_y + ky - padding_y;
//                         ix = ox / stride_x + kx - padding_x;

// 						if(ix >= 0 && iy >= 0 && ix < dim_im_in_y && iy< dim_im_in_y)
// 						{
// 							in_pix_loc = (iy * dim_im_in_x + ix) * ch_im_in;
// 							wt_loc = oc * ch_im_in * dim_kernel_y * dim_kernel_x + (ky * dim_kernel_x + kx) * ch_im_in;
						
// 							for (kc = 0; kc < ch_im_in; kc++)
// 							{    
// 								conv_out += Im_in[in_pix_loc + kc] * wt[wt_loc + kc];
// 							} 
// 						}
//                     }
//                 }

//  				Im_out[oc + (oy * dim_im_out_x + ox) * ch_im_out] = (q7_t) __NNOM_SSAT((conv_out >> out_shift), 8);
//  			}
//  		}
//  	}
//  }

{
	int i, j, k, l, m, n;
	int64_t conv_out;
	int in_row, in_col;
	int kernel_start_x,kernel_end_x;
	int kernel_start_y,kernel_end_y;
	int in_row_start, in_col_start;
	int is_zero;

	for (i = 0; i < ch_im_out; i++) {
		for (j = 0; j < dim_im_out_y; j++) {
			is_zero = alg_deconv2d_calculate_position(j, stride_y, padding_y, dim_kernel_y,
					dim_im_in_y, &in_row_start, &kernel_start_y, &kernel_end_y);

			if(is_zero) {
				conv_out = ((q31_t)(bias[i]) << bias_shift) + NNOM_ROUND(out_shift);
				conv_out = (q15_t) __NNOM_SSAT((conv_out >> out_shift), 16);
				for (k = 0; k < dim_im_out_x; k++) {
					Im_out[i + (j * dim_im_out_x + k) * ch_im_out] = (q15_t) conv_out;
				}
				continue;
			}

			for (k = 0; k < dim_im_out_x; k++) {
				conv_out = ((q31_t)(bias[i]) << bias_shift) + NNOM_ROUND(out_shift);

				is_zero = alg_deconv2d_calculate_position(k, stride_x, padding_x, dim_kernel_x,
						dim_im_in_x, &in_col_start, &kernel_start_x, &kernel_end_x);

				if(is_zero) {
					Im_out[i + (j * dim_im_out_x + k) * ch_im_out] = conv_out;
					continue;
				}

				for (m = kernel_start_y, in_row = in_row_start; m <= kernel_end_y; m+=stride_y, in_row++) {
					for (n = kernel_start_x, in_col = in_col_start; n <= kernel_end_x; n+=stride_x, in_col++) {
						if ((in_row >= 0) && (in_col >= 0) &&
							(in_row < dim_im_in_y) && (in_col < dim_im_in_x)) {
							for (l = 0; l < ch_im_in; l++) {
								conv_out += Im_in[(in_row * dim_im_in_x + in_col) * ch_im_in + l] *
									wt[i * ch_im_in * dim_kernel_y * dim_kernel_x + (m * dim_kernel_x + n) * ch_im_in + l];
							}
						}
					}
				}

				Im_out[i + (j * dim_im_out_x + k) * ch_im_out] = (q15_t) __NNOM_SSAT((conv_out >> out_shift), 16);
			}
		}
	}
}




void local_depthwise_separable_conv_HWC_q15_nonsquare(const q15_t *Im_in,// input image
	const uint16_t dim_im_in_x,                                        // input image dimention x
	const uint16_t dim_im_in_y,                                        // input image dimention y
	const uint16_t ch_im_in,                                           // number of input image channels
	const q7_t *wt,                                                    // kernel weights
	const uint16_t ch_im_out,                                          // number of filters, i.e., output image channels
	const uint16_t dim_kernel_x,                                       // filter kernel size x
	const uint16_t dim_kernel_y,                                       // filter kernel size y
	const uint16_t padding_x,                                          // padding sizes x
	const uint16_t padding_y,                                          // padding sizes y
	const uint16_t stride_x,                                           // stride x
	const uint16_t stride_y,                                           // stride y
    const uint16_t dilation_x,                                         // dilation x
	const uint16_t dilation_y,                                         // dilation y
	const q7_t *bias,                                                  // bias
	const nnom_qformat_param_t *bias_shift,                            // bias shifts
    const nnom_qformat_param_t *out_shift,                             // output shift
    const nnom_qtype_t q_type,                                         // per channel or per tensor
    q15_t *Im_out,                                                      // output image
	const uint16_t dim_im_out_x,                                       // output image dimension x
	const uint16_t dim_im_out_y,                                       // output image dimension y
	q15_t *bufferA,                                                    //buffer space for input
	q7_t *bufferB                                                      //buffer space for output
)
{
    int i_out_y, i_out_x, i_ch_out, i_ch_in, i_ch_mult;
    int i_ker_y, i_ker_x;
    int i_out = 0;
    int shift_idx;
    int ch_mult = ch_im_out / ch_im_in;
    int64_t conv_out;

    for (i_out_y = 0; i_out_y < dim_im_out_y; i_out_y++)
    {
        const int32_t base_idx_y = stride_y * i_out_y - padding_y;
        for (i_out_x = 0; i_out_x < dim_im_out_x; i_out_x++)
        {
            const int32_t base_idx_x = stride_x * i_out_x - padding_x;
            for (i_ch_in = 0; i_ch_in < ch_im_in; i_ch_in++)
            {
                for(i_ch_mult = 0; i_ch_mult < ch_mult; i_ch_mult++)
                {
                    i_ch_out = i_ch_mult + i_ch_in * ch_mult;
                    int32_t ker_y_start = MAX(0, -base_idx_y);
                    int32_t ker_x_start = MAX(0, -base_idx_x);
                    int32_t ker_y_end = MIN(dim_kernel_y, dim_im_in_y - base_idx_y);
                    int32_t ker_x_end = MIN(dim_kernel_x, dim_im_in_x - base_idx_x);

                    shift_idx = q_type == NNOM_QTYPE_PER_AXIS ? i_ch_out : 0;
                    if (bias)
                        conv_out = ((q31_t)(bias[i_ch_out]) << bias_shift[shift_idx]) + NNOM_ROUND(out_shift[shift_idx]);
                    else
                        conv_out = (q31_t)NNOM_ROUND(out_shift[shift_idx]);

                    for (i_ker_y = ker_y_start; i_ker_y < ker_y_end; i_ker_y++)
                    {
                        const int32_t idx_y = base_idx_y + i_ker_y * dilation_y;
                        for (i_ker_x = ker_x_start; i_ker_x < ker_x_end; i_ker_x++)
                        {
                            const int32_t idx_x = base_idx_x + i_ker_x * dilation_x;
                            int32_t in_pix_loc = (idx_y * dim_im_in_x + idx_x) * ch_im_in + i_ch_in;
                            int32_t wt_loc = (i_ker_y * dim_kernel_x + i_ker_x) * (ch_im_in * ch_mult) + i_ch_out;
                            conv_out += Im_in[in_pix_loc] * wt[wt_loc];
                        }
                    }
                    Im_out[i_out++] = (q15_t)__NNOM_SSAT((conv_out >> out_shift[shift_idx]), 16);
                }
            }
        }
    }
}

void local_depthwise_separable_conv_CHW_q15_nonsquare(const q15_t *Im_in,// input image
	const uint16_t dim_im_in_x,                                        // input image dimention x
	const uint16_t dim_im_in_y,                                        // input image dimention y
	const uint16_t ch_im_in,                                           // number of input image channels
	const q7_t *wt,                                                    // kernel weights
	const uint16_t ch_im_out,                                          // number of filters, i.e., output image channels
	const uint16_t dim_kernel_x,                                       // filter kernel size x
	const uint16_t dim_kernel_y,                                       // filter kernel size y
	const uint16_t padding_x,                                          // padding sizes x
	const uint16_t padding_y,                                          // padding sizes y
	const uint16_t stride_x,                                           // stride x
	const uint16_t stride_y,                                           // stride y
    const uint16_t dilation_x,                                         // dilation x
	const uint16_t dilation_y,                                         // dilation y
	const q7_t *bias,                                                  // bias
	const nnom_qformat_param_t *bias_shift,                            // bias shifts
    const nnom_qformat_param_t *out_shift,                             // output shift
    const nnom_qtype_t q_type,                                         // per channel or per tensor
    q15_t *Im_out,                                                      // output image
	const uint16_t dim_im_out_x,                                       // output image dimension x
	const uint16_t dim_im_out_y,                                       // output image dimension y
	q15_t *bufferA,                                                    //buffer space for input
	q7_t *bufferB                                                      //buffer space for output
)
{
    int i_out_y, i_out_x, i_ch_out, i_ch_in, i_ch_mult;
    int i_ker_y, i_ker_x;
    int shift_idx;
    int ch_mult = ch_im_out / ch_im_in;
    int64_t conv_out;

    for (i_out_y = 0; i_out_y < dim_im_out_y; i_out_y++)
    {
        const int32_t base_idx_y = stride_y * i_out_y - padding_y;
        for (i_out_x = 0; i_out_x < dim_im_out_x; i_out_x++)
        {
            const int32_t base_idx_x = stride_x * i_out_x - padding_x;
            for (i_ch_in = 0; i_ch_in < ch_im_in; i_ch_in++)
            {
                for (i_ch_mult = 0; i_ch_mult < ch_mult; i_ch_mult++)
                {
                    i_ch_out = i_ch_mult + i_ch_in * ch_mult;
                    int32_t ker_y_start = MAX(0, -base_idx_y);
                    int32_t ker_x_start = MAX(0, -base_idx_x);
                    int32_t ker_y_end = MIN(dim_kernel_y, dim_im_in_y - base_idx_y);
                    int32_t ker_x_end = MIN(dim_kernel_x, dim_im_in_x - base_idx_x);

                    shift_idx = q_type == NNOM_QTYPE_PER_AXIS ? i_ch_out : 0;
                    if (bias)
                        conv_out = ((q31_t)(bias[i_ch_out]) << bias_shift[shift_idx]) + NNOM_ROUND(out_shift[shift_idx]);
                    else
                        conv_out = (q31_t)NNOM_ROUND(out_shift[shift_idx]);

                    for (i_ker_y = ker_y_start; i_ker_y < ker_y_end; i_ker_y++)
                    {
                        const int32_t idx_y = base_idx_y + i_ker_y * dilation_y;
                        for (i_ker_x = ker_x_start; i_ker_x < ker_x_end; i_ker_x++)
                        {
                            const int32_t idx_x = base_idx_x + i_ker_x * dilation_x;
                            int32_t in_pix_loc = (idx_y * dim_im_in_x + idx_x) + i_ch_in * dim_im_in_x * dim_im_in_y;
                            int32_t wt_loc = (i_ker_y * dim_kernel_x + i_ker_x) * ch_im_out +  i_ch_out;
                            conv_out += Im_in[in_pix_loc] * wt[wt_loc];
                        }
                    }
                    Im_out[i_ch_out * dim_im_out_x * dim_im_out_y + (i_out_y * dim_im_out_x + i_out_x)] = 
                        (q15_t)__NNOM_SSAT((conv_out >> out_shift[shift_idx]), 16);
                }
            }
        }
    }

}

void local_zero_padding_HWC_q15(const q15_t *Im_in,           // input image
	const uint16_t dim_im_in_x,    // input image dimention x
	const uint16_t dim_im_in_y,    // input image dimention y
	const uint16_t ch_im_in,       // number of input image channels
	const uint16_t padding_top,    // padding sizes y
	const uint16_t padding_bottom, // padding sizes y
	const uint16_t padding_left,   // padding sizes x
	const uint16_t padding_right,  // padding sizes x
	q15_t *Im_out,                  // output image
	const uint16_t dim_im_out_x,   // output image dimension x
	const uint16_t dim_im_out_y)   // output image dimension y 
{
	int i, size;
	q15_t * p_out = Im_out; 
	
	// top rows
	size = dim_im_out_x*ch_im_in*padding_top;
	nnom_memset(p_out, 0, size*sizeof(q15_t)); 
	p_out += size;
	
	// middle
	for(i=0; i<dim_im_in_y; i++)
	{
		// left - set to 0
		size = ch_im_in * padding_left;
		nnom_memset(p_out, 0, size*sizeof(q15_t)); 
		p_out += size;
		// data - copy a row
		size = dim_im_in_x * ch_im_in;
		nnom_memcpy(p_out, Im_in + i*size, size*sizeof(q15_t));
		p_out += size;
		// right - set to 0
		size = ch_im_in * padding_right;
		nnom_memset(p_out, 0, size*sizeof(q15_t)); 
		p_out += size;
	}
	// bottom rows
	nnom_memset(p_out, 0, dim_im_out_x*ch_im_in*padding_bottom*sizeof(q15_t)); 
}

void local_zero_padding_CHW_q15(const q15_t *Im_in,           // input image
	const uint16_t dim_im_in_x,    // input image dimention x
	const uint16_t dim_im_in_y,    // input image dimention y
	const uint16_t ch_im_in,       // number of input image channels
	const uint16_t padding_top,    // padding sizes y
	const uint16_t padding_bottom, // padding sizes y
	const uint16_t padding_left,   // padding sizes x
	const uint16_t padding_right,  // padding sizes x
	q15_t *Im_out,                  // output image
	const uint16_t dim_im_out_x,   // output image dimension x
	const uint16_t dim_im_out_y)   // output image dimension y 
{
	int i, size, ch_offset;
	q15_t * p_out = Im_out; 
	
	for(int ch=0; ch < ch_im_in; ch++)
	{
		p_out = Im_out + ch * dim_im_out_x * dim_im_out_y;
		// top rows
		size = dim_im_out_x*padding_top;
		nnom_memset(p_out, 0, size*sizeof(q15_t));
		p_out += size;
		
		// middle
		ch_offset = ch*dim_im_in_x*dim_im_in_y;
		for(i=0; i<dim_im_in_y; i++)
		{
			// left - set to 0
			nnom_memset(p_out, 0, padding_left*sizeof(q15_t)); 
			p_out += padding_left;
			// data - copy a row
			nnom_memcpy(p_out, Im_in + i*dim_im_in_x + ch_offset, dim_im_in_x*sizeof(q15_t));
			p_out += dim_im_in_x;
			// right - set to 0
			nnom_memset(p_out, 0, size*sizeof(q15_t)); 
			p_out += padding_right;
		}
		// bottom
		nnom_memset(p_out, 0, dim_im_out_x*padding_bottom*sizeof(q15_t)); 
	}

}

void local_cropping_HWC_q15(const q15_t *Im_in,           // input image
	const uint16_t dim_im_in_x,    // input image dimention x
	const uint16_t dim_im_in_y,    // input image dimention y
	const uint16_t ch_im_in,       // number of input image channels
	const uint16_t padding_top,    // padding sizes y
	const uint16_t padding_bottom, // padding sizes y
	const uint16_t padding_left,   // padding sizes x
	const uint16_t padding_right,  // padding sizes x
	q15_t *Im_out,                  // output image
	const uint16_t dim_im_out_x,   // output image dimension x
	const uint16_t dim_im_out_y)   // output image dimension y 
{
	int i, row_size;
	const q15_t * p_in = Im_in; 
	
	// top rows to ignore
	p_in += dim_im_in_x*ch_im_in*padding_top;
	
	// middle
	row_size = dim_im_out_x * ch_im_in;
	for(i=0; i<dim_im_out_y; i++)
	{
		// left to ignore          
		p_in += ch_im_in * padding_left;
		// data - copy a row
		nnom_memcpy(Im_out + i*row_size, p_in, row_size*sizeof(q15_t));
		p_in += row_size;
		// right to ingore
		p_in += ch_im_in * padding_right;
	}

}

void local_cropping_CHW_q15(const q15_t *Im_in,           // input image
	const uint16_t dim_im_in_x,    // input image dimention x
	const uint16_t dim_im_in_y,    // input image dimention y
	const uint16_t ch_im_in,       // number of input image channels
	const uint16_t padding_top,    // padding sizes y
	const uint16_t padding_bottom, // padding sizes y
	const uint16_t padding_left,   // padding sizes x
	const uint16_t padding_right,  // padding sizes x
	q15_t *Im_out,                  // output image
	const uint16_t dim_im_out_x,   // output image dimension x
	const uint16_t dim_im_out_y)   // output image dimension y 
{
	int i, ch, ch_offset;
	const q15_t * p_in; 
	
	for(ch=0; ch < ch_im_in; ch++)
	{
		p_in = Im_in + dim_im_in_x * dim_im_in_y * ch; 	// ch offset to input image
		p_in += dim_im_in_x*padding_top; 				// top to ignore
		
		ch_offset = ch*dim_im_out_x*dim_im_out_y;
		for(i=0; i<dim_im_out_y; i++)
		{	
			// data - middle of a row
			nnom_memcpy(Im_out + i*dim_im_out_x + ch_offset, p_in+padding_left, dim_im_out_x*sizeof(q15_t)); 
			p_in += dim_im_in_x; // middle and right padding	
		}
	}	
}


void local_dot_q15(const q15_t *pV, // pointer to vector
	const q15_t *pM,               // pointer to matrix
	const uint16_t dim_vec,       // length of the vector
	const uint16_t num_of_rows,   // numCol of A
	const uint16_t out_shift,     // amount of right-shift for output
	 q15_t *pOut)                   // output operand)
{
    for (int i = 0; i < num_of_rows; i++)
    {
        int64_t ip_out = (q31_t) NNOM_ROUND(out_shift); // q31 might not be enough
        for (int j = 0; j < dim_vec; j++)
        {
            ip_out += pV[j] * pM[i * dim_vec + j];
        }
        pOut[i] = (q15_t)__NNOM_SSAT((ip_out >> out_shift), 16);
    }
}

void local_dot_q15_opt(const q15_t * pV,
	const q15_t * pM,
	const uint16_t dim_vec,
	const uint16_t num_of_rows,
	const uint16_t out_shift, 
	q15_t * pOut)
{
    /* Run the following code as reference implementation for Cortex-M0 and Cortex-M3 */
    uint16_t  rowCnt = num_of_rows >> 2;
    const q15_t *pB = pM;
    const q15_t *pA;
    q15_t    *pO = pOut;

    while (rowCnt)
    {
        int64_t     sum =  (q31_t) NNOM_ROUND(out_shift);
        int64_t     sum2 = (q31_t) NNOM_ROUND(out_shift);
        int64_t     sum3 = (q31_t) NNOM_ROUND(out_shift);
        int64_t     sum4 = (q31_t) NNOM_ROUND(out_shift);
        uint16_t  colCnt = dim_vec >> 1;
        pA = pV;
        while (colCnt)
        {
            q15_t     inA1 = *pA++;
            q15_t     inA2 = *pA++;
            q15_t     inB1 = *pB++;
            q15_t     inB2 = *pB++;
            sum += inA1 * inB1 + inA2 * inB2;

            inB1 = *pB++;
            inB2 = *pB++;
            sum2 += inA1 * inB1 + inA2 * inB2;

            inB1 = *pB++;
            inB2 = *pB++;
            sum3 += inA1 * inB1 + inA2 * inB2;

            inB1 = *pB++;
            inB2 = *pB++;
            sum4 += inA1 * inB1 + inA2 * inB2;

            colCnt--;
        }
        colCnt = dim_vec & 0x1;
        while (colCnt)
        {
            q15_t     inA = *pA++;
            q15_t     inB = *pB++;
            sum += inA * inB;
            inB = *pB++;
            sum2 += inA * inB;
            inB = *pB++;
            sum3 += inA * inB;
            inB = *pB++;
            sum4 += inA * inB;
            colCnt--;
        }
        *pO++ = (q15_t) __NNOM_SSAT((sum >> out_shift), 16);
        *pO++ = (q15_t) __NNOM_SSAT((sum2 >> out_shift), 16);
        *pO++ = (q15_t) __NNOM_SSAT((sum3 >> out_shift), 16);
        *pO++ = (q15_t) __NNOM_SSAT((sum4 >> out_shift), 16);

        rowCnt--;
    }
    rowCnt = num_of_rows & 0x3;

    while (rowCnt)
    {
        int64_t       ip_out = (q31_t) + NNOM_ROUND(out_shift);
        int       j;

        pA = pV;
        for (j = 0; j < dim_vec; j++)
        {
            q15_t     inA = *pA++;
            q15_t     inB = *pB++;
            ip_out += inA * inB;
        }
        *pO++ = (q15_t) __NNOM_SSAT((ip_out >> out_shift), 16);

        rowCnt--;
    }
}

void local_fully_connected_mat_q7_vec_q15_opt(const q15_t * pV,
                                       const q7_t * pM,
                                       const uint16_t dim_vec,
                                       const uint16_t num_of_rows,
                                       const uint16_t bias_shift,
                                       const uint16_t out_shift, 
                                       const q7_t * bias, 
                                       q15_t * pOut, 
                                       q15_t * vec_buffer)
{

    (void)vec_buffer;

    /* Run the following code as reference implementation for Cortex-M0 and Cortex-M3 */
    uint16_t  rowCnt = num_of_rows >> 2;
    const q7_t *pB = pM;
    const q15_t *pA;
    q15_t    *pO = pOut;
    const q7_t *pBias = bias;

    while (rowCnt)
    {
        int64_t     sum;
        int64_t     sum2;
        int64_t     sum3;
        int64_t     sum4;
        uint16_t  colCnt = dim_vec >> 1;

        // quick and dirty to support none bias fully connected
        if(bias)
        {
            sum =  ((q31_t)(*pBias++) << bias_shift) + NNOM_ROUND(out_shift);
            sum2 = ((q31_t)(*pBias++) << bias_shift) + NNOM_ROUND(out_shift);
            sum3 = ((q31_t)(*pBias++) << bias_shift) + NNOM_ROUND(out_shift);
            sum4 = ((q31_t)(*pBias++) << bias_shift) + NNOM_ROUND(out_shift);
        }
        else
        {
            sum =  (q31_t) NNOM_ROUND(out_shift);
            sum2 = (q31_t) NNOM_ROUND(out_shift);
            sum3 = (q31_t) NNOM_ROUND(out_shift);
            sum4 = (q31_t) NNOM_ROUND(out_shift);
        }
        
        pA = pV;
        while (colCnt)
        {
            q15_t     inA1 = *pA++;
            q15_t     inA2 = *pA++;

            q7_t      inB1 = *pB++;
            q7_t      inB3 = *pB++;
            q7_t      inB2 = *pB++;
            q7_t      inB4 = *pB++;

            sum += inA1 * inB1 + inA2 * inB2;
            sum2 += inA1 * inB3 + inA2 * inB4;

            inB1 = *pB++;
            inB3 = *pB++;
            inB2 = *pB++;
            inB4 = *pB++;

            sum3 += inA1 * inB1 + inA2 * inB2;
            sum4 += inA1 * inB3 + inA2 * inB4;

            colCnt--;
        }

        colCnt = dim_vec & 0x1;
        while (colCnt)
        {
            q15_t     inA = *pA++;
            q7_t      inB = *pB++;
            sum += inA * inB;
            inB = *pB++;
            sum2 += inA * inB;
            inB = *pB++;
            sum3 += inA * inB;
            inB = *pB++;
            sum4 += inA * inB;

            colCnt--;
        }
        *pO++ = (q15_t) __NNOM_SSAT((sum >> out_shift), 16);
        *pO++ = (q15_t) __NNOM_SSAT((sum2 >> out_shift), 16);
        *pO++ = (q15_t) __NNOM_SSAT((sum3 >> out_shift), 16);
        *pO++ = (q15_t) __NNOM_SSAT((sum4 >> out_shift), 16);

        rowCnt--;
    }

    rowCnt = num_of_rows & 0x3;

    while (rowCnt)
    {
        int64_t       ip_out;
        int       j;

        // quick and dirty to support none bias fully connected
        if(bias)
            ip_out = ((q31_t)(*pBias++) << bias_shift) + NNOM_ROUND(out_shift);
        else
            ip_out = (q31_t)NNOM_ROUND(out_shift);
        
        pA = pV;
        for (j = 0; j < dim_vec; j++)
        {
            q15_t     inA = *pA++;
            q7_t      inB = *pB++;
            ip_out += inA * inB;
        }
        *pO++ = (q15_t) __NNOM_SSAT((ip_out >> out_shift), 16);

        rowCnt--;
    }
}

void local_fully_connected_mat_q7_vec_q15(const q15_t * pV,
                                   const q7_t * pM,
                                   const uint16_t dim_vec,
                                   const uint16_t num_of_rows,
                                   const uint16_t bias_shift,
                                   const uint16_t out_shift,
                                   const q7_t * bias,
                                   q15_t * pOut,
                                   q15_t * vec_buffer)
{
    int  i, j;

    // a quick solution for none-bias dot. 
    if(bias == NULL)
    {
        for (i = 0; i < num_of_rows; i++)
        {
            int64_t ip_out = (q31_t) NNOM_ROUND(out_shift);
            for (j = 0; j < dim_vec; j++)
            {
                ip_out += pV[j] * pM[i * dim_vec + j];
            }
            pOut[i] = (q15_t) __NNOM_SSAT((ip_out >> out_shift), 16);
        }
    }
    else
    {
        for (i = 0; i < num_of_rows; i++)
        {
            int64_t ip_out = ((q31_t)(bias[i]) << bias_shift) + NNOM_ROUND(out_shift);
            for (j = 0; j < dim_vec; j++)
            {
                ip_out += pV[j] * pM[i * dim_vec + j];
            }
            pOut[i] = (q15_t) __NNOM_SSAT((ip_out >> out_shift), 16);
        }
    }
}

// This softmax is a copy from ARM CMSIS implimentation as it was efficient and written in pure-C.
// original implementation: https://github.com/ARM-software/CMSIS_5/blob/develop/CMSIS/NN/Source/SoftmaxFunctions/arm_softmax_q15.c
void local_softmax_q15(const q15_t * vec_in, const uint16_t dim_vec, q15_t * p_out)
{
    q31_t     sum;
    int16_t   i;
    uint8_t   shift;
    q31_t     base;
    base = -1 * 0x100000;
    for (i = 0; i < dim_vec; i++)
    {
        if (vec_in[i] > base)
        {
            base = vec_in[i];
        }
    }

    /* we ignore really small values
     * anyway, they will be 0 after shrinking
     * to q15_t
     */
    base = base - 16;

    sum = 0;

    for (i = 0; i < dim_vec; i++)
    {
        if (vec_in[i] > base)
        {
            shift = (uint8_t)__NNOM_USAT(vec_in[i] - base, 5);
            sum += 0x1 << shift;
        }
    }

    /* This is effectively (0x1 << 32) / sum */
    int64_t div_base = 0x100000000LL;
    int output_base = (int32_t)(div_base / sum);

    /* Final confidence will be output_base >> ( 17 - (vec_in[i] - base) )
     * so 32768 (0x1<<15) -> 100% confidence when sum = 0x1 << 16, output_base = 0x1 << 16
     * and vec_in[i]-base = 16
     */
    for (i = 0; i < dim_vec; i++)
    {
        if (vec_in[i] > base)
        {
            /* Here minimum value of 17+base-vec[i] will be 1 */
            shift = (uint8_t)__NNOM_USAT(17+base-vec_in[i], 5);
            p_out[i] = (q15_t) __NNOM_SSAT((output_base >> shift), 16);
        } else
        {
            p_out[i] = 0;
        }
    }

}


// hard sigmoid, 
// y=-1 if x < -2.5
// y=1  if x > 2.5
// otherwise y = 0.2 * x + 0.5 (y=0.20315 * x + 0.5)
void local_hard_sigmoid_q15(q15_t *data, uint32_t size, int16_t dec_bit)
{
	int16_t limit = 2.5f * (1 << dec_bit)-1; 
	int16_t offset = 16384;	// 0.5 * 32768
	int16_t mult = 6554;  	// 0.2 * 32768

	// int bit >= 0
	for(int i=0; i<size; i++)
	{
		if(data[i] <= -limit)
			data[i] = 0;
		else if(data[i] >= limit)
			data[i] = 32767;
		else
		{
			data[i] = ((int32_t)(data[i] * mult) >> dec_bit) + offset;
		} 
	}   
 }

// hard tanh
// y=-1 if x < -1
// y=1  if x > 1
// otherwise y = x
void local_hard_tanh_q15(q15_t *data, uint32_t size, int16_t dec_bit)
{
    int16_t int_bit = 15 - dec_bit;
    int16_t limit = 1 << dec_bit;

    if(dec_bit == 15)
        return;
	
	// int bit < 0
	if(int_bit < 0)
		for(int i=0; i<size; i++)
		{
			if(data[i] <= -limit)
				data[i] = -32768;
			else if(data[i] >= limit)
				data[i] = 32767;
			else
			{
				data[i] = data[i] >> (-int_bit);
			}
		}
	else
		// int bit >= 0
		for(int i=0; i<size; i++)
		{
			if(data[i] <= -limit)
				data[i] = -32768;
			else if(data[i] >= limit)
				data[i] = 32767;
			else
			{
				data[i] = data[i] << int_bit;
			}
		}
}

void local_relu_q15(q15_t *data, uint32_t size)
{
    uint32_t i;

    for (i = 0; i < size; i++)
    {
        if (data[i] < 0)
            data[i] = 0;
    }
}

// alpha in q7 format with dec_bit=7
void local_leaky_relu_q15(q15_t *data, q7_t alpha, uint32_t size)
{
    uint32_t i;
    
    for (i = 0; i < size; i++)
    {
        if (data[i] < 0)
        {
            data[i] = data[i] * alpha / 128;
        }
    }
}

// alpha in q7 format with dec_bit=7
// max and threshold has the same Q format with the activation
void local_adv_relu_q15(q15_t *data, q7_t negative_slope, q15_t max, q15_t threshold, uint32_t size)
{
    uint32_t i;
    for (i = 0; i < size; i++)
    {
        //   `f(x) = max_value` for `x >= max_value`,
        //   `f(x) = x` for `threshold <= x < max_value`,
        //   `f(x) = alpha * (x - threshold)` otherwise.

        if(data[i] > max)
            data[i] = max;
        if (data[i] < threshold)
            data[i] = (data[i] - threshold) * negative_slope / 128;    
    }
}

// ARM's CMSIS implementation. 
static void local_activation_q15(q15_t * data, uint32_t size, uint16_t int_width, const q15_t*lookup_table)
{
    uint32_t  i = size;
    q15_t    *pIn = data;
    q15_t    *pOut = data;
    uint16_t  shift_size = 8 + 3 - int_width;
    uint32_t  bit_mask = 0x7FF >> int_width;
    uint32_t  full_frac = bit_mask + 1;
    while (i)
    {
        q15_t     out;
        q15_t     in = *pIn++;
        q15_t     frac = (uint32_t) in & bit_mask;
        q15_t     value = lookup_table[(uint8_t)(in >> shift_size)];
        if ((in >> shift_size) != 0x7f)
        {
            q15_t    value2 = lookup_table[(uint8_t)(1 + ((uint8_t)(in >> shift_size)))];
            /* doing the interpolation here for better accuracy */
            out = ((q31_t) (full_frac - frac) * value + (q31_t) value2 * frac) >> shift_size;
        } else
        {
            /* the largest positive value does not have a right side for linear interpolation */
            out = value;
        }
        *pOut++ = out;
        i--;
    }
}

void local_sigmoid_q15(q15_t * data, uint32_t size, uint16_t int_width)
{
    local_activation_q15(data, size, int_width, nnom_sigmoid_table_q15);
}

void local_tanh_q15(q15_t * data, uint32_t size, uint16_t int_width)
{
    local_activation_q15(data, size, int_width, nnom_tanh_table_q15);
}

// matrix ops q15
void local_mult_q15(q15_t *pSrcA,
                   q15_t *pSrcB,
                   q15_t *pDst,
                   const uint16_t out_shift,
                   uint32_t blockSize)
{
    uint32_t i;

    for (i = 0; i < blockSize; i++)
    {
        q31_t product = pSrcA[i] * pSrcB[i];
        pDst[i] = (q15_t) __NNOM_SSAT(((product + NNOM_ROUND(out_shift)) >> out_shift), 16);
    }
}

void local_add_q15(q15_t *pSrcA,
                  q15_t *pSrcB,
                  q15_t *pDst,
                  const uint16_t out_shift,
                  uint32_t blockSize)
{
    uint32_t i;

    for (i = 0; i < blockSize; i++)
    {
        q31_t sum = pSrcA[i] + pSrcB[i];
        pDst[i] = (q15_t) __NNOM_SSAT(((sum + NNOM_ROUND(out_shift)) >> out_shift), 16);
    }
}

void local_sub_q15(q15_t *pSrcA,
                  q15_t *pSrcB,
                  q15_t *pDst,
                  const uint16_t out_shift,
                  uint32_t blockSize)
{
    uint32_t i;

    for (i = 0; i < blockSize; i++)
    {
        q31_t sub = pSrcA[i] - pSrcB[i];
        pDst[i] = (q15_t) __NNOM_SSAT(((sub + NNOM_ROUND(out_shift)) >> out_shift), 16);
    }
}


void local_multiple_add_q15( q15_t *p_dst,
                  const int16_t out_shift,
                  uint32_t block_size,
                  uint32_t num_block,
                  q15_t **p_src)
{
    uint32_t i, blk;
    q31_t sum; 

    for (i = 0; i < block_size; i++)
    {
        sum = 0;
        for(blk=0; blk < num_block; blk++)
            sum += p_src[blk][i];
        p_dst[i] = (q15_t) __NNOM_SSAT(((sum + NNOM_ROUND(out_shift)) >> out_shift), 16);
    }
}

void local_multiple_mult_q15( q15_t *p_dst,
                  const int16_t out_shift,
                  uint32_t block_size,
                  uint32_t num_block,
                  q15_t **p_src)
{
    uint32_t i, blk;
    q63_t product; 

    for (i = 0; i < block_size; i++)
    {
        product = 1;
        for(blk=0; blk < num_block; blk++)
            product *= p_src[blk][i];
        p_dst[i] = (q15_t) __NNOM_SSAT(((product + NNOM_ROUND(out_shift)) >> out_shift), 16);
    }
}

void local_multiple_sub_q15( q15_t *p_dst,
                  const int16_t out_shift,
                  uint32_t block_size,
                  uint32_t num_block,
                  q15_t **p_src)
{
    uint32_t i, blk;
    q31_t sub; 

    for (i = 0; i < block_size; i++)
    {
        sub = p_src[0][i];
        for(blk=1; blk < num_block; blk++)
            sub -= p_src[blk][i];
        p_dst[i] = (q15_t) __NNOM_SSAT(((sub + NNOM_ROUND(out_shift)) >> out_shift), 16);
    }
}

// y = 1 - x
void local_1_minor_z_q15(q15_t* src, q15_t*des, uint16_t dec_bit, uint32_t size)
{
    int32_t one = (1 << dec_bit)-1;
    for(int i=0; i<size/8; i++)
    {
        *des++ = one - *src++;
        *des++ = one - *src++;
        *des++ = one - *src++;
        *des++ = one - *src++;
        *des++ = one - *src++;
        *des++ = one - *src++;
        *des++ = one - *src++;
        *des++ = one - *src++;
    }
    for(int i=0; i<size%8; i++)
    {
        *des++ = one - *src++;
    }
}


