#ifndef _INFRA_LOG_H_
#define _INFRA_LOG_H_

#include <stdarg.h>
#include "infra_defs.h"

int HAL_Snprintf(char *str, const int len, const char *fmt, ...);
void HAL_Printf(const char *fmt, ...);
int HAL_Vsnprintf(char *str, const int len, const char *format, va_list ap);

#define LITE_printf                     HAL_Printf
#define LITE_snprintf                   HAL_Snprintf
#define LITE_vsnprintf                  HAL_Vsnprintf
#define LITE_LOG_ENABLED

#define LOG_MSG_MAXLEN                  (255)
#define LOG_MOD_NAME_LEN                (7)
#define LOG_PREFIX_FMT                  "[%s] %s(%d): "

#define HEXDUMP_SEP_LINE                "+" \
    "-----------------------" \
    "-----------------------" \
    "-----------------------"

#if defined(_PLATFORM_IS_LINUX_)
    #undef  LOG_MSG_MAXLEN
    #define LOG_MSG_MAXLEN              (512)
#endif

typedef struct {
    char            name[LOG_MOD_NAME_LEN + 1];
    int             priority;
    char            text_buf[LOG_MSG_MAXLEN + 1];
} log_client;

int     LITE_get_loglevel(void);
void    LITE_set_loglevel(int level);
int     LITE_hexdump(const char *title, const void *buf, const int len);

void    LITE_syslog_routine(char *m, const char *f, const int l, const int level, const char *fmt, va_list *params);
void    LITE_syslog(char *m, const char *f, const int l, const int level, const char *fmt, ...);

#define LOG_NONE_LEVEL                  (0)     /* no log printed at all */
#define LOG_CRIT_LEVEL                  (1)     /* current application aborting */
#define LOG_ERR_LEVEL                   (2)     /* current app-module error */
#define LOG_WARNING_LEVEL               (3)     /* using default parameters */
#define LOG_INFO_LEVEL                  (4)     /* running messages */
#define LOG_DEBUG_LEVEL                 (5)     /* debugging messages */
#define LOG_FLOW_LEVEL                  (6)     /* code/packet flow messages */

#if defined(INFRA_LOG) && !defined(INFRA_LOG_ALL_MUTED)
    #if defined(INFRA_LOG_MUTE_FLW)
        #define log_flow(mod, ...)
    #else
        #define log_flow(mod, ...)          LITE_syslog(mod, __FUNCTION__, __LINE__, LOG_FLOW_LEVEL, __VA_ARGS__)
    #endif

    #if defined(INFRA_LOG_MUTE_DBG)
        #define log_debug(mod, ...)
    #else
        #define log_debug(mod, ...)         LITE_syslog(mod, __FUNCTION__, __LINE__, LOG_DEBUG_LEVEL, __VA_ARGS__)
    #endif

    #if defined(INFRA_LOG_MUTE_INF)
        #define log_info(mod, ...)
    #else
        #define log_info(mod, ...)          LITE_syslog(mod, __FUNCTION__, __LINE__, LOG_INFO_LEVEL, __VA_ARGS__)
    #endif

    #if defined(INFRA_LOG_MUTE_WRN)
        #define log_warning(mod, ...)
    #else
        #define log_warning(mod, ...)       LITE_syslog(mod, __FUNCTION__, __LINE__, LOG_WARNING_LEVEL, __VA_ARGS__)
    #endif

    #if defined(INFRA_LOG_MUTE_ERR)
        #define log_err(mod, ...)
    #else
        #define log_err(mod, ...)           LITE_syslog(mod, __FUNCTION__, __LINE__, LOG_ERR_LEVEL, __VA_ARGS__)
    #endif

    #if defined(INFRA_LOG_MUTE_CRT)
        #define log_crit(mod, ...)
    #else
        #define log_crit(mod, ...)          LITE_syslog(mod, __FUNCTION__, __LINE__, LOG_CRIT_LEVEL, __VA_ARGS__)
    #endif
#else   /* #if defined(INFRA_LOG) */

    #define log_flow(mod, ...)
    #define log_debug(mod, ...)
    #define log_info(mod, ...)
    #define log_warning(mod, ...)
    #define log_err(mod, ...)
    #define log_crit(mod, ...)

#endif

int     log_multi_line_internal(const char *f, const int l,
                                const char *title, int level, char *payload, const char *mark);
#define log_multi_line(level, title, fmt, payload, mark) \
    log_multi_line_internal(__func__, __LINE__, title, level, payload, mark)

void    LITE_rich_hexdump(const char *f, const int l,
                          const int level,
                          const char *buf_str,
                          const void *buf_ptr,
                          const int buf_len
                         );

#define HEXDUMP_DEBUG(buf, len) \
    LITE_rich_hexdump(__func__, __LINE__, LOG_DEBUG_LEVEL, #buf, (const void *)buf, (const int)len)

#define HEXDUMP_INFO(buf, len)      \
    LITE_rich_hexdump(__func__, __LINE__, LOG_INFO_LEVEL, #buf, (const void *)buf, (const int)len)

int     iotx_facility_json_print(const char *str, int level, ...);

#endif

