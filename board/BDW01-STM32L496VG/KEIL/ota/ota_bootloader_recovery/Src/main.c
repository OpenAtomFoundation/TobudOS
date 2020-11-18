/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
#include "stdio.h"
#include "stm32l4xx_hal.h"

#include "ota_recovery.h"

#include "tos_kv.h"

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_USART1_UART_Init(void);

UART_HandleTypeDef huart1;

int fputc(int ch, FILE *f)
{
   if (ch == '\n') {
    HAL_UART_Transmit(&huart1, (void *)"\r", 1,30000);
  }
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);
  return ch;
}

extern ota_flash_drv_t stm32l4_norflash_onchip_drv_ota;
extern ota_flash_prop_t stm32l4_norflash_onchip_prop_ota;

int main(void)
{
    ota_err_t ret;
    
    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* Configure the system clock */
    SystemClock_Config();
	MX_GPIO_Init();
    MX_USART1_UART_Init();

    uint32_t partition_addr = 0x080FF800;
	

    if ((ret = ota_env_init(OTA_UPDATE_IN_POSITION, partition_addr, &stm32l4_norflash_onchip_drv_ota, &stm32l4_norflash_onchip_prop_ota)) != OTA_ERR_NONE) {
        printf("env init failed!OTA errcode = %d\n", ret);
        return -1;
    } else {
        printf("env init successfully!\r\n");
		printf("+-------------------------+\r\n");
		printf("|Active APP |  0x%08x |\r\n", ota_partition_start(OTA_PARTITION_ACTIVE_APP));
		printf("|   OTA     |  0x%08x |\r\n", ota_partition_start(OTA_PARTITION_OTA));
		printf("|   KV      |  0x%08x |\r\n", ota_partition_start(OTA_PARTITION_KV));
		printf("| Version   |      %d.%d    |\r\n", ota_partition_init_version_get().major, ota_partition_init_version_get().minor);
		printf("+-------------------------+\r\n");
    }
	
//	while(1)
//	{;}
	
//ota_img_vs_t new_version;
//new_version.major = 0;
//new_version.minor = 2;

//tos_kv_set("new_version", &new_version, sizeof(ota_img_vs_t));

#if 0
    if ((ret = ota_recovery_xip()) != OTA_ERR_NONE) {
        printf("recovery failed, OTA errcode = %d!\r\n", ret);
    } else {
        printf("recovery successfully!\r\n");
    }
#else
    if ((ret = ota_recovery()) != OTA_ERR_NONE) {
        printf("recovery failed, OTA errcode = %d!\r\n", ret);
    } else {
        printf("recovery successfully!\r\n");
    }
#endif
	

    uint32_t jmp_addr, app_addr = ota_partition_start(OTA_PARTITION_ACTIVE_APP);

    /* test if user code is programmed starting from address app_addr */
    if (((*(uint32_t *)app_addr) & 0x2FFE0000) == 0x20000000) {
        /* Jump to user application */
        jmp_addr = *(uint32_t *)(app_addr + 4);

        /* Initialize user application's Stack Pointer */
        __set_MSP(*(uint32_t *)app_addr);
        ((void (*)(void))jmp_addr)();
    }
}

void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
  	printf("err %s,L%d\r\n",file,line);
  }
  /* USER CODE END Error_Handler_Debug */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE
                              |RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_11;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_USART2
                              |RCC_PERIPHCLK_USART3|RCC_PERIPHCLK_UART4
                              |RCC_PERIPHCLK_LPUART1|RCC_PERIPHCLK_SAI2
                              |RCC_PERIPHCLK_I2C2|RCC_PERIPHCLK_I2C3
                              |RCC_PERIPHCLK_USB|RCC_PERIPHCLK_SDMMC1
                              |RCC_PERIPHCLK_ADC;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.Usart3ClockSelection = RCC_USART3CLKSOURCE_PCLK1;
  PeriphClkInit.Uart4ClockSelection = RCC_UART4CLKSOURCE_PCLK1;
  PeriphClkInit.Lpuart1ClockSelection = RCC_LPUART1CLKSOURCE_PCLK1;
  PeriphClkInit.I2c2ClockSelection = RCC_I2C2CLKSOURCE_PCLK1;
  PeriphClkInit.I2c3ClockSelection = RCC_I2C3CLKSOURCE_PCLK1;
  PeriphClkInit.Sai2ClockSelection = RCC_SAI2CLKSOURCE_PLLSAI1;
  PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_PLLSAI1;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLLSAI1;
  PeriphClkInit.Sdmmc1ClockSelection = RCC_SDMMC1CLKSOURCE_PLLSAI1;
  PeriphClkInit.PLLSAI1.PLLSAI1Source = RCC_PLLSOURCE_HSE;
  PeriphClkInit.PLLSAI1.PLLSAI1M = 1;
  PeriphClkInit.PLLSAI1.PLLSAI1N = 12;
  PeriphClkInit.PLLSAI1.PLLSAI1P = RCC_PLLP_DIV4;
  PeriphClkInit.PLLSAI1.PLLSAI1Q = RCC_PLLQ_DIV4;
  PeriphClkInit.PLLSAI1.PLLSAI1R = RCC_PLLR_DIV4;
  PeriphClkInit.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_SAI1CLK|RCC_PLLSAI1_48M2CLK
                              |RCC_PLLSAI1_ADC1CLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the main internal regulator output voltage 
    */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/**
 * @brief USART2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART1_UART_Init(void)
{

 huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}




#ifdef  USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(char *file, uint32_t line)
{
}

#endif /* USE_FULL_ASSERT */

