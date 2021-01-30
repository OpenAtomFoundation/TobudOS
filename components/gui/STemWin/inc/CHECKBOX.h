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
File        : CHECKBOX.h
Purpose     : CHECKBOX include
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
  
#ifndef CHECKBOX_H
#define CHECKBOX_H

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

/*********************************************************************
*
*       Color indices
*/
#define CHECKBOX_CI_DISABLED 0
#define CHECKBOX_CI_ENABLED  1

/*********************************************************************
*
*       Bitmap indices
*/
#define CHECKBOX_BI_INACTIV_UNCHECKED 0
#define CHECKBOX_BI_ACTIV_UNCHECKED   1
#define CHECKBOX_BI_INACTIV_CHECKED   2
#define CHECKBOX_BI_ACTIV_CHECKED     3
#define CHECKBOX_BI_INACTIV_3STATE    4
#define CHECKBOX_BI_ACTIV_3STATE      5

/*********************************************************************
*
*       Skinning property indices
*/
#define CHECKBOX_SKINFLEX_PI_ENABLED     0
#define CHECKBOX_SKINFLEX_PI_DISABLED    1

/*********************************************************************
*
*       Public Types
*
**********************************************************************
*/
typedef WM_HMEM CHECKBOX_Handle;

typedef struct {
  GUI_COLOR aColorFrame[3];
  GUI_COLOR aColorInner[2];
  GUI_COLOR ColorCheck;
  int       ButtonSize;
} CHECKBOX_SKINFLEX_PROPS;

/*********************************************************************
*
*       Create functions
*
**********************************************************************
*/
CHECKBOX_Handle CHECKBOX_Create        (int x0, int y0, int xSize, int ySize, WM_HWIN hParent, int Id, int Flags);
CHECKBOX_Handle CHECKBOX_CreateEx      (int x0, int y0, int xSize, int ySize, WM_HWIN hParent, int WinFlags, int ExFlags, int Id);
CHECKBOX_Handle CHECKBOX_CreateUser    (int x0, int y0, int xSize, int ySize, WM_HWIN hParent, int WinFlags, int ExFlags, int Id, int NumExtraBytes);
CHECKBOX_Handle CHECKBOX_CreateIndirect(const GUI_WIDGET_CREATE_INFO * pCreateInfo, WM_HWIN hWinParent, int x0, int y0, WM_CALLBACK * cb);

/*********************************************************************
*
*       The callback ...
*
* Do not call it directly ! It is only to be used from within an
* overwritten callback.
*/
void CHECKBOX_Callback(WM_MESSAGE * pMsg);

/*********************************************************************
*
*       Managing default values
*
**********************************************************************
*/

int              CHECKBOX_GetDefaultAlign     (void);
GUI_COLOR        CHECKBOX_GetDefaultBkColor   (void);
const GUI_FONT * CHECKBOX_GetDefaultFont      (void);
int              CHECKBOX_GetDefaultSpacing   (void);
int              CHECKBOX_GetDefaultTextAlign (void);
GUI_COLOR        CHECKBOX_GetDefaultTextColor (void);
int              CHECKBOX_GetUserData         (CHECKBOX_Handle hObj, void * pDest, int NumBytes);
void             CHECKBOX_SetDefaultAlign     (int Align);
void             CHECKBOX_SetDefaultBkColor   (GUI_COLOR Color);
GUI_COLOR        CHECKBOX_SetDefaultFocusColor(GUI_COLOR Color);
void             CHECKBOX_SetDefaultFont      (const GUI_FONT * pFont);
void             CHECKBOX_SetDefaultImage     (const GUI_BITMAP * pBitmap, unsigned int Index);
void             CHECKBOX_SetDefaultSpacing   (int Spacing);
void             CHECKBOX_SetDefaultTextAlign (int Align);
void             CHECKBOX_SetDefaultTextColor (GUI_COLOR Color);

/*********************************************************************
*
*       Member functions
*
**********************************************************************
*/

int       CHECKBOX_GetState     (CHECKBOX_Handle hObj);
int       CHECKBOX_GetText      (CHECKBOX_Handle hObj, char * pBuffer, int MaxLen);
int       CHECKBOX_IsChecked    (CHECKBOX_Handle hObj);
void      CHECKBOX_SetBkColor   (CHECKBOX_Handle hObj, GUI_COLOR Color);
GUI_COLOR CHECKBOX_SetBoxBkColor(CHECKBOX_Handle hObj, GUI_COLOR Color, int Index);
GUI_COLOR CHECKBOX_SetFocusColor(CHECKBOX_Handle hObj, GUI_COLOR Color);
void      CHECKBOX_SetFont      (CHECKBOX_Handle hObj, const GUI_FONT * pFont);
void      CHECKBOX_SetImage     (CHECKBOX_Handle hObj, const GUI_BITMAP * pBitmap, unsigned int Index);
void      CHECKBOX_SetNumStates (CHECKBOX_Handle hObj, unsigned NumStates);
void      CHECKBOX_SetSpacing   (CHECKBOX_Handle hObj, unsigned Spacing);
void      CHECKBOX_SetState     (CHECKBOX_Handle hObj, unsigned State);
void      CHECKBOX_SetText      (CHECKBOX_Handle hObj, const char * pText);
void      CHECKBOX_SetTextAlign (CHECKBOX_Handle hObj, int Align);
void      CHECKBOX_SetTextColor (CHECKBOX_Handle hObj, GUI_COLOR Color);
int       CHECKBOX_SetUserData  (CHECKBOX_Handle hObj, const void * pSrc, int NumBytes);

/*********************************************************************
*
*       Member functions: Skinning
*
**********************************************************************
*/
void CHECKBOX_GetSkinFlexProps      (CHECKBOX_SKINFLEX_PROPS * pProps, int Index);
void CHECKBOX_SetSkinClassic        (CHECKBOX_Handle hObj);
void CHECKBOX_SetSkin               (CHECKBOX_Handle hObj, WIDGET_DRAW_ITEM_FUNC * pfDrawSkin);
int  CHECKBOX_DrawSkinFlex          (const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo);
void CHECKBOX_SetSkinFlexProps      (const CHECKBOX_SKINFLEX_PROPS * pProps, int Index);
void CHECKBOX_SetDefaultSkinClassic (void);
int  CHECKBOX_GetSkinFlexButtonSize (CHECKBOX_Handle hObj);
void CHECKBOX_SetSkinFlexButtonSize (CHECKBOX_Handle hObj, int ButtonSize);
WIDGET_DRAW_ITEM_FUNC * CHECKBOX_SetDefaultSkin(WIDGET_DRAW_ITEM_FUNC * pfDrawSkin);

#define CHECKBOX_SKIN_FLEX    CHECKBOX_DrawSkinFlex

/*********************************************************************
*
*       Macros for compatibility
*
**********************************************************************
*/

#define CHECKBOX_Check(hObj)   CHECKBOX_SetState(hObj, 1)
#define CHECKBOX_Uncheck(hObj) CHECKBOX_SetState(hObj, 0)

#define CHECKBOX_BI_INACTIV    CHECKBOX_BI_INACTIV_CHECKED
#define CHECKBOX_BI_ACTIV      CHECKBOX_BI_ACTIV_CHECKED

#if defined(__cplusplus)
  }
#endif

#endif  // GUI_WINSUPPORT
#endif  // CHECKBOX_H

/*************************** End of file ****************************/
