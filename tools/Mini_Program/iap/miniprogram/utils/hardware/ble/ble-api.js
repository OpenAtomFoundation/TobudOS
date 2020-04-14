/**
 * 初始化蓝牙模块
 *
 * @return {Object} 调用结果
 */
function openBluetoothAdapter() {
  return new Promise((resolve, reject) => {
    wx.openBluetoothAdapter({
      success(res) {
        resolve(res)
      },
      fail(res) {
        reject(res)
      }
    })
  })
}

/**
 * 关闭蓝牙模块
 *
 * @return {Object} 调用结果
 */
function closeBluetoothAdapter() {
  return new Promise((resolve, reject) => {
    wx.closeBluetoothAdapter({
      complete(res) {
        resolve(res)
      }
    })
  })
}

/**
 * 监听蓝牙状态
 *
 * @param {function} onBluetoothStateChange 函数对象，监听蓝牙状态变化的回调函数
 * @return void
 */
function onBluetoothAdapterStateChange(onBluetoothStateChange) {
  return new Promise((resolve) => {
    wx.onBluetoothAdapterStateChange(function(res) {
      // console.log('onBluetoothAdapterStateChange, now is', res)
      onBluetoothStateChange(res)
    })
    resolve()
  })
}

/**
 * 启动搜索蓝牙设备功能
 *
 * @param {string} services 蓝牙服务
 * @return void
 */
function startBluetoothDevicesDiscovery(services) {
  return new Promise((resolve, reject) => {
    wx.startBluetoothDevicesDiscovery({
      services: services || [],
      allowDuplicatesKey: false,
      success(res) {
        console.log("wx.startBluetoothDevicesDiscovery success")
        resolve(res)
      },
      fail(res) {
        console.log("wx.startBluetoothDevicesDiscovery fail")
        reject(res)
      }
    })
  })
}

/**
 * 关闭搜索蓝牙设备功能
 *
 * @return void
 */
function stopBluetoothDevicesDiscovery() {
  return new Promise((resolve, reject) => {
    wx.stopBluetoothDevicesDiscovery({
      complete(res) {
        console.log("wx.stopBluetoothDevicesDiscovery success")
        resolve()
      }
    })
  })
}

/**
 * 断开与低功耗蓝牙设备的连接
 *
 * @param {string} deviceId 蓝牙设备ID
 * @return void
 */
function closeBLEConnection(deviceId) {
  return new Promise((resolve, reject) => {
    wx.closeBLEConnection({
      deviceId,
      success(res) {
        console.log(`closeBLEConnection ${deviceId} success`, res)
      },
      fail(res) {
        console.log(`closeBLEConnection ${deviceId} fail`, res)
      },
      complete(res) {
        resolve()
      }
    })
  })
}

/**
 * 连接低功耗蓝牙设备
 *
 * @param {string} deviceId 蓝牙设备ID
 * @return {Object} 调用结果
 */
function createBLEConnection(deviceId) {
  return new Promise((resolve, reject) => {
    wx.createBLEConnection({
      deviceId: deviceId,
      timeout: 5000, // TO_CHECK: 设置连接蓝牙超时时间
      success: function(res) {
        console.log(`createBLEConnection ${deviceId} success`, res)
        resolve(res)
      },
      fail: function(res) {
        console.log(`createBLEConnection ${deviceId} fail`, res);
        reject(new Error(res.errMsg || 'wx.createBLEConnection fail'))
      }
    })
  })
}

/**
 * 监听低功耗蓝牙连接状态的改变事件
 *
 * @param {function} onStateChange 函数对象，连接状态变化的回调函数
 * @return void
 */
function onBLEConnectionStateChange(onStateChange) {
  return new Promise((resolve) => {
    wx.onBLEConnectionStateChange(function(res) { // 该方法回调中可以用于处理连接意外断开等异常情况                
      // console.log(`onBLEConnectionStateChange device ${res.deviceId} state has changed, connected: ${res.connected}`)
      onStateChange(res)
    })
    resolve()
  })
}

/**
 * 获取蓝牙设备所有服务
 *
 * @param {string} deviceId 蓝牙设备ID
 * @return {Object} 调用结果
 */
function getBLEDeviceServices(deviceId) {
  return new Promise((resolve, reject) => {
    wx.getBLEDeviceServices({
      deviceId: deviceId,
      success: function(res) {
        console.log(`wx.getBLEDeviceServices ${deviceId} success:`, res.services)
        resolve(res.services)
      },
      fail(res) { // 获取蓝牙服务失败
        console.log(`wx.getBLEDeviceServices ${deviceId} fail:`, res)
        reject(res.errMsg || 'wx.getBLEDeviceServices fail')
      }
    })
  })
}

