#ifndef _HAL_AES_H_
#define _HAL_AES_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define AES_LITTLE_ENDIAN   0x00000100
#define AES_MAX_KEY_LENGTH  32
#define AES_IT_DATAINT      0x4
#define AES_IT_KEYINT       0x2
#define AES_IT_BUSY         0x1
#define AES_IT_ALL          0x7
#define AES_CRYPTO_KEYEXP   0x2
#define CONFIG_AES_NUM      1 
#define AES_WORK_ENABLE_OFFSET  0
#define AES_INT_KEY_OFFSET      1
#define AES_INT_ENABLE_OFFSET   2
#define AES_MODE_OFFSET         3
#define AES_KEY_LEN_OFFSET      4
#define AES_OPCODE_OFFSET       6
#define __IOM    volatile 
#define AES   (void*)REG_AES_BASE
#define DRV_ERRNO_AES_BASE      0x40400000
  
typedef enum {
    DRV_ERROR = 1,//ERRNO_DRV_START,   ///< Unspecified error
    DRV_ERROR_BUSY,                ///< Driver is busy
    DRV_ERROR_TIMEOUT,             ///< Timeout occurred
    DRV_ERROR_UNSUPPORTED,         ///< Operation not supported
    DRV_ERROR_PARAMETER,           ///< Parameter error
    DRV_ERROR_SPECIFIC             ///< Start of driver specific errors
} drv_err_e;
    
typedef enum  {
    DRV_POWER_OFF,                        ///< Power off: no operation possible
    DRV_POWER_LOW,                        ///< Low Power mode: retain state, detect and signal wake-up events
    DRV_POWER_FULL,                       ///< Power on: full operation at maximum performance
    DRV_POWER_SUSPEND,                    ///< Power suspend: power saving operation
} aes_power_stat_e;

typedef void *aes_handle_t;
typedef enum {
    AES_CRYPTO_MODE_ENCRYPT                  = 0,   ///< encrypt Mode
    AES_CRYPTO_MODE_DECRYPT,                        ///< decrypt Mode
} aes_crypto_mode_e;
typedef enum {
    AES_KEY_LEN_BITS_128        = 0,      ///< 128 Data bits
    AES_KEY_LEN_BITS_192,                 ///< 192 Data bits
    AES_KEY_LEN_BITS_256                  ///< 256 Data bits
} aes_key_len_bits_e;

typedef struct {
    __IOM uint32_t DATAIN[4];                 /* Offset: 0x000 (R/W) Data input 0~127 */
    __IOM uint32_t KEY[8];                    /* Offset: 0x010 (R/W) Key 0~255 */
    __IOM uint32_t IV[4];                     /* Offset: 0x030 (R/W) Initial Vector: 0~127 */
    __IOM uint32_t CTRL;                      /* Offset: 0x040 (R/W) AES Control Register */
    __IOM uint32_t STATE;                     /* Offset: 0x044 (R/W) AES State Register */
    __IOM uint32_t DATAOUT[4];                /* Offset: 0x048 (R/W) Data Output 0~31 */
} aes_reg_t;

/****** AES specific error codes *****/
typedef enum {
    AES_ERROR_MODE  = (DRV_ERROR_SPECIFIC + 1),        ///< Specified Mode not supported
    AES_ERROR_DATA_BITS,                               ///< Specified number of Data bits not supported
    AES_ERROR_ENDIAN                                   ///< Specified endian not supported
} aes_error_e;

/*----- AES Control Codes: Mode -----*/
typedef enum {
    AES_MODE_ECB                  = 0,   ///< ECB Mode
    AES_MODE_CBC,                        ///< CBC Mode
    AES_MODE_CFB1,                       ///< CFB1 Mode
    AES_MODE_CFB8,                       ///< CFB8 Mode
    AES_MODE_CFB128,                     ///< CFB128 Mode
    AES_MODE_OFB,                        ///< OFB Mode
    AES_MODE_CTR                         ///< CTR Mode
} aes_mode_e;

