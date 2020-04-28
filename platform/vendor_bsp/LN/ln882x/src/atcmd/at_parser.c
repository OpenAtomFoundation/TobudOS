#include "atcmd/at_parser.h"



AT_COMMAND at_registered_commands[AT_PLUS_CMD_NUM];
AT_COMMAND at_registered_short_commands[AT_SHORT_CMD_NUM];


unsigned long at_hash(char *str)
{
    unsigned long hash = 5381;
    int c = *str;

    while(c) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
        str++;
        c = *str;
    }

    return hash;
}

void at_register_command(char * command, at_callback getter, at_callback setter, at_callback test, at_callback execute)
{
    int i;
    AT_COMMAND new_cmd;
    new_cmd.hash = at_hash(command);
    new_cmd.getter = getter;
    new_cmd.setter = setter;
    new_cmd.execute = execute;
    new_cmd.test = test;

    for(i = 0; i < AT_PLUS_CMD_NUM; i++) 
    {
        if(at_registered_commands[i].hash == 0) {
            at_registered_commands[i] = new_cmd;
            return;
        }
    }
}


void at_register_short_command(char * command, at_callback getter, at_callback setter, at_callback test, at_callback execute)
{
    int i;
    AT_COMMAND new_cmd;
    new_cmd.hash = at_hash(command);
    new_cmd.getter = getter;
    new_cmd.setter = setter;
    new_cmd.execute = execute;
    new_cmd.test = test;

    for(i = 0; i < AT_SHORT_CMD_NUM; i++) 
    {
        if(at_registered_short_commands[i].hash == 0) {
            at_registered_short_commands[i] = new_cmd;
            return;
        }
    }
}


char at_execute_command(char * command, char *value, char type)
{
    int i;

    char result = AT_ERROR;

    unsigned long hash = at_hash(command);

    for(i = 0; i < AT_PLUS_CMD_NUM; i++) 
    {
        if(at_registered_commands[i].hash == hash) 
        {
            switch(type) 
            {
                case AT_PARSER_STATE_WRITE:
                    if(at_registered_commands[i].setter && at_registered_commands[i].setter(value) == AT_OK) {
                        result = AT_OK;
                    }
                    break;
                case AT_PARSER_STATE_READ:
                    if(at_registered_commands[i].getter && at_registered_commands[i].getter(value) == AT_OK) {
                        result = AT_OK;
                    }
                    break;
                case AT_PARSER_STATE_TEST:
                    if(at_registered_commands[i].test && at_registered_commands[i].test(value) == AT_OK) {
                        result = AT_OK;
                    }
                    break;
                case AT_PARSER_STATE_COMMAND:
                    if(at_registered_commands[i].execute && at_registered_commands[i].execute(value) == AT_OK) {
                        result = AT_OK;
                    }
                    break;
                default:
                    result = AT_ERROR;
            }
            return result;
        }
    }

    return result;
}

/*
   AT+COMMAND=?     -> List
   AT+COMMAND=VALUE -> Write
   AT+COMMAND?      -> Read
   AT+COMMAND       -> Execute
*/

char at_parse_plus_cmd(char * line)
{
    uint16_t i;
    char *value = NULL;
    char result = AT_ERROR;

    char state = AT_PARSER_STATE_COMMAND;

    int16_t start             = at_str_find(line, (char *)AT_COMMAND_MARKER);
    uint16_t line_len         = at_strlen(line);
    int16_t index_write_start = -1;
    int16_t index_command_end = line_len - 1;

    char temp[AT_MAX_TEMP_STRING];

    if(start >= 0) 
    {
        // Skip the marker
        start += at_strlen((char *)AT_COMMAND_MARKER);

        for(i = start; i < line_len; i++) 
        {
            // Execute 'read' command
            if(line[i] == '?' && state == AT_PARSER_STATE_COMMAND) 
            {
                index_command_end = i - 1;
                state = AT_PARSER_STATE_READ;
                break;
            } 
            else if(line[i] == '=' && state == AT_PARSER_STATE_COMMAND) 
            {
                index_command_end = i - 1;

                if(i < (line_len - 1)) 
                {
                    if(line[i + 1] == '?') {
                        state = AT_PARSER_STATE_TEST;
                    } else {
                        index_write_start = i + 1;
                        value = &line[index_write_start];
                        state = AT_PARSER_STATE_WRITE;
                    }
                    break;
                } 
                else 
                {
                    at_printf("\r\nAT command error or no this command(%s)\r\n", temp);
                    return AT_ERROR;
                }
            }
        }

        switch(state) 
        {
            case AT_PARSER_STATE_COMMAND:
            case AT_PARSER_STATE_READ:
            case AT_PARSER_STATE_TEST:
                at_array_slice_to_string(line, start, index_command_end, temp);
                result = at_execute_command(temp, value, state);
                if(result == AT_ERROR){
                    at_printf("\r\nAT command error or no this command(%s)\r\n", temp);
                }
                break;

            case AT_PARSER_STATE_WRITE:
                at_array_slice_to_string(line, start, index_command_end, temp);
                if(index_write_start <= (line_len - 1)) {
                    result = at_execute_command(temp, value, state);
                } else {
                    result = at_execute_command(temp, value, state);
                }
                
                if(result == AT_ERROR){
                    at_printf("\r\nAT command error or no this command(%s)\r\n", temp);
                }
                
                break;

            default:
                at_printf("\r\nAT command error or no this command(%s)\r\n", temp);
                return AT_ERROR;
        }
    }
    return result;
}

char at_parse_short_cmd(char * line)
{
    char result = AT_ERROR;
    uint8_t i;   
    uint32_t hash_code = 0;
    
    hash_code = at_hash(line);

    for(i = 0; i < AT_SHORT_CMD_NUM; i++) 
    {
        if(at_registered_short_commands[i].hash == hash_code) 
        {
            if(at_registered_short_commands[i].execute == NULL) {
                result = AT_ERROR;
            }
            
            result = at_registered_short_commands[i].execute(line);
            
            if(result == AT_ERROR){
                at_printf("\r\nAT command error or no this command(%s)\r\n", line);
            }
            break;
        }
    }

    return result;
}


