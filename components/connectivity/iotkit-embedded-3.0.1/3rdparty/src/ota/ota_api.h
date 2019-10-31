/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __OTA_EXPORT_H__
#define __OTA_EXPORT_H__

#include "infra_types.h"
#include "infra_defs.h"

#if defined(__cplusplus)
extern "C" {
#endif

#define OTA_CH_SIGNAL_MQTT      (0)
#define OTA_CH_SIGNAL_COAP      (1)
#define OTA_CH_FETCH_HTTP       (1)


typedef enum {

    IOT_OTAE_GENERAL = -1,
    IOT_OTAE_INVALID_PARAM = -2,
    IOT_OTAE_INVALID_STATE = -3,
    IOT_OTAE_STR_TOO_LONG = -4,
    IOT_OTAE_FETCH_FAILED = -5,
    IOT_OTAE_NOMEM = -6,
    IOT_OTAE_OSC_FAILED = -7,
    IOT_OTAE_NONE = 0,

} IOT_OTA_Err_t;


/* State of OTA */
typedef enum {
    IOT_OTAS_UNINITED = 0,  /* Uninitialized State */
    IOT_OTAS_INITED,        /* Initialized State */
    IOT_OTAS_FETCHING,      /* Fetching firmware */
    IOT_OTAS_FETCHED        /* Fetching firmware finish */
} IOT_OTA_State_t;

typedef enum {
    IOT_OTAT_NONE,
    IOT_OTAT_COTA,
    IOT_OTAT_FOTA
} IOT_OTA_Type_t;

/* Progress of OTA */
typedef enum {

    /* Burn firmware file failed */
    IOT_OTAP_BURN_FAILED = -4,

    /* Check firmware file failed */
    IOT_OTAP_CHECK_FALIED = -3,

    /* Fetch firmware file failed */
    IOT_OTAP_FETCH_FAILED = -2,

    /* Initialized failed */
    IOT_OTAP_GENERAL_FAILED = -1,


    /* [0, 100], percentage of fetch progress */

    /* The minimum percentage of fetch progress */
    IOT_OTAP_FETCH_PERCENTAGE_MIN = 0,

    /* The maximum percentage of fetch progress */
    IOT_OTAP_FETCH_PERCENTAGE_MAX = 100

} IOT_OTA_Progress_t;


typedef enum {
    IOT_OTAG_COTA_CONFIG_ID,
    IOT_OTAG_COTA_CONFIG_SIZE,
    IOT_OTAG_COTA_SIGN,
    IOT_OTAG_COTA_SIGN_METHOD,
    IOT_OTAG_COTA_URL,
    IOT_OTAG_COTA_GETTYPE,
    IOT_OTAG_OTA_TYPE,
    IOT_OTAG_FETCHED_SIZE,     /* option for get already fetched size */
    IOT_OTAG_FILE_SIZE,        /* size of file */
    IOT_OTAG_MD5SUM,           /* md5 in string format */
    IOT_OTAG_VERSION,          /* version in string format */
    IOT_OTAG_CHECK_FIRMWARE,    /* Check firmware is valid or not */
    IOT_OTAG_CHECK_CONFIG,      /* Check config file is valid or not */
    IOT_OTAG_RESET_FETCHED_SIZE /* reset the size_fetched parameter to be 0 */
} IOT_OTA_CmdType_t;

/** @defgroup group_api api
 *  @{
 */

/** @defgroup group_api_ota ota
 *  @{
 */

/**
 * @brief Initialize OTA module, and return handle.
 *        The MQTT client must be construct before calling this interface.
 *
 * @param [in] product_key: specify the product key.
 * @param [in] device_name: specify the device name.
 * @param [in] ch_signal: specify the signal channel.
 *
 * @retval  0 : Successful.
 * @retval -1 : Failed.
 * @see None.
 */
void *IOT_OTA_Init(const char *product_key, const char *device_name, void *ch_signal);


/**
 * @brief Deinitialize OTA module specified by the 'handle', and release the related resource.
 *        You must call this interface to release resource if reboot is not invoked after downloading.
 *
 * @param [in] handle: specify the OTA module.
 *
 * @retval   0 : Successful.
 * @retval < 0 : Failed, the value is error code.
 * @see None.
 */
int IOT_OTA_Deinit(void *handle);


/**
 * @brief Report firmware version information to OTA server (optional).
 *        NOTE: please
 *
 * @param [in] handle: specify the OTA module.
 * @param [in] version: specify the firmware version in string format.
 *
 * @retval   0 : Successful.
 * @retval < 0 : Failed, the value is error code.
 * @see None.
 */
int IOT_OTA_ReportVersion(void *handle, const char *version);

/**
 * @brief Report detail progress to OTA server (optional).
 *        NOTE: please
 *
 * @param [in] handle: specify the OTA module.
 * @param [in] progress: specify the progress defined by 'IOT_OTA_Progress_t'.
 * @param [in] msg: detail progress information in string.
 *
 * @retval   0 : Successful.
 * @retval < 0 : Failed, the value is error code.
 * @see None.
 */
int IOT_OTA_ReportProgress(void *handle, IOT_OTA_Progress_t progress, const char *msg);


/**
 * @brief Check whether is on fetching state
 *
 * @param [in] handle: specify the OTA module.
 *
 * @retval 1 : Yes.
 * @retval 0 : No.
 * @see None.
 */
int IOT_OTA_IsFetching(void *handle);


/**
 * @brief Check whether is on end-of-fetch state.
 *
 * @param [in] handle: specify the OTA module.
 *
 * @retval 1 : Yes.
 * @retval 0 : False.
 * @see None.
 */
int IOT_OTA_IsFetchFinish(void *handle);


/**
 * @brief fetch firmware from remote server with specific timeout value.
 *        NOTE: If you want to download more faster, the bigger 'buf' should be given.
 *
 * @param [in] handle: specify the OTA module.
 * @param [out] buf: specify the space for storing firmware data.
 * @param [in] buf_len: specify the length of 'buf' in bytes.
 * @param [in] timeout_s: specify the timeout value in second.
 *
 * @retval      < 0 : Error occur..
 * @retval        0 : No any data be downloaded in 'timeout_s' timeout period.
 * @retval (0, len] : The length of data be downloaded in 'timeout_s' timeout period in bytes.
 * @see None.
 */
int IOT_OTA_FetchYield(void *handle, char *buf, uint32_t buf_len, uint32_t timeout_s);


/**
 * @brief Get OTA information specified by 'type'.
 *        By this interface, you can get information like state, size of file, md5 of file, etc.
 *
 * @param [in] handle: handle of the specific OTA
 * @param [in] type: specify what information you want, see detail 'IOT_OTA_CmdType_t'
 * @param [out] buf: specify buffer for data exchange
 * @param [in] buf_len: specify the length of 'buf' in byte.
 * @return
   @verbatim
      NOTE:
      1) When type is IOT_OTAG_FETCHED_SIZE, 'buf' should be pointer of uint32_t, and 'buf_len' should be 4.
      2) When type is IOT_OTAG_FILE_SIZE, 'buf' should be pointer of uint32_t, and 'buf_len' should be 4.
      3) When type is IOT_OTAG_MD5SUM, 'buf' should be a buffer, and 'buf_len' should be 33.
      4) When type is IOT_OTAG_VERSION, 'buf' should be a buffer, and 'buf_len' should be OTA_VERSION_LEN_MAX.
      5) When type is IOT_OTAG_CHECK_FIRMWARE, 'buf' should be pointer of uint32_t, and 'buf_len' should be 4.
         0, firmware is invalid; 1, firmware is valid.
  @endverbatim
 *
 * @retval   0 : Successful.
 * @retval < 0 : Failed, the value is error code.
 * @see None.
 */
int IOT_OTA_Ioctl(void *handle, IOT_OTA_CmdType_t type, void *buf, int buf_len);


/**
 * @brief Get last error code.
 *
 * @param [in] handle: specify the OTA module.
 *
 * @return The error code.
 * @see None.
 */
int IOT_OTA_GetLastError(void *handle);

/** @} */ /* end of api_ota */
/** @} */ /* end of api */

#if defined(__cplusplus)
}
#endif

#endif /* __OTA_EXPORT_H__ */


