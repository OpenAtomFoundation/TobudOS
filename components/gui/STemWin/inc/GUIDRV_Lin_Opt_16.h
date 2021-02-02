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
File        : GUIDRV_Lin_Opt_16.h
Purpose     : Optimized routines, included by GUIDRV_Lin_..._16.c
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
*       _FillRectOpt16
*
* Purpose:
*   Optimized filling routine for 16 bpp
*/
static void _FillRectOpt16(GUI_DEVICE * pDevice, int x0, int y0, int x1, int y1) {
  DRIVER_CONTEXT * pContext;
  U32 * pDest;
  U32 * pDest0;
  U32 Off, OffLine;
  int RemPixels, NumLines, RemLines, RemItems;
  U32 Data, ColorMask;
  LCD_PIXELINDEX ColorIndex;

  pContext   = (DRIVER_CONTEXT *)pDevice->u.pContext;
  ColorIndex = LCD__GetColorIndex();
  Off        = XY2OFF32(pContext->vxSizePhys, x0, y0);
  pDest0     = OFF2PTR32(pContext->VRAMAddr, Off);
  RemPixels  = x1 - x0 + 1;
  NumLines   = y1 - y0 + 1;
  OffLine    = pContext->vxSizePhys >> 1;
  pDest      = NULL;
  if (GUI_pContext->DrawMode & LCD_DRAWMODE_XOR) {
    //
    // First DWORD
    //
    if (x0 & 1) {
      for (RemLines = NumLines, pDest = pDest0; RemLines; RemLines--) {
        Data = READ_MEM32P(pDest);
        #if (LCD_ENDIAN_BIG == 0)
          Data ^= 0xFFFF0000;
        #else
          Data ^= 0xFFFF;
        #endif
        WRITE_MEM32P(pDest, Data);
        pDest += OffLine;
      }
      pDest0++;
      RemPixels--;
    }
    //
    // Complete DWORDS
    //
    if (RemPixels >= 2) {
      for (RemLines = NumLines; RemLines; RemLines--) {
        RemItems = RemPixels;
        pDest    = pDest0 + OffLine * (RemLines - 1);
        do {
          Data  = READ_MEM32P(pDest);
          Data ^= 0xFFFFFFFF;
          WRITE_MEM32P(pDest, Data);
          pDest++;
          RemItems -= 2;
        } while (RemItems >= 2);
      }
      pDest0 = pDest;
      RemPixels -= (RemPixels >> 1) << 1;
    }
    //
    // Last DWORD
    //
    if (RemPixels > 0) {
      for (RemLines = NumLines, pDest = pDest0; RemLines; RemLines--) {
        Data = READ_MEM32P(pDest);
        #if (LCD_ENDIAN_BIG == 0)
          Data ^= 0xFFFF;
        #else
          Data ^= 0xFFFF0000;
        #endif
        WRITE_MEM32P(pDest, Data);
        pDest += OffLine;
      }
    }
  } else {
    //
    // First DWORD
    //
    if (x0 & 1) {
      for (RemLines = NumLines, pDest = pDest0; RemLines; RemLines--) {
        Data = READ_MEM32P(pDest);
        #if (LCD_ENDIAN_BIG == 0)
          Data &= 0xFFFF;
          Data |= (((U32)ColorIndex) << 16);
        #else
          Data &= 0xFFFF0000;
          Data |= ColorIndex;
        #endif
        WRITE_MEM32P(pDest, Data);
        pDest += OffLine;
      }
      pDest0++;
      RemPixels--;
    }
    //
    // Complete DWORDS
    //
    ColorMask = ColorIndex * 0x00010001;
    if (RemPixels >= 16) {
      RemPixels -= 16;
      
      for (RemLines = NumLines; RemLines; RemLines--) {
        RemItems = RemPixels;
        pDest    = pDest0 + OffLine * (RemLines - 1);
        do {
          WRITE_MEM32P(pDest,     ColorMask);
          WRITE_MEM32P(pDest + 1, ColorMask);
          WRITE_MEM32P(pDest + 2, ColorMask);
          WRITE_MEM32P(pDest + 3, ColorMask);
          WRITE_MEM32P(pDest + 4, ColorMask);
          WRITE_MEM32P(pDest + 5, ColorMask);
          WRITE_MEM32P(pDest + 6, ColorMask);
          WRITE_MEM32P(pDest + 7, ColorMask);
          pDest    += 8;
          RemItems -= 16;
        } while (RemItems >= 0);
      }
      pDest0 = pDest;

      RemPixels += 16;
      RemPixels -= (RemPixels >> 4) << 4;
    }
    if (RemPixels >= 2) {
      for (RemLines = NumLines; RemLines; RemLines--) {
        RemItems = RemPixels;
        pDest    = pDest0 + OffLine * (RemLines - 1);
        do {
          WRITE_MEM32P(pDest, ColorMask);
          pDest++;
          RemItems -= 2;
        } while (RemItems >= 2);
      }
      pDest0 = pDest;
      RemPixels -= (RemPixels >> 1) << 1;
    }
    //
    // Last DWORD
    //
    if (RemPixels > 0) {
      for (RemLines = NumLines, pDest = pDest0; RemLines; RemLines--) {
        Data = READ_MEM32P(pDest);
        #if (LCD_ENDIAN_BIG == 0)
          Data &= 0xFFFF0000;
          Data |= ColorIndex;
        #else
          Data &= 0xFFFF;
          Data |= (((U32)ColorIndex) << 16);
        #endif
        WRITE_MEM32P(pDest, Data);
        pDest += OffLine;
      }
    }
  }
}

/*************************** End of file ****************************/
