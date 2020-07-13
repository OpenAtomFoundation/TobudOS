#ifndef __ART_STRING_H_
#define __ART_STRING_H_
#include <stdbool.h>

/***************************************************************
 *	函 数 名：substring
 *	函数功能：获得字符串中指定位置的子串
 *	参    数：
 *			  strSource：源字符串
 *            strDest：  子串的输出buffer
 *			  startPos：子串在源串的起始位置
 *			  endPos：  子串在源串的结束位置
 *	返 回 值：
 *			  如果成功，则返回true
 *			  如果失败，则返回false
***************************************************************/
extern bool substing(const char *strSource, char *strDest, const int startPos, const int endPos);
extern unsigned char char2hex(char c);
extern bool str2mac(const unsigned char *macstr, unsigned char *mac);
extern unsigned char *str2hex(unsigned char *dest, const char *src);
extern bool is_valid_mac_string(const char *macstr);
extern bool is_valid_mac(const char *mac);
extern bool art_string_extract_domain_content(char *domain_begin, char *domain_end, char domain_separator, char *flag, char *dest_buffer);
extern bool mem_is_zero(const void *addr, size_t size);
extern int generate_mac_randomly(unsigned char *addr);
#endif /* __ART_STRING_H_ */

