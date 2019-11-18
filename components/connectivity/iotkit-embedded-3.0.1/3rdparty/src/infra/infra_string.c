#include "infra_config.h"

#ifdef INFRA_STRING

#include <stdio.h>
#include <string.h>
#include "infra_types.h"
#include "infra_string.h"

int8_t infra_hex2char(uint8_t hex)
{
    hex = hex & 0xF;
    return (int8_t)(hex < 10 ? '0' + hex : hex - 10 + 'a');
}

void infra_hex2str(uint8_t *input, uint16_t input_len, char *output)
{
    char *zEncode = "0123456789ABCDEF";
    int i = 0, j = 0;

    for (i = 0; i < input_len; i++) {
        output[j++] = zEncode[(input[i] >> 4) & 0xf];
        output[j++] = zEncode[(input[i]) & 0xf];
    }
}

void infra_int2str(uint32_t input, char output[10])
{
    uint8_t i = 0, j = 0;
    char tmp[10] = {0};

    do {
        tmp[i++] = input%10 + '0';
    }while((input/=10)>0);

    do {
        output[--i] = tmp[j++];
    }while(i > 0);
}

char *infra_strtok(char *str, const char *delim)
{
    int only_delim = 1;
    static char *pos = NULL;
    static char *target = NULL;

    pos = (str == NULL)?(pos):(str);

    if (pos == NULL || delim == NULL ||
        strlen(pos) <= strlen(delim)) {
        return NULL;
    }

    target = pos;
    while (strlen(pos) >= strlen(delim)) {
        if (memcmp(pos,delim,strlen(delim)) != 0) {
            only_delim = 0;
            pos++;
            continue;
        }

        if (strlen(pos) == strlen(delim)) {
            memset(pos,0,strlen(delim));
            if (only_delim) {
                return NULL;
            }
            return target;
        }

        if (target == pos) {
            pos += strlen(delim);
            target = pos;
        }else{
            memset(pos,0,strlen(delim));
            pos += strlen(delim);
            break;
        }
    }

    return target;
}

#define LITE_isdigit(c)             (((c) <= '9' && (c) >= '0') ? (1) : (0))

static uint8_t _hexval_of_char(char hex)
{
    if (LITE_isdigit(hex)) {
        return (hex - '0');
    }
    if (hex >= 'a' && hex <= 'f') {
        return (hex - 'a' + 10);
    }
    if (hex >= 'A' && hex <= 'F') {
        return (hex - 'A' + 10);
    }

    return 0;
}

void LITE_hexstr_convert(char *input, int input_len, unsigned char *output, int output_len)
{
    int             i = 0;
    uint8_t         ch0, ch1;

    if (input_len % 2 != 0) {
        return;
    }

    while (i < input_len / 2 && i < output_len) {
        ch0 = _hexval_of_char((char)input[2 * i]);
        ch1 = _hexval_of_char((char)input[2 * i + 1]);
        output[i] = (ch0 << 4 | ch1);
        i++;
    }
}

void LITE_hexbuf_convert(unsigned char *digest, char *out, int in_len, int uppercase)
{
    static char    *zEncode[] = {"0123456789abcdef", "0123456789ABCDEF"};
    int             j = 0;
    int             i = 0;
    int             idx = uppercase ? 1 : 0;

    for (i = 0; i < in_len; i ++) {
        int         a = digest[i];

        out[j++] = zEncode[idx][(a >> 4) & 0xf];
        out[j++] = zEncode[idx][a & 0xf];
    }
}

int infra_str2int(const char *input, int *val)
{
    int sign = 0;
    int temp = 0;

    if (input == NULL || val == NULL) {
        return -1;
    }

    while(*input == ' ') {  /* only support skipping space */
        input++;
    }

    if (*input == '+') {
        input++;
    }
    else if (*input == '-') {
        input++;
        sign = -1;
    }

    while (*input != 0) {
        if (*input < '0' || *input > '9') {
            break;
        }

        temp = temp * 10 + (*input - '0');
        input++;
    }

    if (sign == -1) {
        temp = -temp;
    }

    *val = temp;
    return 0;
}

#endif

#ifdef INFRA_RANDOM

uint64_t HAL_UptimeMs(void);
void HAL_Srandom(uint32_t seed);
uint32_t HAL_Random(uint32_t region);

int infra_randstr(char *random, int length)
{
    int index = 0;

    HAL_Srandom(HAL_UptimeMs());

    for (index = 0; index < length; index++) {
        switch (HAL_Random(3)) {
            case 0: {
                random[index] = 'A' + HAL_Random(26);
            }
            break;
            case 1: {
                random[index]  = 'a' + HAL_Random(26);
            }
            break;
            case 2: {
                random[index] = '0' + HAL_Random(10);
            }
            break;
            default: {
                return -1;
            }
        }
    }

    return 0;
}
#endif

