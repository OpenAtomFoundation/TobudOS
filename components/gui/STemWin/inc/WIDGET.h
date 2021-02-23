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
File        : WIDGET.h
Purpose     : Widget interface
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
  
#ifndef WIDGET_H        /* Avoid multiple inclusion  */
#define WIDGET_H

#if defined(__cplusplus)
extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

#include "WM_Intern.h"  /* Window manager, including some internals, which speed things up */

#if GUI_WINSUPPORT

/*********************************************************************
*
*       Typedefs
*
**********************************************************************
*/
typedef struct {
  WM_HWIN    hWin;
  int        Cmd;         /* WIDGET_ITEM_GET_XSIZE, WIDGET_ITEM_GET_YSIZE, WIDGET_ITEM_DRAW, */
  int        ItemIndex;
  int        Col;
  int        x0, y0, x1, y1;
  void     * p;
} WIDGET_ITEM_DRAW_INFO;

typedef int  WIDGET_DRAW_ITEM_FUNC(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo);
typedef void WIDGET_PAINT         (WM_HWIN hObj);
typedef void WIDGET_CREATE        (WM_HWIN hObj);

typedef struct {
  WIDGET_PAINT  * pfPaint;
  WIDGET_CREATE * pfCreate;
  void          * pSkinPrivate;
} WIDGET_SKIN;

/*********************************************************************
*
*       Important: WIDGET_DRAW_ITEM_FUNC needs to be defined
*                  in SCROLLBAR.h!
*
**********************************************************************
*/
#include "SCROLLBAR.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
/*********************************************************************
*
*       Unique widget id's
*/
#define BUTTON_ID    0x42555454UL /* BUTT */
#define CHECKBOX_ID  0x43484543UL /* CHEC */
#define DROPDOWN_ID  0x44524f50UL /* DROP */
#define EDIT_ID      0x45444954UL /* EDIT */
#define FRAMEWIN_ID  0x4652414dUL /* FRAM */
#define FRAMECLNT_ID 0x46524143UL /* FRAC */
#define GRAPH_ID     0x47524150UL /* GRAP */
#define HEADER_ID    0x48454144UL /* HEAD */
#define LISTBOX_ID   0x4C495342UL /* LISB */
#define LISTVIEW_ID  0x4C495356UL /* LISV */
#define LISTWHEEL_ID 0x4C495357UL /* LISW */
#define MENU_ID      0x4d454e55UL /* MENU */
#define MULTIEDIT_ID 0x4d554c45UL /* MULE */
#define MULTIPAGE_ID 0x4d554c50UL /* MULP */
#define MPAGECLNT_ID 0x4d50434CUL /* MPCL */
#define PROGBAR_ID   0x50524f47UL /* PROG */
#define RADIO_ID     0x52414449UL /* RADI */
#define SCROLLBAR_ID 0x5343524fUL /* SCRO */
#define SLIDER_ID    0x534c4944UL /* SLID */
#define SWIPELIST_ID 0x53574950UL /* SWIP */
#define TEXT_ID      0x54455854UL /* TEXT */
#define TREEVIEW_ID  0x54524545UL /* TREE */
#define ICONVIEW_ID  0x49434f4eUL /* ICON */
#define IMAGE_ID     0x494d4147UL /* IMAG */
#define SPINBOX_ID   0x5350494eUL /* SPIN */
#define KNOB_ID      0x4b4e4f42UL /* KNOB */
#define WINDOW_ID    0x57494e44UL /* WIND */

#define WIDGET_LOCK(hWin)       ((WIDGET*)GUI_LOCK_H(hWin))

/*********************************************************************
*
*       Config switches
*/

#ifndef   WIDGET_USE_PARENT_EFFECT
  #define WIDGET_USE_PARENT_EFFECT 0
#endif
#ifndef   WIDGET_USE_FLEX_SKIN
  #define WIDGET_USE_FLEX_SKIN     1
