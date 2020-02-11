//app.js
App({
  globalData: {
    // 腾讯云物联网开发平台explorer中获取 产品ID和设备名称
    productId: "U2LPAXBT2C", // 产品ID
    deviceName: "royye_light", // 设备名称
    // 腾讯云控制台-访问管理-访问密钥-API密钥管理中获取 SecretId, secretKey
    secretId: "xxx",
    secretKey: "xxx",
  },
  onLaunch: function(options) {
    console.log("onLaunch")
  },
})