/*----- AES Control Codes: Mode Parameters: Endian -----*/
typedef enum {
    AES_ENDIAN_LITTLE          = 0,       ///< Little Endian
    AES_ENDIAN_BIG                        ///< Big Endian
} aes_endian_mode_e;

/**
\brief AES Status
*/
typedef struct {
    uint32_t busy             : 1;        ///< busy flag
} aes_status_t;

/****** AES Event *****/
typedef enum {
    AES_EVENT_CRYPTO_COMPLETE    = 0   ///< Encrypt completed
} aes_event_e;
typedef void (*aes_event_cb_t)(int32_t idx, aes_event_e event);   ///< Pointer to \ref aes_event_cb_t : AES Event call back.


/**
\brief AES Device Driver Capabilities.
*/
typedef struct {
    uint32_t ecb_mode           : 1;      ///< supports ECB mode
    uint32_t cbc_mode           : 1;      ///< supports CBC mode
    uint32_t cfb1_mode          : 1;      ///< supports CFB1 mode
    uint32_t cfb8_mode          : 1;      ///< supports CFB8 mode
    uint32_t cfb128_mode        : 1;      ///< supports CFB128 mode
    uint32_t ofb_mode           : 1;      ///< supports OFB mode
    uint32_t ctr_mode           : 1;      ///< supports CTR mode
    uint32_t bits_128           : 1;      ///< supports 128bits key length
    uint32_t bits_192           : 1;      ///< supports 192bits key length
    uint32_t bits_256           : 1;      ///< supports 256bits key length
} aes_capabilities_t;


// Function documentation

/**
  \brief       aes_interrupt_handler 
  \param[in]   idx   device id
  \param[in]   event  interrupt event  type
  \return      NULL
*/
void aes_interrupt_handler(int32_t idx, aes_event_e event);
/**
  \brief       Initialize AES Interface. 1. Initializes the resources needed for the AES interface 2.registers event callback function
  \param[in]   idx   device id
  \param[in]   cb_event  event callback function \ref aes_event_cb_t
  \return      if success return aes handle else return NULL
*/
aes_handle_t hal_aes_initialize(int32_t idx, aes_event_cb_t cb_event);

/**
  \brief       De-initialize AES Interface. stops operation and releases the software resources used by the interface
  \param[in]   handle  aes handle to operate.
  \return      error code
*/
int32_t hal_aes_uninitialize(aes_handle_t handle);

/**
  \brief       control aes power.
  \param[in]   handle  aes handle to operate.
  \param[in]   state   power state.\ref aes_power_stat_e.
  \return      error code
*/
#ifdef CONFIG_LPM
int32_t hal_aes_power_control(aes_handle_t handle, aes_power_stat_e state);
#endif
/**
  \brief       Get driver capabilities.
  \param[in]   idx device id
  \return      \ref aes_capabilities_t
*/
aes_capabilities_t hal_aes_get_capabilities(int32_t idx);

/**
  \brief       config aes mode.
  \param[in]   handle  aes handle to operate.
  \param[in]   mode      \ref aes_mode_e
  \param[in]   keylen_bits \ref aes_key_len_bits_e
  \param[in]   endian    \ref aes_endian_mode_e
  \return      error code
*/
int32_t hal_aes_config(aes_handle_t handle,
                       aes_mode_e mode,
                       aes_key_len_bits_e keylen_bits,
                       aes_endian_mode_e endian
                      );

/**
  \brief       set crypto key.
  \param[in]   handle    aes handle to operate.
  \param[in]   context   aes information context
  \param[in]   key       Pointer to the key buf
  \param[in]   key_len   Pointer to \ref aes_key_len_bits_e
  \param[in]   enc       \ref aes_crypto_mode_e
  \return      error code
*/
int32_t hal_aes_set_key(aes_handle_t handle, void *context, void *key, aes_key_len_bits_e key_len, aes_crypto_mode_e enc);

