/*!
 * \file      radio.c
 *
 * \brief     Radio driver API definition
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
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 *
 * \author    Gregory Cristian ( Semtech )
 */
#include <math.h>
#include <string.h>
#include "utilities.h"
#include "timer.h"
#include "delay.h"
#include "radio.h"
#include "sx126x.h"
#include "sx126x-board.h"
#include "board.h"

/*!
 * \brief Initializes the radio
 *
 * \param [IN] events Structure containing the driver callback functions
 */
void RadioInit( RadioEvents_t *events );

/*!
 * Return current radio status
 *
 * \param status Radio status.[RF_IDLE, RF_RX_RUNNING, RF_TX_RUNNING]
 */
RadioState_t RadioGetStatus( void );

/*!
 * \brief Configures the radio with the given modem
 *
 * \param [IN] modem Modem to be used [0: FSK, 1: LoRa]
 */
void RadioSetModem( RadioModems_t modem );

/*!
 * \brief Sets the channel frequency
 *
 * \param [IN] freq         Channel RF frequency
 */
void RadioSetChannel( uint32_t freq );

/*!
 * \brief Checks if the channel is free for the given time
 *
 * \param [IN] modem      Radio modem to be used [0: FSK, 1: LoRa]
 * \param [IN] freq       Channel RF frequency
 * \param [IN] rssiThresh RSSI threshold
 * \param [IN] maxCarrierSenseTime Max time while the RSSI is measured
 *
 * \retval isFree         [true: Channel is free, false: Channel is not free]
 */
bool RadioIsChannelFree( RadioModems_t modem, uint32_t freq, int16_t rssiThresh, uint32_t maxCarrierSenseTime );

/*!
 * \brief Generates a 32 bits random value based on the RSSI readings
 *
 * \remark This function sets the radio in LoRa modem mode and disables
 *         all interrupts.
 *         After calling this function either Radio.SetRxConfig or
 *         Radio.SetTxConfig functions must be called.
 *
 * \retval randomValue    32 bits random value
 */
uint32_t RadioRandom( void );

/*!
 * \brief Sets the reception parameters
 *
 * \param [IN] modem        Radio modem to be used [0: FSK, 1: LoRa]
 * \param [IN] bandwidth    Sets the bandwidth
 *                          FSK : >= 2600 and <= 250000 Hz
 *                          LoRa: [0: 125 kHz, 1: 250 kHz,
 *                                 2: 500 kHz, 3: Reserved]
 * \param [IN] datarate     Sets the Datarate
 *                          FSK : 600..300000 bits/s
 *                          LoRa: [6: 64, 7: 128, 8: 256, 9: 512,
 *                                10: 1024, 11: 2048, 12: 4096  chips]
 * \param [IN] coderate     Sets the coding rate (LoRa only)
 *                          FSK : N/A ( set to 0 )
 *                          LoRa: [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]
 * \param [IN] bandwidthAfc Sets the AFC Bandwidth (FSK only)
 *                          FSK : >= 2600 and <= 250000 Hz
 *                          LoRa: N/A ( set to 0 )
 * \param [IN] preambleLen  Sets the Preamble length
 *                          FSK : Number of bytes
 *                          LoRa: Length in symbols (the hardware adds 4 more symbols)
 * \param [IN] symbTimeout  Sets the RxSingle timeout value
 *                          FSK : timeout in number of bytes
 *                          LoRa: timeout in symbols
 * \param [IN] fixLen       Fixed length packets [0: variable, 1: fixed]
 * \param [IN] payloadLen   Sets payload length when fixed length is used
 * \param [IN] crcOn        Enables/Disables the CRC [0: OFF, 1: ON]
 * \param [IN] FreqHopOn    Enables disables the intra-packet frequency hopping
 *                          FSK : N/A ( set to 0 )
 *                          LoRa: [0: OFF, 1: ON]
 * \param [IN] HopPeriod    Number of symbols between each hop
 *                          FSK : N/A ( set to 0 )
 *                          LoRa: Number of symbols
 * \param [IN] iqInverted   Inverts IQ signals (LoRa only)
 *                          FSK : N/A ( set to 0 )
 *                          LoRa: [0: not inverted, 1: inverted]
 * \param [IN] rxContinuous Sets the reception in continuous mode
 *                          [false: single mode, true: continuous mode]
 */
void RadioSetRxConfig( RadioModems_t modem, uint32_t bandwidth,
                          uint32_t datarate, uint8_t coderate,
                          uint32_t bandwidthAfc, uint16_t preambleLen,
                          uint16_t symbTimeout, bool fixLen,
                          uint8_t payloadLen,
                          bool crcOn, bool FreqHopOn, uint8_t HopPeriod,
                          bool iqInverted, bool rxContinuous );

