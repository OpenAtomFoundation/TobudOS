const app = getApp()
const config = require('../../config.js')
var util = require('../../utils/util.js');
var wxCharts = require('../../utils/wxcharts.js');
var lineChart = null;
var startPos = null;

Page({
  data: {
    tempHigh: 20,
    sensorDevice: {},
    executeDevice: {},
    chartDataTemp: [],
    chartDataHumi: [],
    chartCategories: [],
  },
  onLoad: async function(options) {
    console.log("index onLoad")
    while (true) {
      let executor = await this.queryExecuteDeviceStatus()
      let sensor = await this.querySensorDeviceStatus()
      this.renderChart(sensor.temperature.Value, sensor.humidity.Value)
      let temp = sensor.temperature.Value
      console.log(temp)
      if (temp >= this.data.tempHigh) {
        if (executor.light == 0) {
          wx.showModal({
            title: '提示',
            content: `温度达到门限(${this.data.tempHigh})，打开风扇和灯`,
            showCancel: false,
          })
          this.turnOnLight()
          this.turnOnFan()
        }
      } else {
        // this.turnOffLight()
        // this.turnOffFan()
      }
      await util.delayMs(5000)
    }
  },
  sliderChange(e) {
    console.log(e.detail)
    this.setData({
      tempHigh: e.detail.value
    })
  },
  renderChart(temperature, humidity) {
    let epoch = Math.round(new Date().getTime() / 1000)
    let timestamp = util.formatEpoch(epoch)
    if (this.data.chartDataTemp.length > 5) {
      this.data.chartDataTemp.pop();
      this.data.chartDataHumi.pop();
      this.data.chartCategories.pop();
    }
    this.data.chartDataTemp.unshift(temperature)
    this.data.chartDataHumi.unshift(humidity)
    this.data.chartCategories.unshift(timestamp.substring(11))
    this.setData({
      timestamp: timestamp,
      chartDataTemp: this.data.chartDataTemp,
      chartDataHumi: this.data.chartDataHumi,
      chartCategories: this.data.chartCategories,
    })
    this.renderChartTemp(400, 160)
    this.renderChartHumi(400, 160)
  },
  // 查询传感器设备状态
  async querySensorDeviceStatus() {
    let res = await wx.cloud.callFunction({
      name: 'query',
      data: config.SensorDevice
    })
    console.log("sensorDevice status", res.result.Data)
    let data = JSON.parse(res.result.Data)
    this.setData({
      sensorDevice: data
    })
    return this.data.sensorDevice
  },
  // 查询执行设备状态
  async queryExecuteDeviceStatus() {
    let res = await wx.cloud.callFunction({
      name: 'iothub-shadow-query',
      data: config.ExecuteDevice,
    })
    console.log("executeDevice status", res.result)
    let deviceData = JSON.parse(res.result.Data)
    // this.setData({
    //   executeDevice: deviceData.payload.state.reported
    // })
    return deviceData.payload.state.reported
  },
  async turnOnLight() {
    await this.control({
      light: 1
    })
    this.setData({
      [`executeDevice.light`]: 1
    })
  },
  async turnOffLight() {
    await this.control({
      light: 0
    })
    this.setData({
      [`executeDevice.light`]: 0
    })
  },
  async turnOnFan() {
    await this.control({
      motor: 1
    })
    this.setData({
      [`executeDevice.motor`]: 1
    })
  },
  async turnOffFan() {
    await this.control({
      motor: 0
    })
    this.setData({
      [`executeDevice.motor`]: 0
    })
  },
  async control(obj) {
    let data = config.ExecuteDevice
    data.Payload = JSON.stringify(obj)
    await wx.cloud.callFunction({
      name: 'iothub-publish',
      data: data
    })
  },
  async switchChange(e) {
    let value = 0
    if (e.detail.value == true) {
      value = 1
    }
    let item = e.currentTarget.dataset.item
    let obj = {
      [`${item}`]: value
    }
    wx.showLoading()
    let data = config.ExecuteDevice
    data.Payload = JSON.stringify(obj)
    console.log(data)
    await wx.cloud.callFunction({
      name: 'iothub-publish',
      data: data
    })
    wx.hideLoading()
  },
  touchHandler: function(e) {
    if (!lineChart) {
      return;
    }
    lineChart.scrollStart(e);
  },
  moveHandler: function(e) {
    if (!lineChart) {
      return;
    }
    lineChart.scroll(e);
  },
  touchEndHandler: function(e) {
    if (!lineChart) {
      return;
    }
    lineChart.scrollEnd(e);
    lineChart.showToolTip(e, {
      format: function(item, category) {
        return category + ' ' + item.name + ':' + item.data
      }
    });
  },
  renderChartHumi: function(windowWidth, windowHeight) {
    lineChart = new wxCharts({
      canvasId: 'chartHumi',
      type: 'line',
      categories: this.data.chartCategories, // simulationData.categories,
      animation: false,
      series: [{
        name: '湿度',
        data: this.data.chartDataHumi, // simulationData.data2,
        format: function(val, name) {
          return val.toFixed(2) + "%";
          // return val + "%";
        }
      }],
      xAxis: {
        disableGrid: false,
        fontColor: '#666666',
      },
      yAxis: {
        // title: '湿度 (%)',
        // format: function (val) {
        //   return val.toFixed(2);
        // },        
        max: 100,
        min: 50,
        disabled: false,
      },
      width: windowWidth,
      height: windowHeight,
      legend: true,
      dataLabel: true,
      dataPointShape: true,
      enableScroll: true,
      extra: {
        lineStyle: 'curve'
      },
    });
  },
  renderChartTemp: function(windowWidth, windowHeight) {
    lineChart = new wxCharts({
      canvasId: 'chartTemp',
      type: 'line',
      categories: this.data.chartCategories, // simulationData.categories,
      animation: false,
      series: [{
        name: '温度',
        data: this.data.chartDataTemp, //simulationData.data,
        format: function(val, name) {
          return val.toFixed(2) + "℃";
          // return val + "℃";
        }
      }],
      xAxis: {
        disableGrid: false
      },
      yAxis: {
        // title: '温度 (℃)',
        // format: function (val) {
        //   return val.toFixed(2);
        // },        
        min: 15,
        max: 25,
        disabled: false,
      },
      width: windowWidth,
      height: windowHeight,
      legend: true,
      dataLabel: true,
      dataPointShape: true,
      enableScroll: true,
      extra: {
        lineStyle: 'curve'
      },
    });
  },
  createSimulationData: function() {
    var categories = [];
    var data = [];
    var data2 = [];
    for (var i = 0; i < 10; i++) {
      categories.push('201620162-' + (i + 1));
      data.push(Math.random() * (20 - 10) + 10);
      data2.push(Math.random() * (20 - 10) + 30);
    }
    return {
      categories: categories,
      data: data,
      data2: data2
    }
  },
})