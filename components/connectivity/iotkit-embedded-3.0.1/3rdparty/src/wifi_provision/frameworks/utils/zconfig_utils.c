/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include "wifi_provision_internal.h"

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

void dump_hex(uint8_t *data, int len, int tab_num)
{
    int i;
    for (i = 0; i < len; i++) {
        HAL_Printf("%02x ", data[i]);

        if (!((i + 1) % tab_num))
            HAL_Printf("\r\n");
    }

    HAL_Printf("\r\n");
}
#if 0
void dump_ascii(uint8_t *data, int len, int tab_num)
{
    int i;
    for (i = 0; i < len; i++) {
        HAL_Printf("%-2c ", data[i]);

        if (!((i + 1) % tab_num))
            HAL_Printf("  ");
    }

    HAL_Printf("\r\n");
}

void dump_mac(uint8_t *src, uint8_t *dst)
{
    uint8_t *mac;

    mac = src;
    HAL_Printf("%02x:%02x:%02x:%02x:%02x:%02x > ",
            mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    mac = dst;
    HAL_Printf("%02x:%02x:%02x:%02x:%02x:%02x\r\n",
            mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    /* elimite compiler warning */
    mac = mac;
}
#endif
/* for smartconfig with encrypt */
uint16_t zconfig_checksum_v3(uint8_t *data, uint8_t len)
{
    uint8_t i;
    uint16_t sum = 0, res;

    for (i = 0; i < len; i++)
        sum += data[i];

    res = sum & (0x3F << 0);
    res |= (sum & (0x3F << 6)) << 2;

    if (!(res & 0x00FF))
        res |= 0x0001;
    if (!(res & 0xFF00))
        res |= 0x0100;

    return res;
}

char is_utf8(const char *ansi_str, int length)
{
    int i = 0;
    char utf8 = 1;
    while (i < length) {
        if ((0x80 & ansi_str[i]) == 0) { /* ASCII */
            i++;
            continue;
        } else if ((0xE0 & ansi_str[i]) == 0xC0) { /* 110xxxxx */
            if (ansi_str[i + 1] == '\0') {
                utf8 = 0;
                break;
            }
            if ((0xC0 & ansi_str[i + 1]) == 0x80) { /* 10xxxxxx */
                i += 2;
                continue;
            } else {
                utf8 = 0;
                break;
            }
        } else if ((0xF0 & ansi_str[i]) == 0xE0) { /* 1110xxxx */
            if (ansi_str[i + 1] == '\0') {
                utf8 = 0;
                break;
            }
            if (ansi_str[i + 2] == '\0') {
                utf8 = 0;
                break;
            }
            if (((0xC0 & ansi_str[i + 1]) == 0x80) && ((0xC0 & ansi_str[i + 2]) == 0x80)) { /* 10xxxxxx 10xxxxxx */
                i += 3;
                continue;
            } else {
                utf8 = 0;
                break;
            }
        } else {
            utf8 = 0;
            break;
        }
    }
    return utf8;
}

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif
