#ifndef _ALCS_BASE64_H_
#define _ALCS_BASE64_H_

#include "infra_types.h"

int utils_base64encode(const uint8_t *data, uint32_t inputLength, uint32_t outputLenMax,
                              uint8_t *encodedData, uint32_t *outputLength);
int utils_base64decode(const uint8_t *data, uint32_t inputLength, uint32_t outputLenMax,
                              uint8_t *decodedData, uint32_t *outputLength);
#endif