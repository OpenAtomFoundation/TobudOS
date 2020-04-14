// 1. 小程序发布版本或者体验版本，连续写ble需要加延时，否则会导致写失败1008。本地调试本身有延时，因此不需要加，需要注释掉
// 2. 安卓系统调试蓝牙会有黏包问题。[ACK, ACK, ..] [C, C] ...
// 3. ymodem.h DOWNLOAD_TIMEOUT 配置需要注意，过小的话，写block时间过长会导致接收端超时，重新进入普通模式
// 4. NAK 重传逻辑暂不支持
// https://www.amobbs.com/thread-5704281-1-1.html

const app = getApp()
const util = require('../../../../utils/util.js');
const bleapi = require('../../../../utils/hardware/ble/ble-api.js');
const Packet = require('../../../../utils/hardware/ble/packet.js')

var msgQueue = []
var fileBuffer
var fileName
var bUse1K = true
var DownLoadMode = 0
const SyncTimeout = 15000 // 等待C
const RecvTimeout = 5000  // 等待ACK

/*
SOH	0x01	协议头(128bytes类型)
STX	0x02	协议头(1k类型)
EOT	0x04	传输结束
ACK	0x06	接收响应
NAK	0x15	失败响应
CAN 0x18	取消传输
C	  0x43	开启文件传输
*/
const EOT = 0x04
const ACK = 0x06
const CAN = 0x18
const NAK = 0x15
const C = 0x43

