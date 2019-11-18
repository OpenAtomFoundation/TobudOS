#ifndef _INFRA_STRING_H_
#define _INFRA_STRING_H_

#include "infra_types.h"

int8_t infra_hex2char(uint8_t hex);
void infra_hex2str(uint8_t *input, uint16_t input_len, char *output);
void infra_int2str(uint32_t input, char output[10]);
char *infra_strtok(char *str, const char *delim);
int infra_randstr(char *random, int length);
void LITE_hexstr_convert(char *input, int input_len, unsigned char *output, int output_len);
int infra_str2int(const char *input, int *val);
void LITE_hexbuf_convert(unsigned char *digest, char *out, int in_len, int uppercase);

#endif

