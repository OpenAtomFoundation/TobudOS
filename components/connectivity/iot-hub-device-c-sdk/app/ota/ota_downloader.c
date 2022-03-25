/**
 * @copyright
 *
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright(C) 2018 - 2021 THL A29 Limited, a Tencent company.All rights reserved.
 *
 * Licensed under the MIT License(the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file ota_downloader.c
 * @brief
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2021-10-20
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2021-10-20 <td>1.0     <td>fancyxu   <td>first commit
 * </table>
 */

#include "ota_downloader.h"
#include "ota_firmware_save.h"
#include "utils_log.h"
#include "utils_md5.h"

#define OTA_HTTP_TIMEOUT_MS      5000
#define OTA_HTTP_BUF_SIZE        1024
#define MAX_SIZE_OF_DOWNLOAD_URL 512

/**
 * @brief Break point info.
 *
 */
typedef struct {
    OTAFirmwareInfo file_id;
    uint32_t        downloaded_size;
} OTADownloadInfo;

/**
 * @brief OTA downloader handle.
 *
 */
typedef struct {
    void* cos_download;
    void* mqtt_client;
    void* downloader;

    OTAFirmwareInfo download_now;
    OTADownloadInfo break_point;

    char          download_url[MAX_SIZE_OF_DOWNLOAD_URL];
    uint8_t       download_buff[OTA_HTTP_BUF_SIZE];
    uint32_t      download_size;
    IotMd5Context download_md5_ctx;

    OTADownloaderStatus status;
} OTADownloaderHandle;

/**
 * @brief Handle.
 *
 */
static OTADownloaderHandle sg_ota_downloader_handle = {0};

// ----------------------------------------------------------------------------
// Downloader function
// ----------------------------------------------------------------------------

// break point function

/**
 * @brief Read break point from file.
 *
 * @param[in,out] usr_data @see OTADownloaderHandle
 * @return 0 for success
 */
static int _ota_break_point_init(void* usr_data)
{
    OTADownloaderHandle* handle = (OTADownloaderHandle*)usr_data;

    sg_ota_downloader_handle.cos_download = NULL;
    utils_md5_reset(&handle->download_md5_ctx);
    return ota_break_point_read((uint8_t*)&handle->break_point, sizeof(handle->break_point)) < 0;
}

/**
 * @brief Memset break point.
 *
 * @param[in,out] usr_data @see OTADownloaderHandle
 */
static void _ota_break_point_deinit(void* usr_data)
{
    OTADownloaderHandle* handle = (OTADownloaderHandle*)usr_data;
    memset(&handle->break_point, 0, sizeof(handle->break_point));
}

/**
 * @brief Set break point using download now info.
 *
 * @param[in,out] usr_data @see OTADownloaderHandle
 * @return 0 for success
 */
static int _ota_break_point_set(void* usr_data)
{
    OTADownloaderHandle* handle = (OTADownloaderHandle*)usr_data;
    // read break point info from flash
    memset(&handle->break_point, 0, sizeof(handle->break_point));
    memcpy(&handle->break_point.file_id, &handle->download_now, sizeof(handle->break_point.file_id));
    return 0;
}

/**
 * @brief Update break point and save.
 *
 * @param[in,out] usr_data @see OTADownloaderHandle
 * @return 0 for success
 */
static int _ota_break_point_save(void* usr_data)
{
    OTADownloaderHandle* handle = (OTADownloaderHandle*)usr_data;

    // update md5 sum & download_size
    handle->break_point.downloaded_size += handle->download_size;
    utils_md5_update(&handle->download_md5_ctx, handle->download_buff, handle->download_size);

    // report progress
    char buf[256];
    int  buf_len = sizeof(buf);
    IOT_OTA_ReportProgress(handle->mqtt_client, buf, buf_len, IOT_OTA_REPORT_TYPE_DOWNLOADING,
                           handle->break_point.downloaded_size * 100 / handle->download_now.file_size,
                           handle->break_point.file_id.version);

    // write to local， only write downloaded size is ok
    return ota_break_point_write((uint8_t*)&handle->break_point, sizeof(OTADownloadInfo));
}

/**
 * @brief Check if break point matches download now info.
 *
 * @param[in,out] usr_data @see OTADownloaderHandle
 * @return 0 for success
 */
