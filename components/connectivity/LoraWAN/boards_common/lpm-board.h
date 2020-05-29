/*!
 * \file      lpm-board.h
 *
 * \brief     Target board low power modes management
 *
 * \copyright Revised BSD License, see section \ref LICENSE.
 *
 * \code
 *                ______                              _
 *               / _____)             _              | |
 *              ( (____  _____ ____ _| |_ _____  ____| |__
 *               \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 *               _____) ) ____| | | || |_| ____( (___| | | |
 *              (______/|_____)_|_|_| \__)_____)\____)_| |_|
 *              (C)2013-2017 Semtech - STMicroelectronics
 *
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 *
 * \author    Gregory Cristian ( Semtech )
 *
 * \author    MCD Application Team (C)( STMicroelectronics International )
 */
#ifndef __LPM_BOARD_H__
#define __LPM_BOARD_H__


#ifdef __cplusplus
extern "C"
{
#endif

#include "board-config.h"

/*!
 * Low power manager configuration
 */
typedef enum
{
    LPM_APPLI_ID   =                                ( 1 << 0 ),
    LPM_LIB_ID     =                                ( 1 << 1 ),
    LPM_RTC_ID     =                                ( 1 << 2 ),
    LPM_GPS_ID     =                                ( 1 << 3 ),
    LPM_UART_RX_ID =                                ( 1 << 4 ),
    LPM_UART_TX_ID =                                ( 1 << 5 ),
} LpmId_t;

/*!
 * Low Power Mode selected
 */
typedef enum
{
    LPM_ENABLE = 0,
    LPM_DISABLE,
} LpmSetMode_t;

typedef enum
{
    LPM_SLEEP_MODE,
    LPM_STOP_MODE,
    LPM_OFF_MODE,
} LpmGetMode_t;

/*!
 * \brief  This API returns the Low Power Mode selected that will be applied when the system will enter low power mode
 *         if there is no update between the time the mode is read with this API and the time the system enters
 *         low power mode.
 *
 * \retval mode Selected low power mode
 */
LpmGetMode_t LpmGetMode( void );

/*!
 * \brief  This API notifies the low power manager if the specified user allows the Stop mode or not.
 *         When the application does not require the system clock, it enters Stop Mode if at least one user disallow
 *         Off Mode. Otherwise, it enters Off Mode.
 *         The default mode selection for all users is Off mode enabled
 *
 * \param [IN] id   Process Id
 * \param [IN] mode Selected mode
 */
void LpmSetStopMode( LpmId_t id, LpmSetMode_t mode );

/*!
 * \brief  This API notifies the low power manager if the specified user allows the Off mode or not.
 *         When the application does not require the system clock, it enters Stop Mode if at least one user disallow
 *         Off Mode. Otherwise, it enters Off Mode.
 *         The default mode selection for all users is Off mode enabled
 *
 * \param [IN] id   Process Id
 * \param [IN] mode Selected mode
 */
void LpmSetOffMode(LpmId_t id, LpmSetMode_t mode );

/*!
 * \brief  This API shall be used by the application when there is no more code to execute so that the system may
 *         enter low-power mode. The mode selected depends on the information received from LpmOffModeSelection( ) and
 *         LpmSysclockRequest( )
 *         This function shall be called in critical section
 */
void LpmEnterLowPower( void );

/*!
 * \brief  This API is called by the low power manager in a critical section (PRIMASK bit set) to allow the
 *         application to implement dedicated code before entering Sleep Mode
 */
void LpmEnterSleepMode( void );

/*!
 * \brief  This API is called by the low power manager in a critical section (PRIMASK bit set) to allow the
 *         application to implement dedicated code before getting out from Sleep Mode
 */
void LpmExitSleepMode( void );

/*!
 * \brief  This API is called by the low power manager in a critical section (PRIMASK bit set) to allow the
 *         application to implement dedicated code before entering Stop Mode
 */
void LpmEnterStopMode( void );

/*!
 * \brief  This API is called by the low power manager in a critical section (PRIMASK bit set) to allow the
 *         application to implement dedicated code before getting out from Stop Mode. This is where the application
 *         should reconfigure the clock tree when needed
 */
void LpmExitStopMode( void );

/*!
 * \brief  This API is called by the low power manager in a critical section (PRIMASK bit set) to allow the
 *         application to implement dedicated code before entering Off mode. This is where the application could save
 *         data in the retention memory as the RAM memory content will be lost
 */
void LpmEnterOffMode( void );

/*!
 * @brief  This API is called by the low power manager in a critical section (PRIMASK bit set) to allow the
 *         application to implement dedicated code before getting out from Off mode. This can only happen when the
 *         Off mode is finally not entered. In that case, the application may reverse some configurations done before
 *         entering Off mode. When Off mode is successful, the system is reset when getting out from this low-power mode
 */
void LpmExitOffMode( void );

#ifdef __cplusplus
}
#endif

#endif /*__LPM_BOARD_H__ */
