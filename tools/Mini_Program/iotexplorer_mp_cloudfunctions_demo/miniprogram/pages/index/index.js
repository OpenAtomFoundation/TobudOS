const app = getApp()

Page({
  data: {
    client: null,
    deviceData: {},
    items: [
      { name: '红色', value: 0, color: "red"},
      { name: '绿色', value: 1, color: "green" },
      { name: '蓝色', value: 2, color: "blue" },
    ]
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
    } else if (!app.globalData.secretId) {
      wx.showToast({
        title: "访问密钥SecretId不能为空",
        icon: 'none',
        duration: 3000
      })
      return
    } else if (!app.globalData.secretKey) {
      wx.showToast({
        title: "访问密钥SecretKey不能为空",
        icon: 'none',
        duration: 3000
      })
      return
    }
    
    this.setData({
      productId: app.globalData.productId,
      deviceName: app.globalData.deviceName,
    })

    this.queryDeviceData()
  },
  queryDeviceData()
  { 
    wx.showLoading()
    let queryData = {
      productId: app.globalData.productId,
      deviceName: app.globalData.deviceName,
      secretId: app.globalData.secretId,
      secretKey: app.globalData.secretKey,
      region: app.globalData.region,
    }
    console.log(queryData);
    // 调用云函数query
    wx.cloud.callFunction({
      name: 'query',
      data: queryData,
      success: res => {
        try {
          let deviceData = JSON.parse(res.result.Data)
          this.setData({
            deviceData: deviceData
          })
          console.log("result:", deviceData)
          wx.showToast({
            title: '调用成功',
          })
        } catch (e) {
          wx.showToast({
            icon: 'none',
            title: '调用失败',
          })
          console.log(res.result.Data, e)
        }
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
  controlDeviceData(e) {
    wx.showLoading()
    let data = e.detail.value
    if (data.power_switch == true) {
      data.power_switch = 1
    } else {
      data.power_switch = 0
    }
    console.log('form data：', data)

    let controlData = {
      productId: app.globalData.productId,
      deviceName: app.globalData.deviceName,
      secretId: app.globalData.secretId,
      secretKey: app.globalData.secretKey,
      region: app.globalData.region,
      data: JSON.stringify(data),
    }
    console.log(controlData);
    // 调用云函数control
    wx.cloud.callFunction({
      name: 'control',
      data: controlData,
      success: res => {
        wx.showToast({
          title: '调用成功',
        })
        console.log(res.result)
      },
      fail: err => {
        wx.showToast({
          icon: 'none',
          title: '调用失败:' + err,
        })
        console.error('[云函数] [iotexplorer] 调用失败：', err)
      }
    })
  },
})