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
File        : GRAPH.h
Purpose     : GRAPH include
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
  
#ifndef GRAPH_H
#define GRAPH_H

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
#define GRAPH_CI_BK                  0
#define GRAPH_CI_BORDER              1
#define GRAPH_CI_FRAME               2
#define GRAPH_CI_GRID                3

#define GRAPH_SCALE_CF_HORIZONTAL    (0 << 0)
#define GRAPH_SCALE_CF_VERTICAL      (1 << 0)

#define GRAPH_SCALE_SF_HORIZONTAL    GRAPH_SCALE_CF_HORIZONTAL
#define GRAPH_SCALE_SF_VERTICAL      GRAPH_SCALE_CF_VERTICAL

#define GRAPH_DRAW_FIRST             0
#define GRAPH_DRAW_AFTER_BORDER      1
#define GRAPH_DRAW_LAST              2

#define GRAPH_ALIGN_RIGHT            (0 << 0)
#define GRAPH_ALIGN_LEFT             (1 << 0)

//
// Creation flags (ExFlags)
//
#define GRAPH_CF_GRID_FIXED_X        (1 << 0)
#define GRAPH_CF_AVOID_SCROLLBAR_H   (1 << 1)
#define GRAPH_CF_AVOID_SCROLLBAR_V   (1 << 2)

//
// Status flags
//
#define GRAPH_SF_AVOID_SCROLLBAR_H   GRAPH_CF_AVOID_SCROLLBAR_H
#define GRAPH_SF_AVOID_SCROLLBAR_V   GRAPH_CF_AVOID_SCROLLBAR_V

/*********************************************************************
*
*       Public Types
*
**********************************************************************
*/
typedef WM_HMEM GRAPH_Handle;
typedef WM_HMEM GRAPH_DATA_Handle;
typedef WM_HMEM GRAPH_SCALE_Handle;

/*********************************************************************
*
*       Create functions
*
**********************************************************************
*/

GRAPH_Handle GRAPH_CreateEx      (int x0, int y0, int xSize, int ySize, WM_HWIN hParent, int WinFlags, int ExFlags, int Id);
GRAPH_Handle GRAPH_CreateUser    (int x0, int y0, int xSize, int ySize, WM_HWIN hParent, int WinFlags, int ExFlags, int Id, int NumExtraBytes);
GRAPH_Handle GRAPH_CreateIndirect(const GUI_WIDGET_CREATE_INFO * pCreateInfo, WM_HWIN hWinParent, int x0, int y0, WM_CALLBACK * cb);

GRAPH_DATA_Handle  GRAPH_DATA_XY_Create(GUI_COLOR Color, unsigned MaxNumItems, GUI_POINT * pData, unsigned NumItems);
GRAPH_DATA_Handle  GRAPH_DATA_YT_Create(GUI_COLOR Color, unsigned MaxNumItems, I16 * pData, unsigned NumItems);
GRAPH_SCALE_Handle GRAPH_SCALE_Create  (int Pos, int TextAlign, unsigned Flags, unsigned TickDist);

/*********************************************************************
*
*       The callback ...
*
* Do not call it directly ! It is only to be used from within an
* overwritten callback.
*/
void GRAPH_Callback(WM_MESSAGE * pMsg);

/*********************************************************************
*
*       Member functions
*
**********************************************************************
*/
void      GRAPH_AttachData             (GRAPH_Handle hObj, GRAPH_DATA_Handle hData);
void      GRAPH_AttachScale            (GRAPH_Handle hObj, GRAPH_SCALE_Handle hScale);
void      GRAPH_DetachData             (GRAPH_Handle hObj, GRAPH_DATA_Handle hData);
void      GRAPH_DetachScale            (GRAPH_Handle hObj, GRAPH_SCALE_Handle hScale);
I32       GRAPH_GetScrollValue         (GRAPH_Handle hObj, U8 Coord);
int       GRAPH_GetUserData            (GRAPH_Handle hObj, void * pDest, int NumBytes);
void      GRAPH_SetAutoScrollbar       (GRAPH_Handle hObj, U8 Coord, U8 OnOff);
void      GRAPH_SetBorder              (GRAPH_Handle hObj, unsigned BorderL, unsigned BorderT, unsigned BorderR, unsigned BorderB);
GUI_COLOR GRAPH_SetColor               (GRAPH_Handle hObj, GUI_COLOR Color, unsigned Index);
unsigned  GRAPH_SetGridFixedX          (GRAPH_Handle hObj, unsigned OnOff);
unsigned  GRAPH_SetGridOffY            (GRAPH_Handle hObj, unsigned Value);
unsigned  GRAPH_SetGridVis             (GRAPH_Handle hObj, unsigned OnOff);
unsigned  GRAPH_SetGridDistX           (GRAPH_Handle hObj, unsigned Value);
unsigned  GRAPH_SetGridDistY           (GRAPH_Handle hObj, unsigned Value);
U8        GRAPH_SetLineStyleH          (GRAPH_Handle hObj, U8 Value);
U8        GRAPH_SetLineStyleV          (GRAPH_Handle hObj, U8 Value);
void      GRAPH_SetLineStyle           (GRAPH_Handle hObj, U8 Value);
void      GRAPH_SetScrollValue         (GRAPH_Handle hObj, U8 Coord, U32 Value);
unsigned  GRAPH_SetVSizeX              (GRAPH_Handle hObj, unsigned Value);
unsigned  GRAPH_SetVSizeY              (GRAPH_Handle hObj, unsigned Value);
int       GRAPH_SetUserData            (GRAPH_Handle hObj, const void * pSrc, int NumBytes);
void      GRAPH_SetUserDraw            (GRAPH_Handle hObj, void (* pOwnerDraw)(WM_HWIN, int));

