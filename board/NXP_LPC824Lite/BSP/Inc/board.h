#ifndef __BOARD_H__
#define __BOARD_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

/* Includes -----------------------------------------------------------------*/
#include <stdint.h>
#include "fsl_gpio.h"

/* Defines ------------------------------------------------------------------*/
#define BOARD_LED_CNT 8

#define	LED0				0
#define	LED1				1
#define	LED2				2
#define	LED3				3
#define	LED4				4
#define	LED5				5
#define	LED6				6
#define	LED7				7

/* Macros -------------------------------------------------------------------*/
/* Typedefs -----------------------------------------------------------------*/
/* Extern variables ---------------------------------------------------------*/
/* Functions API ------------------------------------------------------------*/
void LPC_Board_Init(void);

/**
 * @brief	Sets the state of a board LED to on or off
 * @param	LEDNumber	: LED number to set state for
 * @param	State		: true for on, false for off
 * @return	None
 */
void Board_LED_Set(uint8_t LEDNumber, bool State);

/**
 * @brief	Toggles the current state of a board LED
 * @param	LEDNumber	: LED number to change state for
 * @return	None
 */
void Board_LED_Toggle(uint8_t LEDNumber);




#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __BOARD_H__ */