/*!
 * \brief Sets the transmission parameters
 *
 * \param [IN] modem        Radio modem to be used [0: FSK, 1: LoRa]
 * \param [IN] power        Sets the output power [dBm]
 * \param [IN] fdev         Sets the frequency deviation (FSK only)
 *                          FSK : [Hz]
 *                          LoRa: 0
 * \param [IN] bandwidth    Sets the bandwidth (LoRa only)
 *                          FSK : 0
 *                          LoRa: [0: 125 kHz, 1: 250 kHz,
 *                                 2: 500 kHz, 3: Reserved]
 * \param [IN] datarate     Sets the Datarate
 *                          FSK : 600..300000 bits/s
 *                          LoRa: [6: 64, 7: 128, 8: 256, 9: 512,
 *                                10: 1024, 11: 2048, 12: 4096  chips]
 * \param [IN] coderate     Sets the coding rate (LoRa only)
 *                          FSK : N/A ( set to 0 )
 *                          LoRa: [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]
 * \param [IN] preambleLen  Sets the preamble length
 *                          FSK : Number of bytes
 *                          LoRa: Length in symbols (the hardware adds 4 more symbols)
 * \param [IN] fixLen       Fixed length packets [0: variable, 1: fixed]
 * \param [IN] crcOn        Enables disables the CRC [0: OFF, 1: ON]
 * \param [IN] FreqHopOn    Enables disables the intra-packet frequency hopping
 *                          FSK : N/A ( set to 0 )
 *                          LoRa: [0: OFF, 1: ON]
 * \param [IN] HopPeriod    Number of symbols between each hop
 *                          FSK : N/A ( set to 0 )
 *                          LoRa: Number of symbols
 * \param [IN] iqInverted   Inverts IQ signals (LoRa only)
 *                          FSK : N/A ( set to 0 )
 *                          LoRa: [0: not inverted, 1: inverted]
 * \param [IN] timeout      Transmission timeout [ms]
 */
void RadioSetTxConfig( RadioModems_t modem, int8_t power, uint32_t fdev,
                          uint32_t bandwidth, uint32_t datarate,
                          uint8_t coderate, uint16_t preambleLen,
                          bool fixLen, bool crcOn, bool FreqHopOn,
                          uint8_t HopPeriod, bool iqInverted, uint32_t timeout );

/*!
 * \brief Checks if the given RF frequency is supported by the hardware
 *
 * \param [IN] frequency RF frequency to be checked
 * \retval isSupported [true: supported, false: unsupported]
 */
bool RadioCheckRfFrequency( uint32_t frequency );

/*!
 * \brief Computes the packet time on air in ms for the given payload
 *
 * \Remark Can only be called once SetRxConfig or SetTxConfig have been called
 *
 * \param [IN] modem      Radio modem to be used [0: FSK, 1: LoRa]
 * \param [IN] pktLen     Packet payload length
 *
 * \retval airTime        Computed airTime (ms) for the given packet payload length
 */
uint32_t RadioTimeOnAir( RadioModems_t modem, uint8_t pktLen );

/*!
 * \brief Sends the buffer of size. Prepares the packet to be sent and sets
 *        the radio in transmission
 *
 * \param [IN]: buffer     Buffer pointer
 * \param [IN]: size       Buffer size
 */
void RadioSend( uint8_t *buffer, uint8_t size );

/*!
 * \brief Sets the radio in sleep mode
 */
void RadioSleep( void );

/*!
 * \brief Sets the radio in standby mode
 */
void RadioStandby( void );

/*!
 * \brief Sets the radio in reception mode for the given time
 * \param [IN] timeout Reception timeout [ms]
 *                     [0: continuous, others timeout]
 */
void RadioRx( uint32_t timeout );

/*!
 * \brief Start a Channel Activity Detection
 */
void RadioStartCad( void );

/*!
 * \brief Sets the radio in continuous wave transmission mode
 *
 * \param [IN]: freq       Channel RF frequency
 * \param [IN]: power      Sets the output power [dBm]
 * \param [IN]: time       Transmission mode timeout [s]
 */
void RadioSetTxContinuousWave( uint32_t freq, int8_t power, uint16_t time );

/*!
 * \brief Reads the current RSSI value
 *
 * \retval rssiValue Current RSSI value in [dBm]
 */
int16_t RadioRssi( RadioModems_t modem );

/*!
 * \brief Writes the radio register at the specified address
 *
 * \param [IN]: addr Register address
 * \param [IN]: data New register value
 */
void RadioWrite( uint16_t addr, uint8_t data );

/*!
 * \brief Reads the radio register at the specified address
 *
 * \param [IN]: addr Register address
 * \retval data Register value
 */
uint8_t RadioRead( uint16_t addr );

/*!
 * \brief Writes multiple radio registers starting at address
 *
 * \param [IN] addr   First Radio register address
 * \param [IN] buffer Buffer containing the new register's values
 * \param [IN] size   Number of registers to be written
 */
void RadioWriteBuffer( uint16_t addr, uint8_t *buffer, uint8_t size );

/*!
 * \brief Reads multiple radio registers starting at address
 *
 * \param [IN] addr First Radio register address
 * \param [OUT] buffer Buffer where to copy the registers data
 * \param [IN] size Number of registers to be read
 */
void RadioReadBuffer( uint16_t addr, uint8_t *buffer, uint8_t size );

/*!
 * \brief Sets the maximum payload length.
 *
 * \param [IN] modem      Radio modem to be used [0: FSK, 1: LoRa]
 * \param [IN] max        Maximum payload length in bytes
 */
void RadioSetMaxPayloadLength( RadioModems_t modem, uint8_t max );

/*!
 * \brief Sets the network to public or private. Updates the sync byte.
 *
 * \remark Applies to LoRa modem only
 *
 * \param [IN] enable if true, it enables a public network
 */
void RadioSetPublicNetwork( bool enable );

/*!
 * \brief Gets the time required for the board plus radio to get out of sleep.[ms]
 *
 * \retval time Radio plus board wakeup time in ms.
 */
uint32_t RadioGetWakeupTime( void );

/*!
 * \brief Process radio irq
 */
void RadioIrqProcess( void );

/*!
 * \brief Sets the radio in reception mode with Max LNA gain for the given time
 * \param [IN] timeout Reception timeout [ms]
 *                     [0: continuous, others timeout]
 */
void RadioRxBoosted( uint32_t timeout );

