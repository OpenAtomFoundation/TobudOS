# NimBLE心率传感器案例

本案例通过实现一个模拟的BLE心率传感器，展示如何通过GATT notification来实现异步的数据传输。

编译工程并烧写后，使用手机或任何能支持心率传感器app（建议使用Nordic官方的`nRF Toolbox`工具）的设备连接开发板创建的Heart Rate Sensor蓝牙设备，app界面上可以获取到传感器所在位置以及上传过来的心率信息。

