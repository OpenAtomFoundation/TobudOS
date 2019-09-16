const app = getApp()
const iotmodel = require("../../iotclient_for_miniprogram/iotmodel.js");
import IotexplorerClient from '../../iotclient_for_miniprogram/iotclient.js';

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

    // 实例化要请求产品的client对象
    this.data.client = new IotexplorerClient(app.globalData.secretId, app.globalData.secretKey)
    this.queryDeviceData()
  },
  queryDeviceData()
  { 
    let that = this
    wx.showLoading()

    // 实例化一个请求对象
    let req = new iotmodel.DescribeDeviceDataRequest();
    req.DeviceName = app.globalData.deviceName;
    req.ProductId = app.globalData.productId;

    // 通过client对象调用想要访问的接口，需要传入请求对象以及响应回调函数
    this.data.client.DescribeDeviceData(req, function (err, response) {
      if (err) {
        console.log("err:", err);
        wx.showToast({
          title: "查询失败 " + err,
          icon: 'none',
          duration: 3000
        })
        return;
      }
      wx.showToast({
        title: "查询成功",
        icon: 'none',
        duration: 1000
      })
      let deviceData = JSON.parse(response.Data)
      that.setData({
        deviceData: deviceData
      })
      console.log(that.data.deviceData);
    });
  },
  controlDeviceData(e) {
    let that = this
    wx.showLoading()

    let data = e.detail.value
    if (data.power_switch == true) {
      data.power_switch = 1
    } else {
      data.power_switch = 0
    }
    console.log('form data：', e.detail.value)

    // 实例化一个请求对象
    let req = new iotmodel.ControlDeviceDataRequest();
    req.DeviceName = app.globalData.deviceName;
    req.ProductId = app.globalData.productId;
    req.Data = JSON.stringify(data);

    // 通过client对象调用想要访问的接口，需要传入请求对象以及响应回调函数
    this.data.client.ControlDeviceData(req, function (err, response) {
      if (err) {
        console.log(err);
        wx.showToast({
          title: "操作失败 " + err,
          icon: 'none',
          duration: 3000
        })
        return;
      }
      console.log(err)
      wx.showToast({
        title: "操作成功",
        icon: 'none',
        duration: 1000
      })
    });
  },
})