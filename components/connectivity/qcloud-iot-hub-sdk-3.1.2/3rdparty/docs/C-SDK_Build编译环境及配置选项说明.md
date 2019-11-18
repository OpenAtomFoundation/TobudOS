# C-SDK 编译方式及配置选项说明

本文档对C-SDK 的编译方式和编译配置选项进行说明，并介绍了 Linux和Windows 开发环境下面的编译环境搭建以及编译示例。

## C-SDK 编译方式说明

C-SDK 支持三种编译方式

#### 1. cmake 方式

推荐使用cmake作为跨平台的编译工具，支持在Linux和Windows开发环境下进行编译。
cmake方式采用CMakeLists.txt作为编译配置选项输入文件

#### 2. Makefile 方式

对不支持cmake的环境，使用Makefile直接编译的方式，该方式与SDK v3.0.3及之前的版本保持一致，采用make.settings作为编译配置选项输入文件，修改完成后执行make即可

#### 3. 代码抽取方式

该方式可根据需求选择功能，将相关代码抽取到一个单独的文件夹，文件夹里面的代码层次目录简洁，方便用户拷贝集成到自己的开发环境。
该方式需要依赖cmake工具，在CMakeLists.txt中配置相关功能模块的开关，并将EXTRACT_SRC置为ON，Linux环境运行以下命令
```
mkdir build
cd build
cmake ..
```
即可在output/qcloud_iot_c_sdk中找到相关代码文件，目录层次如下：
```
 qcloud_iot_c_sdk
 ├── include
 │   ├── config.h
 │   ├── exports
 ├── platform
 └── sdk_src
     └── internal_inc
```
include目录为SDK供用户使用的API及可变参数，其中config.h为根据编译选项生成的编译宏。API具体介绍请参考**C-SDK_API及可变参数说明**。
platform目录为平台相关的代码，可根据设备的具体情况进行修改适配。具体的函数说明请参考文档**C-SDK_Porting跨平台移植概述**
sdk_src为SDK的核心逻辑及协议相关代码，一般不需要修改，其中internal_inc为SDK内部使用的头文件。

用户可将qcloud_iot_c_sdk拷贝到其目标平台的编译开发环境，并根据具体情况修改编译选项。

## C-SDK 编译选项说明

#### 1. 编译配置选项
以下配置选项大部分都适用于cmake和make.setting。cmake中的ON值对应于make.setting的y，OFF对应于n。

| 名称                             | cmake值            | 说明                                                         |
| :------------------------------- | ------------- | ------------------------------------------------------------ |
| BUILD_TYPE                       | release/debug | release：不启用IOT_DEBUG信息，编译输出到release目录下<br />debug：启用IOT_DEBUG信息，编译输出到debug目录下 |
| EXTRACT_SRC                      | ON/OFF        | 代码抽取功能开关，仅对使用cmake有效                                             |
| COMPILE_TOOLS                    | gcc           | 支持gcc和msvc，也可以是交叉编译器比如arm-none-linux-gnueabi-gcc |
| PLATFORM                         | linux         | 包括linux/windows/freertos/nonos                             |
| FEATURE_MQTT_COMM_ENABLED        | ON/OFF        | MQTT通道总开关                                               |
| FEATURE_MQTT_DEVICE_SHADOW       | ON/OFF        | 设备影子总开关                                               |
| FEATURE_COAP_COMM_ENABLED        | ON/OFF        | CoAP通道总开关                                               |
| FEATURE_GATEWAY_ENABLED          | ON/OFF        | 网关功能总开关                                               |
| FEATURE_OTA_COMM_ENABLED         | ON/OFF        | OTA固件升级总开关                                            |
| FEATURE_OTA_SIGNAL_CHANNEL       | MQTT/COAP     | OTA信令通道类型                                              |
| FEATURE_AUTH_MODE                | KEY/CERT      | 接入认证方式                                                 |
| FEATURE_AUTH_WITH_NOTLS          | ON/OFF        | OFF: TLS使能, ON: TLS关闭                                    |
| FEATURE_DEV_DYN_REG_ENABLED      | ON/OFF        | 设备动态注册开关                                             |
| FEATURE_LOG_UPLOAD_ENABLED       | ON/OFF        | 日志上报开关                                                 |
| FEATURE_DEBUG_DEV_INFO_USED      | ON/OFF        | 设备信息获取来源开关                                         |
| FEATURE_SYSTEM_COMM_ENABLED      | ON/OFF        | 获取后台时间开关                                             |
| FEATURE_AT_TCP_ENABLED           | ON/OFF        | AT模组TCP功能开关                                            |
| FEATURE_AT_UART_RECV_IRQ         | ON/OFF        | AT模组中断接受功能开关                                       |
| FEATURE_AT_OS_USED               | ON/OFF        | AT模组多线程功能开关                                         |
| FEATURE_AT_DEBUG                 | ON/OFF        | AT模组调试功能开关                                           |
| FEATURE_MULTITHREAD_TEST_ENABLED | ON/OFF        | 是否编译Linux多线程测试例程                                  |

配置选项之间存在依赖关系，当依赖选项的值为有效值时，部分配置选项才有效，主要如下：

