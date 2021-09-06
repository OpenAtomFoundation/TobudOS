#include "onnx.h"

void add(const int *input,              // pointer to vector
         const int *bias,             // pointer to matrix
         const uint16_t dim_vec,         // length of the vector
         int *output)
{
    for (int i = 0; i < dim_vec; i++)
    {
        output[i] = input[i] + bias[i];
    }
}

int* add_layer(Onnx__GraphProto* graph, const int *input, int64_t* shapeInput, int64_t* shapeOutput, const char* layer_name)
{
    //assert(graph != NULL && input != NULL && layer_name != "" );

    Onnx__NodeProto* node = onnx_graph_get_node_by_name(graph, layer_name);
    const char* bias = node->input[1];

    int* B = onnx_graph_get_weights_by_name(graph, bias);
    int64_t* shapeB =  onnx_graph_get_dims_by_name(graph, bias);
    if(shapeB == NULL)
    {
        return NULL;
    }

    int* output = (int*) malloc(sizeof(int)*shapeB[0]);
    memset(output, 0, sizeof(sizeof(int)*shapeB[0]));
    add(input, B, shapeB[0], output);

    memcpy(shapeInput, shapeOutput, sizeof(int64_t)*3);

    return output;
}
