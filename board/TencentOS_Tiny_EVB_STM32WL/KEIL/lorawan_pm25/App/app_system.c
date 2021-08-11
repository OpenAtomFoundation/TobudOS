#include <stdio.h>
#include <string.h>
#include "main.h"
#include "app_system.h"
#include "adc_if.h"

#include "stm32_systime.h"
#include "stm32_lpm.h"

#include "usart_if.h"
/**
  * @brief  Set all pins such to minimized consumption (necessary for some STM32 families)
  * @param none
  * @retval None
  */
static void Gpio_PreInit(void);
/**
  * @brief Returns sec and msec based on the systime in use
  * @param none
  * @retval  none
  */
static void TimestampNow(uint8_t *buff, uint16_t* size);

/* Exported functions ---------------------------------------------------------*/

/**
  * @brief initialises the system (dbg pins, trace, mbmux, systiemr, LPM, ...)
  * @param none
  * @retval  none
  */
void SystemApp_Init(void)
{
	Gpio_PreInit();

	vcom_Init(NULL);
    
    MX_USART2_UART_Init();

	/*Initialize the temperature and Battery measurement services */
	SYS_InitMeasurement();

#if LOW_POWER_MODE_ENBLE	
	/*Init low power manager*/
	UTIL_LPM_Init();
	/* Disable Stand-by mode */
	UTIL_LPM_SetOffMode(LPM_APPLI_Id, UTIL_LPM_DISABLE);

	#if defined (LPM_STOP_MODE_DISABLE) && (LPM_STOP_MODE_DISABLE == 1)
	/* Disable Stop Mode */
		UTIL_LPM_SetStopMode(LPM_APPLI_Id, UTIL_LPM_DISABLE);
	#elif !defined (LPM_STOP_MODE_DISABLE)
		#error LPM_STOP_MODE_DISABLE not defined
	#endif
#endif
}

/**
  * @brief redefines __weak function in stm32_seq.c such to enter low power
  * @param none
  * @retval  none
  */
void UTIL_SEQ_Idle(void)
{
#if LOW_POWER_MODE_ENBLE
	UTIL_LPM_EnterLowPower();
#endif
}

/* Private functions ---------------------------------------------------------*/

static void TimestampNow(uint8_t *buff, uint16_t* size)
{
	SysTime_t curtime = SysTimeGet();
	sprintf((char *)buff, "%ds%d:", curtime.Seconds, curtime.SubSeconds);
	*size = strlen((char *)buff);
}



static void Gpio_PreInit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	/* Configure all IOs in analog input              */
	/* Except PA143 and PA14 (SWCLK and SWD) for debug*/
	/* PA13 and PA14 are configured in debug_init     */
	/* Configure all GPIO as analog to reduce current consumption on non used IOs */
	/* Enable GPIOs clock */
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();

	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	/* All GPIOs except debug pins (SWCLK and SWD) */
	GPIO_InitStruct.Pin = GPIO_PIN_All & (~(GPIO_PIN_13 | GPIO_PIN_14));
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/* All GPIOs */
	GPIO_InitStruct.Pin = GPIO_PIN_All;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

	/* Disable GPIOs clock */
	__HAL_RCC_GPIOA_CLK_DISABLE();
	__HAL_RCC_GPIOB_CLK_DISABLE();
	__HAL_RCC_GPIOC_CLK_DISABLE();
	__HAL_RCC_GPIOH_CLK_DISABLE();
}

/* Disable StopMode when traces need to be printed */
void UTIL_ADV_TRACE_PreSendHook (void)
{
#if LOW_POWER_MODE_ENBLE
	UTIL_LPM_SetStopMode(LPM_UART_TX_Id , UTIL_LPM_DISABLE );
#endif
}
/* Reenable StopMode when traces have been printed */
void UTIL_ADV_TRACE_PostSendHook (void)
{
#if LOW_POWER_MODE_ENBLE
	UTIL_LPM_SetStopMode(LPM_UART_TX_Id , UTIL_LPM_ENABLE );
#endif
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

