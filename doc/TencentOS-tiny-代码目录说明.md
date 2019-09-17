# TencentOS tiny 代码目录说明


|一级目录 | 二级目录 | 三级目录 |说明 |
|---------|---------|---------|---------|
| arch | arm |   |TencentOS tiny适配的IP核架构（含M核中断、调度、tick相关代码）  |
| board     | TencentOS_tiny_EVB_MX |   |TencentOS tiny 定制开发板demo，包含AT适配框架、MQTT协议、安全组件等 |
| component      | connectivity | loraWAN  | loRaWAN协议栈实现源码及适配层|
|   |  |Eclipse-Paho-MQTT| MQTT协议栈实现源码及适配层|
|   |  |TencentCloud_SDK| 腾讯云C-SDK实现源码及适配层|
|   | fs |   | 文件系统实现源码|
|   | security |   | mbedtls 安全协议源码 |
| devices      |  |   |TencentOS tiny适配的一些外设驱动（如串口wifi gprs 驱动等）   |
| doc    |  |   |TencentOS tiny相关技术文档及开发指南  |
| examples    |  |   |TencentOS tiny提供的功能示例  |
| kernel      | core |   | TencentOS tiny内核源码|
|   | hal |   | TencentOS tiny驱动抽象层|
|   | pm |   | TencentOS tiny低功耗模块源码 |
| net      | at |   | TencentOS tiny为串口类通信模组提供的AT框架实现层|
|   | lora_module_wrapper |   | TencentOS tiny为串口类LoraWAN模块提供的移植框架|
|   | lwip |   | Lwip协议实现源码及适配层 |
|   | sal_module_wrapper |   | TencentOS tiny为串口类网络模块（wifi gprs）提供的socket移植框架 |
|   | tencent_firmware_module_wrapper |   | TencentOS tiny提供的腾讯定制模组移植框架 |
| osal      | cmsis_os |   |TencentOS tiny提供的cmsis os 适配   |
| platform      | hal |   | TencentOS tiny适配的部分芯片的驱动实现源码|
|   | vendor_bsp |   | 芯片厂家提供的原厂bsp固件库，如STM32的HAL库|
| test      |  |   |存放TencentOS tiny提供的一些测试代码，含内核及上层模块示例及测试代码  |
| tools    |  |   |存放TencentOS tiny提供的工具，小程序，配置工具等  |






