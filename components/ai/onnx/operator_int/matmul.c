#include "onnx.h"

void matmul(const int *input,              // pointer to vector
           const int *weight,             // pointer to matrix
           const uint16_t dim_vec,         // length of the vector
           const uint16_t num_of_rows,     // numCol of A
           int *output)
{
    for (int i = 0; i < num_of_rows; i++)
    {
        int ip_out = 0;
        for (int j = 0; j < dim_vec; j++)
        {
            ip_out += input[j] * weight[i * dim_vec + j];
        }
        output[i] = ip_out;
    }
}

int* matmul_layer(Onnx__GraphProto* graph, const int *input, int64_t* shapeInput, int64_t* shapeOutput, const char* layer_name)
{
    //assert(graph != NULL && input != NULL && layer_name != "" );

    Onnx__NodeProto* node = onnx_graph_get_node_by_name(graph, layer_name);
    const char* weight = node->input[1];

    int64_t* shapeW =  onnx_graph_get_dims_by_name(graph, weight);
    if(shapeW == NULL)
    {
        return NULL;
    }
    int64_t dimW = onnx_graph_get_dim_by_name(graph, weight);
    if(dimW < 0)
    {
        return NULL;
    }

    //assert(shapeW[0] == shapeInput[1]);

    int64_t permW_t[] = {1, 0};
    int* W = onnx_graph_get_weights_by_name(graph, weight);
    if(W == NULL)
    {
        return NULL;
    }
    int* W_t = transpose(W, shapeW, dimW, permW_t);

    int* output = (int*) malloc(sizeof(int)*shapeW[1]);
    if(output == NULL)
    {
        // No memory
        return NULL;
    }
    memset(output, 0, sizeof(sizeof(int)*shapeW[1]));
    matmul(input, W_t, shapeW[0], shapeW[1], output);

    shapeOutput[0] = shapeInput[0];
    shapeOutput[1] = shapeW[1];

    free(W_t);

    return output;
}
