#ifndef __CRC16_H__
#define __CRC16_H__

#include <stdint.h>

uint16_t crc16_ccitt(const char *buf, int len);

#endif // __CRC16_H__
