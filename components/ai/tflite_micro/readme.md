# Tensorflow Lite Micro 使用指南

## 1.组件介绍

TensorFlow Lite Micro 组件是 Google TensorFlow 团队针对微处理器平台专门设计的端侧推理框架，该推理框架主要解决在资源, 功耗, 性能等资源受限的嵌入式系统中, 部署基于 Tensorflow Lite 框架实现深度学习模型任务。

## 2.目录结构

组件内部整体的目录结构如下图所示：

```
tflite_micro
    ├─Source           		//放置了 TensorFlow Lite Micro 的全部源码
    ├─KEIL  			//针对 KEIL 环境需要的适配文件
    ├─ARM_CortexM4_lib		//针对 ARM Cortex M4 生成的 tensorflow_lite_micro.lib 库
    ├─ARM_CortexM7_lib		//针对 ARM Cortex M7 生成的 tensorflow_lite_micro.lib 库
    ├─LICENSE	
    ├─readme.md				
    └─TFlite_Micro_Component_User_Guide.md //本组件使用指南
    
```

其中，KEIL 文件夹中放置了针对 KEIL 编程环境需要额外添加的源文件，如果使用的是 KEIL 编程环境则需要将其中的源文件一同加入工程。

ARM_CortexM4_lib 和 ARM_CortexM7_lib 两个文件夹中存放了针对 Cortex M4、M7 平台预先优化、编译好的 tensorflow_lite_micro.lib 库，优化后的性能数据可以参考附录部分；这样做的目的是：如果在开发过程中使用了 ARM Cortex M4 或 M7 系列的芯片可以直接使用对应的 tensorflow_lite_micro.lib 库，并包含相关的头文件，可以极大的简化神经网络任务的部署流程。

## 3.移植案例

参考[Neclue STM32L496RG案例](../../../board/NUCLEO_STM32L496ZG/KEIL/tflitemicro_person_detection/TFlite_Micro_Demo移植参考指南（Keil版）.md)

## 4. 附录：Tensorflow Lite Micro 性能优化情况

- 硬件平台：Necluo STM32L496ZG
- 涉及的加速库：ARM CMSIS-NN
- 测试输入图片：`tensorflow\lite\micro\tools\make\downloads\person_model_int8` 目录 `person_image_data.cc` 和 `no_person_image_data.cc` 中保存的 96 * 96 ( uint_8 ) 灰度图。
- 单次执行和 10 次迭代的测试结果如下：

|                      Case                      |   Without ARM-CMSIS-NN   |   With ARM-CMSISNN   | Improvement |
| :--------------------------------------------: | :----------------------: | :------------------: | :---------: |
|          Initialize_Benchmark_Runner           |     65 ticks (65 ms)     |   66 ticks (66 ms)   |      *      |
|              Run_Single_Iteration              |  12476 ticks (12476 ms)  |  633 ticks (633 ms)  |   19.71X    |
|   Person_Detection_Ten_Ierations_With_Person   | 124769 ticks (124769 ms) | 6324 ticks (6324 ms) |   19.73X    |
| Person_Detection_Ten_Ierations_With_out_Person | 124770 ticks (124770 ms) | 6325 ticks (6325 ms) |   19.72X    |

