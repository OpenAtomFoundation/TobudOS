#include "usart.h"
#include "fsl_swm.h"
/* Defines ------------------------------------------------------------------*/
#define DEBUG_USART         USART1
#define DEMO_USART_CLKSRC   kCLOCK_CoreSysClk
#define DEMO_USART_CLK_FREQ CLOCK_GetFreq(kCLOCK_CoreSysClk)

/* Typedefs -----------------------------------------------------------------*/
/* Macros -------------------------------------------------------------------*/
/* Local variables ----------------------------------------------------------*/
/* Extern variables ---------------------------------------------------------*/
/* Global variables ---------------------------------------------------------*/
/* Private function prototypes ----------------------------------------------*/
static void USART1_Pins_Init(void)
{
    SWM_Type* pSwm = SWM0;
    
    CLOCK_EnableClock(kCLOCK_Swm);
    CLOCK_EnableClock(kCLOCK_Uart1);

    CLOCK_SetClkDivider(kCLOCK_DivUsartClk, 1);
    
	/* Connect the TXD_O and RXD_I signals to port pins(P0.4, P0.0) */
    SWM_SetFixedPinSelect(pSwm, kSWM_ACMP_INPUT1, false);
    SWM_SetFixedPinSelect(pSwm, kSWM_ADC_CHN11, false);

    SWM_SetMovablePinSelect(pSwm, kSWM_USART1_TXD, kSWM_PortPin_P0_4);
    SWM_SetMovablePinSelect(pSwm, kSWM_USART1_RXD, kSWM_PortPin_P0_0);

    CLOCK_DisableClock(kCLOCK_Swm);
}
/* Public functions ---------------------------------------------------------*/
void Debug_USART1_Init(void)
{
    usart_config_t usartConfig;

    USART1_Pins_Init();

    USART_GetDefaultConfig(&usartConfig);
    
    usartConfig.baudRate_Bps = 115200U;
    usartConfig.enableTx = true;
    usartConfig.enableRx = true;
    USART_Init(DEBUG_USART, &usartConfig, DEMO_USART_CLK_FREQ);
}
/* Private functions --------------------------------------------------------*/
/* define fputc */
#if defined ( __CC_ARM ) || defined ( __ICCARM__ )  /* KEIL and IAR: printf will call fputc to print */
int fputc(int ch, FILE *f)
{
    USART_WriteBlocking(DEBUG_USART, (uint8_t *)&ch, 1);
    return ch;
}
#elif defined ( __GNUC__ )  /* GCC: printf will call _write to print */
__attribute__((used)) int _write(int fd, char *ptr, int len)
{
    USART_WriteBlocking(DEBUG_USART, (uint8_t *)ptr, len);
    return len;
}
#endif

