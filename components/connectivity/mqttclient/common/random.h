/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2020-01-09 19:59:19
 * @LastEditTime: 2020-10-17 14:18:15
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */

#ifndef _RANDOM_H_
#define _RANDOM_H_

#ifdef __cplusplus
extern "C" {
#endif

#define RANDOM_MAX 0x7FFFFFFF

int random_number(void);
int random_number_range(unsigned int min, unsigned int max);
char *random_string(unsigned int len);

#ifdef __cplusplus
}
#endif

#endif /* _RANDOM_H_ */
