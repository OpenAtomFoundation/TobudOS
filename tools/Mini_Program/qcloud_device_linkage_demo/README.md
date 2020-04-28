# 设备联动Demo

## 配置文件 miniprogram/config.js
```
// 传感设备配置
const SensorDevice = {
  // 腾讯云物联网通信平台中获取 产品ID和设备名称
  ProductId: "ZLN6Q20QHU", // 产品ID
  DeviceName: "dev001",    // 设备名称
  // 腾讯云控制台-访问管理-访问密钥-API密钥管理中获取 SecretId, SecretKey
  SecretId: "XXXX",
  SecretKey: "XXXX",
}

// 执行设备配置
const ExecuteDevice = {
  ProductId: "U1BZWHF7F9", // 产品ID
  DeviceName: "dev_01",    // 设备名称
  // 腾讯云控制台-访问管理-访问密钥-API密钥管理中获取 SecretId, SecretKey
  SecretId: "XXXX",
  SecretKey: "XXXX",
  Topic: "U1BZWHF7F9/dev_01/data"
}
```