static int _ota_break_point_check(void* usr_data)
{
    OTADownloaderHandle* handle = (OTADownloaderHandle*)usr_data;
    // version should be the same, download size should not bigger than file size
    Log_d("download now:%s,%d,%s", handle->download_now.version, handle->download_now.file_size,
          handle->download_now.md5sum);
    Log_d("break point:%s,%d,%d,%s", handle->break_point.file_id.version, handle->break_point.file_id.file_size,
          handle->break_point.downloaded_size, handle->break_point.file_id.md5sum);
    return strncmp(handle->break_point.file_id.version, handle->download_now.version, MAX_SIZE_OF_FW_VERSION) ||
           handle->break_point.file_id.file_size != handle->download_now.file_size ||
           handle->break_point.downloaded_size > handle->download_now.file_size ||
           strncmp(handle->break_point.file_id.md5sum, handle->download_now.md5sum, 32);
}

/**
 * @brief Calculate md5 sum according break point.
 *
 * @param[in,out] usr_data @see OTADownloaderHandle
 * @return 0 for success
 */
static int _ota_break_point_restore(void* usr_data)
{
    OTADownloaderHandle* handle = (OTADownloaderHandle*)usr_data;

    // update md5 according downloaded data
    size_t rlen, total_read = 0, size = 0;

    size = handle->break_point.downloaded_size;

    while (size > 0) {
        rlen = (size > OTA_HTTP_BUF_SIZE) ? OTA_HTTP_BUF_SIZE : size;
        if (ota_firmware_read(handle->download_buff, rlen, total_read) < 0) {
            Log_e("read data failed");
            handle->break_point.downloaded_size = 0;
            break;
        }
        utils_md5_update(&handle->download_md5_ctx, handle->download_buff, rlen);
        size -= rlen;
        total_read += rlen;
    }
    return 0;
}

// data download function

/**
 * @brief Init cos downloader.
 *
 * @param[in,out] usr_data @see OTADownloaderHandle
 * @return 0 for success
 */
static int _ota_data_download_init(void* usr_data)
{
    OTADownloaderHandle* handle = (OTADownloaderHandle*)usr_data;
    IotCosDownloadParams params = {
        .url              = handle->download_url,
        .offset           = handle->break_point.downloaded_size,
        .file_size        = handle->break_point.file_id.file_size,
        .is_fragmentation = false,
        .is_https_enabled = false,
    };
    handle->cos_download = IOT_COS_DownloadInit(&params);
    return handle->cos_download ? 0 : -1;
}

/**
 * @brief Deinit cos downloader.
 *
 * @param[in,out] usr_data @see OTADownloaderHandle
 */
static void _ota_data_download_deinit(void* usr_data)
{
    OTADownloaderHandle* handle = (OTADownloaderHandle*)usr_data;
    IOT_COS_DownloadDeinit(handle->cos_download);
}

/**
 * @brief Check if download finished.
 *
 * @param[in,out] usr_data @see OTADownloaderHandle
 * @return 0 for success
 */
static int _ota_data_download_is_over(void* usr_data)
{
    // check download is over
    OTADownloaderHandle* handle = (OTADownloaderHandle*)usr_data;
    return handle->break_point.downloaded_size == handle->download_now.file_size;
}

/**
 * @brief Download from cos server.
 *
 * @param[in,out] usr_data @see OTADownloaderHandle
 * @return 0 for success
 */
static int _ota_data_download_recv(void* usr_data)
{
    // download data using http
    OTADownloaderHandle* handle = (OTADownloaderHandle*)usr_data;
    // TODO: https download
    handle->download_size =
        IOT_COS_DownloadFetch(handle->cos_download, handle->download_buff, OTA_HTTP_BUF_SIZE, OTA_HTTP_TIMEOUT_MS);
    return handle->download_size;
}

/**
 * @brief Sava firmware to file.
 *
 * @param[in,out] usr_data @see OTADownloaderHandle
 * @return 0 for success
 */
static int _ota_data_download_save(void* usr_data)
{
    OTADownloaderHandle* handle = (OTADownloaderHandle*)usr_data;
    return handle->download_size > 0
               ? ota_firmware_write(handle->download_buff, handle->download_size, handle->break_point.downloaded_size)
               : 0;
}

/**
 * @brief Process download result.
 *
 * @param[in,out] usr_data @see OTADownloaderHandle
 * @param[in] status status when finish download, @see UtilsDownloaderStatus
 * @return 0 for success
 */