Page({
  data: {
    device: {
      // connected: true,
      // deviceId: "90:9A:77:26:5D:64",
      // name: "HC-08",
    },
    receiveText: '',
    downloadLog: '',
  },
  acceptDataFromPrevPage() {
    const eventChannel = this.getOpenerEventChannel()
    return new Promise((resolve, reject) => {
      eventChannel.on('eventData', function (data) {
        resolve(data)
      })
    })
  },
  onLoad: async function(options) {
    console.log(util.formatTime())
    let data = await this.acceptDataFromPrevPage()
    console.log("accept data from prev page", data)
    this.setData({
      device: data.device,
    })

    // 监听低功耗蓝牙连接状态的改变事件。包括开发者主动连接或断开连接，设备丢失，连接异常断开等等
    await bleapi.onBLEConnectionStateChange(this.onStateChange)
    // 监听低功耗蓝牙设备的特征值变化事件。必须先启用 notifyBLECharacteristicValueChange 接口才能接收到设备推送的 notification
    if (this.data.device.characteristic.properties.notify == true) {
      console.log("notifyBLECharacteristicValueChange and register value change callback")
      await bleapi.notifyBLECharacteristicValueChanged(
        this.data.device.deviceId,
        this.data.device.service.uuid,
        this.data.device.characteristic.uuid)
      await bleapi.onBLECharacteristicValueChange(this.onMessage)
    }
  },
  onUnload: function(e) {
    let deviceId = this.data.device.deviceId
    console.log("onUnload close the ble connection", deviceId)
    bleapi.closeBLEConnection(deviceId)
  },
  onStateChange: function(res) {
    console.log("onBLEConnectionStateChange", res.connected)
    this.setData({
      [`device.connected`]: res.connected
    })
  },
  formSubmit: async function(e) {
    console.log(e.detail.value)
    let value = e.detail.value.textarea
    try {
      await this.writeData(util.str2abUint8(value))
      wx.showToast({
        icon: 'none',
        title: '写数据成功',
        duration: 1000,
      })
    } catch (e) {
      wx.showToast({
        icon: 'none',
        title: '写数据失败',
        duration: 2000,
      })
    }
  },
  formReset(e) {
    console.log(e)
    this.setData({
      inputText: '',
    })
  },
  bindTextAreaBlur: function(e) {
    console.log(e.detail.value)
    this.setData({
      inputText: e.detail.value,
    })
  },
  chose: function() {
    wx.chooseMessageFile({
      count: 1,
      type: 'file',
      success: (res) => {
        console.log(res)
        fileName = res.tempFiles[0].name
        wx.getFileSystemManager().readFile({
          filePath: res.tempFiles[0].path,
          // encoding: 'binary',
          success: (res2) => {
            console.log(res2)
            fileBuffer = new Uint8Array(res2.data)
            this.setData({
              file_name: fileName,
              file_size: res.tempFiles[0].size,
            })
          }
        })
      }
    })
  },
  writeData: async function(payload) {
    console.log(`writeData len(${payload.byteLength})`, payload, new Date().toISOString())
    if (!this.data.device.connected) {
      wx.showModal({
        title: '提示',
        content: '蓝牙已断开，请重新连接设备',
        showCancel: false,
        success: function(res) {}
      })
      return Promise.reject(`ble disconnect`)
    }

    let pos = 0;
    let bytes = payload.byteLength
    while (bytes > 0) {
      let frameBuffer;
      if (bytes > 20) {
        frameBuffer = payload.slice(pos, pos + 20);
        pos += 20;
        bytes -= 20;
      } else {
        frameBuffer = payload.slice(pos, pos + bytes);
        pos += bytes;
        bytes -= bytes;
      }
      // console.log(`frame(${frameBuffer.byteLength})`, frameBuffer)
      try {
        await bleapi.writeBLECharacteristicValue(
          this.data.device.deviceId,
          this.data.device.service.uuid,
          this.data.device.characteristic.uuid,
          frameBuffer
        )
        // Android手机，连续调用会存在写失败的可能性，建议加延时
        await util.delayMs(20)
      } catch (e) {
        return Promise.reject(e)
      }
    }
    return Promise.resolve()
  },
  onMessage: function(data) {
    let datastr = util.ab2strHex(data)
    console.log('onMessage:', datastr, msgQueue, new Date().toISOString())
    let str = util.ArrayBuffer2str(data)
    this.setData({
      receiveText: this.data.receiveText + str
    })

    if (DownLoadMode) { // 下载模式
      let arr = new Uint8Array(data)
      for (let i = 0; i < arr.length; i++) {
        if (arr[i] == CAN) { // 取消传输
          DownLoadMode = false
        }
        msgQueue.push(arr[i])
      }
      this.setData({
        downloadLog: this.data.downloadLog + `${datastr}\r\n`
      })
    } else { // 普通模式
      console.log("normal onReceive:", str)
    }
  },
  upload: async function() {
    let that = this
    if (!this.data.device.connected) {
      wx.showModal({
        title: '提示',
        content: '蓝牙已断开',
        showCancel: false,
        success: function(res) {}
      })
      return
    }

    if (!fileName || fileBuffer.length == 0) {
      wx.showModal({
        content: "请先选择固件",
        showCancel: false
      });
      return
    }

    wx.showLoading({
      title: '准备上传...',
      mask: true,
    })
    this.setData({
      send_percent: 0,
      timestamp_start: "",
      timestamp_end: "",
    })
    DownLoadMode = 1
    console.log(`upload begin. bytes: ${fileBuffer.length} /1024: ${fileBuffer.length/1024}`)
    try {
      await this.recieveByte(C, SyncTimeout)
      
      wx.showLoading({
        title: '开始上传...',
        mask: true,
      })

      this.setData({
        timestamp_start: util.formatTime(),
        date_start: new Date()
      })

      console.log("send blockZero begin")
      let id = 0
      let blockZero = Packet.getNormalPacket(id, Packet.getZeroContent(fileName, fileBuffer.length))
      await that.writeData(blockZero.buffer)
      await that.recieveByte(ACK, RecvTimeout)
      await that.recieveByte(C, SyncTimeout)
      console.log("send blockZero complete")
      wx.showLoading({
        title: '上传中...',
        mask: true,
      })

      console.log("send file begin")
      let nInterval = (bUse1K == true) ? 1024 : 128;
      for (let i = 0; i < fileBuffer.length; i += nInterval) {
        console.log("send block " + (i / nInterval + 1) + " start");
        let upper = (fileBuffer.length < i + nInterval) ?
          fileBuffer.length : i + nInterval;
        let payloadBuf = new Uint8Array(nInterval);
        for (let j = i; j < upper; j++) {
          payloadBuf[j - i] = fileBuffer[j];
        }
        id = i / nInterval + 1;
        let block = (bUse1K == true) ? Packet.getLongPacket(id, payloadBuf) : Packet.getNormalPacket(id, payloadBuf);

        await that.writeData(block.buffer)
        try {
          // TODO: NAK 重传逻辑
          await that.recieveByte(ACK, RecvTimeout)
        } catch (e) {
          console.error(`${(i / nInterval) + 1} block lost a ack`, e)
        }

        console.log("send block " + ((i / nInterval) + 1) + " succceed!");
        this.setData({
          send_percent: parseInt((i) / fileBuffer.length * 100),
        })
      }
      console.log("send file complete")

      // EOT处理
      // 超时，
      // 返回 NAK, 则重传EOT
      // 返回 ACK, 则结束
      console.log("send EOT");
      let sendEOTAndRetry = this.autoRetry(async function(buf) {
        await that.writeData(new Uint8Array([EOT]).buffer)
        console.log(`waiting ACK`, new Date().toISOString())
        let c = await that.recieve(RecvTimeout)
        if (c == ACK) {
          console.log(`receive ACK 0x${c.toString(16)}`)
          // 处理黏包的情况 [ACK, ACK]
          while (msgQueue.length > 0) {
            if (msgQueue[0] == ACK) {
              let c = msgQueue.shift()
              console.log(`shift ${c.toString(16)} from queue`, msgQueue)
            } else {
              break
            }
          }
        } else if (c == NAK) {
          console.log(`receive NAK 0x${c.toString(16)}`)
          // 处理黏包情况 [NAK, ACK, C]
          if (msgQueue.length > 0) {
            await that.recieveByte(ACK, RecvTimeout)
          } else {
            throw `receive NAK 0x${c.toString(16)}, resend EOT`
          }
        }
      }, 2)
      await sendEOTAndRetry(new Uint8Array([EOT]).buffer)
      await that.recieveByte(C, SyncTimeout)

      console.log("send last block begin")
      let blockLast = Packet.getNormalPacket(0, new Uint8Array(128));
      await that.writeData(blockLast.buffer)
      await that.recieveByte(ACK, RecvTimeout)
      console.log("send last block compelte")

      DownLoadMode = 0
      let diff = new Date().getTime() - this.data.date_start.getTime()
      let rate = Math.round(fileBuffer.length * 1000 / diff) * 100 / 100
      console.log("upload complete")
      this.setData({
        send_percent: 100,
        timestamp_end: util.formatTime(),
        rate: rate,
      })
      wx.showModal({
        content: "上传完成",
        showCancel: false
      });
    } catch (e) {
      DownLoadMode = 0
      console.error("upload fail:", e)
      wx.showModal({
        content: "上传失败，请重试\r\n" + e,
        showCancel: false
      });
    }
    wx.hideLoading()
    console.log("msgQueue", msgQueue)
    msgQueue = [] // clear msgQueue
  },
  reciveByteAndRetry: async function(expect, timeout, retryNum) {
    let func = this.autoRetry(this.recieveByte, retryNum)
    await func(expect, timeout)
  },
  recieveByte: async function(expect, timeout) {
    console.log(`waiting 0x${expect.toString(16)} for ${timeout}ms ...`, new Date().toISOString())
    try {
      let c = await this.recieve(timeout)
      if (c == CAN) { // 接收端终止
        throw `receive 0x${c.toString(16)}. cancel tranmission`
      }
      if (c != expect) {
        throw `expect 0x${expect.toString(16)}, but recieve 0x${c.toString(16)}`
      }
      // 处理黏包的情况
      while (msgQueue.length > 0) {
        if (msgQueue[0] == expect) {
          let c = msgQueue.shift()
          console.log(`shift ${c.toString(16)} from queue`, msgQueue)
        } else {
          break
        }
      }
      Promise.resolve(c)
    } catch (e) {
      throw `waiting 0x${expect.toString(16)} fail: ${e}`
    }
  },
  recieve: async function(timeout) {
    // console.log("start recieve", new Date().toISOString())
    let elaspe = timeout
    const interval = 500
    while (elaspe > 0) {
      elaspe = elaspe - interval
      // console.log(`xxxx`, msgQueue.length, elaspe)
      if (msgQueue.length > 0) {
        let c = msgQueue.shift();
        console.log(`receive from queue: ${c.toString(16)}`, msgQueue)
        return Promise.resolve(c)
      }
      await util.delayMs(interval)
    }
    return Promise.reject(`receive timeout ${timeout}ms ${new Date().toISOString()}`)
  },
  /*
  example:
  let sendLastblockAndRetry = this.autoRetry(async function (buf) {
    await that.writeData(buf);
    await that.recieveByte(ACK, RecvTimeout)
  }, 3)
  await sendLastblockAndRetry(blockLast.buffer)
  */
  autoRetry(func, retryMax) {
    let retryNum = retryMax;
    return async function funcR() {
      let params = arguments
      while (retryNum--) {
        try {
          await func(...params)
          break
        } catch (e) {
          if (retryNum > 0) {
            console.error(`retry ${retryMax - retryNum} time. error:`, e);
            continue
          } else {
            throw e
          }
        }
      }
    }
  },
  RecvCleanTap: function() {
    this.setData({
      receiveText: ''
    })
  },
  sendValue: async function(e) {
    let val = e.currentTarget.dataset.val
    try {
      await this.writeData(util.str2abUint8(val))
      wx.showToast({
        icon: 'none',
        title: '发送成功',
        duration: 1000,
      })
    } catch (e) {
      wx.showToast({
        icon: 'none',
        title: '发送失败',
        duration: 2000,
      })
    }
  },
})