/**
 * 获取蓝牙设备某个服务中所有特征值
 *
 * @param {string} deviceId 蓝牙设备ID
 * @param {string} serviceId 蓝牙服务ID
 * @return {Object} 调用结果
 */
function getBLEDeviceCharacteristics(deviceId, serviceId) {
  return new Promise((resolve, reject) => {
    wx.getBLEDeviceCharacteristics({
      deviceId: deviceId,
      serviceId: serviceId,
      success(res) {
        console.log(`wx.getBLEDeviceCharacteristics ${deviceId} ${serviceId} success:`, res.characteristics)
        resolve(res.characteristics)
      },
      fail(res) { // 读取蓝牙特征值失败
        console.log(`wx.getBLEDeviceCharacteristics ${deviceId} ${serviceId} fail`, res)
        reject(res.errMsg || 'wx.getBLEDeviceCharacteristics fail')
      },
    })
  })
}

/**
 * 启用低功耗蓝牙设备特征值变化时的 notify 功能，订阅特征值
 * 注：必须设备的特征值支持 notify 或者 indicate 才可以成功调用
 *
 * @param {string} deviceId 蓝牙设备ID
 * @param {string} serviceId 蓝牙服务ID
 * @param {string} notifyCharacteristicId 蓝牙特征值ID
 * @return {Object} 调用结果
 */
function notifyBLECharacteristicValueChanged(deviceId, serviceId, notifyCharacteristicId) {
  return new Promise((resolve, reject) => {
    wx.notifyBLECharacteristicValueChanged({
      deviceId: deviceId,
      serviceId: serviceId,
      characteristicId: notifyCharacteristicId,
      state: true,
      success(res) {
        console.log(`wx.notifyBLECharacteristicValueChanged ${deviceId} ${serviceId} ${notifyCharacteristicId} success`);
        resolve(true)
      },
      fail(err) {
        console.log(`wx.notifyBLECharacteristicValueChanged ${deviceId} ${serviceId} ${notifyCharacteristicId} fail`, err);
        reject("启用蓝牙特征值notify功能失败")
      }
    })
  })
}

/**
 * 监听低功耗蓝牙设备的特征值变化事件
 * 注：必须先启用 notifyBLECharacteristicValueChange 接口才能接收到设备推送的 notification
 *
 * @param {function} onMessage 函数对象，读数据的回调函数
 * @return void
 */
function onBLECharacteristicValueChange(onMessage) {
  var receivedData = null
  return new Promise((resolve, reject) => {
    wx.onBLECharacteristicValueChange(function(res) {
      //{value: ArrayBuffer, deviceId: "D8:00:D2:4F:24:17", serviceId: "ba11f08c-5f14-0b0d-1080-007cbe238851-0x600000460240", characteristicId: "0000cd04-0000-1000-8000-00805f9b34fb-0x60800069fb80"} 
      onMessage(res.value)
    })
    resolve()
  })
}

/**
 * 向低功耗蓝牙设备特征值中写入二进制数据
 * 注：必须设备的特征值支持 write 才可以成功调用
 *
 * @param {string} deviceId 蓝牙设备ID
 * @param {string} serviceId 蓝牙服务ID
 * @param {string} writeCharacteristicId 蓝牙特征值ID
 * @param {string} payload 数据，必须是ArrayBuffer类型
 * @return {Object} 调用结果
 */
function writeBLECharacteristicValue(deviceId, serviceId, writeCharacteristicId, payload) {
  return new Promise((resolve, reject) => {
    wx.writeBLECharacteristicValue({
      deviceId: deviceId,
      serviceId: serviceId,
      characteristicId: writeCharacteristicId,
      value: payload, // parameter.value should be ArrayBuffer
      fail(res) {
        console.log(`writeBLECharacteristicValue fail: ${deviceId} ${serviceId} ${writeCharacteristicId}`, res)
        reject(res)
      },
      success(res) {
        resolve(res)
      }
    })
  })
}

/**
 * 读取低功耗蓝牙设备的特征值的二进制数据值。
 * 注：必须设备的特征值支持 read 才可以成功调用; 接口读取到的信息需要在 onBLECharacteristicValueChange 方法注册的回调中获取
 *
 * @param {string} deviceId 蓝牙设备ID
 * @param {string} serviceId 蓝牙服务ID
 * @param {string} readCharacteristicId 蓝牙特征值ID
 * @return {Object} 调用结果
 */
