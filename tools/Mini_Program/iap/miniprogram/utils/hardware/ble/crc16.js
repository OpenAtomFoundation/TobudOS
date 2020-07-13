"use strict";

function UpdateCRC16(crcIn, byte) {
  let crc = crcIn;
  let inNum = byte | 0x100;
  do {
    crc <<= 1;
    crc = crc & 0xFFFFFFFF;
    inNum <<= 1;
    inNum = inNum & 0xFFFFFFFF;

    if (inNum & 0x100) {
      ++crc;
    }

    if (crc & 0x10000) {
      crc ^= 0x1021;
      crc = crc & 0xFFFFFFFF;
    }

  } while (!(inNum & 0x10000));

  return crc & 0xffff;
}

function crc16(data, size) {
  let crc = 0;

  for (let i = 0; i < size; i++) {
    crc = UpdateCRC16(crc, data[i])
  }
  crc = UpdateCRC16(crc, 0);
  crc = UpdateCRC16(crc, 0);

  return crc & 0xffff;
}


function crc16_backup(buf, len) {
  // let crc = 0x0000;
  let crc = 0xFFFF;

  for (let pos = 0; pos < len; pos++) {
    // XOR byte into least sig. byte of crc
    crc ^= buf[pos];
    crc = crc & 0xFFFF;
    for (let i = 8; i != 0; i--) { // Loop over each bit
      if ((crc & 0x0001) != 0) { // If the LSB is set
        crc >>= 1; // Shift right and XOR 0xA001
        crc ^= 0xA001;
        crc = crc & 0xFFFF;
      }
      else {// Else LSB is not set
        crc >>= 1; // Just shift right
        crc = crc & 0xFFFF;
      }
    }
  }
  return crc;
}
/*
int calcrc(char *ptr, int count)
{
    int  crc;
    char i;
    crc = 0;
    while (--count >= 0)
    {
        crc = crc ^ (int) *ptr++ << 8;
        i = 8;
        do
        {
            if (crc & 0x8000)
                crc = crc << 1 ^ 0x1021;
            else
                crc = crc << 1;
        } while(--i);
    }
    return (crc);
}
*/

module.exports = crc16;