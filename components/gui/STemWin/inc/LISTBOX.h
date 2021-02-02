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
File        : LISTBOX.h
Purpose     : LISTBOX widget include
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
  
#ifndef LISTBOX_H
#define LISTBOX_H

#include "WM.h"
#include "WIDGET.h"             /* Req. for WIDGET_DRAW_ITEM_FUNC */
#include "DIALOG_Intern.h"      /* Req. for Create indirect data structure */

#if GUI_WINSUPPORT

#if defined(__cplusplus)
  extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

/*********************************************************************
*
*       defines
*
**********************************************************************
*/

#define LISTBOX_ALL_ITEMS  -1

/*********************************************************************
*
*       Color indices
*/
#define LISTBOX_CI_UNSEL    0
#define LISTBOX_CI_SEL      1
#define LISTBOX_CI_SELFOCUS 2
#define LISTBOX_CI_DISABLED 3

/************************************************************
*
*       States
*/
typedef WM_HMEM LISTBOX_Handle;

/*********************************************************************
*
*               Notification codes
*
* The following is the list of notification codes specific to this widget,
* Send with the WM_NOTIFY_PARENT message
*/
#define LISTBOX_NOTIFICATION_LOST_FOCUS    (WM_NOTIFICATION_WIDGET + 0)

/************************************************************
*
*       Create / Status flags
*/
#define LISTBOX_CF_AUTOSCROLLBAR_H   (1<<0)
#define LISTBOX_CF_AUTOSCROLLBAR_V   (1<<1)
#define LISTBOX_CF_MULTISEL          (1<<2)
#define LISTBOX_CF_WRAP              (1<<3)
#define LISTBOX_SF_AUTOSCROLLBAR_H   LISTBOX_CF_AUTOSCROLLBAR_H
#define LISTBOX_SF_AUTOSCROLLBAR_V   LISTBOX_CF_AUTOSCROLLBAR_V
#define LISTBOX_SF_MULTISEL          LISTBOX_CF_MULTISEL
#define LISTBOX_SF_WRAP              LISTBOX_CF_WRAP

/*********************************************************************
*
*       Create functions
*
**********************************************************************
*/

LISTBOX_Handle LISTBOX_Create        (const GUI_ConstString * ppText, int x0, int y0, int xSize, int ySize, int Flags);
LISTBOX_Handle LISTBOX_CreateAsChild (const GUI_ConstString * ppText, WM_HWIN hWinParent, int x0, int y0, int xSize, int ySize, int Flags);
LISTBOX_Handle LISTBOX_CreateEx      (int x0, int y0, int xSize, int ySize, WM_HWIN hParent, int WinFlags, int ExFlags, int Id, const GUI_ConstString * ppText);
LISTBOX_Handle LISTBOX_CreateUser    (int x0, int y0, int xSize, int ySize, WM_HWIN hParent, int WinFlags, int ExFlags, int Id, const GUI_ConstString * ppText, int NumExtraBytes);
LISTBOX_Handle LISTBOX_CreateIndirect(const GUI_WIDGET_CREATE_INFO * pCreateInfo, WM_HWIN hWinParent, int x0, int y0, WM_CALLBACK * cb);

/*********************************************************************
*
*       The callback ...
*
* Do not call it directly ! It is only to be used from within an
* overwritten callback.
*/
void LISTBOX_Callback(WM_MESSAGE * pMsg);

/*********************************************************************
*
*       Member functions
*
**********************************************************************
*/

