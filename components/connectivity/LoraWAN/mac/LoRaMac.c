/*!
 * \file      LoRaMac.c
 *
 * \brief     LoRa MAC layer implementation
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
 *              (C)2013-2017 Semtech
 *
 *               ___ _____ _   ___ _  _____ ___  ___  ___ ___
 *              / __|_   _/_\ / __| |/ / __/ _ \| _ \/ __| __|
 *              \__ \ | |/ _ \ (__| ' <| _| (_) |   / (__| _|
 *              |___/ |_/_/ \_\___|_|\_\_| \___/|_|_\\___|___|
 *              embedded.connectivity.solutions===============
 *
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 *
 * \author    Gregory Cristian ( Semtech )
 *
 * \author    Daniel Jaeckle ( STACKFORCE )
 *
 * \author    Johannes Bruder ( STACKFORCE )
 */
#include "utilities.h"
#include "region/Region.h"
#include "LoRaMacClassB.h"
#include "LoRaMacCrypto.h"
#include "secure-element.h"
#include "LoRaMacTest.h"
#include "LoRaMacTypes.h"
#include "LoRaMacConfirmQueue.h"
#include "LoRaMacHeaderTypes.h"
#include "LoRaMacMessageTypes.h"
#include "LoRaMacParser.h"
#include "LoRaMacCommands.h"
#include "LoRaMacAdr.h"
#include "LoRaMacSerializer.h"

#include "LoRaMac.h"

#ifndef LORAMAC_VERSION
/*!
 * LORaWAN version definition.
 */
#define LORAMAC_VERSION                             0x01000300
#endif

/*!
 * Maximum PHY layer payload size
 */
#define LORAMAC_PHY_MAXPAYLOAD                      255

/*!
 * Maximum MAC commands buffer size
 */
#define LORA_MAC_COMMAND_MAX_LENGTH                 128

/*!
 * Maximum length of the fOpts field
 */
#define LORA_MAC_COMMAND_MAX_FOPTS_LENGTH           15

/*!
 * LoRaMac duty cycle for the back-off procedure during the first hour.
 */
#define BACKOFF_DC_1_HOUR                           100

/*!
 * LoRaMac duty cycle for the back-off procedure during the next 10 hours.
 */
#define BACKOFF_DC_10_HOURS                         1000

/*!
 * LoRaMac duty cycle for the back-off procedure during the next 24 hours.
 */
#define BACKOFF_DC_24_HOURS                         10000

/*!
 * LoRaMac internal states
 */
enum eLoRaMacState
{
    LORAMAC_IDLE          = 0x00000000,
    LORAMAC_STOPPED       = 0x00000001,
    LORAMAC_TX_RUNNING    = 0x00000002,
    LORAMAC_RX            = 0x00000004,
    LORAMAC_ACK_RETRY     = 0x00000010,
    LORAMAC_TX_DELAYED    = 0x00000020,
    LORAMAC_TX_CONFIG     = 0x00000040,
    LORAMAC_RX_ABORT      = 0x00000080,
};

/*
 * Request permission state
 */
typedef enum eLoRaMacRequestHandling
{
    LORAMAC_REQUEST_HANDLING_OFF = 0,
    LORAMAC_REQUEST_HANDLING_ON = !LORAMAC_REQUEST_HANDLING_OFF
}LoRaMacRequestHandling_t;

typedef struct sLoRaMacNvmCtx
{
    /*
     * LoRaMac region.
     */
    LoRaMacRegion_t Region;
    /*
     * LoRaMac default parameters
     */
    LoRaMacParams_t MacParamsDefaults;
    /*
     * Network ID ( 3 bytes )
     */
    uint32_t NetID;
    /*
     * Mote Address
     */
    uint32_t DevAddr;
    /*!
    * Multicast channel list
    */
    MulticastCtx_t MulticastChannelList[LORAMAC_MAX_MC_CTX];
    /*
     * Actual device class
     */
    DeviceClass_t DeviceClass;
    /*
     * Indicates if the node is connected to
     * a private or public network
     */
    bool PublicNetwork;
    /*
     * LoRaMac ADR control status
     */
    bool AdrCtrlOn;
    /*
     * Counts the number of missed ADR acknowledgements
     */
    uint32_t AdrAckCounter;

    /*
     * LoRaMac parameters
     */
    LoRaMacParams_t MacParams;
    /*
     * Maximum duty cycle
     * \remark Possibility to shutdown the device.
     */
    uint8_t MaxDCycle;
    /*
    * Enables/Disables duty cycle management (Test only)
    */
    bool DutyCycleOn;
    /*
     * Buffer containing the MAC layer commands
     */
    uint8_t MacCommandsBuffer[LORA_MAC_COMMAND_MAX_LENGTH];
    /*
     * If the server has sent a FRAME_TYPE_DATA_CONFIRMED_DOWN this variable indicates
     * if the ACK bit must be set for the next transmission
     */
    bool SrvAckRequested;
    /*
     * Aggregated duty cycle management
     */
    uint16_t AggregatedDCycle;
    /*
    * Aggregated duty cycle management
    */
    TimerTime_t LastTxDoneTime;
    TimerTime_t AggregatedTimeOff;
    /*
    * Stores the time at LoRaMac initialization.
    *
    * \remark Used for the BACKOFF_DC computation.
    */
    SysTime_t InitializationTime;
    /*
     * Current LoRaWAN Version
     */
    Version_t Version;
    /*
     * End-Device network activation
     */
    ActivationType_t NetworkActivation;
    /*!
     * Last received Message integrity Code (MIC)
     */
    uint32_t LastRxMic;
}LoRaMacNvmCtx_t;

typedef struct sLoRaMacCtx
{
    /*
    * Length of packet in PktBuffer
    */
    uint16_t PktBufferLen;
    /*
    * Buffer containing the data to be sent or received.
    */
    uint8_t PktBuffer[LORAMAC_PHY_MAXPAYLOAD];
    /*!
    * Current processed transmit message
    */
    LoRaMacMessage_t TxMsg;
    /*!
    * Buffer containing the data received by the application.
    */
    uint8_t AppData[LORAMAC_PHY_MAXPAYLOAD];
    /*
    * Size of buffer containing the application data.
    */
    uint8_t AppDataSize;
    /*
    * Buffer containing the upper layer data.
    */
    uint8_t RxPayload[LORAMAC_PHY_MAXPAYLOAD];
    SysTime_t LastTxSysTime;
    /*
    * LoRaMac internal state
    */
    uint32_t MacState;
    /*
    * LoRaMac upper layer event functions
    */
    LoRaMacPrimitives_t* MacPrimitives;
    /*
    * LoRaMac upper layer callback functions
    */
    LoRaMacCallback_t* MacCallbacks;
    /*
    * Radio events function pointer
    */
    RadioEvents_t RadioEvents;
    /*
    * LoRaMac duty cycle delayed Tx timer
    */
    TimerEvent_t TxDelayedTimer;
    /*
    * LoRaMac reception windows timers
    */
    TimerEvent_t RxWindowTimer1;
    TimerEvent_t RxWindowTimer2;
    /*
    * LoRaMac reception windows delay
    * \remark normal frame: RxWindowXDelay = ReceiveDelayX - RADIO_WAKEUP_TIME
    *         join frame  : RxWindowXDelay = JoinAcceptDelayX - RADIO_WAKEUP_TIME
    */
    uint32_t RxWindow1Delay;
    uint32_t RxWindow2Delay;
    /*
    * LoRaMac Rx windows configuration
    */
    RxConfigParams_t RxWindow1Config;
    RxConfigParams_t RxWindow2Config;
    RxConfigParams_t RxWindowCConfig;
    /*
     * Limit of uplinks without any donwlink response before the ADRACKReq bit will be set.
     */
    uint16_t AdrAckLimit;
    /*
     * Limit of uplinks without any donwlink response after a the first frame with set ADRACKReq bit
     * before the trying to regain the connectivity.
     */
    uint16_t AdrAckDelay;
    /*
    * Acknowledge timeout timer. Used for packet retransmissions.
    */
    TimerEvent_t AckTimeoutTimer;
    /*
     * Uplink messages repetitions counter
     */
    uint8_t ChannelsNbTransCounter;
    /*
     * Number of trials to get a frame acknowledged
     */
    uint8_t AckTimeoutRetries;
    /*
     * Number of trials to get a frame acknowledged
     */
    uint8_t AckTimeoutRetriesCounter;
    /*
     * Indicates if the AckTimeout timer has expired or not
     */
    bool AckTimeoutRetry;
    /*
     * If the node has sent a FRAME_TYPE_DATA_CONFIRMED_UP this variable indicates
     * if the nodes needs to manage the server acknowledgement.
     */
    bool NodeAckRequested;
    /*
     * Current channel index
     */
    uint8_t Channel;
    /*
    * Last transmission time on air
    */
    TimerTime_t TxTimeOnAir;
    /*
    * Structure to hold an MCPS indication data.
    */
    McpsIndication_t McpsIndication;
    /*
    * Structure to hold MCPS confirm data.
    */
    McpsConfirm_t McpsConfirm;
    /*
    * Structure to hold MLME confirm data.
    */
    MlmeConfirm_t MlmeConfirm;
    /*
    * Structure to hold MLME indication data.
    */
    MlmeIndication_t MlmeIndication;
    /*
    * Holds the current rx window slot
    */
    LoRaMacRxSlot_t RxSlot;
    /*
    * LoRaMac tx/rx operation state
    */
    LoRaMacFlags_t MacFlags;
    /*
    * Data structure indicating if a request is allowed or not.
    */
    LoRaMacRequestHandling_t AllowRequests;
    /*
    * Non-volatile module context structure
    */
    LoRaMacNvmCtx_t* NvmCtx;
    /*
    * Duty cycle wait time
    */
    TimerTime_t DutyCycleWaitTime;
}LoRaMacCtx_t;

/*
 * Module context.
 */
static LoRaMacCtx_t MacCtx;

/*
 * Non-volatile module context.
 */
static LoRaMacNvmCtx_t NvmMacCtx;



/*
 * List of module contexts.
 */
LoRaMacCtxs_t Contexts;

/*!
 * Defines the LoRaMac radio events status
 */
typedef union uLoRaMacRadioEvents
{
    uint32_t Value;
    struct sEvents
    {
        uint32_t RxTimeout : 1;
        uint32_t RxError   : 1;
        uint32_t TxTimeout : 1;
        uint32_t RxDone    : 1;
        uint32_t TxDone    : 1;
    }Events;
}LoRaMacRadioEvents_t;

/*!
 * LoRaMac radio events status
 */
LoRaMacRadioEvents_t LoRaMacRadioEvents = { .Value = 0 };

/*!
 * \brief Function to be executed on Radio Tx Done event
 */
static void OnRadioTxDone( void );

/*!
 * \brief This function prepares the MAC to abort the execution of function
 *        OnRadioRxDone in case of a reception error.
 */
static void PrepareRxDoneAbort( void );

/*!
 * \brief Function to be executed on Radio Rx Done event
 */
static void OnRadioRxDone( uint8_t* payload, uint16_t size, int16_t rssi, int8_t snr );

/*!
 * \brief Function executed on Radio Tx Timeout event
 */
static void OnRadioTxTimeout( void );

/*!
 * \brief Function executed on Radio Rx error event
 */
static void OnRadioRxError( void );

/*!
 * \brief Function executed on Radio Rx Timeout event
 */
static void OnRadioRxTimeout( void );

/*!
 * \brief Function executed on duty cycle delayed Tx  timer event
 */
static void OnTxDelayedTimerEvent( void* context );

/*!
 * \brief Function executed on first Rx window timer event
 */
static void OnRxWindow1TimerEvent( void* context );

/*!
 * \brief Function executed on second Rx window timer event
 */
static void OnRxWindow2TimerEvent( void* context );

/*!
 * \brief Function executed on AckTimeout timer event
 */
static void OnAckTimeoutTimerEvent( void* context );

/*!
 * \brief Configures the events to trigger an MLME-Indication with
 *        a MLME type of MLME_SCHEDULE_UPLINK.
 */
static void SetMlmeScheduleUplinkIndication( void );

/*!
 * Computes next 32 bit downlink counter value and determines the frame counter ID.
 *
 * \param[IN]     addrID                - Address identifier
 * \param[IN]     fType                 - Frame type
 * \param[IN]     macMsg                - Data message object, holding the current 16 bit transmitted frame counter
 * \param[IN]     lrWanVersion          - LoRaWAN version
 * \param[IN]     maxFCntGap            - Maximum allowed frame counter difference (only for 1.0.X necessary)
 * \param[OUT]    fCntID                - Frame counter identifier
 * \param[OUT]    currentDown           - Current downlink counter value
 *
 * \retval                              - Status of the operation
 */
static LoRaMacCryptoStatus_t GetFCntDown( AddressIdentifier_t addrID, FType_t fType, LoRaMacMessageData_t* macMsg, Version_t lrWanVersion,
                                          uint16_t maxFCntGap, FCntIdentifier_t* fCntID, uint32_t* currentDown );

/*!
 * \brief Switches the device class
 *
 * \param [IN] deviceClass Device class to switch to
 */
static LoRaMacStatus_t SwitchClass( DeviceClass_t deviceClass );

/*!
 * \brief Gets the maximum application payload length in the absence of the optional FOpt field.
 *
 * \param [IN] datarate        Current datarate
 *
 * \retval                    Max length
 */
static uint8_t GetMaxAppPayloadWithoutFOptsLength( int8_t datarate );

/*!
 * \brief Validates if the payload fits into the frame, taking the datarate
 *        into account.
 *
 * \details Refer to chapter 4.3.2 of the LoRaWAN specification, v1.0
 *
 * \param lenN Length of the application payload. The length depends on the
 *             datarate and is region specific
 *
 * \param datarate Current datarate
 *
 * \param fOptsLen Length of the fOpts field
 *
 * \retval [false: payload does not fit into the frame, true: payload fits into
 *          the frame]
 */
static bool ValidatePayloadLength( uint8_t lenN, int8_t datarate, uint8_t fOptsLen );

/*!
 * \brief Decodes MAC commands in the fOpts field and in the payload
 *
 * \param [IN] payload      A pointer to the payload
 * \param [IN] macIndex     The index of the payload where the MAC commands start
 * \param [IN] commandsSize The size of the MAC commands
 * \param [IN] snr          The SNR value  of the frame
 * \param [IN] rxSlot       The RX slot where the frame was received
 */
static void ProcessMacCommands( uint8_t* payload, uint8_t macIndex, uint8_t commandsSize, int8_t snr, LoRaMacRxSlot_t rxSlot );

/*!
 * \brief LoRaMAC layer generic send frame
 *
 * \param [IN] macHdr      MAC header field
 * \param [IN] fPort       MAC payload port
 * \param [IN] fBuffer     MAC data buffer to be sent
 * \param [IN] fBufferSize MAC data buffer size
 * \retval status          Status of the operation.
 */
LoRaMacStatus_t Send( LoRaMacHeader_t* macHdr, uint8_t fPort, void* fBuffer, uint16_t fBufferSize );

/*!
 * \brief LoRaMAC layer send join/rejoin request
 *
 * \param [IN] joinReqType Type of join-request or rejoin
 *
 * \retval status          Status of the operation.
 */
LoRaMacStatus_t SendReJoinReq( JoinReqIdentifier_t joinReqType );

/*!
 * \brief LoRaMAC layer frame buffer initialization
 *
 * \param [IN] macHdr      MAC header field
 * \param [IN] fCtrl       MAC frame control field
 * \param [IN] fOpts       MAC commands buffer
 * \param [IN] fPort       MAC payload port
 * \param [IN] fBuffer     MAC data buffer to be sent
 * \param [IN] fBufferSize MAC data buffer size
 * \retval status          Status of the operation.
 */
LoRaMacStatus_t PrepareFrame( LoRaMacHeader_t* macHdr, LoRaMacFrameCtrl_t* fCtrl, uint8_t fPort, void* fBuffer, uint16_t fBufferSize );

/*
 * \brief Schedules the frame according to the duty cycle
 *
 * \param [IN] allowDelayedTx When set to true, the a frame will be delayed,
 *                            the duty cycle restriction is active
 * \retval Status of the operation
 */
static LoRaMacStatus_t ScheduleTx( bool allowDelayedTx );

/*
 * \brief Secures the current processed frame ( TxMsg )
 * \param[IN]     txDr      Data rate used for the transmission
 * \param[IN]     txCh      Index of the channel used for the transmission
 * \retval status           Status of the operation
 */
static LoRaMacStatus_t SecureFrame( uint8_t txDr, uint8_t txCh );

/*
 * \brief Calculates the aggregated back off time.
 */
static void CalculateBackOff( void );

/*
 * \brief Function to remove pending MAC commands
 *
 * \param [IN] rxSlot     The RX slot on which the frame was received
 * \param [IN] fCtrl      The frame control field of the received frame
 * \param [IN] request    The request type
 */
static void RemoveMacCommands( LoRaMacRxSlot_t rxSlot, LoRaMacFrameCtrl_t fCtrl, Mcps_t request );

/*!
 * \brief LoRaMAC layer prepared frame buffer transmission with channel specification
 *
 * \remark PrepareFrame must be called at least once before calling this
 *         function.
 *
 * \param [IN] channel     Channel to transmit on
 * \retval status          Status of the operation.
 */
LoRaMacStatus_t SendFrameOnChannel( uint8_t channel );

/*!
 * \brief Sets the radio in continuous transmission mode
 *
 * \remark Uses the radio parameters set on the previous transmission.
 *
 * \param [IN] timeout     Time in seconds while the radio is kept in continuous wave mode
 * \retval status          Status of the operation.
 */
LoRaMacStatus_t SetTxContinuousWave( uint16_t timeout );

/*!
 * \brief Sets the radio in continuous transmission mode
 *
 * \remark Uses the radio parameters set on the previous transmission.
 *
 * \param [IN] timeout     Time in seconds while the radio is kept in continuous wave mode
 * \param [IN] frequency   RF frequency to be set.
 * \param [IN] power       RF output power to be set.
 * \retval status          Status of the operation.
 */
LoRaMacStatus_t SetTxContinuousWave1( uint16_t timeout, uint32_t frequency, uint8_t power );

/*!
 * \brief Resets MAC specific parameters to default
 */
static void ResetMacParameters( void );

/*!
 * \brief Initializes and opens the reception window
 *
 * \param [IN] rxTimer  Window timer to be topped.
 * \param [IN] rxConfig Window parameters to be setup
 */
static void RxWindowSetup( TimerEvent_t* rxTimer, RxConfigParams_t* rxConfig );

/*!
 * \brief Opens up a continuous RX C window. This is used for
 *        class c devices.
 */
static void OpenContinuousRxCWindow( void );

/*!
 * \brief   Returns a pointer to the internal contexts structure.
 *
 * \retval  void Points to a structure containing all contexts
 */
LoRaMacCtxs_t* GetCtxs( void );

/*!
 * \brief   Restoring of internal module contexts
 *
 * \details This function allows to restore module contexts by a given pointer.
 *
 *
 * \retval  LoRaMacStatus_t Status of the operation. Possible returns are:
 *          returns are:
 *          \ref LORAMAC_STATUS_OK,
 *          \ref LORAMAC_STATUS_PARAMETER_INVALID,
 */
LoRaMacStatus_t RestoreCtxs( LoRaMacCtxs_t* contexts );

/*!
 * \brief   Determines the frame type
 *
 * \param [IN] macMsg Data message object
 *
 * \param [OUT] fType Frame type
 *
 * \retval  LoRaMacStatus_t Status of the operation. Possible returns are:
 *          returns are:
 *          \ref LORAMAC_STATUS_OK,
 *          \ref LORAMAC_STATUS_PARAMETER_INVALID,
 */
LoRaMacStatus_t DetermineFrameType( LoRaMacMessageData_t* macMsg, FType_t* fType );

/*!
 * \brief Checks if the retransmission should be stopped in case of a unconfirmed uplink
 *
 * \retval Returns true if it should be stopped.
 */
static bool CheckRetransUnconfirmedUplink( void );

/*!
 * \brief Checks if the retransmission should be stopped in case of a confirmed uplink
 *
 * \retval Returns true it should be stopped.
 */
static bool CheckRetransConfirmedUplink( void );

/*!
 * \brief Stops the uplink retransmission
 *
 * \retval Returns true if successful.
 */
static bool StopRetransmission( void );

/*!
 * \brief Handles the ACK retries algorithm.
 *        Increments the re-tries counter up until the specified number of
 *        trials or the allowed maximum. Decrease the uplink datarate every 2
 *        trials.
 */
static void AckTimeoutRetriesProcess( void );

/*!
 * \brief Finalizes the ACK retries algorithm.
 *        If no ACK is received restores the default channels
 */
static void AckTimeoutRetriesFinalize( void );

/*!
 * \brief Calls the callback to indicate that a context changed
 */
static void CallNvmCtxCallback( LoRaMacNvmCtxModule_t module );

/*!
 * \brief MAC NVM Context has been changed
 */
static void EventMacNvmCtxChanged( void );

/*!
 * \brief Region NVM Context has been changed
 */
static void EventRegionNvmCtxChanged( void );

/*!
 * \brief Crypto NVM Context has been changed
 */
static void EventCryptoNvmCtxChanged( void );

/*!
 * \brief Secure Element NVM Context has been changed
 */
static void EventSecureElementNvmCtxChanged( void );

/*!
 * \brief MAC commands module nvm context has been changed
 */
static void EventCommandsNvmCtxChanged( void );

/*!
 * \brief Class B module nvm context has been changed
 */
static void EventClassBNvmCtxChanged( void );

/*!
 * \brief Confirm Queue module nvm context has been changed
 */
static void EventConfirmQueueNvmCtxChanged( void );

/*!
 * \brief Verifies if a request is pending currently
 *
 *\retval 1: Request pending, 0: request not pending
 */
static uint8_t IsRequestPending( void );

/*!
 * \brief Enabled the possibility to perform requests
 *
 * \param [IN] requestState Request permission state
 */
static void LoRaMacEnableRequests( LoRaMacRequestHandling_t requestState );

/*!
 * \brief This function verifies if a RX abort occurred
 */
static void LoRaMacCheckForRxAbort( void );

/*!
 * \brief This function verifies if a beacon acquisition MLME
 *        request was pending
 *
 * \retval 1: Request pending, 0: no request pending
 */
static uint8_t LoRaMacCheckForBeaconAcquisition( void );

/*!
 * \brief This function handles join request
 */
static void LoRaMacHandleMlmeRequest( void );

/*!
 * \brief This function handles mcps request
 */
static void LoRaMacHandleMcpsRequest( void );

/*!
 * \brief This function handles callback events for requests
 */
static void LoRaMacHandleRequestEvents( void );

/*!
 * \brief This function handles callback events for indications
 */
static void LoRaMacHandleIndicationEvents( void );

/*!
 * Structure used to store the radio Tx event data
 */
struct
{
    TimerTime_t CurTime;
}TxDoneParams;

/*!
 * Structure used to store the radio Rx event data
 */
struct
{
    TimerTime_t LastRxDone;
    uint8_t *Payload;
    uint16_t Size;
    int16_t Rssi;
    int8_t Snr;
}RxDoneParams;

static void OnRadioTxDone( void )
{
    TxDoneParams.CurTime = TimerGetCurrentTime( );
    MacCtx.LastTxSysTime = SysTimeGet( );

    LoRaMacRadioEvents.Events.TxDone = 1;

    if( ( MacCtx.MacCallbacks != NULL ) && ( MacCtx.MacCallbacks->MacProcessNotify != NULL ) )
    {
        MacCtx.MacCallbacks->MacProcessNotify( );
    }
}

