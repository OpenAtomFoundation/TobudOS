## 说明
本工程模拟了一个甲醛监测场景，ESP32任务定期模拟甲醛数据(也可以通过甲醛监测传感器实时获取)，ESP32通过BLE和腾讯连连小程序连接，使用LLSync协议传输数据，由腾讯连连小程序将甲醛数据上传到物联网开发平台。

## 使用说明
1. 安装`ESP-IDF`，请参考[官网文档](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html#step-2-get-esp-idf)
```c
mkdir -p ~/esp
cd ~/esp
git clone --recursive https://github.com/espressif/esp-idf.git
```
2. 抽取`qcloud-iot-ble-esp32`组件
```c
cd scripts
python3 code_extract/code_extract.py esp32
```
3. 拷贝`qcloud-iot-ble-esp32`组件到`ESP-IDF`目录下
```c
cp -r scripts/code_extract/qcloud-iot-ble-esp32 $IDF_PATH
cd $IDF_PATH/qcloud-iot-ble-esp32
```
4. 登陆[物联网开发平台](https://cloud.tencent.com/product/iotexplorer), 使用`qcloud_llsync/date_template/esp32.json`作为数据模版创建设备。
使用新设备的三元信息替换`qcloud_llsync/hal/ble_qiot_ble_device.c`中宏定义中的设备信息，编译并烧录到开发板。
```c
idf.py flash
```
5. 打开腾讯连连小程序，添加设备，观察串口输出和小程序界面，串口输出如下：
```c
report ppm 0.028
I (271165) post data: 00 00 05 60 43 60 e5 3c
I (271165) LLSYNC: ESP_GATTS_CONF_EVT, status = 0, attr_handle 46
I (271445) LLSYNC: GATT_WRITE_EVT, handle = 44, value len = 2, value :
I (271445) LLSYNC: 20 00
I (271445) LLSYNC: ble_lldata_write_cb
I (271445) LLSYNC: GATT_WRITE_EVT OVER
```
可以观察到小程序界面甲醛数据同步变化。
