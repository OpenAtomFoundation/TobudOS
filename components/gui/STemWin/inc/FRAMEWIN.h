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
File        : FRAMEWIN.h
Purpose     : Frame window include
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
  
#ifndef FRAMEWIN_H
#define FRAMEWIN_H

#include "WM.h"
#include "WIDGET.h"             /* Req. for WIDGET_DRAW_ITEM_FUNC */
#if GUI_WINSUPPORT
#include "DIALOG_Intern.h"      /* Req. for Create indirect data structure */

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
*       Configuration
*/
#ifndef   FRAMEWIN_ALLOW_DRAG_ON_FRAME
  #define FRAMEWIN_ALLOW_DRAG_ON_FRAME 1
#endif

/*********************************************************************
*
*       Color indices
*/
#define FRAMEWIN_CI_INACTIVE 0
#define FRAMEWIN_CI_ACTIVE   1

/*********************************************************************
*
*       Create / Status flags
*/
#define FRAMEWIN_CF_ACTIVE     (1<<3)
#define FRAMEWIN_CF_MOVEABLE   (1<<4)
#define FRAMEWIN_CF_TITLEVIS   (1<<5)
#define FRAMEWIN_CF_MINIMIZED  (1<<6)
#define FRAMEWIN_CF_MAXIMIZED  (1<<7)
#define FRAMEWIN_CF_DRAGGING   (1<<8)

#define FRAMEWIN_SF_ACTIVE     FRAMEWIN_CF_ACTIVE
#define FRAMEWIN_SF_MOVEABLE   FRAMEWIN_CF_MOVEABLE
#define FRAMEWIN_SF_TITLEVIS   FRAMEWIN_CF_TITLEVIS
#define FRAMEWIN_SF_MINIMIZED  FRAMEWIN_CF_MINIMIZED
#define FRAMEWIN_SF_MAXIMIZED  FRAMEWIN_CF_MAXIMIZED
#define FRAMEWIN_SF_DRAGGING   FRAMEWIN_CF_DRAGGING

/*********************************************************************
*
*       BUTTON Flags
*/
#define FRAMEWIN_BUTTON_RIGHT   (1<<0)
#define FRAMEWIN_BUTTON_LEFT    (1<<1)

/*********************************************************************
*
*       Skinning property indices
*/
#define FRAMEWIN_SKINFLEX_PI_ACTIVE   0
#define FRAMEWIN_SKINFLEX_PI_INACTIVE 1

/*********************************************************************
*
*       Getting border size
*/
#define FRAMEWIN_BORDERSIZE_T 0
#define FRAMEWIN_BORDERSIZE_L 1
#define FRAMEWIN_BORDERSIZE_B 2
#define FRAMEWIN_BORDERSIZE_R 3

/*********************************************************************
*
*       Types
*
**********************************************************************
*/
typedef WM_HMEM FRAMEWIN_Handle;

typedef struct {
  GUI_COLOR aColorFrame[3];
  GUI_COLOR aColorTitle[2];
  int Radius;
  int SpaceX;
  int BorderSizeL;
  int BorderSizeR;
  int BorderSizeT;
  int BorderSizeB;
} FRAMEWIN_SKINFLEX_PROPS;

/*********************************************************************
*
*       Create functions
*
**********************************************************************
*/
FRAMEWIN_Handle FRAMEWIN_Create        (const char * pTitle, WM_CALLBACK * cb, int Flags, int x0, int y0, int xSize, int ySize);
FRAMEWIN_Handle FRAMEWIN_CreateAsChild (int x0, int y0, int xSize, int ySize, WM_HWIN hParent, const char * pText, WM_CALLBACK * cb, int Flags);
FRAMEWIN_Handle FRAMEWIN_CreateEx      (int x0, int y0, int xSize, int ySize, WM_HWIN hParent, int WinFlags, int ExFlags, int Id, const char * pTitle, WM_CALLBACK * cb);
FRAMEWIN_Handle FRAMEWIN_CreateUser    (int x0, int y0, int xSize, int ySize, WM_HWIN hParent, int WinFlags, int ExFlags, int Id, const char * pTitle, WM_CALLBACK * cb, int NumExtraBytes);
FRAMEWIN_Handle FRAMEWIN_CreateIndirect(const GUI_WIDGET_CREATE_INFO * pCreateInfo, WM_HWIN hWinParent, int x0, int y0, WM_CALLBACK * cb);

