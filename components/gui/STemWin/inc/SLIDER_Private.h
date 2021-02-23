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
File        : SLIDER_Private.h
Purpose     : SLIDER private header file
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
  
#ifndef SLIDER_PRIVATE_H
#define SLIDER_PRIVATE_H

#include "WM.h"
#include "WIDGET.h"
#include "SLIDER.h"

#if GUI_WINSUPPORT

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
//
// Flags
//
#define SLIDER_FLAG_DRAW_FOCUS_RECT (1 << 0)

/*********************************************************************
*
*       Object definition
*
**********************************************************************
*/
typedef struct {
  WIDGET_DRAW_ITEM_FUNC * pfDrawSkin;
} SLIDER_SKIN_PRIVATE;

typedef struct {
  U8                  Flags;
  GUI_COLOR           BkColor;
  GUI_COLOR           BarColor;
  GUI_COLOR           FocusColor;
  GUI_COLOR           TickColor;
  SLIDER_SKIN_PRIVATE SkinPrivate;
} SLIDER_PROPS;

typedef struct {
  WIDGET              Widget;
  SLIDER_PROPS        Props;
  WIDGET_SKIN const * pWidgetSkin;
  int                 NumTicks;
  int                 Max;
  int                 Min;
  int                 v;
  I16                 Width;
} SLIDER_Obj;

/*********************************************************************
*
*       Macros for internal use
*
**********************************************************************
*/
#if GUI_DEBUG_LEVEL >= GUI_DEBUG_LEVEL_CHECK_ALL
  #define SLIDER_INIT_ID(p) (p->Widget.DebugId = SLIDER_ID)
#else
  #define SLIDER_INIT_ID(p)
#endif

#if GUI_DEBUG_LEVEL >= GUI_DEBUG_LEVEL_CHECK_ALL
  SLIDER_Obj * SLIDER_LockH(SLIDER_Handle h);
  #define SLIDER_LOCK_H(h)  SLIDER_LockH(h)
#else
  #define SLIDER_LOCK_H(h)  (SLIDER_Obj *)GUI_LOCK_H(h)
#endif

#ifndef   SLIDER_SUPPORT_TRANSPARENCY
  #define SLIDER_SUPPORT_TRANSPARENCY WM_SUPPORT_TRANSPARENCY
#endif

/*********************************************************************
*
*       Public data (internal defaults)
*
**********************************************************************
*/
extern       SLIDER_PROPS   SLIDER__DefaultProps;
extern const WIDGET_SKIN    SLIDER__SkinClassic;
extern       WIDGET_SKIN    SLIDER__Skin;
extern const WIDGET_SKIN  * SLIDER__pSkinDefault;

#endif  // GUI_WINSUPPORT
#endif  // SLIDER_PRIVATE_H

/*************************** End of file ****************************/
