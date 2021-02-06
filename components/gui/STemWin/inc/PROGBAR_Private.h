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
File        : PROGBAR_Private.h
Purpose     : Internal header file
---------------------------END-OF-HEADER------------------------------
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
  
#ifndef PROGBAR_PRIVATE_H
#define PROGBAR_PRIVATE_H

#include "PROGBAR.h"
#include "WIDGET.h"

#if GUI_WINSUPPORT

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define PROGBAR_SF_HORIZONTAL PROGBAR_CF_HORIZONTAL
#define PROGBAR_SF_VERTICAL   PROGBAR_CF_VERTICAL
#define PROGBAR_SF_USER       PROGBAR_CF_USER

/*********************************************************************
*
*       Types
*
**********************************************************************
*/
typedef struct {
  WIDGET_DRAW_ITEM_FUNC * pfDrawSkin;
} PROGBAR_SKIN_PRIVATE;

typedef struct {
  const GUI_FONT * pFont;
  GUI_COLOR aBarColor[2];
  GUI_COLOR aTextColor[2];
  PROGBAR_SKIN_PRIVATE SkinPrivate;
} PROGBAR_PROPS;

typedef struct {
  WIDGET Widget;
  int v;
  WM_HMEM hpText;
  I16 XOff, YOff;
  I16 TextAlign;
  int Min, Max;
  PROGBAR_PROPS Props;
  WIDGET_SKIN const * pWidgetSkin;
  U8 Flags;
} PROGBAR_Obj;

/*********************************************************************
*
*       Macros for internal use
*
**********************************************************************
*/
#if GUI_DEBUG_LEVEL >= GUI_DEBUG_LEVEL_CHECK_ALL
  #define PROGBAR_INIT_ID(p) p->Widget.DebugId = PROGBAR_ID
#else
  #define PROGBAR_INIT_ID(p)
#endif

#if GUI_DEBUG_LEVEL >= GUI_DEBUG_LEVEL_CHECK_ALL
  PROGBAR_Obj * PROGBAR_LockH(PROGBAR_Handle h);
  #define PROGBAR_LOCK_H(h)   PROGBAR_LockH(h)
#else
  #define PROGBAR_LOCK_H(h)   (PROGBAR_Obj *)GUI_LOCK_H(h)
#endif

/*********************************************************************
*
*       Public data (internal defaults)
*
**********************************************************************
*/
extern PROGBAR_PROPS PROGBAR__DefaultProps;

extern const WIDGET_SKIN PROGBAR__SkinClassic;
extern       WIDGET_SKIN PROGBAR__Skin;

extern WIDGET_SKIN const * PROGBAR__pSkinDefault;

/*********************************************************************
*
*       Public functions (internal)
*
**********************************************************************
*/
char * PROGBAR__GetTextLocked(const PROGBAR_Obj * pObj);
void   PROGBAR__GetTextRect  (const PROGBAR_Obj * pObj, GUI_RECT * pRect, const char * pText);
int    PROGBAR__Value2Pos    (const PROGBAR_Obj * pObj, int v);

#endif /* GUI_WINSUPPORT */
#endif /* PROGBAR_PRIVATE_H */

/*************************** End of file ****************************/
