/*!
 * \file      LmHandler.h
 *
 * \brief     Implements the LoRaMac layer handling. 
 *            Provides the possibility to register applicative packages.
 *
 * \remark    Inspired by the examples provided on the en.i-cube_lrwan fork.
 *            MCD Application Team ( STMicroelectronics International )
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
 *              (C)2013-2018 Semtech
 *
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 */
#ifndef __LORAMAC_HANDLER_H__
#define __LORAMAC_HANDLER_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "LmHandlerTypes.h"
#include "LmhpCompliance.h"


typedef struct LmHandlerJoinParams_s
{
    CommissioningParams_t *CommissioningParams;
    int8_t Datarate;
    LmHandlerErrorStatus_t Status;
}LmHandlerJoinParams_t;

typedef struct LmHandlerTxParams_s
{
    uint8_t IsMcpsConfirm;
    LoRaMacEventInfoStatus_t Status;
    CommissioningParams_t *CommissioningParams;
    LmHandlerMsgTypes_t MsgType;
    uint8_t AckReceived;
    int8_t Datarate;
    uint32_t UplinkCounter;
    LmHandlerAppData_t AppData;
    int8_t TxPower;
    uint8_t Channel;
}LmHandlerTxParams_t;

typedef struct LmHandlerRxParams_s
{
    uint8_t IsMcpsIndication;
    LoRaMacEventInfoStatus_t Status;
    CommissioningParams_t *CommissioningParams;
    int8_t Datarate;
    int8_t Rssi;
    int8_t Snr;
    uint32_t DownlinkCounter;
    int8_t RxSlot;
}LmHandlerRxParams_t;

typedef struct LoRaMAcHandlerBeaconParams_s
{
    LoRaMacEventInfoStatus_t Status;
    LmHandlerBeaconState_t State;
    BeaconInfo_t Info;
}LoRaMAcHandlerBeaconParams_t;

typedef struct LmHandlerParams_s
{
    /*!
     * Region
     */
    LoRaMacRegion_t Region;
    /*!
     * Holds the ADR state
     */
    bool AdrEnable;
    /*!
     * Uplink datarate, when \ref AdrEnable is OFF
     */
    int8_t TxDatarate;
    /*!
     * Enables/Disables a public network usage
     */
    bool PublicNetworkEnable;
    /*!
    * LoRaWAN ETSI duty cycle control enable/disable
    *
    * \remark Please note that ETSI mandates duty cycled transmissions. Use only for test purposes
    */
    bool DutyCycleEnabled;
    /*!
     * Application data buffer maximum size
     */
    uint8_t DataBufferMaxSize;
    /*!
     * Application data buffer pointer
     */
    uint8_t *DataBuffer;
}LmHandlerParams_t;

typedef struct LmHandlerCallbacks_s
{
    /*!
     * Get the current battery level
     *
     * \retval value  Battery level ( 0: very low, 254: fully charged )
     */
    uint8_t ( *GetBatteryLevel )( void );
    /*!
     * Get the current temperature
     *
     * \retval value  Temperature in degree Celsius
     */
    float ( *GetTemperature )( void );
    /*!
     * Returns a pseudo random seed generated using the MCU Unique ID
     *
     * \retval seed Generated pseudo random seed
     */
    uint32_t ( *GetRandomSeed )( void );
    /*!
     *\brief    Will be called each time a Radio IRQ is handled by the MAC
     *          layer.
     * 
     *\warning  Runs in a IRQ context. Should only change variables state.
     */
    void ( *OnMacProcess )( void );
    /*!
     * Notifies the upper layer that the NVM context has changed
     *
     * \param [IN] stored Indicates if we are storing (true) or
     *                    restoring (false) the NVM context
     */
    void ( *OnNvmContextChange )( LmHandlerNvmContextStates_t state );
    /*!
     * Notifies the upper layer that a network parameters have been set
     *
     * \param [IN] params notification parameters
     */
    void ( *OnNetworkParametersChange )( CommissioningParams_t *params );
    /*!
     * Notifies the upper layer that a MCPS request has been made to the MAC layer
     *
     * \param   [IN] status      - Request returned status
     * \param   [IN] mcpsRequest - Performed MCPS-Request. Refer to \ref McpsReq_t.
     * \param   [IN] nextTxDelay - Time to wait until another TX is possible.
     */
    void ( *OnMacMcpsRequest )( LoRaMacStatus_t status, McpsReq_t *mcpsReq, TimerTime_t nextTxDelay );
    /*!
     * Notifies the upper layer that a MLME request has been made to the MAC layer
     *
     * \param   [IN] status      - Request returned status
     * \param   [IN] mlmeRequest - Performed MLME-Request. Refer to \ref MlmeReq_t.
     * \param   [IN] nextTxDelay - Time to wait until another TX is possible.
     */
    void ( *OnMacMlmeRequest )( LoRaMacStatus_t status, MlmeReq_t *mlmeReq, TimerTime_t nextTxDelay );
    /*!
     * Notifies the upper layer that a network has been joined
     *
     * \param [IN] params notification parameters
     */
    void ( *OnJoinRequest )( LmHandlerJoinParams_t *params );
    /*!
     * Notifies upper layer that a frame has been transmitted
     *
     * \param [IN] params notification parameters
     */
    void ( *OnTxData )( LmHandlerTxParams_t *params );
    /*!
     * Notifies the upper layer that an applicative frame has been received
     *
     * \param [IN] appData Received applicative data
     * \param [IN] params notification parameters
     */
    void ( *OnRxData )( LmHandlerAppData_t *appData, LmHandlerRxParams_t *params );
    /*!
     * Confirms the LoRaWAN device class change
     *
     * \param [IN] deviceClass New end-device class
     */
    void ( *OnClassChange )( DeviceClass_t deviceClass );
    /*!
     * Notifies the upper layer that the beacon status has changed
     *
     * \param [IN] params notification parameters
     */
    void ( *OnBeaconStatusChange )( LoRaMAcHandlerBeaconParams_t *params );
#if( LMH_SYS_TIME_UPDATE_NEW_API == 1 )
    /*!
     * Notifies the upper layer that the system time has been updated.
     *
     * \param [in] isSynchronized Indicates if the system time is synchronized in the range +/-1 second
     * \param [in] timeCorrection Received time correction value
     */
    void ( *OnSysTimeUpdate )( bool isSynchronized, int32_t timeCorrection );
#else
    /*!
     * Notifies the upper layer that the system time has been updated.
     */
    void ( *OnSysTimeUpdate )( void );
#endif
}LmHandlerCallbacks_t;

