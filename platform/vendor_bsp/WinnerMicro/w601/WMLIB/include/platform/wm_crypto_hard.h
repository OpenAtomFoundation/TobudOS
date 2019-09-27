/**
 * @file    wm_crypto_hard.h
 *
 * @brief   hard crypto module
 *
 * @author  dave
 *
 * Copyright (c) 2014 Winner Microelectronics Co., Ltd.
 */

#ifndef WM_CRYPTO_HARD_H
#define WM_CRYPTO_HARD_H

#include "wm_type_def.h"
#include "cryptoConfig.h"
#include "digest.h"
#include "pstm.h"
#include "symmetric.h"
#include "cryptolib.h"

#include "cryptoApi.h"

#ifndef min
	#define min(a,b)    (((a) < (b)) ? (a) : (b))
#endif /* min */

/** The base address of the registers of encryption/decryption module. */ 
#define HR_CRYPTO_BASE_ADDR   0x40003200
/** The address of the source text address register. */
#define HR_CRYPTO_SRC_ADDR     (HR_CRYPTO_BASE_ADDR + 0x00) 
/** The address of the encrypted/decrypted text address register. */
#define HR_CRYPTO_DEST_ADDR     (HR_CRYPTO_BASE_ADDR + 0x04) 
/** The address of the configuration register. */
#define HR_CRYPTO_SEC_CFG     (HR_CRYPTO_BASE_ADDR + 0x08) 
/** The address of the control register. */
#define HR_CRYPTO_SEC_CTRL     (HR_CRYPTO_BASE_ADDR + 0x0C) 
/** The address of the key0 register. */
#define HR_CRYPTO_KEY0     (HR_CRYPTO_BASE_ADDR + 0x10) 
/** The address of the key1 register. */
#define HR_CRYPTO_KEY1     (HR_CRYPTO_BASE_ADDR + 0x14) 
/** The address of the key2 register. */
#define HR_CRYPTO_KEY2     (HR_CRYPTO_BASE_ADDR + 0x18) 
/** The address of the key3 register. */
#define HR_CRYPTO_KEY3     (HR_CRYPTO_BASE_ADDR + 0x1C) 
/** The address of the key4 register. */
#define HR_CRYPTO_KEY4     (HR_CRYPTO_BASE_ADDR + 0x20) 
/** The address of the key5 register. */
#define HR_CRYPTO_KEY5     (HR_CRYPTO_BASE_ADDR + 0x24) 

/** The address of the IV0 register. */
#define HR_CRYPTO_IV0       (HR_CRYPTO_BASE_ADDR + 0x28) 
/** The address of the IV1 register. */
#define HR_CRYPTO_IV1       (HR_CRYPTO_BASE_ADDR + 0x20) 

/** The address of the encrypted/decrypted status register. */
#define HR_CRYPTO_SEC_STS     (HR_CRYPTO_BASE_ADDR + 0x30) 

/** The address of the SHA1/MD5 digest0 register. */
#define HR_CRYPTO_SHA1_DIGEST0     (HR_CRYPTO_BASE_ADDR + 0x34) 
/** The address of the SHA1/MD5 digest1 register. */
#define HR_CRYPTO_SHA1_DIGEST1     (HR_CRYPTO_BASE_ADDR + 0x38) 
/** The address of the SHA1/MD5 digest2 register. */
#define HR_CRYPTO_SHA1_DIGEST2     (HR_CRYPTO_BASE_ADDR + 0x3C) 
/** The address of the SHA1/MD5 digest3 register. */
#define HR_CRYPTO_SHA1_DIGEST3     (HR_CRYPTO_BASE_ADDR + 0x40) 
/** The address of the SHA1 digest4 and CRC in/output register. */
#define HR_CRYPTO_SHA1_DIGEST4     (HR_CRYPTO_BASE_ADDR + 0x44) 
/** The address of the RNG RESULT register. */
#define HR_CRYPTO_RNG_RESULT     (HR_CRYPTO_BASE_ADDR + 0x48) 


/** The address of the CRC in register. */
#define HR_CRYPTO_CRC_KEY   HR_CRYPTO_SHA1_DIGEST4
/** The address of the CRC output register. */
#define HR_CRYPTO_CRC_RESULT    HR_CRYPTO_SHA1_DIGEST4

