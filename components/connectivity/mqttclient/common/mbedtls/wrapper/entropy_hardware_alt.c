/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2020-01-11 02:04:49
 * @LastEditTime : 2020-01-11 02:12:16
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#include "mbedtls/entropy.h"
#include "random.h"

#if defined(MBEDTLS_ENTROPY_HARDWARE_ALT)

static int mbedtls_get_random(unsigned char *buf, size_t len)
{
    int i, j;
    unsigned long tmp;

    for (i = 0; i < ((len + 3) & ~3) / 4; i++) {
        tmp = random_number();

        for (j = 0; j < 4; j++) {
            if ((i * 4 + j) < len) {
                buf[i * 4 + j] = (unsigned char)(tmp >> (j * 8));
            } else {
                break;
            }
        }
    }

    return 0;
}

int mbedtls_hardware_poll(void *data, unsigned char *output, size_t len, size_t *olen)
{
    mbedtls_get_random(output, len);
    *olen = len;

    return 0;
}

#endif
