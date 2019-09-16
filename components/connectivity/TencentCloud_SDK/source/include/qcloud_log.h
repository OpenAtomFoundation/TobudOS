#ifndef _QCLOUD_LOG_H_
#define  _QCLOUD_LOG_H_

/* 单条日志最大长度 */
#define QCLOUD_LOG_LEN_MAX		(1024)

/*用户自定义日志打印函数回调*/
typedef int (*qcloud_log_handler_t)(const char *message);

/**
 * @brief 设置日志回调函数，用户接管日志内容用于输出到文件等操作
 *
 * @param handler 回调函数指针
 *
 */
void qcloud_log_handler_set(qcloud_log_handler_t handler);

/**
 * @brief 日志打印及上报函数，默认打印到标准输出，当用户设置日志打印handler时，回调handler
 *
 * 当日志上报功能打开时，会将日志缓存后上报到云端日志服务器
 *
 * @param file 源文件名
 * @param func 函数名
 * @param line 行号
 * @param level 日志等级
 */
void qcloud_log_write(const char *file, const char *func, const int line, const int level, const char *fmt, ...);

/* 日志打印及上报对外接口*/

#if (QCLOUD_CFG_LOG_LEVEL >= QCLOUD_LOG_LEVEL_DEBUG)
#define QCLOUD_LOG_D(args...)   qcloud_log_write(__FILE__, __FUNCTION__, __LINE__, QCLOUD_LOG_LEVEL_DEBUG, args)
#else
#define QCLOUD_LOG_D(args...)
#endif

#if (QCLOUD_CFG_LOG_LEVEL >= QCLOUD_LOG_LEVEL_INFO)
#define QCLOUD_LOG_I(args...)   qcloud_log_write(__FILE__, __FUNCTION__, __LINE__, QCLOUD_LOG_LEVEL_INFO, args)
#else
#define QCLOUD_LOG_I(args...)
#endif

#if (QCLOUD_CFG_LOG_LEVEL >= QCLOUD_LOG_LEVEL_WARN)
#define QCLOUD_LOG_W(args...)   qcloud_log_write(__FILE__, __FUNCTION__, __LINE__, QCLOUD_LOG_LEVEL_WARN, args)
#else
#define QCLOUD_LOG_W(args...)
#endif

#if (QCLOUD_CFG_LOG_LEVEL >= QCLOUD_LOG_LEVEL_ERROR)
#define QCLOUD_LOG_E(args...)   qcloud_log_write(__FILE__, __FUNCTION__, __LINE__, QCLOUD_LOG_LEVEL_ERROR, args)
#else
#define QCLOUD_LOG_E(args...)
#endif

#endif

