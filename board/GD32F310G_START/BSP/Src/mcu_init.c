#include "mcu_init.h"

#define USART_CONSOLE (USART0)
int fputc(int ch, FILE *f)
{
  if (ch == '\n') {
    while(RESET == usart_flag_get(USART_CONSOLE, USART_FLAG_TBE));
    /* Auto complete return char */
    usart_data_transmit(USART_CONSOLE, '\r');
  }
  while(RESET == usart_flag_get(USART_CONSOLE, USART_FLAG_TBE));
  usart_data_transmit(USART_CONSOLE, ch);
  return ch;
}

int _write(int fd, char *ptr, int len)
{
    int i = 0;
    for (; i < len; i++)
    {
        if (*(ptr + i) == '\n') {
          while(RESET == usart_flag_get(USART_CONSOLE, USART_FLAG_TBE));
          /* Auto complete return char */
          usart_data_transmit(USART_CONSOLE, '\r');
        }
        while(RESET == usart_flag_get(USART_CONSOLE, USART_FLAG_TBE));
        usart_data_transmit(USART_CONSOLE, *(ptr + i));
    }
    return len;
}

void board_init(void)
{
    board_gpio_init();
    board_usart_init();
}
