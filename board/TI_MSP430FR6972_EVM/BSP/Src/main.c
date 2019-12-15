/****************************************************************************************
  TencentOS tiny Blink the LED Demo - Software Toggle P1.0 and P9.7

  Description; Toggle P1.0 and P9.7 inside of a software loop.
  ACLK = 32768Hz, MCLK = SMCLK = default DCO = 8MHz

                MSP430FR6972
             -----------------
         /|\|              XIN|-
          | |                 |  32768
          --|RST          XOUT|-
            |                 |
            |             P1.0|-->LED1
            |             P9.7|-->LED2
            |                 |
            |        P3.4(TXD)|-->  PC
            |        P3.5(RXD)|<--
  Built with IAR6.50.1
*****************************************************************************************/

#include <msp430.h>
#include <driverlib.h>
#include <stdint.h>
#include "tos.h"

#define TASK1_STK_SIZE		320 // more than 300 for stb printf
#define TASK2_STK_SIZE		320 // more than 300 for stb printf

#define TASK1_PRIO              3
#define TASK2_PRIO              3

k_task_t task1;
k_task_t task2;

__persistent k_stack_t task1_stack[TASK1_STK_SIZE];
__persistent k_stack_t task2_stack[TASK2_STK_SIZE];


void task1_entry(void *arg);
void task2_entry(void *arg);


/* EUSCI-UART-UCA1 115200 @ 8M */
EUSCI_A_UART_initParam uart_config = 
{
    EUSCI_A_UART_CLOCKSOURCE_SMCLK, // SMCLK Clock Source
    
    4,                             // BRDIV 
    5,                             // UCxBRF 
    85,                            // UCxBRS 

    EUSCI_A_UART_NO_PARITY,        // No Parity
    EUSCI_A_UART_LSB_FIRST,        // MSB First
    EUSCI_A_UART_ONE_STOP_BIT,     // One stop bit
    EUSCI_A_UART_MODE,             // UART mode
    EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION // Oversampling Baudrate
};

/**
 * @brief   systick_handler
 * @details rtos tick handle
 * @param   None
 * @return  None
 */
void systick_handler(void)
{
  if(tos_knl_is_running())					
  {
      tos_knl_irq_enter();					
      tos_tick_handler();       				            
      tos_knl_irq_leave();       	 			
  }
}

/**
 * @brief   board_init
 * @details initlize the board
 * @param   None
 * @return  None
 */
void board_init(void)
{
    WDT_A_hold(WDT_A_BASE);

    /* Set PJ.4 and PJ.5 for LFXT. */
    GPIO_setAsPeripheralModuleFunctionInputPin(  GPIO_PORT_PJ, GPIO_PIN4 + GPIO_PIN5, GPIO_PRIMARY_MODULE_FUNCTION  );

    /* Set DCO frequency to 8 MHz. */
    CS_setDCOFreq( CS_DCORSEL_0, CS_DCOFSEL_6 );

    /* Set external clock frequency to 32.768 KHz. */
    CS_setExternalClockSource( 32768, 0 );

    /* Set ACLK = LFXT. */
    CS_initClockSignal( CS_ACLK, CS_LFXTCLK_SELECT, CS_CLOCK_DIVIDER_1 );

    /* Set SMCLK = DCO with frequency divider of 1. */
    CS_initClockSignal( CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1 );

    /* Set MCLK = DCO with frequency divider of 1. */
    CS_initClockSignal( CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1 );

    /* Start XT1 with no time out. */
    CS_turnOnLFXT( CS_LFXT_DRIVE_0 );

    /* Disable the GPIO power-on default high-impedance mode. */
    PMM_unlockLPM5();
        
    /* Set P1.0(LED1) to output direction */
    GPIO_setAsOutputPin(GPIO_PORT_P1,GPIO_PIN0);

    /* SET P9.7(LED2) to output direction */
    GPIO_setAsOutputPin(GPIO_PORT_P9,GPIO_PIN7);

    /* Configure P3.4 - UCA1TXD and P3.5 - UCA1RXD. */
    GPIO_setOutputLowOnPin( GPIO_PORT_P3, GPIO_PIN4 );
    GPIO_setAsOutputPin( GPIO_PORT_P3, GPIO_PIN4 );
    GPIO_setAsPeripheralModuleFunctionInputPin( GPIO_PORT_P3, GPIO_PIN5, GPIO_PRIMARY_MODULE_FUNCTION );
    GPIO_setAsPeripheralModuleFunctionOutputPin( GPIO_PORT_P3, GPIO_PIN4, GPIO_PRIMARY_MODULE_FUNCTION );
    
    EUSCI_A_UART_init(EUSCI_A1_BASE, &uart_config);
    EUSCI_A_UART_enable(EUSCI_A1_BASE);
    //EUSCI_A_UART_enableInterrupt(EUSCI_A1_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
}

/**
 * @brief   putchar
 * @details redefine printf to uart
 * @param   int ch
 * @return  ch
 */
int putchar(int ch)
{  
    EUSCI_A_UART_transmitData(EUSCI_A1_BASE, ch);

    return (ch);
}

/**
 * @brief   task1_entry
 * @details task1 process
 * @param   void *arg
 * @return  None
 */
void task1_entry(void *arg)
{
    int count = 1;
    while (1) 
    {
        printf("###This is task1, %d\r\n", count++);
        
        GPIO_toggleOutputOnPin(GPIO_PORT_P1,GPIO_PIN0);

        tos_task_delay(tos_millisec2tick(2000));
    }
}

/**
 * @brief   task2_entry
 * @details task2 process
 * @param   void *arg
 * @return  None
 */
void task2_entry(void *arg)
{
    int count = 1;
    while (1) 
    {      
        printf("***This is task2, %d\r\n", count++);

        GPIO_toggleOutputOnPin(GPIO_PORT_P9,GPIO_PIN7);
  
        tos_task_delay(tos_millisec2tick(1000));
    }
}

int main(void)
{
    k_err_t err;
     
    board_init();
    
    printf("Welcome to TencentOS tiny\r\n");
    
    tos_knl_init();

    err = tos_task_create(&task1, "task1", task1_entry,
                            K_NULL, TASK1_PRIO,
                            task1_stack, sizeof(task1_stack),0);
    err = tos_task_create(&task2, "task2", task2_entry,
                            K_NULL, TASK2_PRIO,
                            task2_stack, sizeof(task2_stack),0);         
    if( err == K_ERR_NONE )
    {
        err = tos_knl_start();
    }
    else
    {
        printf("TencentOS tiny fail to creat tasks \r\n");
        
        while(1);
    }
}