function readBLECharacteristicValue(deviceId, serviceId, readCharacteristicId) {
  return new Promise((resolve, reject) => {
    wx.readBLECharacteristicValue({
      deviceId: deviceId,
      serviceId: serviceId,
      characteristicId: readCharacteristicId,
      fail(res) {
        console.log(`readBLECharacteristicValue fail: ${deviceId} ${serviceId} ${readCharacteristicId}`, res)
        reject(res)
      },
      success(res) {
        resolve(res)
      }
    })
  })
}

/**
 * 连接设备
 *
 * @param {string} deviceId 蓝牙设备ID
 * @return {Object} 设备连接信息
 */
function connectDevice(deviceId) {
  return closeBLEConnection(deviceId).then(() => {
    // 断开重连需要间隔1s,否则会出现假连接成功
    // 还有一种方式是 getConnectedBluetoothDevices，对比已连接的蓝牙，如果已连接直接跳到设备页面
    return createBLEConnection(deviceId)
  }).then(() => {
    // 读取 BLE services
    return getBLEDeviceServices(deviceId)
  }).then(([deviceId, services]) => {
    // 读取 BLE Characteristics
    for (let i = 0; i < services.length; i++) {
      if (services[i].isPrimary) { // 确定对应服务。这里使用第一个primary服务
        let serviceId = services[i].uuid
        return getBLEDeviceCharacteristics(deviceId, serviceId)
      }
    }
    return Promise.reject(new Error('未找到蓝牙主服务'))
  }).then(
    // 必须先启用 notifyBLECharacteristicValueChange 才能监听到设备 onBLECharacteristicValueChange 事件
    ([deviceId, serviceId, characteristics]) => {
      let notifyCharacteristicId = null
      let writeCharacteristicId = null
      let readCharacteristicId = null
      characteristics.forEach(function(item, index) {
        if (item.properties.notify == true) {
          notifyCharacteristicId = item.uuid
          notifyBLECharacteristicValueChanged(deviceId, serviceId, item.uuid)
        }
        if (item.properties.write == true) {
          writeCharacteristicId = item.uuid
        }
        if (item.properties.read == true) {
          readCharacteristicId = item.uuid
        }
      })
      let device = {
        connected: true,
        deviceId: deviceId,
        serviceId: serviceId,
        notifyCharacteristicId: notifyCharacteristicId,
        writeCharacteristicId: writeCharacteristicId,
        readCharacteristicId: readCharacteristicId,
      }
      return Promise.resolve(device)
    }
  )
}

/**
 * 根据名称搜索设备
 *
 * @param {string} name 蓝牙设备名称
 * @param {string} timeout 搜索超时时间，单位ms
 * @param {array} services 蓝牙设备的服务ID
 * @return {string} deviceId 蓝牙设备ID
 */
function searchDeviceName(name, timeout, services = ["6E400001-B5A3-F393-E0A9-E50E24DCCA9E"]) {
  console.log("search begin", name)
  let timer = null
  let searchTimeout = new Promise((resolve, reject) => {
    timer = setTimeout(function() {
      console.log("searchDeviceName timeout", name)
      stopBluetoothDevicesDiscovery()
      reject(null)
    }, timeout);
  })

  let search = Promise.resolve()
    .then(() => {
      return closeBluetoothAdapter()
    })
    .then(() => {
      return openBluetoothAdapter()
    })
    .then(() => {
      return startBluetoothDevicesDiscovery(services)
    })
    .then(() => {
      return new Promise((resolve) => {
        console.log("wx.onBluetoothDeviceFound start")
        wx.onBluetoothDeviceFound(function(res) {
          let foundName = res.devices[0].localName // TODO: localName or name?
          if (foundName && foundName == name) {
            console.log("searchDeviceName found:", name, res);
            wx.offBluetoothDeviceFound(this)
            clearTimeout(timer)
            stopBluetoothDevicesDiscovery()
            resolve(res.devices[0].deviceId)
          }
        })
      })
    })

  return Promise.race([search, searchTimeout])
}

