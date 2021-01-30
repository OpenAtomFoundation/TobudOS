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
File        : MULTIPAGE_Private.h
Purpose     : Private MULTIPAGE include
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
  
#ifndef MULTIPAGE_PRIVATE_H
#define MULTIPAGE_PRIVATE_H

#include "GUI_Debug.h"
#include "GUI_ARRAY.h"
#include "MULTIPAGE.h"

#if GUI_WINSUPPORT

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define MULTIPAGE_STATE_ENABLED     (1 << 0)
#define MULTIPAGE_STATE_SCROLLMODE  WIDGET_STATE_USER0

#define MULTIPAGE_NUMCOLORS         2

/*********************************************************************
*
*       Object definition
*
**********************************************************************
*/
//
// MULTIPAGE_PAGE
//
typedef struct {
  WM_HWIN hWin;
  U8      Status;
  int     ItemWidth;
  WM_HMEM hDrawObj[3];
  char    acText;
} MULTIPAGE_PAGE;

//
// MULTIPAGE_SKIN_PRIVATE
//
typedef struct {
  WIDGET_DRAW_ITEM_FUNC * pfDrawSkin;
} MULTIPAGE_SKIN_PRIVATE;

//
// MULTIPAGE_PROPS
//
typedef struct {
  const GUI_FONT          * pFont;
  unsigned                  Align;
  GUI_COLOR                 aBkColor[MULTIPAGE_NUMCOLORS];
  GUI_COLOR                 aTextColor[MULTIPAGE_NUMCOLORS];
  MULTIPAGE_SKIN_PRIVATE    SkinPrivate;
  int                       BorderSize0;
  int                       BorderSize1;
  unsigned                  TextAlign;
  unsigned                  Scrollbar;
  int                    (* pfGetTouchedPage)(MULTIPAGE_Handle hObj, int x, int y);
  int                    (* pfGetTabBarWidth)(MULTIPAGE_Handle hObj);
} MULTIPAGE_PROPS;

//
// MULTIPAGE_Obj
//
typedef struct MULTIPAGE_Obj MULTIPAGE_Obj;

struct MULTIPAGE_Obj {
  WIDGET                 Widget;
  void                (* pfDrawTextItem)(MULTIPAGE_Obj * pObj, const char * pText, unsigned Index, const GUI_RECT * pRect, int x0, int xSize, int ColorIndex);
  WM_HWIN                hClient;
  GUI_ARRAY              hPageArray;
  unsigned               Selection;
  int                    ScrollState;
  MULTIPAGE_PROPS        Props;
  WIDGET_SKIN const    * pWidgetSkin;
  MULTIPAGE_SKIN_PROPS   SkinProps;
  int                    ItemHeight;
  int                    MaxHeight;
};

/*********************************************************************
*
*       Macros for internal use
*
**********************************************************************
*/
#if GUI_DEBUG_LEVEL >= GUI_DEBUG_LEVEL_CHECK_ALL
  #define MULTIPAGE_INIT_ID(p) (p->Widget.DebugId = MULTIPAGE_ID)
#else
  #define MULTIPAGE_INIT_ID(p)
#endif

#if GUI_DEBUG_LEVEL >= GUI_DEBUG_LEVEL_CHECK_ALL
  MULTIPAGE_Obj * MULTIPAGE_LockH(MULTIPAGE_Handle h);
  #define MULTIPAGE_LOCK_H(h)   MULTIPAGE_LockH(h)
#else
  #define MULTIPAGE_LOCK_H(h)   (MULTIPAGE_Obj *)GUI_LOCK_H(h)
#endif

/*********************************************************************
*
*       Externals
*
**********************************************************************
*/
extern GUI_COLOR           MULTIPAGE__aEffectColor[2];
extern MULTIPAGE_PROPS     MULTIPAGE__DefaultProps;

extern const WIDGET_SKIN   MULTIPAGE__SkinClassic;
extern       WIDGET_SKIN   MULTIPAGE__Skin;

extern WIDGET_SKIN const * MULTIPAGE__pSkinDefault;

/*********************************************************************
*
*       Private functions
*
**********************************************************************
*/
void MULTIPAGE__CalcBorderRect (MULTIPAGE_Obj * pObj, GUI_RECT * pRect);
void MULTIPAGE__CalcClientRect (MULTIPAGE_Handle hObj, GUI_RECT * pRect);
void MULTIPAGE__DeleteScrollbar(MULTIPAGE_Handle hObj);
void MULTIPAGE__DrawTextItemH  (MULTIPAGE_Obj * pObj, const char * pText, unsigned Index, const GUI_RECT * pRect, int x0, int w, int ColorIndex);
int  MULTIPAGE__GetPagePos     (MULTIPAGE_Handle hObj, unsigned Index);
int  MULTIPAGE__GetPageWidth   (MULTIPAGE_Handle hObj, unsigned Index);
void MULTIPAGE__GetTabBarRect  (MULTIPAGE_Handle hObj, GUI_RECT * pRect);
int  MULTIPAGE__SetDrawObj     (MULTIPAGE_Handle hObj, GUI_DRAW_HANDLE hDrawObj, int Index, int State);
void MULTIPAGE__UpdatePositions(MULTIPAGE_Handle hObj);

/*********************************************************************
*
*       Private Skinning functions
*
**********************************************************************
*/
int  MULTIPAGE_SKIN__GetPagePos    (MULTIPAGE_Handle hObj, unsigned Index);
int  MULTIPAGE_SKIN__GetTabBarWidth(MULTIPAGE_Handle hObj);
int  MULTIPAGE_SKIN__GetTouchedPage(MULTIPAGE_Handle hObj, int TouchX, int TouchY);

#endif  // GUI_WINSUPPORT
#endif  // MULTIPAGE_PRIVATE_H

/*************************** End of file ****************************/
