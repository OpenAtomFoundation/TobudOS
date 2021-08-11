/*
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
    (C)2013 Semtech

Description: Helper functions implementation

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Miguel Luis and Gregory Cristian
*/

 /**
   ******************************************************************************
  *  
  *          Portions COPYRIGHT 2019 STMicroelectronics                         
  *
  * @file    utilities.h
  * @author  MCD Application Team
  * @brief   Header for driver utilities.c module
  ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __UTILITIES_H__
#define __UTILITIES_H__

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "utilities_conf.h"


/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Defines -------------------------------------------------------------------*/



#define TIMERTIME_T_MAX                             ( ( uint32_t )~0 )

/*!
 * @brief Returns the minimum value between a and b
 *
 * @param [IN] a 1st value
 * @param [IN] b 2nd value
 * @retval minValue Minimum value
 */
#ifndef MIN
  #define MIN( a, b ) ( ( ( a ) < ( b ) ) ? ( a ) : ( b ) )
#endif

/*!
 * @brief Returns the maximum value between a and b
 *
 * @param [IN] a 1st value
 * @param [IN] b 2nd value
 * @retval maxValue Maximum value
 */
#ifndef MAX
  #define MAX( a, b ) ( ( ( a ) > ( b ) ) ? ( a ) : ( b ) )
#endif

/*!
 * @brief Returns 2 raised to the power of n
 *
 * @param [IN] n power value
 * @retval result of raising 2 to the power n
 */
#define POW2( n ) ( 1 << n )



/*!
 * @brief Initializes the pseudo random generator initial value
 * @param [IN] seed Pseudo random generator initial value
 * @retval none
 */
void srand1( uint32_t seed );

/*!
 * @brief Computes a random number between min and max
 * @param [IN] min range minimum value
 * @param [IN] max range maximum value
 * @retval random random value in range min..max
 */
int32_t randr( int32_t min, int32_t max );

/*!
 * @brief  Computes a random number 
 * @param  none
 * @retval random value in range min..max
 */
int32_t rand1( void );

/*!
 * @brief Copies size elements of src array to dst array
 * @remark STM32 Standard memcpy function only works on pointers that are aligned
 * @param [OUT] dst  Destination array
 * @param [IN]  src  Source array
 * @param [IN]  size Number of bytes to be copied
 * @retval none
 */
void memcpy1( uint8_t *dst, const uint8_t *src, uint16_t size );

/*!
 * @brief Copies size elements of src array to dst array reversing the byte order
 * @param [OUT] dst  Destination array
 * @param [IN]  src  Source array
 * @param [IN]  size Number of bytes to be copied
 * @retval none
 */
void memcpyr( uint8_t *dst, const uint8_t *src, uint16_t size );

/*!
 * @brief Set size elements of dst array with value
 * @remark STM32 Standard memset function only works on pointers that are aligned
 * @param [OUT] dst   Destination array
 * @param [IN]  value Default value
 * @param [IN]  size  Number of bytes to be copied
 * @retval none
 */
void memset1( uint8_t *dst, uint8_t value, uint16_t size );

/*!
 * @brief Converts a nibble to an hexadecimal character
 * @param [IN] a   Nibble to be converted
 * @retval hexChar Converted hexadecimal character
 */
int8_t Nibble2HexChar( uint8_t a );

#endif // __UTILITIES_H__
