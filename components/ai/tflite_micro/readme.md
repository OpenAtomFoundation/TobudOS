# Tensorflow Lite Micro 使用指南

## 1.组件介绍

TensorFlow Lite Micro组件以下简称TFLu)是Google TensorFlow团队针对微处理器(以下简称MCU)专门设计的端侧推理框架，该推理框架主要解决在资源, 功耗, 性能等资源受限的嵌入式系统中, 部署基于Tensorflow Lite框架实现深度学习模型任务。

## 2.目录结构

组件内部整体的目录结构如下图所示：

```
tflite_micro
    ├─Source           		//放置了TensorFlow Lite Micro的全部源码
    ├─KEIL  			//针对KEIL环境需要的适配文件
    ├─ARM_CortexM4_lib		//针对ARM Cortex M4生成的tensorflow_lite_micro.lib库
    ├─ARM_CortexM7_lib		//针对ARM Cortex M7生成的tensorflow_lite_micro.lib库
    ├─ARM_CortexM55_lib		//针对ARM Cortex M55生成的tensorflow_lite_micro.lib库
    ├─LICENSE	
    ├─readme.md				
    └─TFlite_Micro_Component_User_Guide.md //本组件使用指南
    
```

其中，KEIL文件夹中放置了针对KEIL编程环境需要额外添加的源文件，如果使用的是KEIL编程环境则需要将其中的源文件一同加入工程。

ARM_CortexM4_lib文件夹、ARM_CortexM55_lib和ARM_CortexM7_lib文件夹中分别放置了针对ARM Cortex M4、ARM Cortex M55和ARM Cortex M7内核生成的tensorflow_lite_micro.lib库，如果使用的芯片中采用了对应系列的内核可以直接将其中的tensorflow_lite_micro.lib库加入工程，并包含相关的头文件，就可以方便的集成Tensorflow Lite Micro工具。

## 3.移植案例

参考[Neclue STM32L496RG案例](../../../board/NUCLEO_STM32L496ZG/KEIL/tflitemicro_person_detection/TFlite_Micro_Demo移植参考指南（Keil版）.md)
