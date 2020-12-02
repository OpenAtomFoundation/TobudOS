# 说明

1. 本工程基于 nRF52832 平台开发，使用的 SDK 版本为 `nRF5_SDK_15.3.0_59ac345`，如需其他版本 SDK，请前往 [NORDIC 官网](https://www.nordicsemi.com/Software-and-tools/Software/nRF5-SDK/Download) 下载
2. 蓝牙协议栈为 s132 版本，在 NORDIC SDK 中的路径为 `\components\softdevice\s132\hex\s132_nrf52_6.1.1_softdevice.hex`，请根据实际需要自行烧录协议栈
3. Keil工程的DeviceFamilyPack为 `NordicSemiconductor.nRF_DeviceFamilyPack.8.24.1.pack`，如需其他 pack 请前往 [NORDIC 官网](http://developer.nordicsemi.com/nRF51_SDK/pieces/nRF_DeviceFamilyPack/) 下载
4. 例程keil工程文件路径 `\qcloud_iot_explorer_ble\samples\nrf52832\pca10040\s132\arm5_no_packs`
5. 使用 code_extract.py 脚本抽取代码得到 `qcloud-iot-ble-nrf52832` 目录，请将该目录放置 NORDIC SDK 的 `\examples\ble_peripheral` 目录下，打开keil工程编译即可
