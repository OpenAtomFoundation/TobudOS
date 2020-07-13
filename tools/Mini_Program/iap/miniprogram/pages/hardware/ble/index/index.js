const app = getApp()
let util = require('../../../../utils/util.js');
let bleapi = require('../../../../utils/hardware/ble/ble-api.js');

Page({
  data: {
    searching: false,
    devicesList: []
  },
  onLoad: async function (options) {
    this.search()
  },
  onUnload: async function () {
    bleapi.closeBluetoothAdapter()
  },
  onBluetoothDeviceFound() {
    let that = this
    return new Promise((resolve) => {
      wx.onBluetoothDeviceFound(function (res) {
        var name = res.devices[0].name
        if (name) {
          console.log("onBluetoothDeviceFound:", name, res);
          that.data.devicesList.push(res.devices[0])
          that.setData({
            devicesList: that.data.devicesList
          })
        }
      })
      resolve()
      console.log("onBluetoothDeviceFound start")
    })
  },
  search: async function() {
    try {
      await bleapi.closeBluetoothAdapter()
      await bleapi.openBluetoothAdapter()
    } catch(e) {
      wx.showModal({
        content: "请检查手机蓝牙是否打开",
        showCancel: false
      });
      return
    }

    try {
      // 开始扫描蓝牙设备
      await bleapi.startBluetoothDevicesDiscovery([])
      this.setData({
        searching: true,
        devicesList: []
      })
      await this.onBluetoothDeviceFound()
      
      // 每次扫描蓝牙设备10秒
      await util.delayMs(10000)
      if (this.data.searching) {
        this.setData({
          searching: false
        })
        await bleapi.stopBluetoothDevicesDiscovery()
      }
    } catch (e) {
      console.error(e)
      wx.showModal({
        content: "搜索设备失败\n" + e,
        showCancel: false
      });
    }
  },
  connect: async function(e) {
    console.log(e.currentTarget)
    wx.showLoading({
      title: '连接蓝牙设备中...',
    })
    let deviceId = e.currentTarget.id
    let deviceName = e.currentTarget.dataset.name

    try {
      await bleapi.stopBluetoothDevicesDiscovery()
      await bleapi.closeBLEConnection(deviceId)
      
      // 创建BLE连接
      await bleapi.createBLEConnection(deviceId)
      
      // 读取BLE设备的 Services
      let service = null
      let services = await bleapi.getBLEDeviceServices(deviceId)
      let serviceId = '0000FFE0-0000-1000-8000-00805F9B34FB' // 指定ServiceID
      for (let i = 0; i < services.length; i++) {
        if (services[i].isPrimary && services[i].uuid == serviceId) {
          service = services[i]
          break
        }
      }

      // 读取BLE设备指定 ServiceId 的 Characteristics
      let characteristics = await bleapi.getBLEDeviceCharacteristics(deviceId, service.uuid)
      let characteristic = characteristics[0] // 默认选择第一个特征值
      console.log('characteristic', characteristic)
      
      // 完成连接BLE设备，跳转到该设备页面
      let device = {
        connected: true,
        name: deviceName,
        deviceId: deviceId,
        service: service,
        characteristic: characteristic,
        services: services,
        characteristics: characteristics,
      }
      console.log(device)
      wx.hideLoading()
      wx.navigateTo({
        url: '../device/index',
        success: function(res) {
          res.eventChannel.emit('eventData', {
            device: device,
          })
        }
      })
    } catch (e) {
      console.error(e)
      wx.showToast({
        title: "连接蓝牙失败，请重试",
        icon: "none"
      })
    }
  },
})