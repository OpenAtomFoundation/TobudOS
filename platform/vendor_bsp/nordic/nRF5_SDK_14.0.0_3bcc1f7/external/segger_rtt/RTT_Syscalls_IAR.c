/*********************************************************************
*               SEGGER MICROCONTROLLER GmbH & Co. KG                 *
*       Solutions for real time microcontroller applications         *
**********************************************************************
*                                                                    *
*       (c) 2014 - 2015  SEGGER Microcontroller GmbH & Co. KG        *
*                                                                    *
*       www.segger.com     Support: support@segger.com               *
*                                                                    *
**********************************************************************
*                                                                    *
* All rights reserved.                                               *
*                                                                    *
* * This software may in its unmodified form be freely redistributed *
*   in source form.                                                  *
* * The source code may be modified, provided the source code        *
*   retains the above copyright notice, this list of conditions and  *
*   the following disclaimer.                                        *
* * Modified versions of this software in source or linkable form    *
*   may not be distributed without prior consent of SEGGER.          *
* * This software may only be used for communication with SEGGER     *
*   J-Link debug probes.                                             *
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
--------  END-OF-HEADER  ---------------------------------------------
File    : SEGGER_RTT_Syscalls_IAR.c
Purpose : Low-level functions for using printf() via RTT in IAR.
          To use RTT for printf output, include this file in your
          application and set the Library Configuration to Normal.
----------------------------------------------------------------------
*/
#if defined(NRF_LOG_USES_RTT) && NRF_LOG_USES_RTT == 1
#include <yfuns.h>
#include "SEGGER_RTT.h"
#pragma module_name = "?__write"

/*********************************************************************
*
*       Function prototypes
*
**********************************************************************
*/
size_t __write(int handle, const unsigned char * buffer, size_t size);

/*********************************************************************
*
*       Global functions
*
**********************************************************************
*/
/*********************************************************************
*
*       __write()
*
* Function description
*   Low-level write function.
*   Standard library subroutines will use this system routine
*   for output to all files, including stdout.
*   Write data via RTT.
*/
size_t __write(int handle, const unsigned char * buffer, size_t size) {
  (void) handle;  /* Not used, avoid warning */
  SEGGER_RTT_Write(0, (const char*)buffer, size);
  return size;
}

/*********************************************************************
*
*       __write_buffered()
*
* Function description
*   Low-level write function.
*   Standard library subroutines will use this system routine
*   for output to all files, including stdout.
*   Write data via RTT.
*/
size_t __write_buffered(int handle, const unsigned char * buffer, size_t size) {
  (void) handle;  /* Not used, avoid warning */
  SEGGER_RTT_Write(0, (const char*)buffer, size);
  return size;
}

#endif /*NRF_LOG_USES_RTT==1*/
/****** End Of File *************************************************/
