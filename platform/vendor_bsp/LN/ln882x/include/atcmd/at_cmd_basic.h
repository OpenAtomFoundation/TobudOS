#ifndef AT_CMD_BASIC_H
#define AT_CMD_BASIC_H

#include "types.h"
#include "atcmd/at_list.h"


char at_at_excute(char *str);
char at_ate0_excute(char *str);
char at_ate1_excute(char *str);
char at_rst_excute(char *str);
char at_gmr_excute(char *str);
char at_gslp_excute(char *str);
char at_restore_excute(char *str);
char at_uart_cur_get(char *str);
char at_sleep_get(char *str);
char at_sleep_set(char *str);
char at_wakeupgpio_set(char *str);
char at_sysgpiodir_set(char *str);
char at_sysgpiowrite_set(char *str);
char at_sysgpioread_set(char *str);
char at_cal_excute(char *str);



#endif
