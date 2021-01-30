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
File        : LISTVIEW.h
Purpose     : LISTVIEW include
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
  
#ifndef LISTVIEW_H
#define LISTVIEW_H

#include "WM.h"
#include "DIALOG_Intern.h"      // Req. for Create indirect data structure
#include "ICONVIEW.h"
#include "HEADER.h"

#if GUI_WINSUPPORT

#if defined(__cplusplus)
  extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

/*********************************************************************
*
*       Defines
*/
#define LISTVIEW_ALL_ITEMS  -1

/*********************************************************************
*
*       Color indices
*/
#define LISTVIEW_CI_UNSEL       0
#define LISTVIEW_CI_SEL         1
#define LISTVIEW_CI_SELFOCUS    2
#define LISTVIEW_CI_DISABLED    3

/************************************************************
*
*       Create / Status flags
*/
#define LISTVIEW_CF_AUTOSCROLLBAR_H   (1 << 0)
#define LISTVIEW_CF_AUTOSCROLLBAR_V   (1 << 1)
#define LISTVIEW_CF_CELL_SELECT       (1 << 2)                     // Create Flag used to enable cell selection
#define LISTVIEW_SF_AUTOSCROLLBAR_H   LISTVIEW_CF_AUTOSCROLLBAR_H
#define LISTVIEW_SF_AUTOSCROLLBAR_V   LISTVIEW_CF_AUTOSCROLLBAR_V

/*********************************************************************
*
*       Public Types
*
**********************************************************************
*/
typedef WM_HMEM LISTVIEW_Handle;

/*********************************************************************
*
*       Create functions
*
**********************************************************************
*/
LISTVIEW_Handle LISTVIEW_Create        (int x0, int y0, int xSize, int ySize, WM_HWIN hParent, int Id, int Flags, int SpecialFlags);
LISTVIEW_Handle LISTVIEW_CreateEx      (int x0, int y0, int xSize, int ySize, WM_HWIN hParent, int WinFlags, int ExFlags, int Id);
LISTVIEW_Handle LISTVIEW_CreateUser    (int x0, int y0, int xSize, int ySize, WM_HWIN hParent, int WinFlags, int ExFlags, int Id, int NumExtraBytes);
LISTVIEW_Handle LISTVIEW_CreateAttached(WM_HWIN hParent, int Id, int SpecialFlags);
LISTVIEW_Handle LISTVIEW_CreateIndirect(const GUI_WIDGET_CREATE_INFO * pCreateInfo, WM_HWIN hWinParent, int x0, int y0, WM_CALLBACK * cb);

/*********************************************************************
*
*       The callback ...
*
* Do not call it directly ! It is only to be used from within an
* overwritten callback.
*/
void LISTVIEW_Callback(WM_MESSAGE * pMsg);

