/*!
 * \file      sysIrqHandlers.c
 *
 * \brief     Default IRQ handlers
 *
 * \copyright Revised BSD License, see section \ref LICENSE.
 *
 * \code
 *                ______                              _
 *               / _____)             _              | |
 *              ( (____  _____ ____ _| |_ _____  ____| |__
 *               \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 *               _____) ) ____| | | || |_| ____( (___| | | |
 *              (______/|_____)_|_|_| \__)_____)\____)_| |_|
 *              (C)2013-2017 Semtech
 *
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 *
 * \author    Gregory Cristian ( Semtech )
 */

/*!
 * \brief  This function handles NMI exception.
 * \param  None
 * \retval None
 */
void NMI_Handler( void )
{
}

/*!
 * \brief  This function handles Hard Fault exception.
 * \param  None
 * \retval None
 */
#if defined( HARD_FAULT_HANDLER_ENABLED )
void HardFault_Handler_C( unsigned int *args )
{
    volatile unsigned int stacked_r0;
    volatile unsigned int stacked_r1;
    volatile unsigned int stacked_r2;
    volatile unsigned int stacked_r3;
    volatile unsigned int stacked_r12;
    volatile unsigned int stacked_lr;
    volatile unsigned int stacked_pc;
    volatile unsigned int stacked_psr;

    stacked_r0 = ( ( unsigned long) args[0] );
    stacked_r1 = ( ( unsigned long) args[1] );
    stacked_r2 = ( ( unsigned long) args[2] );
    stacked_r3 = ( ( unsigned long) args[3] );

    stacked_r12 = ( ( unsigned long) args[4] );
    stacked_lr = ( ( unsigned long) args[5] );
    stacked_pc = ( ( unsigned long) args[6] );
    stacked_psr = ( ( unsigned long) args[7] );

    ( void )stacked_r0;
    ( void )stacked_r1;
    ( void )stacked_r2;
    ( void )stacked_r3;

    ( void )stacked_r12;
    ( void )stacked_lr ;
    ( void )stacked_pc ;
    ( void )stacked_psr;

    while( 1 );
}

#if defined(__CC_ARM)
__asm void HardFault_Handler(void)
{
    TST LR, #4
    ITE EQ
    MRSEQ r0, MSP
    MRSNE r0, PSP
    B __cpp(HardFault_Handler_C)
}
#elif defined(__ICCARM__)
void HardFault_Handler(void)
{
    __asm("TST LR, #4");
    __asm("ITE EQ");
    __asm("MRSEQ r0, MSP");
    __asm("MRSNE r0, PSP");
    __asm("B HardFault_Handler_C");
}
#elif defined(__GNUC__)
void HardFault_Handler(void)
{
    __asm volatile( "TST LR, #4" );
    __asm volatile( "ITE EQ" );
    __asm volatile( "MRSEQ R0, MSP" );
    __asm volatile( "MRSNE R0, PSP" );
    __asm volatile( "B HardFault_Handler_C" );
}
#else
    #warning Not supported compiler type
#endif

#endif

/*!
 * \brief  This function handles Memory Manage exception.
 * \param  None
 * \retval None
 */
void MemManage_Handler( void )
{
    /* Go to infinite loop when Memory Manage exception occurs */
    while ( 1 )
    {
    }
}

/*!
 * \brief  This function handles Bus Fault exception.
 * \param  None
 * \retval None
 */
void BusFault_Handler( void )
{
    /* Go to infinite loop when Bus Fault exception occurs */
    while ( 1 )
    {
    }
}

/*!
 * \brief  This function handles Usage Fault exception.
 * \param  None
 * \retval None
 */
void UsageFault_Handler( void )
{
    /* Go to infinite loop when Usage Fault exception occurs */
    while ( 1 )
    {
    }
}

/*!
 * \brief  This function handles Debug Monitor exception.
 * \param  None
 * \retval None
 */
void DebugMon_Handler( void )
{
}