static int _ota_data_download_finish(void* usr_data, UtilsDownloaderStatus status)
{
    OTADownloaderHandle* handle = (OTADownloaderHandle*)usr_data;

    int  rc = 0;
    char buf[256];
    int  buf_len = sizeof(buf);

    switch (status) {
        case UTILS_DOWNLOADER_STATUS_SUCCESS:
            utils_md5_finish(&handle->download_md5_ctx);
            ota_firmware_finish(handle->download_now.file_size);

            int valid = !utils_md5_compare(&handle->download_md5_ctx, handle->download_now.md5sum);

            if (!valid) {
                memset(&handle->break_point, 0, sizeof(handle->break_point));
                ota_break_point_write((uint8_t*)&handle->break_point, sizeof(handle->break_point));
            }

            rc = valid ? IOT_OTA_ReportProgress(handle->mqtt_client, buf, buf_len, IOT_OTA_REPORT_TYPE_UPGRADE_SUCCESS,
                                                0, handle->download_now.version)
                       : IOT_OTA_ReportProgress(handle->mqtt_client, buf, buf_len, IOT_OTA_REPORT_TYPE_MD5_NOT_MATCH, 0,
                                                handle->download_now.version);
            break;
        case UTILS_DOWNLOADER_STATUS_NETWORK_FAILED:
            rc = IOT_OTA_ReportProgress(handle->mqtt_client, buf, buf_len, IOT_OTA_REPORT_TYPE_DOWNLOAD_TIMEOUT, 0,
                                        handle->download_now.version);
            break;
        case UTILS_DOWNLOADER_STATUS_BREAK_POINT_FAILED:
        case UTILS_DOWNLOADER_STATUS_DATA_DOWNLOAD_FAILED:
            rc = IOT_OTA_ReportProgress(handle->mqtt_client, buf, buf_len, IOT_OTA_REPORT_TYPE_UPGRADE_FAIL, 0,
                                        handle->download_now.version);
            break;
        default:
            break;
    }
    handle->status = OTA_DOWNLOADER_STATUS_FINISHED;
    return rc;
}

// ----------------------------------------------------------------------------
// API
// ----------------------------------------------------------------------------

/**
 * @brief Init ota downloader.
 *
 * @param[in,out] client pointer to mqtt client
 * @return 0 for success.
 */
int ota_downloader_init(void* client)
{
    // downloader init
    UtilsDownloaderFunction ota_callback = {
        .downloader_malloc = HAL_Malloc,
        .downloader_free   = HAL_Free,
        // break point
        .break_point_init    = _ota_break_point_init,
        .break_point_deinit  = _ota_break_point_deinit,
        .break_point_set     = _ota_break_point_set,
        .break_point_save    = _ota_break_point_save,
        .break_point_check   = _ota_break_point_check,
        .break_point_restore = _ota_break_point_restore,

        // data download
        .data_download_init    = _ota_data_download_init,
        .data_download_deinit  = _ota_data_download_deinit,
        .data_download_is_over = _ota_data_download_is_over,
        .data_download_recv    = _ota_data_download_recv,
        .data_download_save    = _ota_data_download_save,
        .data_download_finish  = _ota_data_download_finish,
    };

    sg_ota_downloader_handle.downloader = utils_downloader_init(ota_callback, &sg_ota_downloader_handle);
    if (!sg_ota_downloader_handle.downloader) {
        Log_e("initialize downloaded failed");
        return -1;
    }
    sg_ota_downloader_handle.mqtt_client = client;
    sg_ota_downloader_handle.status      = OTA_DOWNLOADER_STATUS_INITTED;
    return 0;
}

/**
 * @brief Set download info of ota firmware.
 *
 * @param[in] firmware_info pointer to firmware info
 * @param[in] url url of cos download
 * @param[in] url_len download length
 */
void ota_downloader_info_set(OTAFirmwareInfo* firmware_info, const char* url, int url_len)
{
    if (OTA_DOWNLOADER_STATUS_DOWNLOADING != sg_ota_downloader_handle.status) {
        memcpy(&sg_ota_downloader_handle.download_now, firmware_info, sizeof(OTAFirmwareInfo));
        strncpy(sg_ota_downloader_handle.download_url, url, url_len);
        sg_ota_downloader_handle.download_url[url_len] = '\0';

        sg_ota_downloader_handle.status = OTA_DOWNLOADER_STATUS_DOWNLOADING;
    }
}

/**
 * @brief Process ota download.
 *
 * @return @see OTADownloaderStatus
 */
OTADownloaderStatus ota_downloader_process(void)
{
    if (OTA_DOWNLOADER_STATUS_DOWNLOADING == sg_ota_downloader_handle.status) {
        utils_downloader_process(sg_ota_downloader_handle.downloader);
    }
    return sg_ota_downloader_handle.status;
}

/**
 * @brief Deinit ota downloader.
 *
 */
void ota_downloader_deinit(void)
{
    utils_downloader_deinit(sg_ota_downloader_handle.downloader);
    memset(&sg_ota_downloader_handle, 0, sizeof(sg_ota_downloader_handle));
}