static void OnRadioRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr )
{
    RxDoneParams.LastRxDone = TimerGetCurrentTime( );
    RxDoneParams.Payload = payload;
    RxDoneParams.Size = size;
    RxDoneParams.Rssi = rssi;
    RxDoneParams.Snr = snr;

    LoRaMacRadioEvents.Events.RxDone = 1;

    if( ( MacCtx.MacCallbacks != NULL ) && ( MacCtx.MacCallbacks->MacProcessNotify != NULL ) )
    {
        MacCtx.MacCallbacks->MacProcessNotify( );
    }
}

static void OnRadioTxTimeout( void )
{
    LoRaMacRadioEvents.Events.TxTimeout = 1;

    if( ( MacCtx.MacCallbacks != NULL ) && ( MacCtx.MacCallbacks->MacProcessNotify != NULL ) )
    {
        MacCtx.MacCallbacks->MacProcessNotify( );
    }
}

static void OnRadioRxError( void )
{
    LoRaMacRadioEvents.Events.RxError = 1;

    if( ( MacCtx.MacCallbacks != NULL ) && ( MacCtx.MacCallbacks->MacProcessNotify != NULL ) )
    {
        MacCtx.MacCallbacks->MacProcessNotify( );
    }
}

static void OnRadioRxTimeout( void )
{
    LoRaMacRadioEvents.Events.RxTimeout = 1;

    if( ( MacCtx.MacCallbacks != NULL ) && ( MacCtx.MacCallbacks->MacProcessNotify != NULL ) )
    {
        MacCtx.MacCallbacks->MacProcessNotify( );
    }
}

static void UpdateRxSlotIdleState( void )
{
    if( MacCtx.NvmCtx->DeviceClass != CLASS_C )
    {
        MacCtx.RxSlot = RX_SLOT_NONE;
    }
    else
    {
        MacCtx.RxSlot = RX_SLOT_WIN_CLASS_C;
    }
}

static void ProcessRadioTxDone( void )
{
    GetPhyParams_t getPhy;
    PhyParam_t phyParam;
    SetBandTxDoneParams_t txDone;

    if( MacCtx.NvmCtx->DeviceClass != CLASS_C )
    {
        Radio.Sleep( );
    }
    // Setup timers
    TimerSetValue( &MacCtx.RxWindowTimer1, MacCtx.RxWindow1Delay );
    TimerStart( &MacCtx.RxWindowTimer1 );
    TimerSetValue( &MacCtx.RxWindowTimer2, MacCtx.RxWindow2Delay );
    TimerStart( &MacCtx.RxWindowTimer2 );

    if( ( MacCtx.NvmCtx->DeviceClass == CLASS_C ) || ( MacCtx.NodeAckRequested == true ) )
    {
        getPhy.Attribute = PHY_ACK_TIMEOUT;
        phyParam = RegionGetPhyParam( MacCtx.NvmCtx->Region, &getPhy );
        TimerSetValue( &MacCtx.AckTimeoutTimer, MacCtx.RxWindow2Delay + phyParam.Value );
        TimerStart( &MacCtx.AckTimeoutTimer );
    }

    // Update Aggregated last tx done time
    MacCtx.NvmCtx->LastTxDoneTime = TxDoneParams.CurTime;

    // Update last tx done time for the current channel
    txDone.Channel = MacCtx.Channel;
    txDone.LastTxDoneTime = TxDoneParams.CurTime;
    txDone.ElapsedTimeSinceStartUp = SysTimeSub( SysTimeGetMcuTime( ), MacCtx.NvmCtx->InitializationTime );
    txDone.LastTxAirTime = MacCtx.TxTimeOnAir;
    txDone.Joined  = true;
    if( MacCtx.NvmCtx->NetworkActivation == ACTIVATION_TYPE_NONE )
    {
        txDone.Joined  = false;
    }

    RegionSetBandTxDone( MacCtx.NvmCtx->Region, &txDone );

    if( MacCtx.NodeAckRequested == false )
    {
        MacCtx.McpsConfirm.Status = LORAMAC_EVENT_INFO_STATUS_OK;
    }
}

static void PrepareRxDoneAbort( void )
{
    MacCtx.MacState |= LORAMAC_RX_ABORT;

    if( MacCtx.NodeAckRequested == true )
    {
        OnAckTimeoutTimerEvent( NULL );
    }

    MacCtx.MacFlags.Bits.McpsInd = 1;
    MacCtx.MacFlags.Bits.MacDone = 1;

    UpdateRxSlotIdleState( );
}

static void ProcessRadioRxDone( void )
{
    LoRaMacHeader_t macHdr;
    ApplyCFListParams_t applyCFList;
    GetPhyParams_t getPhy;
    PhyParam_t phyParam;
    LoRaMacCryptoStatus_t macCryptoStatus = LORAMAC_CRYPTO_ERROR;

    LoRaMacMessageData_t macMsgData;
    LoRaMacMessageJoinAccept_t macMsgJoinAccept;
    uint8_t *payload = RxDoneParams.Payload;
    uint16_t size = RxDoneParams.Size;
    int16_t rssi = RxDoneParams.Rssi;
    int8_t snr = RxDoneParams.Snr;

    uint8_t pktHeaderLen = 0;

    uint32_t downLinkCounter = 0;
    uint32_t address = MacCtx.NvmCtx->DevAddr;
    uint8_t multicast = 0;
    AddressIdentifier_t addrID = UNICAST_DEV_ADDR;
    FCntIdentifier_t fCntID;

    MacCtx.McpsConfirm.AckReceived = false;
    MacCtx.McpsIndication.Rssi = rssi;
    MacCtx.McpsIndication.Snr = snr;
    MacCtx.McpsIndication.RxSlot = MacCtx.RxSlot;
    MacCtx.McpsIndication.Port = 0;
    MacCtx.McpsIndication.Multicast = 0;
    MacCtx.McpsIndication.FramePending = 0;
    MacCtx.McpsIndication.Buffer = NULL;
    MacCtx.McpsIndication.BufferSize = 0;
    MacCtx.McpsIndication.RxData = false;
    MacCtx.McpsIndication.AckReceived = false;
    MacCtx.McpsIndication.DownLinkCounter = 0;
    MacCtx.McpsIndication.McpsIndication = MCPS_UNCONFIRMED;
    MacCtx.McpsIndication.DevAddress = 0;
    MacCtx.McpsIndication.DeviceTimeAnsReceived = false;

    Radio.Sleep( );
    TimerStop( &MacCtx.RxWindowTimer2 );

    // This function must be called even if we are not in class b mode yet.
    if( LoRaMacClassBRxBeacon( payload, size ) == true )
    {
        MacCtx.MlmeIndication.BeaconInfo.Rssi = rssi;
        MacCtx.MlmeIndication.BeaconInfo.Snr = snr;
        return;
    }
    // Check if we expect a ping or a multicast slot.
    if( MacCtx.NvmCtx->DeviceClass == CLASS_B )
    {
        if( LoRaMacClassBIsPingExpected( ) == true )
        {
            LoRaMacClassBSetPingSlotState( PINGSLOT_STATE_CALC_PING_OFFSET );
            LoRaMacClassBPingSlotTimerEvent( NULL );
            MacCtx.McpsIndication.RxSlot = RX_SLOT_WIN_CLASS_B_PING_SLOT;
        }
        else if( LoRaMacClassBIsMulticastExpected( ) == true )
        {
            LoRaMacClassBSetMulticastSlotState( PINGSLOT_STATE_CALC_PING_OFFSET );
            LoRaMacClassBMulticastSlotTimerEvent( NULL );
            MacCtx.McpsIndication.RxSlot = RX_SLOT_WIN_CLASS_B_MULTICAST_SLOT;
        }
    }

    macHdr.Value = payload[pktHeaderLen++];

    switch( macHdr.Bits.MType )
    {
        case FRAME_TYPE_JOIN_ACCEPT:
            // Check if the received frame size is valid
            if( size < LORAMAC_JOIN_ACCEPT_FRAME_MIN_SIZE )
            {
                MacCtx.McpsIndication.Status = LORAMAC_EVENT_INFO_STATUS_ERROR;
                PrepareRxDoneAbort( );
                return;
            }
            macMsgJoinAccept.Buffer = payload;
            macMsgJoinAccept.BufSize = size;

            // Abort in case if the device isn't joined yet and no rejoin request is ongoing.
            if( MacCtx.NvmCtx->NetworkActivation != ACTIVATION_TYPE_NONE )
            {
                MacCtx.McpsIndication.Status = LORAMAC_EVENT_INFO_STATUS_ERROR;
                PrepareRxDoneAbort( );
                return;
            }
            macCryptoStatus = LoRaMacCryptoHandleJoinAccept( JOIN_REQ, SecureElementGetJoinEui( ), &macMsgJoinAccept );

            if( LORAMAC_CRYPTO_SUCCESS == macCryptoStatus )
            {
                // Network ID
                MacCtx.NvmCtx->NetID = ( uint32_t ) macMsgJoinAccept.NetID[0];
                MacCtx.NvmCtx->NetID |= ( ( uint32_t ) macMsgJoinAccept.NetID[1] << 8 );
                MacCtx.NvmCtx->NetID |= ( ( uint32_t ) macMsgJoinAccept.NetID[2] << 16 );

                // Device Address
                MacCtx.NvmCtx->DevAddr = macMsgJoinAccept.DevAddr;

                // DLSettings
                MacCtx.NvmCtx->MacParams.Rx1DrOffset = macMsgJoinAccept.DLSettings.Bits.RX1DRoffset;
                MacCtx.NvmCtx->MacParams.Rx2Channel.Datarate = macMsgJoinAccept.DLSettings.Bits.RX2DataRate;
                MacCtx.NvmCtx->MacParams.RxCChannel.Datarate = macMsgJoinAccept.DLSettings.Bits.RX2DataRate;

                // RxDelay
                MacCtx.NvmCtx->MacParams.ReceiveDelay1 = macMsgJoinAccept.RxDelay;
                if( MacCtx.NvmCtx->MacParams.ReceiveDelay1 == 0 )
                {
                    MacCtx.NvmCtx->MacParams.ReceiveDelay1 = 1;
                }
                MacCtx.NvmCtx->MacParams.ReceiveDelay1 *= 1000;
                MacCtx.NvmCtx->MacParams.ReceiveDelay2 = MacCtx.NvmCtx->MacParams.ReceiveDelay1 + 1000;

                MacCtx.NvmCtx->Version.Fields.Minor = 0;

                // Apply CF list
                applyCFList.Payload = macMsgJoinAccept.CFList;
                // Size of the regular payload is 12. Plus 1 byte MHDR and 4 bytes MIC
                applyCFList.Size = size - 17;

                RegionApplyCFList( MacCtx.NvmCtx->Region, &applyCFList );

                MacCtx.NvmCtx->NetworkActivation = ACTIVATION_TYPE_OTAA;

                // MLME handling
                if( LoRaMacConfirmQueueIsCmdActive( MLME_JOIN ) == true )
                {
                    LoRaMacConfirmQueueSetStatus( LORAMAC_EVENT_INFO_STATUS_OK, MLME_JOIN );
                }
            }
            else
            {
                // MLME handling
                if( LoRaMacConfirmQueueIsCmdActive( MLME_JOIN ) == true )
                {
                    LoRaMacConfirmQueueSetStatus( LORAMAC_EVENT_INFO_STATUS_JOIN_FAIL, MLME_JOIN );
                }
            }
            break;
        case FRAME_TYPE_DATA_CONFIRMED_DOWN:
            MacCtx.McpsIndication.McpsIndication = MCPS_CONFIRMED;
            // Intentional fall through
        case FRAME_TYPE_DATA_UNCONFIRMED_DOWN:
            // Check if the received payload size is valid
            getPhy.UplinkDwellTime = MacCtx.NvmCtx->MacParams.DownlinkDwellTime;
            getPhy.Datarate = MacCtx.McpsIndication.RxDatarate;
            getPhy.Attribute = PHY_MAX_PAYLOAD;
            phyParam = RegionGetPhyParam( MacCtx.NvmCtx->Region, &getPhy );
            if( ( MAX( 0, ( int16_t )( ( int16_t ) size - ( int16_t ) LORAMAC_FRAME_PAYLOAD_OVERHEAD_SIZE ) ) > ( int16_t )phyParam.Value ) ||
                ( size < LORAMAC_FRAME_PAYLOAD_MIN_SIZE ) )
            {
                MacCtx.McpsIndication.Status = LORAMAC_EVENT_INFO_STATUS_ERROR;
                PrepareRxDoneAbort( );
                return;
            }
            macMsgData.Buffer = payload;
            macMsgData.BufSize = size;
            macMsgData.FRMPayload = MacCtx.RxPayload;
            macMsgData.FRMPayloadSize = LORAMAC_PHY_MAXPAYLOAD;

            if( LORAMAC_PARSER_SUCCESS != LoRaMacParserData( &macMsgData ) )
            {
                MacCtx.McpsIndication.Status = LORAMAC_EVENT_INFO_STATUS_ERROR;
                PrepareRxDoneAbort( );
                return;
            }

            // Store device address
            MacCtx.McpsIndication.DevAddress = macMsgData.FHDR.DevAddr;

            FType_t fType;
            if( LORAMAC_STATUS_OK != DetermineFrameType( &macMsgData, &fType ) )
            {
                MacCtx.McpsIndication.Status = LORAMAC_EVENT_INFO_STATUS_ERROR;
                PrepareRxDoneAbort( );
                return;
            }

            //Check if it is a multicast message
            multicast = 0;
            downLinkCounter = 0;
            for( uint8_t i = 0; i < LORAMAC_MAX_MC_CTX; i++ )
            {
                if( ( MacCtx.NvmCtx->MulticastChannelList[i].ChannelParams.Address == macMsgData.FHDR.DevAddr ) &&
                    ( MacCtx.NvmCtx->MulticastChannelList[i].ChannelParams.IsEnabled == true ) )
                {
                    multicast = 1;
                    addrID = MacCtx.NvmCtx->MulticastChannelList[i].ChannelParams.GroupID;
                    downLinkCounter = *( MacCtx.NvmCtx->MulticastChannelList[i].DownLinkCounter );
                    address = MacCtx.NvmCtx->MulticastChannelList[i].ChannelParams.Address;
                    if( MacCtx.NvmCtx->DeviceClass == CLASS_C )
                    {
                        MacCtx.McpsIndication.RxSlot = RX_SLOT_WIN_CLASS_C_MULTICAST;
                    }
                    break;
                }
            }

            // Filter messages according to multicast downlink exceptions
            if( ( multicast == 1 ) && ( ( fType != FRAME_TYPE_D ) ||
                                        ( macMsgData.FHDR.FCtrl.Bits.Ack == true ) ||
                                        ( macMsgData.FHDR.FCtrl.Bits.AdrAckReq == true ) ) )
            {
                MacCtx.McpsIndication.Status = LORAMAC_EVENT_INFO_STATUS_ERROR;
                PrepareRxDoneAbort( );
                return;
            }

            // Get maximum allowed counter difference
            getPhy.Attribute = PHY_MAX_FCNT_GAP;
            phyParam = RegionGetPhyParam( MacCtx.NvmCtx->Region, &getPhy );

            // Get downlink frame counter value
            macCryptoStatus = GetFCntDown( addrID, fType, &macMsgData, MacCtx.NvmCtx->Version, phyParam.Value, &fCntID, &downLinkCounter );
            if( macCryptoStatus != LORAMAC_CRYPTO_SUCCESS )
            {
                if( macCryptoStatus == LORAMAC_CRYPTO_FAIL_FCNT_DUPLICATED )
                {
                    // Catch the case of repeated downlink frame counter
                    MacCtx.McpsIndication.Status = LORAMAC_EVENT_INFO_STATUS_DOWNLINK_REPEATED;
                    if( ( MacCtx.NvmCtx->Version.Fields.Minor == 0 ) && ( macHdr.Bits.MType == FRAME_TYPE_DATA_CONFIRMED_DOWN ) && ( MacCtx.NvmCtx->LastRxMic == macMsgData.MIC ) )
                    {
                        MacCtx.NvmCtx->SrvAckRequested = true;
                    }
                }
                else if( macCryptoStatus == LORAMAC_CRYPTO_FAIL_MAX_GAP_FCNT )
                {
                    // Lost too many frames
                    MacCtx.McpsIndication.Status = LORAMAC_EVENT_INFO_STATUS_DOWNLINK_TOO_MANY_FRAMES_LOSS;
                }
                else
                {
                    // Other errors
                    MacCtx.McpsIndication.Status = LORAMAC_EVENT_INFO_STATUS_ERROR;
                }
                MacCtx.McpsIndication.DownLinkCounter = downLinkCounter;
                PrepareRxDoneAbort( );
                return;
            }

            macCryptoStatus = LoRaMacCryptoUnsecureMessage( addrID, address, fCntID, downLinkCounter, &macMsgData );
            if( macCryptoStatus != LORAMAC_CRYPTO_SUCCESS )
            {
                if( macCryptoStatus == LORAMAC_CRYPTO_FAIL_ADDRESS )
                {
                    // We are not the destination of this frame.
                    MacCtx.McpsIndication.Status = LORAMAC_EVENT_INFO_STATUS_ADDRESS_FAIL;
                }
                else
                {
                    // MIC calculation fail
                    MacCtx.McpsIndication.Status = LORAMAC_EVENT_INFO_STATUS_MIC_FAIL;
                }
                PrepareRxDoneAbort( );
                return;
            }

            // Frame is valid
            MacCtx.McpsIndication.Status = LORAMAC_EVENT_INFO_STATUS_OK;
            MacCtx.McpsIndication.Multicast = multicast;
            MacCtx.McpsIndication.FramePending = macMsgData.FHDR.FCtrl.Bits.FPending;
            MacCtx.McpsIndication.Buffer = NULL;
            MacCtx.McpsIndication.BufferSize = 0;
            MacCtx.McpsIndication.DownLinkCounter = downLinkCounter;
            MacCtx.McpsIndication.AckReceived = macMsgData.FHDR.FCtrl.Bits.Ack;

            MacCtx.McpsConfirm.Status = LORAMAC_EVENT_INFO_STATUS_OK;
            MacCtx.McpsConfirm.AckReceived = macMsgData.FHDR.FCtrl.Bits.Ack;

            // Reset ADR ACK Counter only, when RX1 or RX2 slot
            if( ( MacCtx.McpsIndication.RxSlot == RX_SLOT_WIN_1 ) ||
                ( MacCtx.McpsIndication.RxSlot == RX_SLOT_WIN_2 ) )
            {
                MacCtx.NvmCtx->AdrAckCounter = 0;
            }

            // MCPS Indication and ack requested handling
            if( multicast == 1 )
            {
                MacCtx.McpsIndication.McpsIndication = MCPS_MULTICAST;
            }
            else
            {
                if( macHdr.Bits.MType == FRAME_TYPE_DATA_CONFIRMED_DOWN )
                {
                    MacCtx.NvmCtx->SrvAckRequested = true;
                    if( MacCtx.NvmCtx->Version.Fields.Minor == 0 )
                    {
                        MacCtx.NvmCtx->LastRxMic = macMsgData.MIC;
                    }
                    MacCtx.McpsIndication.McpsIndication = MCPS_CONFIRMED;
                }
                else
                {
                    MacCtx.NvmCtx->SrvAckRequested = false;
                    MacCtx.McpsIndication.McpsIndication = MCPS_UNCONFIRMED;
                }
            }

            RemoveMacCommands( MacCtx.McpsIndication.RxSlot, macMsgData.FHDR.FCtrl, MacCtx.McpsConfirm.McpsRequest );

            switch( fType )
            {
                case FRAME_TYPE_A:
                {  /* +----------+------+-------+--------------+
                    * | FOptsLen | Fopt | FPort |  FRMPayload  |
                    * +----------+------+-------+--------------+
                    * |    > 0   |   X  |  > 0  |       X      |
                    * +----------+------+-------+--------------+
                    */

                    // Decode MAC commands in FOpts field
                    ProcessMacCommands( macMsgData.FHDR.FOpts, 0, macMsgData.FHDR.FCtrl.Bits.FOptsLen, snr, MacCtx.McpsIndication.RxSlot );
                    MacCtx.McpsIndication.Port = macMsgData.FPort;
                    MacCtx.McpsIndication.Buffer = macMsgData.FRMPayload;
                    MacCtx.McpsIndication.BufferSize = macMsgData.FRMPayloadSize;
                    MacCtx.McpsIndication.RxData = true;
                    break;
                }
                case FRAME_TYPE_B:
                {  /* +----------+------+-------+--------------+
                    * | FOptsLen | Fopt | FPort |  FRMPayload  |
                    * +----------+------+-------+--------------+
                    * |    > 0   |   X  |   -   |       -      |
                    * +----------+------+-------+--------------+
                    */

                    // Decode MAC commands in FOpts field
                    ProcessMacCommands( macMsgData.FHDR.FOpts, 0, macMsgData.FHDR.FCtrl.Bits.FOptsLen, snr, MacCtx.McpsIndication.RxSlot );
                    MacCtx.McpsIndication.Port = macMsgData.FPort;
                    break;
                }
                case FRAME_TYPE_C:
                {  /* +----------+------+-------+--------------+
                    * | FOptsLen | Fopt | FPort |  FRMPayload  |
                    * +----------+------+-------+--------------+
                    * |    = 0   |   -  |  = 0  | MAC commands |
                    * +----------+------+-------+--------------+
                    */

                    // Decode MAC commands in FRMPayload
                    ProcessMacCommands( macMsgData.FRMPayload, 0, macMsgData.FRMPayloadSize, snr, MacCtx.McpsIndication.RxSlot );
                    MacCtx.McpsIndication.Port = macMsgData.FPort;
                    break;
                }
                case FRAME_TYPE_D:
                {  /* +----------+------+-------+--------------+
                    * | FOptsLen | Fopt | FPort |  FRMPayload  |
                    * +----------+------+-------+--------------+
                    * |    = 0   |   -  |  > 0  |       X      |
                    * +----------+------+-------+--------------+
                    */

                    // No MAC commands just application payload
                    MacCtx.McpsIndication.Port = macMsgData.FPort;
                    MacCtx.McpsIndication.Buffer = macMsgData.FRMPayload;
                    MacCtx.McpsIndication.BufferSize = macMsgData.FRMPayloadSize;
                    MacCtx.McpsIndication.RxData = true;
                    break;
                }
                default:
                    MacCtx.McpsIndication.Status = LORAMAC_EVENT_INFO_STATUS_ERROR;
                    PrepareRxDoneAbort( );
                    break;
            }

            // Provide always an indication, skip the callback to the user application,
            // in case of a confirmed downlink retransmission.
            MacCtx.MacFlags.Bits.McpsInd = 1;

            break;
        case FRAME_TYPE_PROPRIETARY:
            memcpy1( MacCtx.RxPayload, &payload[pktHeaderLen], size - pktHeaderLen );

            MacCtx.McpsIndication.McpsIndication = MCPS_PROPRIETARY;
            MacCtx.McpsIndication.Status = LORAMAC_EVENT_INFO_STATUS_OK;
            MacCtx.McpsIndication.Buffer = MacCtx.RxPayload;
            MacCtx.McpsIndication.BufferSize = size - pktHeaderLen;

            MacCtx.MacFlags.Bits.McpsInd = 1;
            break;
        default:
            MacCtx.McpsIndication.Status = LORAMAC_EVENT_INFO_STATUS_ERROR;
            PrepareRxDoneAbort( );
            break;
    }

    // Verify if we need to disable the AckTimeoutTimer
    if( MacCtx.NodeAckRequested == true )
    {
        if( MacCtx.McpsConfirm.AckReceived == true )
        {
            OnAckTimeoutTimerEvent( NULL );
        }
    }
    else
    {
        if( MacCtx.NvmCtx->DeviceClass == CLASS_C )
        {
            OnAckTimeoutTimerEvent( NULL );
        }
    }
    MacCtx.MacFlags.Bits.MacDone = 1;

    UpdateRxSlotIdleState( );
}

