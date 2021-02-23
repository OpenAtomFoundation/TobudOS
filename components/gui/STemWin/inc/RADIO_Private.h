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
File        : RADIO_Private.h
Purpose     : RADIO private header file
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
  
#ifndef RADIO_PRIVATE_H
#define RADIO_PRIVATE_H

#include "WM.h"

#if GUI_WINSUPPORT

#include "RADIO.h"
#include "WIDGET.h"
#include "GUI_ARRAY.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
/* Define default image inactiv */
#ifndef   RADIO_IMAGE0_DEFAULT
  #define RADIO_IMAGE0_DEFAULT        &RADIO__abmRadio[0]
#endif

/* Define default image activ */
#ifndef   RADIO_IMAGE1_DEFAULT
  #define RADIO_IMAGE1_DEFAULT        &RADIO__abmRadio[1]
#endif

/* Define default image check */
#ifndef   RADIO_IMAGE_CHECK_DEFAULT
  #define RADIO_IMAGE_CHECK_DEFAULT   &RADIO__bmCheck
#endif

/* Define default font */
#ifndef   RADIO_FONT_DEFAULT
  #if   WIDGET_USE_SCHEME_SMALL
    #define RADIO_SPACING_DEFAULT 20
    #define RADIO_FONT_DEFAULT &GUI_Font13_1
  #elif WIDGET_USE_SCHEME_MEDIUM
    #define RADIO_SPACING_DEFAULT 24
    #define RADIO_FONT_DEFAULT &GUI_Font16_1
  #elif WIDGET_USE_SCHEME_LARGE
    #define RADIO_SPACING_DEFAULT 30
    #define RADIO_FONT_DEFAULT &GUI_Font24_1
  #endif
#endif

/* Define vertical default spacing */
#ifndef   RADIO_SPACING_DEFAULT
  #define RADIO_SPACING_DEFAULT 20
#endif

/* Define default text color */
#ifndef   RADIO_DEFAULT_TEXT_COLOR
  #define RADIO_DEFAULT_TEXT_COLOR    GUI_BLACK
#endif

/* Define default background color */
#ifndef   RADIO_DEFAULT_BKCOLOR
  #define RADIO_DEFAULT_BKCOLOR       GUI_GRAY_C0
#endif

#ifndef   RADIO_FOCUSCOLOR_DEFAULT
  #define RADIO_FOCUSCOLOR_DEFAULT    GUI_BLACK
#endif

#define RADIO_BORDER                  2

/*********************************************************************
*
*       Object definition
*
**********************************************************************
*/
typedef struct {
  unsigned (* pfGetButtonSize)(RADIO_Handle hObj);
  WIDGET_DRAW_ITEM_FUNC * pfDrawSkin;
} RADIO_SKIN_PRIVATE;

typedef struct {
  GUI_COLOR BkColor;
  GUI_COLOR TextColor;
  GUI_COLOR FocusColor;
  const GUI_FONT * pFont;
  const GUI_BITMAP * apBmRadio[2];
  const GUI_BITMAP * pBmCheck;
  RADIO_SKIN_PRIVATE SkinPrivate;
} RADIO_PROPS;

typedef struct {
  WIDGET Widget;
  RADIO_PROPS Props;
  WIDGET_SKIN const * pWidgetSkin;
  GUI_ARRAY TextArray;
  I16 Sel;                   /* current selection */
  U16 Spacing;
  U16 NumItems;
  U8  GroupId;
} RADIO_Obj;

/*********************************************************************
*
*       Macros for internal use
*
**********************************************************************
*/
#if GUI_DEBUG_LEVEL >= GUI_DEBUG_LEVEL_CHECK_ALL
  #define RADIO_INIT_ID(p) p->Widget.DebugId = RADIO_ID
#else
  #define RADIO_INIT_ID(p)
#endif

#if GUI_DEBUG_LEVEL >= GUI_DEBUG_LEVEL_CHECK_ALL
  RADIO_Obj * RADIO_LockH(RADIO_Handle h);
  #define RADIO_LOCK_H(h)   RADIO_LockH(h)
#else
  #define RADIO_LOCK_H(h)   (RADIO_Obj *)GUI_LOCK_H(h)
#endif

/*********************************************************************
*
*       Types
*
**********************************************************************
*/

typedef void tRADIO_SetValue(RADIO_Handle hObj, int v);

/*********************************************************************
*
*       Extern data
*
**********************************************************************
*/

extern RADIO_PROPS RADIO__DefaultProps;

extern const WIDGET_SKIN RADIO__SkinClassic;
extern       WIDGET_SKIN RADIO__Skin;

extern WIDGET_SKIN const * RADIO__pSkinDefault;

extern const GUI_BITMAP             RADIO__abmRadio[2];
extern const GUI_BITMAP             RADIO__bmCheck;
extern       tRADIO_SetValue*       RADIO__pfHandleSetValue;

/*********************************************************************
*
*       public functions (internal)
*
**********************************************************************
*/
void     RADIO__SetValue     (RADIO_Handle hObj, int v);
unsigned RADIO__GetButtonSize(RADIO_Handle hObj);

#endif   /* GUI_WINSUPPORT */
#endif   /* RADIO_PRIVATE_H */

/************************* end of file ******************************/
