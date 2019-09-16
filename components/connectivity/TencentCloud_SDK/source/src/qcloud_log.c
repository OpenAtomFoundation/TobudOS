#include "qcloud.h"

static const char *level_str[] = {
    "DIS", "ERR", "WRN", "INF", "DBG"
};

static qcloud_log_handler_t sg_log_message_handler= NULL;

__QCLOUD_STATIC__ const char *file_path2name(const char *file_path)
{
    const char *suffix = NULL;

    suffix = strrchr(file_path, '\\');
    if (!suffix) {
        return file_path;
    }

    return ++suffix;
}

void qcloud_log_handler_set(qcloud_log_handler_t handler)
{
	sg_log_message_handler = handler;
}

void qcloud_log_write(const char *file, const char *func, const int line, const int level, const char *fmt, ...)
{
    /* format log content */
	const char *file_name = file_path2name(file);

	char sg_text_buf[QCLOUD_LOG_LEN_MAX + 1];
	char		*tmp_buf = sg_text_buf;
	char        *o = tmp_buf;
    memset(tmp_buf, 0, sizeof(sg_text_buf));

    o += osal_snprintf(o, sizeof(sg_text_buf), "%s|%s|%s|%s(%d): ", level_str[level], osal_timer_current(), file_name, func, line);

    va_list     ap;
    va_start(ap, fmt);
    o += vsnprintf(o, QCLOUD_LOG_LEN_MAX - 2 - strlen(tmp_buf), fmt, ap);
    va_end(ap);

    strcat(tmp_buf, "\r\n");

    /* customer defined log print handler */
	if (sg_log_message_handler != NULL && sg_log_message_handler(tmp_buf)) {
		return;
	}

    /* default log handler: print to console */
    osal_printf("%s", tmp_buf);

    return;
}