/*!
 * \brief Sets the Rx duty cycle management parameters
 *
 * \param [in]  rxTime        Structure describing reception timeout value
 * \param [in]  sleepTime     Structure describing sleep timeout value
 */
void RadioSetRxDutyCycle( uint32_t rxTime, uint32_t sleepTime );

/*!
 * Radio driver structure initialization
 */
const struct Radio_s Radio =
{
    RadioInit,
    RadioGetStatus,
    RadioSetModem,
    RadioSetChannel,
    RadioIsChannelFree,
    RadioRandom,
    RadioSetRxConfig,
    RadioSetTxConfig,
    RadioCheckRfFrequency,
    RadioTimeOnAir,
    RadioSend,
    RadioSleep,
    RadioStandby,
    RadioRx,
    RadioStartCad,
    RadioSetTxContinuousWave,
    RadioRssi,
    RadioWrite,
    RadioRead,
    RadioWriteBuffer,
    RadioReadBuffer,
    RadioSetMaxPayloadLength,
    RadioSetPublicNetwork,
    RadioGetWakeupTime,
    RadioIrqProcess,
    // Available on SX126x only
    RadioRxBoosted,
    RadioSetRxDutyCycle
};

/*
 * Local types definition
 */


 /*!
 * FSK bandwidth definition
 */
typedef struct
{
    uint32_t bandwidth;
    uint8_t  RegValue;
}FskBandwidth_t;

/*!
 * Precomputed FSK bandwidth registers values
 */
const FskBandwidth_t FskBandwidths[] =
{
    { 4800  , 0x1F },
    { 5800  , 0x17 },
    { 7300  , 0x0F },
    { 9700  , 0x1E },
    { 11700 , 0x16 },
    { 14600 , 0x0E },
    { 19500 , 0x1D },
    { 23400 , 0x15 },
    { 29300 , 0x0D },
    { 39000 , 0x1C },
    { 46900 , 0x14 },
    { 58600 , 0x0C },
    { 78200 , 0x1B },
    { 93800 , 0x13 },
    { 117300, 0x0B },
    { 156200, 0x1A },
    { 187200, 0x12 },
    { 234300, 0x0A },
    { 312000, 0x19 },
    { 373600, 0x11 },
    { 467000, 0x09 },
    { 500000, 0x00 }, // Invalid Bandwidth
};

const RadioLoRaBandwidths_t Bandwidths[] = { LORA_BW_125, LORA_BW_250, LORA_BW_500 };

//                                          SF12    SF11    SF10    SF9    SF8    SF7
static double RadioLoRaSymbTime[3][6] = {{ 32.768, 16.384, 8.192, 4.096, 2.048, 1.024 },  // 125 KHz
                                         { 16.384, 8.192,  4.096, 2.048, 1.024, 0.512 },  // 250 KHz
                                         { 8.192,  4.096,  2.048, 1.024, 0.512, 0.256 }}; // 500 KHz

uint8_t MaxPayloadLength = 0xFF;

uint32_t TxTimeout = 0;
uint32_t RxTimeout = 0;

bool RxContinuous = false;


PacketStatus_t RadioPktStatus;
uint8_t RadioRxPayload[255];

bool IrqFired = false;

/*
 * SX126x DIO IRQ callback functions prototype
 */

/*!
 * \brief DIO 0 IRQ callback
 */
void RadioOnDioIrq( void* context );

/*!
 * \brief Tx timeout timer callback
 */
void RadioOnTxTimeoutIrq( void* context );

/*!
 * \brief Rx timeout timer callback
 */
void RadioOnRxTimeoutIrq( void* context );

/*
 * Private global variables
 */


/*!
 * Holds the current network type for the radio
 */
typedef struct
{
    bool Previous;
    bool Current;
}RadioPublicNetwork_t;

static RadioPublicNetwork_t RadioPublicNetwork = { false };

/*!
 * Radio callbacks variable
 */
static RadioEvents_t* RadioEvents;

/*
 * Public global variables
 */

/*!
 * Radio hardware and global parameters
 */
SX126x_t SX126x;

/*!
 * Tx and Rx timers
 */
TimerEvent_t TxTimeoutTimer;
TimerEvent_t RxTimeoutTimer;

/*!
 * Returns the known FSK bandwidth registers value
 *
 * \param [IN] bandwidth Bandwidth value in Hz
 * \retval regValue Bandwidth register value.
 */
static uint8_t RadioGetFskBandwidthRegValue( uint32_t bandwidth )
{
    uint8_t i;

    if( bandwidth == 0 )
    {
        return( 0x1F );
    }

    for( i = 0; i < ( sizeof( FskBandwidths ) / sizeof( FskBandwidth_t ) ) - 1; i++ )
    {
        if( ( bandwidth >= FskBandwidths[i].bandwidth ) && ( bandwidth < FskBandwidths[i + 1].bandwidth ) )
        {
            return FskBandwidths[i+1].RegValue;
        }
    }
    // ERROR: Value not found
    while( 1 );
}

void RadioInit( RadioEvents_t *events )
{
    RadioEvents = events;

    SX126xInit( RadioOnDioIrq );
    SX126xSetStandby( STDBY_RC );
    SX126xSetRegulatorMode( USE_DCDC );

    SX126xSetBufferBaseAddress( 0x00, 0x00 );
    SX126xSetTxParams( 0, RADIO_RAMP_200_US );
    SX126xSetDioIrqParams( IRQ_RADIO_ALL, IRQ_RADIO_ALL, IRQ_RADIO_NONE, IRQ_RADIO_NONE );

    // Initialize driver timeout timers
    TimerInit( &TxTimeoutTimer, RadioOnTxTimeoutIrq );
    TimerInit( &RxTimeoutTimer, RadioOnRxTimeoutIrq );

    IrqFired = false;
}