static void ProcessRadioTxTimeout( void )
{
    if( MacCtx.NvmCtx->DeviceClass != CLASS_C )
    {
        Radio.Sleep( );
    }
    UpdateRxSlotIdleState( );

    MacCtx.McpsConfirm.Status = LORAMAC_EVENT_INFO_STATUS_TX_TIMEOUT;
    LoRaMacConfirmQueueSetStatusCmn( LORAMAC_EVENT_INFO_STATUS_TX_TIMEOUT );
    if( MacCtx.NodeAckRequested == true )
    {
        MacCtx.AckTimeoutRetry = true;
    }
    MacCtx.MacFlags.Bits.MacDone = 1;
}

static void HandleRadioRxErrorTimeout( LoRaMacEventInfoStatus_t rx1EventInfoStatus, LoRaMacEventInfoStatus_t rx2EventInfoStatus )
{
    bool classBRx = false;

    if( MacCtx.NvmCtx->DeviceClass != CLASS_C )
    {
        Radio.Sleep( );
    }

    if( LoRaMacClassBIsBeaconExpected( ) == true )
    {
        LoRaMacClassBSetBeaconState( BEACON_STATE_TIMEOUT );
        LoRaMacClassBBeaconTimerEvent( NULL );
        classBRx = true;
    }
    if( MacCtx.NvmCtx->DeviceClass == CLASS_B )
    {
        if( LoRaMacClassBIsPingExpected( ) == true )
        {
            LoRaMacClassBSetPingSlotState( PINGSLOT_STATE_CALC_PING_OFFSET );
            LoRaMacClassBPingSlotTimerEvent( NULL );
            classBRx = true;
        }
        if( LoRaMacClassBIsMulticastExpected( ) == true )
        {
            LoRaMacClassBSetMulticastSlotState( PINGSLOT_STATE_CALC_PING_OFFSET );
            LoRaMacClassBMulticastSlotTimerEvent( NULL );
            classBRx = true;
        }
    }

    if( classBRx == false )
    {
        if( MacCtx.RxSlot == RX_SLOT_WIN_1 )
        {
            if( MacCtx.NodeAckRequested == true )
            {
                MacCtx.McpsConfirm.Status = rx1EventInfoStatus;
            }
            LoRaMacConfirmQueueSetStatusCmn( rx1EventInfoStatus );

            if( TimerGetElapsedTime( MacCtx.NvmCtx->LastTxDoneTime ) >= MacCtx.RxWindow2Delay )
            {
                TimerStop( &MacCtx.RxWindowTimer2 );
                MacCtx.MacFlags.Bits.MacDone = 1;
            }
        }
        else
        {
            if( MacCtx.NodeAckRequested == true )
            {
                MacCtx.McpsConfirm.Status = rx2EventInfoStatus;
            }
            LoRaMacConfirmQueueSetStatusCmn( rx2EventInfoStatus );

            if( MacCtx.NvmCtx->DeviceClass != CLASS_C )
            {
                MacCtx.MacFlags.Bits.MacDone = 1;
            }
        }
    }

    UpdateRxSlotIdleState( );
}

static void ProcessRadioRxError( void )
{
    HandleRadioRxErrorTimeout( LORAMAC_EVENT_INFO_STATUS_RX1_ERROR, LORAMAC_EVENT_INFO_STATUS_RX2_ERROR );
}

static void ProcessRadioRxTimeout( void )
{
    HandleRadioRxErrorTimeout( LORAMAC_EVENT_INFO_STATUS_RX1_TIMEOUT, LORAMAC_EVENT_INFO_STATUS_RX2_TIMEOUT );
}

static void LoRaMacHandleIrqEvents( void )
{
    LoRaMacRadioEvents_t events;

    CRITICAL_SECTION_BEGIN( );
    events = LoRaMacRadioEvents;
    LoRaMacRadioEvents.Value = 0;
    CRITICAL_SECTION_END( );

    if( events.Value != 0 )
    {
        if( events.Events.TxDone == 1 )
        {
            ProcessRadioTxDone( );
        }
        if( events.Events.RxDone == 1 )
        {
            ProcessRadioRxDone( );
        }
        if( events.Events.TxTimeout == 1 )
        {
            ProcessRadioTxTimeout( );
        }
        if( events.Events.RxError == 1 )
        {
            ProcessRadioRxError( );
        }
        if( events.Events.RxTimeout == 1 )
        {
            ProcessRadioRxTimeout( );
        }
    }
}

bool LoRaMacIsBusy( void )
{
    if( ( MacCtx.MacState == LORAMAC_IDLE ) &&
        ( MacCtx.AllowRequests == LORAMAC_REQUEST_HANDLING_ON ) )
    {
        return false;
    }
    return true;
}


static void LoRaMacEnableRequests( LoRaMacRequestHandling_t requestState )
{
    MacCtx.AllowRequests = requestState;
}

static void LoRaMacHandleRequestEvents( void )
{
    // Handle events
    LoRaMacFlags_t reqEvents = MacCtx.MacFlags;

    if( MacCtx.MacState == LORAMAC_IDLE )
    {
        // Update event bits
        if( MacCtx.MacFlags.Bits.McpsReq == 1 )
        {
            MacCtx.MacFlags.Bits.McpsReq = 0;
        }

        if( MacCtx.MacFlags.Bits.MlmeReq == 1 )
        {
            MacCtx.MacFlags.Bits.MlmeReq = 0;
        }

        // Allow requests again
        LoRaMacEnableRequests( LORAMAC_REQUEST_HANDLING_ON );

        // Handle callbacks
        if( reqEvents.Bits.McpsReq == 1 )
        {
            MacCtx.MacPrimitives->MacMcpsConfirm( &MacCtx.McpsConfirm );
        }

        if( reqEvents.Bits.MlmeReq == 1 )
        {
            LoRaMacConfirmQueueHandleCb( &MacCtx.MlmeConfirm );
            if( LoRaMacConfirmQueueGetCnt( ) > 0 )
            {
                MacCtx.MacFlags.Bits.MlmeReq = 1;
            }
        }

        // Start beaconing again
        LoRaMacClassBResumeBeaconing( );

        // Procedure done. Reset variables.
        MacCtx.MacFlags.Bits.MacDone = 0;
    }
}

static void LoRaMacHandleScheduleUplinkEvent( void )
{
    // Handle events
    if( MacCtx.MacState == LORAMAC_IDLE )
    {
        // Verify if sticky MAC commands are pending or not
        bool isStickyMacCommandPending = false;
        LoRaMacCommandsStickyCmdsPending( &isStickyMacCommandPending );
        if( isStickyMacCommandPending == true )
        {// Setup MLME indication
            SetMlmeScheduleUplinkIndication( );
        }
    }
}

static void LoRaMacHandleIndicationEvents( void )
{
    // Handle MLME indication
    if( MacCtx.MacFlags.Bits.MlmeInd == 1 )
    {
        MacCtx.MacFlags.Bits.MlmeInd = 0;
        MacCtx.MacPrimitives->MacMlmeIndication( &MacCtx.MlmeIndication );
    }

    if( MacCtx.MacFlags.Bits.MlmeSchedUplinkInd == 1 )
    {
        MlmeIndication_t schduleUplinkIndication;
        schduleUplinkIndication.MlmeIndication = MLME_SCHEDULE_UPLINK;
        schduleUplinkIndication.Status = LORAMAC_EVENT_INFO_STATUS_OK;

        MacCtx.MacPrimitives->MacMlmeIndication( &schduleUplinkIndication );
        MacCtx.MacFlags.Bits.MlmeSchedUplinkInd = 0;
    }

    // Handle MCPS indication
    if( MacCtx.MacFlags.Bits.McpsInd == 1 )
    {
        MacCtx.MacFlags.Bits.McpsInd = 0;
        MacCtx.MacPrimitives->MacMcpsIndication( &MacCtx.McpsIndication );
    }
}

static void LoRaMacHandleMcpsRequest( void )
{
    // Handle MCPS uplinks
    if( MacCtx.MacFlags.Bits.McpsReq == 1 )
    {
        bool stopRetransmission = false;
        bool waitForRetransmission = false;

        if( ( MacCtx.McpsConfirm.McpsRequest == MCPS_UNCONFIRMED ) ||
            ( MacCtx.McpsConfirm.McpsRequest == MCPS_PROPRIETARY ) )
        {
            stopRetransmission = CheckRetransUnconfirmedUplink( );
        }
        else if( MacCtx.McpsConfirm.McpsRequest == MCPS_CONFIRMED )
        {
            if( MacCtx.AckTimeoutRetry == true )
            {
                stopRetransmission = CheckRetransConfirmedUplink( );

                if( MacCtx.NvmCtx->Version.Fields.Minor == 0 )
                {
                    if( stopRetransmission == false )
                    {
                        AckTimeoutRetriesProcess( );
                    }
                    else
                    {
                        AckTimeoutRetriesFinalize( );
                    }
                }
            }
            else
            {
                waitForRetransmission = true;
            }
        }

        if( stopRetransmission == true )
        {// Stop retransmission
            TimerStop( &MacCtx.TxDelayedTimer );
            MacCtx.MacState &= ~LORAMAC_TX_DELAYED;
            StopRetransmission( );
        }
        else if( waitForRetransmission == false )
        {// Arrange further retransmission
            MacCtx.MacFlags.Bits.MacDone = 0;
            // Reset the state of the AckTimeout
            MacCtx.AckTimeoutRetry = false;
            // Sends the same frame again
            OnTxDelayedTimerEvent( NULL );
        }
    }
}

static void LoRaMacHandleMlmeRequest( void )
{
    // Handle join request
    if( MacCtx.MacFlags.Bits.MlmeReq == 1 )
    {
        if( LoRaMacConfirmQueueIsCmdActive( MLME_JOIN ) == true )
        {
            if( LoRaMacConfirmQueueGetStatus( MLME_JOIN ) == LORAMAC_EVENT_INFO_STATUS_OK )
            {// Node joined successfully
                MacCtx.ChannelsNbTransCounter = 0;
            }
            MacCtx.MacState &= ~LORAMAC_TX_RUNNING;
        }
        else if( ( LoRaMacConfirmQueueIsCmdActive( MLME_TXCW ) == true ) ||
                 ( LoRaMacConfirmQueueIsCmdActive( MLME_TXCW_1 ) == true ) )
        {
            MacCtx.MacState &= ~LORAMAC_TX_RUNNING;
        }
    }
}

static uint8_t LoRaMacCheckForBeaconAcquisition( void )
{
    if( ( LoRaMacConfirmQueueIsCmdActive( MLME_BEACON_ACQUISITION ) == true ) &&
        ( MacCtx.MacFlags.Bits.McpsReq == 0 ) )
    {
        if( MacCtx.MacFlags.Bits.MlmeReq == 1 )
        {
            MacCtx.MacState &= ~LORAMAC_TX_RUNNING;
            return 0x01;
        }
    }
    return 0x00;
}

static void LoRaMacCheckForRxAbort( void )
{
    // A error occurs during receiving
    if( ( MacCtx.MacState & LORAMAC_RX_ABORT ) == LORAMAC_RX_ABORT )
    {
        MacCtx.MacState &= ~LORAMAC_RX_ABORT;
        MacCtx.MacState &= ~LORAMAC_TX_RUNNING;
    }
}


void LoRaMacProcess( void )
{
    uint8_t noTx = false;

    LoRaMacHandleIrqEvents( );
    LoRaMacClassBProcess( );

    // MAC proceeded a state and is ready to check
    if( MacCtx.MacFlags.Bits.MacDone == 1 )
    {
        LoRaMacEnableRequests( LORAMAC_REQUEST_HANDLING_OFF );
        LoRaMacCheckForRxAbort( );

        // An error occurs during transmitting
        if( IsRequestPending( ) > 0 )
        {
            noTx |= LoRaMacCheckForBeaconAcquisition( );
        }

        if( noTx == 0x00 )
        {
            LoRaMacHandleMlmeRequest( );
            LoRaMacHandleMcpsRequest( );
        }
        LoRaMacHandleRequestEvents( );
        LoRaMacHandleScheduleUplinkEvent( );
        LoRaMacEnableRequests( LORAMAC_REQUEST_HANDLING_ON );
    }
    LoRaMacHandleIndicationEvents( );
    if( MacCtx.RxSlot == RX_SLOT_WIN_CLASS_C )
    {
        OpenContinuousRxCWindow( );
    }
}

static void OnTxDelayedTimerEvent( void* context )
{
    TimerStop( &MacCtx.TxDelayedTimer );
    MacCtx.MacState &= ~LORAMAC_TX_DELAYED;

    // Schedule frame, allow delayed frame transmissions
    switch( ScheduleTx( true ) )
    {
        case LORAMAC_STATUS_OK:
        case LORAMAC_STATUS_DUTYCYCLE_RESTRICTED:
        {
            break;
        }
        default:
        {
            // Stop retransmission attempt
            MacCtx.McpsConfirm.Datarate = MacCtx.NvmCtx->MacParams.ChannelsDatarate;
            MacCtx.McpsConfirm.NbRetries = MacCtx.AckTimeoutRetriesCounter;
            MacCtx.McpsConfirm.Status = LORAMAC_EVENT_INFO_STATUS_TX_DR_PAYLOAD_SIZE_ERROR;
            LoRaMacConfirmQueueSetStatusCmn( LORAMAC_EVENT_INFO_STATUS_TX_DR_PAYLOAD_SIZE_ERROR );
            StopRetransmission( );
            break;
        }
    }
}

static void OnRxWindow1TimerEvent( void* context )
{
    MacCtx.RxWindow1Config.Channel = MacCtx.Channel;
    MacCtx.RxWindow1Config.DrOffset = MacCtx.NvmCtx->MacParams.Rx1DrOffset;
    MacCtx.RxWindow1Config.DownlinkDwellTime = MacCtx.NvmCtx->MacParams.DownlinkDwellTime;
    MacCtx.RxWindow1Config.RxContinuous = false;
    MacCtx.RxWindow1Config.RxSlot = RX_SLOT_WIN_1;

    RxWindowSetup( &MacCtx.RxWindowTimer1, &MacCtx.RxWindow1Config );
}

static void OnRxWindow2TimerEvent( void* context )
{
    // Check if we are processing Rx1 window.
    // If yes, we don't setup the Rx2 window.
    if( MacCtx.RxSlot == RX_SLOT_WIN_1 )
    {
        return;
    }
    MacCtx.RxWindow2Config.Channel = MacCtx.Channel;
    MacCtx.RxWindow2Config.Frequency = MacCtx.NvmCtx->MacParams.Rx2Channel.Frequency;
    MacCtx.RxWindow2Config.DownlinkDwellTime = MacCtx.NvmCtx->MacParams.DownlinkDwellTime;
    MacCtx.RxWindow2Config.RxContinuous = false;
    MacCtx.RxWindow2Config.RxSlot = RX_SLOT_WIN_2;

    RxWindowSetup( &MacCtx.RxWindowTimer2, &MacCtx.RxWindow2Config );
}

static void OnAckTimeoutTimerEvent( void* context )
{
    TimerStop( &MacCtx.AckTimeoutTimer );

    if( MacCtx.NodeAckRequested == true )
    {
        MacCtx.AckTimeoutRetry = true;
    }
    if( MacCtx.NvmCtx->DeviceClass == CLASS_C )
    {
        MacCtx.MacFlags.Bits.MacDone = 1;
    }
    if( ( MacCtx.MacCallbacks != NULL ) && ( MacCtx.MacCallbacks->MacProcessNotify != NULL ) )
    {
        MacCtx.MacCallbacks->MacProcessNotify( );
    }
}

static LoRaMacCryptoStatus_t GetFCntDown( AddressIdentifier_t addrID, FType_t fType, LoRaMacMessageData_t* macMsg, Version_t lrWanVersion,
                                          uint16_t maxFCntGap, FCntIdentifier_t* fCntID, uint32_t* currentDown )
{
    if( ( macMsg == NULL ) || ( fCntID == NULL ) ||
        ( currentDown == NULL ) )
    {
        return LORAMAC_CRYPTO_ERROR_NPE;
    }

    // Determine the frame counter identifier and choose counter from FCntList
    switch( addrID )
    {
        case UNICAST_DEV_ADDR:
            if( lrWanVersion.Fields.Minor == 1 )
            {
                if( ( fType == FRAME_TYPE_A ) || ( fType == FRAME_TYPE_D ) )
                {
                    *fCntID = A_FCNT_DOWN;
                }
                else
                {
                    *fCntID = N_FCNT_DOWN;
                }
            }
            else
            { // For LoRaWAN 1.0.X
                *fCntID = FCNT_DOWN;
            }
            break;
        case MULTICAST_0_ADDR:
            *fCntID = MC_FCNT_DOWN_0;
            break;
        case MULTICAST_1_ADDR:
            *fCntID = MC_FCNT_DOWN_1;
            break;
        case MULTICAST_2_ADDR:
            *fCntID = MC_FCNT_DOWN_2;
            break;
        case MULTICAST_3_ADDR:
            *fCntID = MC_FCNT_DOWN_3;
            break;
        default:
            return LORAMAC_CRYPTO_FAIL_FCNT_ID;
    }

    return LoRaMacCryptoGetFCntDown( *fCntID, maxFCntGap, macMsg->FHDR.FCnt, currentDown );
}

static LoRaMacStatus_t SwitchClass( DeviceClass_t deviceClass )
{
    LoRaMacStatus_t status = LORAMAC_STATUS_PARAMETER_INVALID;

    switch( MacCtx.NvmCtx->DeviceClass )
    {
        case CLASS_A:
        {
            if( deviceClass == CLASS_A )
            {
                // Revert back RxC parameters
                MacCtx.NvmCtx->MacParams.RxCChannel = MacCtx.NvmCtx->MacParams.Rx2Channel;
            }
            if( deviceClass == CLASS_B )
            {
                status = LoRaMacClassBSwitchClass( deviceClass );
                if( status == LORAMAC_STATUS_OK )
                {
                    MacCtx.NvmCtx->DeviceClass = deviceClass;
                }
            }

            if( deviceClass == CLASS_C )
            {
                MacCtx.NvmCtx->DeviceClass = deviceClass;

                MacCtx.RxWindowCConfig = MacCtx.RxWindow2Config;
                MacCtx.RxWindowCConfig.RxSlot = RX_SLOT_WIN_CLASS_C;

                for( int8_t i = 0; i < LORAMAC_MAX_MC_CTX; i++ )
                {
                    if( MacCtx.NvmCtx->MulticastChannelList[i].ChannelParams.IsEnabled == true )
                    // TODO: Check multicast channel device class.
                    {
                        MacCtx.NvmCtx->MacParams.RxCChannel.Frequency = MacCtx.NvmCtx->MulticastChannelList[i].ChannelParams.RxParams.ClassC.Frequency;
                        MacCtx.NvmCtx->MacParams.RxCChannel.Datarate = MacCtx.NvmCtx->MulticastChannelList[i].ChannelParams.RxParams.ClassC.Datarate;

                        MacCtx.RxWindowCConfig.Channel = MacCtx.Channel;
                        MacCtx.RxWindowCConfig.Frequency = MacCtx.NvmCtx->MacParams.RxCChannel.Frequency;
                        MacCtx.RxWindowCConfig.DownlinkDwellTime = MacCtx.NvmCtx->MacParams.DownlinkDwellTime;
                        MacCtx.RxWindowCConfig.RxSlot = RX_SLOT_WIN_CLASS_C_MULTICAST;
                        MacCtx.RxWindowCConfig.RxContinuous = true;
                        break;
                    }
                }

                // Set the NodeAckRequested indicator to default
                MacCtx.NodeAckRequested = false;
                // Set the radio into sleep mode in case we are still in RX mode
                Radio.Sleep( );

                OpenContinuousRxCWindow( );

                status = LORAMAC_STATUS_OK;
            }
            break;
        }
        case CLASS_B:
        {
            status = LoRaMacClassBSwitchClass( deviceClass );
            if( status == LORAMAC_STATUS_OK )
            {
                MacCtx.NvmCtx->DeviceClass = deviceClass;
            }
            break;
        }
        case CLASS_C:
        {
            if( deviceClass == CLASS_A )
            {
                MacCtx.NvmCtx->DeviceClass = deviceClass;

                // Set the radio into sleep to setup a defined state
                Radio.Sleep( );

                status = LORAMAC_STATUS_OK;
            }
            break;
        }
    }

    return status;
}

static uint8_t GetMaxAppPayloadWithoutFOptsLength( int8_t datarate )
{
    GetPhyParams_t getPhy;
    PhyParam_t phyParam;

    // Setup PHY request
    getPhy.UplinkDwellTime = MacCtx.NvmCtx->MacParams.UplinkDwellTime;
    getPhy.Datarate = datarate;
    getPhy.Attribute = PHY_MAX_PAYLOAD;
    phyParam = RegionGetPhyParam( MacCtx.NvmCtx->Region, &getPhy );

    return phyParam.Value;
}

static bool ValidatePayloadLength( uint8_t lenN, int8_t datarate, uint8_t fOptsLen )
{
    uint16_t maxN = 0;
    uint16_t payloadSize = 0;

    maxN = GetMaxAppPayloadWithoutFOptsLength( datarate );

    // Calculate the resulting payload size
    payloadSize = ( lenN + fOptsLen );

    // Validation of the application payload size
    if( ( payloadSize <= maxN ) && ( payloadSize <= LORAMAC_PHY_MAXPAYLOAD ) )
    {
        return true;
    }
    return false;
}

static void SetMlmeScheduleUplinkIndication( void )
{
    MacCtx.MacFlags.Bits.MlmeSchedUplinkInd = 1;
}