int          LISTBOX_AddKey          (LISTBOX_Handle hObj, int Key);
void         LISTBOX_AddString       (LISTBOX_Handle hObj, const char * s);
void         LISTBOX_AddStringH      (LISTBOX_Handle hObj, WM_HMEM hString); /* Not to be documented!!! */
void         LISTBOX_DecSel          (LISTBOX_Handle hObj);
void         LISTBOX_DeleteItem      (LISTBOX_Handle hObj, unsigned int Index);
void         LISTBOX_EnableWrapMode  (LISTBOX_Handle hObj, int OnOff);
unsigned     LISTBOX_GetItemSpacing  (LISTBOX_Handle hObj);
unsigned     LISTBOX_GetNumItems     (LISTBOX_Handle hObj);
int          LISTBOX_GetSel          (LISTBOX_Handle hObj);
const GUI_FONT * LISTBOX_GetFont     (LISTBOX_Handle hObj);
int          LISTBOX_GetItemDisabled (LISTBOX_Handle hObj, unsigned Index);
int          LISTBOX_GetItemSel      (LISTBOX_Handle hObj, unsigned Index);
void         LISTBOX_GetItemText     (LISTBOX_Handle hObj, unsigned Index, char * pBuffer, int MaxSize);
int          LISTBOX_GetMulti        (LISTBOX_Handle hObj);
int          LISTBOX_GetScrollStepH  (LISTBOX_Handle hObj);
int          LISTBOX_GetTextAlign    (LISTBOX_Handle hObj);
int          LISTBOX_GetUserData     (LISTBOX_Handle hObj, void * pDest, int NumBytes);
void         LISTBOX_IncSel          (LISTBOX_Handle hObj);
void         LISTBOX_InsertString    (LISTBOX_Handle hObj, const char * s, unsigned int Index);
void         LISTBOX_InvalidateItem  (LISTBOX_Handle hObj, int Index);
int          LISTBOX_OwnerDraw       (const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo);
void         LISTBOX_SetAutoScrollH  (LISTBOX_Handle hObj, int OnOff);
void         LISTBOX_SetAutoScrollV  (LISTBOX_Handle hObj, int OnOff);
void         LISTBOX_SetBkColor      (LISTBOX_Handle hObj, unsigned int Index, GUI_COLOR color);
void         LISTBOX_SetFont         (LISTBOX_Handle hObj, const GUI_FONT * pFont);
void         LISTBOX_SetItemDisabled (LISTBOX_Handle hObj, unsigned Index, int OnOff);
void         LISTBOX_SetItemSel      (LISTBOX_Handle hObj, unsigned Index, int OnOff);
void         LISTBOX_SetItemSpacing  (LISTBOX_Handle hObj, unsigned Value);
void         LISTBOX_SetMulti        (LISTBOX_Handle hObj, int Mode);
void         LISTBOX_SetOwner        (LISTBOX_Handle hObj, WM_HWIN hOwner);
void         LISTBOX_SetOwnerDraw    (LISTBOX_Handle hObj, WIDGET_DRAW_ITEM_FUNC * pfDrawItem);
void         LISTBOX_SetScrollStepH  (LISTBOX_Handle hObj, int Value);
void         LISTBOX_SetSel          (LISTBOX_Handle hObj, int Sel);
void         LISTBOX_SetScrollbarColor(LISTBOX_Handle hObj, unsigned Index, GUI_COLOR Color);
void         LISTBOX_SetScrollbarWidth(LISTBOX_Handle hObj, unsigned Width);
void         LISTBOX_SetString       (LISTBOX_Handle hObj, const char * s, unsigned int Index);
void         LISTBOX_SetText         (LISTBOX_Handle hObj, const GUI_ConstString * ppText);
void         LISTBOX_SetTextAlign    (LISTBOX_Handle hObj, int Align);
GUI_COLOR    LISTBOX_SetTextColor    (LISTBOX_Handle hObj, unsigned int Index, GUI_COLOR Color);
int          LISTBOX_SetUserData     (LISTBOX_Handle hObj, const void * pSrc, int NumBytes);
int          LISTBOX_UpdateScrollers (LISTBOX_Handle hObj);

/*********************************************************************
*
*       Managing default values
*
**********************************************************************
*/

const GUI_FONT * LISTBOX_GetDefaultFont(void);
int              LISTBOX_GetDefaultScrollStepH (void);
GUI_COLOR        LISTBOX_GetDefaultBkColor     (unsigned Index);
int              LISTBOX_GetDefaultTextAlign   (void);
GUI_COLOR        LISTBOX_GetDefaultTextColor   (unsigned Index);
void             LISTBOX_SetDefaultFont        (const GUI_FONT * pFont);
void             LISTBOX_SetDefaultScrollStepH (int Value);
void             LISTBOX_SetDefaultBkColor     (unsigned Index, GUI_COLOR Color);
void             LISTBOX_SetDefaultTextAlign   (int Align);
void             LISTBOX_SetDefaultTextColor   (unsigned Index, GUI_COLOR Color);

/*********************************************************************
*
*       Compatibility to older versions
*
**********************************************************************
*/

#define LISTBOX_SetBackColor(hObj, Index, Color) LISTBOX_SetBkColor(hObj, Index, Color)
#define LISTBOX_DeleteString  LISTBOX_DeleteItem

#if defined(__cplusplus)
  }
#endif

#endif  // GUI_WINSUPPORT
#endif  // LISTBOX_H

/*************************** End of file ****************************/