/*********************************************************************
*
*       The callback ...
*
* Do not call it directly ! It is only to be used from within an
* overwritten callback.
*/
void FRAMEWIN_Callback(WM_MESSAGE * pMsg);

/*********************************************************************
*
*       Member functions: Set Properties
*
**********************************************************************
*/
WM_HWIN FRAMEWIN_AddButton     (FRAMEWIN_Handle hObj, int Flags, int Off, int Id);
WM_HWIN FRAMEWIN_AddCloseButton(FRAMEWIN_Handle hObj, int Flags, int Off);
WM_HWIN FRAMEWIN_AddMaxButton  (FRAMEWIN_Handle hObj, int Flags, int Off);
void    FRAMEWIN_AddMenu       (FRAMEWIN_Handle hObj, WM_HWIN hMenu);
WM_HWIN FRAMEWIN_AddMinButton  (FRAMEWIN_Handle hObj, int Flags, int Off);
void    FRAMEWIN_Minimize      (FRAMEWIN_Handle hObj);
void    FRAMEWIN_Maximize      (FRAMEWIN_Handle hObj);
void    FRAMEWIN_Restore       (FRAMEWIN_Handle hObj);
void    FRAMEWIN_SetActive     (FRAMEWIN_Handle hObj, int State);
void    FRAMEWIN_SetBarColor   (FRAMEWIN_Handle hObj, unsigned Index, GUI_COLOR Color);
void    FRAMEWIN_SetBorderSize (FRAMEWIN_Handle hObj, unsigned Size);
void    FRAMEWIN_SetClientColor(FRAMEWIN_Handle hObj, GUI_COLOR Color);
void    FRAMEWIN_SetFont       (FRAMEWIN_Handle hObj, const GUI_FONT * pFont);
void    FRAMEWIN_SetMoveable   (FRAMEWIN_Handle hObj, int State);
void    FRAMEWIN_SetOwnerDraw  (FRAMEWIN_Handle hObj, WIDGET_DRAW_ITEM_FUNC * pfDrawItem);
void    FRAMEWIN_SetResizeable (FRAMEWIN_Handle hObj, int State);
void    FRAMEWIN_SetText       (FRAMEWIN_Handle hObj, const char* s);
void    FRAMEWIN_SetTextAlign  (FRAMEWIN_Handle hObj, int Align);
void    FRAMEWIN_SetTextColor  (FRAMEWIN_Handle hObj, GUI_COLOR Color);
void    FRAMEWIN_SetTextColorEx(FRAMEWIN_Handle hObj, unsigned Index, GUI_COLOR Color);
void    FRAMEWIN_SetTitleVis   (FRAMEWIN_Handle hObj, int Show);
int     FRAMEWIN_SetTitleHeight(FRAMEWIN_Handle hObj, int Height);
int     FRAMEWIN_SetUserData   (FRAMEWIN_Handle hObj, const void * pSrc, int NumBytes);

/*********************************************************************
*
*       Member functions: Skinning
*
**********************************************************************
*/
void FRAMEWIN_GetSkinFlexProps     (FRAMEWIN_SKINFLEX_PROPS * pProps, int Index);
void FRAMEWIN_SetSkinClassic       (FRAMEWIN_Handle hObj);
void FRAMEWIN_SetSkin              (FRAMEWIN_Handle hObj, WIDGET_DRAW_ITEM_FUNC * pfDrawSkin);
int  FRAMEWIN_DrawSkinFlex         (const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo);
void FRAMEWIN_SetSkinFlexProps     (const FRAMEWIN_SKINFLEX_PROPS * pProps, int Index);
void FRAMEWIN_SetDefaultSkinClassic(void);
WIDGET_DRAW_ITEM_FUNC * FRAMEWIN_SetDefaultSkin(WIDGET_DRAW_ITEM_FUNC * pfDrawSkin);

