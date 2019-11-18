
/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <stdlib.h>
#include "infra_types.h"
#include "infra_defs.h"
#include "alcs_base64.h"

static int8_t g_encodingTable[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                                   'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                   'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                                   'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                                   'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                   'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                   'w', 'x', 'y', 'z', '0', '1', '2', '3',
                                   '4', '5', '6', '7', '8', '9', '+', '/'
                                  };

static int8_t g_decodingTable[256];
static int32_t g_modTable[] = { 0, 2, 1 };

static void build_decoding_table()
{
    static int32_t signal = 0;
    int32_t i = 0;

    if (signal != 0) {
        return;
    }

    for (i = 0; i < 64; i++) {
        g_decodingTable[(uint8_t) g_encodingTable[i]] = i;
    }

    signal = 1;
    return;
}

int utils_base64encode(const uint8_t *data, uint32_t inputLength, uint32_t outputLenMax,
                       uint8_t *encodedData, uint32_t *outputLength)
{
    uint32_t i = 0;
    uint32_t j = 0;

    if (NULL == encodedData) {
        return FAIL_RETURN;
    }

    *outputLength = 4 * ((inputLength + 2) / 3);

    if (outputLenMax < *outputLength) {
        return FAIL_RETURN;
    }

    for (i = 0, j = 0; i < inputLength;) {
        uint32_t octet_a = i < inputLength ? (uint8_t) data[i++] : 0;
        uint32_t octet_b = i < inputLength ? (uint8_t) data[i++] : 0;
        uint32_t octet_c = i < inputLength ? (uint8_t) data[i++] : 0;

        uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

        encodedData[j++] = g_encodingTable[(triple >> 3 * 6) & 0x3F];
        encodedData[j++] = g_encodingTable[(triple >> 2 * 6) & 0x3F];
        encodedData[j++] = g_encodingTable[(triple >> 1 * 6) & 0x3F];
        encodedData[j++] = g_encodingTable[(triple >> 0 * 6) & 0x3F];
    }

    for (i = 0; i < g_modTable[inputLength % 3]; i++) {
        encodedData[*outputLength - 1 - i] = '=';
    }

    return SUCCESS_RETURN;
}

int utils_base64decode(const uint8_t *data, uint32_t inputLength, uint32_t outputLenMax,
                       uint8_t *decodedData, uint32_t *outputLength)
{
    uint32_t i = 0;
    uint32_t j = 0;
    uint32_t sextet_a = 0;
    uint32_t sextet_b = 0;
    uint32_t sextet_c = 0;
    uint32_t sextet_d = 0;
    uint32_t triple = 0;

    build_decoding_table();

    if (inputLength % 4 != 0) {
        return FAIL_RETURN;
    }

    *outputLength = inputLength / 4 * 3;


    if (data[inputLength - 1] == '=') {
        (*outputLength)--;
    }

    if (data[inputLength - 2] == '=') {
        (*outputLength)--;
    }

    if (outputLenMax < *outputLength) {
        return FAIL_RETURN;
    }

    for (i = 0, j = 0; i < inputLength;) {
        sextet_a = data[i] == '=' ? 0 & i++ : g_decodingTable[data[i++]];
        sextet_b = data[i] == '=' ? 0 & i++ : g_decodingTable[data[i++]];
        sextet_c = data[i] == '=' ? 0 & i++ : g_decodingTable[data[i++]];
        sextet_d = data[i] == '=' ? 0 & i++ : g_decodingTable[data[i++]];

        triple = (sextet_a << 3 * 6) + (sextet_b << 2 * 6) + (sextet_c << 1 * 6) + (sextet_d << 0 * 6);

        if (j < *outputLength) {
            decodedData[j++] = (triple >> 2 * 8) & 0xFF;
        }

        if (j < *outputLength) {
            decodedData[j++] = (triple >> 1 * 8) & 0xFF;
        }

        if (j < *outputLength) {
            decodedData[j++] = (triple >> 0 * 8) & 0xFF;
        }
    }

    return SUCCESS_RETURN;
}