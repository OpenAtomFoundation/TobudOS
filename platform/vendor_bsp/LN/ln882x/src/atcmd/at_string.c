#include <stdio.h>
#include <string.h>
#include "atcmd/at_string.h"

uint16_t at_strlen(char * string)
{
    uint16_t i;
    for(i = 0; *string; i++, string++);
    return i;
}

void at_shift_array(char *array, uint16_t len)
{
    int i;
    for(i = 0; i < len - 1; i++)
    {
        array[i] = array[i + 1];
    }
}

char at_str_equal(char * string1, uint16_t len1, char * string2, uint16_t len2)
{
    int i;

    for(i = 0; i < len2; i++)
    {
        if(string1[len1 - len2 + i] != string2[i])
        {
            return 0;
        }
    }

    return 1;
}

int16_t at_str_find(char * haystack, char * needle)
{
    int i;
    uint16_t needle_len = at_strlen(needle);
    uint16_t haystack_len = at_strlen(haystack);

    if(needle_len > haystack_len)
    {
        return -1;
    }

    for(i = 0; i < haystack_len - needle_len; i++)
    {
        if(at_str_equal(haystack, needle_len, needle, needle_len))
        {
            return i;
        }

        haystack++;
    }

    return -1;
}


AT_CMD_TYPE at_check_cmdtype(char * line)
{
    uint8_t len = at_strlen(line);
    if(len >= 2)
    {
        if((len >= 3) && !strncmp(line, "AT+", strlen("AT+")))
            return AT_PLUS_CMD;
        else if(!strncmp(line, "AT", strlen("AT")))
            return AT_SHORT_CMD;
        else
            return AT_CMD_INVALID;
    }
    return AT_CMD_INVALID;
}


void at_dump_array(char *array, uint16_t max_buffer, uint16_t reverse_pointer)
{
    int i;
    for(i = 0; i < max_buffer - reverse_pointer; i++)
    {
        printf("[%c]", array[reverse_pointer + i]);
    }
}

uint16_t at_power(uint16_t base, uint16_t exponent)
{
    uint16_t ret = 1;
    int i;
    for(i = 0; i < exponent; i++)
    {
        ret *= base;
    }
    return ret;
}

uint16_t at_array_to_number(char * array, uint16_t max_buffer, uint16_t reverse_pointer)
{
    int i;

    uint16_t ret = 0;

    for(i = 0; i < max_buffer - reverse_pointer; i++)
    {
        ret +=  (array[reverse_pointer + i] - '0') * at_power(10, max_buffer - reverse_pointer - i - 1);
    }

    return ret;
}

void at_array_slice_to_string(char * array, uint16_t start, uint16_t end, char *ret)
{
    uint16_t i, n = 0;

    for(i = start; i <= end; i++)
    {
        ret[n] = array[i];
        n++;
    }

    ret[n] = 0;
    ret[++n] = 0;
}

void str_remove_cr_lf(char * str)
{
    while(*str) 
    {
        if((*str == '\r') || (*str == '\n')){
            *str = '\0';
            break;
        }
        str++;
    }
}

