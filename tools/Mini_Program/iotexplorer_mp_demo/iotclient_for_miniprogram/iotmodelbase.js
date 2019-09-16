/**
 * @inner
 */
class AbstractModel {
  constructor() {

  }

  /**
   * @inner
   */
  deserialize(params) {
  }

  /**
   * 将object转化为json格式的string
   * @return {string}
   */
  to_json_string() {
    return JSON.stringify(this);
  }

  /**
   * 将json格式的string转化为object
   * @param  {string} dataString
   */
  from_json_string(dataString) {
    let params = JSON.parse(dataString);
    this.deserialize(params);
  }
}
module.exports = AbstractModel;