RadioState_t RadioGetStatus( void )
{
    switch( SX126xGetOperatingMode( ) )
    {
        case MODE_TX:
            return RF_TX_RUNNING;
        case MODE_RX:
            return RF_RX_RUNNING;
        case MODE_CAD:
            return RF_CAD;
        default:
            return RF_IDLE;
    }
}

void RadioSetModem( RadioModems_t modem )
{
    switch( modem )
    {
    default:
    case MODEM_FSK:
        SX126xSetPacketType( PACKET_TYPE_GFSK );
        // When switching to GFSK mode the LoRa SyncWord register value is reset
        // Thus, we also reset the RadioPublicNetwork variable
        RadioPublicNetwork.Current = false;
        break;
    case MODEM_LORA:
        SX126xSetPacketType( PACKET_TYPE_LORA );
        // Public/Private network register is reset when switching modems
        if( RadioPublicNetwork.Current != RadioPublicNetwork.Previous )
        {
            RadioPublicNetwork.Current = RadioPublicNetwork.Previous;
            RadioSetPublicNetwork( RadioPublicNetwork.Current );
        }
        break;
    }
}

void RadioSetChannel( uint32_t freq )
{
    SX126xSetRfFrequency( freq );
}

bool RadioIsChannelFree( RadioModems_t modem, uint32_t freq, int16_t rssiThresh, uint32_t maxCarrierSenseTime )
{
    bool status = true;
    int16_t rssi = 0;
    uint32_t carrierSenseTime = 0;

    RadioSetModem( modem );

    RadioSetChannel( freq );

    RadioRx( 0 );

    DelayMs( 1 );

    carrierSenseTime = TimerGetCurrentTime( );

    // Perform carrier sense for maxCarrierSenseTime
    while( TimerGetElapsedTime( carrierSenseTime ) < maxCarrierSenseTime )
    {
        rssi = RadioRssi( modem );

        if( rssi > rssiThresh )
        {
            status = false;
            break;
        }
    }
    RadioSleep( );
    return status;
}

uint32_t RadioRandom( void )
{
    uint8_t i;
    uint32_t rnd = 0;

    /*
     * Radio setup for random number generation
     */
    // Set LoRa modem ON
    RadioSetModem( MODEM_LORA );

    // Set radio in continuous reception
    SX126xSetRx( 0 );

    for( i = 0; i < 32; i++ )
    {
        DelayMs( 1 );
        // Unfiltered RSSI value reading. Only takes the LSB value
        rnd |= ( ( uint32_t )SX126xGetRssiInst( ) & 0x01 ) << i;
    }

    RadioSleep( );

    return rnd;
}

