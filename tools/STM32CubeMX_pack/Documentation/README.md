![](media/7cdcedffbe9a424e13fbb8b9d86c710d.png)

# 基于 STM32Cube MX 开发的 TencentOS-Tiny 软件包

TencentOS-Tiny software package based on STM32Cube MX

2021/11/10

东南大学

导师：汪礼超

学员：崔林威

Email：1797878653@qq.com

<https://github.com/OpenAtomFoundation/TencentOS-tiny>

# 摘要

腾讯物联网操作系统（TencentOS
tiny）是腾讯面向物联网领域开发的实时操作系统，具有低功耗，低资源占用，模块化，可裁剪等特性。TencentOS
tiny 提供了最精简的 RTOS 内核，内核组件可裁剪可配置，可灵活移植到多种终端
MCU 上；基于 RTOS 内核提供了 COAP/MQTT/TLS/DTLS 等常用物联网协议栈及组件，方便用户快速接入腾讯云物联网通信 IoT
Hub；同时，为物联网终端厂家提供一站式软件解决方案，方便各种物联网设备快速接入腾讯云，可支撑智慧城市、智能穿戴、车联网等多种行业应用。

为了减少开发人员移植 TencentOS
tiny 到 STM32 系列单片机上的开发时间，研究了**STM32Cube MX 软件包的制作**，基于 STM32
PackCreator 完成了 TencentOS Tiny 软件包的封装，并在 STM32Cube IDE 和 MDK-ARM
v5 上完成了软件包的**移植测试**，从而用户在安装本软件包后，能够使用 pack**在 STM32Cube
MX 上直**接生成适合不同 MCU 的 TencentOS Tiny 工程。

目录

[1、STM32 Cube MX 软件包介绍](#1stm32-cube-mx软件包介绍)

[1.1 软件包简介](#11-软件包简介)

[1.2 软件包制作](#12-软件包开发)

[1.2.1 软件包开发过程](#121-软件包开发过程)

[1.2.2 PDSC 文件的编写](#122-利用stm32packcreator制作软件包)

[1.2.3 生成软件包](#_Toc82013881)

[2、TencentOS-tiny 软件包](#2tencentos-tiny软件包)

[2.1 软件包内容](#21-软件包内容)

[2.2 软件包安装](#22-软件包安装)

[3、软件包测试](#3软件包测试)

[3.1 ARM 内核移植 TencentOS
tiny 软件包](#31-stm32-mcu移植测试生成stm32-cubeide工程)

[3.2 STM32 不依赖裸机工程移植](#32-stm32-board移植测试生成mdk-arm-v5工程)

[3.3 单片机裸机工程移植](#33-单片机测试)

[4、总结](#_Toc82013889)

[5、开发参考](#4开发参考)

[6、附录-移植配置参考](#_Toc82013891)

[6.1 MDK5.14 版本移植到 ARM 内核](#_Toc82013892)

[6.1.1 Cortex-M0 内核移植](#_Toc82013893)

[6.1.2 Cortex-M0+内核移植](#_Toc82013894)

[6.1.3 Cortex-M3 内核移植](#_Toc82013895)

[6.1.4 Cortex-M4 内核移植](#_Toc82013896)

[6.1.5 Cortex-M7 内核移植](#_Toc82013897)

[6.2 MDK5.14 版本移植到基于 ARM 内核的芯片](#_Toc82013898)

[6.2.1 移植到 stm32f103c8 芯片](#_Toc82013899)

[6.2.2 移植到 stm32f767igt 芯片](#_Toc82013900)

[6.3
MDK5.30 和 MDK5.35 版本移植（Cortex-M0+、0、3、4、7 内核和芯片）](#_Toc82013901)

[6.4 MDK5.30 和 MDK5.35 版本移植（Cortex-M23、33）](#_Toc82013902)

[6.4.1 Cortex-M23 内核移植](#_Toc82013903)

[6.4.2 Cortex-M33 内核移植](#_Toc82013904)

# 1、STM32 Cube MX 软件包介绍

## 1.1 软件包简介

在进行嵌入式软件开发时，ARM 为我们提供了软件包功能，能够将软件算法等模块进行集成封装，从而方便第三方用户使用。ARM 软件包能够为微控制器设备和开发板提供支持，包含软件组件（Software
Component）如驱动程序和中间件，还可以包含示例项目和代码模板等，主要有以下类型的软件包：

(1) 器件系列包（Device Family
Pack）：由硅供应商或工具供应商生成，为特定的目标微控制器创建软件应用提供支持；

(2) 板级支持包（Board Support
Pack）：由电路板供应商发布，为安装在电路板上的外围硬件提供软件支持。

(3) CMSIS 软件包：由 ARM 提供，包括对 CMSIS 核心、DSP 和 RTOS 的支持；

(4) 中间件包（Middleware
Pack）：由芯片供应商、工具供应商或第三方创建；通过提供对常用软件组件（如软件堆栈、特殊硬件库等）的软件集成，从而减少开发时间；

(5) 内部组件（In-house components）：由工具用户开发，用于内部或外部分发。

软件组件包括以下几部分：

(1) 源代码、库、头文件/配置文件和文档；

(2) 完整的示例项目，展示了软件组件的使用，可以下载并在评估硬件上执行；

(3) 代码模板，方便使用软件组件。

一个完整的软件包是一个 ZIP 文件，包含所有需要的软件库和文件，以及一个包含软件包所有信息的包描述文件（PDSC 文件），ARM 软件包的结构是在 CMSIS 中定义的(<http://www.keil.com/CMSIS/Pack>)。

STM32
CubeMX 是 ST 公司推出了专门用于生成 STM32 的 HAL 代码的代码生成软件，可以通过可视化界面完成工程的配置，同时生成能够在 STM32
CubeMX、Keil 等软件中运行的工程。STM32 Cube
MX 软件包是在 ARM 软件包的基础上，结合 ST 公司提供的软件包生成工具 STM32
PackCreator 制作的，利用这个软件我们可以很方便地以图形化界面的方式进行软件包的制作。

如下图所示，使用 STM32PackCreator 需要我们的操作系统安装 Java™ Runtime Environment
(JRE)和 JavaFX™，目前 STM32CubeMX 已经更新到了 6.3 版本，但这个版本并没有集成 JavaFX，最为方便的方式是安装 STM32
CubeMX 6.2 版本，省去了 JavaFX 的配置问题。

![](media/e7f22ab88c1c1a558d17287582631d83.png)

图 1.1 STM32PackCreator 使用要求

具体步骤如下：

（1）安装 STM32CubeMX6.2.0：[STM32CubeMX - STM32Cube 初始化代码生成器 -
STMicroelectronics](https://www.st.com/zh/development-tools/stm32cubemx.html#get-software)；

![](media/bece761bd6b5e020a7eb2f315023f5d2.png)

图 1.2 下载 STM32CubeMX6.2.0

（2）双击安装目录下的 STM32PackCreator，此时会提示需要安装 JRE，点击确定，跳转到[https://java.com/zh-CN/download/](https://aijishu.com/link?target=https%3A%2F%2Fjava.com%2Fzh-CN%2Fdownload%2F)，然后下载安装。

![](media/95511cb01e058ea0e8b39f299f69b5e3.png)

![](media/fd5ec491a0fe52b056f0c5e0f1a862b7.png)

![](media/18bd45f297b4028820de6851bd922274.png)

图 1.3 STM32PackCreator 使用前的配置

（3）然后再次双击打开 STM32PackCreator.exe，STM32 PackCreator 的界面如下图所示。

![](media/bbc029d5204e1f954af5c3adb0717d00.png)

图 1.4 STM32 PackCreator 界面

## 1.2 软件包开发

### 1.2.1 软件包开发过程

软件包的开发过程相当于完成了一项产品的制作，因此引入产品生命周期管理（PLM）的概念，PLM 包括以下四个阶段：（1）概念的产生，基于软件包需求进行产品定义，并创建第一个功能原型；（2）设计，根据技术特征和要求，进行原型测试和产品的实施，通过广泛的测试验证产品的功能与规格；（3）发布，产品被制造出来并推向市场；（4）服务，对产品的维护，包括对客户的支持，最后不断优化，结束产品的周期。

在制作软件包时，主要面临以下几个过程：

![](media/b2be2d798f47f13077e4eafee94fad29.png)

图 1.5 软件包开发流程

首先，根据特定组件生成软件包即根据需求将相应的头文件、库文件等软件组件利用 PDSC 文件进行组织，在组织完成后即可利用软件包生成工具生成对应版本的软件包，然后对新生成的软件包进行测试，给出示例测试程序，再将其包含如 PDSC 文件中，最后经测试完成后生成最终的软件包。

### 1.2.2 利用 STM32PackCreator 制作软件包

STM32PackCreator 能够通过图形化界面设计软件包，省去了我们手动编写 PDSC 文件和软件包生成脚本文件的步骤，使用起来非常方便快捷，以下是使用 STM32PackCreator 制作软件包的步骤。

（1）打开 STM32 PackCreator 后，点击 File-new project from scratch。

![](media/075a0efa2aac322fa7f04f18e494170c.png)

图 1.6 生成新软件包

（2）进行软件包的配置，包括软件包生成的文件夹、软件包的 vendor、name 和 description 属性。

![](media/cc16fd410629fd560c7438445e30084f.png)

图 1.7 软件包初始配置

（3）配置完成后，可以看到有四个属性栏，他们分别代表软件包的基本配置、版本控制、组件内容和附加文件，如图 1.7 所示：

![](media/59fc851e13b1db9fa9967a14d720317b.png)

图 1.8 软件包属性栏

（4）如下为制作 Tencent OS-tiny 软件包时的配置：

![](media/08f21f42eb03be8560ac0aa54248040d.png)

（a）基础配置

![](media/60608e9a968a8164576bec6fe00bdb99.png)

（b）软件包版本控制

图 1.9 软件包配置

（5）制作软件包的关键在于 Pack
Details 的配置，这里实际上就是手工编写 PDSC 文件的图形化界面显示，我们按照 ARM
PDSC 文件的编写规范设计 condition、设计组件结构、添加组件即可。

![](media/0700d9cae7ef1ae1de93fc03bb09668d.png)

图 1.10 软件包组件配置

（6）添加附件，在 Additional
Files 中，我们需要添加上软件包的说明文件（README.md）、利用软件包生成的示例工程（Projects）和 STM32CubeMX 文件夹，此时我们点击 File-Save
and Generate
Pack，可以在之前设定的路径下看到软件包。然后我们再次将这个软件包也加入到 Additional
Files 中，如图 1.11 所示，我们再次点击 File-Save and Generate
Pack，就可以生成一个完整的 STM32CubeMX 软件包了。

![](media/4315430d5fce58833e07811993aab0ad.png)

图 1.11 STM32CubeMX 文件夹

![](media/9da795e383943b24b6502dcecbfc1720.png)

图 1.12 Additional Files 界面

![](media/8f3894f199a4fd3f480f2b9d8881e45f.png)

![](media/a5518d6314ecc5d20dc9f5ddd21e4952.png)

图 1.13 生成软件包

（7）另外，STM32PackCreator 提供了参数预先配置的功能，使得我们在 STM32 Cube
MX 生成工程时，可以对软件包的参数进行提前配置。首先在图 1.13(a)中添加我们需要用户提前配置的一系列参数，然后在图 1.13(b)中制作一个模板文件，此处我设置为头文件的形式，然后将之前设置的一系列参数添加过来并保存，这样用户在使用软件包生成工程的时候就可以预先配置参数，并在生成工程时自动生成名为 tos_config 的文件。

![](media/7cfea692c4f0cf573994647241c2a666.png)
![](media/0dc47d61ee9335677e6f1230aaa2aa22.png)

（a） （b）

图 1.14 软件包参数配置

# 2、TencentOS-tiny 软件包

## 2.1 软件包内容

结合 TencentOS tiny 的算法架构，本文设计的软件包包括如表 2-1 所示的内容：

表 2-1 软件包内容

| 内容     | 功能                                                                                                                                |                                              |
| -------- | ----------------------------------------------------------------------------------------------------------------------------------- | -------------------------------------------- |
| arch     | 包括 TencentOS-tiny\\arch\\arm 下内核为 Cortex-M0+、Cortex-M0、Cortex-M3、Cortex-M4、Cortex-M7、Cortex-M23、Cortex-M33 的 arch 文件 |                                              |
| kernel   | 包括 TencentOS-tiny\\kernel 下的 core、hal 路径中的文件、tos_config 文件                                                            |                                              |
| cmsis_os | 对应 TencentOS-tiny\\osal\\cmsis_os 的文件                                                                                          |                                              |
| example  | helloworld_main                                                                                                                     | 用于测试软件包的 main 文件                   |
|          | mcu_it.c                                                                                                                            | 移植软件包时需要按照该文件对中断函数进行修改 |
|          | mcu_platform.h                                                                                                                      | 用户可在此文件在添加对应单片机的头文件       |

软件包具有以下功能：

（1）软件包针对 ARMCortex-M0+、Cortex-M0、Cortex-M3、Cortex-M4、Cortex-M7、Cortex-M23 和 Cortex-M33 内核进行了 TencentOS
tiny 软件的封装，用户在安装软件包后能够快速将 TencentOS tiny 相应内核的 Keil 工程中；

（2）软件包能够自动适应用户所选的内核，arch 文件能够根据内核自动显示，从而方便用户使用；

（3）用户在勾选一个组件时，软件包会自动提示还需要勾选其他模块，并可利用界面中的 Resolve 一键勾选，防止遗漏；

（4）用户可在 STM32CubeMX 上修改 tos_config 文件中的参数，对 TencentOS
tiny 的功能进行裁剪。

## 2.2 软件包安装

接下来介绍 Tencent.TencentOS-tiny 软件包的安装，首先打开 STM32 Cube
MX 软件，如图 2.1(a)所示，点击 INSTALL/REMOVE，然后将软件包拖进来，点击 I agree to
all the terms of the preceding License
Agreement，再点击 next 进行安装，安装完成界面如图 2.1(b)所示。

![](media/ba55cec0f5647efd53f9a8f4ee4dd49e.png)![](media/33809a718992a785c9601305ca1b2706.png)

（a） （b）

图 2.1 软件包安装

安装好的软件包可以在安装路径中查看内容，路径在 Help-Updater Settings 中查看。

![](media/078139b6bd2274da5fa133a02e31707a.png)

(a)安装路径

![](media/595f8808efff49057812ba88be66a7c8.png)

(b) 软件包安装查看

图 2.2 软件包安装

# 3、软件包测试

## 3.1 STM32 MCU 移植测试（生成 STM32 CubeIDE 工程）

移植步骤如下：

（1）点击 ACCESS TO MCU SELECTOR：

![](media/7d19cc0ecebfe342f6102fb63ddc48c0.png)

图 3.1 打开 MCU 选择的界面

（2）选择 STM32F407ZGTx，点击 Start Project：

![](media/1c357c9b931d9ca757cff186bea35cee.png)

图 3.2 选择 MCU

（3）点击红框中选项

![](media/f85e252032706686b1509648da2ad34b.png)

图 3.3 选择软件包

（4）此时生成使用 STM32
CubeIDE 编译的工程，首先选择 gcc 版本的 arch（如果是 MDK-ARM 版本的话就点击 armcc 版本的 arch），然后点击黄色感叹号，再点击 Resolve。之后手动添加 helloworld_main 和 cmsis_os，并选择对应 MCU 版本的 mcu_platform。最后点击 ok 完成配置。

![](media/1cbb77bbe19809ad29348e80d449a28f.png)

![](media/9aa108534e529c9dc6974682d42ba98a.png)

图 3.4 软件包配置

（5）点击 Software Packs，打勾，并在下方进行软件包的参数配置。

![](media/80d8807155995bdfc49bfa6bb76a4f95.png)

图 3.5 软件包参数配置

（6）注释掉中断函数 PendSV()、void
SysTick_Handler(void)等，这些在软件包中都进行了配置。

![](media/96c9b931cff86a48dbb47953b80ad114.png)

图 3.6 中断函数处理

（7）进行生成工程配置，按如下界面进行配置，最后点击 GENERATE CODE，并点击 Open
Project。

![](media/9dc4a0abb5af8ded362902f988d49697.png)

![](media/a9cf1b1a0aeb488bbcc0cf27f937a331.png)

![](media/4c814923c4381488406c90a55e61b703.png)

图 3.7 工程生成配置

（8）在 tos_config 头文件中加入头文件：`#include "mcu_platform_M4.h"`，点击 Build All，如下界面所示，编译通过，完成软件包的移植。

![](media/ebec579e6ab8041ad25a1b969a25f8ee.png)

图 3.8 工程配置与编译

## 3.2 STM32 Board 移植测试（生成 MDK-ARM v5 工程）

（1）点击 ACCESS TO BOARD SELECTOR，选择 Board 为 NUCLEO-F401RE，点击 Start Project：

![](media/5f2d8da51e008ca34d307348243d4f79.png)

![](media/6d042e869f8150df48283e994a69a658.png)

图 3.9 Board 选择

（2）点击红框中选项

![](media/f85e252032706686b1509648da2ad34b.png)

图 3.10 选择软件包

（3）此时生成使用 MDK-ARM 编译的工程，首先选择 armcc 版本的 arch（如果是 STM32
CubeIDE 版本的话就点击 gcc 版本的 arch），然后点击黄色感叹号，再点击 Resolve。之后手动添加 helloworld_main 和 cmsis_os，并选择对应 MCU 版本的 mcu_platform。最后点击 ok 完成配置。

![](media/ad29e758a59f2cbaa4a3d2a9074f4cc4.png)

![](media/369b97c66d8f160c7f6e32237b22ecb7.png)

图 3.11 软件包配置

（4）点击 Software Packs，打勾，并在下方进行软件包的参数配置。

![](media/80d8807155995bdfc49bfa6bb76a4f95.png)

图 3.12 软件包参数配置

（5）注释掉中断函数 PendSV()、void
SysTick_Handler(void)等，这些在软件包中都进行了配置。

![](media/96c9b931cff86a48dbb47953b80ad114.png)

图 3.13 中断函数处理

（6）进行生成工程配置，按如下界面进行配置，最后点击 GENERATE CODE，并点击 Open
Project。

![](media/c5e8dc1958c672144bfb2300540bd2ff.png)

![](media/f7596c4361d6b6688700018ef4c8d8a0.png)

![](media/f64c22f774625cc3e1df74194231ce98.png)

图 3.14 工程生成配置

（8）打开 main_example.c，跳转到 tos_config 头文件，在其中加入头文件：`#include "mcu_platform_M4.h"`，点击 Build All，如下界面所示，编译通过，完成软件包的移植。

![](media/4682072cfe5bad6cbf4da59010037ddf.png)

图 3.15 移植编译

## 3.3 单片机测试

对单片机开发板进行测试，以正点原子探索者 STM32F407ZGT6 为例介绍 TencentOS-tiny 软件包的移植。

（1）点击 ACCESS TO MCU SELECTOR：

![](media/7d19cc0ecebfe342f6102fb63ddc48c0.png)

图 3.16 打开 MCU 选择的界面

（2）选择 STM32F407ZGTx，点击 Start Project：

![](media/1c357c9b931d9ca757cff186bea35cee.png)

图 3.17 选择 MCU

（3）点击红框中选项

![](media/f85e252032706686b1509648da2ad34b.png)

图 3.18 选择软件包

（4）此时生成使用 MDK-ARM 编译的工程，首先选择 armcc 版本的 arch（如果是 STM32
CubeIDE 版本的话就点击 gcc 版本的 arch），然后点击黄色感叹号，再点击 Resolve。之后手动添加 cmsis_os，并选择对应 MCU 版本的 mcu_platform。最后点击 ok 完成配置。

![](media/42a491cb7b7351c6ab5db4fe11010a6b.png)

图 3.19 软件包配置

（5）点击 Software Packs，打勾，并在下方进行软件包的参数配置。

![](media/80d8807155995bdfc49bfa6bb76a4f95.png)

图 3.20 软件包参数配置

（6）注释掉中断函数 PendSV()、void
SysTick_Handler(void)等，这些在软件包中都进行了配置，然后我们还需要对串口、下载等模块进行修改，如下：

![](media/96c9b931cff86a48dbb47953b80ad114.png)

（a）中断函数处理

![](media/8808da3c4fb3e4a3d04283d2eeeacbad.png)

（b）中断优先级配置

![](media/44e4a8429efc9d8f3258bd455d7c5b9f.png)

（c）RCC 配置

![](media/3655de0bd1a7df8340684ccc43aedfa9.png)

（d）SYS 配置

![](media/d930f57f1821a959e0dd9f522a2a6c4f.png)

（e）串口 1 配置

![](media/ef798ec862f32dc341a9d72777193977.png)

（f）时钟树配置

图 3.21 模块配置

（7）进行生成工程配置，按如下界面进行配置，最后点击右上角 GENERATE
CODE，生成工程后点击 Open Project。

![](media/726701d4d352048af6380ee3fb7dd31e.png)

![](media/82fd548a37df739720f71faa065b7dde.png)

图 3.22 工程配置

（8）打开 tos_config 头文件，在其中加入头文件：`#include "mcu_platform_M4.h"`，然后使用如下的 main 文件：

```c
/* USER CODE BEGIN Includes */

#include "cmsis_os.h"

/* USER CODE END Includes */

/* USER CODE BEGIN 0 */

//task1

#define TASK1_STK_SIZE 512

void task1(void *pdata);

osThreadDef(task1, osPriorityNormal, 1, TASK1_STK_SIZE);

//task2

#define TASK2_STK_SIZE 512

void task2(void *pdata);

osThreadDef(task2, osPriorityNormal, 1, TASK2_STK_SIZE);

void task1(void *pdata)
{

    int count = 1;

    char buffer[64] = {0};

    while(1) {
        snprintf(buffer, sizeof(buffer), "task 1 %04d\r\n", count++);

        HAL_UART_Transmit(&huart1, (uint8_t*)buffer, strlen(buffer), 0xFFFF);

        osDelay(2000);
    }

}

void task2(void *pdata)
{

int count = 1;

char buffer[64] = {0};

    while(1) {

        snprintf(buffer, sizeof(buffer), "task 2 %04d\r\n", count++);

        HAL_UART_Transmit(&huart1, (uint8_t\*)buffer, strlen(buffer), 0xFFFF);

        osDelay(1000);

    }

}

/* USER CODE END 0 */

int main(void)
{
    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */

    HAL_Init();

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */

    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */

    MX_GPIO_Init();

    MX_USART1_UART_Init();

    /* USER CODE BEGIN 2 */

    osKernelInitialize(); //TOS Tiny kernel initialize

    osThreadCreate(osThread(task1), NULL);// Create task1

    osThreadCreate(osThread(task2), NULL);// Create task2

    osKernelStart(); //Start TOS Tiny

    /* USER CODE END 2 */

    /* Infinite loop */

    /* USER CODE BEGIN WHILE */

    while (1)
    {
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
    }

    /* USER CODE END 3 */
}
```

（9）编译，然后使用 ST-LINK 下载到正点原子探索者单片机中

![](media/e5fa29fccefcb0cf1675ab7e763fa087.png)

图 3.23 编译、下载程序

（10）使用串口助手查看，两个任务的延时设置不一样，运行效果也不一样：

![](media/c1a77804bb7bdff73a478692034146cd.png)

图 3.24 串口查看界面

# 4、开发参考

1、腾讯物联网操作系统网址<https://github.com/OpenAtomFoundation/TencentOS-tiny>

2、ST 官方教程[How to develop a STM32Cube Expansion Package - stm32mcu
(stmicroelectronics.cn)](https://wiki.stmicroelectronics.cn/stm32mcu/wiki/How_to_develop_a_STM32Cube_Expansion_Package)

3、[制作 STM32Cube 软件包-STM32 PackCreator 的使用及软件包的制作 - 极术社区 -
连接开发者与智能计算生态 (aijishu.com)](https://aijishu.com/a/1060000000243533)

4、[（1）STM32
CubeMX 软件包的制作---STM32PackCreator 的使用\_linwei_Cui 的博客-CSDN 博客](https://blog.csdn.net/qq_40259429/article/details/120557456?spm=1001.2014.3001.5501)

5、[（2）STM32
CubeMX 软件包的制作---简易软件包的制作\_linwei_Cui 的博客-CSDN 博客](https://blog.csdn.net/qq_40259429/article/details/120583659?spm=1001.2014.3001.5501)
