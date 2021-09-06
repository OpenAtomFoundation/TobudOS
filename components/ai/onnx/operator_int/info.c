#include "onnx.h"

void onnx_tensor_info(const int* A, int64_t* shape, int64_t dim)
{
    int elem = 1;
    for(int i = 0; i < dim; i++)
    {
        elem = elem * shape[i];
    }

    printf("Array size: %d\n", elem);
    for(int i = 0; i < elem; i++)
    {
        printf( "%f ", A[i] );
        int split = 1;
        for(int j = dim-1; j > 0; j--)
        {
            split = split * shape[j];
            if( (i+1) % split == 0)
            {
                printf("\n");
            }
        }
    }
}