void RadioSetRxConfig( RadioModems_t modem, uint32_t bandwidth,
                         uint32_t datarate, uint8_t coderate,
                         uint32_t bandwidthAfc, uint16_t preambleLen,
                         uint16_t symbTimeout, bool fixLen,
                         uint8_t payloadLen,
                         bool crcOn, bool freqHopOn, uint8_t hopPeriod,
                         bool iqInverted, bool rxContinuous )
{

    RxContinuous = rxContinuous;
    if( rxContinuous == true )
    {
        symbTimeout = 0;
    }
    if( fixLen == true )
    {
        MaxPayloadLength = payloadLen;
    }
    else
    {
        MaxPayloadLength = 0xFF;
    }

    switch( modem )
    {
        case MODEM_FSK:
            SX126xSetStopRxTimerOnPreambleDetect( false );
            SX126x.ModulationParams.PacketType = PACKET_TYPE_GFSK;

            SX126x.ModulationParams.Params.Gfsk.BitRate = datarate;
            SX126x.ModulationParams.Params.Gfsk.ModulationShaping = MOD_SHAPING_G_BT_1;
            SX126x.ModulationParams.Params.Gfsk.Bandwidth = RadioGetFskBandwidthRegValue( bandwidth );

            SX126x.PacketParams.PacketType = PACKET_TYPE_GFSK;
            SX126x.PacketParams.Params.Gfsk.PreambleLength = ( preambleLen << 3 ); // convert byte into bit
            SX126x.PacketParams.Params.Gfsk.PreambleMinDetect = RADIO_PREAMBLE_DETECTOR_08_BITS;
            SX126x.PacketParams.Params.Gfsk.SyncWordLength = 3 << 3; // convert byte into bit
            SX126x.PacketParams.Params.Gfsk.AddrComp = RADIO_ADDRESSCOMP_FILT_OFF;
            SX126x.PacketParams.Params.Gfsk.HeaderType = ( fixLen == true ) ? RADIO_PACKET_FIXED_LENGTH : RADIO_PACKET_VARIABLE_LENGTH;
            SX126x.PacketParams.Params.Gfsk.PayloadLength = MaxPayloadLength;
            if( crcOn == true )
            {
                SX126x.PacketParams.Params.Gfsk.CrcLength = RADIO_CRC_2_BYTES_CCIT;
            }
            else
            {
                SX126x.PacketParams.Params.Gfsk.CrcLength = RADIO_CRC_OFF;
            }
            SX126x.PacketParams.Params.Gfsk.DcFree = RADIO_DC_FREEWHITENING;

            RadioStandby( );
            RadioSetModem( ( SX126x.ModulationParams.PacketType == PACKET_TYPE_GFSK ) ? MODEM_FSK : MODEM_LORA );
            SX126xSetModulationParams( &SX126x.ModulationParams );
            SX126xSetPacketParams( &SX126x.PacketParams );
            SX126xSetSyncWord( ( uint8_t[] ){ 0xC1, 0x94, 0xC1, 0x00, 0x00, 0x00, 0x00, 0x00 } );
            SX126xSetWhiteningSeed( 0x01FF );

            RxTimeout = ( uint32_t )( symbTimeout * ( ( 1.0 / ( double )datarate ) * 8.0 ) * 1000 );
            break;

        case MODEM_LORA:
            SX126xSetStopRxTimerOnPreambleDetect( false );
            SX126xSetLoRaSymbNumTimeout( symbTimeout );
            SX126x.ModulationParams.PacketType = PACKET_TYPE_LORA;
            SX126x.ModulationParams.Params.LoRa.SpreadingFactor = ( RadioLoRaSpreadingFactors_t )datarate;
            SX126x.ModulationParams.Params.LoRa.Bandwidth = Bandwidths[bandwidth];
            SX126x.ModulationParams.Params.LoRa.CodingRate = ( RadioLoRaCodingRates_t )coderate;

            if( ( ( bandwidth == 0 ) && ( ( datarate == 11 ) || ( datarate == 12 ) ) ) ||
            ( ( bandwidth == 1 ) && ( datarate == 12 ) ) )
            {
                SX126x.ModulationParams.Params.LoRa.LowDatarateOptimize = 0x01;
            }
            else
            {
                SX126x.ModulationParams.Params.LoRa.LowDatarateOptimize = 0x00;
            }

            SX126x.PacketParams.PacketType = PACKET_TYPE_LORA;

            if( ( SX126x.ModulationParams.Params.LoRa.SpreadingFactor == LORA_SF5 ) ||
                ( SX126x.ModulationParams.Params.LoRa.SpreadingFactor == LORA_SF6 ) )
            {
                if( preambleLen < 12 )
                {
                    SX126x.PacketParams.Params.LoRa.PreambleLength = 12;
                }
                else
                {
                    SX126x.PacketParams.Params.LoRa.PreambleLength = preambleLen;
                }
            }
            else
            {
                SX126x.PacketParams.Params.LoRa.PreambleLength = preambleLen;
            }

            SX126x.PacketParams.Params.LoRa.HeaderType = ( RadioLoRaPacketLengthsMode_t )fixLen;

            SX126x.PacketParams.Params.LoRa.PayloadLength = MaxPayloadLength;
            SX126x.PacketParams.Params.LoRa.CrcMode = ( RadioLoRaCrcModes_t )crcOn;
            SX126x.PacketParams.Params.LoRa.InvertIQ = ( RadioLoRaIQModes_t )iqInverted;

            RadioSetModem( ( SX126x.ModulationParams.PacketType == PACKET_TYPE_GFSK ) ? MODEM_FSK : MODEM_LORA );
            SX126xSetModulationParams( &SX126x.ModulationParams );
            SX126xSetPacketParams( &SX126x.PacketParams );

            // Timeout Max, Timeout handled directly in SetRx function
            RxTimeout = 0xFFFF;

            break;
    }
}

