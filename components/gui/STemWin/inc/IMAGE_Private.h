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
File        : IMAGE_Private.h
Purpose     : Private IMAGE include
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
  
#ifndef IMAGE_PRIVATE_H
#define IMAGE_PRIVATE_H

#include "IMAGE.h"
#include "GUI_Private.h"

#if GUI_WINSUPPORT

/*********************************************************************
*
*       Private config defaults
*
**********************************************************************
*/
#ifndef IMAGE_SUPPORT_TRANSPARENCY
  #define IMAGE_SUPPORT_TRANSPARENCY WM_SUPPORT_TRANSPARENCY
#endif

/*********************************************************************
*
*       Object definition
*
**********************************************************************
*/
typedef struct {
  U32 Flags;
} IMAGE_PROPS;

typedef struct {
  WIDGET              Widget;
  IMAGE_PROPS         Props;
  const void        * pData;                                                          // Data pointer of the object to be drawn (Bitmap, BMP, GIF, JPEG, PNG)
  void             (* pfDrawImageAt) (IMAGE_Handle hObj, int xPos, int yPos);         // Object specific (Bitmap, BMP, GIF, JPEG, PNG) code
  void             (* pfPaint)       (IMAGE_Handle hObj);                             // Drawing method specific (Default, Tiled, Magnified) code
  void             (* pfGetImageSize)(IMAGE_Handle hObj, int * pxSize, int * pySize); // Returns the image size of the attached item
  void             (* pfOnTimer)     (IMAGE_Handle hObj);                             // Timer function for animated images (currently only animated GIFs are supported)
  U32                 FileSize;
  //
  // Data items used by IAMGE_GIF.c
  //
  int                 NumImages;    // Number of (sub)images
  int                 CurrentImage; // Image index used for animated images
  GUI_TIMER_HANDLE    hTimer;       // Timer used for animated images
  //
  // Data items used by IAMGE_DTA.c
  //
  GUI_BITMAP          Bitmap;       // Bitmap structure
  GUI_LOGPALETTE      Palette;      // Palette structure
  //
  // Data items used by Image_...Ex() - functions
  //
  void              * pVoid;        // Void pointer passed to GetData() function
  GUI_GET_DATA_FUNC * pfGetData;    // Pointer to GetData() function
  //
  // Data items used if memory devices are available and IMAGE_CF_MEMDEV has been set
  //
  #if GUI_SUPPORT_MEMDEV
    GUI_MEMDEV_Handle hMem;
  #endif
} IMAGE_OBJ;

/*********************************************************************
*
*       Macros for internal use
*
**********************************************************************
*/
#if GUI_DEBUG_LEVEL >= GUI_DEBUG_LEVEL_CHECK_ALL
  #define IMAGE_INIT_ID(p) (p->Widget.DebugId = IMAGE_ID)
#else
  #define IMAGE_INIT_ID(p)
#endif

#if GUI_DEBUG_LEVEL >= GUI_DEBUG_LEVEL_CHECK_ALL
  IMAGE_OBJ * IMAGE__LockH(IMAGE_Handle h);
  #define IMAGE_LOCK_H(h)   IMAGE__LockH(h)
#else
  #define IMAGE_LOCK_H(h)   (IMAGE_OBJ *)GUI_LOCK_H(h)
#endif

/*********************************************************************
*
*       Module internal data
*
**********************************************************************
*/
extern IMAGE_PROPS IMAGE__DefaultProps;

/*********************************************************************
*
*       Private functions
*
**********************************************************************
*/
void IMAGE__SetWindowSize(IMAGE_Handle hObj);
void IMAGE__FreeAttached (IMAGE_Handle hObj, int LeaveTimer);

#endif // GUI_WINSUPPORT
#endif // IMAGE_PRIVATE_H

/*************************** End of file ****************************/