#endif
#if !defined(WIDGET_USE_SCHEME_SMALL) && !defined(WIDGET_USE_SCHEME_MEDIUM) && !defined(WIDGET_USE_SCHEME_LARGE)
  #define WIDGET_USE_SCHEME_SMALL  1
  #define WIDGET_USE_SCHEME_MEDIUM 0
  #define WIDGET_USE_SCHEME_LARGE  0
#endif
#ifndef   WIDGET_USE_SCHEME_SMALL
  #define WIDGET_USE_SCHEME_SMALL  0
#endif
#ifndef   WIDGET_USE_SCHEME_MEDIUM
  #define WIDGET_USE_SCHEME_MEDIUM 0
#endif
#ifndef   WIDGET_USE_SCHEME_LARGE
  #define WIDGET_USE_SCHEME_LARGE  0
#endif
#if (WIDGET_USE_SCHEME_SMALL + WIDGET_USE_SCHEME_MEDIUM + WIDGET_USE_SCHEME_LARGE) > 1
  #error Only one scheme can be selected!
#endif

/*********************************************************************
*
*       States
*/

#define WIDGET_STATE_FOCUS              (1 << 0)
#define WIDGET_STATE_VERTICAL           (1 << 3)
#define WIDGET_STATE_FOCUSSABLE         (1 << 4)

#define WIDGET_STATE_USER0              (1 << 8)    /* Freely available for derived widget */
#define WIDGET_STATE_USER1              (1 << 9)    /* Freely available for derived widget */
#define WIDGET_STATE_USER2              (1 << 10)   /* Freely available for derived widget */

/*********************************************************************
*
*       Skinning message identifiers
*/
#define WIDGET_ITEM_CREATE              0
#define WIDGET_ITEM_DRAW                1
#define WIDGET_ITEM_DRAW_ARROW          2
#define WIDGET_ITEM_DRAW_ARROW_L        3
#define WIDGET_ITEM_DRAW_ARROW_R        4
#define WIDGET_ITEM_DRAW_BACKGROUND     5
#define WIDGET_ITEM_DRAW_BITMAP         6
#define WIDGET_ITEM_DRAW_BUTTON         7
#define WIDGET_ITEM_DRAW_BUTTON_L       8
#define WIDGET_ITEM_DRAW_BUTTON_R       9
#define WIDGET_ITEM_DRAW_FOCUS         10
#define WIDGET_ITEM_DRAW_FRAME         11
#define WIDGET_ITEM_DRAW_OVERLAP       12
#define WIDGET_ITEM_DRAW_OVERLAY       13
#define WIDGET_ITEM_DRAW_SEP           14
#define WIDGET_ITEM_DRAW_SHAFT         15
#define WIDGET_ITEM_DRAW_SHAFT_L       16
#define WIDGET_ITEM_DRAW_SHAFT_R       17
#define WIDGET_ITEM_DRAW_TEXT          18
#define WIDGET_ITEM_DRAW_THUMB         19
#define WIDGET_ITEM_DRAW_TICKS         20
#define WIDGET_ITEM_GET_BORDERSIZE_B   21
#define WIDGET_ITEM_GET_BORDERSIZE_L   22
#define WIDGET_ITEM_GET_BORDERSIZE_R   23
#define WIDGET_ITEM_GET_BORDERSIZE_T   24
#define WIDGET_ITEM_GET_BUTTONSIZE     25
#define WIDGET_ITEM_GET_XSIZE          26
#define WIDGET_ITEM_GET_YSIZE          27
#define WIDGET_ITEM_GET_RADIUS         28
#define WIDGET_ITEM_APPLY_PROPS        29  // Not to be documented. Use this message identifier to update the
                                           // properties of attached widgets from <WIDGET>_DrawSkinFlex().

#define WIDGET_DRAW_OVERLAY    WIDGET_ITEM_DRAW_OVERLAY   
#define WIDGET_DRAW_BACKGROUND WIDGET_ITEM_DRAW_BACKGROUND

/*********************************************************************
*
*       Messages
*/

#define WM_WIDGET_SET_EFFECT    (WM_WIDGET + 0)

/*********************************************************************
*
*       Create flags
*/

#define WIDGET_CF_VERTICAL      WIDGET_STATE_VERTICAL

