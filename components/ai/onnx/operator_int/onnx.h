#ifndef __ONNX_H__
#define __ONNX_H__

#include <stdio.h>
#include <stdint.h>
#include <string.h>
//#include <int.h>
#include <math.h>

#include <onnx-parser.h>

#define ONNX_USE_NWHC

#ifdef  ONNX_USE_NWHC
    // NWHC
    #define W_INDEX 0
    #define H_INDEX 1
    #define C_INDEX 2
#else
    // NCWH
    #define C_INDEX 0
    #define W_INDEX 1
    #define H_INDEX 2
#endif

// Model
void onnx_tensor_info(const int* A, int64_t* shape, int64_t dim);
int* onnx_model_run(Onnx__ModelProto* model, int* input, int64_t* shapeInput);

// Layers
int* conv2D_layer(Onnx__GraphProto* graph, const int *input, int64_t* shapeInput, int64_t* shapeOutput, const char* layer_name);
int* relu_layer(Onnx__GraphProto* graph, const int *input, int64_t* shapeInput, int64_t* shapeOutput, const char* layer_name);
int* maxpool_layer(Onnx__GraphProto* graph, int* input, int64_t* shapeInput, int64_t* shapeOutput, const char* layer_name);
int* matmul_layer(Onnx__GraphProto* graph, const int *input, int64_t* shapeInput, int64_t* shapeOutput, const char* layer_name);
int* add_layer(Onnx__GraphProto* graph, const int *input, int64_t* shapeInput, int64_t* shapeOutput, const char* layer_name);
int* softmax_layer(Onnx__GraphProto* graph, const int *input, int64_t* shapeInput, int64_t* shapeOutput, const char* layer_name);

// Operators
int* transpose(const int* A, int64_t* shape, int64_t dim, int64_t* perm);

void conv2D(const int *input,                                                // input image
            const uint16_t dim_im_in_x,                                        // input image dimention x
            const uint16_t dim_im_in_y,                                        // input image dimention y
            const uint16_t ch_im_in,                                           // number of input image channels
            const int *weight,                                               // kernel weights
            const uint16_t ch_im_out,                                          // number of filters, i.e., output image channels
            const uint16_t dim_kernel_x,                                       // filter kernel size x
            const uint16_t dim_kernel_y,                                       // filter kernel size y
            const uint16_t padding_x,                                          // padding sizes x
            const uint16_t padding_y,                                          // padding sizes y
            const uint16_t stride_x,                                           // stride x
            const uint16_t stride_y,                                           // stride y
            const int *bias,                                                 // bias
            int *output,                                                     // output image
            const uint16_t dim_im_out_x,                                       // output image dimension x
            const uint16_t dim_im_out_y                                        // output image dimension y
);

void relu(const int *input, uint32_t size, int* output);

void maxpool(const int *input,
             const uint16_t dim_im_in_x,    // input image dimension x or W
             const uint16_t dim_im_in_y,    // input image dimension y or H
             const uint16_t ch_im_in,       // number of input image channels
             const uint16_t dim_kernel_x,   // window kernel size
             const uint16_t dim_kernel_y,   // window kernel size
             const uint16_t padding_x,      // padding sizes
             const uint16_t padding_y,      // padding sizes
             const uint16_t stride_x,       // stride
             const uint16_t stride_y,       // stride
             const uint16_t dim_im_out_x,   // output image dimension x or W
             const uint16_t dim_im_out_y,   // output image dimension y or H
             int *output);

void matmul(const int *input,             // pointer to vector
           const int *weight,             // pointer to matrix
           const uint16_t dim_vec,          // length of the vector
           const uint16_t num_of_rows,      // numCol of A
           int *output);

void add(const int *input,                // pointer to vector
           const int *bias,               // pointer to matrix
           const uint16_t dim_vec,          // length of the vector
           int *output);

void dense(const int *input,              // pointer to vector
           const int *weight,             // pointer to matrix
           const uint16_t dim_vec,          // length of the vector
           const uint16_t num_of_rows,      // numCol of A
           const int *bias,
           int *output);

void softmax(const int *input, const uint32_t dim_vec, int *output);

#endif // __ONNX_H__