static void ProcessMacCommands( uint8_t *payload, uint8_t macIndex, uint8_t commandsSize, int8_t snr, LoRaMacRxSlot_t rxSlot )
{
    uint8_t status = 0;
    bool adrBlockFound = false;
    uint8_t macCmdPayload[2] = { 0x00, 0x00 };

    while( macIndex < commandsSize )
    {
        // Make sure to parse only complete MAC commands
        if( ( LoRaMacCommandsGetCmdSize( payload[macIndex] ) + macIndex ) > commandsSize )
        {
            return;
        }

        // Decode Frame MAC commands
        switch( payload[macIndex++] )
        {
            case SRV_MAC_LINK_CHECK_ANS:
            {
                if( LoRaMacConfirmQueueIsCmdActive( MLME_LINK_CHECK ) == true )
                {
                    LoRaMacConfirmQueueSetStatus( LORAMAC_EVENT_INFO_STATUS_OK, MLME_LINK_CHECK );
                    MacCtx.MlmeConfirm.DemodMargin = payload[macIndex++];
                    MacCtx.MlmeConfirm.NbGateways = payload[macIndex++];
                }
                break;
            }
            case SRV_MAC_LINK_ADR_REQ:
            {
                LinkAdrReqParams_t linkAdrReq;
                int8_t linkAdrDatarate = DR_0;
                int8_t linkAdrTxPower = TX_POWER_0;
                uint8_t linkAdrNbRep = 0;
                uint8_t linkAdrNbBytesParsed = 0;

                if( adrBlockFound == false )
                {
                    adrBlockFound = true;

                    // Fill parameter structure
                    linkAdrReq.Payload = &payload[macIndex - 1];
                    linkAdrReq.PayloadSize = commandsSize - ( macIndex - 1 );
                    linkAdrReq.AdrEnabled = MacCtx.NvmCtx->AdrCtrlOn;
                    linkAdrReq.UplinkDwellTime = MacCtx.NvmCtx->MacParams.UplinkDwellTime;
                    linkAdrReq.CurrentDatarate = MacCtx.NvmCtx->MacParams.ChannelsDatarate;
                    linkAdrReq.CurrentTxPower = MacCtx.NvmCtx->MacParams.ChannelsTxPower;
                    linkAdrReq.CurrentNbRep = MacCtx.NvmCtx->MacParams.ChannelsNbTrans;
                    linkAdrReq.Version = MacCtx.NvmCtx->Version;

                    // Process the ADR requests
                    status = RegionLinkAdrReq( MacCtx.NvmCtx->Region, &linkAdrReq, &linkAdrDatarate,
                                               &linkAdrTxPower, &linkAdrNbRep, &linkAdrNbBytesParsed );

                    if( ( status & 0x07 ) == 0x07 )
                    {
                        MacCtx.NvmCtx->MacParams.ChannelsDatarate = linkAdrDatarate;
                        MacCtx.NvmCtx->MacParams.ChannelsTxPower = linkAdrTxPower;
                        MacCtx.NvmCtx->MacParams.ChannelsNbTrans = linkAdrNbRep;
                    }

                    // Add the answers to the buffer
                    for( uint8_t i = 0; i < ( linkAdrNbBytesParsed / 5 ); i++ )
                    {
                        LoRaMacCommandsAddCmd( MOTE_MAC_LINK_ADR_ANS, &status, 1 );
                    }
                    // Update MAC index
                    macIndex += linkAdrNbBytesParsed - 1;
                }
                break;
            }
            case SRV_MAC_DUTY_CYCLE_REQ:
            {
                MacCtx.NvmCtx->MaxDCycle = payload[macIndex++] & 0x0F;
                MacCtx.NvmCtx->AggregatedDCycle = 1 << MacCtx.NvmCtx->MaxDCycle;
                LoRaMacCommandsAddCmd( MOTE_MAC_DUTY_CYCLE_ANS, macCmdPayload, 0 );
                break;
            }
            case SRV_MAC_RX_PARAM_SETUP_REQ:
            {
                RxParamSetupReqParams_t rxParamSetupReq;
                status = 0x07;

                rxParamSetupReq.DrOffset = ( payload[macIndex] >> 4 ) & 0x07;
                rxParamSetupReq.Datarate = payload[macIndex] & 0x0F;
                macIndex++;

                rxParamSetupReq.Frequency = ( uint32_t ) payload[macIndex++];
                rxParamSetupReq.Frequency |= ( uint32_t ) payload[macIndex++] << 8;
                rxParamSetupReq.Frequency |= ( uint32_t ) payload[macIndex++] << 16;
                rxParamSetupReq.Frequency *= 100;

                // Perform request on region
                status = RegionRxParamSetupReq( MacCtx.NvmCtx->Region, &rxParamSetupReq );

                if( ( status & 0x07 ) == 0x07 )
                {
                    MacCtx.NvmCtx->MacParams.Rx2Channel.Datarate = rxParamSetupReq.Datarate;
                    MacCtx.NvmCtx->MacParams.RxCChannel.Datarate = rxParamSetupReq.Datarate;
                    MacCtx.NvmCtx->MacParams.Rx2Channel.Frequency = rxParamSetupReq.Frequency;
                    MacCtx.NvmCtx->MacParams.RxCChannel.Frequency = rxParamSetupReq.Frequency;
                    MacCtx.NvmCtx->MacParams.Rx1DrOffset = rxParamSetupReq.DrOffset;
                }
                macCmdPayload[0] = status;
                LoRaMacCommandsAddCmd( MOTE_MAC_RX_PARAM_SETUP_ANS, macCmdPayload, 1 );
                // Setup indication to inform the application
                SetMlmeScheduleUplinkIndication( );
                break;
            }
            case SRV_MAC_DEV_STATUS_REQ:
            {
                uint8_t batteryLevel = BAT_LEVEL_NO_MEASURE;
                if( ( MacCtx.MacCallbacks != NULL ) && ( MacCtx.MacCallbacks->GetBatteryLevel != NULL ) )
                {
                    batteryLevel = MacCtx.MacCallbacks->GetBatteryLevel( );
                }
                macCmdPayload[0] = batteryLevel;
                macCmdPayload[1] = ( uint8_t )( snr & 0x3F );
                LoRaMacCommandsAddCmd( MOTE_MAC_DEV_STATUS_ANS, macCmdPayload, 2 );
                break;
            }
            case SRV_MAC_NEW_CHANNEL_REQ:
            {
                NewChannelReqParams_t newChannelReq;
                ChannelParams_t chParam;
                status = 0x03;

                newChannelReq.ChannelId = payload[macIndex++];
                newChannelReq.NewChannel = &chParam;

                chParam.Frequency = ( uint32_t ) payload[macIndex++];
                chParam.Frequency |= ( uint32_t ) payload[macIndex++] << 8;
                chParam.Frequency |= ( uint32_t ) payload[macIndex++] << 16;
                chParam.Frequency *= 100;
                chParam.Rx1Frequency = 0;
                chParam.DrRange.Value = payload[macIndex++];

                status = RegionNewChannelReq( MacCtx.NvmCtx->Region, &newChannelReq );

                macCmdPayload[0] = status;
                LoRaMacCommandsAddCmd( MOTE_MAC_NEW_CHANNEL_ANS, macCmdPayload, 1 );
                break;
            }
            case SRV_MAC_RX_TIMING_SETUP_REQ:
            {
                uint8_t delay = payload[macIndex++] & 0x0F;

                if( delay == 0 )
                {
                    delay++;
                }
                MacCtx.NvmCtx->MacParams.ReceiveDelay1 = delay * 1000;
                MacCtx.NvmCtx->MacParams.ReceiveDelay2 = MacCtx.NvmCtx->MacParams.ReceiveDelay1 + 1000;
                LoRaMacCommandsAddCmd( MOTE_MAC_RX_TIMING_SETUP_ANS, macCmdPayload, 0 );
                // Setup indication to inform the application
                SetMlmeScheduleUplinkIndication( );
                break;
            }
            case SRV_MAC_TX_PARAM_SETUP_REQ:
            {
                TxParamSetupReqParams_t txParamSetupReq;
                GetPhyParams_t getPhy;
                PhyParam_t phyParam;
                uint8_t eirpDwellTime = payload[macIndex++];

                txParamSetupReq.UplinkDwellTime = 0;
                txParamSetupReq.DownlinkDwellTime = 0;

                if( ( eirpDwellTime & 0x20 ) == 0x20 )
                {
                    txParamSetupReq.DownlinkDwellTime = 1;
                }
                if( ( eirpDwellTime & 0x10 ) == 0x10 )
                {
                    txParamSetupReq.UplinkDwellTime = 1;
                }
                txParamSetupReq.MaxEirp = eirpDwellTime & 0x0F;

                // Check the status for correctness
                if( RegionTxParamSetupReq( MacCtx.NvmCtx->Region, &txParamSetupReq ) != -1 )
                {
                    // Accept command
                    MacCtx.NvmCtx->MacParams.UplinkDwellTime = txParamSetupReq.UplinkDwellTime;
                    MacCtx.NvmCtx->MacParams.DownlinkDwellTime = txParamSetupReq.DownlinkDwellTime;
                    MacCtx.NvmCtx->MacParams.MaxEirp = LoRaMacMaxEirpTable[txParamSetupReq.MaxEirp];
                    // Update the datarate in case of the new configuration limits it
                    getPhy.Attribute = PHY_MIN_TX_DR;
                    getPhy.UplinkDwellTime = MacCtx.NvmCtx->MacParams.UplinkDwellTime;
                    phyParam = RegionGetPhyParam( MacCtx.NvmCtx->Region, &getPhy );
                    MacCtx.NvmCtx->MacParams.ChannelsDatarate = MAX( MacCtx.NvmCtx->MacParams.ChannelsDatarate, ( int8_t )phyParam.Value );

                    // Add command response
                    LoRaMacCommandsAddCmd( MOTE_MAC_TX_PARAM_SETUP_ANS, macCmdPayload, 0 );
                }
                break;
            }
            case SRV_MAC_DL_CHANNEL_REQ:
            {
                DlChannelReqParams_t dlChannelReq;
                status = 0x03;

                dlChannelReq.ChannelId = payload[macIndex++];
                dlChannelReq.Rx1Frequency = ( uint32_t ) payload[macIndex++];
                dlChannelReq.Rx1Frequency |= ( uint32_t ) payload[macIndex++] << 8;
                dlChannelReq.Rx1Frequency |= ( uint32_t ) payload[macIndex++] << 16;
                dlChannelReq.Rx1Frequency *= 100;

                status = RegionDlChannelReq( MacCtx.NvmCtx->Region, &dlChannelReq );
                macCmdPayload[0] = status;
                LoRaMacCommandsAddCmd( MOTE_MAC_DL_CHANNEL_ANS, macCmdPayload, 1 );
                // Setup indication to inform the application
                SetMlmeScheduleUplinkIndication( );
                break;
            }
            case SRV_MAC_DEVICE_TIME_ANS:
            {
                if( LoRaMacConfirmQueueIsCmdActive( MLME_DEVICE_TIME ) == true )
                {
                    LoRaMacConfirmQueueSetStatus( LORAMAC_EVENT_INFO_STATUS_OK, MLME_DEVICE_TIME );
                    SysTime_t gpsEpochTime = { 0 };
                    SysTime_t sysTime = { 0 };
                    SysTime_t sysTimeCurrent = { 0 };

                    gpsEpochTime.Seconds = ( uint32_t )payload[macIndex++];
                    gpsEpochTime.Seconds |= ( uint32_t )payload[macIndex++] << 8;
                    gpsEpochTime.Seconds |= ( uint32_t )payload[macIndex++] << 16;
                    gpsEpochTime.Seconds |= ( uint32_t )payload[macIndex++] << 24;
                    gpsEpochTime.SubSeconds = payload[macIndex++];

                    // Convert the fractional second received in ms
                    // round( pow( 0.5, 8.0 ) * 1000 ) = 3.90625
                    gpsEpochTime.SubSeconds = ( int16_t )( ( ( int32_t )gpsEpochTime.SubSeconds * 1000 ) >> 8 );

                    // Copy received GPS Epoch time into system time
                    sysTime = gpsEpochTime;
                    // Add Unix to Gps epcoh offset. The system time is based on Unix time.
                    sysTime.Seconds += UNIX_GPS_EPOCH_OFFSET;

                    // Compensate time difference between Tx Done time and now
                    sysTimeCurrent = SysTimeGet( );
                    sysTime = SysTimeAdd( sysTimeCurrent, SysTimeSub( sysTime, MacCtx.LastTxSysTime ) );

                    // Apply the new system time.
                    SysTimeSet( sysTime );
                    LoRaMacClassBDeviceTimeAns( );
                    MacCtx.McpsIndication.DeviceTimeAnsReceived = true;
                }
                break;
            }
            case SRV_MAC_PING_SLOT_INFO_ANS:
            {
                if( LoRaMacConfirmQueueIsCmdActive( MLME_PING_SLOT_INFO ) == true )
                {
                    LoRaMacConfirmQueueSetStatus( LORAMAC_EVENT_INFO_STATUS_OK, MLME_PING_SLOT_INFO );
                    // According to the specification, it is not allowed to process this answer in
                    // a ping or multicast slot
                    if( ( MacCtx.RxSlot != RX_SLOT_WIN_CLASS_B_PING_SLOT ) && ( MacCtx.RxSlot != RX_SLOT_WIN_CLASS_B_MULTICAST_SLOT ) )
                    {
                        LoRaMacClassBPingSlotInfoAns( );
                    }
                }
                break;
            }
            case SRV_MAC_PING_SLOT_CHANNEL_REQ:
            {
                uint8_t status = 0x03;
                uint32_t frequency = 0;
                uint8_t datarate;

                frequency = ( uint32_t )payload[macIndex++];
                frequency |= ( uint32_t )payload[macIndex++] << 8;
                frequency |= ( uint32_t )payload[macIndex++] << 16;
                frequency *= 100;
                datarate = payload[macIndex++] & 0x0F;

                status = LoRaMacClassBPingSlotChannelReq( datarate, frequency );
                macCmdPayload[0] = status;
                LoRaMacCommandsAddCmd( MOTE_MAC_PING_SLOT_FREQ_ANS, macCmdPayload, 1 );
                break;
            }
            case SRV_MAC_BEACON_TIMING_ANS:
            {
                if( LoRaMacConfirmQueueIsCmdActive( MLME_BEACON_TIMING ) == true )
                {
                    LoRaMacConfirmQueueSetStatus( LORAMAC_EVENT_INFO_STATUS_OK, MLME_BEACON_TIMING );
                    uint16_t beaconTimingDelay = 0;
                    uint8_t beaconTimingChannel = 0;

                    beaconTimingDelay = ( uint16_t )payload[macIndex++];
                    beaconTimingDelay |= ( uint16_t )payload[macIndex++] << 8;
                    beaconTimingChannel = payload[macIndex++];

                    LoRaMacClassBBeaconTimingAns( beaconTimingDelay, beaconTimingChannel, RxDoneParams.LastRxDone );
                }
                break;
            }
            case SRV_MAC_BEACON_FREQ_REQ:
                {
                    uint32_t frequency = 0;

                    frequency = ( uint32_t )payload[macIndex++];
                    frequency |= ( uint32_t )payload[macIndex++] << 8;
                    frequency |= ( uint32_t )payload[macIndex++] << 16;
                    frequency *= 100;

                    if( LoRaMacClassBBeaconFreqReq( frequency ) == true )
                    {
                        macCmdPayload[0] = 1;
                    }
                    else
                    {
                        macCmdPayload[0] = 0;
                    }
                    LoRaMacCommandsAddCmd( MOTE_MAC_BEACON_FREQ_ANS, macCmdPayload, 1 );
                }
                break;
            default:
                // Unknown command. ABORT MAC commands processing
                return;
        }
    }
}

LoRaMacStatus_t Send( LoRaMacHeader_t* macHdr, uint8_t fPort, void* fBuffer, uint16_t fBufferSize )
{
    LoRaMacFrameCtrl_t fCtrl;
    LoRaMacStatus_t status = LORAMAC_STATUS_PARAMETER_INVALID;
    int8_t datarate = MacCtx.NvmCtx->MacParams.ChannelsDatarate;
    int8_t txPower = MacCtx.NvmCtx->MacParams.ChannelsTxPower;
    uint32_t adrAckCounter = MacCtx.NvmCtx->AdrAckCounter;
    CalcNextAdrParams_t adrNext;

    // Check if we are joined
    if( MacCtx.NvmCtx->NetworkActivation == ACTIVATION_TYPE_NONE )
    {
        return LORAMAC_STATUS_NO_NETWORK_JOINED;
    }
    if( MacCtx.NvmCtx->MaxDCycle == 0 )
    {
        MacCtx.NvmCtx->AggregatedTimeOff = 0;
    }

    fCtrl.Value = 0;
    fCtrl.Bits.FOptsLen      = 0;
    fCtrl.Bits.Adr           = MacCtx.NvmCtx->AdrCtrlOn;

    // Check class b
    if( MacCtx.NvmCtx->DeviceClass == CLASS_B )
    {
        fCtrl.Bits.FPending      = 1;
    }
    else
    {
        fCtrl.Bits.FPending      = 0;
    }

    // Check server ack
    if( MacCtx.NvmCtx->SrvAckRequested == true )
    {
        fCtrl.Bits.Ack = 1;
    }

    // ADR next request
    adrNext.Version = MacCtx.NvmCtx->Version;
    adrNext.UpdateChanMask = true;
    adrNext.AdrEnabled = fCtrl.Bits.Adr;
    adrNext.AdrAckCounter = MacCtx.NvmCtx->AdrAckCounter;
    adrNext.AdrAckLimit = MacCtx.AdrAckLimit;
    adrNext.AdrAckDelay = MacCtx.AdrAckDelay;
    adrNext.Datarate = MacCtx.NvmCtx->MacParams.ChannelsDatarate;
    adrNext.TxPower = MacCtx.NvmCtx->MacParams.ChannelsTxPower;
    adrNext.UplinkDwellTime = MacCtx.NvmCtx->MacParams.UplinkDwellTime;
    adrNext.Region = MacCtx.NvmCtx->Region;

    fCtrl.Bits.AdrAckReq = LoRaMacAdrCalcNext( &adrNext, &MacCtx.NvmCtx->MacParams.ChannelsDatarate,
                                               &MacCtx.NvmCtx->MacParams.ChannelsTxPower, &adrAckCounter );

    // Prepare the frame
    status = PrepareFrame( macHdr, &fCtrl, fPort, fBuffer, fBufferSize );

    // Validate status
    if( ( status == LORAMAC_STATUS_OK ) || ( status == LORAMAC_STATUS_SKIPPED_APP_DATA ) )
    {
        // Schedule frame, do not allow delayed transmissions
        status = ScheduleTx( false );
    }

    // Post processing
    if( status != LORAMAC_STATUS_OK )
    {
        // Bad case - restore
        // Store local variables
        MacCtx.NvmCtx->MacParams.ChannelsDatarate = datarate;
        MacCtx.NvmCtx->MacParams.ChannelsTxPower = txPower;
    }
    else
    {
        // Good case
        MacCtx.NvmCtx->SrvAckRequested = false;
        MacCtx.NvmCtx->AdrAckCounter = adrAckCounter;
        // Remove all none sticky MAC commands
        if( LoRaMacCommandsRemoveNoneStickyCmds( ) != LORAMAC_COMMANDS_SUCCESS )
        {
            return LORAMAC_STATUS_MAC_COMMAD_ERROR;
        }
    }
    return status;
}

LoRaMacStatus_t SendReJoinReq( JoinReqIdentifier_t joinReqType )
{
    LoRaMacStatus_t status = LORAMAC_STATUS_OK;
    LoRaMacHeader_t macHdr;
    macHdr.Value = 0;
    bool allowDelayedTx = true;

    // Setup join/rejoin message
    switch( joinReqType )
    {
        case JOIN_REQ:
        {
            SwitchClass( CLASS_A );

            MacCtx.TxMsg.Type = LORAMAC_MSG_TYPE_JOIN_REQUEST;
            MacCtx.TxMsg.Message.JoinReq.Buffer = MacCtx.PktBuffer;
            MacCtx.TxMsg.Message.JoinReq.BufSize = LORAMAC_PHY_MAXPAYLOAD;

            macHdr.Bits.MType = FRAME_TYPE_JOIN_REQ;
            MacCtx.TxMsg.Message.JoinReq.MHDR.Value = macHdr.Value;

            memcpy1( MacCtx.TxMsg.Message.JoinReq.JoinEUI, SecureElementGetJoinEui( ), LORAMAC_JOIN_EUI_FIELD_SIZE );
            memcpy1( MacCtx.TxMsg.Message.JoinReq.DevEUI, SecureElementGetDevEui( ), LORAMAC_DEV_EUI_FIELD_SIZE );

            allowDelayedTx = false;

            break;
        }
        default:
            status = LORAMAC_STATUS_SERVICE_UNKNOWN;
            break;
    }

    // Schedule frame
    status = ScheduleTx( allowDelayedTx );
    return status;
}

static LoRaMacStatus_t CheckForClassBCollision( void )
{
    if( LoRaMacClassBIsBeaconExpected( ) == true )
    {
        return LORAMAC_STATUS_BUSY_BEACON_RESERVED_TIME;
    }

    if( MacCtx.NvmCtx->DeviceClass == CLASS_B )
    {
        if( LoRaMacClassBIsPingExpected( ) == true )
        {
            return LORAMAC_STATUS_BUSY_PING_SLOT_WINDOW_TIME;
        }
        else if( LoRaMacClassBIsMulticastExpected( ) == true )
        {
            return LORAMAC_STATUS_BUSY_PING_SLOT_WINDOW_TIME;
        }
    }
    return LORAMAC_STATUS_OK;
}

static void ComputeRxWindowParameters( void )
{
    // Compute Rx1 windows parameters
    RegionComputeRxWindowParameters( MacCtx.NvmCtx->Region,
                                     RegionApplyDrOffset( MacCtx.NvmCtx->Region,
                                                          MacCtx.NvmCtx->MacParams.DownlinkDwellTime,
                                                          MacCtx.NvmCtx->MacParams.ChannelsDatarate,
                                                          MacCtx.NvmCtx->MacParams.Rx1DrOffset ),
                                     MacCtx.NvmCtx->MacParams.MinRxSymbols,
                                     MacCtx.NvmCtx->MacParams.SystemMaxRxError,
                                     &MacCtx.RxWindow1Config );
    // Compute Rx2 windows parameters
    RegionComputeRxWindowParameters( MacCtx.NvmCtx->Region,
                                     MacCtx.NvmCtx->MacParams.Rx2Channel.Datarate,
                                     MacCtx.NvmCtx->MacParams.MinRxSymbols,
                                     MacCtx.NvmCtx->MacParams.SystemMaxRxError,
                                     &MacCtx.RxWindow2Config );

    // Default setup, in case the device joined
    MacCtx.RxWindow1Delay = MacCtx.NvmCtx->MacParams.ReceiveDelay1 + MacCtx.RxWindow1Config.WindowOffset;
    MacCtx.RxWindow2Delay = MacCtx.NvmCtx->MacParams.ReceiveDelay2 + MacCtx.RxWindow2Config.WindowOffset;

    if( MacCtx.NvmCtx->NetworkActivation == ACTIVATION_TYPE_NONE )
    {
        MacCtx.RxWindow1Delay = MacCtx.NvmCtx->MacParams.JoinAcceptDelay1 + MacCtx.RxWindow1Config.WindowOffset;
        MacCtx.RxWindow2Delay = MacCtx.NvmCtx->MacParams.JoinAcceptDelay2 + MacCtx.RxWindow2Config.WindowOffset;
    }
}

static LoRaMacStatus_t VerifyTxFrame( void )
{
    size_t macCmdsSize = 0;

    if( MacCtx.NvmCtx->NetworkActivation != ACTIVATION_TYPE_NONE )
    {
        if( LoRaMacCommandsGetSizeSerializedCmds( &macCmdsSize ) != LORAMAC_COMMANDS_SUCCESS )
        {
            return LORAMAC_STATUS_MAC_COMMAD_ERROR;
        }

        if( ValidatePayloadLength( MacCtx.AppDataSize, MacCtx.NvmCtx->MacParams.ChannelsDatarate, macCmdsSize ) == false )
        {
            return LORAMAC_STATUS_LENGTH_ERROR;
        }
    }
    return LORAMAC_STATUS_OK;
}

