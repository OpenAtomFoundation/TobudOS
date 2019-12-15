#ifndef __AWSS_RESET__
#define __AWSS_RESET__

#define AWSS_RESET_PKT_LEN         (256)
#define AWSS_RESET_TOPIC_LEN       (128)
#define AWSS_RESET_MSG_ID_LEN      (16)

#define TOPIC_RESET_REPORT         "/sys/%s/%s/thing/reset"
#define TOPIC_RESET_REPORT_REPLY   "/sys/%s/%s/thing/reset_reply"
#define METHOD_RESET_REPORT        "thing.reset"

#define AWSS_RESET_REQ_FMT         "{\"id\":%s, \"version\":\"1.0\", \"method\":\"%s\", \"params\":%s}"

#define AWSS_KV_RST                "awss.rst"

int awss_check_reset();

/**
 * @brief   report reset to cloud.
 *
 * @retval  -1 : failure
 * @retval  0 : sucess
 * @note
 *      device will save reset flag if device dosen't connect cloud, device will fails to send reset to cloud.
 *      when connection between device and cloud is ready, device will retry to report reset to cloud.
 */
int awss_report_reset();

/**
 * @brief   stop to report reset to cloud.
 *
 * @retval  -1 : failure
 * @retval  0 : sucess
 * @note
 *      just stop report reset to cloud without any touch reset flag in flash.
 */
int awss_stop_report_reset();

#endif
