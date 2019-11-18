
## 准备源码和工具
TencentOS tiny的源码地址在这: [TencentOS tiny](https://github.com/Tencent/TencentOS-tiny).
MDK软件下载在这：[MDK5.28](http://www2.keil.com/nuvoton/M0-M23) 已有MDK请忽略
项目需要的资料我已经上传 [M251_Sourse](https://download.csdn.net/download/qq_30078751/11912357)

## 准备硬件、软件
**硬件：** 						
		1个新唐 Numaker_M251_252SD demo板，如果你有其他板子，只要是M251或者M252都可以，可以正常下程序的
		1个Nulink 用来下载程序的
		
**软件：**
		TencentOS tiny代码包
		我上传的资料包里边的Nu-Link_Keil_Driver 3.00.6909.exe一路安装，提示是否需要加入软件包的时候点确定即可
		解压M251_252_Series_BSP_CMSIS_V3.00.002.zip备用，以下是先把一个裸机工程的必须文件提取，如下
		桌面新建一个文件夹名为Nuvoton Tos（随意）,然后新建如下几个文件
![文件夹](https://img-blog.csdnimg.cn/20191024105014884.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzMwMDc4NzUx,size_16,color_FFFFFF,t_70)
TencentOS里边新建三个文件夹 arch 、kernel、TOS_CONFIG这三个文件夹
![tos](https://img-blog.csdnimg.cn/20191024141937118.png)
然后先拷贝CMSIS文件,把M251_252_Series_BSP_CMSIS_V3.00.002图中三个文件拷贝到工程的CMSIS下
![CMSIS](https://img-blog.csdnimg.cn/20191024142124153.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzMwMDc4NzUx,size_16,color_FFFFFF,t_70)

到M251_252_Series_BSP_CMSIS_V3.00.002中复制图中红框内的文件夹和文件到工程的STARTUP文件夹内
![在这里插入图片描述](https://img-blog.csdnimg.cn/20191024142315597.png)

![在这里插入图片描述](https://img-blog.csdnimg.cn/20191024142353268.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzMwMDc4NzUx,size_16,color_FFFFFF,t_70)
然后进入ARM文件夹内，复制startup_M251.s
![在这里插入图片描述](https://img-blog.csdnimg.cn/20191024142430760.png)
然后把整个外设驱动文件夹内全部复制到工程的StdDriver内
![在这里插入图片描述](https://img-blog.csdnimg.cn/20191024142803865.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzMwMDc4NzUx,size_16,color_FFFFFF,t_70)
最后到TencentOS tiny的移植
进入TencentOS tiny 文件夹  \TencentOS-tiny\arch\arm\arm-v7m\common  和
 \TencentOS-tiny\arch\arm\arm-v7m\cortex-m0+\armcc
 复制框中的文件夹到工程的TencentOS 的 arch内  这些就是操作系统和内核的接口文件
![在这里插入图片描述](https://img-blog.csdnimg.cn/20191024143011825.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzMwMDc4NzUx,size_16,color_FFFFFF,t_70)
![在这里插入图片描述](https://img-blog.csdnimg.cn/20191024143155486.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzMwMDc4NzUx,size_16,color_FFFFFF,t_70)
然后把 kernel内的所有文件复制到工程的kernel内这些就是实现操作系统的底层文件

![在这里插入图片描述](https://img-blog.csdnimg.cn/20191024143340579.png)
TOS_CONFIG 文件夹内的文件在 \TencentOS-tiny\board\TencentOS_tiny_EVB_MX\TOS-CONFIG
把它复制到工程的TOS_CONFIG 文件夹内
![在这里插入图片描述](https://img-blog.csdnimg.cn/20191024143453956.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzMwMDc4NzUx,size_16,color_FFFFFF,t_70)
这样准备工作就基本完成，接下来就是移植了
## 移植开始
打开MDK 新建一个工程  然后把工程保存到工程Project文件夹内，名字随意取
![在这里插入图片描述](https://img-blog.csdnimg.cn/20191024143719280.png)
![在这里插入图片描述](https://img-blog.csdnimg.cn/20191024143824496.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzMwMDc4NzUx,size_16,color_FFFFFF,t_70)
接下来是选择目标单片机，我这里是M251KG6AE，看你的板子是什么单片机就选什么单片机
![在这里插入图片描述](https://img-blog.csdnimg.cn/20191024143915381.png)
然后勾选CMSIS的CORE
![在这里插入图片描述](https://img-blog.csdnimg.cn/2019102414394649.png)
一个空工程模板就建好了，接下来要放入文件，添加对应对应文件夹内的代码
![在这里插入图片描述](https://img-blog.csdnimg.cn/20191024144241912.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzMwMDc4NzUx,size_16,color_FFFFFF,t_70)
这是添加好的代码 红框内的文件是在kernel 的 pm文件夹内，此时USER文件夹内是空的，需要新建一个main.c文件
![在这里插入图片描述](https://img-blog.csdnimg.cn/20191024144519643.png)
新建main.c  放入以下代码，里面就是初始化系统，选择时钟源，然后创建两个任务

```c
#include "stdio.h"
#include "NuMicro.h"
#include "tos.h"

#define task1_size   512  //任务1堆栈大小
#define oled_size 	 1024

k_task_t task1;   //任务1结构体
k_task_t oled_task;      //显示任务结构体

k_stack_t task1_stack[task1_size]; //任务1堆栈
k_stack_t oled_stack[oled_size];   //任务2的堆栈大小

void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/
		/* Unlock protected registers */
    SYS_UnlockReg();
	
		/* Set XT1_OUT(PF.2) and XT1_IN(PF.3) to input mode to prevent leakage  */
    PF->MODE &= ~(GPIO_MODE_MODE2_Msk | GPIO_MODE_MODE3_Msk);

    /* Disable digital input path of analog pin XT1_OUT to prevent leakage */
    GPIO_DISABLE_DIGITAL_PATH(PF, BIT2 | BIT3);
   
	  /* Enable， HXT  clock */
    CLK_EnableXtalRC(CLK_PWRCTL_HXTEN_Msk);		
	
    /* Switch HCLK clock source to PLL */
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HXT, CLK_CLKDIV0_HCLK(1));  //这里选择外部晶振，DEMO板子上的是32M
		
	/* Set the core clock freq*/
	CLK_SetCoreClock(FREQ_32MHZ);   //这里是设定内核的时钟频率，实测不能倍频，只能小于或等于你的外部晶振的频率	
    /* Enable UART module clock */
    CLK_EnableModuleClock(UART0_MODULE);

    /* Select UART module clock source as HIRC and UART module clock divider as 1 */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HIRC, 	
   
    CLK_CLKDIV0_UART0(1));
		
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    Uart0DefaultMPF();

}

void UART0_Init()
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init UART                                                                                               */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Reset UART module */
    SYS_ResetModule(UART0_RST);
    /* Configure UART0 and set UART0 baud rate */
    UART_Open(UART0, 115200);
}

void task1_fun(void *Parameter)
{

	while(1)
	{
		printf("Task1 ：移植CORTEX-M23成功\r\n");	
		PB14 =~ PB14;     //板子上的LED灯
		tos_task_delay(1000);  //延时1s
	}

}

//*OLED显示任务函数
void oled_display_task(void *Parameter)
{

	k_err_t err;
	while(1)
	{				
		printf("Task2 请欢呼雀跃，谢谢\r\n");		
		tos_task_delay(100);   //100mS刷新 
	}
}

int main()
{
	k_err_t err;
	 /* Unlock protected registers */
    SYS_UnlockReg();  //解锁时钟保护寄存器
    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();		
	
	tos_knl_init();   //tos init 系统初始化是包含初始化滴答定时器，所以必须在时钟保护寄存器解锁后调用	
    /* Lock protected registers */
    SYS_LockReg();	  //上锁
    /* Init UART0 for printf */
    UART0_Init();
  	
	GPIO_SetMode(PB, BIT14, GPIO_MODE_OUTPUT);
	err = tos_task_create(
			&task1,
			"Task1",
			task1_fun,
			NULL,
			2,
			task1_stack,
			task1_size,
			100
		);
  	err = tos_task_create(
			&oled_task,					//内核任务的结构体
			"display",				//任务的名字的指针
			oled_display_task,			//任务实体函数入口
			NULL,						//传递给任务函数的参数
			2,              //优先级  数字越小优先级越大
			oled_stack,		   //任务在运行时使用的栈空间的起始地址
			oled_size,			//任务堆栈大小
			100);						//时间片轮转机制下当前任务的时间片大小。当timeslice为0时，任务调度时间片会被设置为默认大小（TOS_CFG_CPU_TICK_PER_SECOND / 10），系统时钟滴答（systick）数 / 10。			
	if(err != K_ERR_NONE)
	printf("TenentOS creat task fail! code is %d\r\n",err);

	tos_knl_start(); //Start TOS TINY
}
```
保存到工程文件夹Project内，然后再添加到MDK USER中即可

然后包含头文件如图：

![在这里插入图片描述](https://img-blog.csdnimg.cn/20191024145509732.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzMwMDc4NzUx,size_16,color_FFFFFF,t_70)

接下来是修改核心文件，先构建系统滴答定时器的中断服务函数 
在 system_M251.c  文件中包含Tos头文件 在最下面添加中断服务
![在这里插入图片描述](https://img-blog.csdnimg.cn/20191024144957999.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzMwMDc4NzUx,size_16,color_FFFFFF,t_70)
修改串口的引脚，在该文件的最下面，我的demo板子是用的uart0，引脚对应的是A0 A1，所以修改一下

```c
void Uart0DefaultMPF(void)
{
    /* Set GPB multi-function pins for UART0 RXD and TXD */
  //  SYS->GPB_MFPH = (SYS->GPB_MFPH & ~SYS_GPB_MFPH_PB12MFP_Msk) | SYS_GPB_MFPH_PB12MFP_UART0_RXD;
 //   SYS->GPB_MFPH = (SYS->GPB_MFPH & ~SYS_GPB_MFPH_PB13MFP_Msk) | SYS_GPB_MFPH_PB13MFP_UART0_TXD;	
	  SYS->GPA_MFPL = (SYS->GPA_MFPL & ~SYS_GPA_MFPL_PA0MFP_Msk) | SYS_GPA_MFPL_PA0MFP_UART0_RXD;
      SYS->GPA_MFPL = (SYS->GPA_MFPL & ~SYS_GPA_MFPL_PA1MFP_Msk) | SYS_GPA_MFPL_PA1MFP_UART0_TXD;
}

```

然后在串口函数下面添加滴答定时器的中断服务函数
```c

//systick 中断处理函数
void SysTick_Handler(void)
{
	if (tos_knl_is_running())
  {
    tos_knl_irq_enter();
    tos_tick_handler();      //系统的心跳
    tos_knl_irq_leave();
  }
}

```
修改内核头文件：打开port_c.c文件，修改单片机内核头文件
![在这里插入图片描述](https://img-blog.csdnimg.cn/20191024145620528.png)
然后修改tos_config文件
![在这里插入图片描述](https://img-blog.csdnimg.cn/20191024145716753.png)
还有下面的设置系统的时钟频率的，这里要和单片机设定的内核时钟频率移植，否则会出现时间不准确的现象。

![时钟](https://img-blog.csdnimg.cn/20191025092047426.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzMwMDc4NzUx,size_16,color_FFFFFF,t_70)

然后编译

![编译](https://img-blog.csdnimg.cn/20191025092105211.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzMwMDc4NzUx,size_16,color_FFFFFF,t_70)
然后就下载到板子上，先选择仿真器，并对仿真器进行设置：
![仿真设置](https://img-blog.csdnimg.cn/20191025092423258.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzMwMDc4NzUx,size_16,color_FFFFFF,t_70)
然后把USB-TTl，链接PA0 PA1，打开串口调试助手，设置115200波特率，打开串口，然后按一下板子上的复位键，即可看到串口打印任务1  和 任务2 ，任务1是每秒打印，任务2是100ms打印，然后板子上的LED灯每秒交替亮灭，说明系统正常跑
![打印](https://img-blog.csdnimg.cn/20191025092727795.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzMwMDc4NzUx,size_16,color_FFFFFF,t_70)


到这里，TencentOS tiny 移植新塘M251 cortex-M23内核成功！