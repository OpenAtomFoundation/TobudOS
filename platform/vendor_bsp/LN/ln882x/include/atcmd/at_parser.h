#ifndef AT_PARSER_H
#define AT_PARSER_H

#include "atcmd/at_string.h"
#include "atcmd/at_list.h"
#include "console/console.h"
#define AT_MAX_TEMP_STRING	50

#define AT_OK 		0
#define AT_ERROR 	1

#define AT_PARSER_STATE_COMMAND 	0
#define AT_PARSER_STATE_TEST		1
#define AT_PARSER_STATE_READ		2
#define AT_PARSER_STATE_WRITE		3
#define AT_PARSER_STATE_EQUAL		4

#ifndef AT_COMMAND_MARKER
#define AT_COMMAND_MARKER "AT+"
#endif

#define at_printf console_printf

unsigned long at_hash(char * str);
void at_register_command(char * command, at_callback getter, at_callback setter, at_callback test, at_callback execute);
void at_register_short_command(char * command, at_callback getter, at_callback setter, at_callback test, at_callback execute);
char at_parse_plus_cmd(char * line);
char at_parse_short_cmd(char * line);



#endif
