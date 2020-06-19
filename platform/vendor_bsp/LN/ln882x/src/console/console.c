#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "types.h"
#include "proj_config.h"
#include "ln88xx.h"
#include "utils/debug/log.h"
#include "utils/debug/art_assert.h"
#include "hal/hal_gpio.h"

#if (CHIP_ROLE == CHIP_MCU)

#include "serial/serial.h"
#include "console/console.h"
#include "atcmd/at_task.h"

Serial_t       m_ConsoleSerial;
CONSOLE_CTRL_T console_ctrl = {0,};

#ifdef __CONFIG_OS_KERNEL
  #define CONSOLE_THREAD_STACK_SIZE    (512)
  static OS_Thread_t    g_console_thread = {0,};
  static OS_Semaphore_t g_rx_sem;
  extern OS_Queue_t     g_at_msg_queue;
#endif /* __CONFIG_OS_KERNEL */

#define CONSOLE_PORT_BAUDRATE  CFG_UART_BAUDRATE_CONSOLE

int command_stdio_write(char *buf, size_t size)
{
    int ret = 0;
    ret = serial_write(&m_ConsoleSerial, (const void *)buf, size);
    return ret;
}

void console_echo_enable(bool en)
{
    CONSOLE_CTRL_T *console = &console_ctrl;
    console->echo = en;
}

void console_exe_command(char *cmd, unsigned int cmd_len)
{
    CONSOLE_CTRL_T *console = &console_ctrl;
    AT_MSG_T msg = {0,};

    ART_ASSERT(cmd);
    msg.len = cmd_len;
    msg.pBuffer = cmd;
#ifdef __CONFIG_OS_KERNEL
    msg.sem = &console->sem;
    OS_QueueSend(&g_at_msg_queue, &msg, OS_WAIT_FOREVER);
    OS_SemaphoreWait(msg.sem, OS_WAIT_FOREVER);
#endif
}

static void lookup_cmd_line(CONSOLE_CTRL_T *console, Serial_t *fd, uint8_t ch)
{
    ART_ASSERT(console && fd);
    //if(isprint(ch)) //normal character  Some chinese characters will be blocked
    if(ch != '\n' && ch != '\r')
    {
        if(console->echo){
            serial_write(fd, &ch, sizeof(char));
        }

        //put this char into console_ctrl.console_buffer
        console->console_cmd_buffer[console->index++] = ch;
        if(console->index >= CMD_CBSIZE){
            memset(console->console_cmd_buffer, 0, CMD_CBSIZE);
            console->index = 0;
            LOG(LOG_LVL_ERROR, "[%s, %d]console_cmd_buffer overflow.\r\n", __func__, __LINE__);
        }
    }
    else //if(ch == '\n' || ch == '\r') // for '\r' '\n'
    {
        if(console->echo){
            serial_write(fd, &ch, sizeof(char));
        }

        //put this char into console_ctrl.console_buffer
        console->console_cmd_buffer[console->index++] = ch;
        if(console->index >= CMD_CBSIZE)
        {
            memset(console->console_cmd_buffer, 0, CMD_PBSIZE);
            console->index = 0;
        }
        else
        {
            if(NULL != strstr(console->console_cmd_buffer, CMD_END_MARK_CRLF))
            {
                memset(console->cmd_line, 0, CMD_PBSIZE);
                memcpy(console->cmd_line, console->console_cmd_buffer, console->index);
                console_exe_command(console->cmd_line, console->index);

                memset(console->console_cmd_buffer, 0, CMD_CBSIZE);
                console->index = 0;
            }
        }
    }
}

#ifdef __CONFIG_OS_KERNEL
static void console_task(void *arg)
{
    Serial_t *port = NULL;
    int len = 0;
    uint8_t ch;

    ART_ASSERT(arg);

    port = (Serial_t *)arg;
    while (1)
    {
        if (OS_OK == OS_SemaphoreWait(&g_rx_sem, OS_WAIT_FOREVER))
        {
            while (!fifo_isempty(&port->rxfifo))
            {
                len = serial_read(port, &ch, 1);
                if(len > 0){
                    lookup_cmd_line(&console_ctrl, port, ch);
                }
            }
        }
    }
}

void serial_rx_callbcak(void){
    OS_SemaphoreRelease(&g_rx_sem);
}
#endif /*__CONFIG_OS_KERNEL */

void console_init(void)
{
    CONSOLE_CTRL_T  *console = &console_ctrl;
    Serial_t *fd = NULL;

#if WIFI_TRACK
    GPIO_InitTypeDef gpio_config = {
            .dir = GPIO_INPUT,
            .debounce = GPIO_DEBOUNCE_YES,
        };

    HAL_GPIO_Init(GPIOA_8, gpio_config);
    int bd = 115200;
    if(HAL_GPIO_ReadPin(GPIOA_8) == GPIO_VALUE_LOW)
        bd = CONSOLE_PORT_BAUDRATE;
    else
        bd = 9600;
    console->echo = false;
#else
    console->echo = true;
#endif

    console->index = 0;
    memset(console->console_cmd_buffer, 0, CMD_CBSIZE);

#ifdef __CONFIG_OS_KERNEL
    if(OS_SemaphoreCreate(&g_rx_sem, 0, 1024) != OS_OK) {
        LOG(LOG_LVL_ERROR, "[%s, %d]OS_SemaphoreCreate g_rx_sem fail.\r\n", __func__, __LINE__);
        return;
    };
#endif

    /*
     *  Init Serial.
     *  Console use SER_PORT_UART0/SER_PORT_UART1.
     */
    fd = &m_ConsoleSerial;

#if WIFI_TRACK
    serial_init(fd, SER_PORT_UART0, bd, serial_rx_callbcak);//SER_PORT_UART0
#elif (defined(AT_LOG_MERGE_TO_UART0) && (AT_LOG_MERGE_TO_UART0 == 1))
    serial_init(fd, SER_PORT_UART0, CONSOLE_PORT_BAUDRATE, serial_rx_callbcak);//SER_PORT_UART0
#else
    // normal mode: log--uart0, at--uart1.
    serial_init(fd, SER_PORT_UART1, CONSOLE_PORT_BAUDRATE, serial_rx_callbcak);//SER_PORT_UART0
#endif // !AT_LOG_MERGE_TO_UART0

#ifdef __CONFIG_OS_KERNEL
    /* start console task */
    if(OS_ThreadCreate(&g_console_thread, "Console_task",
                console_task,
                fd,
                OS_THREAD_PRIO_CONSOLE,
                CONSOLE_THREAD_STACK_SIZE) != OS_OK){
        LOG(LOG_LVL_ERROR, "[%s, %d]Create console task failed.\r\n", __func__, __LINE__);
        return ;
    }

    if (OS_SemaphoreCreateBinary(&console->sem) != OS_OK) {
        LOG(LOG_LVL_ERROR, "[%s, %d]OS_SemaphoreCreateBinary console->sem fail.\r\n", __func__, __LINE__);
        return;
    }
#endif
#if WIFI_TRACK==0
    serial_write(fd, "Console init ok!\r\n", strlen("Console init ok!\r\n"));
#endif
}
#endif

