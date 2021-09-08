
# Neural Network on Microcontroller (NNoM)
[![Build Status](https://travis-ci.com/majianjia/nnom.svg?branch=master)](https://travis-ci.com/majianjia/nnom)
[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)
[![DOI](https://zenodo.org/badge/166869630.svg)](https://zenodo.org/badge/latestdoi/166869630)

NNoM is a high-level inference Neural Network library specifically for microcontrollers. 

介绍详情可以参考：https://github.com/majianjia/nnom

原作者提供了基于keras的训练模型方法，以及如何配置NNoM的详细文档介绍



本项目提供一个基于NNoM的软件包，方便在tos上的快捷移植（测试通过平台为stm32l496ZG）

mnist示例可以参考board/NUCLEO_STM32L496ZG/KEIL/nnom_mnist

## 在TencentOS-tiny上的使用说明

1. 在keil工程里添加components / ai / nnom中的src文件夹下的backends、core、layers三个文件夹中的全部.c文件
2. 在keil工程中包含inc和port文件夹中的全部头文件
3. 在nnom_port.h指定内存使用方法（测试示例中开启了 NNOM_USING_STATIC_MEMORY宏 ），若使用非静态内存方法需要将nnom_malloc(n)和nnom_free(n)定义为os本身的内存api，对tos是tos_mmheap_alloc(n)和tos_mmheap_free(n)
4. 若使用静态内存，则需要定义static_buf[size]并使用nnom_set_static_buf(static_buf, sizeof(static_buf))函数去指定静态内存地址与大小，并根据模型需要调整静态内存大小。
5. 编写示例函数，参考：example/nnom_mnsit中的nnom_mnist_example写法，按照需要实现系统api，比如使用tos_systick_get()去获取系统tick，从而计算推理时间。



## 注意事项

在keil下确认printf已经成功实现（检查microlib选项），并注意选择ARM Compiler为Use default compiler version 5



## Licenses

NNoM is released under Apache License 2.0 since nnom-V0.2.0. 
License and copyright information can be found within the code.

