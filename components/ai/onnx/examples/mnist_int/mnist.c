#include <stdio.h>
#include <stdlib.h>
#include "mnist_int.h"
#include "onnx.h"

static const char codeLib[] = "@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\"^`'.   ";
int data[5]={1.5 , 2.5 , 3.5 , 4.5 , 5.5};
static const int img[2][784] = {IMG0, IMG1};

static const int img1[784] = {1,2,3,4,5};

int hello()
{
    printf("hello pnnx\r\n");
    return 0;
}

void print_img(void * buf)
{
    int index = 0;
    //char ch = '@';
    int x = 0;
    int y = 0;
    
    printf("test2\r\n");

    for(y = 0; y < 28; y++) 
    {
        for (x = 0; x < 28; x++) 
        {
            
            index = 0;
            if(((int*)buf)[y*28+x] > 600)
            {
                index =69;       
            }
            if(index < 0)
            {
                index = 0;
            }
            
            
            printf("%c",codeLib[index]);
            printf("%c",codeLib[index]);
        }
        printf("\r\n");
    }
}

int mnist()
{
    printf("test1\r\n");
    int img_index = 1;
    print_img(img[img_index]);

    printf("img ok\r\n");

    // 1. Conv2D
    int64_t shapeW3[] = {2, 1, 3, 3};
    int64_t dimW3 = 4;
    int64_t permW3_t[] = { 0, 2, 3, 1};
    int* W3_t = transpose(W3, shapeW3, dimW3, permW3_t);

    printf("transpose ok\r\n");

    int* conv1 = (int*) malloc(sizeof(int)*28*28*2);
    memset(conv1, 0, sizeof(sizeof(int)*28*28*2));

    conv2D(img[img_index], 28, 28, 1, W3, 2, 3, 3, 1, 1, 1, 1, B3, conv1, 28, 28);
    
    free(W3_t);

    printf("Conv2D ok \r\n");

    // 2. Relu
    int* relu1 = (int*) malloc(sizeof(int)*28*28*2);
    relu(conv1, 28*28*2, relu1);

    free(conv1);

    printf("Relu ok\r\n");

    // 3. Maxpool
    int* maxpool1 = (int*) malloc(sizeof(int)*14*14*2);
    memset(maxpool1, 0, sizeof(sizeof(int)*14*14*2));
    maxpool(relu1, 28, 28, 2, 2, 2, 0, 0, 2, 2, 14, 14, maxpool1);

    free(relu1);

    printf("Maxpool ok\r\n");

    // 4. Conv2D
    int64_t shapeW2[] = {2, 2, 3, 3};
    int64_t dimW2 = 4;
    int64_t perm_t[] = { 0, 2, 3, 1};
    int* W2_t = transpose(W2, shapeW2, dimW2, perm_t);

    int* conv2 = (int*) malloc(sizeof(int)*14*14*2);
    memset(conv2, 0, sizeof(sizeof(int)*14*14*2));
    conv2D(maxpool1, 14, 14, 2, W2_t, 2, 3, 3, 1, 1, 1, 1, B2, conv2, 14, 14);

    free(W2_t);
    free(maxpool1);

    printf("Conv2D ok\r\n");

    // 5. Relu
    int* relu2 = (int*) malloc(sizeof(int)*14*14*2);
    relu(conv2, 14*14*2, relu2);

    free(conv2);

    printf("Relu ok\r\n");

    // 6. Maxpool
    int* maxpool2 = (int*) malloc(sizeof(int)*7*7*2);
    memset(maxpool2, 0, sizeof(sizeof(int)*7*7*2));
    maxpool(relu2, 14, 14, 2, 2, 2, 0, 0, 2, 2, 7, 7, maxpool2);

    free(relu2);

    printf("Maxpool ok\r\n");

    // Flatten NOT REQUIRED

    // 7. Dense
    int64_t shapeW1[] = {98, 4};
    int64_t dimW1 = 2;
    int64_t permW1_t[] = { 1, 0};
    int* W1_t = transpose(W1, shapeW1, dimW1, permW1_t);

    int* dense1 = (int*) malloc(sizeof(int)*4);
    memset(dense1, 0, sizeof(sizeof(int)*4));
    dense(maxpool2, W1_t, 98, 4, B1, dense1);

    free(W1_t);
    free(maxpool2);

    printf("Dense ok\r\n");

    // 8. Dense
    int64_t shapeW[] = {4, 10};
    int64_t dimW = 2;
    int64_t permW_t[] = { 1, 0};
    int* W_t = transpose(W, shapeW, dimW, permW_t);

    int* dense2 = (int*) malloc(sizeof(int)*10);
    memset(dense2, 0, sizeof(sizeof(int)*10));
    dense(dense1, W_t, 4, 10, B, dense2);

    free(W_t);
    free(dense1);

    printf("Dense ok\r\n");

    // 9. Softmax
    int* output = (int*) malloc(sizeof(int)*10);
    memset(output, 0, sizeof(sizeof(int)*10));
    softmax(dense2, 10, output);

    printf("Softmax ok\r\n");
    
    int max = 0;
    int min = output[0];
    int max_index = 0;
    int min_index = 0;
    printf("\n\rPredictions: \n\r");
    for(int i = 0; i < 10; i++)
    {
        printf("%d ", output[i]);
        if(output[i] > max)
        {
            max = output[i];
            max_index = i;
        }
        if(output[i] < min)
        {
            min = output[i];
            min_index = i;
        }
    }
    printf("\n\r");
    printf("\n\rThe number is %d\n\r", min_index);

    free(dense2);
    free(output);

    printf("Result ok\r\n");

    return 0;
}
