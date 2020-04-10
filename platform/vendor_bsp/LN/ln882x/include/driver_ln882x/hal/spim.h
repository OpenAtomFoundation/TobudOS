#ifndef __SPIM_H__
#define __SPIM_H__

#ifdef __cplusplus
    extern "C" {
#endif // __cplusplus

#include "types.h"

void spim_init(uint32_t baud);
// FIXME: uint8_t ? <at least be uint16_t>
int spim_write(uint8_t addr, uint16_t data);
int spim_read(uint8_t addr, uint16_t *data);


#ifdef __cplusplus
}
#endif // __cplusplus
#endif // __SPIM_H__
