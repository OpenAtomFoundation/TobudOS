const tencentcloud = require("tencentcloud-sdk-nodejs");

const IotcloudClient = tencentcloud.iotcloud.v20180614.Client;
const models = tencentcloud.iotcloud.v20180614.Models;

const Credential = tencentcloud.common.Credential;
const ClientProfile = tencentcloud.common.ClientProfile;
const HttpProfile = tencentcloud.common.HttpProfile;

exports.main = async (event, context) => {
  let cred = new Credential(event.SecretId, event.SecretKey);
  let httpProfile = new HttpProfile();
  httpProfile.endpoint = "iotcloud.tencentcloudapi.com";
  let clientProfile = new ClientProfile();
  clientProfile.httpProfile = httpProfile;
  let client = new IotcloudClient(cred, "ap-guangzhou", clientProfile);
  let req = new models.DescribeDeviceShadowRequest();
  req.DeviceName = event.DeviceName;
  req.ProductId = event.ProductId;

  return new Promise((resolve, reject)=>{
    client.DescribeDeviceShadow(req, function (errMsg, response) {

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
