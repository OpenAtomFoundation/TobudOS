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
File        : GUIDRV_Lin_Opt_32.h
Purpose     : Optimized routines, included by GUIDRV_Lin_..._32.c
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
*       _FillRectOpt32
*
* Purpose:
*   Optimized filling routine for 32 bpp
*/
static void _FillRectOpt32(GUI_DEVICE * pDevice, int x0, int y0, int x1, int y1) {
  DRIVER_CONTEXT * pContext;
  register LCD_PIXELINDEX ColorIndex;
  U32 * pDest;
  U32 * pDest0;
  U32 Off, Off0, RemPixels, NumLines, RemLines, OffLine, RemItems;
  LCD_PIXELINDEX IndexMask;

  pContext  = (DRIVER_CONTEXT *)pDevice->u.pContext;
  RemPixels = x1 - x0 + 1;
  NumLines  = y1 - y0 + 1;
  OffLine   = pContext->vxSizePhys;
  pDest     = NULL;
  if (GUI_pContext->DrawMode & LCD_DRAWMODE_XOR) {
    IndexMask = pDevice->pColorConvAPI->pfGetIndexMask();
    Off0      = XY2OFF32(pContext->vxSizePhys, x0, y0);
    if (RemPixels) {
      for (RemLines = NumLines; RemLines; RemLines--) {
        RemItems = RemPixels;
        Off = Off0 + OffLine * (RemLines - 1);
        do {
          ColorIndex  = READ_MEM32(pContext->VRAMAddr, Off);
          ColorIndex ^= IndexMask;
          WRITE_MEM32(pContext->VRAMAddr, Off, ColorIndex);
          Off++;
        } while (--RemItems);
      }
    }
  } else {
    Off        = XY2OFF32(pContext->vxSizePhys, x0, y0);
    pDest0     = OFF2PTR32(pContext->VRAMAddr, Off);
    ColorIndex = LCD__GetColorIndex();
    if (RemPixels >= 16) {
      for (RemLines = NumLines; RemLines; RemLines--) {
        RemItems = RemPixels;
        pDest    = pDest0 + OffLine * (RemLines - 1);
        do {
          WRITE_MEM32P(pDest     , ColorIndex);
          WRITE_MEM32P(pDest +  1, ColorIndex);
          WRITE_MEM32P(pDest +  2, ColorIndex);
          WRITE_MEM32P(pDest +  3, ColorIndex);
          WRITE_MEM32P(pDest +  4, ColorIndex);
          WRITE_MEM32P(pDest +  5, ColorIndex);
          WRITE_MEM32P(pDest +  6, ColorIndex);
          WRITE_MEM32P(pDest +  7, ColorIndex);
          WRITE_MEM32P(pDest +  8, ColorIndex);
          WRITE_MEM32P(pDest +  9, ColorIndex);
          WRITE_MEM32P(pDest + 10, ColorIndex);
          WRITE_MEM32P(pDest + 11, ColorIndex);
          WRITE_MEM32P(pDest + 12, ColorIndex);
          WRITE_MEM32P(pDest + 13, ColorIndex);
          WRITE_MEM32P(pDest + 14, ColorIndex);
          WRITE_MEM32P(pDest + 15, ColorIndex);
          pDest     += 16;
          RemItems -= 16;
        } while (RemItems >= 16);
      }
      pDest0 = pDest;
      RemPixels -= (RemPixels >> 4) << 4;
    }
    if (RemPixels) {
      for (RemLines = NumLines; RemLines; RemLines--) {
        RemItems = RemPixels;
        pDest    = pDest0 + OffLine * (RemLines - 1);
        do {
          WRITE_MEM32P(pDest, ColorIndex);
          pDest++;
        } while (--RemItems);
      }
    }
  }
}

/*************************** End of file ****************************/