/**
  \brief       aes ecb encrypt or decrypt
  \param[in]   handle  aes handle to operate.
  \param[in]   context aes information context
  \param[in]   in   Pointer to the Source data
  \param[out]  out  Pointer to the Result data.
  \param[in]   len  the Source data len.
  \return      error code
*/
int32_t hal_aes_ecb_crypto(aes_handle_t handle, void *context, void *in, void *out, uint32_t len);

/**
  \brief       aes cbc encrypt or decrypt
  \param[in]   handle  aes handle to operate.
  \param[in]   context aes information context
  \param[in]   in   Pointer to the Source data
  \param[out]  out  Pointer to the Result data.
  \param[in]   len  the Source data len.
  \param[in]   iv   Pointer to initialization vector
  \return      error code
*/
int32_t hal_aes_cbc_crypto(aes_handle_t handle, void *context, void *in, void *out, uint32_t len, uint8_t iv[16]);

/**
  \brief       aes cfb1 encrypt or decrypt
  \param[in]   handle  aes handle to operate.
  \param[in]   context aes information context
  \param[in]   in   Pointer to the Source data
  \param[out]  out  Pointer to the Result data.
  \param[in]   len  the Source data len.
  \param[in]   iv   Pointer to initialization vector
  \return      error code
*/
int32_t hal_aes_cfb1_crypto(aes_handle_t handle, void *context, void *in, void *out,  uint32_t len, uint8_t iv[16]);

/**
  \brief       aes cfb8 encrypt or decrypt
  \param[in]   handle  aes handle to operate.
  \param[in]   context aes information context
  \param[in]   in   Pointer to the Source data
  \param[out]  out  Pointer to the Result data.
  \param[in]   len  the Source data len.
  \param[in]   iv   Pointer to initialization vector
  \return      error code
*/
int32_t hal_aes_cfb8_crypto(aes_handle_t handle, void *context, void *in, void *out, uint32_t len, uint8_t iv[16]);

/**
  \brief       aes cfb128 encrypt or decrypt
  \param[in]   handle  aes handle to operate.
  \param[in]   context aes information context
  \param[in]   in   Pointer to the Source data
  \param[out]  out  Pointer to the Result data.
  \param[in]   len  the Source data len.
  \param[in]   iv   Pointer to initialization vector
  \param[in]   num  the number of the 128-bit block we have used
  \return      error code
*/
int32_t hal_aes_cfb128_crypto(aes_handle_t handle, void *context, void *in, void *out, uint32_t len, uint8_t iv[16], uint32_t *num);

/**
  \brief       aes ofb encrypt or decrypt
  \param[in]   handle  aes handle to operate.
  \param[in]   context aes information context
  \param[in]   in   Pointer to the Source data
  \param[out]  out  Pointer to the Result data.
  \param[in]   len  the Source data len.
  \param[in]   iv   Pointer to initialization vector
  \param[in]   num  the number of the 128-bit block we have used
  \return      error code
*/
int32_t hal_aes_ofb_crypto(aes_handle_t handle, void *context, void *in, void *out, uint32_t len, uint8_t iv[16], uint32_t *num);

/**
  \brief       aes ctr encrypt or decrypt
  \param[in]   handle  aes handle to operate.
  \param[in]   context aes information context
  \param[in]   in   Pointer to the Source data
  \param[out]  out  Pointer to the Result data.
  \param[in]   len  the Source data len.
  \param[in]   nonce_counter   Pointer to the 128-bit nonce and counter
  \param[in]   stream_block  Pointer to the saved stream-block for resuming
  \param[in]   num  the number of the 128-bit block we have used
  \return      error code
*/
int32_t hal_aes_ctr_crypto(aes_handle_t handle, void *context, void *in, void *out, uint32_t len, uint8_t nonce_counter[16], uint8_t stream_block[16], uint32_t *num);

/**
  \brief       Get AES status.
  \param[in]   handle  aes handle to operate.
  \return      AES status \ref aes_status_t
*/
aes_status_t hal_aes_get_status(aes_handle_t handle);

void aes_irqhandler(int32_t idx);

#ifdef __cplusplus
}
#endif

#endif /* _HAL_AES_H_ */
