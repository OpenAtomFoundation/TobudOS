/*****************************************************************************
* Copyright (c) 2019, Nations Technologies Inc.
*
* All rights reserved.
* ****************************************************************************
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* - Redistributions of source code must retain the above copyright notice,
* this list of conditions and the disclaimer below.
*
* Nations' name may not be used to endorse or promote products derived from
* this software without specific prior written permission.
*
* DISCLAIMER: THIS SOFTWARE IS PROVIDED BY NATIONS "AS IS" AND ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
* DISCLAIMED. IN NO EVENT SHALL NATIONS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
* OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
* ****************************************************************************/
/**
* @file    usbd_ccid_cmd.h
* @author Nations Solution Team
* @version v1.0.0
* @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
* @brief   CCID Commands handling prototype
* @addtogroup USB
* @{
*/


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBD_CCID_CMD_H__
#define __USBD_CCID_CMD_H__

/* Includes ------------------------------------------------------------------*/
#include "n32g020xx.h"
#include "core_cm0.h"
#include "n32g020xx_usb_regs.h"
#include "usbd_ccid_core.h"

/* Exported types ------------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/

/******************************************************************************/
/*  ERROR CODES for USB Bulk In Messages : bError                   */
/******************************************************************************/

#define   SLOT_NO_ERROR         0x81
#define   SLOTERROR_UNKNOWN     0x82

/* Index of not supported / incorrect message parameter : 7Fh to 01h */
/* These Values are used for Return Types between Firmware Layers    */
/*
Failure of a command 
The CCID cannot parse one parameter or the ICC is not supporting one parameter. 
Then the Slot Error register contains the index of the first bad parameter as a 
positive number (1-127). For instance, if the CCID receives an ICC command to 
an unimplemented slot, then the Slot Error register shall be set to 
‘5’ (index of bSlot field).
  */

#define   SLOTERROR_BAD_LENTGH                    0x01
#define   SLOTERROR_BAD_SLOT                      0x05
#define   SLOTERROR_BAD_POWERSELECT               0x07
#define   SLOTERROR_BAD_PROTOCOLNUM               0x07
#define   SLOTERROR_BAD_CLOCKCOMMAND              0x07
#define   SLOTERROR_BAD_ABRFU_3B                  0x07
#define   SLOTERROR_BAD_BMCHANGES                 0x07
#define   SLOTERROR_BAD_BFUNCTION_MECHANICAL      0x07
#define   SLOTERROR_BAD_ABRFU_2B                  0x08
#define   SLOTERROR_BAD_LEVELPARAMETER            0x08
#define   SLOTERROR_BAD_FIDI                      0x0A
#define   SLOTERROR_BAD_T01CONVCHECKSUM           0x0B
#define   SLOTERROR_BAD_GUARDTIME                 0x0C
#define   SLOTERROR_BAD_WAITINGINTEGER            0x0D
#define   SLOTERROR_BAD_CLOCKSTOP                 0x0E
#define   SLOTERROR_BAD_IFSC                      0x0F
#define   SLOTERROR_BAD_NAD                       0x10
#define   SLOTERROR_BAD_DWLENGTH                  0x08  /* Used in PC_to_RDR_XfrBlock*/

/*----------  Table 6.2-2 Slot error register when bmCommandStatus = 1        */
#define   SLOTERROR_CMD_ABORTED                    0xFF
#define   SLOTERROR_ICC_MUTE                       0xFE
#define   SLOTERROR_XFR_PARITY_ERROR               0xFD
#define   SLOTERROR_XFR_OVERRUN                    0xFC
#define   SLOTERROR_HW_ERROR                       0xFB
#define   SLOTERROR_BAD_ATR_TS                     0xF8
#define   SLOTERROR_BAD_ATR_TCK                    0xF7
#define   SLOTERROR_ICC_PROTOCOL_NOT_SUPPORTED     0xF6
#define   SLOTERROR_ICC_CLASS_NOT_SUPPORTED        0xF5
#define   SLOTERROR_PROCEDURE_BYTE_CONFLICT        0xF4
#define   SLOTERROR_DEACTIVATED_PROTOCOL           0xF3
#define   SLOTERROR_BUSY_WITH_AUTO_SEQUENCE        0xF2
#define   SLOTERROR_PIN_TIMEOUT                    0xF0
#define   SLOTERROR_PIN_CANCELLED                  0xEF
#define   SLOTERROR_CMD_SLOT_BUSY                  0xE0
#define   SLOTERROR_CMD_NOT_SUPPORTED              0x00


