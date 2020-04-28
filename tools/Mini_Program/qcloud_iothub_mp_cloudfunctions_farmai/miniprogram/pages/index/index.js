const app = getApp()

Page({
  data: {
    productId: app.globalData.productId,
    deviceName: app.globalData.deviceName,
    stateReported: {},
  }, 
  onLoad: function (options) {
    console.log("index onLoad")
    if (!app.globalData.productId) {
      wx.showToast({
        title: "产品ID不能为空",
        icon: 'none',
        duration: 3000
      })
      return
    } else if (!app.globalData.deviceName) {
      wx.showToast({
        title: "设备名称不能为空",
        icon: 'none',
        duration: 3000
      })
      return
    }
    this.update()
  },
  update() {
    wx.showLoading()
    wx.cloud.callFunction({
      name: 'iothub-shadow-query',
      data: {
        ProductId: app.globalData.productId,
        DeviceName: app.globalData.deviceName,
        SecretId: app.globalData.secretId,
        SecretKey: app.globalData.secretKey,
      },
      success: res => {
        wx.showToast({
          title: '调用成功',
        })
        let deviceData = JSON.parse(res.result.Data)
        this.setData({
          stateReported: deviceData.payload.state.reported
        })
        console.log("result:", deviceData)
      },
      fail: err => {
        wx.showToast({
          icon: 'none',
          title: '调用失败',
        })
        console.error('[云函数] [iotexplorer] 调用失败：', err)
      }
    }) 
  },
  switchChange(e) {
    let value = 0
    if (e.detail.value == true) {
      value = 1
    }
    let item = e.currentTarget.dataset.item
    let obj = {
      [`${item}`]: value
    }
    let payload = JSON.stringify(obj)
    console.log(payload)
    wx.showLoading()
    wx.cloud.callFunction({
      name: 'iothub-publish',
      data: {
        SecretId: app.globalData.secretId,
        SecretKey: app.globalData.secretKey,
        ProductId: app.globalData.productId,
        DeviceName: app.globalData.deviceName,
        Topic: app.globalData.productId + "/" + app.globalData.deviceName + "/data",
        Payload: payload,
      },
      success: res => {
        wx.showToast({
          title: '调用成功',
        })
        console.log("res:", res)
      },
      fail: err => {
        wx.showToast({
          icon: 'none',
          title: '调用失败',
        })
        console.error('[云函数] [iotexplorer] 调用失败：', err)
      }
    })  
  },
})