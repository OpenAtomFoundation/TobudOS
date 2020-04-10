#ifndef AT_LIST_H
#define AT_LIST_H

#include "types.h"

typedef char (*at_callback)(char *value);


typedef struct _at_command
{
    unsigned long hash;
    at_callback setter;
    at_callback getter;
    at_callback test;
    at_callback execute;
} AT_COMMAND;

#ifndef AT_PLUS_CMD_NUM
#define AT_PLUS_CMD_NUM 80

#endif

#ifndef AT_SHORT_CMD_NUM
#define AT_SHORT_CMD_NUM 3
#endif



void at_cmd_init(void);




#endif

