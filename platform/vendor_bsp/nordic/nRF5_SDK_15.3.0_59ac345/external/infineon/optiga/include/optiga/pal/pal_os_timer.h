/**
* MIT License
*
* Copyright (c) 2018 Infineon Technologies AG
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE
*
*
* \file
*
* \brief This file implements the prototype declarations of pal os timer functionalities.
*
* \addtogroup  grPAL
* @{
*/
#ifndef _PAL_OS_TIMER_H_
#define _PAL_OS_TIMER_H_

/**********************************************************************************************************************
 * HEADER FILES
 *********************************************************************************************************************/

#include "optiga/pal/pal.h"

/*********************************************************************************************************************
 * pal_os_timer.h
*********************************************************************************************************************/


/**********************************************************************************************************************
 * MACROS
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * ENUMS
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * DATA STRUCTURES
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * API Prototypes
 *********************************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Gets tick count value in milliseconds
 *
 * \retval  uint32_t time in milliseconds
 */
uint32_t pal_os_timer_get_time_in_milliseconds(void);

/**
 * @brief Waits or delay until the supplied milliseconds
 *
 * \param[in] milliseconds Delay value in milliseconds
 *
 */
void pal_os_timer_delay_in_milliseconds(uint16_t milliseconds);


#ifdef __cplusplus
}
#endif

#endif /* _PAL_OS_TIMER_H_ */

/**
* @}
*/

