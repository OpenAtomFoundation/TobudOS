/**
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "common.h"
#include "flash_if.h"
#include "menu.h"
#include "ymodem.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
pFunction JumpToApplication;
uint32_t JumpAddress;
uint32_t FlashProtection = 0;
uint8_t aFileName[FILE_NAME_LENGTH];

/* Private function prototypes -----------------------------------------------*/
void SerialDownload(void);
void SerialUpload(void);

/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Download a file via serial port
 * @param  None
 * @retval None
 */
void SerialDownload(void)
{
    uint8_t number[11] = {0};
    uint32_t size = 0;
    COM_StatusTypeDef result;

    Serial_PutString((uint8_t *)"Waiting for the file to be sent ... (press 'a' to abort)\n\r");
    result = Ymodem_Receive( &size );
    if (result == COM_OK)
    {
        Serial_PutString((uint8_t *)"\n\n\r Programming Completed Successfully!\n\r--------------------------------\r\n Name: ");
        Serial_PutString(aFileName);
        Int2Str(number, size);
        Serial_PutString((uint8_t *)"\n\r Size: ");
        Serial_PutString(number);
        Serial_PutString((uint8_t *)" Bytes\r\n");
        Serial_PutString((uint8_t *)"-------------------\n");
    }
    else if (result == COM_LIMIT)
    {
        Serial_PutString((uint8_t *)"\n\n\rThe image size is higher than the allowed space memory!\n\r");
    }
    else if (result == COM_DATA)
    {
        Serial_PutString((uint8_t *)"\n\n\rVerification failed!\n\r");
    }
    else if (result == COM_ABORT)
    {
        Serial_PutString((uint8_t *)"\r\n\nAborted by user.\n\r");
    }
    else
    {
        Serial_PutString((uint8_t *)"\n\rFailed to receive the file!\n\r");
    }
}

/**
 * @brief  Upload a file via serial port.
 * @param  None
 * @retval None
 */
void SerialUpload(void)
{
    uint8_t status = 0;

    Serial_PutString((uint8_t *)"\n\n\rSelect Receive File\n\r");

    HAL_UART_Receive(iap_uart, &status, 1, RX_TIMEOUT);
    if ( status == CRC16)
    {
        /* Transmit the flash image through ymodem protocol */
        status = Ymodem_Transmit((uint8_t*)APPLICATION_ADDRESS, (const uint8_t*)"UploadedFlashImage.bin", USER_FLASH_SIZE);

        if (status != 0)
        {
            Serial_PutString((uint8_t *)"\n\rError Occurred while Transmitting File\n\r");
        }
        else
        {
            Serial_PutString((uint8_t *)"\n\rFile uploaded successfully \n\r");
        }
    }
}

/**
 * @brief  Display the Main Menu on HyperTerminal
 * @param  None
 * @retval None
 */
void Main_Menu(void)
{
    uint8_t key = 0;

    /* Test if any sector of Flash memory where user application will be loaded is write protected */
    FlashProtection = FLASH_If_GetWriteProtectionStatus();

    while (1)
    {

        Serial_PutString((uint8_t *)"\r\n=================== Main Menu ============================\r\n\n");
        Serial_PutString((uint8_t *)"  Download image to the internal Flash ----------------- 1\r\n\n");
        Serial_PutString((uint8_t *)"  Upload image from the internal Flash ----------------- 2\r\n\n");
        Serial_PutString((uint8_t *)"  Execute the loaded application ----------------------- 3\r\n\n");


        if(FlashProtection != FLASHIF_PROTECTION_NONE)
        {
            Serial_PutString((uint8_t *)"  Disable the write protection ------------------------- 4\r\n\n");
            if((FlashProtection & (FLASHIF_PROTECTION_PCROPENABLED | FLASHIF_PROTECTION_RDPENABLED)) != 0x0)
            {
                Serial_PutString((uint8_t *)"  The write protection disable will erase all the flash\r\n");
                Serial_PutString((uint8_t *)"  Please use STlink utility to disable the protection  \r\n\n");
                while(1);
            }
        }
        else
        {
            Serial_PutString((uint8_t *)"  Enable the write protection -------------------------- 4\r\n\n");
        }
        Serial_PutString((uint8_t *)"==========================================================\r\n\n");

        /* Clean the input path */
        __HAL_UART_FLUSH_DRREGISTER(iap_uart);

        /* Receive key */
        HAL_UART_Receive(iap_uart, &key, 1, RX_TIMEOUT);

        switch (key)
        {
            case '1' :
                /* Download user application in the Flash */
                SerialDownload();
                break;
            case '2' :
                /* Upload user application from the Flash */
                SerialUpload();
                break;
            case '3' :
                Serial_PutString((uint8_t *)"Start program execution......\r\n\n");
                /* execute the new program */
                JumpAddress = *(__IO uint32_t*) (APPLICATION_ADDRESS + 4);
                /* Jump to user application */
                JumpToApplication = (pFunction) JumpAddress;
                /* Initialize user application's Stack Pointer */
                __set_MSP(*(__IO uint32_t*) APPLICATION_ADDRESS);
                JumpToApplication();
                break;
            case '4' :
                if (FlashProtection != FLASHIF_PROTECTION_NONE)
                {
                    /* Disable the write protection */
                    if (FLASH_If_WriteProtectionConfig(FLASHIF_WRP_DISABLE) == FLASHIF_OK)
                    {
                        Serial_PutString((uint8_t *)"Write Protection disabled...\r\n");
                        Serial_PutString((uint8_t *)"System will now restart...\r\n");
                        /* Launch the option byte loading */
                        HAL_FLASH_OB_Launch();
                    }
                    else
                    {
                        Serial_PutString((uint8_t *)"Error: Flash write un-protection failed...\r\n");
                    }
                }
                else
                {
                    if (FLASH_If_WriteProtectionConfig(FLASHIF_WRP_ENABLE) == FLASHIF_OK)
                    {
                        Serial_PutString((uint8_t *)"Write Protection enabled...\r\n");
                        Serial_PutString((uint8_t *)"System will now restart...\r\n");
                        /* Launch the option byte loading */
                        HAL_FLASH_OB_Launch();
                    }
                    else
                    {
                        Serial_PutString((uint8_t *)"Error: Flash write protection failed...\r\n");
                    }
                }
                break;
            default:
                Serial_PutString((uint8_t *)"Invalid Number ! ==> The number should be either 1, 2, 3 or 4\r");
                break;
        }
    }
}

