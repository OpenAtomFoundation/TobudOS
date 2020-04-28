/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support
 * ----------------------------------------------------------------------------
 * Copyright (c) 2009, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Atmel's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ----------------------------------------------------------------------------
 */

/**
 * \file
 *
 * Implementation of ADS7843 driver.
 */

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include <stdint.h>
#include <board.h>
#include <string.h>

#ifdef BOARD_TSC_ADS7843

#include "tsd.h"
#include "tsd_com.h"
#include <pio/pio.h>
#include <pio/pio_it.h>
#include <ads7843/ads7843.h>
#include <lcd/lcdd.h>
#include <lcd/draw.h>
#include <lcd/font.h>
#include "tsd/tsd_ads7843.h"

/*----------------------------------------------------------------------------
 *        Definitions
 *----------------------------------------------------------------------------*/

/** Delay for pushbutton debouncing (the time-base is 10 ms). */
#define DEBOUNCE_TIME       6               /* 10 * 6 = 60 ms */

/** Color of calibration points. */
#define POINTS_COLOR        0x0000FF

/** Size in pixels of calibration points. */
#define POINTS_SIZE         4

/** Maximum difference in pixels between the test point and the measured point. */
#define POINTS_MAX_ERROR    5

/*----------------------------------------------------------------------------
 *        Types
 *----------------------------------------------------------------------------*/

/** pen state */
typedef enum {
    STATE_PEN_RELEASED  = 0,
    STATE_PEN_PRESSED   = 1,
    STATE_PEN_DEBOUNCE  = 2
} e_pen_state;

/*----------------------------------------------------------------------------
 *        Local variables
 *----------------------------------------------------------------------------*/

/** Pins used by Interrupt Signal for Touch Screen Controller */
static const Pin pinPenIRQ  = PIN_TCS_IRQ;

/** Global timestamp in milliseconds since start of application. */
static volatile uint32_t timestamp = 0;

/** last time when the pen is pressed on the touchscreen */
static volatile uint32_t timePress = 0;

/** last time when the pen is released */
static volatile uint32_t timeRelease = 0;

/** pen state */
static volatile e_pen_state penState = STATE_PEN_RELEASED;

/** Touch screen initiallized flag */
static uint32_t tsInitFlag = 0;

/*----------------------------------------------------------------------------
 *        Local functions
 *----------------------------------------------------------------------------*/

/**
 * \brief Timer handler for touch screen. Increments the timestamp counter.
 * Determine the state "Pen Pressed" or "Pen Released". To change state,
 * the penIRQ has to keep the same value during DEBOUNCE_TIME.
 *
 * \note External timer interrupt should call it per 10ms.
 */
extern void TSD_TimerHandler( void )
{
    uint32_t data[2];
    static uint32_t point[2];

    if (!tsInitFlag) return;

    timestamp++;
    /* Get the current position of the pen if penIRQ has low value (pen pressed) */
    if (PIO_Get(&pinPenIRQ) == 0) {
        /* Get the current position of the pressed pen */
        if(TSDCom_IsCalibrationOk()) {
            TSD_GetRawMeasurement(data);
            TSDCom_InterpolateMeasurement(data, point);
        }

        /* call the callback function */
        if(penState == STATE_PEN_PRESSED) {
            if(TSDCom_IsCalibrationOk()) {
                TSD_PenMoved(point[0], point[1]);
            }
        }
    }

    /* Determine the pen state */
    if (PIO_Get(&pinPenIRQ) == 0) {

        /* reinit the last time when release */
        timeRelease = timestamp;
        if(penState == STATE_PEN_DEBOUNCE) {
            if( (timestamp - timePress) > DEBOUNCE_TIME) {
                /* pen is pressed during an enough time : the state change */
                penState = STATE_PEN_PRESSED;
                /* call the callback function */
                if(TSDCom_IsCalibrationOk()) {
                    TSD_PenPressed(point[0], point[1]);
                }
            }
        }
    }
    else {
        /* reinit the last time when release */
        timePress = timestamp;

        if(penState == STATE_PEN_DEBOUNCE) {
            if( (timestamp - timeRelease) > DEBOUNCE_TIME) {
                /* pen is released during an enough time : the state change */
                penState = STATE_PEN_RELEASED;
                /* call the callback function */
                if(TSDCom_IsCalibrationOk()) {
                    TSD_PenReleased(point[0], point[1]);
                }
            }
        }
    }
}

/**
 * \breif Interrupt handler for Touchscreen.
 */
static void ISR_PenIRQ(void)
{
    /* Check if the pen has been pressed */
    if (!PIO_Get(&pinPenIRQ)) {
        if(penState == STATE_PEN_RELEASED) {

            timePress = timestamp;
            penState = STATE_PEN_DEBOUNCE;
        }
    }
    else {
        if(penState == STATE_PEN_PRESSED) {

            timeRelease = timestamp;
            penState = STATE_PEN_DEBOUNCE;
        }
    }
}

/**
 * \brief Configure PENIRQ for interrupt.
 */
static void ConfigurePenIRQ(void)
{
    /* Configure pios */
    PIO_Configure(&pinPenIRQ, PIO_LISTSIZE(pinPenIRQ));

    /* Initialize interrupts */
    PIO_InitializeInterrupts(0);
    PIO_ConfigureIt(&pinPenIRQ, (void (*)(const Pin *)) ISR_PenIRQ);

    /* Enable the interrupt */
    PIO_EnableIt(&pinPenIRQ);
}

/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/

/**
 * \brief Reads and store a touchscreen measurement in the provided array.
 *
 * \param pData  Array where the measurements will be stored
 */
extern void TSD_GetRawMeasurement(uint32_t *pData)
{
    /* Get the current position of the pressed pen */
    PIO_DisableIt(&pinPenIRQ);
    ADS7843_GetPosition(&pData[0], &pData[1]);
    PIO_EnableIt(&pinPenIRQ);
}

/**
 * \brief Wait pen pressed.
 */
extern void TSD_WaitPenPressed( void )
{
    /* Wait for touch & end of conversion */
    while (penState != STATE_PEN_RELEASED);
    /*while (penState != STATE_PEN_PRESSED); */
    while (penState != STATE_PEN_PRESSED) {
    }
}

/**
 * \brief Wait pen released.
 */
extern void TSD_WaitPenReleased( void )
{
    /* Wait for contact loss */
    while (penState != STATE_PEN_PRESSED);
    while (penState != STATE_PEN_RELEASED);
}

/**
 * \brief Do calibration.
 *
 * \return 1 if calibration is Ok, 0 else.
 */
uint8_t TSD_Calibrate(void)
{
    uint8_t ret = 0;

    /* Calibration is done only once */
    if(TSDCom_IsCalibrationOk()) {
        return 1;
    }

    /* Do calibration */
    ret = TSDCom_Calibrate();

    return ret;
}

/**
 * \brief Initializes the touchscreen driver and starts the calibration process. When
 * finished, the touchscreen is operational.
 *
 * \note Important: the LCD driver must have been initialized prior to calling this
 * function.
 */
extern void TSD_Initialize( int8_t calEn )
{
    ADS7843_Initialize();
    ConfigurePenIRQ();

    tsInitFlag = 1;

    /* Calibration */
    if(calEn) {
       while (!TSD_Calibrate());
    }
}

/**
 * \brief Stop the Touchscreen, disable interrupt.
 */
void TSD_Reset(void)
{
    /* Disable SPI 0 */
    ADS7843_Reset();

    /* Disable the interrupt */
    PIO_DisableIt(&pinPenIRQ);
}

#endif /* #ifdef BOARD_TSC_ADS7843 */
