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
File        : TREEVIEW.h
Purpose     : TREEVIEW include
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
  
#ifndef TREEVIEW_H
#define TREEVIEW_H

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
/* Status- and create flags */
#define TREEVIEW_CF_HIDELINES       (1 << 0)
#define TREEVIEW_CF_ROWSELECT       (1 << 1)
#define TREEVIEW_CF_AUTOSCROLLBAR_H (1 << 2)
#define TREEVIEW_CF_AUTOSCROLLBAR_V (1 << 3)
#define TREEVIEW_SF_HIDELINES       TREEVIEW_CF_HIDELINES
#define TREEVIEW_SF_ROWSELECT       TREEVIEW_CF_ROWSELECT
#define TREEVIEW_SF_AUTOSCROLLBAR_H TREEVIEW_CF_AUTOSCROLLBAR_H
#define TREEVIEW_SF_AUTOSCROLLBAR_V TREEVIEW_CF_AUTOSCROLLBAR_V

/* Bitmap indices */
#define TREEVIEW_BI_CLOSED 0
#define TREEVIEW_BI_OPEN   1
#define TREEVIEW_BI_LEAF   2
#define TREEVIEW_BI_PLUS   3
#define TREEVIEW_BI_MINUS  4
#define TREEVIEW_BI_PM     5

/* Color indices */
#define TREEVIEW_CI_UNSEL    0
#define TREEVIEW_CI_SEL      1
#define TREEVIEW_CI_DISABLED 2

/* Relative positions (create) */
#define TREEVIEW_INSERT_ABOVE       0
#define TREEVIEW_INSERT_BELOW       1
#define TREEVIEW_INSERT_FIRST_CHILD 2

/* Relative positions (retrieve) */
#define TREEVIEW_GET_FIRST        0
#define TREEVIEW_GET_LAST         1
#define TREEVIEW_GET_NEXT_SIBLING 2
#define TREEVIEW_GET_PREV_SIBLING 3
#define TREEVIEW_GET_FIRST_CHILD  4
#define TREEVIEW_GET_PARENT       5

/* Item flags */
#define TREEVIEW_ITEM_TYPE_LEAF (0 << 0)
#define TREEVIEW_ITEM_TYPE_NODE (1 << 0)

/* Selection mode */
#define TREEVIEW_SELMODE_ROW  1
#define TREEVIEW_SELMODE_TEXT 0

/************************************************************
*
*       Types
*
*************************************************************
*/
typedef WM_HMEM TREEVIEW_Handle;
typedef WM_HMEM TREEVIEW_ITEM_Handle;

typedef struct {
  int IsNode;
  int IsExpanded;
  int HasLines;
  int HasRowSelect;
  int Level;
} TREEVIEW_ITEM_INFO;

typedef struct {
  GUI_COLOR ColorBk;
  GUI_COLOR ColorText;
  GUI_COLOR ColorTextBk;
  GUI_COLOR ColorLines;
  GUI_RECT rText;
  TREEVIEW_ITEM_Handle hItem;
  const GUI_FONT * pFont;
  char * pText;
  U8 NumLines;
  I16 ax0[3];
  I16 ay0[3];
  I16 ax1[3];
  I16 ay1[3];
  U8 NumConnectors;
  I16 axc[16];
  const GUI_BITMAP * pBmPM;
  const GUI_BITMAP * pBmOCL;
  I16 xPosPM, xPosOCL;
  U8 IndexPM;
  U8 IndexOCL;
} TREEVIEW_ITEM_DRAW_INFO;

/*********************************************************************
*
*       Create functions
*
**********************************************************************
*/
TREEVIEW_Handle      TREEVIEW_CreateEx      (int x0, int y0, int xSize, int ySize, WM_HWIN hParent, int WinFlags, int ExFlags, int Id);
TREEVIEW_Handle      TREEVIEW_CreateUser    (int x0, int y0, int xSize, int ySize, WM_HWIN hParent, int WinFlags, int ExFlags, int Id, int NumExtraBytes);
TREEVIEW_Handle      TREEVIEW_CreateIndirect(const GUI_WIDGET_CREATE_INFO * pCreateInfo, WM_HWIN hWinParent, int x0, int y0, WM_CALLBACK * cb);

/*********************************************************************
*
*       The callback ...
*
* Do not call it directly ! It is only to be used from within an
* overwritten callback.
*/
void TREEVIEW_Callback(WM_MESSAGE * pMsg);