void RadioSetTxConfig( RadioModems_t modem, int8_t power, uint32_t fdev,
                        uint32_t bandwidth, uint32_t datarate,
                        uint8_t coderate, uint16_t preambleLen,
                        bool fixLen, bool crcOn, bool freqHopOn,
                        uint8_t hopPeriod, bool iqInverted, uint32_t timeout )
{

    switch( modem )
    {
        case MODEM_FSK:
            SX126x.ModulationParams.PacketType = PACKET_TYPE_GFSK;
            SX126x.ModulationParams.Params.Gfsk.BitRate = datarate;

            SX126x.ModulationParams.Params.Gfsk.ModulationShaping = MOD_SHAPING_G_BT_1;
            SX126x.ModulationParams.Params.Gfsk.Bandwidth = RadioGetFskBandwidthRegValue( bandwidth );
            SX126x.ModulationParams.Params.Gfsk.Fdev = fdev;

            SX126x.PacketParams.PacketType = PACKET_TYPE_GFSK;
            SX126x.PacketParams.Params.Gfsk.PreambleLength = ( preambleLen << 3 ); // convert byte into bit
            SX126x.PacketParams.Params.Gfsk.PreambleMinDetect = RADIO_PREAMBLE_DETECTOR_08_BITS;
            SX126x.PacketParams.Params.Gfsk.SyncWordLength = 3 << 3 ; // convert byte into bit
            SX126x.PacketParams.Params.Gfsk.AddrComp = RADIO_ADDRESSCOMP_FILT_OFF;
            SX126x.PacketParams.Params.Gfsk.HeaderType = ( fixLen == true ) ? RADIO_PACKET_FIXED_LENGTH : RADIO_PACKET_VARIABLE_LENGTH;

            if( crcOn == true )
            {
                SX126x.PacketParams.Params.Gfsk.CrcLength = RADIO_CRC_2_BYTES_CCIT;
            }
            else
            {
                SX126x.PacketParams.Params.Gfsk.CrcLength = RADIO_CRC_OFF;
            }
            SX126x.PacketParams.Params.Gfsk.DcFree = RADIO_DC_FREEWHITENING;

            RadioStandby( );
            RadioSetModem( ( SX126x.ModulationParams.PacketType == PACKET_TYPE_GFSK ) ? MODEM_FSK : MODEM_LORA );
            SX126xSetModulationParams( &SX126x.ModulationParams );
            SX126xSetPacketParams( &SX126x.PacketParams );
            SX126xSetSyncWord( ( uint8_t[] ){ 0xC1, 0x94, 0xC1, 0x00, 0x00, 0x00, 0x00, 0x00 } );
            SX126xSetWhiteningSeed( 0x01FF );
            break;

        case MODEM_LORA:
            SX126x.ModulationParams.PacketType = PACKET_TYPE_LORA;
            SX126x.ModulationParams.Params.LoRa.SpreadingFactor = ( RadioLoRaSpreadingFactors_t ) datarate;
            SX126x.ModulationParams.Params.LoRa.Bandwidth =  Bandwidths[bandwidth];
            SX126x.ModulationParams.Params.LoRa.CodingRate= ( RadioLoRaCodingRates_t )coderate;

            if( ( ( bandwidth == 0 ) && ( ( datarate == 11 ) || ( datarate == 12 ) ) ) ||
            ( ( bandwidth == 1 ) && ( datarate == 12 ) ) )
            {
                SX126x.ModulationParams.Params.LoRa.LowDatarateOptimize = 0x01;
            }
            else
            {
                SX126x.ModulationParams.Params.LoRa.LowDatarateOptimize = 0x00;
            }

            SX126x.PacketParams.PacketType = PACKET_TYPE_LORA;

            if( ( SX126x.ModulationParams.Params.LoRa.SpreadingFactor == LORA_SF5 ) ||
                ( SX126x.ModulationParams.Params.LoRa.SpreadingFactor == LORA_SF6 ) )
            {
                if( preambleLen < 12 )
                {
                    SX126x.PacketParams.Params.LoRa.PreambleLength = 12;
                }
                else
                {
                    SX126x.PacketParams.Params.LoRa.PreambleLength = preambleLen;
                }
            }
            else
            {
                SX126x.PacketParams.Params.LoRa.PreambleLength = preambleLen;
            }

            SX126x.PacketParams.Params.LoRa.HeaderType = ( RadioLoRaPacketLengthsMode_t )fixLen;
            SX126x.PacketParams.Params.LoRa.PayloadLength = MaxPayloadLength;
            SX126x.PacketParams.Params.LoRa.CrcMode = ( RadioLoRaCrcModes_t )crcOn;
            SX126x.PacketParams.Params.LoRa.InvertIQ = ( RadioLoRaIQModes_t )iqInverted;

            RadioStandby( );
            RadioSetModem( ( SX126x.ModulationParams.PacketType == PACKET_TYPE_GFSK ) ? MODEM_FSK : MODEM_LORA );
            SX126xSetModulationParams( &SX126x.ModulationParams );
            SX126xSetPacketParams( &SX126x.PacketParams );
            break;
    }
    SX126xSetRfTxPower( power );
    TxTimeout = timeout;
}

bool RadioCheckRfFrequency( uint32_t frequency )
{
    return true;
}

uint32_t RadioTimeOnAir( RadioModems_t modem, uint8_t pktLen )
{
    uint32_t airTime = 0;

    switch( modem )
    {
    case MODEM_FSK:
        {
           airTime = rint( ( 8 * ( SX126x.PacketParams.Params.Gfsk.PreambleLength +
                                     ( SX126x.PacketParams.Params.Gfsk.SyncWordLength >> 3 ) +
                                     ( ( SX126x.PacketParams.Params.Gfsk.HeaderType == RADIO_PACKET_FIXED_LENGTH ) ? 0.0 : 1.0 ) +
                                     pktLen +
                                     ( ( SX126x.PacketParams.Params.Gfsk.CrcLength == RADIO_CRC_2_BYTES ) ? 2.0 : 0 ) ) /
                                     SX126x.ModulationParams.Params.Gfsk.BitRate ) * 1e3 );
        }
        break;
    case MODEM_LORA:
        {
            double ts = RadioLoRaSymbTime[SX126x.ModulationParams.Params.LoRa.Bandwidth - 4][12 - SX126x.ModulationParams.Params.LoRa.SpreadingFactor];
            // time of preamble
            double tPreamble = ( SX126x.PacketParams.Params.LoRa.PreambleLength + 4.25 ) * ts;
            // Symbol length of payload and time
            double tmp = ceil( ( 8 * pktLen - 4 * SX126x.ModulationParams.Params.LoRa.SpreadingFactor +
                                 28 + 16 * SX126x.PacketParams.Params.LoRa.CrcMode -
                                 ( ( SX126x.PacketParams.Params.LoRa.HeaderType == LORA_PACKET_FIXED_LENGTH ) ? 20 : 0 ) ) /
                                 ( double )( 4 * ( SX126x.ModulationParams.Params.LoRa.SpreadingFactor -
                                 ( ( SX126x.ModulationParams.Params.LoRa.LowDatarateOptimize > 0 ) ? 2 : 0 ) ) ) ) *
                                 ( ( SX126x.ModulationParams.Params.LoRa.CodingRate % 4 ) + 4 );
            double nPayload = 8 + ( ( tmp > 0 ) ? tmp : 0 );
            double tPayload = nPayload * ts;
            // Time on air
            double tOnAir = tPreamble + tPayload;
            // return milli seconds
            airTime = floor( tOnAir + 0.999 );
        }
        break;
    }
    return airTime;
}

