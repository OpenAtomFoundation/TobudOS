/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2020-01-09 19:25:05
 * @LastEditTime : 2020-01-10 08:54:24
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#include <stdlib.h>
#include "random.h"
#include "platform_timer.h"
#include "platform_memory.h"

static unsigned int last_seed = 1;

static int do_random(unsigned int seed)
{
    srand(seed);
    return rand();
}

int random_number(void)
{
    unsigned int seed = (unsigned int) platform_timer_now();
    last_seed += (seed >> ((seed ^ last_seed) % 3));
    return do_random(last_seed ^ seed);
}

char *random_string(unsigned int len)
{
    unsigned int i, flag, seed, random;

    char *str = platform_memory_alloc((size_t)(len + 1));
    if (NULL == str)
        return NULL;
    
    seed = (unsigned int) random_number();
    seed += (unsigned int) ((size_t)str ^ seed);
    
    random = (unsigned int)do_random(seed);
    
	for (i = 0; i < len; i++) {
        random = do_random(seed ^ random);
		flag = (unsigned int)random % 3;
		switch (flag) {
            case 0:
                str[i] = 'A' + do_random(random ^ (i & flag)) % 26;
                break;
            case 1:
                str[i] = 'a' + do_random(random ^ (i & flag)) % 26;
                break;
            case 2:
                str[i] = '0' + do_random(random ^ (i & flag)) % 10;
                break;
            default:
                str[i] = 'x';
                break;
		}
        random += ((0xb433e5c6 ^ random)  << (i & flag));
	}

    str[len] = '\0';
	return str;
}