/*********************************************************************
*
*       Common functions
*
**********************************************************************
*/
int                  TREEVIEW_AttachItem     (TREEVIEW_Handle hObj, TREEVIEW_ITEM_Handle hItem, TREEVIEW_ITEM_Handle hItemAt, int Position);
void                 TREEVIEW_DecSel         (TREEVIEW_Handle hObj);
TREEVIEW_ITEM_Handle TREEVIEW_GetItem        (TREEVIEW_Handle hObj, TREEVIEW_ITEM_Handle hItem, int Flags);
TREEVIEW_ITEM_Handle TREEVIEW_GetSel         (TREEVIEW_Handle hObj);
int                  TREEVIEW_GetUserData    (TREEVIEW_Handle hObj, void * pDest, int NumBytes);
void                 TREEVIEW_IncSel         (TREEVIEW_Handle hObj);
TREEVIEW_ITEM_Handle TREEVIEW_InsertItem     (TREEVIEW_Handle hObj, int IsNode, TREEVIEW_ITEM_Handle hItemPrev, int Position, const char * s);
int                  TREEVIEW_OwnerDraw      (const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo);
void                 TREEVIEW_ScrollToSel    (TREEVIEW_Handle hObj);
void                 TREEVIEW_SetAutoScrollH (TREEVIEW_Handle hObj, int State);
void                 TREEVIEW_SetAutoScrollV (TREEVIEW_Handle hObj, int State);
void                 TREEVIEW_SetBitmapOffset(TREEVIEW_Handle hObj, int Index, int xOff, int yOff);
void                 TREEVIEW_SetBkColor     (TREEVIEW_Handle hObj, int Index, GUI_COLOR Color);
void                 TREEVIEW_SetFont        (TREEVIEW_Handle hObj, const GUI_FONT * pFont);
void                 TREEVIEW_SetHasLines    (TREEVIEW_Handle hObj, int State);
void                 TREEVIEW_SetImage       (TREEVIEW_Handle hObj, int Index, const GUI_BITMAP * pBitmap);
int                  TREEVIEW_SetIndent      (TREEVIEW_Handle hObj, int Indent);
void                 TREEVIEW_SetLineColor   (TREEVIEW_Handle hObj, int Index, GUI_COLOR Color);
void                 TREEVIEW_SetOwnerDraw   (TREEVIEW_Handle hObj, WIDGET_DRAW_ITEM_FUNC * pfDrawItem);
void                 TREEVIEW_SetSel         (TREEVIEW_Handle hObj, TREEVIEW_ITEM_Handle hItem);
void                 TREEVIEW_SetSelMode     (TREEVIEW_Handle hObj, int Mode);
void                 TREEVIEW_SetTextColor   (TREEVIEW_Handle hObj, int Index, GUI_COLOR Color);
int                  TREEVIEW_SetTextIndent  (TREEVIEW_Handle hObj, int TextIndent);
int                  TREEVIEW_SetUserData    (TREEVIEW_Handle hObj, const void * pSrc, int NumBytes);


/*********************************************************************
*
*       Treeview item related functions
*
**********************************************************************
*/
void                 TREEVIEW_ITEM_Collapse   (TREEVIEW_ITEM_Handle hItem);
void                 TREEVIEW_ITEM_CollapseAll(TREEVIEW_ITEM_Handle hItem);
TREEVIEW_ITEM_Handle TREEVIEW_ITEM_Create     (int IsNode, const char * s, U32 UserData);
void                 TREEVIEW_ITEM_Delete     (TREEVIEW_ITEM_Handle hItem);
void                 TREEVIEW_ITEM_Detach     (TREEVIEW_ITEM_Handle hItem);
void                 TREEVIEW_ITEM_Expand     (TREEVIEW_ITEM_Handle hItem);
void                 TREEVIEW_ITEM_ExpandAll  (TREEVIEW_ITEM_Handle hItem);
void                 TREEVIEW_ITEM_GetInfo    (TREEVIEW_ITEM_Handle hItem, TREEVIEW_ITEM_INFO * pInfo);
void                 TREEVIEW_ITEM_GetText    (TREEVIEW_ITEM_Handle hItem, U8 * pBuffer, int MaxNumBytes);
U32                  TREEVIEW_ITEM_GetUserData(TREEVIEW_ITEM_Handle hItem);
void                 TREEVIEW_ITEM_SetImage   (TREEVIEW_ITEM_Handle hItem, int Index, const GUI_BITMAP * pBitmap);
TREEVIEW_ITEM_Handle TREEVIEW_ITEM_SetText    (TREEVIEW_ITEM_Handle hItem, const char * s);
void                 TREEVIEW_ITEM_SetUserData(TREEVIEW_ITEM_Handle hItem, U32 UserData);

/*********************************************************************
*
*       Managing default values
*
**********************************************************************
*/
GUI_COLOR        TREEVIEW_GetDefaultBkColor  (int Index);
const GUI_FONT * TREEVIEW_GetDefaultFont     (void);
GUI_COLOR        TREEVIEW_GetDefaultLineColor(int Index);
GUI_COLOR        TREEVIEW_GetDefaultTextColor(int Index);
void             TREEVIEW_SetDefaultBkColor  (int Index, GUI_COLOR Color);
void             TREEVIEW_SetDefaultFont     (const GUI_FONT * pFont);
void             TREEVIEW_SetDefaultLineColor(int Index, GUI_COLOR Color);
void             TREEVIEW_SetDefaultTextColor(int Index, GUI_COLOR Color);

#if defined(__cplusplus)
  }
#endif

#endif  // GUI_WINSUPPORT
#endif  // TREEVIEW_H

/*************************** End of file ****************************/
