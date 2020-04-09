#ifndef __GBK_TO_UNICODE_H__
#define __GBK_TO_UNICODE_H__

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ZH_CN_WHOLE_SET_SUPPORT                                 (0)

bool is_str_gbk(const char* str);
bool is_str_big5(const char* str);
bool is_str_utf8(const char* str);
int encode_gbk_to_utf8(unsigned char *gbk_str, unsigned char *utf8, int utf8_len_max);
int encode_gbk_to_big5(unsigned char *gbk_str, unsigned char *big5, int big5_len_max);
int encode_utf8_to_big5(unsigned char *utf8_str, unsigned char *big5);
#ifdef __cplusplus
}
#endif

#endif /* __GBK_TO_UNICODE_H__ */