/*!
 * LoRaMac handler initialisation
 *
 * \param [IN] callbacks     LoRaMac handler callbacks
 * \param [IN] handlerParams LoRaMac handler parameters
 *
 * \retval none
 */
LmHandlerErrorStatus_t LmHandlerInit( LmHandlerCallbacks_t *callbacks,
                                      LmHandlerParams_t *handlerParams );

/*!
 * Indicates if the LoRaMacHandler is busy
 * 
 * \retval status [true] Busy, [false] free
 */
bool LmHandlerIsBusy( void );

/*!
 * Processes the LoRaMac and Radio events. 
 * When no pendig operation asks to go in low power mode.
 *
 * \remark This function must be called in the main loop.
 */
void LmHandlerProcess( void );

/*!
 * Instructs the MAC layer to send a ClassA uplink
 *
 * \param [IN] appData Data to be sent
 * \param [IN] isTxConfirmed Indicates if the uplink requires an acknowledgement
 *
 * \retval status Returns \ref LORAMAC_HANDLER_SUCCESS if request has been
 *                processed else \ref LORAMAC_HANDLER_ERROR
 */
LmHandlerErrorStatus_t LmHandlerSend( LmHandlerAppData_t *appData, LmHandlerMsgTypes_t isTxConfirmed );

/*!
 * Join a LoRa Network in classA
 *
 * \Note if the device is ABP, this is a pass through function
 */
void LmHandlerJoin( void );

/*!
 * Check whether the Device is joined to the network
 *
 * \param [IN] none
 *
 * \retval status Returns \ref LORAMAC_HANDLER_SET if joined else \ref LORAMAC_HANDLER_RESET
 */
LmHandlerFlagStatus_t LmHandlerJoinStatus( void );

/*!
 * Informs the server on the ping-slot periodicity to use
 *
 * \param [IN] periodicity Is equal to 2^periodicity seconds.
 *                         Example: 2^3 = 8 seconds. The end-device will open an Rx slot every 8 seconds.
 *
 * \retval status Returns \ref LORAMAC_HANDLER_SUCCESS if request has been
 *                processed else \ref LORAMAC_HANDLER_ERROR
 */
LmHandlerErrorStatus_t LmHandlerPingSlotReq( uint8_t periodicity );

/*!
 * Request the MAC layer to change LoRaWAN class
 *
 * \Note Callback \ref LmHandlerConfirmClass informs upper layer that the change has occurred
 * \Note Only switch from class A to class B/C OR from class B/C to class A is allowed
 *
 * \param [IN] newClass New class to be requested
 *
 * \retval status Returns \ref LORAMAC_HANDLER_SUCCESS if request has been
 *                processed else \ref LORAMAC_HANDLER_ERROR
 */
LmHandlerErrorStatus_t LmHandlerRequestClass( DeviceClass_t newClass );

/*!
 * Gets the current LoRaWAN class
 *
 * \retval currentClass Current LoRaWAN class
 */
DeviceClass_t LmHandlerGetCurrentClass( void );

/*!
 * Gets the current datarate
 *
 * \retval currentDatarate Current datarate
 */
int8_t LmHandlerGetCurrentDatarate( void );

/*!
 * Gets the current active region
 *
 * \retval currentRegion Current active region
 */
LoRaMacRegion_t LmHandlerGetActiveRegion( void );

/*!
 * Set system maximum tolerated rx error in milliseconds
 *
 * \param [IN] maxErrorInMs Maximum tolerated error in milliseconds
 *
 * \retval status Returns \ref LORAMAC_HANDLER_SUCCESS if request has been
 *                processed else \ref LORAMAC_HANDLER_ERROR
 */
LmHandlerErrorStatus_t LmHandlerSetSystemMaxRxError( uint32_t maxErrorInMs );

/*
 *=============================================================================
 * PACKAGES HANDLING
 *=============================================================================
 */
LmHandlerErrorStatus_t LmHandlerPackageRegister( uint8_t id, void *params );
bool LmHandlerPackageIsInitialized( uint8_t id );
bool LmHandlerPackageIsRunning( uint8_t id );

#ifdef __cplusplus
}
#endif

#endif // __LORAMAC_HANDLER_H__
