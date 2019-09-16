/*-----------------------------------------------------------------------------
 * Name:    LED_FRDM-KL25Z.c
 * Purpose: LED interface for Freescale FRDM-KL25Z board
 * Rev.:    1.00
 *----------------------------------------------------------------------------*/

/* Copyright (c) 2015 ARM LIMITED

   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
   - Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   - Neither the name of ARM nor the names of its contributors may be used
     to endorse or promote products derived from this software without
     specific prior written permission.
   *
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
   ---------------------------------------------------------------------------*/

#include "MKL25Z4.h"                    // Device header
#include "Board_LED.h"                  // ::Board Support:LED

#define NUM_LEDS  (3)                   // Number of available LEDs

                          /*        red,      green,       blue */
const uint32_t led_pin[]  = {        18,         19,          1};
const uint32_t led_port[] = {PORTB_BASE, PORTB_BASE, PORTD_BASE};
const uint32_t led_gpio[] = {GPIOB_BASE, GPIOB_BASE, GPIOD_BASE};

/**
  \fn          int32_t LED_Initialize (void)
  \brief       Initialize LEDs
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t LED_Initialize (void) {
  uint32_t i;

  SIM->SCGC5 |= ((1U << SIM_SCGC5_PORTB_SHIFT) |
                 (1U << SIM_SCGC5_PORTD_SHIFT)  );

  /* Port Initialization */
  for (i = 0; i < NUM_LEDS; i++) {
    ((PORT_Type *)led_port[i])->PCR[led_pin[i]] = (1U << 8);
    ((PORT_Type *)led_port[i])->ISFR           &= (1U << led_pin[i]);
  }

  /* GPIO Initialization */
  for (i = 0; i < NUM_LEDS; i++) {
    ((GPIO_Type *)led_gpio[i])->PDOR           |=  (1U << led_pin[i]);
    ((GPIO_Type *)led_gpio[i])->PDDR           |=  (1U << led_pin[i]);
  }

//  LED_SetOut(0);

  return 0;
}

/**
  \fn          int32_t LED_Uninitialize (void)
  \brief       De-initialize LEDs
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t LED_Uninitialize (void) {

  return 0;
}

/**
  \fn          int32_t LED_On (uint32_t num)
  \brief       Turn on requested LED
  \param[in]   num  LED number
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t LED_On (uint32_t num) {

  if (num < NUM_LEDS) {
    ((GPIO_Type *)led_gpio[num])->PCOR  =  (1U << led_pin[num]);
  }

  return 0;
}

/**
  \fn          int32_t LED_Off (uint32_t num)
  \brief       Turn off requested LED
  \param[in]   num  LED number
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t LED_Off (uint32_t num) {

  if (num < NUM_LEDS) {
    ((GPIO_Type *)led_gpio[num])->PSOR  =  (1U << led_pin[num]);
  }

  return 0;
}

/**
  \fn          int32_t LED_SetOut (uint32_t val)
  \brief       Write value to LEDs
  \param[in]   val  value to be displayed on LEDs
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t LED_SetOut(uint32_t value) {
  int i;

  for (i = 0; i < NUM_LEDS; i++) {
    if (value & (1<<i)) {
      LED_On (i);
    } else {
      LED_Off(i);
    }
  }
  return 0;
}

/**
  \fn          uint32_t LED_GetCount (void)
  \brief       Get number of LEDs
  \return      Number of available LEDs
*/
uint32_t LED_GetCount (void) {
  return NUM_LEDS;
}