static LoRaMacStatus_t SerializeTxFrame( void )
{
    LoRaMacSerializerStatus_t serializeStatus;

    switch( MacCtx.TxMsg.Type )
    {
        case LORAMAC_MSG_TYPE_JOIN_REQUEST:
            serializeStatus = LoRaMacSerializerJoinRequest( &MacCtx.TxMsg.Message.JoinReq );
            if( LORAMAC_SERIALIZER_SUCCESS != serializeStatus )
            {
                return LORAMAC_STATUS_CRYPTO_ERROR;
            }
            MacCtx.PktBufferLen = MacCtx.TxMsg.Message.JoinReq.BufSize;
            break;
        case LORAMAC_MSG_TYPE_DATA:
            serializeStatus = LoRaMacSerializerData( &MacCtx.TxMsg.Message.Data );
            if( LORAMAC_SERIALIZER_SUCCESS != serializeStatus )
            {
                return LORAMAC_STATUS_CRYPTO_ERROR;
            }
            MacCtx.PktBufferLen = MacCtx.TxMsg.Message.Data.BufSize;
            break;
        case LORAMAC_MSG_TYPE_JOIN_ACCEPT:
        case LORAMAC_MSG_TYPE_UNDEF:
        default:
            return LORAMAC_STATUS_PARAMETER_INVALID;
    }
    return LORAMAC_STATUS_OK;
}

static LoRaMacStatus_t ScheduleTx( bool allowDelayedTx )
{
    LoRaMacStatus_t status = LORAMAC_STATUS_PARAMETER_INVALID;
    NextChanParams_t nextChan;

    // Check class b collisions
    status = CheckForClassBCollision( );
    if( status != LORAMAC_STATUS_OK )
    {
        return status;
    }

    // Update back-off
    CalculateBackOff( );

    // Serialize frame
    status = SerializeTxFrame( );
    if( status != LORAMAC_STATUS_OK )
    {
        return status;
    }

    nextChan.AggrTimeOff = MacCtx.NvmCtx->AggregatedTimeOff;
    nextChan.Datarate = MacCtx.NvmCtx->MacParams.ChannelsDatarate;
    nextChan.DutyCycleEnabled = MacCtx.NvmCtx->DutyCycleOn;
    nextChan.ElapsedTimeSinceStartUp = SysTimeSub( SysTimeGetMcuTime( ), MacCtx.NvmCtx->InitializationTime );
    nextChan.LastAggrTx = MacCtx.NvmCtx->LastTxDoneTime;
    nextChan.LastTxIsJoinRequest = false;
    nextChan.Joined = true;
    nextChan.PktLen = MacCtx.PktBufferLen;

    // Setup the parameters based on the join status
    if( MacCtx.NvmCtx->NetworkActivation == ACTIVATION_TYPE_NONE )
    {
        nextChan.LastTxIsJoinRequest = true;
        nextChan.Joined = false;
    }

    // Select channel
    status = RegionNextChannel( MacCtx.NvmCtx->Region, &nextChan, &MacCtx.Channel, &MacCtx.DutyCycleWaitTime, &MacCtx.NvmCtx->AggregatedTimeOff );

    if( status != LORAMAC_STATUS_OK )
    {
        if( ( status == LORAMAC_STATUS_DUTYCYCLE_RESTRICTED ) &&
            ( allowDelayedTx == true ) )
        {
            // Allow delayed transmissions. We have to allow it in case
            // the MAC must retransmit a frame with the frame repetitions
            if( MacCtx.DutyCycleWaitTime != 0 )
            {// Send later - prepare timer
                MacCtx.MacState |= LORAMAC_TX_DELAYED;
                TimerSetValue( &MacCtx.TxDelayedTimer, MacCtx.DutyCycleWaitTime );
                TimerStart( &MacCtx.TxDelayedTimer );
            }
            return LORAMAC_STATUS_OK;
        }
        else
        {// State where the MAC cannot send a frame
            return status;
        }
    }

    // Compute window parameters, offsets, rx symbols, system errors etc.
    ComputeRxWindowParameters( );

    // Verify TX frame
    status = VerifyTxFrame( );
    if( status != LORAMAC_STATUS_OK )
    {
        return status;
    }

    // Try to send now
    return SendFrameOnChannel( MacCtx.Channel );
}

static LoRaMacStatus_t SecureFrame( uint8_t txDr, uint8_t txCh )
{
    LoRaMacCryptoStatus_t macCryptoStatus = LORAMAC_CRYPTO_ERROR;
    uint32_t fCntUp = 0;

    switch( MacCtx.TxMsg.Type )
    {
        case LORAMAC_MSG_TYPE_JOIN_REQUEST:
            macCryptoStatus = LoRaMacCryptoPrepareJoinRequest( &MacCtx.TxMsg.Message.JoinReq );
            if( LORAMAC_CRYPTO_SUCCESS != macCryptoStatus )
            {
                return LORAMAC_STATUS_CRYPTO_ERROR;
            }
            MacCtx.PktBufferLen = MacCtx.TxMsg.Message.JoinReq.BufSize;
            break;
        case LORAMAC_MSG_TYPE_DATA:

            if( LORAMAC_CRYPTO_SUCCESS != LoRaMacCryptoGetFCntUp( &fCntUp ) )
            {
                return LORAMAC_STATUS_FCNT_HANDLER_ERROR;
            }

            if( ( MacCtx.ChannelsNbTransCounter >= 1 ) || ( MacCtx.AckTimeoutRetriesCounter > 1 ) )
            {
                fCntUp -= 1;
            }

            macCryptoStatus = LoRaMacCryptoSecureMessage( fCntUp, txDr, txCh, &MacCtx.TxMsg.Message.Data );
            if( LORAMAC_CRYPTO_SUCCESS != macCryptoStatus )
            {
                return LORAMAC_STATUS_CRYPTO_ERROR;
            }
            MacCtx.PktBufferLen = MacCtx.TxMsg.Message.Data.BufSize;
            break;
        case LORAMAC_MSG_TYPE_JOIN_ACCEPT:
        case LORAMAC_MSG_TYPE_UNDEF:
        default:
            return LORAMAC_STATUS_PARAMETER_INVALID;
    }
    return LORAMAC_STATUS_OK;
}

static void CalculateBackOff( void )
{
    // Make sure that the calculation of the backoff time for the aggregated time off will only be done in
    // case the value is zero. It will be set to zero in the function RegionNextChannel.
    if( MacCtx.NvmCtx->AggregatedTimeOff == 0 )
    {
        // Update aggregated time-off. This must be an assignment and no incremental
        // update as we do only calculate the time-off based on the last transmission
        MacCtx.NvmCtx->AggregatedTimeOff = ( MacCtx.TxTimeOnAir * MacCtx.NvmCtx->AggregatedDCycle - MacCtx.TxTimeOnAir );
    }
}

static void RemoveMacCommands( LoRaMacRxSlot_t rxSlot, LoRaMacFrameCtrl_t fCtrl, Mcps_t request )
{
    if( rxSlot == RX_SLOT_WIN_1 || rxSlot == RX_SLOT_WIN_2  )
    {
        // Remove all sticky MAC commands answers since we can assume
        // that they have been received by the server.
        if( request == MCPS_CONFIRMED )
        {
            if( fCtrl.Bits.Ack == 1 )
            {  // For confirmed uplinks only if we have received an ACK.
                LoRaMacCommandsRemoveStickyAnsCmds( );
            }
        }
        else
        {
            LoRaMacCommandsRemoveStickyAnsCmds( );
        }
    }
}


static void ResetMacParameters( void )
{
    MacCtx.NvmCtx->NetworkActivation = ACTIVATION_TYPE_NONE;

    // ADR counter
    MacCtx.NvmCtx->AdrAckCounter = 0;

    MacCtx.ChannelsNbTransCounter = 0;
    MacCtx.AckTimeoutRetries = 1;
    MacCtx.AckTimeoutRetriesCounter = 1;
    MacCtx.AckTimeoutRetry = false;

    MacCtx.NvmCtx->MaxDCycle = 0;
    MacCtx.NvmCtx->AggregatedDCycle = 1;

    MacCtx.NvmCtx->MacParams.ChannelsTxPower = MacCtx.NvmCtx->MacParamsDefaults.ChannelsTxPower;
    MacCtx.NvmCtx->MacParams.ChannelsDatarate = MacCtx.NvmCtx->MacParamsDefaults.ChannelsDatarate;
    MacCtx.NvmCtx->MacParams.Rx1DrOffset = MacCtx.NvmCtx->MacParamsDefaults.Rx1DrOffset;
    MacCtx.NvmCtx->MacParams.Rx2Channel = MacCtx.NvmCtx->MacParamsDefaults.Rx2Channel;
    MacCtx.NvmCtx->MacParams.RxCChannel = MacCtx.NvmCtx->MacParamsDefaults.RxCChannel;
    MacCtx.NvmCtx->MacParams.UplinkDwellTime = MacCtx.NvmCtx->MacParamsDefaults.UplinkDwellTime;
    MacCtx.NvmCtx->MacParams.DownlinkDwellTime = MacCtx.NvmCtx->MacParamsDefaults.DownlinkDwellTime;
    MacCtx.NvmCtx->MacParams.MaxEirp = MacCtx.NvmCtx->MacParamsDefaults.MaxEirp;
    MacCtx.NvmCtx->MacParams.AntennaGain = MacCtx.NvmCtx->MacParamsDefaults.AntennaGain;

    MacCtx.NodeAckRequested = false;
    MacCtx.NvmCtx->SrvAckRequested = false;

    // Reset to application defaults
    InitDefaultsParams_t params;
    params.Type = INIT_TYPE_INIT;
    params.NvmCtx = NULL;
    RegionInitDefaults( MacCtx.NvmCtx->Region, &params );

    // Initialize channel index.
    MacCtx.Channel = 0;

    // Initialize Rx2 config parameters.
    MacCtx.RxWindow2Config.Channel = MacCtx.Channel;
    MacCtx.RxWindow2Config.Frequency = MacCtx.NvmCtx->MacParams.Rx2Channel.Frequency;
    MacCtx.RxWindow2Config.DownlinkDwellTime = MacCtx.NvmCtx->MacParams.DownlinkDwellTime;
    MacCtx.RxWindow2Config.RxContinuous = false;
    MacCtx.RxWindow2Config.RxSlot = RX_SLOT_WIN_2;

    // Initialize RxC config parameters.
    MacCtx.RxWindowCConfig = MacCtx.RxWindow2Config;
    MacCtx.RxWindowCConfig.RxContinuous = true;
    MacCtx.RxWindowCConfig.RxSlot = RX_SLOT_WIN_CLASS_C;

}

/*!
 * \brief Initializes and opens the reception window
 *
 * \param [IN] rxTimer  Window timer to be topped.
 * \param [IN] rxConfig Window parameters to be setup
 */
static void RxWindowSetup( TimerEvent_t* rxTimer, RxConfigParams_t* rxConfig )
{
    TimerStop( rxTimer );

    // Ensure the radio is Idle
    Radio.Standby( );

    if( RegionRxConfig( MacCtx.NvmCtx->Region, rxConfig, ( int8_t* )&MacCtx.McpsIndication.RxDatarate ) == true )
    {
        Radio.Rx( MacCtx.NvmCtx->MacParams.MaxRxWindow );
        MacCtx.RxSlot = rxConfig->RxSlot;
    }
}

static void OpenContinuousRxCWindow( void )
{
    // Compute RxC windows parameters
    RegionComputeRxWindowParameters( MacCtx.NvmCtx->Region,
                                     MacCtx.NvmCtx->MacParams.RxCChannel.Datarate,
                                     MacCtx.NvmCtx->MacParams.MinRxSymbols,
                                     MacCtx.NvmCtx->MacParams.SystemMaxRxError,
                                     &MacCtx.RxWindowCConfig );

    MacCtx.RxWindowCConfig.RxSlot = RX_SLOT_WIN_CLASS_C;
    // Setup continuous listening
    MacCtx.RxWindowCConfig.RxContinuous = true;

    // At this point the Radio should be idle.
    // Thus, there is no need to set the radio in standby mode.
    if( RegionRxConfig( MacCtx.NvmCtx->Region, &MacCtx.RxWindowCConfig, ( int8_t* )&MacCtx.McpsIndication.RxDatarate ) == true )
    {
        Radio.Rx( 0 ); // Continuous mode
        MacCtx.RxSlot = MacCtx.RxWindowCConfig.RxSlot;
    }
}

LoRaMacStatus_t PrepareFrame( LoRaMacHeader_t* macHdr, LoRaMacFrameCtrl_t* fCtrl, uint8_t fPort, void* fBuffer, uint16_t fBufferSize )
{
    MacCtx.PktBufferLen = 0;
    MacCtx.NodeAckRequested = false;
    uint32_t fCntUp = 0;
    size_t macCmdsSize = 0;
    uint8_t availableSize = 0;

    if( fBuffer == NULL )
    {
        fBufferSize = 0;
    }

    memcpy1( MacCtx.AppData, ( uint8_t* ) fBuffer, fBufferSize );
    MacCtx.AppDataSize = fBufferSize;
    MacCtx.PktBuffer[0] = macHdr->Value;

    switch( macHdr->Bits.MType )
    {
        case FRAME_TYPE_DATA_CONFIRMED_UP:
            MacCtx.NodeAckRequested = true;
            // Intentional fall through
        case FRAME_TYPE_DATA_UNCONFIRMED_UP:
            MacCtx.TxMsg.Type = LORAMAC_MSG_TYPE_DATA;
            MacCtx.TxMsg.Message.Data.Buffer = MacCtx.PktBuffer;
            MacCtx.TxMsg.Message.Data.BufSize = LORAMAC_PHY_MAXPAYLOAD;
            MacCtx.TxMsg.Message.Data.MHDR.Value = macHdr->Value;
            MacCtx.TxMsg.Message.Data.FPort = fPort;
            MacCtx.TxMsg.Message.Data.FHDR.DevAddr = MacCtx.NvmCtx->DevAddr;
            MacCtx.TxMsg.Message.Data.FHDR.FCtrl.Value = fCtrl->Value;
            MacCtx.TxMsg.Message.Data.FRMPayloadSize = MacCtx.AppDataSize;
            MacCtx.TxMsg.Message.Data.FRMPayload = MacCtx.AppData;

            if( LORAMAC_CRYPTO_SUCCESS != LoRaMacCryptoGetFCntUp( &fCntUp ) )
            {
                return LORAMAC_STATUS_FCNT_HANDLER_ERROR;
            }
            MacCtx.TxMsg.Message.Data.FHDR.FCnt = ( uint16_t )fCntUp;

            // Reset confirm parameters
            MacCtx.McpsConfirm.NbRetries = 0;
            MacCtx.McpsConfirm.AckReceived = false;
            MacCtx.McpsConfirm.UpLinkCounter = fCntUp;

            // Handle the MAC commands if there are any available
            if( LoRaMacCommandsGetSizeSerializedCmds( &macCmdsSize ) != LORAMAC_COMMANDS_SUCCESS )
            {
                return LORAMAC_STATUS_MAC_COMMAD_ERROR;
            }

            if( macCmdsSize > 0 )
            {
                availableSize = GetMaxAppPayloadWithoutFOptsLength( MacCtx.NvmCtx->MacParams.ChannelsDatarate );

                // There is application payload available and the MAC commands fit into FOpts field.
                if( ( MacCtx.AppDataSize > 0 ) && ( macCmdsSize <= LORA_MAC_COMMAND_MAX_FOPTS_LENGTH ) )
                {
                    if( LoRaMacCommandsSerializeCmds( LORA_MAC_COMMAND_MAX_FOPTS_LENGTH, &macCmdsSize, MacCtx.TxMsg.Message.Data.FHDR.FOpts ) != LORAMAC_COMMANDS_SUCCESS )
                    {
                        return LORAMAC_STATUS_MAC_COMMAD_ERROR;
                    }
                    fCtrl->Bits.FOptsLen = macCmdsSize;
                    // Update FCtrl field with new value of FOptionsLength
                    MacCtx.TxMsg.Message.Data.FHDR.FCtrl.Value = fCtrl->Value;
                }
                // There is application payload available but the MAC commands does NOT fit into FOpts field.
                else if( ( MacCtx.AppDataSize > 0 ) && ( macCmdsSize > LORA_MAC_COMMAND_MAX_FOPTS_LENGTH ) )
                {

                    if( LoRaMacCommandsSerializeCmds( availableSize, &macCmdsSize, MacCtx.NvmCtx->MacCommandsBuffer ) != LORAMAC_COMMANDS_SUCCESS )
                    {
                        return LORAMAC_STATUS_MAC_COMMAD_ERROR;
                    }
                    return LORAMAC_STATUS_SKIPPED_APP_DATA;
                }
                // No application payload available therefore add all mac commands to the FRMPayload.
                else
                {
                    if( LoRaMacCommandsSerializeCmds( availableSize, &macCmdsSize, MacCtx.NvmCtx->MacCommandsBuffer ) != LORAMAC_COMMANDS_SUCCESS )
                    {
                        return LORAMAC_STATUS_MAC_COMMAD_ERROR;
                    }
                    // Force FPort to be zero
                    MacCtx.TxMsg.Message.Data.FPort = 0;

                    MacCtx.TxMsg.Message.Data.FRMPayload = MacCtx.NvmCtx->MacCommandsBuffer;
                    MacCtx.TxMsg.Message.Data.FRMPayloadSize = macCmdsSize;
                }
            }

            break;
        case FRAME_TYPE_PROPRIETARY:
            if( ( fBuffer != NULL ) && ( MacCtx.AppDataSize > 0 ) )
            {
                memcpy1( MacCtx.PktBuffer + LORAMAC_MHDR_FIELD_SIZE, ( uint8_t* ) fBuffer, MacCtx.AppDataSize );
                MacCtx.PktBufferLen = LORAMAC_MHDR_FIELD_SIZE + MacCtx.AppDataSize;
            }
            break;
        default:
            return LORAMAC_STATUS_SERVICE_UNKNOWN;
    }

    return LORAMAC_STATUS_OK;
}

LoRaMacStatus_t SendFrameOnChannel( uint8_t channel )
{
    LoRaMacStatus_t status = LORAMAC_STATUS_PARAMETER_INVALID;
    TxConfigParams_t txConfig;
    int8_t txPower = 0;

    txConfig.Channel = channel;
    txConfig.Datarate = MacCtx.NvmCtx->MacParams.ChannelsDatarate;
    txConfig.TxPower = MacCtx.NvmCtx->MacParams.ChannelsTxPower;
    txConfig.MaxEirp = MacCtx.NvmCtx->MacParams.MaxEirp;
    txConfig.AntennaGain = MacCtx.NvmCtx->MacParams.AntennaGain;
    txConfig.PktLen = MacCtx.PktBufferLen;

    RegionTxConfig( MacCtx.NvmCtx->Region, &txConfig, &txPower, &MacCtx.TxTimeOnAir );

    MacCtx.McpsConfirm.Status = LORAMAC_EVENT_INFO_STATUS_ERROR;
    MacCtx.McpsConfirm.Datarate = MacCtx.NvmCtx->MacParams.ChannelsDatarate;
    MacCtx.McpsConfirm.TxPower = txPower;
    MacCtx.McpsConfirm.Channel = channel;

    // Store the time on air
    MacCtx.McpsConfirm.TxTimeOnAir = MacCtx.TxTimeOnAir;
    MacCtx.MlmeConfirm.TxTimeOnAir = MacCtx.TxTimeOnAir;

    if( LoRaMacClassBIsBeaconModeActive( ) == true )
    {
        // Currently, the Time-On-Air can only be computed when the radio is configured with
        // the TX configuration
        TimerTime_t collisionTime = LoRaMacClassBIsUplinkCollision( MacCtx.TxTimeOnAir );

        if( collisionTime > 0 )
        {
            return LORAMAC_STATUS_BUSY_UPLINK_COLLISION;
        }
    }

    if( MacCtx.NvmCtx->DeviceClass == CLASS_B )
    {
        // Stop slots for class b
        LoRaMacClassBStopRxSlots( );
    }

    LoRaMacClassBHaltBeaconing( );

    // Secure frame
    status = SecureFrame( MacCtx.NvmCtx->MacParams.ChannelsDatarate, MacCtx.Channel );
    if( status != LORAMAC_STATUS_OK )
    {
        return status;
    }

    MacCtx.MacState |= LORAMAC_TX_RUNNING;
    if( MacCtx.NodeAckRequested == false )
    {
        MacCtx.ChannelsNbTransCounter++;
    }

    // Send now
    Radio.Send( MacCtx.PktBuffer, MacCtx.PktBufferLen );

    return LORAMAC_STATUS_OK;
}

LoRaMacStatus_t SetTxContinuousWave( uint16_t timeout )
{
    ContinuousWaveParams_t continuousWave;

    continuousWave.Channel = MacCtx.Channel;
    continuousWave.Datarate = MacCtx.NvmCtx->MacParams.ChannelsDatarate;
    continuousWave.TxPower = MacCtx.NvmCtx->MacParams.ChannelsTxPower;
    continuousWave.MaxEirp = MacCtx.NvmCtx->MacParams.MaxEirp;
    continuousWave.AntennaGain = MacCtx.NvmCtx->MacParams.AntennaGain;
    continuousWave.Timeout = timeout;

    RegionSetContinuousWave( MacCtx.NvmCtx->Region, &continuousWave );

    MacCtx.MacState |= LORAMAC_TX_RUNNING;

    return LORAMAC_STATUS_OK;
}

LoRaMacStatus_t SetTxContinuousWave1( uint16_t timeout, uint32_t frequency, uint8_t power )
{
    Radio.SetTxContinuousWave( frequency, power, timeout );

    MacCtx.MacState |= LORAMAC_TX_RUNNING;

    return LORAMAC_STATUS_OK;
}

LoRaMacCtxs_t* GetCtxs( void )
{
    Contexts.MacNvmCtx = &NvmMacCtx;
    Contexts.MacNvmCtxSize = sizeof( NvmMacCtx );
    Contexts.CryptoNvmCtx = LoRaMacCryptoGetNvmCtx( &Contexts.CryptoNvmCtxSize );
    GetNvmCtxParams_t params ={ 0 };
    Contexts.RegionNvmCtx = RegionGetNvmCtx( MacCtx.NvmCtx->Region, &params );
    Contexts.RegionNvmCtxSize = params.nvmCtxSize;
    Contexts.SecureElementNvmCtx = SecureElementGetNvmCtx( &Contexts.SecureElementNvmCtxSize );
    Contexts.CommandsNvmCtx = LoRaMacCommandsGetNvmCtx( &Contexts.CommandsNvmCtxSize );
    Contexts.ClassBNvmCtx = LoRaMacClassBGetNvmCtx( &Contexts.ClassBNvmCtxSize );
    Contexts.ConfirmQueueNvmCtx = LoRaMacConfirmQueueGetNvmCtx( &Contexts.ConfirmQueueNvmCtxSize );
    return &Contexts;
}

