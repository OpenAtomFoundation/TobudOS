# Tensorflow Lite Micro 使用指南

## 1.组件介绍

​	TensorFlow Lite Micro组件的前身(以下简称TFLu)是Google TensorFlow团队针对微处理器(以下简称MCU)专门设计的端侧推理框架，该推理框架主要解决在资源, 功耗, 性能等资源受限的嵌入式系统中, 部署基于Tensorflow Lite框架实现深度学习模型任务；本组件是基于原生Google TensorFlow Lite Micro框架对Tencent OS-tiny进行了定制化适配，为Tencent OS-tiny赋能AI。

## 2.目录结构

组件内部整体的目录结构如下图所示：

```
tflite_micro
    ├─KEIL  				//针对KEIL环境需要的适配文件
    ├─ARM_CortexM4_lib		//针对ARM Cortex M4生成的tflite person detect .lib库
    └─ARM_CortexM7_lib		//针对ARM Cortex M4生成的tflite person detect .lib库
```

其中，KEIL文件夹中放置了针对KEIL编程环境需要额外添加的源文件，如果使用的是KEIL编程环境则需要将其中的源文件一同加入工程。

ARM Cortex M4_lib文件夹和ARM Cortex M7_lib文件夹中主要放置了基于ARM Cortex M4和Cortex M7内核生成的tflite.lib库，如果使用的是基于ARM Cortex M4系列的板子可以直接将其中的tflite.lib库加入工程，并包含相关的头文件之后就可以完成编译运行，方便模块化集成。

## 3.移植案例

参考[Neclue STM32L496RG案例](./test)