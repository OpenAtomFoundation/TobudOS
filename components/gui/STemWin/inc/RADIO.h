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
File        : RADIO.h
Purpose     : RADIO include
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
  
#ifndef RADIO_H
#define RADIO_H

#include "WM.h"
#include "DIALOG_Intern.h"      /* Req. for Create indirect data structure */
#include "WIDGET.h"

#if GUI_WINSUPPORT

#if defined(__cplusplus)
  extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

/*********************************************************************
*
*       Bitmap indices
*/
#define RADIO_BI_INACTIV 0
#define RADIO_BI_ACTIV   1
#define RADIO_BI_CHECK   2

/*********************************************************************
*
*       Skinning property indices
*/
#define RADIO_SKINFLEX_PI_PRESSED     0
#define RADIO_SKINFLEX_PI_UNPRESSED   1

/*********************************************************************
*
*       Defaults for public configuration switches
*
**********************************************************************

The following are defaults for config switches which affect the
interface specified in this module
*/

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define RADIO_TEXTPOS_RIGHT       0
#define RADIO_TEXTPOS_LEFT        WIDGET_STATE_USER0  /* Not implemented, TBD */

/*********************************************************************
*
*       Public Types
*
**********************************************************************
*/
typedef WM_HMEM RADIO_Handle;

typedef struct {
  GUI_COLOR aColorButton[4];
  int       ButtonSize;
} RADIO_SKINFLEX_PROPS;

/*********************************************************************
*
*       Create functions
*
**********************************************************************
*/

RADIO_Handle RADIO_Create        (int x0, int y0, int xSize, int ySize, WM_HWIN hParent, int Id, int Flags, unsigned Para);
RADIO_Handle RADIO_CreateEx      (int x0, int y0, int xSize, int ySize, WM_HWIN hParent, int WinFlags, int ExFlags, int Id, int NumItems, int Spacing);
RADIO_Handle RADIO_CreateUser    (int x0, int y0, int xSize, int ySize, WM_HWIN hParent, int WinFlags, int ExFlags, int Id, int NumItems, int Spacing, int NumExtraBytes);
RADIO_Handle RADIO_CreateIndirect(const GUI_WIDGET_CREATE_INFO * pCreateInfo, WM_HWIN hWinParent, int x0, int y0, WM_CALLBACK * cb);

/*********************************************************************
*
*       The callback ...
*
* Do not call it directly ! It is only to be used from within an
* overwritten callback.
*/
void RADIO_Callback(WM_MESSAGE * pMsg);

/*********************************************************************
*
*       Managing default values
*
**********************************************************************
*/

void             RADIO_SetDefaultFont      (const GUI_FONT * pFont);
GUI_COLOR        RADIO_SetDefaultFocusColor(GUI_COLOR Color);
void             RADIO_SetDefaultImage     (const GUI_BITMAP * pBitmap, unsigned int Index);
void             RADIO_SetDefaultTextColor (GUI_COLOR TextColor);
const GUI_FONT * RADIO_GetDefaultFont      (void);
GUI_COLOR        RADIO_GetDefaultTextColor (void);

/*********************************************************************
*
*       Member functions
*
**********************************************************************
*/

void      RADIO_AddValue     (RADIO_Handle hObj, int Add);
void      RADIO_Dec          (RADIO_Handle hObj);
int       RADIO_GetText      (RADIO_Handle hObj, unsigned Index, char * pBuffer, int MaxLen);
int       RADIO_GetUserData  (RADIO_Handle hObj, void * pDest, int NumBytes);
void      RADIO_Inc          (RADIO_Handle hObj);
void      RADIO_SetBkColor   (RADIO_Handle hObj, GUI_COLOR Color);
GUI_COLOR RADIO_SetFocusColor(RADIO_Handle hObj, GUI_COLOR Color);
void      RADIO_SetFont      (RADIO_Handle hObj, const GUI_FONT * pFont);
void      RADIO_SetGroupId   (RADIO_Handle hObj, U8 GroupId);
void      RADIO_SetImage     (RADIO_Handle hObj, const GUI_BITMAP * pBitmap, unsigned int Index);
void      RADIO_SetText      (RADIO_Handle hObj, const char* pText, unsigned Index);
void      RADIO_SetTextColor (RADIO_Handle hObj, GUI_COLOR Color);
void      RADIO_SetValue     (RADIO_Handle hObj, int v);
int       RADIO_SetUserData  (RADIO_Handle hObj, const void * pSrc, int NumBytes);

const GUI_BITMAP * RADIO_GetImage(RADIO_Handle hObj, unsigned int Index);

/*********************************************************************
*
*       Member functions: Skinning
*
**********************************************************************
*/
void RADIO_GetSkinFlexProps     (RADIO_SKINFLEX_PROPS * pProps, int Index);
void RADIO_SetSkinClassic       (RADIO_Handle hObj);
void RADIO_SetSkin              (RADIO_Handle hObj, WIDGET_DRAW_ITEM_FUNC * pfDrawSkin);
int  RADIO_DrawSkinFlex         (const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo);
void RADIO_SetSkinFlexProps     (const RADIO_SKINFLEX_PROPS * pProps, int Index);
void RADIO_SetDefaultSkinClassic(void);
WIDGET_DRAW_ITEM_FUNC * RADIO_SetDefaultSkin(WIDGET_DRAW_ITEM_FUNC * pfDrawSkin);

#define RADIO_SKIN_FLEX    RADIO_DrawSkinFlex

/*********************************************************************
*
*       Query state
*
**********************************************************************
*/
int RADIO_GetValue(RADIO_Handle hObj);

#if defined(__cplusplus)
  }
#endif

#endif  // GUI_WINSUPPORT
#endif  // RADIO_H

/*************************** End of file ****************************/
