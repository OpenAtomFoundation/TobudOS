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
File        : CALENDAR.h
Purpose     : Message box interface
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
  
#ifndef CALENDAR_H
#define CALENDAR_H

#include "WM.h"

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
#define CALENDAR_CI_WEEKEND 0
#define CALENDAR_CI_WEEKDAY 1
#define CALENDAR_CI_SEL     2

#define CALENDAR_CI_HEADER  3
#define CALENDAR_CI_MONTH   4
#define CALENDAR_CI_LABEL   5
#define CALENDAR_CI_FRAME   6

#define CALENDAR_FI_CONTENT 0
#define CALENDAR_FI_HEADER  1

#define CALENDAR_SI_HEADER  0
#define CALENDAR_SI_CELL_X  1
#define CALENDAR_SI_CELL_Y  2

/*********************************************************************
*
*       Notification codes
*
* The following is the list of notification codes specific to this widget,
* Send with the WM_NOTIFY_PARENT message
*/
#define CALENDAR_NOTIFICATION_MONTH_CLICKED  (WM_NOTIFICATION_WIDGET + 0)
#define CALENDAR_NOTIFICATION_MONTH_RELEASED (WM_NOTIFICATION_WIDGET + 1)

/*********************************************************************
*
*       Types
*
**********************************************************************
*/
/*********************************************************************
*
*       CALENDAR_DATE
*/
typedef struct {
  int Year;
  int Month;
  int Day;
} CALENDAR_DATE;

/*********************************************************************
*
*       CALENDAR_SKINFLEX_PROPS
*/
typedef struct {
  GUI_COLOR aColorFrame[3]; // Frame colors of buttons
  GUI_COLOR aColorUpper[2]; // Upper gradient colors of buttons
  GUI_COLOR aColorLower[2]; // Lower gradient colors of buttons
  GUI_COLOR ColorArrow;     // Arrow colors
} CALENDAR_SKINFLEX_PROPS;

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
WM_HWIN CALENDAR_Create           (WM_HWIN hParent, int xPos, int yPos, unsigned Year, unsigned Month, unsigned Day, unsigned FirstDayOfWeek, int Id, int Flags);
void    CALENDAR_GetDate          (WM_HWIN hWin, CALENDAR_DATE * pDate);
void    CALENDAR_GetSel           (WM_HWIN hWin, CALENDAR_DATE * pDate);
void    CALENDAR_SetDate          (WM_HWIN hWin, CALENDAR_DATE * pDate);
void    CALENDAR_SetSel           (WM_HWIN hWin, CALENDAR_DATE * pDate);
void    CALENDAR_ShowDate         (WM_HWIN hWin, CALENDAR_DATE * pDate);

/*********************************************************************
*
*       Default related
*/
void    CALENDAR_SetDefaultBkColor(unsigned Index, GUI_COLOR Color);
void    CALENDAR_SetDefaultColor  (unsigned Index, GUI_COLOR Color);
void    CALENDAR_SetDefaultDays   (const char ** apDays);
void    CALENDAR_SetDefaultFont   (unsigned Index, const GUI_FONT * pFont);
void    CALENDAR_SetDefaultMonths (const char ** apMonths);
void    CALENDAR_SetDefaultSize   (unsigned Index, unsigned Size);

/*********************************************************************
*
*       Skinning related
*/
void    CALENDAR_GetSkinFlexProps (CALENDAR_SKINFLEX_PROPS * pProps, int Index);
void    CALENDAR_SetSkinFlexProps (const CALENDAR_SKINFLEX_PROPS * pProps, int Index);

/*********************************************************************
*
*       The callback ...
*
* Do not call it directly ! It is only to be used from within an
* overwritten callback.
*/
void CALENDAR_Callback(WM_MESSAGE * pMsg);

#if defined(__cplusplus)
  }
#endif

#endif // GUI_WINSUPPORT
#endif // CALENDAR_H

/*************************** End of file ****************************/
