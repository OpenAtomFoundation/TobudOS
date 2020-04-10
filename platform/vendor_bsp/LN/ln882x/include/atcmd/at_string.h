#ifndef __AT_STRING__
#define __AT_STRING__

#include "types.h"


typedef enum
{
    AT_SHORT_CMD=0,
    AT_PLUS_CMD,
    AT_CMD_INVALID
}AT_CMD_TYPE;


int16_t at_str_find(char *haystack, char *needle);
void at_array_slice_to_string(char *array, uint16_t start, uint16_t end, char *ret);
uint16_t at_strlen(char *string);
AT_CMD_TYPE at_check_cmdtype(char * line);
void str_remove_cr_lf(char * str);

#endif