#define   DEFAULT_FIDI              0x11 /* DEFAULT_FIDI_VALUE */
#define   DEFAULT_T01CONVCHECKSUM   0x00
#define   DEFAULT_EXTRA_GUARDTIME   0x00
#define   DEFAULT_WAITINGINTEGER    0x0A
#define   DEFAULT_CLOCKSTOP         0x00
#define   DEFAULT_IFSC              0x20
#define   DEFAULT_NAD               0x00

#define   DEFAULT_DATA_RATE    0x000025CD
#define   DEFAULT_CLOCK_FREQ   0x00000E10


/*
Offset=0 bmICCStatus 2 bit  0, 1, 2
    0 - An ICC is present and active (power is on and stable, RST is inactive)
    1 - An ICC is present and inactive (not activated or shut down by hardware error)
    2 - No ICC is present
    3 - RFU
Offset=0 bmRFU 4 bits 0 RFU
Offset=6 bmCommandStatus 2 bits 0, 1, 2
    0 - Processed without error
    1 - Failed (error code provided by the error register)
    2 - Time Extension is requested
    3 - RFU
  */

#define BM_ICC_PRESENT_ACTIVE 0x00
#define BM_ICC_PRESENT_INACTIVE 0x01
#define BM_ICC_NO_ICC_PRESENT   0x02

#define BM_COMMAND_STATUS_OFFSET 0x06
#define BM_COMMAND_STATUS_NO_ERROR 0x00 
#define BM_COMMAND_STATUS_FAILED   (0x01 << BM_COMMAND_STATUS_OFFSET)
#define BM_COMMAND_STATUS_TIME_EXTN  (0x02 << BM_COMMAND_STATUS_OFFSET)

/* defines for the CCID_CMD Layers */
#define SIZE_OF_ATR 33
#define LEN_RDR_TO_PC_SLOTSTATUS 10
#define LEN_PROTOCOL_STRUCT_T0   5

#define BPROTOCOL_NUM_T0  0
#define BPROTOCOL_NUM_T1  1

/************************************************************************************/
/*   ERROR CODES for RDR_TO_PC_HARDWAREERROR Message : bHardwareErrorCode           */
/************************************************************************************/

#define   HARDWAREERRORCODE_OVERCURRENT       0x01
#define   HARDWAREERRORCODE_VOLTAGEERROR      0x02
#define   HARDWAREERRORCODE_OVERCURRENT_IT    0x04
#define   HARDWAREERRORCODE_VOLTAGEERROR_IT   0x08

typedef enum 
{
  CHK_PARAM_SLOT = 1,
  CHK_PARAM_DWLENGTH = (1<<1),
  CHK_PARAM_abRFU2 = (1<<2), 
  CHK_PARAM_abRFU3 = (1<<3),
  CHK_PARAM_CARD_PRESENT = (1<<4),
  CHK_PARAM_ABORT = (1<<5),
  CHK_ACTIVE_STATE = (1<<6)
} ChkParam_t;


/* Exported functions ------------------------------------------------------- */

uint8_t  PC_to_RDR_IccPowerOn(void);
uint8_t  PC_to_RDR_IccPowerOff(void);
uint8_t  PC_to_RDR_GetSlotStatus(void);
uint8_t  PC_to_RDR_XfrBlock(void);
uint8_t  PC_to_RDR_GetParameters(void);
uint8_t  PC_to_RDR_ResetParameters(void);
uint8_t  PC_to_RDR_SetParameters(void);
uint8_t  PC_to_RDR_Escape(void);
uint8_t  PC_to_RDR_IccClock(void);
uint8_t  PC_to_RDR_Abort(void);
uint8_t  PC_TO_RDR_T0Apdu(void);
uint8_t  PC_TO_RDR_Mechanical(void);
uint8_t  PC_TO_RDR_SetDataRateAndClockFrequency(void);
uint8_t  PC_TO_RDR_Secure(void);

void RDR_to_PC_DataBlock(unsigned char );
void RDR_to_PC_NotifySlotChange(void);
void RDR_to_PC_SlotStatus(unsigned char );
void RDR_to_PC_Parameters(unsigned char );
void RDR_to_PC_Escape(unsigned char );
void RDR_to_PC_DataRateAndClockFrequency(uint8_t  errorCode);


void CCID_UpdSlotStatus (uint8_t );
void CCID_UpdSlotChange (uint8_t );
uint8_t CCID_IsSlotStatusChange (void);
uint8_t CCID_CmdAbort(uint8_t slot, uint8_t seq);

#endif /* __USBD_CCID_CMD_H__ */

/** @} */

