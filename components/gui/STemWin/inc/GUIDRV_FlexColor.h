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
File        : GUIDRV_FlexColor.h
Purpose     : Interface definition for GUIDRV_FlexColor driver
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
  
#ifndef GUIDRV_FLEXCOLOR_H
#define GUIDRV_FLEXCOLOR_H

#include "GUI.h"

#if defined(__cplusplus)
extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

/*********************************************************************
*
*       Configuration macros
*/
//
// Operation modes (16bpp)
//
#define GUIDRV_FLEXCOLOR_M16C0B8  GUIDRV_FlexColor_SetMode16bppC0B8
#define GUIDRV_FLEXCOLOR_M16C1B8  GUIDRV_FlexColor_SetMode16bppC1B8
#define GUIDRV_FLEXCOLOR_M16C0B16 GUIDRV_FlexColor_SetMode16bppC0B16
#define GUIDRV_FLEXCOLOR_M16C1B16 GUIDRV_FlexColor_SetMode16bppC1B16

//
// Operation modes (18bpp)
//
#define GUIDRV_FLEXCOLOR_M18C0B9  GUIDRV_FlexColor_SetMode18bppC0B9
#define GUIDRV_FLEXCOLOR_M18C1B9  GUIDRV_FlexColor_SetMode18bppC1B9
#define GUIDRV_FLEXCOLOR_M18C0B18 GUIDRV_FlexColor_SetMode18bppC0B18
#define GUIDRV_FLEXCOLOR_M18C1B18 GUIDRV_FlexColor_SetMode18bppC1B18

//
// Controller selection
//
#define GUIDRV_FLEXCOLOR_F66702   GUIDRV_FlexColor_SetFunc66702
#define GUIDRV_FLEXCOLOR_F66708   GUIDRV_FlexColor_SetFunc66708
#define GUIDRV_FLEXCOLOR_F66709   GUIDRV_FlexColor_SetFunc66709
#define GUIDRV_FLEXCOLOR_F66712   GUIDRV_FlexColor_SetFunc66712
#define GUIDRV_FLEXCOLOR_F66714   GUIDRV_FlexColor_SetFunc66714
#define GUIDRV_FLEXCOLOR_F66715   GUIDRV_FlexColor_SetFunc66715
#define GUIDRV_FLEXCOLOR_F66718   GUIDRV_FlexColor_SetFunc66718
#define GUIDRV_FLEXCOLOR_F66719   GUIDRV_FlexColor_SetFunc66719
#define GUIDRV_FLEXCOLOR_F66720   GUIDRV_FlexColor_SetFunc66720
#define GUIDRV_FLEXCOLOR_F66721   GUIDRV_FlexColor_SetFunc66721
#define GUIDRV_FLEXCOLOR_F66722   GUIDRV_FlexColor_SetFunc66722
#define GUIDRV_FLEXCOLOR_F66772   GUIDRV_FlexColor_SetFunc66772

//
// Hardware interfaces
//
#define GUIDRV_FLEXCOLOR_IF_TYPE_I  0
#define GUIDRV_FLEXCOLOR_IF_TYPE_II 1

//
// Reading interface
//
#define GUIDRV_FLEXCOLOR_READ_FUNC_I   0
#define GUIDRV_FLEXCOLOR_READ_FUNC_II  1
#define GUIDRV_FLEXCOLOR_READ_FUNC_III 2

/*********************************************************************
*
*       Configuration structure
*/
typedef struct {
  //
  // Driver specific configuration items
  //
  int FirstSEG;
  int FirstCOM;
  int Orientation;
  U16 RegEntryMode;
  int NumDummyReads;
} CONFIG_FLEXCOLOR;

/*********************************************************************
*
*       Display drivers
*/
//
// Addresses
//
extern const GUI_DEVICE_API GUIDRV_Win_API;

extern const GUI_DEVICE_API GUIDRV_FlexColor_API;

//
// Macros to be used in configuration files
//
#if defined(WIN32) && !defined(LCD_SIMCONTROLLER)

  #define GUIDRV_FLEXCOLOR &GUIDRV_Win_API

