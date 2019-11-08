/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <ctype.h>

unsigned int platform_aton(const char *ip_str)
{
    char c;
    unsigned char base;
    unsigned int  val = 0;
    unsigned int parts[4] = {0};
    unsigned int *pp = parts;

    c = *ip_str;
    for (;;) {
        /*
         * Collect number up to ``.''.
         * Values are specified as for C:
         * 0x=hex, 0=octal, 1-9=decimal.
         */
        if (!isdigit(c))
            return (0);

        val = 0;
        base = 10;
        if (c == '0') {
            c = *++ip_str;
            if (c == 'x' || c == 'X') {
                base = 16;
                c = *++ip_str;
            } else {
                base = 8;
            }
        }
        for (;;) {
            if (isdigit(c)) {
                val = (val * base) + (int)(c - '0');
                c = *++ip_str;
            } else if (base == 16 && isxdigit(c)) {
                val = (val << 4) | (int)(c + 10 - (islower(c) ? 'a' : 'A'));
                c = *++ip_str;
            } else {
                break;
            }
        }
        if (c == '.') {
            /*
             * Internet format:
             *  a.b.c.d
             *  a.b.c   (with c treated as 16 bits)
             *  a.b (with b treated as 24 bits)
             */
            if (pp >= parts + 3)
                return (0);
            *pp++ = val;
            c = *++ip_str;
        } else {
            break;
        }
    }
    /*
     * Check for trailing characters.
     */
    if (c != '\0' && !isspace(c))
        return (0);
    /*
     * Concoct the address according to
     * the number of parts specified.
     */
    switch (pp - parts + 1) {
        case 0:
            return (0);                                    /* initial nondigit */
        case 1:                                            /* a -- 32 bits */
            break;
        case 2:                                            /* a.b -- 8.24 bits */
            if (val > 0xffffffUL)
                return (0);
            val |= parts[0] << 24;
            break;
        case 3:                                            /* a.b.c -- 8.8.16 bits */
            if (val > 0xffff)
                return (0);
            val |= (parts[0] << 24) | (parts[1] << 16);
            break;
        case 4:                                            /* a.b.c.d -- 8.8.8.8 bits */
            if (val > 0xff)
                return (0);
            val |= (parts[0] << 24) | (parts[1] << 16) | (parts[2] << 8);
            break;
        default:
            break;
    }

    return val;
}


int platform_is_multicast(const char *ip_str)
{
    unsigned int addr_in;
    addr_in = platform_aton(ip_str);
    return (addr_in > 0xE00000FF && addr_in <= 0xEFFFFFFF);
}
