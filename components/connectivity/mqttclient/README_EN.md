[![](https://img.shields.io/github/v/tag/jiejietop/mqttclient?color=brightgreen&label=version)](https://github.com/jiejieTop/mqttclient/releases)

[![license](https://img.shields.io/badge/license-Apache-blue.svg)](https://github.com/jiejieTop/mqttclient/blob/master/LICENSE)

![](https://img.shields.io/github/forks/jiejieTop/mqttclient)

![](https://img.shields.io/github/stars/jiejieTop/mqttclient)

![](https://img.shields.io/badge/platform-Linux|Windows|Mac|Embedded-orange.svg)

[中文说明](README_CN.md)
# mqttclient

**A high-performance, high-stability, cross-platform MQTT client**

A high-performance, high-stability, cross-platform MQTT client, developed based on the socket API, can be used on embedded devices (FreeRTOS/LiteOS/RT-Thread/TencentOS tiny), Linux, Windows, Mac, and has a very concise The API interface realizes the quality of service of QOS2 with very few resources, and seamlessly connects the mbedtls encryption library.

## Advantage:

- **Extremely high stability**: Whether it is **drop and reconnect**, **packet loss and retransmission**, it is **strictly abide by the MQTT protocol standard**, in addition to The test of **large data volume** is very stable whether it is receiving or sending, and the **high frequency** test is also very stable.

- **Lightweight**: The entire code project is extremely simple. Without mbedtls, it takes up very few resources. The author used the esp8266 module to communicate with the cloud. The entire project code consumes less than 15k of RAM.

- **Support mbedtls encrypted transmission**, make the network transmission more secure, and the interface layer does not require users to care, no matter whether it is encrypted or not, mqttclient is **fixed** for the API interface provided by the user, which is very compatible A set of codes on behalf of the application layer can be transmitted with or without encryption.

- **Supports multiple clients**, compatible with multiple clients running at the same time, one device connected to multiple servers.

- **Supports synchronous and asynchronous processing**, applications need not block and wait to waste CPU resources.

- **Support interceptor configuration**, on some platforms, the client will automatically subscribe to the system theme by default, and the theme from the server changes every time. In this case, you need to use an interceptor to intercept and separate the theme and data Information and deliver it to users, greatly improving flexibility.

- **With online code generation tool**, the code can be generated with extremely simple configuration, address: [https://jiejietop.gitee.io/mqtt/index.html](https://jiejietop.gitee.io/mqtt/index.html)

- **Has a very simple API interface**, in general, mqttclient configuration has default values, basically can be used without configuration, can also be arbitrarily configured, the configuration has robustness detection, so designed The API interface is also very simple.

- **Multifunctional parameters can be configured and tailored**, reconnect time interval, heartbeat period, maximum number of subscriptions, command timeout, read and write buffer size, interceptor processing, etc. Parameters can be tailored and configurable to meet the needs of developers Complex and simple to use in various development environments.

- **Support automatic re-subscription of topics**, after automatic reconnection to ensure that the topics will not be lost.

- **Support theme wildcards ""#", "+"`**.

- **Subscribed topics are completely separated from message processing**, making programming logic easier and easier to use, users don’t have to deal with intricate logical relationships.

- **The keep-alive processing mechanism has been implemented in mqttclient**, without the user having to deal with the psychological experience, the user only needs to concentrate on the application function.

- **Has a very good design**, designed the **recording mechanism** with very few resources, and retransmits the message when it is lost to ensure that the qos1 and qos2 service quality levels guarantee its service quality.

- **There are very good code styles and ideas**: The whole code adopts a layered design, and the code implementation adopts the idea of ​​asynchronous processing to reduce coupling and improve performance.

- **Developed on top of standard BSD socket**, as long as it is compatible with BSD socket system.

- **Seamless connection of salof**: It is a synchronous and asynchronous log output framework. It outputs the corresponding log information when it is idle, and it can also write the information into flash to save it, which is convenient for debugging.

- **Use the famous paho mqtt library package**.

- **No other dependencies**.

## Online code generation tool

This project has a code generation tool that only requires online configuration to generate code, which is extremely simple and easy to use. The code generation tool address is: [https://jiejietop.gitee.io/mqtt/index.html](https://jiejietop.gitee.io/mqtt/index.html)

![Online code generation tool](png/mqtt-tool.png)

## occupied resource size

A total of **10857 bytes** of ROM, and the overhead of RAM is almost only dependent on dynamic memory. Without using TLS encrypted transmission, the communication dynamic memory that maintains the QOS0 quality of service level requires only about **3694** bytes. Including 1024 read buffer + 1024 write buffer + 1024 internal thread stack size, compared with other MQTT clients, mqttclient requires very little RAM resource overhead.

| Code | RO Data | RW Data | ZI Data | Object Name |
| -- | -- | -- | -- | -- |
| 7118 | 791 | 0 | 0 | mqttclient.o |
| 546 | 0 | 0 | 0 | mqttconnectclient.o |
| 212 | 0 | 0 | 0 | mqttdeserializepublish.o |
| 476 | 0 | 4 | 0 | mqttpacket.o |
| 236 | 0 | 0 | 0 | mqttserializepublish.o |
| 310 | 0 | 0 | 0 | mqttsubscribeclient.o |
| 38 | 0 | 0 | 0 | mqttunsubscribeclient.o |
| 56 | 0 | 0 | 0 | nettype_tcp.o |
| 62 | 0 | 0 | 0 | network.o |
| 24 | 0 | 0 | 0 | platform_memory.o |
| 40 | 0 | 0 | 0 | platform_mutex.o |
| 344 | 0 | 0 | 0 | platform_net_socket.o |
| 94 | 0 | 0 | 0 | platform_thread.o |
| 70 | 0 | 0 | 0 | platform_timer.o |
| 246 | 0 | 4 | 0 | random.o |
| 62 | 0 | 0 | 0 | mqtt_list.o |
| - | - | - | - | - |
| 10066  | 791 | 8 | 0 | total |

## Overall framework

Has a very clear layered framework.

![Overall Frame](png/mqttclient.png)

- At the top of the framework is the **API** function interface, which implements the client's **application, release, set parameters, connect to the server, disconnect, subscribe topic, unsubscribe topic, publish message** and other functional interfaces.

- The famous **paho mqtt** library is used as the MQTT message packet library.

- Asynchronous processing mechanism is used to manage all the acks. It does not need to wait for the server's response when sending the message, but only records it. After receiving the server's ack, cancel this record, **very efficient**; and When the mqtt message (QoS1/QoS2) is sent and no response is received from the server, the message will be **retransmitted**.

- An **mqtt yield** thread is implemented internally to handle all content in a unified manner, such as **timeout processing, ack message processing, and receiving publish message from the server**, at this time the callback function will be called Inform the user of the data received, **post release, post completion message processing, heartbeat message (keep alive), when disconnected from the server, you need to try to reconnect, resubscribe to the topic, resend the message or reply* *Wait.

- Message processing, such as **reading and writing messages, decoding mqtt messages, setting messages (dup flag), destroying messages** and other operations.

- **network** is a network component, which can **automatically select a data channel**, if it is an encryption method, **tls encryption** is used for data transmission, and tls can choose mbedtls as the encryption backend; it can also be The **tcp direct connection** method is ultimately transmitted via tcp.

- **platform** is a platform abstraction layer that encapsulates things from different systems, such as socke or AT, thread, time, mutex, memory management**, these are dealing with the system and are also necessary for cross-platform Package.

- On the far right is the general content, **list processing, log library, error handling, software random number generator**, etc.

## Supported platforms

**At present, Linux, TencentOS tiny, FreeRTOS, RT-Thread platforms have been implemented (software package is named kawaii-mqtt`), in addition to TencentOS tiny AT framework can also be used, and the stability is excellent!**

| Platform | Code Location |
| -------------- | -------- |
| Linux | [https://github.com/jiejieTop/mqttclient](https://github.com/jiejieTop/mqttclient) |
| TencentOS tiny | [https://github.com/Tencent/TencentOS-tiny/tree/master/board/Fire_STM32F429](https://github.com/Tencent/TencentOS-tiny/tree/master/board/Fire_STM32F429) |
| TencentOS tiny AT framework | [https://github.com/jiejieTop/gokit3-board-mqttclient](https://github.com/jiejieTop/gokit3-board-mqttclient) |
| RT-Thread | [https://github.com/jiejieTop/kawaii-mqtt](https://github.com/jiejieTop/kawaii-mqtt) |
| FreeRTOS | [https://github.com/jiejieTop/freertos-mqttclient](https://github.com/jiejieTop/freertos-mqttclient) |


## Version

| Release Version | Description |
| --- | --- |
| [v1.0.0] | Initial release, complete basic framework and stability verification |
| [v1.0.1] | Fix the logical processing when actively disconnecting from the server |
| [v1.0.2] | Add a new feature-interceptor, fix some small bugs |
| [v1.0.3] | To avoid global pollution, modify the naming of log and list related functions |
| [v1.0.4] | Network structure and mbedtls data channel readjusted |
| [v1.1.0] | A larger version of the update, refactoring part of the code, optimizing the logic of MQTT processing, improving the overall stability, supporting multiple clients, supporting setting the will, optimizing the API interface, and adding multiple cloud platforms Test code and documentation, add online code generation tool, online cutting configuration tool |
## question

Welcome to submit issues and bug reports in the form of [GitHub Issues](https://github.com/jiejieTop/mqttclient/issues)

## Copyright and License

mqttclient follows the [Apache License v2.0](https://github.com/jiejieTop/mqttclient/blob/master/LICENSE) open source agreement. Encourage code sharing and respect the copyright of the original author. You can freely use and modify the source code, or you can publish the modified code as open source or closed source software.

## Test and use under Linux platform

### Install cmake:

```bash
    sudo apt-get install cmake g++
```

### test program

| Test Platform | Location |
| - | - |
| emqx (my privately deployed server) | [./test/emqx/test.c](./test/emqx/test.c) |
| Baidu Tiangong | [./test/baidu/test.c](./test/baidu/test.c) |
| onenet | [./test/onenet/test.c](./test/onenet/test.c) |
| Alibaba Cloud Internet of Things | [./test/ali/test.c](./test/ali/test.c) |

### Compile & Run

```bash
    ./build.sh
```

After running the **build.sh** script, the executable files **emqx**, **baidu**, **onenet** and other platforms will be generated under the **./build/bin/** directory Executable programs can be run directly.

```bash
    ./build/bin/emqx
```

### Compile into a dynamic library libmqttclient.so

```bash
    ./make-libmqttclient.sh
```

After running the `make-libmqttclient.sh` script, a dynamic library file `libmqttclient.so` will be generated in the `./libmqttclient/lib` directory, and installed into the system’s `/usr/lib` directory, the relevant header files have been Copy to the `./libmqttclient/include` directory and copy it to your project. You only need to link the dynamic library when compiling the application. `-lmqttclient -lpthread`, the configuration file of the dynamic library is based on` ./test/mqtt_config.h` configuration.

If you are using a cross-compiler, you should export the corresponding environment variables according to the compiler you are using (the cross-compiler used here is arm-linux-gnueabihf-gcc), and you must also set the dynamic library file `libmqttclient .so` is copied to the `/usr/lib` directory of your embedded system:

```bash
    export CROSS_COMPILE=arm-linux-gnueabihf-
```

If you need to uninstall **libmqttclient.so**, execute the following command:

```bash
    ./make-libmqttclient.sh remove
```

## Learn more

Please see the documentation:

- [MQTT Protocol Introduction](./docs/mqtt-introduction.md)

- [MQTT protocol communication process](./docs/mqtt-communication.md)

- [mqttclient code generation tool](./docs/mqtt-tool.md)

- [mqttclient configuration and cutting tool](./docs/mqtt-config.md)

- [mqttclient design and implementation](./docs/mqtt-design.md)

- [mqttclient connects to Baidu Tiangongwu Access](./docs/mqtt-baidu.md)

- [mqttclient connects to OneNET cloud platform](./docs/mqtt-onenet.md)

- [mqttclient connects to Alibaba Cloud IoT platform](./docs/mqtt-aliyun.md)