/*********************************************************************
*
*        Widget object
*
* The widget object is the base class for most widgets
*/
typedef struct {
  int EffectSize;
  void (* pfDrawUp)      (void);
  void (* pfDrawUpRect)  (const GUI_RECT * pRect);
  void (* pfDrawDown)    (void);
  void (* pfDrawDownRect)(const GUI_RECT * pRect);
  void (* pfDrawFlat)    (void);
  void (* pfDrawFlatRect)(const GUI_RECT * pRect);
} WIDGET_EFFECT;

typedef struct {
  WM_Obj      Win;
  const WIDGET_EFFECT* pEffect;
  I16 Id;
  U16 State;
  #if GUI_DEBUG_LEVEL > 1
    U32 DebugId;
  #endif  
} WIDGET;


/*********************************************************************
*
*         GUI_DRAW
*
* The GUI_DRAW object is used as base class for selfdrawing,
* non-windows objects. They are used as content of different widgets,
* such as the bitmap or header widgets.
*/
/* Declare Object struct */
typedef struct GUI_DRAW GUI_DRAW;
typedef void   GUI_DRAW_SELF_CB (WM_HWIN hWin);
typedef WM_HMEM GUI_DRAW_HANDLE;

/* Declare Object constants (member functions etc)  */
typedef struct {
  void (* pfDraw)    (GUI_DRAW_HANDLE hDrawObj, WM_HWIN hObj, int x, int y);
  int  (* pfGetXSize)(GUI_DRAW_HANDLE hDrawObj);
  int  (* pfGetYSize)(GUI_DRAW_HANDLE hDrawObj);
} GUI_DRAW_CONSTS;

/* Declare Object */
struct GUI_DRAW {
  const GUI_DRAW_CONSTS* pConsts;
  union {
    const void * pData;
    GUI_DRAW_SELF_CB* pfDraw;
  } Data;
  I16 xOff, yOff;
};

/* GUI_DRAW_ API */
void GUI_DRAW__Draw    (GUI_DRAW_HANDLE hDrawObj, WM_HWIN hObj, int x, int y);
int  GUI_DRAW__GetXSize(GUI_DRAW_HANDLE hDrawObj);
int  GUI_DRAW__GetYSize(GUI_DRAW_HANDLE hDrawObj);

/* GUI_DRAW_ Constructurs for different objects */
WM_HMEM GUI_DRAW_BITMAP_Create  (const GUI_BITMAP* pBitmap, int x, int y);
WM_HMEM GUI_DRAW_BMP_Create     (const void* pBMP, int x, int y);
WM_HMEM GUI_DRAW_STREAMED_Create(const GUI_BITMAP_STREAM * pBitmap, int x, int y);
WM_HMEM GUI_DRAW_SELF_Create(GUI_DRAW_SELF_CB* pfDraw, int x, int y);

/*********************************************************************
*
*       Global data
*
**********************************************************************
*/

extern const WIDGET_EFFECT WIDGET_Effect_3D;
extern const WIDGET_EFFECT WIDGET_Effect_3D1L;
extern const WIDGET_EFFECT WIDGET_Effect_3D2L;
extern const WIDGET_EFFECT WIDGET_Effect_None;
extern const WIDGET_EFFECT WIDGET_Effect_Simple;

/*********************************************************************
*
*       Internal API routines
*
**********************************************************************
*/

