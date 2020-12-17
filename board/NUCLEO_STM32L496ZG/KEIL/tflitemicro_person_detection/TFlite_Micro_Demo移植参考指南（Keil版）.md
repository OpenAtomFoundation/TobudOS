# TensorFlow Lite Micro移植参考指南（Keil版）

作者：

日期：

联系方式：

## 概述

本教程是基于STM32 NUCLEO-L496ZG（Cortex-M4, 80Mhz）开发板，在运行TencentOS tiny的基础上，使用Tensorflow Lite Micro框架和CMSIS-NN库（算子加速），在STM32L496ZG上实现了**行人检测模型**的推理。

关于Tensorflow Lite Micro组件的详细介绍可以参考`TencentOS-tiny\components\tflite_micro`目录下的TFlite_Micro_Component_User_Guide.md文档。

本例程中，传入神经网络的RGB图像大小为 18kb（96*96 * 2byte），在STM32L496平台消耗的内存资源（经过优化后）如下：

- SRAM：168 Kbyte
- Flash：314 Kbyte

理论上满足以b上内存要求的STM32 Cortex-M系列MCU可以参考本指南进行移植。

## 一、移植前的准备

###   1. 准备目标硬件（开发板/传感器/模组）

需要准备如下硬件：

- 开发板：NUCLEO-L496ZG，MCU为STM32L496ZG；
- Camera：获取RGB图像，本例程使用OV2640摄像头；
- LCD：显示RGB图像，本例程使用2.4寸LCD（SPI通信）；

硬件实物图如下：

<div align=center>
<img src="image/all.jpg" width=50% />
</div>

###   2.准备TencentOS tiny基础keil工程代码

- 首先，参考TencentOS tiny基于keil的移植教程进行移植：
  https://github.com/Tencent/TencentOS-tiny/blob/master/doc/10.Porting_Manual_for_KEIL.md
- 值得注意的是，为了方便初始化MCU的外设，后续要继续使用STM32CubeMX软件，请确保安装了该软件；

- 移植成功后，工程可以进行线程任务切换，通过串口打印"hello world"，基础keil工程代码准备完毕。



###   3. 获取Tensorflow Lite Micro

有两种方式获取tflite_micro源码：

1. 从TencentOS tiny 代码仓库 `components/tflite_micro`目录获取；
2. 从Tensorflow代码仓库获取，TFlite_Micro的源码已经开源，github下载地址为：https://github.com/tensorflow/tensorflow ，TFlite_Micro的代码位于`tensorflow/tensorflow/lite/micro/`目录下。

如果没有tflite_micro开发经验，建议您以**第一种方式**获取tflite_micro源码，如果您希望自行获取最新tflite_micro源码，请参考`TencentOS-tiny\components\tflite_micro`目录的TFlite_Micro_Component_User_Guide.md文档。

## 二、BSP准备

### 1. 工程目录规划

以下是整个例程的目录规划：

| 一级目录  |           二级目录           | 三级目录 |                             说明                             |
| :-------: | :--------------------------: | :------: | :----------------------------------------------------------: |
|   arch    |             arm              |          | TencentOS tiny适配的IP核架构（含M核中断、调度、tick相关代码） |
|   board   |      NUCLEO_STM32L496ZG      |          |                    移植目标芯片的工程文件                    |
|           |                              |   BSP    |            板级支持包，外设驱动代码在Hardware目录            |
| component |         tflite_micro         |          |                       tflite_micro源码                       |
| examples  | tflitemicro_person_detection |          |                       行人检测demo示例                       |
|  kernel   |             core             |          |                    TencentOS tiny内核源码                    |
|           |              pm              |          |                 TencentOS tiny低功耗模块源码                 |
|   osal    |           cmsis_os           |          |              TencentOS tiny提供的cmsis os 适配               |

完成TencentOS tiny基础keil工程准备工作后，在这个keil工程的基础上继续添加外设驱动代码。

