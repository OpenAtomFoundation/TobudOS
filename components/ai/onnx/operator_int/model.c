#include <inttypes.h>
#include "onnx.h"

int* onnx_model_run(Onnx__ModelProto* model, int* input, int64_t* shapeInput)
{
    int64_t* shapeOutput = (int64_t*) malloc(sizeof(int64_t)*3);
    shapeOutput[0] = -1; shapeOutput[1] = -1; shapeOutput[2] =  -1;

    Onnx__NodeProto* node = onnx_graph_get_node_by_input(model->graph, model->graph->input[0]->name);

    int i = 0;
    int* output;
    while(node != NULL)
    {
        printf("[%2d] %-10s %-20s ", i++, node->op_type, node->name);
        if(strcmp(node->op_type, "Conv") == 0)
        {
            output = conv2D_layer(model->graph, input, shapeInput, shapeOutput, node->name);
        }
        else if(strcmp(node->op_type, "Relu") == 0)
        {
            output = relu_layer(model->graph, input, shapeInput, shapeOutput, node->name);
        }
        else if(strcmp(node->op_type, "MaxPool") == 0)
        {
            output = maxpool_layer(model->graph, input, shapeInput, shapeOutput, node->name);
        }
        else if(strcmp(node->op_type, "Softmax") == 0)
        {
            output = softmax_layer(model->graph, input, shapeInput, shapeOutput, node->name);
        }
        else if(strcmp(node->op_type, "MatMul") == 0)
        {
            output = matmul_layer(model->graph, input, shapeInput, shapeOutput, node->name);
        }
        else if(strcmp(node->op_type, "Add") == 0)
        {
            output = add_layer(model->graph, input, shapeInput, shapeOutput, node->name);
        }
        else if(strcmp(node->op_type, "Identity") == 0)
        {
            node = onnx_graph_get_node_by_input(model->graph, node->output[0]);
            printf("\n");

            continue;
        }
        else if(strcmp(node->op_type, "Transpose") == 0)
        {
            node = onnx_graph_get_node_by_input(model->graph, node->output[0]);
            printf("\n");

            continue;
        }
        else if(strcmp(node->op_type, "Reshape") == 0)
        {
            shapeOutput[1] = shapeOutput[0] * shapeOutput[1] * shapeOutput[2]; 
            shapeOutput[2] = 1; 
            shapeOutput[0] = 1; 
            printf("[%2" PRId64 ", %2" PRId64 ", %2" PRId64 "] --> [%2" PRId64 ", %2" PRId64 ", %2" PRId64 "]\n", shapeInput[0], shapeInput[1], shapeInput[2], shapeOutput[0], shapeOutput[1], shapeOutput[2]);

            // free(input);
            // input = output;
            memcpy(shapeInput, shapeOutput, sizeof(int64_t)*3);

            node = onnx_graph_get_node_by_input(model->graph, node->output[0]);
            continue;
        }
        else
        {
            printf("Unsupported operand: %s\n", node->op_type);
        }
        printf("[%2" PRId64 ", %2" PRId64 ", %2" PRId64 "] --> [%2" PRId64 ", %2" PRId64 ", %2" PRId64 "]\n", shapeInput[0], shapeInput[1], shapeInput[2], shapeOutput[0], shapeOutput[1], shapeOutput[2]);

        free(input);
        input = output;
        memcpy(shapeInput, shapeOutput, sizeof(int64_t)*3);

        node = onnx_graph_get_node_by_input(model->graph, node->output[0]);
    }
    output = input;
    free(shapeOutput);

    return output;
}