void RadioSend( uint8_t *buffer, uint8_t size )
{
    SX126xSetDioIrqParams( IRQ_TX_DONE | IRQ_RX_TX_TIMEOUT,
                           IRQ_TX_DONE | IRQ_RX_TX_TIMEOUT,
                           IRQ_RADIO_NONE,
                           IRQ_RADIO_NONE );

    if( SX126xGetPacketType( ) == PACKET_TYPE_LORA )
    {
        SX126x.PacketParams.Params.LoRa.PayloadLength = size;
    }
    else
    {
        SX126x.PacketParams.Params.Gfsk.PayloadLength = size;
    }
    SX126xSetPacketParams( &SX126x.PacketParams );

    SX126xSendPayload( buffer, size, 0 );
    TimerSetValue( &TxTimeoutTimer, TxTimeout );
    TimerStart( &TxTimeoutTimer );
}

void RadioSleep( void )
{
    SleepParams_t params = { 0 };

    params.Fields.WarmStart = 1;
    SX126xSetSleep( params );

    DelayMs( 2 );
}

void RadioStandby( void )
{
    SX126xSetStandby( STDBY_RC );
}

void RadioRx( uint32_t timeout )
{
    SX126xSetDioIrqParams( IRQ_RADIO_ALL, //IRQ_RX_DONE | IRQ_RX_TX_TIMEOUT,
                           IRQ_RADIO_ALL, //IRQ_RX_DONE | IRQ_RX_TX_TIMEOUT,
                           IRQ_RADIO_NONE,
                           IRQ_RADIO_NONE );

    if( timeout != 0 )
    {
        TimerSetValue( &RxTimeoutTimer, timeout );
        TimerStart( &RxTimeoutTimer );
    }

    if( RxContinuous == true )
    {
        SX126xSetRx( 0xFFFFFF ); // Rx Continuous
    }
    else
    {
        SX126xSetRx( RxTimeout << 6 );
    }
}

void RadioRxBoosted( uint32_t timeout )
{
    SX126xSetDioIrqParams( IRQ_RADIO_ALL, //IRQ_RX_DONE | IRQ_RX_TX_TIMEOUT,
                           IRQ_RADIO_ALL, //IRQ_RX_DONE | IRQ_RX_TX_TIMEOUT,
                           IRQ_RADIO_NONE,
                           IRQ_RADIO_NONE );

    if( timeout != 0 )
    {
        TimerSetValue( &RxTimeoutTimer, timeout );
        TimerStart( &RxTimeoutTimer );
    }

    if( RxContinuous == true )
    {
        SX126xSetRxBoosted( 0xFFFFFF ); // Rx Continuous
    }
    else
    {
        SX126xSetRxBoosted( RxTimeout << 6 );
    }
}

void RadioSetRxDutyCycle( uint32_t rxTime, uint32_t sleepTime )
{
    SX126xSetRxDutyCycle( rxTime, sleepTime );
}

void RadioStartCad( void )
{
    SX126xSetCad( );
}

void RadioTx( uint32_t timeout )
{
    SX126xSetTx( timeout << 6 );
}

void RadioSetTxContinuousWave( uint32_t freq, int8_t power, uint16_t time )
{
    SX126xSetRfFrequency( freq );
    SX126xSetRfTxPower( power );
    SX126xSetTxContinuousWave( );

    TimerSetValue( &TxTimeoutTimer, time  * 1e3 );
    TimerStart( &TxTimeoutTimer );
}

int16_t RadioRssi( RadioModems_t modem )
{
    return SX126xGetRssiInst( );
}

void RadioWrite( uint16_t addr, uint8_t data )
{
    SX126xWriteRegister( addr, data );
}

uint8_t RadioRead( uint16_t addr )
{
    return SX126xReadRegister( addr );
}

void RadioWriteBuffer( uint16_t addr, uint8_t *buffer, uint8_t size )
{
    SX126xWriteRegisters( addr, buffer, size );
}

void RadioReadBuffer( uint16_t addr, uint8_t *buffer, uint8_t size )
{
    SX126xReadRegisters( addr, buffer, size );
}

void RadioWriteFifo( uint8_t *buffer, uint8_t size )
{
    SX126xWriteBuffer( 0, buffer, size );
}

void RadioReadFifo( uint8_t *buffer, uint8_t size )
{
    SX126xReadBuffer( 0, buffer, size );
}

void RadioSetMaxPayloadLength( RadioModems_t modem, uint8_t max )
{
    if( modem == MODEM_LORA )
    {
        SX126x.PacketParams.Params.LoRa.PayloadLength = MaxPayloadLength = max;
        SX126xSetPacketParams( &SX126x.PacketParams );
    }
    else
    {
        if( SX126x.PacketParams.Params.Gfsk.HeaderType == RADIO_PACKET_VARIABLE_LENGTH )
        {
            SX126x.PacketParams.Params.Gfsk.PayloadLength = MaxPayloadLength = max;
            SX126xSetPacketParams( &SX126x.PacketParams );
        }
    }
}

void RadioSetPublicNetwork( bool enable )
{
    RadioPublicNetwork.Current = RadioPublicNetwork.Previous = enable;

    RadioSetModem( MODEM_LORA );
    if( enable == true )
    {
        // Change LoRa modem SyncWord
        SX126xWriteRegister( REG_LR_SYNCWORD, ( LORA_MAC_PUBLIC_SYNCWORD >> 8 ) & 0xFF );
        SX126xWriteRegister( REG_LR_SYNCWORD + 1, LORA_MAC_PUBLIC_SYNCWORD & 0xFF );
    }
    else
    {
        // Change LoRa modem SyncWord
        SX126xWriteRegister( REG_LR_SYNCWORD, ( LORA_MAC_PRIVATE_SYNCWORD >> 8 ) & 0xFF );
        SX126xWriteRegister( REG_LR_SYNCWORD + 1, LORA_MAC_PRIVATE_SYNCWORD & 0xFF );
    }
}