LoRaMacStatus_t RestoreCtxs( LoRaMacCtxs_t* contexts )
{
    if( contexts == NULL )
    {
        return LORAMAC_STATUS_PARAMETER_INVALID;
    }
    if( MacCtx.MacState != LORAMAC_STOPPED )
    {
        return LORAMAC_STATUS_BUSY;
    }

    if( contexts->MacNvmCtx != NULL )
    {
        memcpy1( ( uint8_t* ) &NvmMacCtx, ( uint8_t* ) contexts->MacNvmCtx, contexts->MacNvmCtxSize );
    }

    InitDefaultsParams_t params;
    params.Type = INIT_TYPE_RESTORE_CTX;
    params.NvmCtx = contexts->RegionNvmCtx;
    RegionInitDefaults( MacCtx.NvmCtx->Region, &params );

    // Initialize RxC config parameters.
    MacCtx.RxWindowCConfig.Channel = MacCtx.Channel;
    MacCtx.RxWindowCConfig.Frequency = MacCtx.NvmCtx->MacParams.RxCChannel.Frequency;
    MacCtx.RxWindowCConfig.DownlinkDwellTime = MacCtx.NvmCtx->MacParams.DownlinkDwellTime;
    MacCtx.RxWindowCConfig.RxContinuous = true;
    MacCtx.RxWindowCConfig.RxSlot = RX_SLOT_WIN_CLASS_C;

    if( SecureElementRestoreNvmCtx( contexts->SecureElementNvmCtx ) != SECURE_ELEMENT_SUCCESS )
    {
        return LORAMAC_STATUS_CRYPTO_ERROR;
    }

    if( LoRaMacCryptoRestoreNvmCtx( contexts->CryptoNvmCtx ) != LORAMAC_CRYPTO_SUCCESS )
    {
        return LORAMAC_STATUS_CRYPTO_ERROR;
    }

    if( LoRaMacCommandsRestoreNvmCtx( contexts->CommandsNvmCtx ) != LORAMAC_COMMANDS_SUCCESS )
    {
        return LORAMAC_STATUS_MAC_COMMAD_ERROR;
    }

    if( LoRaMacClassBRestoreNvmCtx( contexts->ClassBNvmCtx ) != true )
    {
        return LORAMAC_STATUS_CLASS_B_ERROR;
    }

    if( LoRaMacConfirmQueueRestoreNvmCtx( contexts->ConfirmQueueNvmCtx ) != true )
    {
        return LORAMAC_STATUS_CONFIRM_QUEUE_ERROR;
    }

    return LORAMAC_STATUS_OK;
}

LoRaMacStatus_t DetermineFrameType( LoRaMacMessageData_t* macMsg, FType_t* fType )
{
    if( ( macMsg == NULL ) || ( fType == NULL ) )
    {
        return LORAMAC_STATUS_PARAMETER_INVALID;
    }

    /* The LoRaWAN specification allows several possible configurations how data up/down frames are built up.
     * In sake of clearness the following naming is applied. Please keep in mind that this is
     * implementation specific since there is no definition in the LoRaWAN specification included.
     *
     * X -> Field is available
     * - -> Field is not available
     *
     * +-------+  +----------+------+-------+--------------+
     * | FType |  | FOptsLen | Fopt | FPort |  FRMPayload  |
     * +-------+  +----------+------+-------+--------------+
     * |   A   |  |    > 0   |   X  |  > 0  |       X      |
     * +-------+  +----------+------+-------+--------------+
     * |   B   |  |   >= 0   |  X/- |   -   |       -      |
     * +-------+  +----------+------+-------+--------------+
     * |   C   |  |    = 0   |   -  |  = 0  | MAC commands |
     * +-------+  +----------+------+-------+--------------+
     * |   D   |  |    = 0   |   -  |  > 0  |       X      |
     * +-------+  +----------+------+-------+--------------+
     */

    if( ( macMsg->FHDR.FCtrl.Bits.FOptsLen > 0 ) && ( macMsg->FPort > 0 ) )
    {
        *fType = FRAME_TYPE_A;
    }
    else if( macMsg->FRMPayloadSize == 0 )
    {
        *fType = FRAME_TYPE_B;
    }
    else if( ( macMsg->FHDR.FCtrl.Bits.FOptsLen == 0 ) && ( macMsg->FPort == 0 ) )
    {
        *fType = FRAME_TYPE_C;
    }
    else if( ( macMsg->FHDR.FCtrl.Bits.FOptsLen == 0 ) && ( macMsg->FPort > 0 ) )
    {
        *fType = FRAME_TYPE_D;
    }
    else
    {
        // Should never happen.
        return LORAMAC_STATUS_ERROR;
    }

    return LORAMAC_STATUS_OK;
}

static bool CheckRetransUnconfirmedUplink( void )
{
    // Unconfirmed uplink, when all retransmissions are done.
    if( MacCtx.ChannelsNbTransCounter >=
        MacCtx.NvmCtx->MacParams.ChannelsNbTrans )
    {
        return true;
    }
    else if( MacCtx.MacFlags.Bits.McpsInd == 1 )
    {
        // For Class A stop in each case
        if( MacCtx.NvmCtx->DeviceClass == CLASS_A )
        {
            return true;
        }
        else
        {// For Class B & C stop only if the frame was received in RX1 window
            if( MacCtx.McpsIndication.RxSlot == RX_SLOT_WIN_1 )
            {
                return true;
            }
        }
    }
    return false;
}

static bool CheckRetransConfirmedUplink( void )
{
    // Confirmed uplink, when all retransmissions ( tries to get a ack ) are done.
    if( MacCtx.AckTimeoutRetriesCounter >=
        MacCtx.AckTimeoutRetries )
    {
        return true;
    }
    else if( MacCtx.MacFlags.Bits.McpsInd == 1 )
    {
        if( MacCtx.McpsConfirm.AckReceived == true )
        {
            return true;
        }
    }
    return false;
}

static bool StopRetransmission( void )
{
    if( ( MacCtx.MacFlags.Bits.McpsInd == 0 ) ||
        ( ( MacCtx.McpsIndication.RxSlot != RX_SLOT_WIN_1 ) &&
          ( MacCtx.McpsIndication.RxSlot != RX_SLOT_WIN_2 ) ) )
    {   // Maximum repetitions without downlink. Increase ADR Ack counter.
        // Only process the case when the MAC did not receive a downlink.
        if( MacCtx.NvmCtx->AdrCtrlOn == true )
        {
            MacCtx.NvmCtx->AdrAckCounter++;
        }
    }

    MacCtx.ChannelsNbTransCounter = 0;
    MacCtx.NodeAckRequested = false;
    MacCtx.AckTimeoutRetry = false;
    MacCtx.MacState &= ~LORAMAC_TX_RUNNING;

    return true;
}

static void AckTimeoutRetriesProcess( void )
{
    if( MacCtx.AckTimeoutRetriesCounter < MacCtx.AckTimeoutRetries )
    {
        MacCtx.AckTimeoutRetriesCounter++;
        if( ( MacCtx.AckTimeoutRetriesCounter % 2 ) == 1 )
        {
            GetPhyParams_t getPhy;
            PhyParam_t phyParam;

            getPhy.Attribute = PHY_NEXT_LOWER_TX_DR;
            getPhy.UplinkDwellTime = MacCtx.NvmCtx->MacParams.UplinkDwellTime;
            getPhy.Datarate = MacCtx.NvmCtx->MacParams.ChannelsDatarate;
            phyParam = RegionGetPhyParam( MacCtx.NvmCtx->Region, &getPhy );
            MacCtx.NvmCtx->MacParams.ChannelsDatarate = phyParam.Value;
        }
    }
}

static void AckTimeoutRetriesFinalize( void )
{
    if( MacCtx.McpsConfirm.AckReceived == false )
    {
        InitDefaultsParams_t params;
        params.Type = INIT_TYPE_RESTORE_DEFAULT_CHANNELS;
        params.NvmCtx = Contexts.RegionNvmCtx;
        RegionInitDefaults( MacCtx.NvmCtx->Region, &params );

        MacCtx.NodeAckRequested = false;
        MacCtx.McpsConfirm.AckReceived = false;
    }
    MacCtx.McpsConfirm.NbRetries = MacCtx.AckTimeoutRetriesCounter;
}

static void CallNvmCtxCallback( LoRaMacNvmCtxModule_t module )
{
    if( ( MacCtx.MacCallbacks != NULL ) && ( MacCtx.MacCallbacks->NvmContextChange != NULL ) )
    {
        MacCtx.MacCallbacks->NvmContextChange( module );
    }
}

static void EventMacNvmCtxChanged( void )
{
    CallNvmCtxCallback( LORAMAC_NVMCTXMODULE_MAC );
}

static void EventRegionNvmCtxChanged( void )
{
    CallNvmCtxCallback( LORAMAC_NVMCTXMODULE_REGION );
}

static void EventCryptoNvmCtxChanged( void )
{
    CallNvmCtxCallback( LORAMAC_NVMCTXMODULE_CRYPTO );
}

static void EventSecureElementNvmCtxChanged( void )
{
    CallNvmCtxCallback( LORAMAC_NVMCTXMODULE_SECURE_ELEMENT );
}

static void EventCommandsNvmCtxChanged( void )
{
    CallNvmCtxCallback( LORAMAC_NVMCTXMODULE_COMMANDS );
}

static void EventClassBNvmCtxChanged( void )
{
    CallNvmCtxCallback( LORAMAC_NVMCTXMODULE_CLASS_B );
}

static void EventConfirmQueueNvmCtxChanged( void )
{
    CallNvmCtxCallback( LORAMAC_NVMCTXMODULE_CONFIRM_QUEUE );
}

static uint8_t IsRequestPending( void )
{
    if( ( MacCtx.MacFlags.Bits.MlmeReq == 1 ) ||
        ( MacCtx.MacFlags.Bits.McpsReq == 1 ) )
    {
        return 1;
    }
    return 0;
}


LoRaMacStatus_t LoRaMacInitialization( LoRaMacPrimitives_t* primitives, LoRaMacCallback_t* callbacks, LoRaMacRegion_t region )
{
    GetPhyParams_t getPhy;
    PhyParam_t phyParam;
    LoRaMacClassBCallback_t classBCallbacks;
    LoRaMacClassBParams_t classBParams;

    if( ( primitives == NULL ) ||
        ( callbacks == NULL ) )
    {
        return LORAMAC_STATUS_PARAMETER_INVALID;
    }

    if( ( primitives->MacMcpsConfirm == NULL ) ||
        ( primitives->MacMcpsIndication == NULL ) ||
        ( primitives->MacMlmeConfirm == NULL ) ||
        ( primitives->MacMlmeIndication == NULL ) )
    {
        return LORAMAC_STATUS_PARAMETER_INVALID;
    }
    // Verify if the region is supported
    if( RegionIsActive( region ) == false )
    {
        return LORAMAC_STATUS_REGION_NOT_SUPPORTED;
    }

    // Confirm queue reset
    LoRaMacConfirmQueueInit( primitives, EventConfirmQueueNvmCtxChanged );

    // Initialize the module context with zeros
    memset1( ( uint8_t* ) &NvmMacCtx, 0x00, sizeof( LoRaMacNvmCtx_t ) );
    memset1( ( uint8_t* ) &MacCtx, 0x00, sizeof( LoRaMacCtx_t ) );
    MacCtx.NvmCtx = &NvmMacCtx;

    // Set non zero variables to its default value
    MacCtx.AckTimeoutRetriesCounter = 1;
    MacCtx.AckTimeoutRetries = 1;
    MacCtx.NvmCtx->Region = region;
    MacCtx.NvmCtx->DeviceClass = CLASS_A;

    // Setup version
    MacCtx.NvmCtx->Version.Value = LORAMAC_VERSION;

    // Reset to defaults
    getPhy.Attribute = PHY_DUTY_CYCLE;
    phyParam = RegionGetPhyParam( MacCtx.NvmCtx->Region, &getPhy );
    MacCtx.NvmCtx->DutyCycleOn = ( bool ) phyParam.Value;

    getPhy.Attribute = PHY_DEF_TX_POWER;
    phyParam = RegionGetPhyParam( MacCtx.NvmCtx->Region, &getPhy );
    MacCtx.NvmCtx->MacParamsDefaults.ChannelsTxPower = phyParam.Value;

    getPhy.Attribute = PHY_DEF_TX_DR;
    phyParam = RegionGetPhyParam( MacCtx.NvmCtx->Region, &getPhy );
    MacCtx.NvmCtx->MacParamsDefaults.ChannelsDatarate = phyParam.Value;

    getPhy.Attribute = PHY_MAX_RX_WINDOW;
    phyParam = RegionGetPhyParam( MacCtx.NvmCtx->Region, &getPhy );
    MacCtx.NvmCtx->MacParamsDefaults.MaxRxWindow = phyParam.Value;

    getPhy.Attribute = PHY_RECEIVE_DELAY1;
    phyParam = RegionGetPhyParam( MacCtx.NvmCtx->Region, &getPhy );
    MacCtx.NvmCtx->MacParamsDefaults.ReceiveDelay1 = phyParam.Value;

    getPhy.Attribute = PHY_RECEIVE_DELAY2;
    phyParam = RegionGetPhyParam( MacCtx.NvmCtx->Region, &getPhy );
    MacCtx.NvmCtx->MacParamsDefaults.ReceiveDelay2 = phyParam.Value;

    getPhy.Attribute = PHY_JOIN_ACCEPT_DELAY1;
    phyParam = RegionGetPhyParam( MacCtx.NvmCtx->Region, &getPhy );
    MacCtx.NvmCtx->MacParamsDefaults.JoinAcceptDelay1 = phyParam.Value;

    getPhy.Attribute = PHY_JOIN_ACCEPT_DELAY2;
    phyParam = RegionGetPhyParam( MacCtx.NvmCtx->Region, &getPhy );
    MacCtx.NvmCtx->MacParamsDefaults.JoinAcceptDelay2 = phyParam.Value;

    getPhy.Attribute = PHY_DEF_DR1_OFFSET;
    phyParam = RegionGetPhyParam( MacCtx.NvmCtx->Region, &getPhy );
    MacCtx.NvmCtx->MacParamsDefaults.Rx1DrOffset = phyParam.Value;

    getPhy.Attribute = PHY_DEF_RX2_FREQUENCY;
    phyParam = RegionGetPhyParam( MacCtx.NvmCtx->Region, &getPhy );
    MacCtx.NvmCtx->MacParamsDefaults.Rx2Channel.Frequency = phyParam.Value;
    MacCtx.NvmCtx->MacParamsDefaults.RxCChannel.Frequency = phyParam.Value;

    getPhy.Attribute = PHY_DEF_RX2_DR;
    phyParam = RegionGetPhyParam( MacCtx.NvmCtx->Region, &getPhy );
    MacCtx.NvmCtx->MacParamsDefaults.Rx2Channel.Datarate = phyParam.Value;
    MacCtx.NvmCtx->MacParamsDefaults.RxCChannel.Datarate = phyParam.Value;

    getPhy.Attribute = PHY_DEF_UPLINK_DWELL_TIME;
    phyParam = RegionGetPhyParam( MacCtx.NvmCtx->Region, &getPhy );
    MacCtx.NvmCtx->MacParamsDefaults.UplinkDwellTime = phyParam.Value;

    getPhy.Attribute = PHY_DEF_DOWNLINK_DWELL_TIME;
    phyParam = RegionGetPhyParam( MacCtx.NvmCtx->Region, &getPhy );
    MacCtx.NvmCtx->MacParamsDefaults.DownlinkDwellTime = phyParam.Value;

    getPhy.Attribute = PHY_DEF_MAX_EIRP;
    phyParam = RegionGetPhyParam( MacCtx.NvmCtx->Region, &getPhy );
    MacCtx.NvmCtx->MacParamsDefaults.MaxEirp = phyParam.fValue;

    getPhy.Attribute = PHY_DEF_ANTENNA_GAIN;
    phyParam = RegionGetPhyParam( MacCtx.NvmCtx->Region, &getPhy );
    MacCtx.NvmCtx->MacParamsDefaults.AntennaGain = phyParam.fValue;

    getPhy.Attribute = PHY_DEF_ADR_ACK_LIMIT;
    phyParam = RegionGetPhyParam( MacCtx.NvmCtx->Region, &getPhy );
    MacCtx.AdrAckLimit = phyParam.Value;

    getPhy.Attribute = PHY_DEF_ADR_ACK_DELAY;
    phyParam = RegionGetPhyParam( MacCtx.NvmCtx->Region, &getPhy );
    MacCtx.AdrAckDelay = phyParam.Value;

    // Init parameters which are not set in function ResetMacParameters
    MacCtx.NvmCtx->MacParamsDefaults.ChannelsNbTrans = 1;
    MacCtx.NvmCtx->MacParamsDefaults.SystemMaxRxError = 10;
    MacCtx.NvmCtx->MacParamsDefaults.MinRxSymbols = 6;

    MacCtx.NvmCtx->MacParams.SystemMaxRxError = MacCtx.NvmCtx->MacParamsDefaults.SystemMaxRxError;
    MacCtx.NvmCtx->MacParams.MinRxSymbols = MacCtx.NvmCtx->MacParamsDefaults.MinRxSymbols;
    MacCtx.NvmCtx->MacParams.MaxRxWindow = MacCtx.NvmCtx->MacParamsDefaults.MaxRxWindow;
    MacCtx.NvmCtx->MacParams.ReceiveDelay1 = MacCtx.NvmCtx->MacParamsDefaults.ReceiveDelay1;
    MacCtx.NvmCtx->MacParams.ReceiveDelay2 = MacCtx.NvmCtx->MacParamsDefaults.ReceiveDelay2;
    MacCtx.NvmCtx->MacParams.JoinAcceptDelay1 = MacCtx.NvmCtx->MacParamsDefaults.JoinAcceptDelay1;
    MacCtx.NvmCtx->MacParams.JoinAcceptDelay2 = MacCtx.NvmCtx->MacParamsDefaults.JoinAcceptDelay2;
    MacCtx.NvmCtx->MacParams.ChannelsNbTrans = MacCtx.NvmCtx->MacParamsDefaults.ChannelsNbTrans;

    InitDefaultsParams_t params;
    params.Type = INIT_TYPE_BANDS;
    params.NvmCtx = NULL;
    RegionInitDefaults( MacCtx.NvmCtx->Region, &params );

    ResetMacParameters( );

    MacCtx.NvmCtx->PublicNetwork = true;

    MacCtx.MacPrimitives = primitives;
    MacCtx.MacCallbacks = callbacks;
    MacCtx.MacFlags.Value = 0;
    MacCtx.MacState = LORAMAC_STOPPED;

    // Reset duty cycle times
    MacCtx.NvmCtx->LastTxDoneTime = 0;
    MacCtx.NvmCtx->AggregatedTimeOff = 0;

    // Initialize timers
    TimerInit( &MacCtx.TxDelayedTimer, OnTxDelayedTimerEvent );
    TimerInit( &MacCtx.RxWindowTimer1, OnRxWindow1TimerEvent );
    TimerInit( &MacCtx.RxWindowTimer2, OnRxWindow2TimerEvent );
    TimerInit( &MacCtx.AckTimeoutTimer, OnAckTimeoutTimerEvent );

    // Store the current initialization time
    MacCtx.NvmCtx->InitializationTime = SysTimeGetMcuTime( );

    // Initialize Radio driver
    MacCtx.RadioEvents.TxDone = OnRadioTxDone;
    MacCtx.RadioEvents.RxDone = OnRadioRxDone;
    MacCtx.RadioEvents.RxError = OnRadioRxError;
    MacCtx.RadioEvents.TxTimeout = OnRadioTxTimeout;
    MacCtx.RadioEvents.RxTimeout = OnRadioRxTimeout;
    Radio.Init( &MacCtx.RadioEvents );

    // Initialize the Secure Element driver
    if( SecureElementInit( EventSecureElementNvmCtxChanged ) != SECURE_ELEMENT_SUCCESS )
    {
        return LORAMAC_STATUS_CRYPTO_ERROR;
    }

    // Initialize Crypto module
    if( LoRaMacCryptoInit( EventCryptoNvmCtxChanged ) != LORAMAC_CRYPTO_SUCCESS )
    {
        return LORAMAC_STATUS_CRYPTO_ERROR;
    }

    // Initialize MAC commands module
    if( LoRaMacCommandsInit( EventCommandsNvmCtxChanged ) != LORAMAC_COMMANDS_SUCCESS )
    {
        return LORAMAC_STATUS_MAC_COMMAD_ERROR;
    }

    // Set multicast downlink counter reference
    if( LoRaMacCryptoSetMulticastReference( MacCtx.NvmCtx->MulticastChannelList ) != LORAMAC_CRYPTO_SUCCESS )
    {
        return LORAMAC_STATUS_CRYPTO_ERROR;
    }

    // Random seed initialization
    srand1( Radio.Random( ) );

    Radio.SetPublicNetwork( MacCtx.NvmCtx->PublicNetwork );
    Radio.Sleep( );

    // Initialize class b
    // Apply callback
    classBCallbacks.GetTemperatureLevel = NULL;
    classBCallbacks.MacProcessNotify = NULL;
    if( callbacks != NULL )
    {
        classBCallbacks.GetTemperatureLevel = callbacks->GetTemperatureLevel;
        classBCallbacks.MacProcessNotify = callbacks->MacProcessNotify;
    }

    // Must all be static. Don't use local references.
    classBParams.MlmeIndication = &MacCtx.MlmeIndication;
    classBParams.McpsIndication = &MacCtx.McpsIndication;
    classBParams.MlmeConfirm = &MacCtx.MlmeConfirm;
    classBParams.LoRaMacFlags = &MacCtx.MacFlags;
    classBParams.LoRaMacDevAddr = &MacCtx.NvmCtx->DevAddr;
    classBParams.LoRaMacRegion = &MacCtx.NvmCtx->Region;
    classBParams.LoRaMacParams = &MacCtx.NvmCtx->MacParams;
    classBParams.MulticastChannels = &MacCtx.NvmCtx->MulticastChannelList[0];

    LoRaMacClassBInit( &classBParams, &classBCallbacks, &EventClassBNvmCtxChanged );

    LoRaMacEnableRequests( LORAMAC_REQUEST_HANDLING_ON );

    return LORAMAC_STATUS_OK;
}

LoRaMacStatus_t LoRaMacStart( void )
{
    MacCtx.MacState = LORAMAC_IDLE;
    return LORAMAC_STATUS_OK;
}

LoRaMacStatus_t LoRaMacStop( void )
{
    if( LoRaMacIsBusy( ) == false )
    {
        MacCtx.MacState = LORAMAC_STOPPED;
        return LORAMAC_STATUS_OK;
    }
    else if(  MacCtx.MacState == LORAMAC_STOPPED )
    {
        return LORAMAC_STATUS_OK;
    }
    return LORAMAC_STATUS_BUSY;
}

