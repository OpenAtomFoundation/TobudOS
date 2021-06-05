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
File        : EDIT_Private.h
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
  
#ifndef EDIT_PRIVATE_H
#define EDIT_PRIVATE_H

#include "EDIT.h"

#if GUI_WINSUPPORT

#include "WIDGET.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define EDIT_REALLOC_SIZE  16

#ifndef EDIT_XOFF
  #define EDIT_XOFF        1
#endif

/*********************************************************************
*
*       Types
*
**********************************************************************
*/
typedef struct EDIT_Obj_struct EDIT_Obj;

typedef struct {
  int              Align;
  int              Border;
  const GUI_FONT * pFont;
  GUI_COLOR        aTextColor[3];
  GUI_COLOR        aBkColor[3];
} EDIT_PROPS;

struct EDIT_Obj_struct {
  WIDGET               Widget;
  WM_HMEM              hpText;
  I16                  MaxLen;
  U16                  BufferSize;
  I32                  Min, Max;        // Min max values as normalized floats (integers)
  U8                   NumDecs;         // Number of decimals
  I32                  CurrentValue;    // Current value
  int                  CursorPos;       // Cursor position. 0 means left most
  unsigned             SelSize;         // Number of selected characters
  U8                   EditMode;        // Insert or overwrite mode
  U8                   XSizeCursor;     // Size of cursor when working in insert mode
  U8                   Flags;
  tEDIT_AddKeyEx     * pfAddKeyEx;      // Handle key input
  tEDIT_UpdateBuffer * pfUpdateBuffer;  // Update textbuffer
  EDIT_PROPS           Props;
  WM_HTIMER            hTimer;
  U8                   MinMaxMode;
};

/*********************************************************************
*
*       Macros for internal use
*
**********************************************************************
*/
#if GUI_DEBUG_LEVEL >= GUI_DEBUG_LEVEL_CHECK_ALL
  #define EDIT_INIT_ID(p) (p->Widget.DebugId = EDIT_ID)
#else
  #define EDIT_INIT_ID(p)
#endif

#if GUI_DEBUG_LEVEL >= GUI_DEBUG_LEVEL_CHECK_ALL
  EDIT_Obj * EDIT_LockH(EDIT_Handle h);
  #define EDIT_LOCK_H(h)   EDIT_LockH(h)
#else
  #define EDIT_LOCK_H(h)   (EDIT_Obj *)GUI_LOCK_H(h)
#endif

/*********************************************************************
*
*       Public data (internal defaults)
*
**********************************************************************
*/
extern EDIT_PROPS EDIT__DefaultProps;

/*********************************************************************
*
*       Public functions (internal)
*
**********************************************************************
*/
U16  EDIT__GetCurrentChar  (EDIT_Obj * pObj);
void EDIT__SetCursorPos    (EDIT_Handle hObj, int CursorPos);
void EDIT__SetValueUnsigned(EDIT_Handle hObj, I32 Value);

#endif // GUI_WINSUPPORT

#endif // EDIT_PRIVATE_H

/*************************** End of file ****************************/
