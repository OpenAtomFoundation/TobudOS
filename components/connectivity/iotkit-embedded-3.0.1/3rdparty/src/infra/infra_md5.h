/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */




#ifndef _INFRA_MD5_H_
#define _INFRA_MD5_H_

#include "infra_types.h"

typedef struct {
    uint32_t total[2];          /*!< number of bytes processed  */
    uint32_t state[4];          /*!< intermediate digest state  */
    unsigned char buffer[64];   /*!< data block being processed */
} iot_md5_context;

/**
 * \brief          Initialize MD5 context
 *
 * \param ctx      MD5 context to be initialized
 */
void utils_md5_init(iot_md5_context *ctx);

/**
 * \brief          Clear MD5 context
 *
 * \param ctx      MD5 context to be cleared
 */
void utils_md5_free(iot_md5_context *ctx);

/**
 * \brief          Clone (the state of) an MD5 context
 *
 * \param dst      The destination context
 * \param src      The context to be cloned
 */
void utils_md5_clone(iot_md5_context *dst,
                     const iot_md5_context *src);

/**
 * \brief          MD5 context setup
 *
 * \param ctx      context to be initialized
 */
void utils_md5_starts(iot_md5_context *ctx);

/**
 * \brief          MD5 process buffer
 *
 * \param ctx      MD5 context
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 */
void utils_md5_update(iot_md5_context *ctx, const unsigned char *input, uint32_t ilen);

/**
 * \brief          MD5 final digest
 *
 * \param ctx      MD5 context
 * \param output   MD5 checksum result
 */
void utils_md5_finish(iot_md5_context *ctx, unsigned char output[16]);

/* Internal use */
void utils_md5_process(iot_md5_context *ctx, const unsigned char data[64]);

/**
 * \brief          Output = MD5( input buffer )
 *
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 * \param output   MD5 checksum result
 */
void utils_md5(const unsigned char *input, uint32_t ilen, unsigned char output[16]);

void utils_hmac_md5(const char *msg, int msg_len, char *digest, const char *key, int key_len);

#endif