async function searchDevices(name, timeout, services = ["6E400001-B5A3-F393-E0A9-E50E24DCCA9E"]) {
  console.log("searchDevices", name)
  let timer = null
  let searchTimeout = new Promise((resolve, reject) => {
    timer = setTimeout(function() {
      console.log("searchDevices timeout", name)
      stopBluetoothDevicesDiscovery()
      reject(null)
    }, timeout);
  })

  let search = Promise.resolve()
    .then(() => {
      return closeBluetoothAdapter()
    })
    .then(() => {
      return openBluetoothAdapter()
    })
    .then(() => {
      return startBluetoothDevicesDiscovery(services)
    })
    .then(() => {
      return new Promise((resolve) => {
        console.log("wx.onBluetoothDeviceFound start")
        wx.onBluetoothDeviceFound(function(res) {
          let foundName = res.devices[0].localName // TODO: localName or name?
          if (foundName && foundName == name) {
            console.log("searchDeviceName found:", name, res);
            wx.offBluetoothDeviceFound(this)
            clearTimeout(timer)
            stopBluetoothDevicesDiscovery()
            resolve(res.devices[0].deviceId)
          }
        })
      })
    })

  return Promise.race([search, searchTimeout])
}

// function searchAndConnect(name, timeout, services = ["6E400001-B5A3-F393-E0A9-E50E24DCCA9E"]) {
function searchAndConnect(name, timeout, services = []) {
  return searchDeviceName(name, timeout, services).then(res => {
    let devicesList = res
    if (devicesList.length == 0) {
      return Promise.reject(false)
    }
    let deviceId = devicesList[0].deviceId
    return connectDevice(deviceId)
  })
}

// 扫描设备
function scanDevicesForSometime(services, ms) {
  startBluetoothDevicesDiscovery(services).then(res => {
    console.log("wx.startBluetoothDevicesDiscovery success", res)
  })
  setTimeout(function() {
    wx.stopBluetoothDevicesDiscovery({
      success(res) {
        console.log("wx.stopBluetoothDevicesDiscovery success", res)
      },
      fail(e) {
        console.log("wx.stopBluetoothDevicesDiscovery fail", res)
      }
    })
  }, ms);
}

// 扫描设备
function connectFailAnalysis(deviceId) {
  console.log("scanDevices ...")
  let that = this
  let found = false
  let discovery = Promise.resolve().then(
    () => {
      return openBluetoothAdapter()
    }
  ).then(
    () => {
      return startBluetoothDevicesDiscovery([])
      // return startBluetoothDevicesDiscovery(["6E400001-B5A3-F393-E0A9-E50E24DCCA9E"])
    }
  ).then(
    () => {
      let devicesList = []
      return new Promise((resolve) => {
        wx.onBluetoothDeviceFound(function(res) {
          let name = res.devices[0].name
          if (name != "") {
            console.log("onBluetoothDeviceFound:", name, res);
            devicesList.push(res.devices[0])
            let foundDeviceId = res.devices[0].deviceId
            console.log("found", foundDeviceId, deviceId)
            // 尝试连接，读取等
            if (foundDeviceId = deviceId) {
              found = true
            }
          }
        })
        resolve()
      })
    }
  )

  let discoveryTimeout = new Promise(resolve => {
    setTimeout(resolve, 10000);
  }).then(res => {
    console.log("discoveryTimeout")
    return stopBluetoothDevicesDiscovery()
  }).then(res => {
    return new Promise(res => {
      console.log("xxxx", found)
      if (found) {
        console.log("found", deviceId)
        resolve(true)
      } else {
        console.log("not found", deviceId)
        reject(false)
      }
    })
  })

  return Promise.race([discovery, discoveryTimeout])
}

module.exports = {
  // 蓝牙适配器
  closeBluetoothAdapter: closeBluetoothAdapter,
  openBluetoothAdapter: openBluetoothAdapter,
  onBluetoothAdapterStateChange: onBluetoothAdapterStateChange,
  startBluetoothDevicesDiscovery: startBluetoothDevicesDiscovery,
  stopBluetoothDevicesDiscovery: stopBluetoothDevicesDiscovery,

  // BLE连接
  closeBLEConnection: closeBLEConnection,
  createBLEConnection: createBLEConnection,
  onBLEConnectionStateChange: onBLEConnectionStateChange,

  // BLE服务和特征值
  getBLEDeviceServices: getBLEDeviceServices,
  getBLEDeviceCharacteristics: getBLEDeviceCharacteristics,
  notifyBLECharacteristicValueChanged: notifyBLECharacteristicValueChanged,
  onBLECharacteristicValueChange: onBLECharacteristicValueChange,

  // BLE读写
  writeBLECharacteristicValue: writeBLECharacteristicValue,
  readBLECharacteristicValue: readBLECharacteristicValue,

  // wrapper
  scanDevicesForSometime: scanDevicesForSometime,
  searchDeviceName: searchDeviceName,
  connectDevice: connectDevice,
  searchAndConnect: searchAndConnect,
  connectFailAnalysis: connectFailAnalysis,
}