void      GRAPH_DATA_YT_AddValue       (GRAPH_DATA_Handle hDataObj, I16 Value);
void      GRAPH_DATA_YT_Clear          (GRAPH_DATA_Handle hDataObj);
void      GRAPH_DATA_YT_Delete         (GRAPH_DATA_Handle hDataObj);
void      GRAPH_DATA_YT_SetAlign       (GRAPH_DATA_Handle hDataObj, int Align);
void      GRAPH_DATA_YT_SetOffY        (GRAPH_DATA_Handle hDataObj, int Off);
void      GRAPH_DATA_YT_MirrorX        (GRAPH_DATA_Handle hDataObj, int OnOff);

void      GRAPH_DATA_XY_AddPoint       (GRAPH_DATA_Handle hDataObj, GUI_POINT * pPoint);
void      GRAPH_DATA_XY_Clear          (GRAPH_DATA_Handle hDataObj);
void      GRAPH_DATA_XY_Delete         (GRAPH_DATA_Handle hDataObj);
unsigned  GRAPH_DATA_XY_GetLineVis     (GRAPH_DATA_Handle hDataObj);
unsigned  GRAPH_DATA_XY_GetPointVis    (GRAPH_DATA_Handle hDataObj);
void      GRAPH_DATA_XY_SetLineStyle   (GRAPH_DATA_Handle hDataObj, U8 LineStyle);
unsigned  GRAPH_DATA_XY_SetLineVis     (GRAPH_DATA_Handle hDataObj, unsigned OnOff);
void      GRAPH_DATA_XY_SetOffX        (GRAPH_DATA_Handle hDataObj, int Off);
void      GRAPH_DATA_XY_SetOffY        (GRAPH_DATA_Handle hDataObj, int Off);
void      GRAPH_DATA_XY_SetPenSize     (GRAPH_DATA_Handle hDataObj, U8 PenSize);
void      GRAPH_DATA_XY_SetPointSize   (GRAPH_DATA_Handle hDataObj, unsigned PointSize);
unsigned  GRAPH_DATA_XY_SetPointVis    (GRAPH_DATA_Handle hDataObj, unsigned OnOff);
void      GRAPH_DATA_XY_SetOwnerDraw   (GRAPH_DATA_Handle hDataObj, WIDGET_DRAW_ITEM_FUNC * pOwnerDraw);

void             GRAPH_SCALE_Delete      (GRAPH_SCALE_Handle hScaleObj);
float            GRAPH_SCALE_SetFactor   (GRAPH_SCALE_Handle hScaleObj, float Factor);
const GUI_FONT * GRAPH_SCALE_SetFont     (GRAPH_SCALE_Handle hScaleObj, const GUI_FONT * pFont);
int              GRAPH_SCALE_SetNumDecs  (GRAPH_SCALE_Handle hScaleObj, int NumDecs);
int              GRAPH_SCALE_SetOff      (GRAPH_SCALE_Handle hScaleObj, int Off);
int              GRAPH_SCALE_SetPos      (GRAPH_SCALE_Handle hScaleObj, int Pos);
GUI_COLOR        GRAPH_SCALE_SetTextColor(GRAPH_SCALE_Handle hScaleObj, GUI_COLOR Color);
unsigned         GRAPH_SCALE_SetTickDist (GRAPH_SCALE_Handle hScaleObj, unsigned Value);

#if defined(__cplusplus)
  }
#endif

#endif  // GUI_WINSUPPORT
#endif  // GRAPH_H

/*************************** End of file ****************************/
