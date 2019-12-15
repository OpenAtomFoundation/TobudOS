/**
 * @file aes.h
 * @brief AES functions
 * @copyright (c) 2003-2006, Jouni Malinen <jkmaline@cc.hut.fi>
 *
 * @note This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Alternatively, this software may be distributed under the terms of BSD
 * license.
 *
 * See README and COPYING for more details.
 */

#ifndef AES_H
#define AES_H
//#include "crypto/cryptoApi.h"
#include <stddef.h>
#include "wm_type_def.h"
#define  USE_AES
#define PSPUBLIC
#ifndef CRYPT_INVALID_KEYSIZE
#define	CRYPT_INVALID_KEYSIZE	-21 
#endif
#ifndef MAXBLOCKSIZE
#define MAXBLOCKSIZE	24
#endif

#define STORE32H(x, y) { \
(y)[0] = (unsigned char)(((x)>>24)&255); \
(y)[1] = (unsigned char)(((x)>>16)&255); \
(y)[2] = (unsigned char)(((x)>>8)&255); \
(y)[3] = (unsigned char)((x)&255); \
}

#define LOAD32H(x, y) { \
x = ((unsigned long)((y)[0] & 255)<<24) | \
((unsigned long)((y)[1] & 255)<<16) | \
((unsigned long)((y)[2] & 255)<<8)  | \
((unsigned long)((y)[3] & 255)); \
}

#define byte(x, n) (((x) >> (8 * (n))) & 255)

#define ROR(x, y) \
	( ((((unsigned long)(x)&0xFFFFFFFFUL)>>(unsigned long)((y)&31)) | \
	((unsigned long)(x)<<(unsigned long)(32-((y)&31)))) & 0xFFFFFFFFUL)

#define psTraceCrypto(x) 

/******************************************************************************/
/*
	Universal return codes
*/
#define PS_SUCCESS			0
#define PS_FAILURE			-1	

/*	NOTE: Failure return codes MUST be < 0 */
/*	NOTE: The range for core error codes should be between -2 and -29 */
#define PS_ARG_FAIL			-6	/* Failure due to bad function param */
#define PS_PLATFORM_FAIL	-7	/* Failure as a result of system call error */
#define PS_MEM_FAIL			-8	/* Failure to allocate requested memory */	
#define PS_LIMIT_FAIL		-9	/* Failure on sanity/limit tests */	
#define PS_UNSUPPORTED_FAIL	-10 /* Unimplemented feature error */	
#define PS_DISABLED_FEATURE_FAIL -11 /* Incorrect #define toggle for feature */
#define PS_PROTOCOL_FAIL	-12 /* A protocol error occurred */
#define PS_TIMEOUT_FAIL		-13 /* A timeout occurred and MAY be an error */
#define PS_INTERRUPT_FAIL	-14 /* An interrupt occurred and MAY be an error */
#define PS_PENDING			-15 /* In process. Not necessarily an error */ 
#define PS_EAGAIN			-16 /* Try again later. Not necessarily an error */ 

#define	PS_TRUE		1
#define	PS_FALSE 	0	

//typedef int  int32;	//调用wm_type_def.h
//typedef unsigned int  uint32;
//typedef short  int16;
//typedef unsigned short  uint16;
//typedef unsigned char uint8;

//typedef long long int64;
//typedef unsigned long long uint64;
//#if (GCC_COMPILE==0)
//typedef unsigned int size_t;
//#endif


/******************************************************************************/
#ifdef USE_AES
/******************************************************************************/


#ifndef USE_AES_CBC_EXTERNAL
typedef struct {
	unsigned int eK[64], dK[64];
	int Nr;
} psAesKey_t;

typedef struct {
	int			blocklen;
	unsigned char	IV[16];
	psAesKey_t		key;
#if defined(USE_AES_GCM) || defined(USE_AES_CCM)
	unsigned char	EncCtr[16];
	unsigned char	CtrBlock[16];
#endif
#ifdef USE_AES_GCM
	unsigned char	gInit[16];
	unsigned int			TagTemp[4];
	unsigned char	Hash_SubKey[16];
	unsigned int			ProcessedBitCount[4];
	unsigned int			InputBufferCount;
	unsigned int			OutputBufferCount;
    union
    {
        unsigned char Buffer[128];
        unsigned int BufferAlignment;
    } Input;
#endif /* USE_AES_GCM */
#ifdef USE_AES_CCM
	unsigned int ccmTagTemp[16 / sizeof(unsigned int)]; /* 32 */
	union
	{
		/* Used for formatting IV. */
		unsigned char Temporary[16];
		/* Used for processing Mac. */
		unsigned char Y0[16];
	} u; /* 48 */
#endif /* USE_AES_CCM */
} psAesCipher_t;

typedef union {
#ifdef USE_AES
	psAesCipher_t	aes;
#endif
} psAesCipherContext_t;

#endif /* USE_AES_CBC_EXTERNAL */

#endif /* USE_AES */

#if 0
void * aes_encrypt_init(const u8 *key, size_t len);
void aes_encrypt(void *ctx, const u8 *plain, u8 *crypt);
void aes_encrypt_deinit(void *ctx);
void * aes_decrypt_init(const u8 *key, size_t len);
void aes_decrypt(void *ctx, const u8 *crypt, u8 *plain);
void aes_decrypt_deinit(void *ctx);
#endif

#ifndef USE_AES
#error "Must Enable USE_AES in cryptoConfig.h!"
#endif

#ifdef USE_AES
/******************************************************************************/
/*
	CBC Mode AES
*/
PSPUBLIC int psAesInit(psAesCipherContext_t *ctx, unsigned char *IV,
						unsigned char *key, unsigned int keylen);
PSPUBLIC int psAesDecrypt(psAesCipherContext_t *ctx, unsigned char *ct,
						unsigned char *pt, unsigned int len);
PSPUBLIC int psAesEncrypt(psAesCipherContext_t *ctx, unsigned char *pt,
						unsigned char *ct, unsigned int len);
/*
	Block Mode AES
*/
PSPUBLIC int psAesInitKey(const unsigned char *key, unsigned int keylen,
						psAesKey_t *skey);
PSPUBLIC void psAesEncryptBlock(const unsigned char *pt, unsigned char *ct,
						psAesKey_t *skey);
PSPUBLIC void psAesDecryptBlock(const unsigned char *ct, unsigned char *pt,
						psAesKey_t *skey);



int  aes_128_cbc_encrypt(const u8 *key, const u8 *iv, u8 *data,
				     size_t data_len);
int  aes_128_cbc_decrypt(const u8 *key, const u8 *iv, u8 *data,
				     size_t data_len);
#endif

#endif /* AES_H */