### 2. LCD驱动

本例程选用一款2.4寸LCD屏幕，分辨率为 240*320， SPI 接口通信，内部控制芯片为IL9341。

开发者也可以使用其他LCD，自行完成LCD的驱动代码移植，方便调试摄像头，以及查看图像是否正常。

#### 2.1 SPI初始化

进入`TencentOS-tiny\board\NUCLEO_STM32L496ZG\BSP`目录，打开TencentOS_tiny.ioc工程，使用STM32CubeMX初始化MCU外设。

<div align=center>
<img src="./image/spi init.png" width=100% />
</div>



#### 2.2 添加驱动代码

添加`lcd_2inch4.c`和`lcd_config.c`，添加头文件`lcd_2inch4.h`和`lcd_config.h`路径。



### 3. 摄像头驱动



#### 3.1 外设初始化



#### 3.2 添加驱动代码



**在main函数重写DCMI帧中断回调函数：**

```C
/* USER CODE BEGIN 4 */
void HAL_DCMI_FrameEventCallback(DCMI_HandleTypeDef *hdcmi)
{
	if(hdcmi->State == 2 && frame_flag != 1){
		frame_flag = 1; 
	}
}
/* USER CODE END 4 */
```



### 4. LCD显示摄像头图像

经过以上步骤，BSP就准备完毕了。

## 三、Tensorflow Lite Micro移植

### 1. tflite_micro源码加入到keil工程



### 2. 解决编译错误



### 3. 测试行人检测模型



### 4. 编写Person_Detection 任务函数



#### 4.1 图像预处理

<div align=center>
<img src="./image/RGB565.jpg" width=50% />
</div>

在本例程中，模型要求输入神经网络的图像为灰度图，为完成摄像头获取的RGB彩图到模型输入需要的灰度图转换，需从输入的RGB565像素格式中解析出R、G、B三通道的值，再根据心理学公式计算出单个像素点的灰度，具体代码如下：

```c
uint8_t rgb565_to_gray(uint16_t bg_color)
{
    uint8_t bg_r = 0;
    uint8_t bg_g = 0;
    uint8_t bg_b = 0;
    bg_r = ((bg_color>>11)&0xff)<<3;
    bg_g = ((bg_color>>5)&0x3f)<<2;
    bg_b = (bg_color&0x1f)<<2;
    uint8_t gray = (bg_r*299 + bg_g*587 + bg_b*114 + 500) / 1000;
    return gray;
}

void input_convert(uint16_t* camera_buffer , uint8_t* model_buffer)
{
	for(int i=0 ; i<OV2640_PIXEL_WIDTH*OV2640_PIXEL_HEIGHT ; i++)
	{
		model_buffer[i] = rgb565_to_gray(camera_buffer[i]);
	}
}
```



#### 4.2 行人检测线程任务函数

```c
void task1(void *arg)
{
    while (1) {
      if(frame_flag == 1){
				printf("***person detection task\r\n");
				if(HAL_DCMI_Stop(&hdcmi))Error_Handler(); //stop DCMI
				input_convert(camera_buffer,model_buffer);//convert input
				person_detect(model_buffer);              //inference
				LCD_2IN4_Display(camera_buffer,OV2640_PIXEL_WIDTH,OV2640_PIXEL_HEIGHT);
                //display
        
				frame_flag = 0;
				
				if(HAL_DCMI_Start_DMA(&hdcmi,DCMI_MODE_CONTINUOUS,\   //restart DCMI
                                     (uint32_t)camera_buffer ,\
					                 (OV2640_PIXEL_WIDTH*OV2640_PIXEL_HEIGHT)/2))
                    Error_Handler(); 
			}
			osDelay(50);
    }
}

void task2(void *arg)
{
    while (1) {
        printf("***task2\r\n");
        osDelay(50);
    }
}
```



## 四、Benchmark


