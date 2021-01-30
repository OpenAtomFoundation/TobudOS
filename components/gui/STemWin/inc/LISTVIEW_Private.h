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
File        : LISTVIEW_Private.h
Purpose     : Private LISTVIEW include
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
  
#ifndef LISTVIEW_PRIVATE_H
#define LISTVIEW_PRIVATE_H

#include "WM.h"

#if GUI_WINSUPPORT

#include <stddef.h>

#include "LISTVIEW.h"
#include "GUI_ARRAY.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define SORT_TYPE                 U16

#define LISTVIEW_CELL_INFO_COLORS (1 << 0)
#define LISTVIEW_CELL_INFO_BITMAP (1 << 1)

/*********************************************************************
*
*       Object definition
*
**********************************************************************
*/

typedef struct {
  GUI_COLOR                    aBkColor[4];
  GUI_COLOR                    aTextColor[4];
  GUI_COLOR                    GridColor;
  const GUI_FONT             * pFont;
  U16                          ScrollStepH;
  GUI_WRAPMODE                 WrapMode;
  int                          DefaultAlign;
} LISTVIEW_PROPS;

typedef struct {
  void                      (* pfDraw)(LISTVIEW_Handle hObj, unsigned Column, unsigned Row, GUI_RECT * pRect);
  void                       * pData;
  GUI_COLOR                    aBkColor[4];
  GUI_COLOR                    aTextColor[4];
  I16                          xOff, yOff;
  U8                           Flags;
} LISTVIEW_CELL_INFO;

typedef struct {
  WM_HMEM                      hCellInfo;                                         // Optional cell info. If 0, there is no cell info. (Defaults used)
  char                         acText[1];
} LISTVIEW_CELL;

typedef struct {
  GUI_ARRAY                    hCellArray;
  char                         Disabled;
  U32                          UserData;
} LISTVIEW_ROW;

typedef struct {
  U8                           Align;
  int                       (* fpCompare)    (const void * p0, const void * p1);  // User function to be called to compare the contents of 2 cells
} LISTVIEW_COLUMN;

typedef struct LISTVIEW_Obj LISTVIEW_Obj;

typedef struct {
  WM_HMEM                      hSortArray;
  SORT_TYPE                    SortArrayNumItems;
  int                       (* fpSort)(LISTVIEW_Handle hObj);                     // Function to be called to get a sorted array
  void                      (* fpFree)(WM_HMEM hObj);                             // Function to be called to free the sort object
  U8                           Reverse;
} LISTVIEW_SORT;

struct LISTVIEW_Obj {
  WIDGET                       Widget;
  HEADER_Handle                hHeader;
  WIDGET_DRAW_ITEM_FUNC *      pfDrawItem;
  GUI_ARRAY                    hRowArray;                                         // Each entry is a handle of LISTVIEW_ROW structure.
  GUI_ARRAY                    hColumnArray;                                      // Each entry is a handle of LISTVIEW_COLUMN structure.
  LISTVIEW_PROPS               Props;
  int                          Sel;
  int                          SelCol;
  int                          ShowGrid;
  int                          SortIndex;                                         // Column for sorting
  unsigned                     RowDistY;
  unsigned                     LBorder;
  unsigned                     RBorder;
  unsigned                     Fixed;
  WM_SCROLL_STATE              ScrollStateV;
  WM_SCROLL_STATE              ScrollStateH;
  WM_HMEM                      hSort;
  U8                           IsSorted;
  U8                           IsPresorted;
  U8                           ReverseSort;                                       // Set to 1 if reverse sorting is required
  U8                           Flags;
};

/*********************************************************************
*
*       Macros for internal use
*
**********************************************************************
*/
#if GUI_DEBUG_LEVEL >= GUI_DEBUG_LEVEL_CHECK_ALL
  #define LISTVIEW_INIT_ID(p) (p->Widget.DebugId = LISTVIEW_ID)
#else
  #define LISTVIEW_INIT_ID(p)
#endif

#if GUI_DEBUG_LEVEL >= GUI_DEBUG_LEVEL_CHECK_ALL
  LISTVIEW_Obj * LISTVIEW_LockH(LISTVIEW_Handle h);
  #define LISTVIEW_LOCK_H(h)   LISTVIEW_LockH(h)
#else
  #define LISTVIEW_LOCK_H(h)   (LISTVIEW_Obj *)GUI_LOCK_H(h)
#endif

/*********************************************************************
*
*       Private (module internal) data
*
**********************************************************************
*/
extern LISTVIEW_PROPS LISTVIEW_DefaultProps;

/*********************************************************************
*
*       Private (module internal) functions
*
**********************************************************************
*/
LISTVIEW_CELL_INFO * LISTVIEW__CreateCellInfoLocked (LISTVIEW_Handle   hObj, unsigned Column, unsigned Row);
unsigned             LISTVIEW__GetNumColumns        (LISTVIEW_Obj    * pObj);
unsigned             LISTVIEW__GetNumRows           (LISTVIEW_Obj    * pObj);
LISTVIEW_CELL_INFO * LISTVIEW__GetpCellInfo         (LISTVIEW_Handle   hObj, unsigned Column, unsigned Row);
LISTVIEW_ROW       * LISTVIEW__GetpRow              (LISTVIEW_Handle   hObj, int Row);
unsigned             LISTVIEW__GetRowDistY          (LISTVIEW_Obj    * pObj);
unsigned             LISTVIEW__GetRowSorted         (LISTVIEW_Handle   hObj, int Row);
void                 LISTVIEW__InvalidateInsideArea (LISTVIEW_Handle   hObj);
void                 LISTVIEW__InvalidateRow        (LISTVIEW_Handle   hObj, int Sel);
void                 LISTVIEW__InvalidateRowAndBelow(LISTVIEW_Handle   hObj, int Sel);
void                 LISTVIEW__SetSel               (LISTVIEW_Handle   hObj, int NewSel);
void                 LISTVIEW__SetSelCol            (LISTVIEW_Handle   hObj, int NewSelCol);
int                  LISTVIEW__UpdateScrollParas    (LISTVIEW_Handle   hObj);

#endif // GUI_WINSUPPORT
#endif // LISTVIEW_PRIVATE_H

/*************************** End of file ****************************/
