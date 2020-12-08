## TencentOS-tiny_Person_Detection_Demo

### 1. 目录结构：

- TencentOS-tiny\board\NUCLEO_STM32L496ZG\BSP\Hardware : **外设驱动代码**
- TencentOS-tiny\examples\tflitemicro_person_detection : **Demo任务函数**
- TencentOS-tiny\board\NUCLEO_STM32L496ZG\KEIL\tflitemicro_person_detection : **keil工程**
- TencentOS-tiny\components\tflite_micro\tensorflow : **tflite_micro代码**

### 2. 完成的工作：

- 使用STM32CubeMX，选择与TOS同版本的固件库重新生成外设初始化代码；
- TOS、摄像头和LCD工作都正常工作；

### 3. 未完成的工作：

- tflite_micro 以component的形式加到工程
- 串口重定向用了TOS的方法，retarget.c还没有引入工程
- 变量名、函数名还没有按照TOS的风格完全统一

