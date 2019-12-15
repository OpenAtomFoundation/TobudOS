int awss_report_cloud(void);

/**
 ** @brief   check reset flag in perisistent storage.
 **
 ** @retval  -1 : failure
 ** @retval  0 : sucess
 ** @note
 **      check reset flag in perisistent storage, if device failed to report reset message last time, retry it.
 **/
int awss_check_reset(void);

/**
 ** @brief   report reset to cloud.
 **
 ** @retval  -1 : failure
 ** @retval  0 : sucess
 ** @note
 **      device will save reset flag if device dosen't connect cloud, device will fails to send reset to cloud.
 **      when connection between device and cloud is ready, device will retry to report reset to cloud.
 **/
int awss_report_reset(void);

/**
 ** @brief   stop to report reset to cloud.
 **
 ** @retval  -1 : failure
 ** @retval  0 : sucess
 ** @note
 **      just stop report reset to cloud without any touch reset flag in flash.
 **/
int awss_stop_report_reset(void);

int awss_bind_deinit(void);

/**
 ** @brief   deinit bind operation.
 **
 ** @retval  -1 : failure
 ** @retval  0 : sucess
 ** @note
 **      stop report token to cloud and release coap topic and handler.
 **/