| 名称                             | 依赖选项                                                | 有效值       |
| :------------------------------- | ------------------------------------------------------- | ------------ |
| FEATURE_MQTT_DEVICE_SHADOW       | FEATURE_MQTT_COMM_ENABLED                               | ON           |
| FEATURE_GATEWAY_ENABLED          | FEATURE_MQTT_COMM_ENABLED                               | ON             |
| FEATURE_OTA_SIGNAL_CHANNEL(MQTT) | FEATURE_OTA_COMM_ENABLED<br />FEATURE_MQTT_COMM_ENABLED | ON<br />ON   |
| FEATURE_OTA_SIGNAL_CHANNEL(COAP) | FEATURE_OTA_COMM_ENABLED<br />FEATURE_COAP_COMM_ENABLED | ON<br />ON   |
| FEATURE_AUTH_WITH_NOTLS          | FEATURE_AUTH_MODE                                       | KEY          |
| FEATURE_AT_UART_RECV_IRQ         | FEATURE_AT_TCP_ENABLED                                  | ON           |
| FEATURE_AT_OS_USED               | FEATURE_AT_TCP_ENABLED                                  | ON           |
| FEATURE_AT_DEBUG                 | FEATURE_AT_TCP_ENABLED                                  | ON           |

#### 2. 设备信息选项

在腾讯云物联控制台创建设备之后，需要将设备信息（ProductID/DeviceName/DeviceSecret/Cert/Key文件）配置在SDK中才能正确运行。在开发阶段，SDK 提供两种方式存储设备信息：
1. 存放在代码中（编译选项DEBUG_DEV_INFO_USED = ON），则在`platform/os/xxx/HAL_Device_xxx.c`中修改设备信息，在无文件系统的平台下可以使用这种方式。
2. 存放在配置文件中（编译选项DEBUG_DEV_INFO_USED = OFF），则在`device_info.json`文件修改设备信息，此方式下更改设备信息不需重新编译SDK，在Linux/Windows平台下开发推荐使用这种方式。


## C-SDK 编译环境

### Linux(Ubuntu)环境

本文使用Ubuntu的版本为14.04或16.04

1. 必要软件安装
```
$ sudo apt-get install -y build-essential make git gcc cmake
```
  SDK需要cmake版本在3.5以上，默认安装的cmake版本较低，可以参考如下链接进行cmake特定版本的下载与安装
  下载链接：https://cmake.org/download/
  安装参考：https://gitlab.kitware.com/cmake/cmake


2. 配置修改
修改CMakeLists.txt确保以下选项存在(以密钥认证设备为例)
```
set(BUILD_TYPE                   "release")
set(COMPILE_TOOLS                "gcc") 
set(PLATFORM 	                "linux")
set(FEATURE_MQTT_COMM_ENABLED ON)
set(FEATURE_AUTH_MODE "KEY")
set(FEATURE_AUTH_WITH_NOTLS OFF)
set(FEATURE_DEBUG_DEV_INFO_USED  OFF)  
```

3.  执行脚本编译
完整编译库和示例
```
./cmake_build.sh 
```
输出的库文件，头文件及示例在`output/release`文件夹中。
在一次完整编译之后，若只需要编译示例，则执行
```
./cmake_build.sh samples
```

4. 填写设备信息
将在腾讯云物联网平台创建的设备的设备信息(以**密钥认证设备**为例)填写到device_info.json中，如：
```
{
  "auth_mode":"KEY",	
  "productId":"S3EUVBRJLB",
  "deviceName":"test_device",	
  "key_deviceinfo":{    
      "deviceSecret":"vX6PQqazsGsMyf5SMfs6OA6y"
  }
}
```

5. 运行示例
示例输出位于`output/release/bin`文件夹中，比如运行mqtt_sample示例，输入`./output/release/bin/mqtt_sample`即可。


### Windows环境

#### 获取和安装Visio Studio 2019开发环境

请访问[Visual Studio下载网站](https://visualstudio.microsoft.com/zh-hans/downloads/)，下载并安装Visio Studio 2019，本文档下载安装的是16.2版本 Community。

![](https://main.qcloudimg.com/raw/730f793e3166db859682cdbfd0c7181f.jpg)

然后选择**使用C++的桌面开发**，并确保勾选**用于Windows的C++ CMAKE工具**：

![](https://main.qcloudimg.com/raw/89a63cb6ae5231a66b17e80acac37267.jpg)

#### 编译并运行

1. 运行visual studio，选择**打开本地文件夹**，并选择下载的C SDK目录。

   ![](https://main.qcloudimg.com/raw/f0c65e7c5ce13ca13b967c178c0449c6.jpg)

2. [修改用户信息]()

3. 双击打开根目录的CMakeLists.txt，并确认编译工具链中设置的平台为**windows**和编译工具为**MSVC**（其他配置选项请参考[cmake编译配置与代码抽取]()）。

   ![](https://main.qcloudimg.com/raw/8a16723698646a2dca5ab08a57927304.jpg)

   ```cmake
   # 编译工具链
   #set(COMPILE_TOOLS "gcc") 
   #set(PLATFORM 	  "linux")
    
   set(COMPILE_TOOLS "MSVC") 
   set(PLATFORM 	  "windows") 
   ```

4. visual studio会自动生成cmake缓存，请等待cmake缓存生成完毕。

   ![](https://main.qcloudimg.com/raw/633bf217afc2d52e92618a90bd7d9da0.jpg)

5. 缓存生成完毕后，点击**生成->全部生成**。

![](https://main.qcloudimg.com/raw/f85d2435c49a802eedd87b751408ecce.jpg)

6. 选择相应的示例运行，示例应与用户信息相对应。

   ![](https://main.qcloudimg.com/raw/a0f2bacc0d31d2da9d327d5c855655a3.jpg)
