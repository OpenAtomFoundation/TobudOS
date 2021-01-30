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
File        : MULTIEDIT.h
Purpose     : MULTIEDIT include
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
  
#ifndef MULTIEDIT_H
#define MULTIEDIT_H

#include "WM.h"
#include "DIALOG_Intern.h"      /* Req. for Create indirect data structure */

#if GUI_WINSUPPORT

#if defined(__cplusplus)
  extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

#define MULTIEDIT_CF_READONLY        (1 << 0)
#define MULTIEDIT_CF_INSERT          (1 << 2)
#define MULTIEDIT_CF_AUTOSCROLLBAR_V (1 << 3)
#define MULTIEDIT_CF_AUTOSCROLLBAR_H (1 << 4)
#define MULTIEDIT_CF_PASSWORD        (1 << 5)

#define MULTIEDIT_SF_READONLY        MULTIEDIT_CF_READONLY
#define MULTIEDIT_SF_INSERT          MULTIEDIT_CF_INSERT
#define MULTIEDIT_SF_AUTOSCROLLBAR_V MULTIEDIT_CF_AUTOSCROLLBAR_V
#define MULTIEDIT_SF_AUTOSCROLLBAR_H MULTIEDIT_CF_AUTOSCROLLBAR_H
#define MULTIEDIT_SF_PASSWORD        MULTIEDIT_CF_PASSWORD

/*********************************************************************
*
*       Color indices
*/
#define MULTIEDIT_CI_EDIT     0
#define MULTIEDIT_CI_READONLY 1

/*********************************************************************
*
*                         Public Types
*
**********************************************************************
*/

typedef WM_HMEM MULTIEDIT_HANDLE;

/*********************************************************************
*
*                 Create functions
*
**********************************************************************
*/
MULTIEDIT_HANDLE MULTIEDIT_Create        (int x0, int y0, int xSize, int ySize, WM_HWIN hParent, int Id, int Flags, int ExFlags, const char * pText, int MaxLen);
MULTIEDIT_HANDLE MULTIEDIT_CreateEx      (int x0, int y0, int xSize, int ySize, WM_HWIN hParent, int WinFlags, int ExFlags, int Id, int BufferSize, const char * pText);
MULTIEDIT_HANDLE MULTIEDIT_CreateIndirect(const GUI_WIDGET_CREATE_INFO* pCreateInfo, WM_HWIN hWinParent, int x0, int y0, WM_CALLBACK * cb);
MULTIEDIT_HANDLE MULTIEDIT_CreateUser    (int x0, int y0, int xSize, int ySize, WM_HWIN hParent, int WinFlags, int ExFlags, int Id, int BufferSize, const char * pText, int NumExtraBytes);

/*********************************************************************
*
*       The callback ...
*
* Do not call it directly ! It is only to be used from within an
* overwritten callback.
*/
void MULTIEDIT_Callback(WM_MESSAGE * pMsg);

/*********************************************************************
*
*                 Member functions
*
**********************************************************************
*/

int  MULTIEDIT_AddKey           (MULTIEDIT_HANDLE hObj, U16 Key);
int  MULTIEDIT_AddText          (MULTIEDIT_HANDLE hObj, const char * s);
void MULTIEDIT_EnableBlink      (MULTIEDIT_HANDLE hObj, int Period, int OnOff);
int  MULTIEDIT_GetCursorCharPos (MULTIEDIT_HANDLE hObj);
void MULTIEDIT_GetCursorPixelPos(MULTIEDIT_HANDLE hObj, int * pxPos, int * pyPos);
void MULTIEDIT_GetPrompt        (MULTIEDIT_HANDLE hObj, char* sDest, int MaxNumChars);
int  MULTIEDIT_GetTextSize      (MULTIEDIT_HANDLE hObj);
void MULTIEDIT_GetText          (MULTIEDIT_HANDLE hObj, char* sDest, int MaxNumChars);
int  MULTIEDIT_GetUserData      (MULTIEDIT_HANDLE hObj, void * pDest, int NumBytes);
void MULTIEDIT_SetTextAlign     (MULTIEDIT_HANDLE hObj, int Align);
void MULTIEDIT_SetAutoScrollH   (MULTIEDIT_HANDLE hObj, int OnOff);
void MULTIEDIT_SetAutoScrollV   (MULTIEDIT_HANDLE hObj, int OnOff);
void MULTIEDIT_SetBkColor       (MULTIEDIT_HANDLE hObj, unsigned Index, GUI_COLOR color);
void MULTIEDIT_SetCursorCharPos (MULTIEDIT_HANDLE hObj, int x, int y);       /* Not yet implemented */
void MULTIEDIT_SetCursorPixelPos(MULTIEDIT_HANDLE hObj, int x, int y);       /* Not yet implemented */
void MULTIEDIT_SetCursorOffset  (MULTIEDIT_HANDLE hObj, int Offset);
void MULTIEDIT_SetHBorder       (MULTIEDIT_HANDLE hObj, unsigned HBorder);
void MULTIEDIT_SetFocussable    (MULTIEDIT_HANDLE hObj, int State);
void MULTIEDIT_SetFont          (MULTIEDIT_HANDLE hObj, const GUI_FONT * pFont);
void MULTIEDIT_SetInsertMode    (MULTIEDIT_HANDLE hObj, int OnOff);
void MULTIEDIT_SetBufferSize    (MULTIEDIT_HANDLE hObj, int BufferSize);
void MULTIEDIT_SetMaxNumChars   (MULTIEDIT_HANDLE hObj, unsigned MaxNumChars);
void MULTIEDIT_SetPrompt        (MULTIEDIT_HANDLE hObj, const char* sPrompt);
void MULTIEDIT_SetReadOnly      (MULTIEDIT_HANDLE hObj, int OnOff);
void MULTIEDIT_SetPasswordMode  (MULTIEDIT_HANDLE hObj, int OnOff);
void MULTIEDIT_SetText          (MULTIEDIT_HANDLE hObj, const char* s);
void MULTIEDIT_SetTextColor     (MULTIEDIT_HANDLE hObj, unsigned Index, GUI_COLOR color);
int  MULTIEDIT_SetUserData      (MULTIEDIT_HANDLE hObj, const void * pSrc, int NumBytes);
void MULTIEDIT_SetWrapNone      (MULTIEDIT_HANDLE hObj);
void MULTIEDIT_SetWrapChar      (MULTIEDIT_HANDLE hObj);
void MULTIEDIT_SetWrapWord      (MULTIEDIT_HANDLE hObj);

/*********************************************************************
*
*       Macros for compatibility with older versions
*
**********************************************************************
*/

#define MULTIEDIT_SetMaxLen(hObj, MaxLen) MULTIEDIT_SetBufferSize(hObj, MaxLen)
#define MULTIEDIT_GetStringSize           MULTIEDIT_GetTextSize

#if defined(__cplusplus)
  }
#endif

#endif  // GUI_WINSUPPORT
#endif  // MULTIEDIT_H

/*************************** End of file ****************************/
