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
File        : SWIPELIST.h
Purpose     : SWIPELIST include
--------------------END-OF-HEADER-------------------------------------
*/

#ifndef SWIPELIST_H
#define SWIPELIST_H

#include "WM.h"
#include "DIALOG_Intern.h"      // Req. for Create indirect data structure
#include "WIDGET.h"
#include "GUI_Debug.h"

#if GUI_WINSUPPORT

#if defined(__cplusplus)
  extern "C" {                  // Make sure we have C-declarations in C++ programs
#endif

/*********************************************************************
*
*       Create flags
*
**********************************************************************
*/
#define SWIPELIST_CF_HORIZONTAL (0 << 0)
#define SWIPELIST_CF_VERTICAL   (1 << 0)

/*********************************************************************
*
*       Getting border size
*/
#define SWIPELIST_BI_LEFT   0
#define SWIPELIST_BI_RIGHT  1
#define SWIPELIST_BI_TOP    2
#define SWIPELIST_BI_BOTTOM 3

/*********************************************************************
*
*       Getting font
*/
#define SWIPELIST_FI_SEP_ITEM    0
#define SWIPELIST_FI_ITEM_HEADER 1
#define SWIPELIST_FI_ITEM_TEXT   2

/*********************************************************************
*
*       Getting color
*/
#define SWIPELIST_CI_ITEM_HEADER_UNSEL 0
#define SWIPELIST_CI_ITEM_HEADER_SEL   1
#define SWIPELIST_CI_ITEM_TEXT_UNSEL   2
#define SWIPELIST_CI_ITEM_TEXT_SEL     3
#define SWIPELIST_CI_SEP_ITEM_TEXT     4

#define SWIPELIST_CI_BK_ITEM_UNSEL     0
#define SWIPELIST_CI_BK_ITEM_SEL       1
#define SWIPELIST_CI_BK_SEP_ITEM       2

/*********************************************************************
*
*       Bitmap align
*/
#define SWIPELIST_BA_LEFT      (0<<0)
#define SWIPELIST_BA_RIGHT	   (1<<0)
#define SWIPELIST_BA_HCENTER	 (2<<0)

#define SWIPELIST_BA_VCENTER   (3<<2)
#define SWIPELIST_BA_TOP	     (0<<2)
#define SWIPELIST_BA_BOTTOM	   (1<<2)


/*********************************************************************
*
*       Public Types
*
**********************************************************************
*/
typedef WM_HMEM SWIPELIST_Handle;

/*********************************************************************
*
*       Create functions
*
**********************************************************************
*/
SWIPELIST_Handle SWIPELIST_CreateAsChild  (int x0, int y0, int xSize, int ySize, WM_HWIN hParent, int Id, int Flags);
SWIPELIST_Handle SWIPELIST_CreateEx       (int x0, int y0, int xSize, int ySize, WM_HWIN hParent, int WinFlags, int ExFlags, int Id);
SWIPELIST_Handle SWIPELIST_CreateIndirect (const GUI_WIDGET_CREATE_INFO * pCreateInfo, WM_HWIN hWinParent, int x0, int y0, WM_CALLBACK * cb);
SWIPELIST_Handle SWIPELIST_CreateUser     (int x0, int y0, int xSize, int ySize, WM_HWIN hParent, int WinFlags, int ExFlags, int Id, int NumExtraBytes);

/*********************************************************************
*
*       The callback ...
*
* Do not call it directly ! It is only to be used from within an
* overwritten callback.
*/
void SWIPELIST_Callback(WM_MESSAGE * pMsg);

/*********************************************************************
*
*       Member functions
*
**********************************************************************
*/


int                SWIPELIST_AddItem                 (SWIPELIST_Handle hObj, const char * sText, int ItemSize);
int                SWIPELIST_AddItemText             (SWIPELIST_Handle hObj, unsigned ItemIndex, const char * sText);
int                SWIPELIST_AddSepItem              (SWIPELIST_Handle hObj, const char * sText, int ItemSize);
void               SWIPELIST_DeleteItem              (SWIPELIST_Handle hObj, unsigned ItemIndex);

const GUI_BITMAP * SWIPELIST_GetBitmap               (SWIPELIST_Handle hObj, unsigned ItemIndex);
int                SWIPELIST_GetBitmapSpace          (SWIPELIST_Handle hObj);
GUI_COLOR          SWIPELIST_GetBkColor              (SWIPELIST_Handle hObj, unsigned Index);
int                SWIPELIST_GetBorderSize           (SWIPELIST_Handle hObj, unsigned Index);
const GUI_FONT *   SWIPELIST_GetFont                 (SWIPELIST_Handle hObj, unsigned Index);
int                SWIPELIST_GetItemSize             (SWIPELIST_Handle hObj, unsigned ItemIndex);
U32                SWIPELIST_GetItemUserData         (SWIPELIST_Handle hObj, unsigned ItemIndex);
int                SWIPELIST_GetNumItems             (SWIPELIST_Handle hObj);
int                SWIPELIST_GetNumText              (SWIPELIST_Handle hObj, unsigned ItemIndex);
int                SWIPELIST_GetReleasedItem         (SWIPELIST_Handle hObj);
int                SWIPELIST_GetScrollPos            (SWIPELIST_Handle hObj);
int                SWIPELIST_GetSelItem              (SWIPELIST_Handle hObj);
GUI_COLOR          SWIPELIST_GetSepColor             (SWIPELIST_Handle hObj, unsigned ItemIndex);
int                SWIPELIST_GetSepSize              (SWIPELIST_Handle hObj, unsigned ItemIndex);
void               SWIPELIST_GetText                 (SWIPELIST_Handle hObj, unsigned ItemIndex, unsigned TextIndex, char * pBuffer, int MaxSize);
int                SWIPELIST_GetTextAlign            (SWIPELIST_Handle hObj, unsigned ItemIndex);
GUI_COLOR          SWIPELIST_GetTextColor            (SWIPELIST_Handle hObj, unsigned Index);
int                SWIPELIST_GetUserData             (SWIPELIST_Handle hObj, void * pDest, int NumBytes);

int                SWIPELIST_ItemAttachWindow        (SWIPELIST_Handle hObj, unsigned ItemIndex, WM_HWIN hWin, int x0, int y0);
void               SWIPELIST_ItemDetachWindow        (SWIPELIST_Handle hObj, WM_HWIN hWin);
int                SWIPELIST_OwnerDraw               (const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo);

void               SWIPELIST_SetAttachedWindowPos    (SWIPELIST_Handle hObj, WM_HWIN hWin, int x0, int y0);
void               SWIPELIST_SetBitmap               (SWIPELIST_Handle hObj, unsigned ItemIndex, int Align, const GUI_BITMAP * pBitmap);
void               SWIPELIST_SetBitmapEx             (SWIPELIST_Handle hObj, unsigned ItemIndex, int Align, const GUI_BITMAP * pBitmap, int x, int y);
void               SWIPELIST_SetBitmapSpace          (SWIPELIST_Handle hObj, unsigned Size);
void               SWIPELIST_SetBkColor              (SWIPELIST_Handle hObj, unsigned Index, GUI_COLOR Color);
void               SWIPELIST_SetBorderSize           (SWIPELIST_Handle hObj, unsigned Index, unsigned Size);
void               SWIPELIST_SetFont                 (SWIPELIST_Handle hObj, unsigned Index, const GUI_FONT * pFont);
void               SWIPELIST_SetItemSize             (SWIPELIST_Handle hObj, unsigned ItemIndex, unsigned Size);
void               SWIPELIST_SetItemUserData         (SWIPELIST_Handle hObj, unsigned ItemIndex, U32 UserData);
void               SWIPELIST_SetOwnerDraw            (SWIPELIST_Handle hObj, WIDGET_DRAW_ITEM_FUNC * pfDrawItem);
void               SWIPELIST_SetScrollPos            (SWIPELIST_Handle hObj, int Pos);
void               SWIPELIST_SetScrollPosItem        (SWIPELIST_Handle hObj, unsigned ItemIndex);
void               SWIPELIST_SetSepColor             (SWIPELIST_Handle hObj, unsigned ItemIndex, GUI_COLOR Color);
void               SWIPELIST_SetSepSize              (SWIPELIST_Handle hObj, unsigned ItemIndex, int Size);
void               SWIPELIST_SetText                 (SWIPELIST_Handle hObj, unsigned ItemIndex, unsigned TextIndex, char * sText);
void               SWIPELIST_SetTextAlign            (SWIPELIST_Handle hObj, unsigned ItemIndex, int Align);
void               SWIPELIST_SetTextColor            (SWIPELIST_Handle hObj, unsigned Index, GUI_COLOR Color);
int                SWIPELIST_SetUserData             (SWIPELIST_Handle hObj, const void * pSrc, int NumBytes);

/*********************************************************************
*
*       Global functions
*
**********************************************************************
*/
int              SWIPELIST_GetDefaultBitmapSpace     (void);
GUI_COLOR        SWIPELIST_GetDefaultBkColor         (unsigned Index);
int              SWIPELIST_GetDefaultBorderSize      (unsigned Index);
const GUI_FONT * SWIPELIST_GetDefaultFont            (unsigned Index);
GUI_COLOR        SWIPELIST_GetDefaultSepColor        (void);
unsigned         SWIPELIST_GetDefaultSepSize         (void);
GUI_COLOR        SWIPELIST_GetDefaultTextColor       (unsigned Index);
int              SWIPELIST_GetDefaultTextAlign       (void);

void             SWIPELIST_SetDefaultBitmapSpace     (unsigned Size);
void             SWIPELIST_SetDefaultBkColor         (unsigned Index, GUI_COLOR Color);
void             SWIPELIST_SetDefaultBorderSize      (unsigned Index, unsigned Size);
void             SWIPELIST_SetDefaultFont            (unsigned Index, const GUI_FONT * pFont);
void             SWIPELIST_SetDefaultSepColor        (GUI_COLOR Color);
void             SWIPELIST_SetDefaultSepSize         (unsigned Size);
void             SWIPELIST_SetDefaultTextColor       (unsigned Index, GUI_COLOR Color);
void             SWIPELIST_SetDefaultTextAlign       (int Align);

#if defined(__cplusplus)
  }
#endif

#endif  // GUI_WINSUPPORT
#endif  // SWIPELIST_H

/*************************** End of file ****************************/