#else

  #define GUIDRV_FLEXCOLOR &GUIDRV_FlexColor_API

#endif

/*********************************************************************
*
*       Configuration interface
*/
#if !defined(WIN32) || defined(LCD_SIMCONTROLLER)

  //
  // Display controller configuration
  //
  void GUIDRV_FlexColor_SetFunc66702     (GUI_DEVICE * pDevice);
  void GUIDRV_FlexColor_SetFunc66708     (GUI_DEVICE * pDevice);
  void GUIDRV_FlexColor_SetFunc66709     (GUI_DEVICE * pDevice);
  void GUIDRV_FlexColor_SetFunc66712     (GUI_DEVICE * pDevice);
  void GUIDRV_FlexColor_SetFunc66714     (GUI_DEVICE * pDevice);
  void GUIDRV_FlexColor_SetFunc66715     (GUI_DEVICE * pDevice);
  void GUIDRV_FlexColor_SetFunc66718     (GUI_DEVICE * pDevice);
  void GUIDRV_FlexColor_SetFunc66719     (GUI_DEVICE * pDevice);
  void GUIDRV_FlexColor_SetFunc66720     (GUI_DEVICE * pDevice);
  void GUIDRV_FlexColor_SetFunc66721     (GUI_DEVICE * pDevice);
  void GUIDRV_FlexColor_SetFunc66722     (GUI_DEVICE * pDevice);
  void GUIDRV_FlexColor_SetFunc66772     (GUI_DEVICE * pDevice);

  //
  // Operation mode configuration (16bpp)
  //
  void GUIDRV_FlexColor_SetMode16bppC0B8 (GUI_DEVICE * pDevice);
  void GUIDRV_FlexColor_SetMode16bppC1B8 (GUI_DEVICE * pDevice);
  void GUIDRV_FlexColor_SetMode16bppC0B16(GUI_DEVICE * pDevice);
  void GUIDRV_FlexColor_SetMode16bppC1B16(GUI_DEVICE * pDevice);

  //
  // Drawing mode configuration (18bpp)
  //
  void GUIDRV_FlexColor_SetMode18bppC0B9 (GUI_DEVICE * pDevice);
  void GUIDRV_FlexColor_SetMode18bppC1B9 (GUI_DEVICE * pDevice);
  void GUIDRV_FlexColor_SetMode18bppC0B18(GUI_DEVICE * pDevice);
  void GUIDRV_FlexColor_SetMode18bppC1B18(GUI_DEVICE * pDevice);

  //
  // User interface
  //
  void GUIDRV_FlexColor_SetFunc(GUI_DEVICE * pDevice, GUI_PORT_API * pHW_API, void (* pfFunc)(GUI_DEVICE *), void (* pfMode)(GUI_DEVICE *));
  void GUIDRV_FlexColor_Config (GUI_DEVICE * pDevice, CONFIG_FLEXCOLOR * pConfig);

  //
  // Setting up hardware interface to be used
  //
  void GUIDRV_FlexColor_SetInterface66712_B9 (GUI_DEVICE * pDevice, int Type);
  void GUIDRV_FlexColor_SetInterface66712_B18(GUI_DEVICE * pDevice, int Type);
  #define GUIDRV_FlexColor_SetInterface66715_B9(pDevice, Type)  GUIDRV_FlexColor_SetInterface66712_B9(pDevice, Type)
  #define GUIDRV_FlexColor_SetInterface66715_B18(pDevice, Type) GUIDRV_FlexColor_SetInterface66712_B18(pDevice, Type)

  //
  // Setting up interface for reading back data
  //
  void GUIDRV_FlexColor_SetReadFunc66709_B16(GUI_DEVICE * pDevice, int Func);
  void GUIDRV_FlexColor_SetReadFunc66712_B9 (GUI_DEVICE * pDevice, int Func);
  void GUIDRV_FlexColor_SetReadFunc66712_B16(GUI_DEVICE * pDevice, int Func);
  #define GUIDRV_FlexColor_SetReadFunc66715_B9(pDevice, Func)  GUIDRV_FlexColor_SetReadFunc66712_B9(pDevice, Func)
  #define GUIDRV_FlexColor_SetReadFunc66715_B16(pDevice, Func) GUIDRV_FlexColor_SetReadFunc66712_B16(pDevice, Func)
  void GUIDRV_FlexColor_SetReadFunc66720_B16(GUI_DEVICE * pDevice, int Func);
  void GUIDRV_FlexColor_SetReadFunc66772_B8 (GUI_DEVICE * pDevice, int Func);
  void GUIDRV_FlexColor_SetReadFunc66772_B16(GUI_DEVICE * pDevice, int Func);

