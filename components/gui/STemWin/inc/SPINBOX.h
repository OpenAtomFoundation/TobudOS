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
File        : SPINBOX.h
Purpose     : SPINBOX header file
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
  
#ifndef SPINBOX_H
#define SPINBOX_H

#include "WM.h"
#include "DIALOG_Intern.h" // Required for Create indirect data structure
#include "WIDGET.h"
#include "GUI_Debug.h"
#include "EDIT.h"

#if GUI_WINSUPPORT

#if defined(__cplusplus)
  extern "C" {             // Make sure we have C-declarations in C++ programs
#endif

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
/*********************************************************************
*
*       States
*/
#define SPINBOX_STATE_PRESSED(x)     (U8)(1 << (U8)x)    // These flags are stored in (SPINBOX_OBJ->State)        | x must be 0 or 1
#define SPINBOX_STATE_FOCUS          WIDGET_STATE_FOCUS  // This is read from         (SPINBOX_OBJ->Widget.State)

#define SPINBOX_EDGE_RIGHT           0
#define SPINBOX_EDGE_LEFT            1
#define SPINBOX_EDGE_CENTER          2

#define SPINBOX_EM_STEP              0
#define SPINBOX_EM_EDIT              1

#ifndef SPINBOX_EM_DEFAULT
  #define SPINBOX_EM_DEFAULT         SPINBOX_EM_STEP
#endif

/*********************************************************************
*
*       Color indices
*/
#define SPINBOX_CI_DISABLED          EDIT_CI_DISABLED
#define SPINBOX_CI_ENABLED           EDIT_CI_ENABLED
#define SPINBOX_CI_PRESSED           2

/*********************************************************************
*
*       Skinning property indices
*/
#define SPINBOX_SKIN_FLEX            SPINBOX_DrawSkinFlex

#define SPINBOX_SKINFLEX_PI_PRESSED  0
#define SPINBOX_SKINFLEX_PI_FOCUSSED 1
#define SPINBOX_SKINFLEX_PI_ENABLED  2
#define SPINBOX_SKINFLEX_PI_DISABLED 3
#define SPINBOX_SKIN_FLEX_RADIUS     2

/*********************************************************************
*
*       Public Types
*
**********************************************************************
*/
typedef WM_HMEM SPINBOX_Handle;

typedef struct {
  GUI_COLOR aColorFrame[2];   // [0] Outer color of surrounding frame.         [1] Inner color of surrounding frame.
  GUI_COLOR aColorUpper[2];   // [0] Upper color of gradient for upper button. [1] Lower color of gradient for upper button.
  GUI_COLOR aColorLower[2];   // [0] Upper color of gradient for lower button. [1] Lower color of gradient for lower button.
  GUI_COLOR ColorArrow;       // Color of the button arrow.
  GUI_COLOR ColorBk;          // Color of the background.                      // See WIDGET_ITEM_CREATE in SPINBOX_DrawSkinFlex()
  GUI_COLOR ColorText;        // Color of the text.                            // See WIDGET_ITEM_CREATE in SPINBOX_DrawSkinFlex()
  GUI_COLOR ColorButtonFrame; // Color of the button frame.
} SPINBOX_SKINFLEX_PROPS;

/*********************************************************************
*
*       Prototypes
*
**********************************************************************
*/
/*********************************************************************
*
*       Creation
*/
SPINBOX_Handle SPINBOX_CreateEx      (int x0, int y0, int xSize, int ySize, WM_HWIN hParent, int WinFlags, int Id, int Min, int Max);
SPINBOX_Handle SPINBOX_CreateUser    (int x0, int y0, int xSize, int ySize, WM_HWIN hParent, int WinFlags, int Id, int Min, int Max, int NumExtraBytes);
SPINBOX_Handle SPINBOX_CreateIndirect(const GUI_WIDGET_CREATE_INFO * pCreateInfo, WM_HWIN hWinParent, int x0, int y0, WM_CALLBACK * cb);

/*********************************************************************
*
*       Callback, should be called only from within a custom callback.
*/
void           SPINBOX_Callback(WM_MESSAGE * pMsg);

/*********************************************************************
*
*       Get / Set properties
*/
void        SPINBOX_EnableBlink     (SPINBOX_Handle hObj, int Period, int OnOff);
GUI_COLOR   SPINBOX_GetBkColor      (SPINBOX_Handle hObj, unsigned int Index);
GUI_COLOR   SPINBOX_GetButtonBkColor(SPINBOX_Handle hObj, unsigned int Index);
EDIT_Handle SPINBOX_GetEditHandle   (SPINBOX_Handle hObj);
int         SPINBOX_GetUserData     (SPINBOX_Handle hObj, void * pDest, int NumBytes);
I32         SPINBOX_GetValue        (SPINBOX_Handle hObj);
void        SPINBOX_SetBkColor      (SPINBOX_Handle hObj, unsigned int Index, GUI_COLOR Color);
void        SPINBOX_SetButtonBkColor(SPINBOX_Handle hObj, unsigned int Index, GUI_COLOR Color);
void        SPINBOX_SetButtonSize   (SPINBOX_Handle hObj, unsigned ButtonSize);
void        SPINBOX_SetEdge         (SPINBOX_Handle hObj, U8 Edge);
void        SPINBOX_SetEditMode     (SPINBOX_Handle hObj, U8 EditMode);
void        SPINBOX_SetFont         (SPINBOX_Handle hObj, const GUI_FONT * pFont);
void        SPINBOX_SetRange        (SPINBOX_Handle hObj, I32 Min, I32 Max);
U16         SPINBOX_SetStep         (SPINBOX_Handle hObj, U16 Step);
void        SPINBOX_SetTextColor    (SPINBOX_Handle hObj, unsigned int Index, GUI_COLOR Color);
int         SPINBOX_SetUserData     (SPINBOX_Handle hObj, const void * pSrc, int NumBytes);
void        SPINBOX_SetValue        (SPINBOX_Handle hObj, I32 Value);

/*********************************************************************
*
*       Managing default values
*/
U16  SPINBOX_GetDefaultButtonSize(void);
void SPINBOX_SetDefaultButtonSize(U16 ButtonSize);

/*********************************************************************
*
*       Skinning
*/
void                    SPINBOX_GetSkinFlexProps     (SPINBOX_SKINFLEX_PROPS * pProps, int Index);
void                    SPINBOX_SetSkinClassic       (SPINBOX_Handle hObj);
void                    SPINBOX_SetSkin              (SPINBOX_Handle hObj, WIDGET_DRAW_ITEM_FUNC * pfDrawSkin);
int                     SPINBOX_DrawSkinFlex         (const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo);
void                    SPINBOX_SetSkinFlexProps     (const SPINBOX_SKINFLEX_PROPS * pProps, int Index);
void                    SPINBOX_SetDefaultSkinClassic(void);
WIDGET_DRAW_ITEM_FUNC * SPINBOX_SetDefaultSkin(WIDGET_DRAW_ITEM_FUNC * pfDrawSkin);

#if defined(__cplusplus)
  }
#endif

#endif  // GUI_WINSUPPORT
#endif  // SPINBOX_H

/*************************** End of file ****************************/
