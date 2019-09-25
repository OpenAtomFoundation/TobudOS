/**************************************************************************
 * File Name                   : utils.h 
 * Author                      : 
 * Version                     : 1.0
 * Date                        : 
 * Description                 : 
 *
 * Copyright (c) 2014 Winner Microelectronics Co., Ltd. 
 * All rights reserved.
 *
 ***************************************************************************/
#ifndef UTILS_H
#define UTILS_H

int chk_crc8(u8 *ptr, u32 len);
u8 get_crc8(u8 *ptr, u32 len);
u8 calculate_crc8(u8 crc8, u8 *ptr, u32 len);
u32 get_crc32(u8 *data, u32 data_size);
u32 checksum(u32 *data, u32 length, u32 init);
int atodec(char ch);
int strtodec(int *dec, char *str);
int atohex(char ch);
int strtohex(u32 *hex, char *str);
int strtohexarray(u8 array[], int cnt, char *str);
int strtoip(u32 *ipadr, char * str);
void iptostr(u32 ip, char *str);
void mactostr(u8 mac[], char *str);

int hex_to_digit(int c);
int digit_to_hex(int c);
int hexstr_to_unit(char *buf, u32 *d);
int string_to_uint(char *buf, u32 *d);
int string_to_ipaddr(const char *buf, u8 *addr);
char * strdup(const char *s);
char * strndup(const char *s, size_t len);
	
#endif /* UTILS_H */
