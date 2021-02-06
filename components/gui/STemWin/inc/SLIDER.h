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
File        : SLIDER.h
Purpose     : SLIDER include
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
  
#ifndef SLIDER_H
#define SLIDER_H

#include "WM.h"
#include "DIALOG_Intern.h"      /* Req. for Create indirect data structure */
#include "WIDGET.h"

#if GUI_WINSUPPORT

#if defined(__cplusplus)
  extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

/************************************************************
*
*       Defines
*
*************************************************************
*/
/************************************************************
*
*       States
*/
#define SLIDER_STATE_PRESSED    WIDGET_STATE_USER0

/************************************************************
*
*       Create / Status flags
*/
#define SLIDER_CF_HORIZONTAL 0
#define SLIDER_CF_VERTICAL   WIDGET_CF_VERTICAL

/************************************************************
*
*       Skinning property indices
*/
#define SLIDER_SKINFLEX_PI_PRESSED   0
#define SLIDER_SKINFLEX_PI_UNPRESSED 1

/*********************************************************************
*
*       Public Types
*
**********************************************************************
*/
typedef WM_HMEM SLIDER_Handle;

typedef struct {
  GUI_COLOR aColorFrame[2];
  GUI_COLOR aColorInner[2];
  GUI_COLOR aColorShaft[3];
  GUI_COLOR ColorTick;
  GUI_COLOR ColorFocus;
  int TickSize;
  int ShaftSize;
} SLIDER_SKINFLEX_PROPS;

typedef struct {
  int Width;
  int NumTicks;
  int Size;
  int IsPressed;
  int IsVertical;
} SLIDER_SKINFLEX_INFO;

/*********************************************************************
*
*       Create functions
*
**********************************************************************
*/
SLIDER_Handle SLIDER_Create        (int x0, int y0, int xSize, int ySize, WM_HWIN hParent, int Id, int WinFlags, int SpecialFlags);
SLIDER_Handle SLIDER_CreateEx      (int x0, int y0, int xSize, int ySize, WM_HWIN hParent, int WinFlags, int ExFlags, int Id);
SLIDER_Handle SLIDER_CreateUser    (int x0, int y0, int xSize, int ySize, WM_HWIN hParent, int WinFlags, int ExFlags, int Id, int NumExtraBytes);
SLIDER_Handle SLIDER_CreateIndirect(const GUI_WIDGET_CREATE_INFO * pCreateInfo, WM_HWIN hWinParent, int x0, int y0, WM_CALLBACK * cb);

/*********************************************************************
*
*       The callback ...
*
* Do not call it directly ! It is only to be used from within an
* overwritten callback.
*/
void SLIDER_Callback(WM_MESSAGE * pMsg);

/*********************************************************************
*
*       Member functions
*
**********************************************************************
*/
void      SLIDER_Dec            (SLIDER_Handle hObj);
void      SLIDER_EnableFocusRect(SLIDER_Handle hObj, int OnOff);
GUI_COLOR SLIDER_GetBarColor    (SLIDER_Handle hObj);
GUI_COLOR SLIDER_GetBkColor     (SLIDER_Handle hObj);
U8        SLIDER_GetFlag        (SLIDER_Handle hObj, U8 Flag);
GUI_COLOR SLIDER_GetFocusColor  (SLIDER_Handle hObj);
void      SLIDER_GetRange       (SLIDER_Handle hObj, int * pMin, int * pMax);
GUI_COLOR SLIDER_GetTickColor   (SLIDER_Handle hObj);
int       SLIDER_GetUserData    (SLIDER_Handle hObj, void * pDest, int NumBytes);
int       SLIDER_GetValue       (SLIDER_Handle hObj);
void      SLIDER_Inc            (SLIDER_Handle hObj);
void      SLIDER_SetBarColor    (SLIDER_Handle hObj, GUI_COLOR Color);
void      SLIDER_SetBkColor     (SLIDER_Handle hObj, GUI_COLOR Color);
GUI_COLOR SLIDER_SetFocusColor  (SLIDER_Handle hObj, GUI_COLOR Color);
void      SLIDER_SetNumTicks    (SLIDER_Handle hObj, int NumTicks);
void      SLIDER_SetRange       (SLIDER_Handle hObj, int Min, int Max);
void      SLIDER_SetTickColor   (SLIDER_Handle hObj, GUI_COLOR Color);
int       SLIDER_SetUserData    (SLIDER_Handle hObj, const void * pSrc, int NumBytes);
void      SLIDER_SetValue       (SLIDER_Handle hObj, int v);
void      SLIDER_SetWidth       (SLIDER_Handle hObj, int Width);

/*********************************************************************
*
*       Member functions: Skinning
*
**********************************************************************
*/
void SLIDER_GetSkinFlexProps     (SLIDER_SKINFLEX_PROPS * pProps, int Index);
void SLIDER_SetSkinClassic       (SLIDER_Handle hObj);
void SLIDER_SetSkin              (SLIDER_Handle hObj, WIDGET_DRAW_ITEM_FUNC * pfDrawSkin);
int  SLIDER_DrawSkinFlex         (const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo);
void SLIDER_SetSkinFlexProps     (const SLIDER_SKINFLEX_PROPS * pProps, int Index);
void SLIDER_SetDefaultSkinClassic(void);
WIDGET_DRAW_ITEM_FUNC * SLIDER_SetDefaultSkin(WIDGET_DRAW_ITEM_FUNC * pfDrawSkin);

#define SLIDER_SKIN_FLEX    SLIDER_DrawSkinFlex

/*********************************************************************
*
*       Managing default values
*
**********************************************************************
*/
GUI_COLOR SLIDER_GetDefaultBkColor   (void);
GUI_COLOR SLIDER_GetDefaultBarColor  (void);
GUI_COLOR SLIDER_GetDefaultFocusColor(void);
GUI_COLOR SLIDER_GetDefaultTickColor (void);
void      SLIDER_SetDefaultBkColor   (GUI_COLOR Color);
void      SLIDER_SetDefaultBarColor  (GUI_COLOR Color);
GUI_COLOR SLIDER_SetDefaultFocusColor(GUI_COLOR Color);
void      SLIDER_SetDefaultTickColor (GUI_COLOR Color);

#if defined(__cplusplus)
  }
#endif

#endif  // GUI_WINSUPPORT
#endif  // SLIDER_H

/*************************** End of file ****************************/
