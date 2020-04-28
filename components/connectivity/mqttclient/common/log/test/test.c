/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-26 20:00:29
 * @LastEditTime : 2019-12-26 20:56:50
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#include "salof.h"
#include <stdio.h>
#include <unistd.h>

int main()
{
    salof_init();
    
    LOG_INFO("asfafasf");
    sleep(1);
    LOG_ERR("asfafasf");
    LOG_WARN("asfafasf");
    LOG_DEBUG("asfafasf\n");

    sleep(1);
}

