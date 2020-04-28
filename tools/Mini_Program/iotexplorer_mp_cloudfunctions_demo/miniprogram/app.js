//app.js
App({
  globalData: {
    // 腾讯云物联网开发平台explorer中获取 产品ID和设备名称
    productId: "U2LPAXBT2C", // 产品ID
    deviceName: "royye_light", // 设备名称
    // 腾讯云控制台-访问管理-访问密钥-API密钥管理中获取 SecretId, secretKey
    secretId: "xxx",
    secretKey: "xxx",
    // 接口 Region 字段。一般不需要修改
    region: "ap-guangzhou",
    // 云开发的环境ID。此处需要替换为云开发创建的环境ID
    env: "tos-demo"
  },
  onLaunch: function () {
    if (!wx.cloud) {
      console.error('请使用 2.2.3 或以上的基础库以使用云能力')
    } else {
      wx.cloud.init({
        // env 参数说明：
        //   env 参数决定接下来小程序发起的云开发调用（wx.cloud.xxx）会默认请求到哪个云环境的资源
        //   此处请填入环境 ID, 环境 ID 可打开云控制台查看
        //   如不填则使用默认环境（第一个创建的环境）
        env: this.globalData.env,
        traceUser: true,
      })
    }
  }
})
