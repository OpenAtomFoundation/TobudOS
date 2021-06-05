const Sign = require("./sign.js");

/**
 * @inner
 */
class AbstractClient {

  /**
   * 实例化client对象
   * @param {string} productid 产品ID
   */
  constructor( secretid,secretkey) {
 
    this.endpoint = "iotexplorer.tencentcloudapi.com";
    this.path = "/";
    this.method = "GET";
    this.apiVersion = "2019-04-23";
    this.region = "ap-guangzhou";

    this.secretid = secretid;
    this.secretkey = secretkey;
  

  }

  /**
   * @inner
   */
  succRequest(resp, cb, data) {
    resp.deserialize(data);
    cb(null, resp);
  }

  /**
   * @inner
   */
  failRequest(err, cb) {
    cb(err, null);
  }

  /**
   * @inner
   */
  //request(action, req, resp, cb) {
  //  this.doRequest(action, req).then(data => this.succRequest(resp, cb, data), error => this.failRequest(error, cb));
  //}

  /**
   * @inner
   */
  request(action, req, resp, cb) {
    let params = this.mergeData(req);
    params = this.formatRequestData(action, params);
    var fullurl = this.formatFullUrl(params);
    var thisclient =  this;

    console.log(fullurl);


    wx.request({
      url: fullurl, 
      header: {
        'content-type': 'application/json' // 默认值
      },
      success(res) {
        console.log("statusCode", res.statusCode)
        // console.log("header", res.header)
        console.log("data", res.data)
        if(res.statusCode != 200){
          thisclient.failRequest(res.statusCode, cb);
        }
        else {
          //data = JSON.parse(res.data);
          var data = res.data;
          if (data.Response.Error) {
            thisclient.failRequest(data.Response.Error.Message, cb);
            return;
          }
          thisclient.succRequest(resp, cb, data.Response);
        }
      },
      fail(res){
        console.log("fail res", res)
        if (!res.statusCode) {
          res.statusCode = res.errMsg
        }
        thisclient.failRequest(res.statusCode, cb);
      }
    })


  }

  /**
   * @inner
   */
  mergeData(data, prefix = "") {
    let ret = {};
    for (let k in data) {
      if (data[k] === null) {
        continue;
      }
      if (data[k] instanceof Array || data[k] instanceof Object) {
        Object.assign(ret, this.mergeData(data[k], prefix + k + "."));
      } else {
        ret[prefix + k] = data[k];
      }
    }
    return ret;
  }

  /**
   * @inner
   */
  formatRequestData(action, params) {
    params.Action = action;
    params.Nonce = Math.round(Math.random() * 65535);
    params.Timestamp = Math.round(Date.now() / 1000);
    params.Version = this.apiVersion;

    if (this.secretid) {
      params.SecretId = this.secretid;
    }

    if (this.region) {
      params.Region = this.region;
    }

    let signStr = this.formatSignString(params);
    params.Signature = encodeURIComponent(Sign.sign(this.secretkey, signStr));
    return params;
  }

  /**
   * @inner
   */
  formatSignString(params) {
    let strParam = "";
    let keys = Object.keys(params);
    keys.sort();
    for (let k in keys) {
      //k = k.replace(/_/g, '.');
      strParam += ("&" + keys[k] + "=" + params[keys[k]]);
    }

    //签名原文串的拼接规则为: 请求方法 + 请求主机 +请求路径 + ? + 请求字符串
    let strSign = this.method + this.endpoint + this.path + "?" + strParam.slice(1);
    return strSign;
  }


  /**
   * @inner
   */
  formatFullUrl(params) {
    let strParam = "";
    let keys = Object.keys(params);
    //keys.sort();
    for (let k in keys) {
      //k = k.replace(/_/g, '.');
      strParam += ("&" + keys[k] + "=" + params[keys[k]]);
    }
    let strUrl = "https://" + this.endpoint + this.path + "?" + strParam.slice(1);
    return strUrl;
  }
}
module.exports = AbstractClient;
