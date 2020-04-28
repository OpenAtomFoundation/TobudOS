// await util.delayMs(1000)
const delayMs = (ms) => {
  return new Promise(resolve => {
    setTimeout(resolve, ms);
  });
}

function Appendzero(obj) {
  if (obj < 10) return "0" + "" + obj;
  else return obj;
}

// epoch => datetime str
// 1578450878 => 2019-10-23 18:05:30
function formatEpoch(epoch) {
  var dateTime = new Date(parseInt(epoch) * 1000)
  var year = dateTime.getFullYear();
  var month = dateTime.getMonth() + 1;
  var day = dateTime.getDate();
  var hour = dateTime.getHours();
  var minute = dateTime.getMinutes();
  var second = dateTime.getSeconds();
  var now = new Date();
  var now_new = Date.parse(now.toDateString()); //typescript转换写法
  var milliseconds = now_new - dateTime;
  var timeSpanStr = year + '-' + Appendzero(month) + '-' + Appendzero(day) + ' ' + Appendzero(hour) + ':' + Appendzero(minute) + ':' + Appendzero(second);
  return timeSpanStr;
}

module.exports = {
  delayMs: delayMs,
  formatEpoch: formatEpoch,
}