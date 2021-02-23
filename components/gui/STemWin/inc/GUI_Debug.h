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
File        : GUI_Debug.h
Purpose     : Debug macros
----------------------------------------------------------------------
   Debug macros for logging

 In the GUI Simulation, all output is transferred into the log window.
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
  
#ifndef GUI_DEBUG_H
#define GUI_DEBUG_H

#include <stddef.h>

#include "GUI.h"

#define GUI_DEBUG_LEVEL_NOCHECK       0  /* No run time checks are performed */
#define GUI_DEBUG_LEVEL_CHECK_PARA    1  /* Parameter checks are performed to avoid crashes */
#define GUI_DEBUG_LEVEL_CHECK_ALL     2  /* Parameter checks and consistency checks are performed */
#define GUI_DEBUG_LEVEL_LOG_ERRORS    3  /* Errors are recorded */
#define GUI_DEBUG_LEVEL_LOG_WARNINGS  4  /* Errors & Warnings are recorded */
#define GUI_DEBUG_LEVEL_LOG_ALL       5  /* Errors, Warnings and Messages are recorded. */

#ifndef GUI_DEBUG_LEVEL
  #ifdef WIN32
    #define GUI_DEBUG_LEVEL GUI_DEBUG_LEVEL_LOG_WARNINGS  /* Simulation should log all warnings */
  #else
    #define GUI_DEBUG_LEVEL GUI_DEBUG_LEVEL_CHECK_PARA  /* For most targets, min. size is important */
  #endif
#endif

#define GUI_LOCK_H(hMem)   GUI_ALLOC_LockH(hMem)
#define GUI_UNLOCK_H(pMem) GUI_ALLOC_UnlockH((void **)&pMem)

/*******************************************************************
*
*               Commandline
*
********************************************************************
*/

#ifdef WIN32
  #define GUI_DEBUG_GETCMDLINE() SIM_GetCmdLine()
#else
  #define GUI_DEBUG_GETCMDLINE() 0
#endif

/*******************************************************************
*
*               Error macros
*
********************************************************************
*/

/* Make sure the macros are actually defined */

#if GUI_DEBUG_LEVEL >= GUI_DEBUG_LEVEL_LOG_ERRORS
  #define GUI_DEBUG_ERROROUT(s)              GUI_ErrorOut(s)
  #define GUI_DEBUG_ERROROUT1(s,p0)          GUI_ErrorOut1(s,p0)
  #define GUI_DEBUG_ERROROUT2(s,p0,p1)       GUI_ErrorOut2(s,p0,p1)
  #define GUI_DEBUG_ERROROUT3(s,p0,p1,p2)    GUI_ErrorOut3(s,p0,p1,p2)
  #define GUI_DEBUG_ERROROUT4(s,p0,p1,p2,p3) GUI_ErrorOut4(s,p0,p1,p2,p3)
  #define GUI_DEBUG_ERROROUT_IF(exp,s)              { if (exp) GUI_DEBUG_ERROROUT(s); }
  #define GUI_DEBUG_ERROROUT1_IF(exp,s,p0)          { if (exp) GUI_DEBUG_ERROROUT1(s,p0); }
  #define GUI_DEBUG_ERROROUT2_IF(exp,s,p0,p1)       { if (exp) GUI_DEBUG_ERROROUT2(s,p0,p1); }
  #define GUI_DEBUG_ERROROUT3_IF(exp,s,p0,p1,p2)    { if (exp) GUI_DEBUG_ERROROUT3(s,p0,p1,p2); }
  #define GUI_DEBUG_ERROROUT4_IF(exp,s,p0,p1,p2,p3) { if (exp) GUI_DEBUG_ERROROUT4(s,p0,p1,p2,p3); }
#else
  #define GUI_DEBUG_ERROROUT(s)
  #define GUI_DEBUG_ERROROUT1(s,p0)
  #define GUI_DEBUG_ERROROUT2(s,p0,p1)
  #define GUI_DEBUG_ERROROUT3(s,p0,p1,p2)
  #define GUI_DEBUG_ERROROUT4(s,p0,p1,p2,p3)
  #define GUI_DEBUG_ERROROUT_IF(exp,s)
  #define GUI_DEBUG_ERROROUT1_IF(exp,s,p0)
  #define GUI_DEBUG_ERROROUT2_IF(exp,s,p0,p1)
  #define GUI_DEBUG_ERROROUT3_IF(exp,s,p0,p1,p2)
  #define GUI_DEBUG_ERROROUT4_IF(exp,s,p0,p1,p2,p3)
#endif

/*******************************************************************
*
*               Warning macros
*
********************************************************************
*/

/* Make sure the macros are actually defined */