/*********************************************************************
*
*       Member functions
*
**********************************************************************
*/
int              LISTVIEW_AddColumn           (LISTVIEW_Handle hObj, int Width, const char * s, int Align);
int              LISTVIEW_AddRow              (LISTVIEW_Handle hObj, const GUI_ConstString * ppText);
int              LISTVIEW_CompareText         (const void * p0, const void * p1);
int              LISTVIEW_CompareDec          (const void * p0, const void * p1);
void             LISTVIEW_DecSel              (LISTVIEW_Handle hObj);
void             LISTVIEW_DeleteAllRows       (LISTVIEW_Handle hObj);
void             LISTVIEW_DeleteColumn        (LISTVIEW_Handle hObj, unsigned Index);
void             LISTVIEW_DeleteRow           (LISTVIEW_Handle hObj, unsigned Index);
void             LISTVIEW_DeleteRowSorted     (LISTVIEW_Handle hObj, int Row);
void             LISTVIEW_DisableRow          (LISTVIEW_Handle hObj, unsigned Row);
void             LISTVIEW_DisableSort         (LISTVIEW_Handle hObj);
void             LISTVIEW_EnableCellSelect    (LISTVIEW_Handle hObj, unsigned OnOff);  // Enables/disables cell selection
void             LISTVIEW_EnableRow           (LISTVIEW_Handle hObj, unsigned Row);
void             LISTVIEW_EnableSort          (LISTVIEW_Handle hObj);
GUI_COLOR        LISTVIEW_GetBkColor          (LISTVIEW_Handle hObj, unsigned Index);
const GUI_FONT * LISTVIEW_GetFont             (LISTVIEW_Handle hObj);
HEADER_Handle    LISTVIEW_GetHeader           (LISTVIEW_Handle hObj);
void             LISTVIEW_GetItemRect         (LISTVIEW_Handle hObj, U32 Col, U32 Row, GUI_RECT * pRect);
void             LISTVIEW_GetItemText         (LISTVIEW_Handle hObj, unsigned Column, unsigned Row, char * pBuffer, unsigned MaxSize);
unsigned         LISTVIEW_GetItemTextLen      (LISTVIEW_Handle hObj, unsigned Column, unsigned Row);
void             LISTVIEW_GetItemTextSorted   (LISTVIEW_Handle hObj, unsigned Column, unsigned Row, char * pBuffer, unsigned MaxSize);
unsigned         LISTVIEW_GetLBorder          (LISTVIEW_Handle hObj);
unsigned         LISTVIEW_GetNumColumns       (LISTVIEW_Handle hObj);
unsigned         LISTVIEW_GetNumRows          (LISTVIEW_Handle hObj);
unsigned         LISTVIEW_GetRBorder          (LISTVIEW_Handle hObj);
int              LISTVIEW_GetSel              (LISTVIEW_Handle hObj);
int              LISTVIEW_GetSelCol           (LISTVIEW_Handle hObj);
int              LISTVIEW_GetSelUnsorted      (LISTVIEW_Handle hObj);
int              LISTVIEW_GetTextAlign        (LISTVIEW_Handle hObj, unsigned ColIndex);
GUI_COLOR        LISTVIEW_GetTextColor        (LISTVIEW_Handle hObj, unsigned Index);
int              LISTVIEW_GetUserData         (LISTVIEW_Handle hObj, void * pDest, int NumBytes);
U32              LISTVIEW_GetUserDataRow      (LISTVIEW_Handle hObj, unsigned Row);
GUI_WRAPMODE     LISTVIEW_GetWrapMode         (LISTVIEW_Handle hObj);
void             LISTVIEW_IncSel              (LISTVIEW_Handle hObj);
int              LISTVIEW_InsertRow           (LISTVIEW_Handle hObj, unsigned Index, const GUI_ConstString * ppText);
int              LISTVIEW_OwnerDraw           (const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo);
unsigned         LISTVIEW_RowIsDisabled       (LISTVIEW_Handle hObj, unsigned Row);
void             LISTVIEW_SetAutoScrollH      (LISTVIEW_Handle hObj, int OnOff);
void             LISTVIEW_SetAutoScrollV      (LISTVIEW_Handle hObj, int OnOff);
void             LISTVIEW_SetItemBitmap       (LISTVIEW_Handle hObj, unsigned Column, unsigned Row, int xOff, int yOff, const GUI_BITMAP * pBitmap);
void             LISTVIEW_SetBkColor          (LISTVIEW_Handle hObj, unsigned int Index, GUI_COLOR Color);
void             LISTVIEW_SetColumnWidth      (LISTVIEW_Handle hObj, unsigned int Index, int Width);
void             LISTVIEW_SetCompareFunc      (LISTVIEW_Handle hObj, unsigned Column, int (* fpCompare)(const void * p0, const void * p1));
unsigned         LISTVIEW_SetFixed            (LISTVIEW_Handle hObj, unsigned Fixed);
void             LISTVIEW_SetFont             (LISTVIEW_Handle hObj, const GUI_FONT * pFont);
int              LISTVIEW_SetGridVis          (LISTVIEW_Handle hObj, int Show);
void             LISTVIEW_SetHeaderHeight     (LISTVIEW_Handle hObj, unsigned HeaderHeight);
void             LISTVIEW_SetItemBkColor      (LISTVIEW_Handle hObj, unsigned Column, unsigned Row, unsigned int Index, GUI_COLOR Color);
void             LISTVIEW_SetItemText         (LISTVIEW_Handle hObj, unsigned Column, unsigned Row, const char * s);
void             LISTVIEW_SetItemTextColor    (LISTVIEW_Handle hObj, unsigned Column, unsigned Row, unsigned int Index, GUI_COLOR Color);
void             LISTVIEW_SetItemTextSorted   (LISTVIEW_Handle hObj, unsigned Column, unsigned Row, const char * pText);
void             LISTVIEW_SetLBorder          (LISTVIEW_Handle hObj, unsigned BorderSize);
void             LISTVIEW_SetOwnerDraw        (LISTVIEW_Handle hObj, WIDGET_DRAW_ITEM_FUNC * pfDrawItem);
void             LISTVIEW_SetRBorder          (LISTVIEW_Handle hObj, unsigned BorderSize);
unsigned         LISTVIEW_SetRowHeight        (LISTVIEW_Handle hObj, unsigned RowHeight);
void             LISTVIEW_SetSel              (LISTVIEW_Handle hObj, int Sel);
void             LISTVIEW_SetSelCol           (LISTVIEW_Handle hObj, int NewCol);
void             LISTVIEW_SetSelUnsorted      (LISTVIEW_Handle hObj, int Sel);
unsigned         LISTVIEW_SetSort             (LISTVIEW_Handle hObj, unsigned Column, unsigned Reverse);
void             LISTVIEW_SetTextAlign        (LISTVIEW_Handle hObj, unsigned int Index, int Align);
void             LISTVIEW_SetTextColor        (LISTVIEW_Handle hObj, unsigned int Index, GUI_COLOR Color);
int              LISTVIEW_SetUserData         (LISTVIEW_Handle hObj, const void * pSrc, int NumBytes);
void             LISTVIEW_SetUserDataRow      (LISTVIEW_Handle hObj, unsigned Row, U32 UserData);
void             LISTVIEW_SetWrapMode         (LISTVIEW_Handle hObj, GUI_WRAPMODE WrapMode);

/*********************************************************************
*
*       Managing default values
*
**********************************************************************
*/

GUI_COLOR        LISTVIEW_SetDefaultBkColor  (unsigned  Index, GUI_COLOR Color);
const GUI_FONT * LISTVIEW_SetDefaultFont     (const GUI_FONT * pFont);
GUI_COLOR        LISTVIEW_SetDefaultGridColor(GUI_COLOR Color);
GUI_COLOR        LISTVIEW_SetDefaultTextColor(unsigned  Index, GUI_COLOR Color);

#if defined(__cplusplus)
  }
#endif

#endif  // GUI_WINSUPPORT
#endif  // LISTVIEW_H

/*************************** End of file ****************************/