#define ERR_CRY_OK          0    ///< No error, everything OK.
#define ERR_FAILURE        -1    /* failure */
#define ERR_ARG_FAIL			-6	/* Failure due to bad function param */

//CRC
#define OUTPUT_REFLECT 1
#define INPUT_REFLECT   2

typedef enum __CRYPTO_METHOD
{
	CRYPTO_METHOD_RSV=0,
	CRYPTO_METHOD_RC4,
	CRYPTO_METHOD_SHA1,
	CRYPTO_METHOD_AES,
	CRYPTO_METHOD_DES,
	CRYPTO_METHOD_3DES,
	CRYPTO_METHOD_CRC, ///< CRC
	CRYPTO_METHOD_MD5 ///< MD5
}CRYPTO_METHOD;

 /** 
 * The enum of the encryption/decryption way.
 */ 
typedef enum __CRYPTO_WAY
{
	CRYPTO_WAY_ENCRYPT=0, ///< Encryption
	CRYPTO_WAY_DECRYPT ///< Decryption
}CRYPTO_WAY;

typedef enum __CRYPTO_RNG_SWITCH
{
	CRYPTO_RNG_SWITCH_16 = 0,
	CRYPTO_RNG_SWITCH_32
}CRYPTO_RNG_SWITCH;

 /** 
 * The enum of the encryption/decryption mode, only for (aes des 3des).
 */
typedef enum __CRYPTO_MODE
{
	CRYPTO_MODE_ECB=0, ///< ECB
	CRYPTO_MODE_CBC,  ///< CBC
	CRYPTO_MODE_CTR ///< CTR, only for AES
}CRYPTO_MODE;
 /** 
 * The enum of the CRC type.
 */
typedef enum __CRYPTO_CRC_TYPE
{
	CRYPTO_CRC_TYPE_8=0, ///< 8 bits CRC
	CRYPTO_CRC_TYPE_16_MODBUS, ///< 16 bits Modbus CRC
	CRYPTO_CRC_TYPE_16_CCITT, ///< 16 bits CCITT CRC
	CRYPTO_CRC_TYPE_32 ///< 32 bits CRC
}CRYPTO_CRC_TYPE;

 /** 
 * The struct of the CRC context.
 */
typedef struct {
	u32 state; ///<  The initial value input and result value output.
	u8 type; ///< The type of CRC, refernec the CRYPTO_CRC_TYPE enum.
	u8 mode; ///< The mode of CRC, bit0 means output reflection and bit1 means input reflection.
}psCrcContext_t;

#if 1

struct hsha1_state {
#ifdef HAVE_NATIVE_INT64
	uint64		length;
#else
	u32		lengthHi;
	u32		lengthLo;
#endif /* HAVE_NATIVE_INT64 */
	u32		state[5], curlen;
	unsigned char	buf[64];
};
struct hmd5_state {
#ifdef HAVE_NATIVE_INT64
    uint64 length;
#else
    u32 lengthHi;
    u32 lengthLo;
#endif /* HAVE_NATIVE_INT64 */
    u32 state[4], curlen;
    unsigned char buf[64];
};

typedef union {
	struct hsha1_state	sha1;
	struct hmd5_state	md5;
} hsDigestContext_t;

typedef u32			hstm_digit;
typedef struct  {
	int16	used, alloc, sign;
	hstm_digit *dp;
} hstm_int;

#endif

/**
 * @brief          	This function is used to stop random produce.
 *
 * @param[in]      	None
 *
 * @retval         	0     		success
 * @retval         	other 	failed
 *
 * @note           	None
 */
int tls_crypto_random_stop(void);


/**
 * @brief          	This function initializes random digit seed and BIT number.
 *
 * @param[in]   	seed 		The random digit seed.
 * @param[in]   	rng_switch 	The random digit bit number.   (0: 16bit    1:32bit)				
 *
 * @retval  		0  			success 
 * @retval  		other   		failed  
 *
 * @note             	None
 */
int tls_crypto_random_init(u32 seed, CRYPTO_RNG_SWITCH rng_switch);


