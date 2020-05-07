# C-SDK 多线程及多设备支持

多线程及多设备支持实例需要首先使能编译配置开关FEATURE_MULTITHREAD_ENABLED

## Multi-Thread 多线程环境使用说明

设备端 C-SDK 支持在多线程多任务环境下使用，但是对接口的使用有一定规范。
比如对于MQTT接口在多线程环境下的使用有如下注意事项：
```
1. 不允许多线程调用IOT_MQTT_Yield，IOT_MQTT_Construct以及IOT_MQTT_Destroy
2. 可以多线程调用IOT_MQTT_Publish，IOT_MQTT_Subscribe及IOT_MQTT_Unsubscribe
3. IOT_MQTT_Yield 作为从socket读取并处理MQTT报文的函数，应保证一定的执行时间，避免被长时间挂起或抢占
```
在多线程环境下，建议可以使用 IOT_MQTT_StartLoop 和 IOT_MQTT_StopLoop 来启动和停止后台 IOT_MQTT_Yield 线程，则在需要读取报文的时候不再需要主动调用IOT_MQTT_Yield函数，由后台线程来读取并处理MQTT报文。
使用示例可以参考 samples/mqtt/multi_thread_mqtt_sample

其他模块接口的使用可以参考MQTT接口的使用

## Multi-Client 多设备接入使用

在SDK版本3.2.0之后，支持同时使用多个设备接入。SDK采用面向对象方式对状态变量和参数进行封装，使得比如一个MQTT/Shadow Client实例可以对应一个设备，通过提供多个设备三元组，创建多个Client实例，则可以同时进行多个设备的接入和通讯。
具体的示例代码可以参考 samples/multi_client 下面的sample，通过创建多个线程，每个线程里面创建各自的 MQTT/Shadow Client，则相当于多个设备在同时访问后台服务。


