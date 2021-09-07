#include "onnx.h"

void dense(const int *input,              // pointer to vector
           const int *weight,             // pointer to matrix
           const uint16_t dim_vec,         // length of the vector
           const uint16_t num_of_rows,     // numCol of A
           const int *bias,
           int *output)                   // output operand
{
    for (int i = 0; i < num_of_rows; i++)
    {
        int ip_out = bias[i];
        for (int j = 0; j < dim_vec; j++)
        {
            ip_out += input[j] * weight[i * dim_vec + j];
        }
        output[i] = ip_out;
    }
}