/**
 * @brief          	This function is used to get random digit content.
 *
 * @param[in]   	out 			Pointer to the output of random digit.
 * @param[in]   	len 			The random digit bit number will output.
 *
 * @retval  		0  			success 
 * @retval  		other   		failed  
 *
 * @note             	None
 */
int tls_crypto_random_bytes(unsigned char *out, u32 len);


/**
 * @brief          	This function initializes a RC4 encryption algorithm,  
 *				i.e. fills the psCipherContext_t structure pointed to by ctx with necessary data. 
 *
 * @param[in]   	ctx 		Pointer to the Cipher Context. 
 * @param[in]   	key 		Pointer to the key. 
 * @param[in]   	keylen 	the length of key. 
 *
 * @retval  		0  		success 
 * @retval  		other   	failed  

 *
 * @note             	The first parameter ctx must be a structure which is allocated externally. 
 *      			And all of Context parameters in the initializing methods should be allocated externally too.
 */
int tls_crypto_rc4_init(psCipherContext_t * ctx, const unsigned char *key, u32 keylen);

/**
 * @brief          	This function encrypts a variable length data stream according to RC4.
 *				The RC4 algorithm it generates a "keystream" which is simply XORed with the plaintext to produce the ciphertext stream. 
 *				Decryption is exactly the same as encryption. This function also decrypts a variable length data stream according to RC4.
 *
 * @param[in]   	ctx 		Pointer to the Cipher Context.
 * @param[in]   	in 		Pointer to the input plaintext data stream(or the encrypted text data stream) of variable length.
 * @param[in]   	out 		Pointer to the resulting ciphertext data stream. 
 * @param[in]		len 		Length of the plaintext data stream in octets.
 *
 * @retval  		0  		success 
 * @retval  		other   	failed  
 *
 * @note             	None
 */
int tls_crypto_rc4(psCipherContext_t * ctx, unsigned char *in, unsigned char *out, u32 len);

/**
 * @brief          	This function initializes a AES encryption algorithm,  i.e. fills the psCipherContext_t structure pointed to by ctx with necessary data.
 *
 * @param[in]   	ctx 		Pointer to the Cipher Context. 
 * @param[in]   	IV 		Pointer to the Initialization Vector
 * @param[in]   	key 		Pointer to the key. 
 * @param[in]		keylen 	the length of key.
 * @param[in]   	cbc 		the encryption mode, AES supports ECB/CBC/CTR modes.
 *
 * @retval  		0  		success 
 * @retval  		other   	failed  
 *
 * @note             	None
 */
int tls_crypto_aes_init(psCipherContext_t * ctx, const unsigned char *IV, const unsigned char *key, u32 keylen, CRYPTO_MODE cbc);

/**
 * @brief			This function encrypts or decrypts a variable length data stream according to AES.
 *
 * @param[in]		ctx 		Pointer to the Cipher Context. 
 * @param[in]		in 		Pointer to the input plaintext data stream(or the encrypted text data stream) of variable length.
 * @param[in]		out 		Pointer to the resulting ciphertext data stream.
 * @param[in]		len 		Length of the plaintext data stream in octets.
 * @param[in]		dec 		The cryption way which indicates encryption or decryption.
 *
 * @retval		0  		success 
 * @retval		other	failed	
 *
 * @note			None
 */
int tls_crypto_aes_encrypt_decrypt(psCipherContext_t * ctx, unsigned char *in, unsigned char *out, u32 len, CRYPTO_WAY dec);

/**
 * @brief			This function initializes a 3DES encryption algorithm,  i.e. fills the psCipherContext_t structure pointed to by ctx with necessary data. 
 *
 * @param[in]		ctx 		Pointer to the Cipher Context. 
 * @param[in]		IV 		Pointer to the Initialization Vector
 * @param[in]		key 		Pointer to the key. 
 * @param[in]		keylen 	the length of key. 
 * @param[in]		cbc 		the encryption mode, 3DES supports ECB/CBC modes.
 *
 * @retval		0  		success 
 * @retval		other	failed	
 *
 * @note			None
 */
int tls_crypto_3des_init(psCipherContext_t * ctx, const unsigned char *IV, const unsigned char *key, u32 keylen, CRYPTO_MODE cbc);