#define FRAMEWIN_SKIN_FLEX    FRAMEWIN_DrawSkinFlex

/*********************************************************************
*
*       Member functions: Get Properties
*
**********************************************************************
*/
const GUI_FONT * FRAMEWIN_GetFont(FRAMEWIN_Handle hObj);

int       FRAMEWIN_GetActive      (FRAMEWIN_Handle hObj);
int       FRAMEWIN_GetTitleHeight (FRAMEWIN_Handle hObj);
GUI_COLOR FRAMEWIN_GetBarColor    (FRAMEWIN_Handle hObj, unsigned Index);
int       FRAMEWIN_GetBorderSize  (FRAMEWIN_Handle hObj);
int       FRAMEWIN_GetBorderSizeEx(FRAMEWIN_Handle hObj, unsigned Edge);
void      FRAMEWIN_GetText        (FRAMEWIN_Handle hObj, char * pBuffer, int MaxLen);
int       FRAMEWIN_GetTextAlign   (FRAMEWIN_Handle hObj);
int       FRAMEWIN_GetUserData    (FRAMEWIN_Handle hObj, void * pDest, int NumBytes);
int       FRAMEWIN_IsMinimized    (FRAMEWIN_Handle hObj);
int       FRAMEWIN_IsMaximized    (FRAMEWIN_Handle hObj);

/*********************************************************************
*
*       Managing default values
*
**********************************************************************
*/
GUI_COLOR        FRAMEWIN_GetDefaultBarColor   (unsigned Index);
int              FRAMEWIN_GetDefaultBorderSize (void);
int              FRAMEWIN_GetDefaultTitleHeight(void);
GUI_COLOR        FRAMEWIN_GetDefaultClientColor(void);
const GUI_FONT * FRAMEWIN_GetDefaultFont       (void);
GUI_COLOR        FRAMEWIN_GetDefaultTextColor  (unsigned Index);
int              FRAMEWIN_OwnerDraw            (const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo);
void             FRAMEWIN_SetDefaultBarColor   (unsigned Index, GUI_COLOR Color);
void             FRAMEWIN_SetDefaultBorderSize (int DefaultBorderSize);
void             FRAMEWIN_SetDefaultTitleHeight(int DefaultTitleHeight);
void             FRAMEWIN_SetDefaultClientColor(GUI_COLOR Color);
void             FRAMEWIN_SetDefaultFont       (const GUI_FONT * pFont);
int              FRAMEWIN_SetDefaultTextAlign  (int TextAlign);
void             FRAMEWIN_SetDefaultTextColor  (unsigned Index, GUI_COLOR Color);

/*********************************************************************
*
*       Macros for compatibility
*
**********************************************************************
*/
#define         FRAMEWIN_SetDefaultCaptionSize(Height)       FRAMEWIN_SetDefaultTitleHeight(Height)
#define         FRAMEWIN_GetDefaultCaptionSize()             FRAMEWIN_GetDefaultTitleHeight()
#define         FRAMEWIN_CreateButton(hObj, Flags, Off, Id)  FRAMEWIN_AddButton(hObj, Flags, Off, Id) 
#define         FRAMEWIN_CreateCloseButton(hObj, Flags, Off) FRAMEWIN_AddCloseButton(hObj, Flags, Off)
#define         FRAMEWIN_CreateMaxButton(hObj, Flags, Off)   FRAMEWIN_AddMaxButton(hObj, Flags, Off)  
#define         FRAMEWIN_CreateMinButton(hObj, Flags, Off)   FRAMEWIN_AddMinButton(hObj, Flags, Off)  

#if defined(__cplusplus)
  }
#endif

#endif  // GUI_WINSUPPORT
#endif  // FRAMEWIN_H

/*************************** End of file ****************************/