uint32_t RadioGetWakeupTime( void )
{
    return SX126xGetBoardTcxoWakeupTime( ) + RADIO_WAKEUP_TIME;
}

void RadioOnTxTimeoutIrq( void* context )
{
    if( ( RadioEvents != NULL ) && ( RadioEvents->TxTimeout != NULL ) )
    {
        RadioEvents->TxTimeout( );
    }
}

void RadioOnRxTimeoutIrq( void* context )
{
    if( ( RadioEvents != NULL ) && ( RadioEvents->RxTimeout != NULL ) )
    {
        RadioEvents->RxTimeout( );
    }
}

void RadioOnDioIrq( void* context )
{
    IrqFired = true;
}

void RadioIrqProcess( void )
{
    if( IrqFired == true )
    {
        CRITICAL_SECTION_BEGIN( );
        // Clear IRQ flag
        IrqFired = false;
        CRITICAL_SECTION_END( );

        uint16_t irqRegs = SX126xGetIrqStatus( );
        SX126xClearIrqStatus( IRQ_RADIO_ALL );

        if( ( irqRegs & IRQ_TX_DONE ) == IRQ_TX_DONE )
        {
            TimerStop( &TxTimeoutTimer );
            //!< Update operating mode state to a value lower than \ref MODE_STDBY_XOSC
            SX126xSetOperatingMode( MODE_STDBY_RC );
            if( ( RadioEvents != NULL ) && ( RadioEvents->TxDone != NULL ) )
            {
                RadioEvents->TxDone( );
            }
        }

        if( ( irqRegs & IRQ_RX_DONE ) == IRQ_RX_DONE )
        {
            uint8_t size;

            TimerStop( &RxTimeoutTimer );
            if( RxContinuous == false )
            {
                //!< Update operating mode state to a value lower than \ref MODE_STDBY_XOSC
                SX126xSetOperatingMode( MODE_STDBY_RC );
            }
            SX126xGetPayload( RadioRxPayload, &size , 255 );
            SX126xGetPacketStatus( &RadioPktStatus );
            if( ( RadioEvents != NULL ) && ( RadioEvents->RxDone != NULL ) )
            {
                RadioEvents->RxDone( RadioRxPayload, size, RadioPktStatus.Params.LoRa.RssiPkt, RadioPktStatus.Params.LoRa.SnrPkt );
            }
        }

        if( ( irqRegs & IRQ_CRC_ERROR ) == IRQ_CRC_ERROR )
        {
            if( RxContinuous == false )
            {
                //!< Update operating mode state to a value lower than \ref MODE_STDBY_XOSC
                SX126xSetOperatingMode( MODE_STDBY_RC );
            }
            if( ( RadioEvents != NULL ) && ( RadioEvents->RxError ) )
            {
                RadioEvents->RxError( );
            }
        }

        if( ( irqRegs & IRQ_CAD_DONE ) == IRQ_CAD_DONE )
        {
            //!< Update operating mode state to a value lower than \ref MODE_STDBY_XOSC
            SX126xSetOperatingMode( MODE_STDBY_RC );
            if( ( RadioEvents != NULL ) && ( RadioEvents->CadDone != NULL ) )
            {
                RadioEvents->CadDone( ( ( irqRegs & IRQ_CAD_ACTIVITY_DETECTED ) == IRQ_CAD_ACTIVITY_DETECTED ) );
            }
        }

        if( ( irqRegs & IRQ_RX_TX_TIMEOUT ) == IRQ_RX_TX_TIMEOUT )
        {
            if( SX126xGetOperatingMode( ) == MODE_TX )
            {
                TimerStop( &TxTimeoutTimer );
                //!< Update operating mode state to a value lower than \ref MODE_STDBY_XOSC
                SX126xSetOperatingMode( MODE_STDBY_RC );
                if( ( RadioEvents != NULL ) && ( RadioEvents->TxTimeout != NULL ) )
                {
                    RadioEvents->TxTimeout( );
                }
            }
            else if( SX126xGetOperatingMode( ) == MODE_RX )
            {
                TimerStop( &RxTimeoutTimer );
                //!< Update operating mode state to a value lower than \ref MODE_STDBY_XOSC
                SX126xSetOperatingMode( MODE_STDBY_RC );
                if( ( RadioEvents != NULL ) && ( RadioEvents->RxTimeout != NULL ) )
                {
                    RadioEvents->RxTimeout( );
                }
            }
        }

        if( ( irqRegs & IRQ_PREAMBLE_DETECTED ) == IRQ_PREAMBLE_DETECTED )
        {
            //__NOP( );
        }

        if( ( irqRegs & IRQ_SYNCWORD_VALID ) == IRQ_SYNCWORD_VALID )
        {
            //__NOP( );
        }

        if( ( irqRegs & IRQ_HEADER_VALID ) == IRQ_HEADER_VALID )
        {
            //__NOP( );
        }

        if( ( irqRegs & IRQ_HEADER_ERROR ) == IRQ_HEADER_ERROR )
        {
            TimerStop( &RxTimeoutTimer );
            if( RxContinuous == false )
            {
                //!< Update operating mode state to a value lower than \ref MODE_STDBY_XOSC
                SX126xSetOperatingMode( MODE_STDBY_RC );
            }
            if( ( RadioEvents != NULL ) && ( RadioEvents->RxTimeout != NULL ) )
            {
                RadioEvents->RxTimeout( );
            }
        }
    }
}