/**
 * @brief			This function encrypts or decrypts a variable length data stream according to 3DES.
 *
 * @param[in]		ctx 		Pointer to the Cipher Context.
 * @param[in]		in 		Pointer to the input plaintext data stream(or the encrypted text data stream) of variable length.
 * @param[in]		out 		Pointer to the resulting ciphertext data stream.
 * @param[in]		len 		Length of the plaintext data stream in octets.
 * @param[in]		dec 		The cryption way which indicates encryption or decryption.
 *
 * @retval		0  		success 
 * @retval		other	failed	
 *
 * @note			None
 */
int tls_crypto_3des_encrypt_decrypt(psCipherContext_t * ctx, unsigned char *in, unsigned char *out, u32 len, CRYPTO_WAY dec);

/**
 * @brief			This function initializes a DES encryption algorithm,  i.e. fills the psCipherContext_t structure pointed to by ctx with necessary data. 
 *
 * @param[in]		ctx 		Pointer to the Cipher Context. 
 * @param[in]		IV 		Pointer to the Initialization Vector
 * @param[in]		key 		Pointer to the key. 
 * @param[in]		keylen 	the length of key. 
 * @param[in]		cbc 		the encryption mode, DES supports ECB/CBC modes.
 *
 * @retval		0  		success 
 * @retval		other	failed	
 *
 * @note			None
 */ 
int tls_crypto_des_init(psCipherContext_t * ctx, const unsigned char *IV, const unsigned char *key, u32 keylen, CRYPTO_MODE cbc);

/**
 * @brief			This function encrypts or decrypts a variable length data stream according to DES.
 *
 * @param[in]		ctx 		Pointer to the Cipher Context. 
 * @param[in]		in 		Pointer to the input plaintext data stream(or the encrypted text data stream) of variable length.
 * @param[in]		out 		Pointer to the resulting ciphertext data stream.
 * @param[in]		len 		Length of the plaintext data stream in octets.
 * @param[in]		dec 		The cryption way which indicates encryption or decryption.
 *
 * @retval		0  		success 
 * @retval		other	failed	
 *
 * @note			None
 */ 
int tls_crypto_des_encrypt_decrypt(psCipherContext_t * ctx, unsigned char *in, unsigned char *out, u32 len, CRYPTO_WAY dec);
 
/**
 * @brief			This function initializes a CRC algorithm,  i.e. fills the psCrcContext_t structure pointed to by ctx with necessary data. 
 *
 * @param[in]		ctx 		Pointer to the CRC Context. 
 * @param[in]		key 		The initialization key. 
 * @param[in]		crc_type 	The CRC type, supports CRC8/CRC16 MODBUS/CRC16 CCITT/CRC32
 * @param[in]		mode 	Set input or outpu reflect.
 * @param[in]		dec 		The cryption way which indicates encryption or decryption.
 *				see OUTPUT_REFLECT
 * 				see INPUT_REFLECT
 *
 * @retval		0		success 
 * @retval		other	failed	
 *
 * @note			None
 */  
int tls_crypto_crc_init(psCrcContext_t * ctx, u32 key, CRYPTO_CRC_TYPE crc_type, u8 mode);

/**
 * @brief			This function updates the CRC value with a variable length bytes.
 *				This function may be called as many times as necessary, so the message may be processed in blocks.
 *
 * @param[in]		ctx 		Pointer to the CRC Context. 
 * @param[in]		in 		Pointer to a variable length bytes
 * @param[in]		len 		The bytes 's length 
 *
 * @retval		0		success 
 * @retval		other	failed	
 *
 * @note			None
 */
int tls_crypto_crc_update(psCrcContext_t * ctx, unsigned char *in, u32 len);

/**
 * @brief			This function ends a CRC operation and produces a CRC value.
 *
 * @param[in]		ctx 		Pointer to the CRC Context. 
 * @param[in]		crc_val 	Pointer to the CRC value.
 *
 * @retval		0		success 
 * @retval		other	failed	
 *
 * @note			None
 */
int tls_crypto_crc_final(psCrcContext_t * ctx, u32 *crc_val);

/**
 * @brief			This function initializes Message-Diggest context for usage in SHA1 algorithm, starts a new SHA1 operation and writes a new Digest Context. 
 *
 * @param[in]		md 		Pointer to the SHA1 Digest Context. 
 *
 * @retval		0		success 
 * @retval		other	failed	
 *
 * @note			None
 */