LoRaMacStatus_t LoRaMacQueryTxPossible( uint8_t size, LoRaMacTxInfo_t* txInfo )
{
    CalcNextAdrParams_t adrNext;
    uint32_t adrAckCounter = MacCtx.NvmCtx->AdrAckCounter;
    int8_t datarate = MacCtx.NvmCtx->MacParamsDefaults.ChannelsDatarate;
    int8_t txPower = MacCtx.NvmCtx->MacParamsDefaults.ChannelsTxPower;
    size_t macCmdsSize = 0;

    if( txInfo == NULL )
    {
        return LORAMAC_STATUS_PARAMETER_INVALID;
    }

    // Setup ADR request
    adrNext.Version = MacCtx.NvmCtx->Version;
    adrNext.UpdateChanMask = false;
    adrNext.AdrEnabled = MacCtx.NvmCtx->AdrCtrlOn;
    adrNext.AdrAckCounter = MacCtx.NvmCtx->AdrAckCounter;
    adrNext.AdrAckLimit = MacCtx.AdrAckLimit;
    adrNext.AdrAckDelay = MacCtx.AdrAckDelay;
    adrNext.Datarate = MacCtx.NvmCtx->MacParams.ChannelsDatarate;
    adrNext.TxPower = MacCtx.NvmCtx->MacParams.ChannelsTxPower;
    adrNext.UplinkDwellTime = MacCtx.NvmCtx->MacParams.UplinkDwellTime;
    adrNext.Region = MacCtx.NvmCtx->Region;

    // We call the function for information purposes only. We don't want to
    // apply the datarate, the tx power and the ADR ack counter.
    LoRaMacAdrCalcNext( &adrNext, &datarate, &txPower, &adrAckCounter );

    txInfo->CurrentPossiblePayloadSize = GetMaxAppPayloadWithoutFOptsLength( datarate );

    if( LoRaMacCommandsGetSizeSerializedCmds( &macCmdsSize ) != LORAMAC_COMMANDS_SUCCESS )
    {
        return LORAMAC_STATUS_MAC_COMMAD_ERROR;
    }

    // Verify if the MAC commands fit into the FOpts and into the maximum payload.
    if( ( LORA_MAC_COMMAND_MAX_FOPTS_LENGTH >= macCmdsSize ) && ( txInfo->CurrentPossiblePayloadSize >= macCmdsSize ) )
    {
        txInfo->MaxPossibleApplicationDataSize = txInfo->CurrentPossiblePayloadSize - macCmdsSize;

        // Verify if the application data together with MAC command fit into the maximum payload.
        if( txInfo->CurrentPossiblePayloadSize >= ( macCmdsSize + size ) )
        {
            return LORAMAC_STATUS_OK;
        }
        else
        {
           return LORAMAC_STATUS_LENGTH_ERROR;
        }
    }
    else
    {
        txInfo->MaxPossibleApplicationDataSize = 0;
        return LORAMAC_STATUS_LENGTH_ERROR;
    }
}

LoRaMacStatus_t LoRaMacMibGetRequestConfirm( MibRequestConfirm_t* mibGet )
{
    LoRaMacStatus_t status = LORAMAC_STATUS_OK;
    GetPhyParams_t getPhy;
    PhyParam_t phyParam;

    if( mibGet == NULL )
    {
        return LORAMAC_STATUS_PARAMETER_INVALID;
    }

    switch( mibGet->Type )
    {
        case MIB_DEVICE_CLASS:
        {
            mibGet->Param.Class = MacCtx.NvmCtx->DeviceClass;
            break;
        }
        case MIB_NETWORK_ACTIVATION:
        {
            mibGet->Param.NetworkActivation = MacCtx.NvmCtx->NetworkActivation;
            break;
        }
        case MIB_DEV_EUI:
        {
            mibGet->Param.DevEui = SecureElementGetDevEui( );
            break;
        }
        case MIB_JOIN_EUI:
        {
            mibGet->Param.JoinEui = SecureElementGetJoinEui( );
            break;
        }
        case MIB_SE_PIN:
        {
            mibGet->Param.JoinEui = SecureElementGetPin( );
            break;
        }
        case MIB_ADR:
        {
            mibGet->Param.AdrEnable = MacCtx.NvmCtx->AdrCtrlOn;
            break;
        }
        case MIB_NET_ID:
        {
            mibGet->Param.NetID = MacCtx.NvmCtx->NetID;
            break;
        }
        case MIB_DEV_ADDR:
        {
            mibGet->Param.DevAddr = MacCtx.NvmCtx->DevAddr;
            break;
        }
        case MIB_PUBLIC_NETWORK:
        {
            mibGet->Param.EnablePublicNetwork = MacCtx.NvmCtx->PublicNetwork;
            break;
        }
        case MIB_CHANNELS:
        {
            getPhy.Attribute = PHY_CHANNELS;
            phyParam = RegionGetPhyParam( MacCtx.NvmCtx->Region, &getPhy );

            mibGet->Param.ChannelList = phyParam.Channels;
            break;
        }
        case MIB_RX2_CHANNEL:
        {
            mibGet->Param.Rx2Channel = MacCtx.NvmCtx->MacParams.Rx2Channel;
            break;
        }
        case MIB_RX2_DEFAULT_CHANNEL:
        {
            mibGet->Param.Rx2Channel = MacCtx.NvmCtx->MacParamsDefaults.Rx2Channel;
            break;
        }
        case MIB_RXC_CHANNEL:
        {
            mibGet->Param.RxCChannel = MacCtx.NvmCtx->MacParams.RxCChannel;
            break;
        }
        case MIB_RXC_DEFAULT_CHANNEL:
        {
            mibGet->Param.RxCChannel = MacCtx.NvmCtx->MacParamsDefaults.RxCChannel;
            break;
        }
        case MIB_CHANNELS_DEFAULT_MASK:
        {
            getPhy.Attribute = PHY_CHANNELS_DEFAULT_MASK;
            phyParam = RegionGetPhyParam( MacCtx.NvmCtx->Region, &getPhy );

            mibGet->Param.ChannelsDefaultMask = phyParam.ChannelsMask;
            break;
        }
        case MIB_CHANNELS_MASK:
        {
            getPhy.Attribute = PHY_CHANNELS_MASK;
            phyParam = RegionGetPhyParam( MacCtx.NvmCtx->Region, &getPhy );

            mibGet->Param.ChannelsMask = phyParam.ChannelsMask;
            break;
        }
        case MIB_CHANNELS_NB_TRANS:
        {
            mibGet->Param.ChannelsNbTrans = MacCtx.NvmCtx->MacParams.ChannelsNbTrans;
            break;
        }
        case MIB_MAX_RX_WINDOW_DURATION:
        {
            mibGet->Param.MaxRxWindow = MacCtx.NvmCtx->MacParams.MaxRxWindow;
            break;
        }
        case MIB_RECEIVE_DELAY_1:
        {
            mibGet->Param.ReceiveDelay1 = MacCtx.NvmCtx->MacParams.ReceiveDelay1;
            break;
        }
        case MIB_RECEIVE_DELAY_2:
        {
            mibGet->Param.ReceiveDelay2 = MacCtx.NvmCtx->MacParams.ReceiveDelay2;
            break;
        }
        case MIB_JOIN_ACCEPT_DELAY_1:
        {
            mibGet->Param.JoinAcceptDelay1 = MacCtx.NvmCtx->MacParams.JoinAcceptDelay1;
            break;
        }
        case MIB_JOIN_ACCEPT_DELAY_2:
        {
            mibGet->Param.JoinAcceptDelay2 = MacCtx.NvmCtx->MacParams.JoinAcceptDelay2;
            break;
        }
        case MIB_CHANNELS_DEFAULT_DATARATE:
        {
            mibGet->Param.ChannelsDefaultDatarate = MacCtx.NvmCtx->MacParamsDefaults.ChannelsDatarate;
            break;
        }
        case MIB_CHANNELS_DATARATE:
        {
            mibGet->Param.ChannelsDatarate = MacCtx.NvmCtx->MacParams.ChannelsDatarate;
            break;
        }
        case MIB_CHANNELS_DEFAULT_TX_POWER:
        {
            mibGet->Param.ChannelsDefaultTxPower = MacCtx.NvmCtx->MacParamsDefaults.ChannelsTxPower;
            break;
        }
        case MIB_CHANNELS_TX_POWER:
        {
            mibGet->Param.ChannelsTxPower = MacCtx.NvmCtx->MacParams.ChannelsTxPower;
            break;
        }
        case MIB_SYSTEM_MAX_RX_ERROR:
        {
            mibGet->Param.SystemMaxRxError = MacCtx.NvmCtx->MacParams.SystemMaxRxError;
            break;
        }
        case MIB_MIN_RX_SYMBOLS:
        {
            mibGet->Param.MinRxSymbols = MacCtx.NvmCtx->MacParams.MinRxSymbols;
            break;
        }
        case MIB_ANTENNA_GAIN:
        {
            mibGet->Param.AntennaGain = MacCtx.NvmCtx->MacParams.AntennaGain;
            break;
        }
        case MIB_NVM_CTXS:
        {
            mibGet->Param.Contexts = GetCtxs( );
            break;
        }
        case MIB_DEFAULT_ANTENNA_GAIN:
        {
            mibGet->Param.DefaultAntennaGain = MacCtx.NvmCtx->MacParamsDefaults.AntennaGain;
            break;
        }
        case MIB_LORAWAN_VERSION:
        {
            mibGet->Param.LrWanVersion.LoRaWan = MacCtx.NvmCtx->Version;
            mibGet->Param.LrWanVersion.LoRaWanRegion = RegionGetVersion( );
            break;
        }
        default:
        {
            status = LoRaMacClassBMibGetRequestConfirm( mibGet );
            break;
        }
    }
    return status;
}

