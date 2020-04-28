//app.js
App({
  globalData: {
  },
  onLaunch: function () {
    this.globalData.SystemInfo = wx.getSystemInfoSync()
    //console.log(this.globalData.SystemInfo)
    wx.setKeepScreenOn({
      keepScreenOn: true
    })
  },
})