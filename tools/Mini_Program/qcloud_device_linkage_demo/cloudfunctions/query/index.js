// 云函数入口文件
const cloud = require('wx-server-sdk')
const tencentcloud = require("tencentcloud-sdk-nodejs");

const IotexplorerClient = tencentcloud.iotexplorer.v20190423.Client;
const models = tencentcloud.iotexplorer.v20190423.Models;

const Credential = tencentcloud.common.Credential;
const ClientProfile = tencentcloud.common.ClientProfile;
const HttpProfile = tencentcloud.common.HttpProfile;
cloud.init()

// 云函数入口函数
exports.main = async(event, context) => {
  console.log("event:", event);
  let cred = new Credential(event.SecretId, event.SecretKey);
  let httpProfile = new HttpProfile();
  httpProfile.endpoint = "iotexplorer.tencentcloudapi.com";
  let clientProfile = new ClientProfile();
  clientProfile.httpProfile = httpProfile;
  let client = new IotexplorerClient(cred, "ap-guangzhou", clientProfile);

  let req = new models.DescribeDeviceDataRequest();
  req.ProductId = event.ProductId;
  req.DeviceName = event.DeviceName;
  console.log("req:", req);
  return new Promise((resolve, reject) => {
    client.DescribeDeviceData(req, function(errMsg, response) {
      if (errMsg) {
        console.log(errMsg);
        reject(errMsg);
      }
      console.log(response);
      resolve(response)
    });
  })
}