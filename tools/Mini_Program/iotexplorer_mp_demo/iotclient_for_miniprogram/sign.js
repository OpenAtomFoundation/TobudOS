const crypto = require('./sha1.js');

/**
 * @inner
 */
class Sign {

  static sign(secretKey, signStr) {
    // let signMethodMap = {
    //  HmacSHA1: "sha1",
    //  HmacSHA256: "sha256"
    // };

    // if (!signMethodMap.hasOwnProperty(signMethod)) {
    //  throw new Exception("signMethod invalid, signMethod only support (HmacSHA1, HmacSHA256)");
    // }
    // let hmac = crypto.createHmac(signMethodMap[signMethod], secretKey || "");
    // return hmac.update(Buffer.from(signStr, 'utf8')).digest('base64')
    // let test = crypto.sha1_digest("12345678", "87654321");
    let hmac = crypto.sha1_digest(secretKey,signStr);
    return hmac;
  }
}
module.exports = Sign;
