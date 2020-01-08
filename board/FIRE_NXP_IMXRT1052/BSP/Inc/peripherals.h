#ifndef _PERIPHERALS_H_
#define _PERIPHERALS_H_

#if defined(__cplusplus)
extern "C" {
#endif /*_cplusplus. */
       /*******************************************************************************
        * BOARD_InitBootPeripherals function
        ******************************************************************************/
void BOARD_InitBootPeripherals(void);

/*******************************************************************************
 * BOARD_InitConfigMPU function
 ******************************************************************************/
void BOARD_ConfigMPU(void);

#if defined(__cplusplus)
}
#endif /*_cplusplus. */

#endif /* _PERIPHERALS_H_ */
