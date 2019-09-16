/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*                        The Embedded Experts                        *
**********************************************************************
*                                                                    *
*       (c) 2014 - 2017  SEGGER Microcontroller GmbH & Co. KG        *
*                                                                    *
*       www.segger.com     Support: support@segger.com               *
*                                                                    *
**********************************************************************
*                                                                    *
*       SEGGER RTT * Real Time Transfer for embedded targets         *
*                                                                    *
**********************************************************************
*                                                                    *
* All rights reserved.                                               *
*                                                                    *
* SEGGER strongly recommends to not make any changes                 *
* to or modify the source code of this software in order to stay     *
* compatible with the RTT protocol and J-Link.                       *
*                                                                    *
* Redistribution and use in source and binary forms, with or         *
* without modification, are permitted provided that the following    *
* conditions are met:                                                *
*                                                                    *
* o Redistributions of source code must retain the above copyright   *
*   notice, this list of conditions and the following disclaimer.    *
*                                                                    *
* o Redistributions in binary form must reproduce the above          *
*   copyright notice, this list of conditions and the following      *
*   disclaimer in the documentation and/or other materials provided  *
*   with the distribution.                                           *
*                                                                    *
* o Neither the name of SEGGER Microcontroller GmbH & Co. KG         *
*   nor the names of its contributors may be used to endorse or      *
*   promote products derived from this software without specific     *
*   prior written permission.                                        *
*                                                                    *
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND             *
* CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,        *
* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF           *
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE           *
* DISCLAIMED. IN NO EVENT SHALL SEGGER Microcontroller BE LIABLE FOR *
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR           *
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT  *
* OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;    *
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF      *
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT          *
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE  *
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH   *
* DAMAGE.                                                            *
*                                                                    *
**********************************************************************
*                                                                    *
*       RTT version: 6.14d                                           *
*                                                                    *
**********************************************************************
---------------------------END-OF-HEADER------------------------------
File    : SEGGER_RTT_Conf.h
Purpose : Implementation of SEGGER real-time transfer (RTT) which
          allows real-time communication on targets which support
          debugger memory accesses while the CPU is running.
Revision: $Rev: 6302 $

*/

#ifndef SEGGER_RTT_CONF_H
#define SEGGER_RTT_CONF_H

#include "nordic_common.h"
#include "app_util_platform.h"
#include "sdk_config.h"

/*********************************************************************
*
*       Defines, configurable
*
**********************************************************************
*/

#define SEGGER_RTT_MAX_NUM_UP_BUFFERS       (SEGGER_RTT_CONFIG_MAX_NUM_UP_BUFFERS)      // Max. number of up-buffers (T->H) available on this target    (Default: 2)
#define SEGGER_RTT_MAX_NUM_DOWN_BUFFERS     (SEGGER_RTT_CONFIG_MAX_NUM_DOWN_BUFFERS)    // Max. number of down-buffers (H->T) available on this target  (Default: 2)

#define BUFFER_SIZE_UP                      (SEGGER_RTT_CONFIG_BUFFER_SIZE_UP)          // Size of the buffer for terminal output of target, up to host (Default: 1k)
#define BUFFER_SIZE_DOWN                    (SEGGER_RTT_CONFIG_BUFFER_SIZE_DOWN)        // Size of the buffer for terminal input to target from host (Usually keyboard input) (Default: 16)

#define SEGGER_RTT_PRINTF_BUFFER_SIZE       (64)                                        // Size of buffer for RTT printf to bulk-send chars via RTT     (Default: 64)

#define SEGGER_RTT_MODE_DEFAULT             SEGGER_RTT_CONFIG_DEFAULT_MODE              // Mode for preinitialized terminal channel (buffer 0)

#define SEGGER_RTT_LOCK()                   CRITICAL_REGION_ENTER()
#define SEGGER_RTT_UNLOCK()                 CRITICAL_REGION_EXIT()

#endif
/*************************** End of file ****************************/
