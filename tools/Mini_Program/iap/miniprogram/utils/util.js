const formatTime = function(date) {
  if (!date) {
    date = new Date()
  }
  const year = date.getFullYear()
  const month = date.getMonth() + 1
  const day = date.getDate()
  const hour = date.getHours()
  const minute = date.getMinutes()
  const second = date.getSeconds()

  return [year, month, day].map(formatNumber).join('-') + ' ' + [hour, minute, second].map(formatNumber).join(':')
}

const formatNumber = n => {
  n = n.toString()
  return n[1] ? n : '0' + n
}

// await util.delayMs(1000)
const delayMs = (ms) => {
  return new Promise(resolve => {
    setTimeout(resolve, ms);
  });
}

// ArrayBuffer转16进制字符串
// ArrayBuffer => hex string  
function ab2strHex(ab) {
  let hexArr = Array.prototype.map.call(
    new Uint8Array(ab),
    function(bit) {
      return ('00' + bit.toString(16)).slice(-2)
    }
  )
  return hexArr.join('');
}

// string => ArrayBuffer Uint8
function str2abUint8(str) {
  var buf = new ArrayBuffer(str.length); // 2 bytes for each char
  var bufView = new Uint8Array(buf);
  for (var i = 0, strLen = str.length; i < strLen; i++) {
    bufView[i] = str.charCodeAt(i);
  }
  return buf;
}

// string => ArrayBuffer Uint16
function str2abUint16(str) {
  var buf = new ArrayBuffer(str.length * 2); // 2 bytes for each char
  var bufView = new Uint16Array(buf);
  for (var i = 0, strLen = str.length; i < strLen; i++) {
    bufView[i] = str.charCodeAt(i);
  }
  return buf;
}

function ArrayBuffer2str(buf) {
  return String.fromCharCode.apply(null, new Uint8Array(buf));
}

function Uint8Array2str(arr) {
  return String.fromCharCode.apply(null, arr);
}

// let retryPromise = util.autoRetry(promise, 1);
// refer to: https://blog.csdn.net/github_38589282/article/details/77414358
function autoRetry(func, retryMax) {
  let retryNum = retryMax;
  return async function funcR() {
    let params = arguments
    while (retryNum--) {
      try {
        await func(...params)
      } catch (e) {
        if (retryNum > 0) {
          console.error(`retry ${retryMax - retryNum} time. error:`, e);
          continue
        } else {
          throw e
        }
      }
    }
  };
}

module.exports = {
  // autoRetry: autoRetry,
  formatTime: formatTime,
  delayMs: delayMs,
  ab2strHex: ab2strHex,
  str2abUint8: str2abUint8,
  str2abUint16: str2abUint16,
  ArrayBuffer2str: ArrayBuffer2str,
  Uint8Array2str: Uint8Array2str,
}