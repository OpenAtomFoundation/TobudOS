# TencentOS tiny内核移植指南(MacOS+STM32CubeIDE版)

## 一、工程创建

 具体的工程创建方法和初始参数设置方法略去。
 
 需要强调的是本文是以`STM32L431`芯片为基础的，如果是其它芯片，后面的步骤请做相应的调整。
 
 最后生成的工程目录如下:
![](https://main.qcloudimg.com/raw/d692d86434c953c178010acfcbecf7e7.png)
 
## 二、TencentOS tiny代码准备

只需要保留TencentOS这些目录下的代码，其它目录皆可删除。

![](https://main.qcloudimg.com/raw/25043ac8b3b6944670867960c4230abb.png)

 
在`arch`目录里需要删除的所有代码目录列表

```
arch/arm/arm-v7m/cortex-m0+/
arch/arm/arm-v7m/cortex-m3/
arch/arm/arm-v7m/cortex-m4/armcc/
arch/arm/arm-v7m/cortex-m4/iccarm/
arch/arm/arm-v7m/cortex-m7/
```

**注: STM32L431是Cortex-M4内核，所以如果是其它芯片，例如STM32F103就应该删除`arch/arm/arm-v7m/cortex-m4/`目录，保留`arch/arm/arm-v7m/cortex-m3/gcc`目录。**

重命名代码顶层目录

```
$ mv TencentOS_tiny tiny
```

## 三、移植工作

1. 把代码`tiny`拷贝到第一步创建的工程目录里

![](https://main.qcloudimg.com/raw/c0cd1bc6d8e58f52939e19a3a13db74a.png)

 刷新工程
 
![](https://main.qcloudimg.com/raw/14a6bf4775db3a9bc7162a8eebeddbfd.png)

 把代码添加到工程里
 
![](https://main.qcloudimg.com/raw/1315cce3621fa7f2ff20a135d4d7bf3a.png)

2. 把下列头文件目录添加到工程里

 ```
 tiny/arch/arm/arm-v7m/common/include/
 tiny/arch/arm/arm-v7m/cortex-m4/gcc/   # 如上所述，这个目录视特定芯片而定
 tiny/kernel/core/include/
 tiny/kernel/pm/include/
 tiny/osal/cmsis_os/
 ```

 ![](./image/porting/porting_cubeide_006.png)


3. 在Inc目录下新建一个头文件`tos_config.h`

 ```
 #ifndef _TOS_CONFIG_H_
 #define _TOS_CONFIG_H_
 
 #include "stm32l431xx.h" // 目标芯片头文件，视特定芯片而定
 
 #include "stddef.h"
 
 #define TOS_CFG_TASK_PRIO_MAX 10u // 配置TencentOS tiny默认支持的最大优先级数量
 
 #define TOS_CFG_ROUND_ROBIN_EN 1u // 配置TencentOS tiny的内核是否开启时间片轮转
 
 #define TOS_CFG_OBJECT_VERIFY 0u // 配置TencentOS tiny是否校验指针合法
 
 #define TOS_CFG_EVENT_EN 1u // TencentOS tiny 事件模块功能宏
 
 #define TOS_CFG_MMHEAP_EN 1u // 配置TencentOS tiny是否开启动态内存模块
 
 #define TOS_CFG_MMHEAP_POOL_SIZE 0x100 // 配置TencentOS tiny动态内存池大小
 
 #define TOS_CFG_MUTEX_EN 1u // 配置TencentOS tiny是否开启互斥锁模块
 
 #define TOS_CFG_QUEUE_EN 1u // 配置TencentOS tiny是否开启队列模块
 
 #define TOS_CFG_TIMER_EN 1u // 配置TencentOS tiny是否开启软件定时器模块
 
 #define TOS_CFG_SEM_EN 1u // 配置TencentOS tiny是否开启信号量模块
 
 #if (TOS_CFG_QUEUE_EN > 0u)
 #define TOS_CFG_MSG_EN 1u
 #else
 #define TOS_CFG_MSG_EN 0u
 #endif
 
 #define TOS_CFG_MSG_POOL_SIZE 10u // 配置TencentOS tiny消息队列大小
 
 #define TOS_CFG_IDLE_TASK_STK_SIZE 256u // 配置TencentOS tiny空闲任务栈大小
 
 #define TOS_CFG_CPU_TICK_PER_SECOND 1000u // 配置TencentOS tiny的tick频率
 
 #define TOS_CFG_CPU_CLOCK (SystemCoreClock) // 配置TencentOS tiny CPU频率
 
 #define TOS_CFG_TIMER_AS_PROC 1u // 配置是否将TIMER配置成函数模式
 
 #endif

 ```
 

4. 修改`Src/stm32l4xx_it.c`里的代码:

 * 将`void PendSV_Handler(void)`修改为`__weak void PendSV_Handler(void)`
 * 在`SysTick_Handler`函数中`HAL_IncTick();`之后添加代码

  ```
  if(tos_knl_is_running()) {
	  tos_knl_irq_enter();
	  tos_tick_handler();
	  tos_knl_irq_leave();
  }
  ```
  
5. 修改`Src/main.c`

 添加代码 
 
 ```
 #include "cmsis_os.h"
 
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
 	while(1)
 	{
 		HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
 		osDelay(200);
 	}
 }
 
 void task2(void *pdata) {
 	while(1) {
 		osDelay(1000);
 	}
 } 
 
 ```

**注：代码中的LED是在创建工程的GPIO选项里指定的，是`PC13`引脚，这个是依据TencentOS tiny EVB_MX开发板而决定的，相应的开发板请做相应调整。**

 主函数代码
 
 ```
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
	/* USER CODE BEGIN 2 */
	
	osKernelInitialize(); //TOS Tiny kernel initialize
	osThreadCreate(osThread(task1), NULL); // Create task1
	osThreadCreate(osThread(task2), NULL); // Create task2
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



## 附1 清理代码目录命令

```
ls | egrep -v "arch|kernel|osal" | xargs rm -rf
rm -rf arch/arm/arm-v7m/cortex-m0+/
rm -rf arch/arm/arm-v7m/cortex-m3/
rm -rf arch/arm/arm-v7m/cortex-m4/armcc/
rm -rf arch/arm/arm-v7m/cortex-m4/iccarm/
rm -rf arch/arm/arm-v7m/cortex-m7/
```
