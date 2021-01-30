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
File        : HEADER.h
Purpose     : HEADER include
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
  
#ifndef HEADER_H
#define HEADER_H

#include "WM.h"
#include "DIALOG_Intern.h"      /* Req. for Create indirect data structure */
#include "WIDGET.h"

#if GUI_WINSUPPORT

#if defined(__cplusplus)
  extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

/*********************************************************************
*
*       Public Types
*
**********************************************************************
*/

typedef WM_HMEM HEADER_Handle;

typedef struct {
  GUI_COLOR aColorFrame[2];
  GUI_COLOR aColorUpper[2];
  GUI_COLOR aColorLower[2];
  GUI_COLOR ColorArrow;
} HEADER_SKINFLEX_PROPS;

/*********************************************************************
*
*       Create functions
*
**********************************************************************
*/

HEADER_Handle HEADER_Create        (int x0, int y0, int xSize, int ySize, WM_HWIN hParent, int Id, int Flags, int SpecialFlags);
HEADER_Handle HEADER_CreateAttached(WM_HWIN hParent, int Id, int SpecialFlags);
HEADER_Handle HEADER_CreateEx      (int x0, int y0, int xSize, int ySize, WM_HWIN hParent, int WinFlags, int ExFlags, int Id);
HEADER_Handle HEADER_CreateUser    (int x0, int y0, int xSize, int ySize, WM_HWIN hParent, int WinFlags, int ExFlags, int Id, int NumExtraBytes);
HEADER_Handle HEADER_CreateIndirect(const GUI_WIDGET_CREATE_INFO * pCreateInfo, WM_HWIN hWinParent, int x0, int y0, WM_CALLBACK * cb);

/*********************************************************************
*
*       The callback ...
*
* Do not call it directly ! It is only to be used from within an
* overwritten callback.
*/
void HEADER_Callback(WM_MESSAGE * pMsg);

/*********************************************************************
*
*       Managing default values
*
**********************************************************************
*/
/* Set defaults */
GUI_COLOR          HEADER_SetDefaultArrowColor(GUI_COLOR Color);
GUI_COLOR          HEADER_SetDefaultBkColor   (GUI_COLOR Color);
const GUI_CURSOR * HEADER_SetDefaultCursor    (const GUI_CURSOR * pCursor);
const GUI_FONT *   HEADER_SetDefaultFont      (const GUI_FONT * pFont);
int                HEADER_SetDefaultBorderH   (int Spacing);
int                HEADER_SetDefaultBorderV   (int Spacing);
GUI_COLOR          HEADER_SetDefaultTextColor (GUI_COLOR Color);

/* Get defaults */
GUI_COLOR          HEADER_GetDefaultArrowColor(void);
GUI_COLOR          HEADER_GetDefaultBkColor   (void);
const GUI_CURSOR * HEADER_GetDefaultCursor    (void);
const GUI_FONT *   HEADER_GetDefaultFont      (void);
int                HEADER_GetDefaultBorderH   (void);
int                HEADER_GetDefaultBorderV   (void);
GUI_COLOR          HEADER_GetDefaultTextColor (void);

