### 智能灯小程序云开发DEMO
1. 说明
本示例小程序配合TencentOS_tiny智能灯数据模板示例使用：TencentOS_tiny/examples/tencent_cloud_sdk_data_template
[腾讯云侧接入文档参见: 智能灯接入指引](https://cloud.tencent.com/document/product/1081/34744)

2. tencent_cloud_sdk_data_template 接入腾讯云explorer后，在 app.js 中修改以下字段即可使用
````
  globalData: {
    // 腾讯云物联网开发平台explorer中获取 产品ID和设备名称
    productId: "U2LPAXBT2C", // 产品ID
    deviceName: "royye_light", // 设备名称
    // 腾讯云控制台-访问管理-访问密钥-API密钥管理中获取 SecretId, secretKey
    secretId: "AKIDxZ2LTTi84eaR3OfS8hnfxd4JHzQatFpF",
    secretKey: "UigMrA9rJXUCeoiBQP8dsJuJ6sYzmNjs",
    // 接口 Region 字段
    region: "ap-guangzhou",
    // 云开发的环境名称。此处需要替换为云开发创建的环境名称
    env: "tos-demo"
  },
````

3. 测试通过后，如果需要发布小程序，则还需要在小程序管理后台进行服务器域名配置
[微信小程序添加服务器端就接口域名](https://jingyan.baidu.com/article/ce09321bb6e9c12bff858f92.html)
进入微信公众平台小程序后台 - 开发 - 开发设置 - 服务器域名 - 输入域名: iotexplorer.tencentcloudapi.com

### 云开发参考文档

- [云开发文档](https://developers.weixin.qq.com/miniprogram/dev/wxcloud/basis/getting-started.html)

