#include "main.h"
#include "cmsis_os.h"
#include "sys_arch.h"
#include "ethernetif.h"


extern void iperf_server_init(void);
extern __lwip_dev lwipdev;
void application_entry(void *arg)
{

    iperf_server_init();
    while(1)
    {
        printf("hello world!\r\n");
        tos_task_delay(100000);
    }
}

