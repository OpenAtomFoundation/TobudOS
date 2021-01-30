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
File        : ICONVIEW.h
Purpose     : ICONVIEW include
--------------------END-OF-HEADER-------------------------------------
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
  
#ifndef ICONVIEW_H
#define ICONVIEW_H

#include "WM.h"
#include "DIALOG_Intern.h"      /* Req. for Create indirect data structure */
#include "WIDGET.h"

#if GUI_WINSUPPORT

#if defined(__cplusplus)
  extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
//
// Status- and create flags
//
#define ICONVIEW_CF_AUTOSCROLLBAR_V (1 << 1)
#define ICONVIEW_SF_AUTOSCROLLBAR_V ICONVIEW_CF_AUTOSCROLLBAR_V

//
// Color indices
//
#define ICONVIEW_CI_BK              0
#define ICONVIEW_CI_UNSEL           0
#define ICONVIEW_CI_SEL             1
#define ICONVIEW_CI_DISABLED        2

//
// Icon alignment flags, horizontal
//
#define ICONVIEW_IA_HCENTER         (0 << 0)
#define ICONVIEW_IA_LEFT            (1 << 0)
#define ICONVIEW_IA_RIGHT           (2 << 0)

//
// Icon alignment flags, vertical
//
#define ICONVIEW_IA_VCENTER         (0 << 2)
#define ICONVIEW_IA_BOTTOM          (1 << 2)
#define ICONVIEW_IA_TOP             (2 << 2)

/*********************************************************************
*
*       Types
*
**********************************************************************
*/
typedef WM_HMEM ICONVIEW_Handle;

/*********************************************************************
*
*       Public functions
*
**********************************************************************
*/
ICONVIEW_Handle ICONVIEW_CreateEx      (int x0, int y0, int xSize, int ySize, WM_HWIN hParent, int WinFlags, int ExFlags, int Id, int xSizeItems, int ySizeItems);
ICONVIEW_Handle ICONVIEW_CreateUser    (int x0, int y0, int xSize, int ySize, WM_HWIN hParent, int WinFlags, int ExFlags, int Id, int xSizeItems, int ySizeItems, int NumExtraBytes);
ICONVIEW_Handle ICONVIEW_CreateIndirect(const GUI_WIDGET_CREATE_INFO * pCreateInfo, WM_HWIN hWinParent, int x0, int y0, WM_CALLBACK * cb);

int  ICONVIEW_AddBitmapItem           (ICONVIEW_Handle hObj, const GUI_BITMAP * pBitmap, const char * pText);
int  ICONVIEW_AddBMPItem              (ICONVIEW_Handle hObj, const U8 * pBMP, const char * pText);
int  ICONVIEW_AddBMPItemEx            (ICONVIEW_Handle hObj, const void * pBMP, GUI_GET_DATA_FUNC * pfGetData, const char * pText);
int  ICONVIEW_AddStreamedBitmapItem   (ICONVIEW_Handle hObj, const void * pStreamedBitmap, const char * pText);
void ICONVIEW_DeleteItem              (ICONVIEW_Handle hObj, unsigned Index);
void ICONVIEW_EnableStreamAuto        (void);
U32  ICONVIEW_GetItemUserData         (ICONVIEW_Handle hObj, int Index);
int  ICONVIEW_GetNumItems             (ICONVIEW_Handle hObj);
int  ICONVIEW_GetItemText             (ICONVIEW_Handle hObj, int Index, char * pBuffer, int MaxSize);
int  ICONVIEW_GetSel                  (ICONVIEW_Handle hObj);
int  ICONVIEW_GetUserData             (ICONVIEW_Handle hObj, void * pDest, int NumBytes);
int  ICONVIEW_InsertBitmapItem        (ICONVIEW_Handle hObj, const GUI_BITMAP * pBitmap, const char * pText, int Index);
int  ICONVIEW_InsertBMPItem           (ICONVIEW_Handle hObj, const U8 * pBMP, const char * pText, int Index);
int  ICONVIEW_InsertBMPItemEx         (ICONVIEW_Handle hObj, const void * pBMP, GUI_GET_DATA_FUNC * pfGetData, const char * pText, int Index);
int  ICONVIEW_InsertStreamedBitmapItem(ICONVIEW_Handle hObj, const void * pStreamedBitmap, const char * pText, int Index);
int  ICONVIEW_SetBitmapItem           (ICONVIEW_Handle hObj, int Index, const GUI_BITMAP * pBitmap);
void ICONVIEW_SetBkColor              (ICONVIEW_Handle hObj, int Index, GUI_COLOR Color);
int  ICONVIEW_SetBMPItem              (ICONVIEW_Handle hObj, const U8 * pBMP, int Index);
int  ICONVIEW_SetBMPItemEx            (ICONVIEW_Handle hObj, const void * pBMP, GUI_GET_DATA_FUNC * pfGetData, int Index);
void ICONVIEW_SetFont                 (ICONVIEW_Handle hObj, const GUI_FONT * pFont);
void ICONVIEW_SetFrame                (ICONVIEW_Handle hObj, int Coord, int Value);
void ICONVIEW_SetItemText             (ICONVIEW_Handle hObj, int Index, const char * pText);
void ICONVIEW_SetItemUserData         (ICONVIEW_Handle hObj, int Index, U32 UserData);
void ICONVIEW_SetSel                  (ICONVIEW_Handle hObj, int Sel);
void ICONVIEW_SetSpace                (ICONVIEW_Handle hObj, int Coord, int Value);
int  ICONVIEW_SetStreamedBitmapItem   (ICONVIEW_Handle hObj, int Index, const void * pStreamedBitmap);
void ICONVIEW_SetIconAlign            (ICONVIEW_Handle hObj, int IconAlign);
void ICONVIEW_SetTextAlign            (ICONVIEW_Handle hObj, int TextAlign);
void ICONVIEW_SetTextColor            (ICONVIEW_Handle hObj, int Index, GUI_COLOR Color);
int  ICONVIEW_SetUserData             (ICONVIEW_Handle hObj, const void * pSrc, int NumBytes);
void ICONVIEW_SetWrapMode             (ICONVIEW_Handle hObj, GUI_WRAPMODE WrapMode);

void ICONVIEW_Callback(WM_MESSAGE * pMsg);

#if defined(__cplusplus)
  }
#endif

#endif  // GUI_WINSUPPORT
#endif  // ICONVIEW_H

/*************************** End of file ****************************/
