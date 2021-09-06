#include "onnx.h"

void relu(const int *input, uint32_t size, int* output)
{
    uint32_t i;
    memcpy(output, input, sizeof(int) * size);
    for (i = 0; i < size; i++)
    {
        if (output[i] < 0)
            output[i] = 0;
    }
}

int* relu_layer(Onnx__GraphProto* graph, const int *input, int64_t* shapeInput, int64_t* shapeOutput, const char* layer_name)
{
    //assert(graph != NULL && input != NULL && layer_name != "" );

    int64_t len = shapeInput[0] * shapeInput[1] * shapeInput[2];
    int* output = (int*) malloc(sizeof(int)*len);
    memset(output, 0, sizeof(sizeof(int)*len));

    relu(input, len, output);

    memcpy(shapeInput, shapeOutput, sizeof(int64_t)*3);

    return output;
}
