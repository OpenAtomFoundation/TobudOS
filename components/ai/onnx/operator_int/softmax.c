#include "onnx.h"
//#include<math.h>


int abs_core(int x)
{
    return x > 0?x:-x;
}

int exp_core(int x)
{
           x = 1 + (x << 8);

           x *= x;       x *= x;       x *= x;       x *= x;

           x *= x;       x *= x;       x *= x;       x *= x;

           return x;

}

void softmax(const int *input, const uint32_t dim_vec, int *output)
{
    long long sum = 0;

    for(int i = 0; i < dim_vec; i++)
    {
        output[i] = input[i] >> 16;
    }

    /*
    for(int i = 0; i < dim_vec; i++)
    {
        output[i] = abs_core(input[i] >> 16);
        sum = sum + (output[i]);
    }
    printf("sum = %ld\r\n" , sum);
    for(int i = 0; i < dim_vec; i++)
    {
        //output[i] = output[i] / (sum);
        output[i] = sum / output[i];
        //output[i] = output[i];
    }*/
}

int* softmax_layer(Onnx__GraphProto* graph, const int *input, int64_t* shapeInput, int64_t* shapeOutput, const char* layer_name)
{
    //assert(graph != NULL && input != NULL && layer_name != "" && shapeInput[1] > 0);

    int* output = (int*) malloc(sizeof(int)*shapeInput[1]);
    memset(output, 0, sizeof(sizeof(int)*shapeInput[1]));
    softmax(input, shapeInput[1], output);

    memcpy(shapeInput, shapeOutput, sizeof(int64_t)*3);

    return output;
}
