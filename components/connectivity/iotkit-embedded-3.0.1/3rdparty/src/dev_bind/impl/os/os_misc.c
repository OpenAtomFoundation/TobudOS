/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include "dev_bind_internal.h"

/****** Convert values between host and big-/little-endian byte order ******/

/* reverse byte order */
static uint16_t reverse_16bit(uint16_t data)
{
    return (data >> 8) | (data << 8);
}

/* host byte order to big endian */
uint16_t os_htobe16(uint16_t data)
{
    if (os_is_big_endian()) {
        return data;
    }

    return reverse_16bit(data);
}

/* host byte order to little endian */
uint16_t os_htole16(uint16_t data)
{
    if (os_is_big_endian()) {
        return reverse_16bit(data);
    }

    return data;
}

/* big endian to host byte order */
uint16_t os_be16toh(uint16_t data)
{
    return os_htobe16(data);
}

/* little endian to host byte order */
uint16_t os_le16toh(uint16_t data)
{
    return os_htole16(data);
}

/* get unaligned data in big endian. */
uint16_t os_get_unaligned_be16(uint8_t * ptr)
{
    uint16_t res;

    memcpy(&res, ptr, sizeof(uint16_t));

    return os_be16toh(res);
}

/* get unaligned data in little endian. */
uint16_t os_get_unaligned_le16(uint8_t * ptr)
{
    uint16_t res;

    memcpy(&res, ptr, sizeof(uint16_t));

    return os_le16toh(res);

}

/* format mac string uppercase */
char *os_wifi_get_mac_str(char mac_str[OS_MAC_LEN])
{
    char *str;
    int colon_num = 0, i;

    str = HAL_Wifi_Get_Mac(mac_str);

    /* sanity check */
    while (str) {
        str = strchr(str, ':');
        if (str) {
            colon_num ++;
            str ++; /* eating char ':' */
        }
    }

    /* convert to capital letter */
    for (i = 0; i < OS_MAC_LEN && mac_str[i]; i ++) {
        if ('a' <= mac_str[i] && mac_str[i] <= 'z') {
            mac_str[i] -= 'a' - 'A';
        }
    }

    return mac_str;
}
char *os_wifi_str2mac(char mac_str[OS_MAC_LEN], char mac[OS_ETH_ALEN])
{
    int i = 0;
    char *ptr = mac_str;
    char mac_addr[OS_ETH_ALEN] = {0};

    if (ptr == NULL)
        return NULL;

    while (isxdigit(*ptr) && i < OS_ETH_ALEN) {
        mac_addr[i ++] = (uint8_t)strtol(ptr, &ptr, 16);
        ++ ptr;
    }

    if (i < OS_ETH_ALEN)  /* don't touch mac when fail */
        return NULL;

    if (mac) memcpy(mac, mac_addr, OS_ETH_ALEN);

    return mac;
}

uint8_t *os_wifi_get_mac(uint8_t mac[OS_ETH_ALEN])
{
    char mac_str[OS_MAC_LEN] = {0};

    os_wifi_get_mac_str(mac_str);

    return (uint8_t *)os_wifi_str2mac(mac_str, (char *)mac);
}

void *os_zalloc(uint32_t size)
{
    void *ptr = HAL_Malloc(size);
    if (ptr != NULL) {
        memset(ptr, 0, size);
    }
    return ptr;
}

uint32_t os_get_time_ms(void)
{
    static uint32_t fixed_delta;

    if (!fixed_delta) {
        fixed_delta = (uint32_t)HAL_UptimeMs() - 0xFFFF0000;
    }

    /* add a big offset, for easier caught time overflow bug */
    return (uint32_t)HAL_UptimeMs() - fixed_delta;
}

uint32_t time_elapsed_ms_since(uint32_t start_timestamp)
{
    uint32_t now = os_get_time_ms();
    return now - start_timestamp;
}

int os_is_big_endian(void)
{
    uint32_t data = 0xFF000000;

    if (0xFF == *(uint8_t *) & data) {
        return 1;                                    /* big endian */
    }

    return 0;                                         /* little endian */
}

