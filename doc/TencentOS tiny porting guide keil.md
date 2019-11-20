# TencentOS tiny 内核移植参考指南（Keil版）

## 一、移植前的准备
###   1. 准备目标硬件（开发板/芯片/模组）

TencentOS tiny目前主要支持ARM Cortex M核芯片的移植，比如STM32 基于Cortex M核全系列、NXP 基于Cortex M核全系列等。本教程将使用STM32官方Demo开发板 NUCLEO-L073RZ进行示例移植，其他 ARM Cortex M系列开发板和芯片移植方法类似。

调试ARM Cortex M核还需要仿真器， NUCLEO-L073RZ自带ST-Link调试器，如果您的开发板或者芯片模组没有板载仿真器，就需要连接外置的仿真器，如J-Link、U-Link之类的。

     

###   2.准备编译器环境

本移植指南针对的是Keil编译器，所以我们移植内核前需要先安装Keil编译器，能编译ARM Cortex M核的Keil编译器现在也叫MDK，最新版本5.28a,下载地址为：[https://www.keil.com/demo/eval/arm.htm]()
填写注册信息即可下载，下载完成在windows环境下按照提示安装即可，安装完成后需要自行购买软件License，避免32K Flash下载限制。
由于新版本的MDK编译器和芯片支持包是分离的，所以MDK（Keil）安装完成后，还需要安装对应芯片的器件支持包（PACK包），比如本教程示例的NUCLEO-L037RZ开发板的芯片是STM32L073RZ，就需要安装*Keil.STM32L0xx_DFP.2.0.1.pack*系列器件支持包，MDK所有支持芯片的PACK包下载地址为：[http://www.keil.com/dd2/Pack/#/eula-container]() ，您只需要根据您的芯片型号下载对应的PACK包即可，当然您也可以在MDK集成开发环境中在线下载安装。

###   3. 准备芯片对应的裸机工程

移植TencentOS tiny基础内核需要您提前准备一个芯片对应的裸机工程，裸机工程包含基本的芯片启动文件、基础配置（时钟、主频等）、以及串口、基本GPIO驱动用于RTOS测试。

本教程使用ST官方的STM32CubeMX软件来自动化生成MDK裸机工程，STM32CubeMX的下载地址为：

[     https://www.st.com/content/st_com/zh/products/development-tools/software-development-tools/stm32-software-development-tools/stm32-configurators-and-code-generators/stm32cubemx.html]()

 安装STM32CubeMx还需要事先安装好JDK环境，您可以在互联网上查找如何安装和配置JDK环境，此处不再赘述。

 CubeMX安装完成后，我们就可以使用CubeMX来给NUCLEO-L073RZ开发板生成裸机工程了，如果您的芯片不是STM32，而是其他厂商的ARM Cortex M系列，您可以根据产商的指导准备裸机工程，后续的内核移植步骤是一致的。

####    3.1 首先启动STM32CubeMX，新建工程

![](https://main.qcloudimg.com/raw/5c6a1eca65dbec90fe73402cc39a2fa4.png)

#### 3.2 选择MCU型号
     
![](https://main.qcloudimg.com/raw/70d3cc4e69c36a9d9707efde2c0e2472.png)
     
如上图所示：通过MCU筛选来找到自己开发板对应的芯片型号，双击后弹出工程配置界面，如下图：
     
![](https://main.qcloudimg.com/raw/f8f05e6b8ef07fc9d30fa3c51a0c82fe.png)
#### 3.3 Pin设置界面配置时钟源     
     
![](https://main.qcloudimg.com/raw/01dcc7684d340dca5d84b88e5b6b707b.png)

#### 3.4 Pin设置界面配置串口
     
![](https://main.qcloudimg.com/raw/ffd52f709fd148ba7e654c8ce320d0ad.png)
     
#### 3.5 Pin设置界面配置GPIO
![](https://main.qcloudimg.com/raw/278977b909359db187519b8d6a9125d4.png)
     
#### 3.6 配置总线时钟
     
![](https://main.qcloudimg.com/raw/72f3f1ed823d1e57bac1eda0d0487b2f.png)
     
#### 3.7 工程生成参数配置
     
![](https://main.qcloudimg.com/raw/ac35d9a0aaa1e0e8adcced561cac179b.png)
     
#### 3.8 代码生成方式配置
     
![](https://main.qcloudimg.com/raw/d46ed81804cdb58e6af8c64df5a470ba.png)
     
#### 3.9 生成工程

![](https://main.qcloudimg.com/raw/ecc132f84a548f8802abb7d8aefc8ba9.png)
     
#### 3.10  keil下的裸机工程
     
点击生成代码后，生成的裸机工程效果如下：
![](https://main.qcloudimg.com/raw/3d071b29139ba20bba78c5521838efc2.png)
     
这样NUCLEO-L073RZ裸机工程生成完成，该工程可直接编译并烧写在板子上运行。
     
###   4. 准备TencentOS tiny的源码
TencentOS tiny的源码已经开源，github下载地址为：[https://github.com/Tencent/TencentOS-tiny.git]()

     

|一级目录 | 二级目录 | 说明 |
|---------|---------|---------|
| arch | arm | TencentOS tiny适配的IP核架构（含M核中断、调度、tick相关代码）  |
| board     | NUCLEO_L073RZ |  移植目标芯片的工程文件 |
| kernel      | core | TencentOS tiny内核源码|
|   | pm | TencentOS tiny低功耗模块源码 |
| osal      | cmsis_os | TencentOS tiny提供的cmsis os 适配   |

由于本教程只介绍TencentOS tiny的内核移植，所以这里只需要用到 arch、board、kernel、osal四个目录下的源码。

## 二、内核移植

### 1. 代码目录规划

![](https://main.qcloudimg.com/raw/1d274de2237132c1136506d5e4ae9eea.png)

如图所示，新建TencentOS_tiny主目录，并在主目录下添加四个子目录，其中arch、kernel、osal从代码仓直接拷贝过来即可，而board目录下则放入我们前面生成的裸机工程代码，我们移植的开发板取名叫NUCLEO_L073RZ,裸机代码全部拷贝到下面即可，如下图所示：

![](https://main.qcloudimg.com/raw/b8bd75a9baec5d5f11a3505014f9ba46.png)

接下来进入TencentOS_tiny\board\NUCLEO_L073RZ\MDK-ARM目录，打开keil工程，我们开始添加TencentOS tiny的内核代码。

### 2. 添加arch平台代码

![](https://main.qcloudimg.com/raw/c75e3d6b6371ceeafa749e200b924c6c.png)

 tos_cpu.c是TencentOS tiny 的CPU适配文件，包括堆栈初始化，中断适配等，如果您的芯片是ARM Cortex M核，该文件可以不做改动，M0、M3 、M4、M7是通用的，其他IP核需要重新适配；

port_s.S 文件是TencentOS tiny的任务调度汇编代码，主要做弹栈压栈等处理的，port_c.c适配systick等，这两个文件 每个IP核和编译器都是不一样的，如果您的芯片是ARM Cortex M核，我们都已经适配好，比如现在我们移植的芯片是STM32L073RZ，是ARM Cortex M0+核，使用的编译器是KEIL，所以我们选择arch\arm\arm-v7m\cortex-m0+\armcc下的适配代码，如果你的开发板是STM32F429IG，M4核，编译器是GCC，则可以选择arch\arm\arm-v7m\cortex-m4\gcc目录下的适配文件。

### 3. 添加内核源码

内核源码kerne目录下包含core和pm两个目录，其中core下为基础内核，pm是内核中的低功耗组件；基础移植的时候可以不添加pm目录下的代码，如下图所示，添加全部基本内核源码：
![](https://main.qcloudimg.com/raw/699d9d0d52d3b29e7fb9583fa0bea69c.png)

### 4. 添加cmsis os源码
	
cmsis os是TencentOS tiny为了兼容cmsis标准而适配的OS抽象层，可以简化大家将业务从其他RTOS迁移到TencentOS tiny的工作量。
	
![](https://main.qcloudimg.com/raw/bf6450c40da00ecabbd0ad1cd21fed87.png)
	
### 5. 添加TencentOS tiny头文件目录

添加头文件目录前，我们在要移植的工程目录下新增一个 TOS_CONFIG文件夹，用于存放TencentOS tiny的配置头文件，也就是接下来要新建的tos_config.h文件；

TencentOS tiny所有要添加的头文件目录如下：

![](https://main.qcloudimg.com/raw/444e6326814b0e3d207fa031ef5268e8.png)


### 6. 新建TencentOS tiny系统配置文件 tos_config.h

```
#ifndef _TOS_CONFIG_H_
#define  _TOS_CONFIG_H_

#include "stm32l0xx.h"	// 目标芯片头文件，用户需要根据情况更改

#define TOS_CFG_TASK_PRIO_MAX           10u 	// 配置TencentOS tiny默认支持的最大优先级数量

#define TOS_CFG_ROUND_ROBIN_EN          0u		// 配置TencentOS tiny的内核是否开启时间片轮转

#define TOS_CFG_OBJECT_VERIFY_EN           1u	// 配置TencentOS tiny是否校验指针合法

#define TOS_CFG_TASK_DYNAMIC_CREATE_EN  1u		// TencentOS tiny 动态任务创建功能宏

#define TOS_CFG_EVENT_EN                1u		// TencentOS tiny 事件模块功能宏

#define TOS_CFG_MMBLK_EN                1u		//配置TencentOS tiny是否开启内存块管理模块

#define TOS_CFG_MMHEAP_EN               1u		//配置TencentOS tiny是否开启动态内存模块

#define TOS_CFG_MMHEAP_DEFAULT_POOL_EN  1u		// TencentOS tiny 默认动态内存池功能宏

#define TOS_CFG_MMHEAP_DEFAULT_POOL_SIZE        0x100	// 配置TencentOS tiny默认动态内存池大小

#define TOS_CFG_MUTEX_EN                1u		// 配置TencentOS tiny是否开启互斥锁模块

#define TOS_CFG_MESSAGE_QUEUE_EN        1u		// 配置TencentOS tiny是否开启消息队列模块

#define TOS_CFG_MAIL_QUEUE_EN           1u		// 配置TencentOS tiny是否开启消息邮箱模块

#define TOS_CFG_PRIORITY_MESSAGE_QUEUE_EN	1u	// 配置TencentOS tiny是否开启优先级消息队列模块

#define TOS_CFG_PRIORITY_MAIL_QUEUE_EN	1u		// 配置TencentOS tiny是否开启优先级消息邮箱模块

#define TOS_CFG_TIMER_EN                1u		// 配置TencentOS tiny是否开启软件定时器模块

#define TOS_CFG_PWR_MGR_EN              0u		// 配置TencentOS tiny是否开启外设电源管理模块

#define TOS_CFG_TICKLESS_EN             0u		// 配置Tickless 低功耗模块开关

#define TOS_CFG_SEM_EN                  1u		// 配置TencentOS tiny是否开启信号量模块

#define TOS_CFG_TASK_STACK_DRAUGHT_DEPTH_DETACT_EN      1u	// 配置TencentOS tiny是否开启任务栈深度检测

#define TOS_CFG_FAULT_BACKTRACE_EN      0u		// 配置TencentOS tiny是否开启异常栈回溯功能

#define TOS_CFG_IDLE_TASK_STK_SIZE      128u	// 配置TencentOS tiny空闲任务栈大小

#define TOS_CFG_CPU_TICK_PER_SECOND     1000u	// 配置TencentOS tiny的tick频率

#define TOS_CFG_CPU_CLOCK               (SystemCoreClock)	// 配置TencentOS tiny CPU频率

#define TOS_CFG_TIMER_AS_PROC           1u		// 配置是否将TIMER配置成函数模式

#endif

```
   按照上面的模板配置好TencentOS tiny的各项功能后，将tos_config.h 文件放入要移植的board工程目录下即可，例如本教程是放到board\NUCLEO_L073RZ\TOS_CONFIG目录下。
   
   这样，TencentOS tiny的源码就全部添加完毕了。

##  三、创建TencentOS tiny任务，测试移植结果

### 1. 修改部分代码
#### 修改stm32l0xx_it.c的中断函数,在stm32l0xx_it.c文件中包含 tos.h 头文件
![](https://main.qcloudimg.com/raw/172245ad4fd6768dca798fd2db209755.png)

在stm32l0xx_it.c文件中的PendSV_Handler函数前添加___weak关键字，因为该函数在TencentOS tiny的调度汇编中已经重新实现；同时在SysTick_Handler函数中添加TencentOS tiny的调度处理函数，如下图所示：
	
![](https://main.qcloudimg.com/raw/855bba41b18a8d7892f7e1771738c76f.png)


### 2.  编写TencentOS tiny 测试任务

#### 在mian.c 中添加TencentOS tiny 头文件，编写任务函数

```
   #include "cmsis_os.h"
   //task1
   #define TASK1_STK_SIZE		256
   void task1(void *pdata);
   osThreadDef(task1, osPriorityNormal, 1, TASK1_STK_SIZE);
   
   //task2
   #define TASK2_STK_SIZE		256
   void task2(void *pdata);
   osThreadDef(task2, osPriorityNormal, 1, TASK2_STK_SIZE);
   
   void task1(void *pdata)
   {
       int count = 1;
       while(1)
       {
           printf("\r\nHello world!\r\n###This is task1 ,count is %d \r\n", count++);
           HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
           osDelay(2000);
       }
   }
   void task2(void *pdata)
   {
       int count = 1;
       while(1)
       {
           printf("\r\nHello TencentOS !\r\n***This is task2 ,count is %d \r\n", count++);
           osDelay(1000);
       }
   }
   
   int fputc(int ch, FILE *f)
   {
     if (ch == '\n') 
     {
       HAL_UART_Transmit(&huart2, (void *)"\r", 1,30000);
     }
     HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xFFFF);
     return ch;
   }

```

如图：

![](https://main.qcloudimg.com/raw/cf4d435f8701d54114255f603a5dcb3d.png)

继续在main.c 的mian函数中硬件外设初始化代码后添加TencentOS tiny的初始化代码：

```
     osKernelInitialize(); //TOS Tiny kernel initialize
     osThreadCreate(osThread(task1), NULL);// Create task1
     osThreadCreate(osThread(task2), NULL);// Create task2
     osKernelStart();//Start TOS Tiny
```


 如图：

![](https://main.qcloudimg.com/raw/6fcf8b45b7eb23c179c69f3deaf9ac79.png)

### 3. 编译下载测试TencentOS tiny移植结果

![](https://main.qcloudimg.com/raw/3ce4c79796f4261fd45d26caa64e1e24.png)
   
   按照上图指示，进行编译下载到开发板即可完成TencentOS tiny的测试，如下图所示，可以看到串口交替打印信息，表示两个任务正在进行调度，切换运行：
   
![](https://main.qcloudimg.com/raw/b0f9d16064c4aeffa5f8c3dfbfbc0dbd.png)
   
   








