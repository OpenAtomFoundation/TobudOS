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
File        : GUITDRV_ADS7846.h
Purpose     : Touch screen driver include
----------------------------------------------------------------------

Notes
=====
(1) The driver needs some function pointer to be filled correctly to be able
    to communicate with the external peripheral correctly. The correct assignment
    of these function pointers is checked during driver configuration.
    
(2) The driver needs some configuration variables filled in to be able to calculate
    the logical screen coordinates from the physical AD values.
    A description of the typical paramaters that have to be known by the driver is
    listed below:
    - Orientation: Orientation of the touch screen if not the same as the physical orientation.
                   A or-combination of the defines GUI_SWAP_XY, GUI_MIRROR_X and GUI_MIRROR_Y
                   can be used.
    - xLog0      : Logical pixel value of horizontal reference point 0. Typically 0.
    - xLog1      : Logical pixel value of horizontal reference point 1. Typically horizontal screen resolution -1.
    - xPhys0     : Physical AD value of horizontal reference point 0.
    - xPhys1     : Physical AD value of horizontal reference point 1.
    - yLog0      : Logical pixel value of vertical reference point 0. Typically 0.
    - yLog1      : Logical pixel value of vertical reference point 1. Typically vertical screen resolution -1.
    - yPhys0     : Physical AD value of vertical reference point 0.
    - yPhys1     : Physical AD value of vertical reference point 1.

(3) If the PENIRQ line of the touch controller is connected to a port of the target hardware
    a touch event can be detected by the driver. Upon polling the driver's exec routine the
    driver can check if a touch event is ready to be sampled by checking the PENIRQ line.
    Without PENIRQ line the driver will always try to sample a touch event even if no touch
    happened which will consume time even if not necessary.
    Without PENIRQ it is the responsibility of the user's pfGetResult() routine to return
    0xFFFF if the measured AD value is out of bounds.
    If both, the PENIRQ and the touch pressure recognition are enabled first the PENIRQ will
    signal that there is a touch event. Afterwards the touch pressure measurement is used to
    confirm that this was a valid touch and the touch had enough pressure to deliver good
    measurements.

(4) The touch pressure measurement is activated by setting a value for PressureMin and
    PressureMax .
    To calculate the correct touch pressure the resistance of the X-plate has to be known.
    This can be set via PlateResistanceX [Ohm].
    A touch event that we have measured with a pressure within these thresholds is confirmed
    as valid touch event with good measurment samples.
    If both, the PENIRQ and the touch pressure recognition are enabled first the PENIRQ will
    signal that there is a touch event. Afterwards the touch pressure measurement is used to
    confirm that this was a valid touch and the touch had enough pressure to deliver good
    measurements.
    If no PENIRQ line is connected this measurement takes place everytime the touch event is
    polled regardless if there is a touch event or not as the driver will only know for sure
    after calculating the touch pressure.
    Setting the value for PlateResistanceX to 0 is invalid. The driver will internally use a
    value of 1 instead.
*/

#ifndef GUITDRV_ADS7846_H            /* Make sure we only include it once */
#define GUITDRV_ADS7846_H

#include "GUI_Type.h"

#if defined(__cplusplus)
extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

/*********************************************************************
*
*       Types
*
**********************************************************************
*/

typedef struct {
  //
  // Function pointer (1)
  //
  void     (* pfSendCmd)   (U8 Data);     // Sends a 8-bit command to the peripheral
  U16      (* pfGetResult) (void);        // Retrieves the result of the AD conversion. 4 dummy bytes have to be shifted out to the left.
  char     (* pfGetBusy)   (void);        // Retrieves the status of the busy line. 0: Not busy; 1: Busy
  void     (* pfSetCS)     (char OnOff);  // Set chip select line. OnOff == 1 means peripheral selected
  //
  // Configuration (2)
  //
  unsigned Orientation;
  int      xLog0;
  int      xLog1;
  int      xPhys0;
  int      xPhys1;
  int      yLog0;
  int      yLog1;
  int      yPhys0;
  int      yPhys1;
  //
  // Optional, touch recognition via PENIRQ line (3)
  //
  char     (* pfGetPENIRQ) (void);        // Retrieves the status of the PENIRQ line to detect a touch event.
  //
  // Optional, touch recognition via touch pressure measurement (4)
  //
  int      PressureMin;                   // Minimum pressure threshold. A measured pressure below this value means we do not have a valid touch event.
  int      PressureMax;                   // Maximum pressure threshold. A measured pressure above this value means we do not have a valid touch event.
  int      PlateResistanceX;              // Resistance of the X-plate of the touch screen. This value is needed for calculation of the touch pressure.
} GUITDRV_ADS7846_CONFIG;

typedef struct {
  int      xPhys;     // Last measured x value
  int      yPhys;     // Last measured y value
  int      z1Phys;    // Last measured z1 value
  int      z2Phys;    // Last measured z2 value
  int      PENIRQ;    // Last sampled PENIRQ state if PENIRQ callback has been set
  int      Pressure;  // Last measured touch pressure if touch pressure measurement is enabled
} GUITDRV_ADS7846_LAST_VAL;

/*********************************************************************
*
*       Prototypes
*
**********************************************************************
*/

void GUITDRV_ADS7846_Config    (GUITDRV_ADS7846_CONFIG * pConfig);
char GUITDRV_ADS7846_Exec      (void);
void GUITDRV_ADS7846_GetLastVal(GUITDRV_ADS7846_LAST_VAL * p);

#if defined(__cplusplus)
}                /* Make sure we have C-declarations in C++ programs */
#endif


#endif   /* GUITDRV_ADS7846_H */

/*************************** End of file ****************************/