void      WIDGET__DrawFocusRect      (WIDGET * pWidget, const GUI_RECT * pRect, int Dist);
void      WIDGET__DrawHLine          (WIDGET * pWidget, int y, int x0, int x1);
void      WIDGET__DrawTriangle       (WIDGET * pWidget, int x, int y, int Size, int Inc);
void      WIDGET__DrawVLine          (WIDGET * pWidget, int x, int y0, int y1);
void      WIDGET__EFFECT_DrawDownRect(WIDGET * pWidget, GUI_RECT * pRect);
void      WIDGET__EFFECT_DrawDown    (WIDGET * pWidget);
void      WIDGET__EFFECT_DrawUpRect  (WIDGET * pWidget, GUI_RECT * pRect);
void      WIDGET__FillRectEx         (WIDGET * pWidget, const GUI_RECT * pRect);
int       WIDGET__GetWindowSizeX     (WM_HWIN hWin);
GUI_COLOR WIDGET__GetBkColor         (WM_HWIN hObj);
int       WIDGET__GetXSize           (const WIDGET * pWidget);
int       WIDGET__GetYSize           (const WIDGET * pWidget);
void      WIDGET__GetClientRect      (WIDGET * pWidget, GUI_RECT * pRect);
void      WIDGET__GetInsideRect      (WIDGET * pWidget, GUI_RECT * pRect);
void      WIDGET__Init               (WIDGET * pWidget, int Id, U16 State);
void      WIDGET__RotateRect90       (WIDGET * pWidget, GUI_RECT * pDest, const GUI_RECT * pRect);
void      WIDGET__SetScrollState     (WM_HWIN hWin, const WM_SCROLL_STATE * pVState, const WM_SCROLL_STATE * pState);
void      WIDGET__FillStringInRect   (const char * pText, const GUI_RECT * pFillRect, const GUI_RECT * pTextRectMax, const GUI_RECT * pTextRectAct);

/*********************************************************************
*
*       API routines
*
**********************************************************************
*/
void  WIDGET_SetState     (WM_HWIN hObj, int State);
void  WIDGET_AndState     (WM_HWIN hObj, int State);
void  WIDGET_OrState      (WM_HWIN hObj, int State);
int   WIDGET_HandleActive (WM_HWIN hObj, WM_MESSAGE* pMsg);
int   WIDGET_GetState     (WM_HWIN hObj);
int   WIDGET_SetWidth     (WM_HWIN hObj, int Width);

void  WIDGET_EFFECT_3D_DrawUp(void);

const WIDGET_EFFECT* WIDGET_SetDefaultEffect(const WIDGET_EFFECT* pEffect);

void  WIDGET_SetEffect              (WM_HWIN hObj, const WIDGET_EFFECT* pEffect);

const WIDGET_EFFECT* WIDGET_GetDefaultEffect(void);

void WIDGET_EFFECT_3D_SetColor    (unsigned Index, GUI_COLOR Color);
void WIDGET_EFFECT_3D1L_SetColor  (unsigned Index, GUI_COLOR Color);
void WIDGET_EFFECT_3D2L_SetColor  (unsigned Index, GUI_COLOR Color);
void WIDGET_EFFECT_Simple_SetColor(unsigned Index, GUI_COLOR Color);

GUI_COLOR WIDGET_EFFECT_3D_GetColor    (unsigned Index);
GUI_COLOR WIDGET_EFFECT_3D1L_GetColor  (unsigned Index);
GUI_COLOR WIDGET_EFFECT_3D2L_GetColor  (unsigned Index);
GUI_COLOR WIDGET_EFFECT_Simple_GetColor(unsigned Index);

int WIDGET_EFFECT_3D_GetNumColors(void);
int WIDGET_EFFECT_3D1L_GetNumColors(void);
int WIDGET_EFFECT_3D2L_GetNumColors(void);
int WIDGET_EFFECT_Simple_GetNumColors(void);

/*********************************************************************
*
*       Compatibility macros
*
**********************************************************************
*/
#define WIDGET_SetDefaultEffect_3D()     WIDGET_SetDefaultEffect(&WIDGET_Effect_3D)
#define WIDGET_SetDefaultEffect_3D1L()   WIDGET_SetDefaultEffect(&WIDGET_Effect_3D1L)
#define WIDGET_SetDefaultEffect_3D2L()   WIDGET_SetDefaultEffect(&WIDGET_Effect_3D2L)
#define WIDGET_SetDefaultEffect_None()   WIDGET_SetDefaultEffect(&WIDGET_Effect_None)
#define WIDGET_SetDefaultEffect_Simple() WIDGET_SetDefaultEffect(&WIDGET_Effect_Simple)

#endif /* GUI_WINSUPPORT */

#if defined(__cplusplus)
  }
#endif

#endif   /* SLIDER_H */