#else

  //
  // Display controller configuration
  //
  #define GUIDRV_FlexColor_SetFunc66708(pDevice)
  #define GUIDRV_FlexColor_SetFunc66709(pDevice)
  #define GUIDRV_FlexColor_SetFunc66712(pDevice)
  #define GUIDRV_FlexColor_SetFunc66714(pDevice)
  #define GUIDRV_FlexColor_SetFunc66715(pDevice)
  #define GUIDRV_FlexColor_SetFunc66718(pDevice)
  #define GUIDRV_FlexColor_SetFunc66719(pDevice)
  #define GUIDRV_FlexColor_SetFunc66720(pDevice)
  #define GUIDRV_FlexColor_SetFunc66721(pDevice)
  #define GUIDRV_FlexColor_SetFunc66722(pDevice)
  #define GUIDRV_FlexColor_SetFunc66772(pDevice)

  //
  // Operation mode configuration (16bpp)
  //
  #define GUIDRV_FlexColor_SetMode16bppC0B8(pDevice)
  #define GUIDRV_FlexColor_SetMode16bppC1B8(pDevice)
  #define GUIDRV_FlexColor_SetMode16bppC0B16(pDevice)
  #define GUIDRV_FlexColor_SetMode16bppC1B16(pDevice)

  //
  // Drawing mode configuration (18bpp)
  //
  #define GUIDRV_FlexColor_SetMode18bppC0B9(pDevice)
  #define GUIDRV_FlexColor_SetMode18bppC1B9(pDevice)
  #define GUIDRV_FlexColor_SetMode18bppC0B18(pDevice)
  #define GUIDRV_FlexColor_SetMode18bppC1B18(pDevice)

  //
  // User interface
  //
  #define GUIDRV_FlexColor_SetFunc(pDevice, pHW_API, pfFunc, pfMode)
  #define GUIDRV_FlexColor_Config(pDevice, pConfig)

  //
  // Setting up hardware interface to be used
  //
  #define GUIDRV_FlexColor_SetInterface66712_B9(pDevice, Type)
  #define GUIDRV_FlexColor_SetInterface66712_B18(pDevice, Type)
  #define GUIDRV_FlexColor_SetInterface66720_B18(pDevice, Type)
  #define GUIDRV_FlexColor_SetInterface66715_B9(pDevice, Type)
  #define GUIDRV_FlexColor_SetInterface66715_B18(pDevice, Type)

  //
  // Setting up interface for reading back data
  //
  #define GUIDRV_FlexColor_SetReadFunc66709_B16(pDevice, Func)
  #define GUIDRV_FlexColor_SetReadFunc66712_B9(pDevice, Func)
  #define GUIDRV_FlexColor_SetReadFunc66712_B16(pDevice, Func)
  #define GUIDRV_FlexColor_SetReadFunc66715_B9(pDevice, Func)
  #define GUIDRV_FlexColor_SetReadFunc66715_B16(pDevice, Func)
  #define GUIDRV_FlexColor_SetReadFunc66720_B16(pDevice, Func)
  #define GUIDRV_FlexColor_SetReadFunc66772_B8(pDevice, Func)
  #define GUIDRV_FlexColor_SetReadFunc66772_B16(pDevice, Func)

#endif

#if defined(__cplusplus)
}
#endif

#endif /* GUIDRV_FLEXCOLOR_H */

/*************************** End of file ****************************/
