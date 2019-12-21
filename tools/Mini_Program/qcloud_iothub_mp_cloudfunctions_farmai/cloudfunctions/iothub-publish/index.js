const tencentcloud = require("tencentcloud-sdk-nodejs");

const IotcloudClient = tencentcloud.iotcloud.v20180614.Client;
const models = tencentcloud.iotcloud.v20180614.Models;

const Credential = tencentcloud.common.Credential;
const ClientProfile = tencentcloud.common.ClientProfile;
const HttpProfile = tencentcloud.common.HttpProfile;

let cred = new Credential("AKIDxZ2LTTi84eaR3OfS8hnfxd4JHzQatFpF", "UigMrA9rJXUCeoiBQP8dsJuJ6sYzmNjs");
let httpProfile = new HttpProfile();
httpProfile.endpoint = "iotcloud.tencentcloudapi.com";
let clientProfile = new ClientProfile();
clientProfile.httpProfile = httpProfile;
let client = new IotcloudClient(cred, "ap-guangzhou", clientProfile);

// 云函数入口函数
exports.main = async (event, context) => {
  let req = new models.PublishMessageRequest();
  req.Topic = "U1BZWHF7F9/dev_01/data";
  req.DeviceName = "dev_01";
  req.ProductId = "U1BZWHF7F9";
  req.Payload = event.Payload;

  return new Promise((resolve, reject)=>{
    client.PublishMessage(req, function (errMsg, response) {
      if (errMsg) {
        console.log(errMsg);
        reject(errMsg)
        return;
      }
      console.log(response.to_json_string());
      resolve(response)
    });
  })
}