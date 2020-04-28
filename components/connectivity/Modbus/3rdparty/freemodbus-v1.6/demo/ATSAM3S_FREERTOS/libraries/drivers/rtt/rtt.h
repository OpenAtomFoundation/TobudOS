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
 * \par Purpose
 *
 * Interface for Real Time Timer (RTT) controller.
 *
 * \par Usage
 *
 * -# Changes the prescaler value of the given RTT and restarts it 
 *    using \ref RTT_SetPrescaler().
 * -# Get current value of the RTT using \ref RTT_GetTime().
 * -# Enables the specified RTT interrupt using \ref RTT_EnableIT().
 * -# Get the status register value of the given RTT using \ref RTT_GetStatus().
 * -# Configures the RTT to generate an alarm at the given time 
 *    using \ref RTT_SetAlarm().
 */

#ifndef RTT_H
#define RTT_H

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include <board.h>

/*----------------------------------------------------------------------------
 *        Definitions
 *----------------------------------------------------------------------------*/

#ifndef AT91C_BASE_RTTC
    #define AT91C_BASE_RTTC         RTT
#endif

/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/

extern void RTT_SetPrescaler(Rtt *rtt, uint16_t prescaler);

extern uint32_t RTT_GetTime(Rtt *rtt);

extern void RTT_EnableIT(Rtt *rtt, uint32_t sources);

extern uint32_t RTT_GetStatus(Rtt *rtt);

extern void RTT_SetAlarm(Rtt *pRtt, uint32_t time);

#endif /* #ifndef RTT_H */

