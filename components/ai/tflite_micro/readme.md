# Tensorflow Lite Micro 使用指南

## 1. 组件介绍

TensorFlow Lite Micro 组件是 Google TensorFlow 团队针对微处理器平台设计的端侧推理框架，该推理框架主要解决在资源, 功耗, 性能受限的嵌入式系统中, 如何简单快速高效部署 Tensorflow Lite 深度学习模型。CMSIS 全称 Software Interface Standard for Arm Cortex-based Microcontrollers，其中 CMSIS-NN 组件为 AI 加速库，包含高效的神经网络算子来减小内存占用和最大化硬件性能。相关代码和文档已经开源并在多个平台上得到验证（ https://www.keil.com/pack/doc/CMSIS/NN/html/index.html ）。

## 2. 目录结构

组件目录结构如下图所示：

```
tflite_micro
    ├─Source           		// TensorFlow Lite Micro 的全部源码
    ├─KEIL  			    //针对 KEIL 开发环境所需的适配文件
    ├─ARM_CortexM4_lib		//针对 ARM Cortex M4 生成的 tensorflow_lite_micro.lib 库
    ├─ARM_CortexM7_lib		//针对 ARM Cortex M7 生成的 tensorflow_lite_micro.lib 库
    ├─LICENSE	
    ├─readme.md				
    └─TFlite_Micro_Component_User_Guide.md //本组件使用指南
    
```

其中，KEIL 文件夹包含针对 KEIL 编程环境需要额外添加的源文件（将其中的源文件一同加入工程）。

ARM_CortexM4_lib 和 ARM_CortexM7_lib 两个文件夹中存放了针对 Cortex M4、Cortex M7 平台预编译的 tensorflow_lite_micro.lib 库，优化后的性能数据可以参考附录部分；如果在开发过程中采用 ARM Cortex M4 或 Cortex M7 系列芯片可以直接使用对应的 tensorflow_lite_micro.lib 库，并包含相关的头文件，极大的缩短了开发时间。

在 [TFlite_Micro_Component_User_Guide.md](./TFlite_Micro_Component_User_Guide.md) 中提供了本组件的使用说明以及上述 .lib 库的制作方法。

## 3. 组件使用案例

基于本组件我们以 STM32L496RG 为例设计了行人检测案例，具体使用流程可参考[Neclue STM32L496RG案例](../../../board/NUCLEO_STM32L496ZG/KEIL/tflitemicro_person_detection/TFlite_Micro_Demo移植参考指南（Keil版）.md)。

## 4. 附录：Tensorflow Lite Micro 性能优化情况

- 硬件平台：Necluo STM32L496ZG
- 加速库：ARM CMSIS-NN
- 测试输入图片：`tensorflow\lite\micro\tools\make\downloads\person_model_int8` 目录中 `person_image_data.cc` 和 `no_person_image_data.cc` 保存的 96 * 96 pixels ( uint_8 ) 灰度图。
- 单次执行和 10 次累计执行的测试结果如下：

|                      Case                      |   Disable ARM-CMSIS-NN   |   Enable ARM-CMSISNN   | Improvement |
| :--------------------------------------------: | :----------------------: | :------------------: | :---------: |
|          Initialize_Benchmark_Runner           |     65 ticks (65 ms)     |   66 ticks (66 ms)   |      *      |
|              Run_Single_Iteration              |  12476 ticks (12476 ms)  |  633 ticks (633 ms)  |   19.71X    |
|   Person_Detection_Ten_Ierations_With_Person   | 124769 ticks (124769 ms) | 6324 ticks (6324 ms) |   19.73X    |
| Person_Detection_Ten_Ierations_With_out_Person | 124770 ticks (124770 ms) | 6325 ticks (6325 ms) |   19.72X    |

## 5. TencentOS-tiny AI 开发组件

TencentOS-tiny 已将 Tensorflow Lite Micro 以及 CMSIS-NN 集成到 AI 组件中，并通过其他组件与腾讯云无缝相连，打通从云到端整条链路，助力 AI 的发展与落地。随着越来越多的厂商采用 Arm Cortex M55 和 Ethos U NPU IP，相信未来端侧AI的应用会更加广阔。

TencentOS-tiny AI组件：https://github.com/rceet/tencentos-tiny-with-tflitemicro-and-iot/tree/master/components/ai/tflite_micro

Arm Cortex M55：https://www.arm.com/products/silicon-ip-cpu/cortex-m/cortex-m55

Arm Ethos U55：https://www.arm.com/products/silicon-ip-cpu/ethos/ethos-u55

Arm Ethos U65：https://www.arm.com/products/silicon-ip-cpu/ethos/ethos-u65