void tls_crypto_sha1_init(psDigestContext_t * md);

/**
 * @brief			Process a message block using SHA1 algorithm.
 *				This function performs a SHA1 block update operation. It continues an SHA1 message-digest operation, 
 *				by processing InputLen-byte length message block pointed to by buf, and by updating the SHA1 context pointed to by md.
 *				This function may be called as many times as necessary, so the message may be processed in blocks.
 *
 * @param[in]		md		Pointer to the SHA1 Digest Context. 
 * @param[in]  	buf 		InputLen-byte length message block
 * @param[in]  	len 		The buf 's length 
 *
 * @returnl		None	
 *
 * @note			None
 */
void tls_crypto_sha1_update(psDigestContext_t * md, const unsigned char *buf, u32 len);

/**
 * @brief			This function ends a SHA1 operation and produces a Message-Digest.
 *				This function finalizes SHA1 algorithm, i.e. ends an SHA1 Message-Digest operation, 
 *				writing the Message-Digest in the 20-byte buffer pointed to by hash in according to the information stored in context. 
 *
 * @param[in]		md		Pointer to the SHA1 Digest Context. 
 * @param[in]		hash 	Pointer to the Message-Digest
 *
 * @retval  		20  		success, return the hash size.
 * @retval  		<0   	failed  

 *
 * @note			None
 */
int tls_crypto_sha1_final(psDigestContext_t * md, unsigned char *hash);

/**
 * @brief			This function initializes Message-Diggest context for usage in MD5 algorithm, starts a new MD5 operation and writes a new Digest Context. 
 *				This function begins a MD5 Message-Diggest Algorithm, i.e. fills the psDigestContext_t structure pointed to by md with necessary data. 
 *				MD5 is the algorithm which takes as input a message of arbitrary length and produces as output a 128-bit "fingerprint" or "message digest" of the input.
 *				It is conjectured that it is computationally infeasible to produce two messages having the same message digest,
 *				or to produce any message having a given prespecified target message digest.
 *
 * @param[in]		md		MD5 Digest Context. 
 *
 * @return		None
 *
 * @note			None
 */
void tls_crypto_md5_init(psDigestContext_t * md);

/**
 * @brief			Process a message block using MD5 algorithm.
 *				This function performs a MD5 block update operation. It continues an MD5 message-digest operation,
 *				by processing InputLen-byte length message block pointed to by buf, and by updating the MD5 context pointed to by md. 
 *				This function may be called as many times as necessary, so the message may be processed in blocks.
 *
 * @param[in]		md		MD5 Digest Context. 
 *  @param[in]  	buf 		InputLen-byte length message block
 *  @param[in]  	len 		The buf 's length 
 *
 * @return		None
 *
 * @note			None
 */
void tls_crypto_md5_update(psDigestContext_t * md, const unsigned char *buf, u32 len);

/**
 * @brief			This function ends a MD5 operation and produces a Message-Digest.
 *				This function finalizes MD5 algorithm, i.e. ends an MD5 Message-Digest operation, 
 *				writing the Message-Digest in the 16-byte buffer pointed to by hash in according to the information stored in context. 
 *
 * @param[in]		md		MD5 Digest Context. 
 * @param[in]		hash 	the Message-Digest
 *
 * @retval  		16  		success, return the hash size.
 * @retval  		<0   	failed  
 *
 * @note			None
 */
int tls_crypto_md5_final(psDigestContext_t * md, unsigned char *hash);

/**
 * @brief			This function implements the large module power multiplication algorithm.
 *				res = a**e (mod n)  
 *
 * @param[in]		a 		Pointer to a bignumber. 
 * @param[in]		e 		Pointer to a bignumber.
 * @param[in]  	n 		Pointer to a bignumber.
 * @param[out]  	res 		Pointer to the result bignumber.
 *
 * @retval  		0  		success 
 * @retval  		other   	failed  
 *
 * @note			None
 */
int tls_crypto_exptmod(pstm_int *a, pstm_int *e, pstm_int *n, pstm_int *res);

/**
 * @brief			This function initializes the encryption module.
 *
 * @param		None
 *
 * @return  		None
 *
 * @note			None
 */
void tls_crypto_init(void);

#endif


