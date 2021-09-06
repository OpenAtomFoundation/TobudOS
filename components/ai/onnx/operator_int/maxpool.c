#include "onnx.h"

void maxpool(const int *input,
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
             int *output)
{
    int16_t i_ch_in, i_x, i_y;
    int16_t k_x, k_y;

    for (i_ch_in = 0; i_ch_in < ch_im_in; i_ch_in++)
    {
        for (i_y = 0; i_y < dim_im_out_y; i_y++)
        {
            for (i_x = 0; i_x < dim_im_out_x; i_x++)
            {
                //int max = FLT_MIN;
                int max = 0;
                for (k_y = i_y * stride_y - padding_y; k_y < i_y * stride_y - padding_y + dim_kernel_y; k_y++)
                {
                    for (k_x = i_x * stride_x - padding_x; k_x < i_x * stride_x - padding_x + dim_kernel_x; k_x++)
                    {
                        if (k_y >= 0 && k_x >= 0 && k_y < dim_im_in_y && k_x < dim_im_in_x)
                        {
                            if (input[i_ch_in + ch_im_in * (k_x + k_y * dim_im_in_x)] > max)
                            {
                                max = input[i_ch_in + ch_im_in * (k_x + k_y * dim_im_in_x)];
                            }
                        }
                    }
                }
                output[i_ch_in + ch_im_in * (i_x + i_y * dim_im_out_x)] = max;
            }
        }
    }
}

int* maxpool_layer(Onnx__GraphProto* graph, int* input, int64_t* shapeInput, int64_t* shapeOutput, const char* layer_name)
{
    //assert(graph != NULL && input != NULL && layer_name != "" );

    Onnx__NodeProto* node = onnx_graph_get_node_by_name(graph, layer_name);
    if(node == NULL)
    {
        // layer not found
        return NULL;
    }

    uint16_t kernel_x =  1;
    uint16_t kernel_y =  1;
    uint16_t padding_x = 0;
    uint16_t padding_y = 0;
    uint16_t stride_x =  1;
    uint16_t stride_y =  1;

    for(int i = 0; i < node->n_attribute; i++)
    {
        if( strcmp(node->attribute[i]->name, "kernel_shape") == 0 )
        {
            kernel_x = node->attribute[i]->ints[0];
            kernel_y = node->attribute[i]->ints[1];
        }
        if( strcmp(node->attribute[i]->name, "strides") == 0 )
        {
            stride_x = node->attribute[i]->ints[0];
            stride_y = node->attribute[i]->ints[1];
        }
    }

    uint16_t out_x = (shapeInput[W_INDEX] - kernel_x + 2 * padding_x) / stride_x + 1;
    uint16_t out_y = (shapeInput[H_INDEX] - kernel_y + 2 * padding_y) / stride_y + 1;

    int* output = (int*) malloc(sizeof(int)*out_x*out_y*shapeInput[C_INDEX]);
    if(output == NULL)
    {
        // No memory
        return NULL;
    }
    memset(output, 0, sizeof(sizeof(int)*out_x*out_y*shapeInput[C_INDEX]));
    maxpool(input, shapeInput[W_INDEX], shapeInput[H_INDEX], shapeInput[C_INDEX], kernel_x, kernel_y, padding_x, padding_y, stride_x, stride_y, out_x, out_y, output);

    shapeOutput[W_INDEX] = out_x;
    shapeOutput[H_INDEX] = out_y;
    shapeOutput[C_INDEX] = shapeInput[C_INDEX];

    return output;
}
