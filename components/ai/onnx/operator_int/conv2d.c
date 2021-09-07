#include "onnx.h"

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
)
{
    int i, j, k, l, m, n;
    int conv_out = 0.0f;
    int in_row, in_col;

    // For each filter
    for (i = 0; i < ch_im_out; i++)
    {
        // For each image dimension
        for (j = 0; j < dim_im_out_y; j++)
        {
            for (k = 0; k < dim_im_out_x; k++)
            {
                conv_out = bias[i];
                // For each kernel dimension
                for (m = 0; m < dim_kernel_y; m++)
                {
                    for (n = 0; n < dim_kernel_x; n++)
                    {
                        // if-for implementation
                        in_row = stride_y * j + m - padding_y;
                        in_col = stride_x * k + n - padding_x;
                        if (in_row >= 0 && in_col >= 0 && in_row < dim_im_in_y && in_col < dim_im_in_x)
                        {
                            // For each input channel
                            for (l = 0; l < ch_im_in; l++)
                            {
                                conv_out += input[(in_row * dim_im_in_x + in_col) * ch_im_in + l] *
                                            weight[i * ch_im_in * dim_kernel_y * dim_kernel_x + (m * dim_kernel_x + n) * ch_im_in +
                                               l];
                            }
                        }
                    }
                }
                output[i + (j * dim_im_out_x + k) * ch_im_out] = conv_out;
            }
        }
    }
}

int* conv2D_layer(Onnx__GraphProto* graph, const int *input, int64_t* shapeInput, int64_t* shapeOutput, const char* layer_name)
{
    //assert(graph != NULL && input != NULL && layer_name != "" );

    Onnx__NodeProto* node = onnx_graph_get_node_by_name(graph, layer_name);
    if(node == NULL)
    {
        // layer not found
        return NULL;
    }
    const char* weight = node->input[1];
    const char* bias = node->input[2];

    // Get weight shape
    int64_t* shapeW = onnx_graph_get_dims_by_name(graph, weight);
    if(shapeW == NULL)
    {
        return NULL;
    }
    int64_t dimW = onnx_graph_get_dim_by_name(graph, weight);
    if(dimW < 0)
    {
        return NULL;
    }

    // Get weights
    // NCWH --> NWHC
    int64_t permW_t[] = { 0, 2, 3, 1};
    int* W = onnx_graph_get_weights_by_name(graph, weight);
    if(W == NULL)
    {
        return NULL;
    }
    int* W_t = transpose(W, shapeW, dimW, permW_t);

    // Get bias
    int* B = onnx_graph_get_weights_by_name(graph, bias);
    if(B == NULL)
    {
        return NULL;
    }

    int* output = (int*) malloc(sizeof(int)*shapeW[0]*shapeInput[W_INDEX]*shapeInput[H_INDEX]);
    memset(output, 0, sizeof(sizeof(int)*shapeW[0]*shapeInput[W_INDEX]*shapeInput[H_INDEX]));
    conv2D(input, shapeInput[W_INDEX], shapeInput[H_INDEX], shapeW[1], W_t, shapeW[0], shapeW[2], shapeW[3], 1, 1, 1, 1, B, output, shapeInput[W_INDEX], shapeInput[H_INDEX]);

    shapeOutput[W_INDEX] = shapeInput[W_INDEX];
    shapeOutput[H_INDEX] = shapeInput[H_INDEX];
    shapeOutput[C_INDEX] = shapeW[0];

    free(W_t);

    return output;
}