#if GUI_DEBUG_LEVEL >= GUI_DEBUG_LEVEL_LOG_WARNINGS
  #define GUI_DEBUG_WARN(s)              GUI_Warn(s)
  #define GUI_DEBUG_WARN1(s,p0)          GUI_Warn1(s,p0)
  #define GUI_DEBUG_WARN2(s,p0,p1)       GUI_Warn2(s,p0,p1)
  #define GUI_DEBUG_WARN3(s,p0,p1,p2)    GUI_Warn3(s,p0,p1,p2)
  #define GUI_DEBUG_WARN4(s,p0,p1,p2,p3) GUI_Warn4(s,p0,p1,p2,p3)
  #define GUI_DEBUG_WARN_IF(exp,s)                  { if (exp) GUI_DEBUG_WARN(s); }
  #define GUI_DEBUG_WARN1_IF(exp,s,p0)              { if (exp) GUI_DEBUG_WARN1(s,p0); }
  #define GUI_DEBUG_WARN2_IF(exp,s,p0,p1)           { if (exp) GUI_DEBUG_WARN2(s,p0,p1); }
  #define GUI_DEBUG_WARN3_IF(exp,s,p0,p1,p2)        { if (exp) GUI_DEBUG_WARN3(s,p0,p1,p2); }
  #define GUI_DEBUG_WARN4_IF(exp,s,p0,p1,p2,p3)     { if (exp) GUI_DEBUG_WARN4(s,p0,p1,p2,p3); }
#else
  #define GUI_DEBUG_WARN(s)
  #define GUI_DEBUG_WARN1(s,p0)
  #define GUI_DEBUG_WARN2(s,p0,p1)
  #define GUI_DEBUG_WARN3(s,p0,p1,p2)
  #define GUI_DEBUG_WARN4(s,p0,p1,p2,p3)
  #define GUI_DEBUG_WARN_IF(exp,s)
  #define GUI_DEBUG_WARN1_IF(exp,s,p0)
  #define GUI_DEBUG_WARN2_IF(exp,s,p0,p1)
  #define GUI_DEBUG_WARN3_IF(exp,s,p0,p1,p2)
  #define GUI_DEBUG_WARN4_IF(exp,s,p0,p1,p2,p3)
#endif

/*******************************************************************
*
*               Logging macros
*
********************************************************************
*/
/* Make sure the macros are actually defined */

#if GUI_DEBUG_LEVEL >= GUI_DEBUG_LEVEL_LOG_ALL
  #define GUI_DEBUG_LOG(s)              GUI_Log(s)
  #define GUI_DEBUG_LOG1(s,p0)          GUI_Log1(s,p0)
  #define GUI_DEBUG_LOG2(s,p0,p1)       GUI_Log2(s,p0,p1)
  #define GUI_DEBUG_LOG3(s,p0,p1,p2)    GUI_Log3(s,p0,p1,p2)
  #define GUI_DEBUG_LOG4(s,p0,p1,p2,p3) GUI_Log4(s,p0,p1,p2,p3)
  #define GUI_DEBUG_LOG_IF(exp,s)                   { if (exp) GUI_DEBUG_LOG(s); }
  #define GUI_DEBUG_LOG1_IF(exp,s,p0)               { if (exp) GUI_DEBUG_LOG1(s,p0); }
  #define GUI_DEBUG_LOG2_IF(exp,s,p0,p1)            { if (exp) GUI_DEBUG_LOG2(s,p0,p1); }
  #define GUI_DEBUG_LOG3_IF(exp,s,p0,p1,p2)         { if (exp) GUI_DEBUG_LOG3(s,p0,p1,p2); }
  #define GUI_DEBUG_LOG4_IF(exp,s,p0,p1,p2,p3)      { if (exp) GUI_DEBUG_LOG4(s,p0,p1,p2,p3); }
#else
  #define GUI_DEBUG_LOG(s)
  #define GUI_DEBUG_LOG1(s,p0)
  #define GUI_DEBUG_LOG2(s,p0,p1)
  #define GUI_DEBUG_LOG3(s,p0,p1,p2)
  #define GUI_DEBUG_LOG4(s,p0,p1,p2,p3)
  #define GUI_DEBUG_LOG_IF(exp,s)
  #define GUI_DEBUG_LOG1_IF(exp,s,p0)
  #define GUI_DEBUG_LOG2_IF(exp,s,p0,p1)
  #define GUI_DEBUG_LOG3_IF(exp,s,p0,p1,p2)
  #define GUI_DEBUG_LOG4_IF(exp,s,p0,p1,p2,p3)
#endif

/*******************************************************************
*
*               Asserts
*
********************************************************************
*/
#if GUI_DEBUG_LEVEL >= GUI_DEBUG_LEVEL_LOG_ERRORS
  #define GUI_DEBUG_ASSERT(exp)                     { if (!exp) GUI_DEBUG_ERROROUT(#exp); }
#else
  #define GUI_DEBUG_ASSERT(exp)
#endif

#endif /* LCD_H */




/*************************** End of file ****************************/