LoRaMacStatus_t LoRaMacMibSetRequestConfirm( MibRequestConfirm_t* mibSet )
{
    LoRaMacStatus_t status = LORAMAC_STATUS_OK;
    ChanMaskSetParams_t chanMaskSet;
    VerifyParams_t verify;

    if( mibSet == NULL )
    {
        return LORAMAC_STATUS_PARAMETER_INVALID;
    }
    if( ( MacCtx.MacState & LORAMAC_TX_RUNNING ) == LORAMAC_TX_RUNNING )
    {
        return LORAMAC_STATUS_BUSY;
    }

    switch( mibSet->Type )
    {
        case MIB_DEVICE_CLASS:
        {
            status = SwitchClass( mibSet->Param.Class );
            break;
        }
        case MIB_NETWORK_ACTIVATION:
        {
            if( mibSet->Param.NetworkActivation != ACTIVATION_TYPE_OTAA  )
            {
                MacCtx.NvmCtx->NetworkActivation = mibSet->Param.NetworkActivation;
            }
            else
            {   // Do not allow to set ACTIVATION_TYPE_OTAA since the MAC will set it automatically after a successful join process.
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
        case MIB_DEV_EUI:
        {
            if( SecureElementSetDevEui( mibSet->Param.DevEui ) != SECURE_ELEMENT_SUCCESS )
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
        case MIB_JOIN_EUI:
        {
            if( SecureElementSetJoinEui( mibSet->Param.JoinEui ) != SECURE_ELEMENT_SUCCESS )
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
        case MIB_SE_PIN:
        {
            if( SecureElementSetPin( mibSet->Param.SePin ) != SECURE_ELEMENT_SUCCESS )
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
        case MIB_ADR:
        {
            MacCtx.NvmCtx->AdrCtrlOn = mibSet->Param.AdrEnable;
            break;
        }
        case MIB_NET_ID:
        {
            MacCtx.NvmCtx->NetID = mibSet->Param.NetID;
            break;
        }
        case MIB_DEV_ADDR:
        {
            MacCtx.NvmCtx->DevAddr = mibSet->Param.DevAddr;
            break;
        }
        case MIB_APP_KEY:
        {
            if( mibSet->Param.AppKey != NULL )
            {
                if( LORAMAC_CRYPTO_SUCCESS != LoRaMacCryptoSetKey( APP_KEY, mibSet->Param.AppKey ) )
                {
                    return LORAMAC_STATUS_CRYPTO_ERROR;
                }
            }
            else
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
        case MIB_NWK_KEY:
        {
            if( mibSet->Param.NwkKey != NULL )
            {
                if( LORAMAC_CRYPTO_SUCCESS != LoRaMacCryptoSetKey( NWK_KEY, mibSet->Param.NwkKey ) )
                {
                    return LORAMAC_STATUS_CRYPTO_ERROR;
                }
            }
            else
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
        case MIB_J_S_INT_KEY:
        {
            if( mibSet->Param.JSIntKey != NULL )
            {
                if( LORAMAC_CRYPTO_SUCCESS != LoRaMacCryptoSetKey( J_S_INT_KEY, mibSet->Param.JSIntKey ) )
                {
                    return LORAMAC_STATUS_CRYPTO_ERROR;
                }
            }
            else
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
        case MIB_J_S_ENC_KEY:
        {
            if( mibSet->Param.JSEncKey != NULL )
            {
                if( LORAMAC_CRYPTO_SUCCESS != LoRaMacCryptoSetKey( J_S_ENC_KEY, mibSet->Param.JSEncKey ) )
                {
                    return LORAMAC_STATUS_CRYPTO_ERROR;
                }
            }
            else
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
        case MIB_F_NWK_S_INT_KEY:
        {
            if( mibSet->Param.FNwkSIntKey != NULL )
            {
                if( LORAMAC_CRYPTO_SUCCESS != LoRaMacCryptoSetKey( F_NWK_S_INT_KEY, mibSet->Param.FNwkSIntKey ) )
                {
                    return LORAMAC_STATUS_CRYPTO_ERROR;
                }
            }
            else
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
        case MIB_S_NWK_S_INT_KEY:
        {
            if( mibSet->Param.SNwkSIntKey != NULL )
            {
                if( LORAMAC_CRYPTO_SUCCESS != LoRaMacCryptoSetKey( S_NWK_S_INT_KEY, mibSet->Param.SNwkSIntKey ) )
                {
                    return LORAMAC_STATUS_CRYPTO_ERROR;
                }
            }
            else
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
        case MIB_NWK_S_ENC_KEY:
        {
            if( mibSet->Param.NwkSEncKey != NULL )
            {
                if( LORAMAC_CRYPTO_SUCCESS != LoRaMacCryptoSetKey( NWK_S_ENC_KEY, mibSet->Param.NwkSEncKey ) )
                {
                    return LORAMAC_STATUS_CRYPTO_ERROR;
                }
            }
            else
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
        case MIB_APP_S_KEY:
        {
            if( mibSet->Param.AppSKey != NULL )
            {
                if( LORAMAC_CRYPTO_SUCCESS != LoRaMacCryptoSetKey( APP_S_KEY, mibSet->Param.AppSKey ) )
                {
                    return LORAMAC_STATUS_CRYPTO_ERROR;
                }
            }
            else
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
        case MIB_MC_KE_KEY:
        {
            if( mibSet->Param.McKEKey != NULL )
            {
                if( LORAMAC_CRYPTO_SUCCESS != LoRaMacCryptoSetKey( MC_KE_KEY, mibSet->Param.McKEKey ) )
                {
                    return LORAMAC_STATUS_CRYPTO_ERROR;
                }
            }
            else
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
        case MIB_MC_KEY_0:
        {
            if( mibSet->Param.McKey0 != NULL )
            {
                if( LORAMAC_CRYPTO_SUCCESS != LoRaMacCryptoSetKey( MC_KEY_0, mibSet->Param.McKey0 ) )
                {
                    return LORAMAC_STATUS_CRYPTO_ERROR;
                }
            }
            else
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
        case MIB_MC_APP_S_KEY_0:
        {
            if( mibSet->Param.McAppSKey0 != NULL )
            {
                if( LORAMAC_CRYPTO_SUCCESS != LoRaMacCryptoSetKey( MC_APP_S_KEY_0, mibSet->Param.McAppSKey0 ) )
                {
                    return LORAMAC_STATUS_CRYPTO_ERROR;
                }
            }
            else
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
        case MIB_MC_NWK_S_KEY_0:
        {
            if( mibSet->Param.McNwkSKey0 != NULL )
            {
                if( LORAMAC_CRYPTO_SUCCESS != LoRaMacCryptoSetKey( MC_NWK_S_KEY_0, mibSet->Param.McNwkSKey0 ) )
                {
                    return LORAMAC_STATUS_CRYPTO_ERROR;
                }
            }
            else
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
        case MIB_MC_KEY_1:
        {
            if( mibSet->Param.McKey1 != NULL )
            {
                if( LORAMAC_CRYPTO_SUCCESS != LoRaMacCryptoSetKey( MC_KEY_1, mibSet->Param.McKey1 ) )
                {
                    return LORAMAC_STATUS_CRYPTO_ERROR;
                }
            }
            else
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
        case MIB_MC_APP_S_KEY_1:
        {
            if( mibSet->Param.McAppSKey1 != NULL )
            {
                if( LORAMAC_CRYPTO_SUCCESS != LoRaMacCryptoSetKey( MC_APP_S_KEY_1, mibSet->Param.McAppSKey1 ) )
                {
                    return LORAMAC_STATUS_CRYPTO_ERROR;
                }
            }
            else
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
        case MIB_MC_NWK_S_KEY_1:
        {
            if( mibSet->Param.McNwkSKey1 != NULL )
            {
                if( LORAMAC_CRYPTO_SUCCESS != LoRaMacCryptoSetKey( MC_NWK_S_KEY_1, mibSet->Param.McNwkSKey1 ) )
                {
                    return LORAMAC_STATUS_CRYPTO_ERROR;
                }
            }
            else
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
        case MIB_MC_KEY_2:
        {
            if( mibSet->Param.McKey2 != NULL )
            {
                if( LORAMAC_CRYPTO_SUCCESS != LoRaMacCryptoSetKey( MC_KEY_2, mibSet->Param.McKey2 ) )
                {
                    return LORAMAC_STATUS_CRYPTO_ERROR;
                }
            }
            else
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
        case MIB_MC_APP_S_KEY_2:
        {
            if( mibSet->Param.McAppSKey2 != NULL )
            {
                if( LORAMAC_CRYPTO_SUCCESS != LoRaMacCryptoSetKey( MC_APP_S_KEY_2, mibSet->Param.McAppSKey2 ) )
                {
                    return LORAMAC_STATUS_CRYPTO_ERROR;
                }
            }
            else
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
        case MIB_MC_NWK_S_KEY_2:
        {
            if( mibSet->Param.McNwkSKey2 != NULL )
            {
                if( LORAMAC_CRYPTO_SUCCESS != LoRaMacCryptoSetKey( MC_NWK_S_KEY_2, mibSet->Param.McNwkSKey2 ) )
                {
                    return LORAMAC_STATUS_CRYPTO_ERROR;
                }
            }
            else
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
        case MIB_MC_KEY_3:
        {
            if( mibSet->Param.McKey3 != NULL )
            {
                if( LORAMAC_CRYPTO_SUCCESS != LoRaMacCryptoSetKey( MC_KEY_3, mibSet->Param.McKey3 ) )
                {
                    return LORAMAC_STATUS_CRYPTO_ERROR;
                }
            }
            else
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
        case MIB_MC_APP_S_KEY_3:
        {
            if( mibSet->Param.McAppSKey3 != NULL )
            {
                if( LORAMAC_CRYPTO_SUCCESS != LoRaMacCryptoSetKey( MC_APP_S_KEY_3, mibSet->Param.McAppSKey3 ) )
                {
                    return LORAMAC_STATUS_CRYPTO_ERROR;
                }
            }
            else
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
        case MIB_MC_NWK_S_KEY_3:
        {
            if( mibSet->Param.McNwkSKey3 != NULL )
            {
                if( LORAMAC_CRYPTO_SUCCESS != LoRaMacCryptoSetKey( MC_NWK_S_KEY_3, mibSet->Param.McNwkSKey3 ) )
                {
                    return LORAMAC_STATUS_CRYPTO_ERROR;
                }
            }
            else
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
        case MIB_PUBLIC_NETWORK:
        {
            MacCtx.NvmCtx->PublicNetwork = mibSet->Param.EnablePublicNetwork;
            Radio.SetPublicNetwork( MacCtx.NvmCtx->PublicNetwork );
            break;
        }
        case MIB_RX2_CHANNEL:
        {
            verify.DatarateParams.Datarate = mibSet->Param.Rx2Channel.Datarate;
            verify.DatarateParams.DownlinkDwellTime = MacCtx.NvmCtx->MacParams.DownlinkDwellTime;

            if( RegionVerify( MacCtx.NvmCtx->Region, &verify, PHY_RX_DR ) == true )
            {
                MacCtx.NvmCtx->MacParams.Rx2Channel = mibSet->Param.Rx2Channel;
            }
            else
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
        case MIB_RX2_DEFAULT_CHANNEL:
        {
            verify.DatarateParams.Datarate = mibSet->Param.Rx2Channel.Datarate;
            verify.DatarateParams.DownlinkDwellTime = MacCtx.NvmCtx->MacParams.DownlinkDwellTime;

            if( RegionVerify( MacCtx.NvmCtx->Region, &verify, PHY_RX_DR ) == true )
            {
                MacCtx.NvmCtx->MacParamsDefaults.Rx2Channel = mibSet->Param.Rx2DefaultChannel;
            }
            else
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
        case MIB_RXC_CHANNEL:
        {
            verify.DatarateParams.Datarate = mibSet->Param.RxCChannel.Datarate;
            verify.DatarateParams.DownlinkDwellTime = MacCtx.NvmCtx->MacParams.DownlinkDwellTime;

            if( RegionVerify( MacCtx.NvmCtx->Region, &verify, PHY_RX_DR ) == true )
            {
                MacCtx.NvmCtx->MacParams.RxCChannel = mibSet->Param.RxCChannel;

                if( ( MacCtx.NvmCtx->DeviceClass == CLASS_C ) && ( MacCtx.NvmCtx->NetworkActivation != ACTIVATION_TYPE_NONE ) )
                {
                    // We can only compute the RX window parameters directly, if we are already
                    // in class c mode and joined. We cannot setup an RX window in case of any other
                    // class type.
                    // Set the radio into sleep mode in case we are still in RX mode
                    Radio.Sleep( );

                    OpenContinuousRxCWindow( );
                }
            }
            else
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
        case MIB_RXC_DEFAULT_CHANNEL:
        {
            verify.DatarateParams.Datarate = mibSet->Param.RxCChannel.Datarate;
            verify.DatarateParams.DownlinkDwellTime = MacCtx.NvmCtx->MacParams.DownlinkDwellTime;

            if( RegionVerify( MacCtx.NvmCtx->Region, &verify, PHY_RX_DR ) == true )
            {
                MacCtx.NvmCtx->MacParamsDefaults.RxCChannel = mibSet->Param.RxCDefaultChannel;
            }
            else
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
        case MIB_CHANNELS_DEFAULT_MASK:
        {
            chanMaskSet.ChannelsMaskIn = mibSet->Param.ChannelsDefaultMask;
            chanMaskSet.ChannelsMaskType = CHANNELS_DEFAULT_MASK;

            if( RegionChanMaskSet( MacCtx.NvmCtx->Region, &chanMaskSet ) == false )
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
        case MIB_CHANNELS_MASK:
        {
            chanMaskSet.ChannelsMaskIn = mibSet->Param.ChannelsMask;
            chanMaskSet.ChannelsMaskType = CHANNELS_MASK;

            if( RegionChanMaskSet( MacCtx.NvmCtx->Region, &chanMaskSet ) == false )
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
        case MIB_CHANNELS_NB_TRANS:
        {
            if( ( mibSet->Param.ChannelsNbTrans >= 1 ) &&
                ( mibSet->Param.ChannelsNbTrans <= 15 ) )
            {
                MacCtx.NvmCtx->MacParams.ChannelsNbTrans = mibSet->Param.ChannelsNbTrans;
            }
            else
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
        case MIB_MAX_RX_WINDOW_DURATION:
        {
            MacCtx.NvmCtx->MacParams.MaxRxWindow = mibSet->Param.MaxRxWindow;
            break;
        }
        case MIB_RECEIVE_DELAY_1:
        {
            MacCtx.NvmCtx->MacParams.ReceiveDelay1 = mibSet->Param.ReceiveDelay1;
            break;
        }
        case MIB_RECEIVE_DELAY_2:
        {
            MacCtx.NvmCtx->MacParams.ReceiveDelay2 = mibSet->Param.ReceiveDelay2;
            break;
        }
        case MIB_JOIN_ACCEPT_DELAY_1:
        {
            MacCtx.NvmCtx->MacParams.JoinAcceptDelay1 = mibSet->Param.JoinAcceptDelay1;
            break;
        }
        case MIB_JOIN_ACCEPT_DELAY_2:
        {
            MacCtx.NvmCtx->MacParams.JoinAcceptDelay2 = mibSet->Param.JoinAcceptDelay2;
            break;
        }
        case MIB_CHANNELS_DEFAULT_DATARATE:
        {
            verify.DatarateParams.Datarate = mibSet->Param.ChannelsDefaultDatarate;

            if( RegionVerify( MacCtx.NvmCtx->Region, &verify, PHY_DEF_TX_DR ) == true )
            {
                MacCtx.NvmCtx->MacParamsDefaults.ChannelsDatarate = verify.DatarateParams.Datarate;
            }
            else
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
        case MIB_CHANNELS_DATARATE:
        {
            verify.DatarateParams.Datarate = mibSet->Param.ChannelsDatarate;
            verify.DatarateParams.UplinkDwellTime = MacCtx.NvmCtx->MacParams.UplinkDwellTime;

            if( RegionVerify( MacCtx.NvmCtx->Region, &verify, PHY_TX_DR ) == true )
            {
                MacCtx.NvmCtx->MacParams.ChannelsDatarate = verify.DatarateParams.Datarate;
            }
            else
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
        case MIB_CHANNELS_DEFAULT_TX_POWER:
        {
            verify.TxPower = mibSet->Param.ChannelsDefaultTxPower;

            if( RegionVerify( MacCtx.NvmCtx->Region, &verify, PHY_DEF_TX_POWER ) == true )
            {
                MacCtx.NvmCtx->MacParamsDefaults.ChannelsTxPower = verify.TxPower;
            }
            else
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
        case MIB_CHANNELS_TX_POWER:
        {
            verify.TxPower = mibSet->Param.ChannelsTxPower;

            if( RegionVerify( MacCtx.NvmCtx->Region, &verify, PHY_TX_POWER ) == true )
            {
                MacCtx.NvmCtx->MacParams.ChannelsTxPower = verify.TxPower;
            }
            else
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
        case MIB_SYSTEM_MAX_RX_ERROR:
        {
            MacCtx.NvmCtx->MacParams.SystemMaxRxError = MacCtx.NvmCtx->MacParamsDefaults.SystemMaxRxError = mibSet->Param.SystemMaxRxError;
            break;
        }
        case MIB_MIN_RX_SYMBOLS:
        {
            MacCtx.NvmCtx->MacParams.MinRxSymbols = MacCtx.NvmCtx->MacParamsDefaults.MinRxSymbols = mibSet->Param.MinRxSymbols;
            break;
        }
        case MIB_ANTENNA_GAIN:
        {
            MacCtx.NvmCtx->MacParams.AntennaGain = mibSet->Param.AntennaGain;
            break;
        }
        case MIB_DEFAULT_ANTENNA_GAIN:
        {
            MacCtx.NvmCtx->MacParamsDefaults.AntennaGain = mibSet->Param.DefaultAntennaGain;
            break;
        }
        case MIB_NVM_CTXS:
        {
            if( mibSet->Param.Contexts != 0 )
            {
                status = RestoreCtxs( mibSet->Param.Contexts );
            }
            else
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
        case MIB_ABP_LORAWAN_VERSION:
        {
            if( mibSet->Param.AbpLrWanVersion.Fields.Minor <= 1 )
            {
                MacCtx.NvmCtx->Version = mibSet->Param.AbpLrWanVersion;

                if( LORAMAC_CRYPTO_SUCCESS != LoRaMacCryptoSetLrWanVersion( mibSet->Param.AbpLrWanVersion ) )
                {
                    return LORAMAC_STATUS_CRYPTO_ERROR;
                }
            }
            else
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
        default:
        {
            status = LoRaMacMibClassBSetRequestConfirm( mibSet );
            break;
        }
    }
    EventRegionNvmCtxChanged( );
    EventMacNvmCtxChanged( );
    return status;
}

LoRaMacStatus_t LoRaMacChannelAdd( uint8_t id, ChannelParams_t params )
{
    ChannelAddParams_t channelAdd;

    // Validate if the MAC is in a correct state
    if( ( MacCtx.MacState & LORAMAC_TX_RUNNING ) == LORAMAC_TX_RUNNING )
    {
        if( ( MacCtx.MacState & LORAMAC_TX_CONFIG ) != LORAMAC_TX_CONFIG )
        {
            return LORAMAC_STATUS_BUSY;
        }
    }

    channelAdd.NewChannel = &params;
    channelAdd.ChannelId = id;

    EventRegionNvmCtxChanged( );
    return RegionChannelAdd( MacCtx.NvmCtx->Region, &channelAdd );
}

LoRaMacStatus_t LoRaMacChannelRemove( uint8_t id )
{
    ChannelRemoveParams_t channelRemove;

    if( ( MacCtx.MacState & LORAMAC_TX_RUNNING ) == LORAMAC_TX_RUNNING )
    {
        if( ( MacCtx.MacState & LORAMAC_TX_CONFIG ) != LORAMAC_TX_CONFIG )
        {
            return LORAMAC_STATUS_BUSY;
        }
    }

    channelRemove.ChannelId = id;

    if( RegionChannelsRemove( MacCtx.NvmCtx->Region, &channelRemove ) == false )
    {
        return LORAMAC_STATUS_PARAMETER_INVALID;
    }

    EventRegionNvmCtxChanged( );
    return LORAMAC_STATUS_OK;
}

LoRaMacStatus_t LoRaMacMcChannelSetup( McChannelParams_t *channel )
{
    if( ( MacCtx.MacState & LORAMAC_TX_RUNNING ) == LORAMAC_TX_RUNNING )
    {
        return LORAMAC_STATUS_BUSY;
    }

    if( channel->GroupID >= LORAMAC_MAX_MC_CTX )
    {
        return LORAMAC_STATUS_MC_GROUP_UNDEFINED;
    }

    MacCtx.NvmCtx->MulticastChannelList[channel->GroupID].ChannelParams = *channel;

    if( channel->IsRemotelySetup == true )
    {
        const KeyIdentifier_t mcKeys[LORAMAC_MAX_MC_CTX] = { MC_KEY_0, MC_KEY_1, MC_KEY_2, MC_KEY_3 };
        if( LoRaMacCryptoSetKey( mcKeys[channel->GroupID], channel->McKeys.McKeyE ) != LORAMAC_CRYPTO_SUCCESS )
        {
            return LORAMAC_STATUS_CRYPTO_ERROR;
        }

        if( LoRaMacCryptoDeriveMcSessionKeyPair( channel->GroupID, channel->Address ) != LORAMAC_CRYPTO_SUCCESS )
        {
            return LORAMAC_STATUS_CRYPTO_ERROR;
        }
    }
    else
    {
        const KeyIdentifier_t mcAppSKeys[LORAMAC_MAX_MC_CTX] = { MC_APP_S_KEY_0, MC_APP_S_KEY_1, MC_APP_S_KEY_2, MC_APP_S_KEY_3 };
        const KeyIdentifier_t mcNwkSKeys[LORAMAC_MAX_MC_CTX] = { MC_NWK_S_KEY_0, MC_NWK_S_KEY_1, MC_NWK_S_KEY_2, MC_NWK_S_KEY_3 };
        if( LORAMAC_CRYPTO_SUCCESS != LoRaMacCryptoSetKey( mcAppSKeys[channel->GroupID], channel->McKeys.Session.McAppSKey ) )
        {
            return LORAMAC_STATUS_CRYPTO_ERROR;
        }
        if( LORAMAC_CRYPTO_SUCCESS != LoRaMacCryptoSetKey( mcNwkSKeys[channel->GroupID], channel->McKeys.Session.McNwkSKey ) )
        {
            return LORAMAC_STATUS_CRYPTO_ERROR;
        }
    }

    if( channel->Class == CLASS_B )
    {
        // Calculate class b parameters
        LoRaMacClassBSetMulticastPeriodicity( &MacCtx.NvmCtx->MulticastChannelList[channel->GroupID] );
    }

    // Reset multicast channel downlink counter to initial value.
    *MacCtx.NvmCtx->MulticastChannelList[channel->GroupID].DownLinkCounter = FCNT_DOWN_INITAL_VALUE;

    EventMacNvmCtxChanged( );
    EventRegionNvmCtxChanged( );
    return LORAMAC_STATUS_OK;
}

LoRaMacStatus_t LoRaMacMcChannelDelete( AddressIdentifier_t groupID )
{
    if( ( MacCtx.MacState & LORAMAC_TX_RUNNING ) == LORAMAC_TX_RUNNING )
    {
        return LORAMAC_STATUS_BUSY;
    }

    if( ( groupID >= LORAMAC_MAX_MC_CTX ) ||
        ( MacCtx.NvmCtx->MulticastChannelList[groupID].ChannelParams.IsEnabled == false ) )
    {
        return LORAMAC_STATUS_MC_GROUP_UNDEFINED;
    }

    McChannelParams_t channel;

    // Set all channel fields with 0
    memset1( ( uint8_t* )&channel, 0, sizeof( McChannelParams_t ) );

    MacCtx.NvmCtx->MulticastChannelList[groupID].ChannelParams = channel;

    EventMacNvmCtxChanged( );
    EventRegionNvmCtxChanged( );
    return LORAMAC_STATUS_OK;
}

uint8_t LoRaMacMcChannelGetGroupId( uint32_t mcAddress )
{
    for( uint8_t i = 0; i < LORAMAC_MAX_MC_CTX; i++ )
    {
        if( mcAddress == MacCtx.NvmCtx->MulticastChannelList[i].ChannelParams.Address )
        {
            return i;
        }
    }
    return 0xFF;
}

LoRaMacStatus_t LoRaMacMcChannelSetupRxParams( AddressIdentifier_t groupID, McRxParams_t *rxParams, uint8_t *status )
{
   *status = 0x1C + ( groupID & 0x03 );

    if( ( MacCtx.MacState & LORAMAC_TX_RUNNING ) == LORAMAC_TX_RUNNING )
    {
        return LORAMAC_STATUS_BUSY;
    }

    DeviceClass_t devClass = MacCtx.NvmCtx->MulticastChannelList[groupID].ChannelParams.Class;
    if( ( devClass == CLASS_A ) || ( devClass > CLASS_C ) )
    {
        return LORAMAC_STATUS_PARAMETER_INVALID;
    }

    if( ( groupID >= LORAMAC_MAX_MC_CTX ) ||
        ( MacCtx.NvmCtx->MulticastChannelList[groupID].ChannelParams.IsEnabled == false ) )
    {
        return LORAMAC_STATUS_MC_GROUP_UNDEFINED;
    }
    *status &= 0x0F; // groupID OK

    VerifyParams_t verify;
    // Check datarate
    if( devClass == CLASS_B )
    {
        verify.DatarateParams.Datarate = rxParams->ClassB.Datarate;
    }
    else
    {
        verify.DatarateParams.Datarate = rxParams->ClassC.Datarate;
    }
    verify.DatarateParams.DownlinkDwellTime = MacCtx.NvmCtx->MacParams.DownlinkDwellTime;

    if( RegionVerify( MacCtx.NvmCtx->Region, &verify, PHY_RX_DR ) == true )
    {
        *status &= 0xFB; // datarate OK
    }

    // Check frequency
    if( devClass == CLASS_B )
    {
        verify.Frequency = rxParams->ClassB.Frequency;
    }
    else
    {
        verify.Frequency = rxParams->ClassC.Frequency;
    }
    if( RegionVerify( MacCtx.NvmCtx->Region, &verify, PHY_FREQUENCY ) == true )
    {
        *status &= 0xF7; // frequency OK
    }

    if( *status == ( groupID & 0x03 ) )
    {
        // Apply parameters
        MacCtx.NvmCtx->MulticastChannelList[groupID].ChannelParams.RxParams = *rxParams;
    }

    EventMacNvmCtxChanged( );
    EventRegionNvmCtxChanged( );
    return LORAMAC_STATUS_OK;
}

LoRaMacStatus_t LoRaMacMlmeRequest( MlmeReq_t* mlmeRequest )
{
    LoRaMacStatus_t status = LORAMAC_STATUS_SERVICE_UNKNOWN;
    MlmeConfirmQueue_t queueElement;
    uint8_t macCmdPayload[2] = { 0x00, 0x00 };

    if( mlmeRequest == NULL )
    {
        return LORAMAC_STATUS_PARAMETER_INVALID;
    }
    if( LoRaMacIsBusy( ) == true )
    {
        return LORAMAC_STATUS_BUSY;
    }
    if( LoRaMacConfirmQueueIsFull( ) == true )
    {
        return LORAMAC_STATUS_BUSY;
    }

    if( LoRaMacConfirmQueueGetCnt( ) == 0 )
    {
        memset1( ( uint8_t* ) &MacCtx.MlmeConfirm, 0, sizeof( MacCtx.MlmeConfirm ) );
    }
    MacCtx.MlmeConfirm.Status = LORAMAC_EVENT_INFO_STATUS_ERROR;

    MacCtx.MacFlags.Bits.MlmeReq = 1;
    queueElement.Request = mlmeRequest->Type;
    queueElement.Status = LORAMAC_EVENT_INFO_STATUS_ERROR;
    queueElement.RestrictCommonReadyToHandle = false;

    switch( mlmeRequest->Type )
    {
        case MLME_JOIN:
        {
            if( ( MacCtx.MacState & LORAMAC_TX_DELAYED ) == LORAMAC_TX_DELAYED )
            {
                return LORAMAC_STATUS_BUSY;
            }

            ResetMacParameters( );

            MacCtx.NvmCtx->MacParams.ChannelsDatarate = RegionAlternateDr( MacCtx.NvmCtx->Region, mlmeRequest->Req.Join.Datarate, ALTERNATE_DR );

            queueElement.Status = LORAMAC_EVENT_INFO_STATUS_JOIN_FAIL;

            status = SendReJoinReq( JOIN_REQ );

            if( status != LORAMAC_STATUS_OK )
            {
                // Revert back the previous datarate ( mainly used for US915 like regions )
                MacCtx.NvmCtx->MacParams.ChannelsDatarate = RegionAlternateDr( MacCtx.NvmCtx->Region, mlmeRequest->Req.Join.Datarate, ALTERNATE_DR_RESTORE );
            }
            break;
        }
        case MLME_LINK_CHECK:
        {
            // LoRaMac will send this command piggy-pack
            status = LORAMAC_STATUS_OK;
            if( LoRaMacCommandsAddCmd( MOTE_MAC_LINK_CHECK_REQ, macCmdPayload, 0 ) != LORAMAC_COMMANDS_SUCCESS )
            {
                status = LORAMAC_STATUS_MAC_COMMAD_ERROR;
            }
            break;
        }
        case MLME_TXCW:
        {
            status = SetTxContinuousWave( mlmeRequest->Req.TxCw.Timeout );
            break;
        }
        case MLME_TXCW_1:
        {

            status = SetTxContinuousWave1( mlmeRequest->Req.TxCw.Timeout, mlmeRequest->Req.TxCw.Frequency, mlmeRequest->Req.TxCw.Power );
            break;
        }
        case MLME_DEVICE_TIME:
        {
            // LoRaMac will send this command piggy-pack
            status = LORAMAC_STATUS_OK;
            if( LoRaMacCommandsAddCmd( MOTE_MAC_DEVICE_TIME_REQ, macCmdPayload, 0 ) != LORAMAC_COMMANDS_SUCCESS )
            {
                status = LORAMAC_STATUS_MAC_COMMAD_ERROR;
            }
            break;
        }
        case MLME_PING_SLOT_INFO:
        {
            if( MacCtx.NvmCtx->DeviceClass == CLASS_A )
            {
                uint8_t value = mlmeRequest->Req.PingSlotInfo.PingSlot.Value;

                // LoRaMac will send this command piggy-pack
                LoRaMacClassBSetPingSlotInfo( mlmeRequest->Req.PingSlotInfo.PingSlot.Fields.Periodicity );
                macCmdPayload[0] = value;
                status = LORAMAC_STATUS_OK;
                if( LoRaMacCommandsAddCmd( MOTE_MAC_PING_SLOT_INFO_REQ, macCmdPayload, 1 ) != LORAMAC_COMMANDS_SUCCESS )
                {
                    status = LORAMAC_STATUS_MAC_COMMAD_ERROR;
                }
            }
            break;
        }
        case MLME_BEACON_TIMING:
        {
            // LoRaMac will send this command piggy-pack
            status = LORAMAC_STATUS_OK;
            if( LoRaMacCommandsAddCmd( MOTE_MAC_BEACON_TIMING_REQ, macCmdPayload, 0 ) != LORAMAC_COMMANDS_SUCCESS )
            {
                status = LORAMAC_STATUS_MAC_COMMAD_ERROR;
            }
            break;
        }
        case MLME_BEACON_ACQUISITION:
        {
            // Apply the request
            queueElement.RestrictCommonReadyToHandle = true;

            if( LoRaMacClassBIsAcquisitionInProgress( ) == false )
            {
                // Start class B algorithm
                LoRaMacClassBSetBeaconState( BEACON_STATE_ACQUISITION );
                LoRaMacClassBBeaconTimerEvent( NULL );

                status = LORAMAC_STATUS_OK;
            }
            else
            {
                status = LORAMAC_STATUS_BUSY;
            }
            break;
        }
        default:
            break;
    }

    // Fill return structure
    mlmeRequest->ReqReturn.DutyCycleWaitTime = MacCtx.DutyCycleWaitTime;

    if( status != LORAMAC_STATUS_OK )
    {
        if( LoRaMacConfirmQueueGetCnt( ) == 0 )
        {
            MacCtx.NodeAckRequested = false;
            MacCtx.MacFlags.Bits.MlmeReq = 0;
        }
    }
    else
    {
        LoRaMacConfirmQueueAdd( &queueElement );
        EventMacNvmCtxChanged( );
    }
    return status;
}

LoRaMacStatus_t LoRaMacMcpsRequest( McpsReq_t* mcpsRequest )
{
    GetPhyParams_t getPhy;
    PhyParam_t phyParam;
    LoRaMacStatus_t status = LORAMAC_STATUS_SERVICE_UNKNOWN;
    LoRaMacHeader_t macHdr;
    VerifyParams_t verify;
    uint8_t fPort = 0;
    void* fBuffer;
    uint16_t fBufferSize;
    int8_t datarate = DR_0;
    bool readyToSend = false;

    if( mcpsRequest == NULL )
    {
        return LORAMAC_STATUS_PARAMETER_INVALID;
    }
    if( LoRaMacIsBusy( ) == true )
    {
        return LORAMAC_STATUS_BUSY;
    }

    macHdr.Value = 0;
    memset1( ( uint8_t* ) &MacCtx.McpsConfirm, 0, sizeof( MacCtx.McpsConfirm ) );
    MacCtx.McpsConfirm.Status = LORAMAC_EVENT_INFO_STATUS_ERROR;

    // AckTimeoutRetriesCounter must be reset every time a new request (unconfirmed or confirmed) is performed.
    MacCtx.AckTimeoutRetriesCounter = 1;

    switch( mcpsRequest->Type )
    {
        case MCPS_UNCONFIRMED:
        {
            readyToSend = true;
            MacCtx.AckTimeoutRetries = 1;

            macHdr.Bits.MType = FRAME_TYPE_DATA_UNCONFIRMED_UP;
            fPort = mcpsRequest->Req.Unconfirmed.fPort;
            fBuffer = mcpsRequest->Req.Unconfirmed.fBuffer;
            fBufferSize = mcpsRequest->Req.Unconfirmed.fBufferSize;
            datarate = mcpsRequest->Req.Unconfirmed.Datarate;
            break;
        }
        case MCPS_CONFIRMED:
        {
            readyToSend = true;
            MacCtx.AckTimeoutRetries = MIN( mcpsRequest->Req.Confirmed.NbTrials, MAX_ACK_RETRIES );

            macHdr.Bits.MType = FRAME_TYPE_DATA_CONFIRMED_UP;
            fPort = mcpsRequest->Req.Confirmed.fPort;
            fBuffer = mcpsRequest->Req.Confirmed.fBuffer;
            fBufferSize = mcpsRequest->Req.Confirmed.fBufferSize;
            datarate = mcpsRequest->Req.Confirmed.Datarate;
            break;
        }
        case MCPS_PROPRIETARY:
        {
            readyToSend = true;
            MacCtx.AckTimeoutRetries = 1;

            macHdr.Bits.MType = FRAME_TYPE_PROPRIETARY;
            fBuffer = mcpsRequest->Req.Proprietary.fBuffer;
            fBufferSize = mcpsRequest->Req.Proprietary.fBufferSize;
            datarate = mcpsRequest->Req.Proprietary.Datarate;
            break;
        }
        default:
            break;
    }

    // Get the minimum possible datarate
    getPhy.Attribute = PHY_MIN_TX_DR;
    getPhy.UplinkDwellTime = MacCtx.NvmCtx->MacParams.UplinkDwellTime;
    phyParam = RegionGetPhyParam( MacCtx.NvmCtx->Region, &getPhy );
    // Apply the minimum possible datarate.
    // Some regions have limitations for the minimum datarate.
    datarate = MAX( datarate, ( int8_t )phyParam.Value );

    if( readyToSend == true )
    {
        if( MacCtx.NvmCtx->AdrCtrlOn == false )
        {
            verify.DatarateParams.Datarate = datarate;
            verify.DatarateParams.UplinkDwellTime = MacCtx.NvmCtx->MacParams.UplinkDwellTime;

            if( RegionVerify( MacCtx.NvmCtx->Region, &verify, PHY_TX_DR ) == true )
            {
                MacCtx.NvmCtx->MacParams.ChannelsDatarate = verify.DatarateParams.Datarate;
            }
            else
            {
                return LORAMAC_STATUS_PARAMETER_INVALID;
            }
        }

        status = Send( &macHdr, fPort, fBuffer, fBufferSize );
        if( status == LORAMAC_STATUS_OK )
        {
            MacCtx.McpsConfirm.McpsRequest = mcpsRequest->Type;
            MacCtx.MacFlags.Bits.McpsReq = 1;
            EventMacNvmCtxChanged( );
        }
        else
        {
            MacCtx.NodeAckRequested = false;
        }
    }

    // Fill return structure
    mcpsRequest->ReqReturn.DutyCycleWaitTime = MacCtx.DutyCycleWaitTime;

    return status;
}

void LoRaMacTestSetDutyCycleOn( bool enable )
{
    VerifyParams_t verify;

    verify.DutyCycle = enable;

    if( RegionVerify( MacCtx.NvmCtx->Region, &verify, PHY_DUTY_CYCLE ) == true )
    {
        MacCtx.NvmCtx->DutyCycleOn = enable;
    }
}

LoRaMacStatus_t LoRaMacDeInitialization( void )
{
    // Check the current state of the LoRaMac
    if ( LoRaMacStop( ) == LORAMAC_STATUS_OK )
    {
        // Stop Timers
        TimerStop( &MacCtx.TxDelayedTimer );
        TimerStop( &MacCtx.RxWindowTimer1 );
        TimerStop( &MacCtx.RxWindowTimer2 );
        TimerStop( &MacCtx.AckTimeoutTimer );

        // Take care about class B
        LoRaMacClassBHaltBeaconing( );

        // Reset Mac parameters
        ResetMacParameters( );

        // Switch off Radio
        Radio.Sleep( );

        // Return success
        return LORAMAC_STATUS_OK;
    }
    else
    {
        return LORAMAC_STATUS_BUSY;
    }
}