/*********************************************************************
*
*       Member functions
*
**********************************************************************
*/
void      HEADER_AddItem            (HEADER_Handle hObj, int Width, const char * s, int Align);
void      HEADER_DeleteItem         (HEADER_Handle hObj, unsigned Index);
GUI_COLOR HEADER_GetArrowColor      (HEADER_Handle hObj);
GUI_COLOR HEADER_GetBkColor         (HEADER_Handle hObj);
int       HEADER_GetHeight          (HEADER_Handle hObj);
int       HEADER_GetItemWidth       (HEADER_Handle hObj, unsigned int Index);
int       HEADER_GetNumItems        (HEADER_Handle hObj);
int       HEADER_GetSel             (HEADER_Handle hObj);
GUI_COLOR HEADER_GetTextColor       (HEADER_Handle hObj);
int       HEADER_GetUserData        (HEADER_Handle hObj, void * pDest, int NumBytes);
void      HEADER_SetArrowColor      (HEADER_Handle hObj, GUI_COLOR Color);
void      HEADER_SetBitmap          (HEADER_Handle hObj, unsigned int Index, const GUI_BITMAP * pBitmap);
void      HEADER_SetBitmapEx        (HEADER_Handle hObj, unsigned int Index, const GUI_BITMAP * pBitmap, int x, int y);
void      HEADER_SetBkColor         (HEADER_Handle hObj, GUI_COLOR Color);
void      HEADER_SetBMP             (HEADER_Handle hObj, unsigned int Index, const void * pBitmap);
void      HEADER_SetBMPEx           (HEADER_Handle hObj, unsigned int Index, const void * pBitmap, int x, int y);
void      HEADER_SetDirIndicator    (HEADER_Handle hObj, int Column, int Reverse); /* !!!Not to be documented!!! */
void      HEADER_SetDragLimit       (HEADER_Handle hObj, unsigned DragLimit);
unsigned  HEADER_SetFixed           (HEADER_Handle hObj, unsigned Fixed);
void      HEADER_SetFont            (HEADER_Handle hObj, const GUI_FONT * pFont);
void      HEADER_SetHeight          (HEADER_Handle hObj, int Height);
void      HEADER_SetTextAlign       (HEADER_Handle hObj, unsigned int Index, int Align);
void      HEADER_SetItemText        (HEADER_Handle hObj, unsigned int Index, const char * s);
void      HEADER_SetItemWidth       (HEADER_Handle hObj, unsigned int Index, int Width);
void      HEADER_SetScrollPos       (HEADER_Handle hObj, int ScrollPos);
void      HEADER_SetStreamedBitmap  (HEADER_Handle hObj, unsigned int Index, const GUI_BITMAP_STREAM * pBitmap);
void      HEADER_SetStreamedBitmapEx(HEADER_Handle hObj, unsigned int Index, const GUI_BITMAP_STREAM * pBitmap, int x, int y);
void      HEADER_SetTextColor       (HEADER_Handle hObj, GUI_COLOR Color);
int       HEADER_SetUserData        (HEADER_Handle hObj, const void * pSrc, int NumBytes);

/*********************************************************************
*
*       Member functions: Skinning
*
**********************************************************************
*/
void HEADER_GetSkinFlexProps     (HEADER_SKINFLEX_PROPS * pProps, int Index);
void HEADER_SetSkinClassic       (HEADER_Handle hObj);
void HEADER_SetSkin              (HEADER_Handle hObj, WIDGET_DRAW_ITEM_FUNC * pfDrawSkin);
int  HEADER_DrawSkinFlex         (const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo);
void HEADER_SetSkinFlexProps     (const HEADER_SKINFLEX_PROPS * pProps, int Index);
void HEADER_SetDefaultSkinClassic(void);
WIDGET_DRAW_ITEM_FUNC * HEADER_SetDefaultSkin(WIDGET_DRAW_ITEM_FUNC * pfDrawSkin);

#define HEADER_SKIN_FLEX          HEADER_DrawSkinFlex

/*********************************************************************
*
*       Macros for compatibility with older versions
*
**********************************************************************
*/

#ifdef HEADER_SPACING_H
  #define HEADER_BORDER_H_DEFAULT HEADER_SPACING_H
#endif
#ifdef HEADER_SPACING_V
  #define HEADER_BORDER_V_DEFAULT HEADER_SPACING_V
#endif
#define HEADER_SetDefaultSpacingH(Value) HEADER_SetDefaultBorderH(Value)
#define HEADER_SetDefaultSpacingV(Value) HEADER_SetDefaultBorderV(Value)
#define HEADER_GetDefaultSpacingH()      HEADER_GetDefaultBorderH()
#define HEADER_GetDefaultSpacingV()      HEADER_GetDefaultBorderV()

#if defined(__cplusplus)
  }
#endif

#endif  // GUI_WINSUPPORT
#endif  // HEADER_H

/*************************** End of file ****************************/
