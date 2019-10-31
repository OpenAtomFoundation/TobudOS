#include "infra_config.h"

extern void **LITE_get_mem_mutex(void);
extern void *HAL_MutexCreate(void);
extern void HAL_MutexDestroy(void *);

#include <string.h>
#include <stdarg.h>
#include "infra_compat.h"
#include "infra_log.h"
#if defined(INFRA_CJSON)
    #include "infra_cjson.h"
#endif

#if defined(INFRA_LOG) && !defined(INFRA_LOG_ALL_MUTED)
static log_client logcb = {
    .name       = "linkkit",
    .priority   = LOG_DEBUG_LEVEL,
    .text_buf   = {0}
};

static char *lvl_names[] = {
    "non", "crt", "err", "wrn", "inf", "dbg", "flw"
};

/* 31, red. 32, green. 33, yellow. 34, blue. 35, magenta. 36, cyan. 37, white. */
char *lvl_color[] = {
    "[0m", "[1;31m", "[1;31m", "[1;35m", "[1;33m", "[1;36m", "[1;37m"
};

void LITE_syslog_routine(char *m, const char *f, const int l, const int level, const char *fmt, va_list *params)
{
    char       *tmpbuf = logcb.text_buf;
    char       *o = tmpbuf;
    int         truncated = 0;

    if (LITE_get_loglevel() < level || level < LOG_NONE_LEVEL) {
        return;
    }

#if !defined(_WIN32)
    LITE_printf("%s%s", "\033", lvl_color[level]);
    LITE_printf(LOG_PREFIX_FMT, lvl_names[level], f, l);
#endif  /* #if !defined(_WIN32) */

    memset(tmpbuf, 0, sizeof(logcb.text_buf));

    o = tmpbuf;
    o += LITE_vsnprintf(o, LOG_MSG_MAXLEN + 1, fmt, *params);

    if (o - tmpbuf > LOG_MSG_MAXLEN) {
        o = tmpbuf + strlen(tmpbuf);
        truncated = 1;
    }
    if (strlen(tmpbuf) == LOG_MSG_MAXLEN) {
        truncated = 1;
    }

    LITE_printf("%s", tmpbuf);
    if (truncated) {
        LITE_printf(" ...");
    }

    if (tmpbuf[strlen(tmpbuf) - 1] != '\n') {
        LITE_printf("\r\n");
    }

#if !defined(_WIN32)
    LITE_printf("%s", "\033[0m");
#endif  /* #if !defined(_WIN32) */
    return;
}

void LITE_syslog(char *m, const char *f, const int l, const int level, const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    LITE_syslog_routine(m, f, l, level, fmt, &ap);
    va_end(ap);
}

int LITE_get_loglevel(void)
{
    return logcb.priority;
}

void LITE_set_loglevel(int pri)
{
    void **mutex = NULL;
    logcb.priority = pri;

#if WITH_MEM_STATS
    mutex = LITE_get_mem_mutex();
    if (pri != LOG_NONE_LEVEL) {
        if (*mutex == NULL) {
            *mutex = HAL_MutexCreate();
            if (*mutex == NULL) {
                LITE_printf("\nCreate memStats mutex error\n");
            }
        }
    } else if (*mutex != NULL) {
        HAL_MutexDestroy(*mutex);
        *mutex = NULL;
    }
#endif
}

void LITE_rich_hexdump(const char *f, const int l,
                       const int level,
                       const char *buf_str,
                       const void *buf_ptr,
                       const int buf_len)
{
    if (LITE_get_loglevel() < level) {
        return;
    }

    LITE_printf("%s%s", "\033", lvl_color[level]);
    LITE_printf(LOG_PREFIX_FMT, lvl_names[level], f, l);
    LITE_printf("HEXDUMP %s @ %p[%d]\r\n", buf_str, buf_ptr, buf_len);
    LITE_hexdump(buf_str, buf_ptr, buf_len);

    LITE_printf("%s", "\033[0m");
    return;
}

