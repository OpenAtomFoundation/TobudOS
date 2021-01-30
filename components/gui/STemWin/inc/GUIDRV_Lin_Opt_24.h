/*********************************************************************
*          Portions COPYRIGHT 2016 STMicroelectronics                *
*          Portions SEGGER Microcontroller GmbH & Co. KG             *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2015  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.32 - Graphical user interface for embedded applications **
All  Intellectual Property rights  in the Software belongs to  SEGGER.
emWin is protected by  international copyright laws.  Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with the following terms:

The  software has  been licensed  to STMicroelectronics International
N.V. a Dutch company with a Swiss branch and its headquarters in Plan-
les-Ouates, Geneva, 39 Chemin du Champ des Filles, Switzerland for the
purposes of creating libraries for ARM Cortex-M-based 32-bit microcon_
troller products commercialized by Licensee only, sublicensed and dis_
tributed under the terms and conditions of the End User License Agree_
ment supplied by STMicroelectronics International N.V.
Full source code is available at: www.segger.com

We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : GUIDRV_Lin_Opt_24.h
Purpose     : Optimized routines, included by GUIDRV_Lin_..._24.c
---------------------------END-OF-HEADER------------------------------
*/

/**
  ******************************************************************************
  * @attention
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */
  
/*********************************************************************
*
*       Static functions
*
**********************************************************************
*/
/*********************************************************************
*
*       _FillRectOpt24
*
* Purpose:
*   Optimized filling routine for 24 bpp
*/
static void _FillRectOpt24(GUI_DEVICE * pDevice, int x0, int y0, int x1, int y1) {
  DRIVER_CONTEXT * pContext;
  U32 Off, Off0, OffLine;
  int RemPixels, NumLines, RemLines, RemItems, Odd;
  U32 Data, Data0, Data1, Data2;
  LCD_PIXELINDEX ColorIndex;

  pContext  = (DRIVER_CONTEXT *)pDevice->u.pContext;
  Off0      = XY2OFF32(pContext->vxSizePhys, x0, y0);
  RemPixels = x1 - x0 + 1;
  Odd       = x0 & 3;
  NumLines  = y1 - y0 + 1;
  OffLine   = (pContext->vxSizePhys + pContext->vxSizePhys +pContext->vxSizePhys) >> 2;
  RemItems  = 0;
  Off       = 0;
  if (GUI_pContext->DrawMode & LCD_DRAWMODE_XOR) {
    //
    // First triple DWORD
    //
    if (Odd) {
      for (RemLines = NumLines; RemLines; RemLines--) {
        RemItems = RemPixels;
        Off      = Off0 + OffLine * (RemLines - 1);
        Data     = READ_MEM32(pContext->VRAMAddr, Off);
        switch (Odd) {
        case 1:
          Data ^= 0xFF000000;
          WRITE_MEM32(pContext->VRAMAddr, Off, Data);
          Off++;
          Data  = READ_MEM32(pContext->VRAMAddr, Off);
          Data ^= 0x0000FFFF;
          RemItems--;
          if (!RemItems) {
            WRITE_MEM32(pContext->VRAMAddr, Off, Data);
            break;
          }
          //
          // no break at this position required...
          //
        case 2:
          Data ^= 0xFFFF0000;
          WRITE_MEM32(pContext->VRAMAddr, Off, Data);
          Off++;
          Data  = READ_MEM32(pContext->VRAMAddr, Off);
          Data ^= 0x000000FF;
          RemItems--;
          if (!RemItems) {
            WRITE_MEM32(pContext->VRAMAddr, Off, Data);
            break;
          }
          //
          // no break at this position required...
          //
        case 3:
          Data ^= 0xFFFFFF00;
          RemItems--;
          WRITE_MEM32(pContext->VRAMAddr, Off, Data);
          Off++;
        }
      }
      Off0       = Off;
      RemPixels -= (RemPixels - RemItems);
    }
    //
    // Complete triple DWORDS
    //
    if (RemPixels >= 4) {
      for (RemLines = NumLines; RemLines; RemLines--) {
        RemItems = RemPixels;
        Off      = Off0 + OffLine * (RemLines - 1);
        do {
          Data  = READ_MEM32(pContext->VRAMAddr, Off + 0);
          Data ^= 0xFFFFFFFF;
          WRITE_MEM32(pContext->VRAMAddr, Off + 0, Data);
          Data  = READ_MEM32(pContext->VRAMAddr, Off + 1);
          Data ^= 0xFFFFFFFF;
          WRITE_MEM32(pContext->VRAMAddr, Off + 1, Data);
          Data  = READ_MEM32(pContext->VRAMAddr, Off + 2);
          Data ^= 0xFFFFFFFF;
          WRITE_MEM32(pContext->VRAMAddr, Off + 2, Data);
          Off += 3;
        } while ((RemItems -= 4) >= 4);
      }
      Off0       = Off;
      RemPixels -= (RemPixels >> 2) << 2;
    }
    //
    // Last triple DWORD
    //
    if (RemPixels) {
      for (RemLines = NumLines; RemLines; RemLines--) {
        RemItems = RemPixels;
        Off      = Off0 + OffLine * (RemLines - 1);
        Data  = READ_MEM32(pContext->VRAMAddr, Off);
        Data ^= 0x00FFFFFF;
        RemItems--;
        if (!RemItems) {
          WRITE_MEM32(pContext->VRAMAddr, Off, Data);
          continue;
        }
        Data ^= 0xFF000000;
        WRITE_MEM32(pContext->VRAMAddr, Off, Data);
        Off++;
        Data  = READ_MEM32(pContext->VRAMAddr, Off);
        Data ^= 0x0000FFFF;
        RemItems--;
        if (!RemItems) {
          WRITE_MEM32(pContext->VRAMAddr, Off, Data);
          continue;
        }
        Data ^= 0xFFFF0000;
        WRITE_MEM32(pContext->VRAMAddr, Off, Data);
        Off++;
        Data  = READ_MEM32(pContext->VRAMAddr, Off);
        Data ^= 0x000000FF;
        WRITE_MEM32(pContext->VRAMAddr, Off, Data);
      }
    }
  } else {
    ColorIndex = LCD__GetColorIndex();
    //
    // First triple DWORD
    //
    if (Odd) {
      for (RemLines = NumLines; RemLines; RemLines--) {
        RemItems = RemPixels;
        Off      = Off0 + OffLine * (RemLines - 1);
        Data  = READ_MEM32(pContext->VRAMAddr, Off);
        switch (Odd) {
        case 1:
          Data &= 0x00FFFFFF;
          Data |= ColorIndex << 24;
          WRITE_MEM32(pContext->VRAMAddr, Off, Data);
          Off++;
          Data  = READ_MEM32(pContext->VRAMAddr, Off);
          Data &= 0xFFFF0000;
          Data |= ColorIndex >> 8;
          RemItems--;
          if (!RemItems) {
            WRITE_MEM32(pContext->VRAMAddr, Off, Data);
            break;
          }
          //
          // no break at this position required...
          //
        case 2:
          Data &= 0x0000FFFF;
          Data |= ColorIndex << 16;
          WRITE_MEM32(pContext->VRAMAddr, Off, Data);
          Off++;
          Data  = READ_MEM32(pContext->VRAMAddr, Off);
          Data &= 0xFFFFFF00;
          Data |= ColorIndex >> 16;
          RemItems--;
          if (!RemItems) {
            WRITE_MEM32(pContext->VRAMAddr, Off, Data);
            break;
          }
          //
          // no break at this position required...
          //
        case 3:
          Data &= 0x000000FF;
          Data |= ColorIndex << 8;
          RemItems--;
          WRITE_MEM32(pContext->VRAMAddr, Off, Data);
          Off++;
        }
      }
      Off0       = Off;
      RemPixels -= (RemPixels - RemItems);
    }
    //
    // Complete triple DWORDS
    //
    if (RemPixels >= 4) {
      for (RemLines = NumLines; RemLines; RemLines--) {
        RemItems = RemPixels;
        Off      = Off0 + OffLine * (RemLines - 1);
        Data0 = (ColorIndex      ) | (ColorIndex << 24);
        Data1 = (ColorIndex >>  8) | (ColorIndex << 16);
        Data2 = (ColorIndex >> 16) | (ColorIndex <<  8);
        do {
          WRITE_MEM32(pContext->VRAMAddr, Off + 0, Data0);
          WRITE_MEM32(pContext->VRAMAddr, Off + 1, Data1);
          WRITE_MEM32(pContext->VRAMAddr, Off + 2, Data2);
          Off += 3;
        } while ((RemItems -= 4) >= 4);
      }
      Off0       = Off;
      RemPixels -= (RemPixels >> 2) << 2;
    }
    //
    // Last triple DWORD
    //
    if (RemPixels) {
      for (RemLines = NumLines; RemLines; RemLines--) {
        RemItems = RemPixels;
        Off      = Off0 + OffLine * (RemLines - 1);
        Data  = READ_MEM32(pContext->VRAMAddr, Off);
        Data &= 0xFF000000;
        Data |= ColorIndex;
        RemItems--;
        if (!RemItems) {
          WRITE_MEM32(pContext->VRAMAddr, Off, Data);
          continue;
        }
        Data &= 0x00FFFFFF;
        Data |= ColorIndex << 24;
        WRITE_MEM32(pContext->VRAMAddr, Off, Data);
        Off++;
        Data  = READ_MEM32(pContext->VRAMAddr, Off);
        Data &= 0xFFFF0000;
        Data |= ColorIndex >> 8;
        RemItems--;
        if (!RemItems) {
          WRITE_MEM32(pContext->VRAMAddr, Off, Data);
          continue;
        }
        Data &= 0x0000FFFF;
        Data |= ColorIndex << 16;
        WRITE_MEM32(pContext->VRAMAddr, Off, Data);
        Off++;
        Data  = READ_MEM32(pContext->VRAMAddr, Off);
        Data &= 0xFFFFFF00;
        Data |= ColorIndex >> 16;
        WRITE_MEM32(pContext->VRAMAddr, Off, Data);
      }
    }
  }
}

/*************************** End of file ****************************/