int log_multi_line_internal(const char *f, const int l,
                            const char *title, int level, char *payload, const char *mark)
{
    const char     *pos;
    const char     *endl;
    int             i;

    if (LITE_get_loglevel() < level) {
        return 1;
    }

    LITE_printf("[%s] %s(%d): %s (Length: %d Bytes)\r\n",
                lvl_names[LITE_get_loglevel()], f, l, title, (int)strlen(payload));

    pos = payload;
    while (pos && *pos) {
        LITE_printf("%s ", mark);

        if (*pos == '\r') {
            LITE_printf("\r\n");
            pos += 2;
            continue;
        }

        endl = strchr(pos + 1, '\r');
        if (endl == NULL) {
            endl = pos;
            do {
                ++endl;
            } while (*endl);
        }

        for (i = 0; i < endl - pos; ++i) {
            LITE_printf("%c", pos[i]);
        }
        LITE_printf("\r\n");

        pos = *endl ? endl + 2 : 0;
    }

    return 0;
}

#define LITE_HEXDUMP_DRAWLINE(start_mark, len, end_mark)    \
    do { \
        int                     i; \
        \
        LITE_printf("%s", start_mark); \
        for(i = 0; i < len; ++i) { LITE_printf("-"); } \
        LITE_printf("%s", end_mark); \
        LITE_printf("\r\n"); \
        \
    } while(0)

int LITE_hexdump(const char *title, const void *buff, const int len)
{
    int                     i, j, written;
    unsigned char           ascii[20] = {0};
    char                    header[64] = {0};
    unsigned char          *buf = (unsigned char *)buff;

    LITE_snprintf(header, sizeof(header), "| %s: (len=%d) |\r\n", title, (int)len);

    LITE_HEXDUMP_DRAWLINE("+", strlen(header) - 4, "+");
    LITE_printf("%s", header);
    LITE_printf("%s\r\n", HEXDUMP_SEP_LINE);

    written = 0;
    for (i = 0; i < len; ++ i) {
        if (i % 16 == 0) {
            LITE_printf("| %08X: ", (unsigned int)(i + (long)buff));
            written += 8;
        }

        LITE_printf("%02X", buf[i]);
        written += 2;

        if (i % 2 == 1) {
            LITE_printf(" ");
            written += 1;
        }
        LITE_snprintf((char *)ascii + i % 16, (1 + 1), "%c", ((buf[i] >= ' ' && buf[i] <= '~') ?  buf[i] : '.'));

        if (((i + 1) % 16 == 0) || (i == len - 1)) {
            for (j = 0; j < 48 - written; ++j) {
                LITE_printf(" ");
            }

            LITE_printf(" %s", ascii);
            LITE_printf("\r\n");

            written = 0;
            memset(ascii, 0, sizeof(ascii));
        }
    }
    LITE_printf("%s\r\n", HEXDUMP_SEP_LINE);

    return 0;
}

void IOT_SetLogLevel(IOT_LogLevel level)
{
    int             lvl = (int)level;

    if (lvl > LOG_DEBUG_LEVEL) {
        HAL_Printf("Invalid input level: %d out of [%d, %d]", level,
                   LOG_NONE_LEVEL,
                   LOG_DEBUG_LEVEL);
        return;
    }

    LITE_set_loglevel(lvl);
    HAL_Printf("[prt] log level set as: [ %d ]\r\n", lvl);
}

#else

void IOT_SetLogLevel(IOT_LogLevel level)
{
    return;
}

int log_multi_line_internal(const char *f, const int l,
                            const char *title, int level, char *payload, const char *mark)
{
    return 0;
}

void LITE_rich_hexdump(const char *f, const int l,
                       const int level,
                       const char *buf_str,
                       const void *buf_ptr,
                       const int buf_len)
{
    return;
}

#endif  /* #if defined(INFRA_LOG) && !defined(INFRA_LOG_ALL_MUTED) */

