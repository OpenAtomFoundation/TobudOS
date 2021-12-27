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
/**
  ******************************************************************************
  *  
  *          Portions COPYRIGHT 2019 STMicroelectronics                       
  *
  * @file    radio.c
  * @author  MCD Application Team
  * @brief   radio API definition
  ******************************************************************************
 */
/* Includes ------------------------------------------------------------------*/
#include <math.h>

#include "radio.h"
#include "timer.h"
#include "radio_driver.h"
#include "radio_conf.h"
#include "mw_log_conf.h"

/*
 * Local types definition
 */
/*
 * Private types
 */
/*!
 * Radio hardware and global parameters
 */
typedef struct SubgRf_s
{
  RadioModems_t modem;
  bool RxContinuous;
  uint32_t TxTimeout;
  uint32_t RxTimeout;
  bool publicNetwork;
  PacketParams_t PacketParams;
  PacketStatus_t PacketStatus;
  ModulationParams_t ModulationParams;
  RadioIrqMasks_t radioIrq;
}SubgRf_t;

#define RADIO_BIT_MASK(__n)  (~(1<<__n))

/*
 * Private functions prototypes
 */


/* **********************************************************************
 * Interrupts functions prototypes
 ************************************************************************/
/*!
 * @brief Process radio irq
 */
void RadioIrqProcess( void );

/*!
 * @brief Tx timeout timer callback
 */
void RadioOnTxTimeoutIrq( void * context );

/*!
 * @brief Rx timeout timer callback
 */
void RadioOnRxTimeoutIrq( void * context );


/* **********************************************************************
 * Middleware Interface functions prototypes
 ************************************************************************/

/*!
 * \brief Initializes the radio
 *
 * \param [IN] events Structure containing the driver callback functions
 */
uint32_t RadioInit( RadioEvents_t *events );

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
uint32_t RadioGetWakeUpTime( void );


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
 * \brief Set Tx
 * \param  timeout
 * \retval none
 */
void RadioTx( uint32_t timeout );



/*!
* @brief D-BPSK to BPSK
*
* @param [in]      inBuffer         buffer with frame to encode
* @param [out]     OutBuffer        buffer with frame encoded
* @param [in]      size            size of the payload to encode
*/
static void payload_integration( uint8_t *outBuffer, uint8_t *inBuffer, uint8_t size);

/*!
 * \brief Set Tx PRBS modulated wave
 * \retval none
 */
void RadioTxPrbs( void );

/*!
 * \brief Set Tx continuous wave
 * \retval none
 */
void RadioTxCw( void );

 /*
 * Private global constants
 */

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
    RadioSetMaxPayloadLength,
    RadioSetPublicNetwork,
    RadioGetWakeUpTime,
    RadioIrqProcess,
    RadioRxBoosted,
    RadioSetRxDutyCycle,
    RadioTxPrbs,
    RadioTxCw,
};



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
static const double RadioLoRaSymbTime[3][6] = {{ 32.768, 16.384, 8.192, 4.096, 2.048, 1.024 },  // 125 KHz
                                         { 16.384, 8.192,  4.096, 2.048, 1.024, 0.512 },  // 250 KHz
                                         { 8.192,  4.096,  2.048, 1.024, 0.512, 0.256 }}; // 500 KHz

static uint8_t MaxPayloadLength = RADIO_RX_BUF_SIZE;

 /* RF switch Power variable */
static uint8_t AntSwitchPower;

static uint8_t RadioRxPayload[RADIO_RX_BUF_SIZE];

bool IrqFired = false;

/*
 * Private global variables
 */




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
SubgRf_t SubgRf;


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

    for( i = 0; i < ( sizeof( FskBandwidths ) / sizeof( FskBandwidth_t ) ); i++ )
    {
        if ( bandwidth < FskBandwidths[i].bandwidth )
        {
            return FskBandwidths[i].RegValue;
        }
    }
    // ERROR: Value not found
    while(1){}
}

uint32_t RadioInit( RadioEvents_t *events )
{
    RadioEvents = events;
    
    SubgRf.RxContinuous = false;
    SubgRf.TxTimeout = 0;
    SubgRf.RxTimeout = 0;

    if( SUBGRF_Init( RadioOnDioIrq ) != SUBGRF_OK)
    {
     /* Initialization Error */
      while(1){}
    }
    SUBGRF_SetStandby( STDBY_RC );
    /*SubgRf.publicNetwork set to false*/
    RadioSetPublicNetwork( false );
    
    SUBGRF_SetRegulatorMode(  );

    SUBGRF_SetBufferBaseAddress( 0x00, 0x00 );
 
    SUBGRF_SetTxParams(RFO_LP, 0, RADIO_RAMP_200_US);
    
   /* Radio IRQ is set to DIO1 by default */
    SUBGRF_SetDioIrqParams( IRQ_RADIO_ALL, IRQ_RADIO_ALL, IRQ_RADIO_NONE, IRQ_RADIO_NONE );


    // Initialize driver timeout timers
    TimerInit( &TxTimeoutTimer, RadioOnTxTimeoutIrq );
    TimerInit( &RxTimeoutTimer, RadioOnRxTimeoutIrq );
    TimerStop( &TxTimeoutTimer );
    TimerStop( &RxTimeoutTimer );
    
    IrqFired = false;
    return ( uint32_t )RadioGetWakeUpTime( );

}

RadioState_t RadioGetStatus( void )
{
    switch( SUBGRF_GetOperatingMode( ) )
    {
        case MODE_TX:
            return RF_TX_RUNNING;
        case MODE_RX:
            return RF_RX_RUNNING;
        case RF_CAD:
            return RF_CAD;
        default:
            return RF_IDLE;
    }
}

void RadioSetModem( RadioModems_t modem )
{
    SubgRf.modem=modem ;
    
    switch( modem )
    {
    default:
    case MODEM_FSK:
        SUBGRF_SetPacketType( PACKET_TYPE_GFSK );
        // When switching to GFSK mode the LoRa SyncWord register value is reset
        // Thus, we also reset the RadioPublicNetwork variable
        break;
    case MODEM_LORA:
        SUBGRF_SetPacketType( PACKET_TYPE_LORA );
        // Public/Private network register is reset when switching modems
        RadioSetPublicNetwork( SubgRf.publicNetwork );
        break;
    case MODEM_SIGFOX_TX:
      SUBGRF_SetPacketType( PACKET_TYPE_BPSK );
      break;
    case MODEM_SIGFOX_RX:
      SUBGRF_SetPacketType( PACKET_TYPE_GFSK );
      break;
    }
}

void RadioSetChannel( uint32_t freq )
{
    SUBGRF_SetRfFrequency( freq );
}

bool RadioIsChannelFree( RadioModems_t modem, uint32_t freq, int16_t rssiThresh, uint32_t maxCarrierSenseTime )
{
    bool status = true;
    int16_t rssi = 0;
    uint32_t carrierSenseTime = 0;

    RadioStandby( );

    RadioSetModem( modem );

    RadioSetChannel( freq );

    RadioRx( 0 );

    RADIO_DELAY_MS( RadioGetWakeUpTime( ) );

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
    RadioStandby( );
    return status;
}

uint32_t RadioRandom( void )
{
    uint32_t rnd = SUBGRF_GetRandom();

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
      
    uint8_t modReg;
    SubgRf.RxContinuous = rxContinuous;
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

        case MODEM_SIGFOX_RX:
            SUBGRF_SetStopRxTimerOnPreambleDetect( true );
            SubgRf.ModulationParams.PacketType = PACKET_TYPE_GFSK;

            SubgRf.ModulationParams.Params.Gfsk.BitRate = datarate;
            SubgRf.ModulationParams.Params.Gfsk.ModulationShaping = MOD_SHAPING_G_BT_05;
            SubgRf.ModulationParams.Params.Gfsk.Fdev = 800;
            SubgRf.ModulationParams.Params.Gfsk.Bandwidth = RadioGetFskBandwidthRegValue( bandwidth );

            SubgRf.PacketParams.PacketType = PACKET_TYPE_GFSK;
            SubgRf.PacketParams.Params.Gfsk.PreambleLength = ( preambleLen << 3 ); // convert byte into bit
            SubgRf.PacketParams.Params.Gfsk.PreambleMinDetect = RADIO_PREAMBLE_DETECTOR_OFF;
            SubgRf.PacketParams.Params.Gfsk.SyncWordLength = 2 << 3; // convert byte into bit
            SubgRf.PacketParams.Params.Gfsk.AddrComp = RADIO_ADDRESSCOMP_FILT_OFF;
            SubgRf.PacketParams.Params.Gfsk.HeaderType = RADIO_PACKET_FIXED_LENGTH;
            SubgRf.PacketParams.Params.Gfsk.PayloadLength = MaxPayloadLength;
            SubgRf.PacketParams.Params.Gfsk.CrcLength = RADIO_CRC_OFF;

            SubgRf.PacketParams.Params.Gfsk.DcFree = RADIO_DC_FREE_OFF;

           // RadioStandby( );
            RadioSetModem( MODEM_SIGFOX_RX );
            SUBGRF_SetModulationParams( &SubgRf.ModulationParams );
            SUBGRF_SetPacketParams( &SubgRf.PacketParams );
            SUBGRF_SetSyncWord( ( uint8_t[] ){0xB2, 0x27, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } );
            SUBGRF_SetWhiteningSeed( 0x01FF );

            /*•	NO gfo reset (better sensitivity). Reg 0x8b8, bit4 = 0*/
            modReg= RadioRead(0x8b8);
            modReg&=RADIO_BIT_MASK(4);
//            modReg =modReg & (1<<2);
           RadioWrite(0x8b8, modReg); 
            /*3.	Lower the threshold of cfo_reset */
            RadioWrite(0x8b9, 0x4 ); 
            
            /*•Bigger rssi_len (stability AGC). Reg 0x89b, bits[2 :4] = 0x1 */
            modReg= RadioRead(0x89b);
            modReg&=( RADIO_BIT_MASK(2) & RADIO_BIT_MASK(3) & RADIO_BIT_MASK(4) );
            RadioWrite(0x89b, (modReg| (0x1<<3) ) ); 

            /*•Biger afc_pbl_len (better frequency correction). Reg 0x6d1, bits[3 :4] = 0x3 */
            modReg= RadioRead(0x6d1);
            modReg&=( RADIO_BIT_MASK(3) & RADIO_BIT_MASK(4) );
            RadioWrite(0x6d1, (modReg| (0x3<<3) )); 

            /*••Use of new bit synchronizer (to avoid CRC errors during PER for payloads with a small amount of transitions). Reg 0x6ac, bits[4 :6] = 0x5 */
            modReg= RadioRead(0x6ac);
            modReg&=( RADIO_BIT_MASK(4) & RADIO_BIT_MASK(5) & RADIO_BIT_MASK(6) );
            RadioWrite(0x6ac, (modReg| (0x5<<4) ));

            SubgRf.RxTimeout = ( uint32_t )( symbTimeout * ( ( 1.0 / ( double )datarate ) * 8.0 ) * 1000 );
            break;
        case MODEM_FSK:
            SUBGRF_SetStopRxTimerOnPreambleDetect( false );
            SubgRf.ModulationParams.PacketType = PACKET_TYPE_GFSK;

            SubgRf.ModulationParams.Params.Gfsk.BitRate = datarate;
            SubgRf.ModulationParams.Params.Gfsk.ModulationShaping = MOD_SHAPING_G_BT_1;
            SubgRf.ModulationParams.Params.Gfsk.Bandwidth = RadioGetFskBandwidthRegValue( bandwidth );

            SubgRf.PacketParams.PacketType = PACKET_TYPE_GFSK;
            SubgRf.PacketParams.Params.Gfsk.PreambleLength = ( preambleLen << 3 ); // convert byte into bit
            SubgRf.PacketParams.Params.Gfsk.PreambleMinDetect = RADIO_PREAMBLE_DETECTOR_08_BITS;
            SubgRf.PacketParams.Params.Gfsk.SyncWordLength = 3 << 3; // convert byte into bit
            SubgRf.PacketParams.Params.Gfsk.AddrComp = RADIO_ADDRESSCOMP_FILT_OFF;
            SubgRf.PacketParams.Params.Gfsk.HeaderType = ( fixLen == true ) ? RADIO_PACKET_FIXED_LENGTH : RADIO_PACKET_VARIABLE_LENGTH;
            SubgRf.PacketParams.Params.Gfsk.PayloadLength = MaxPayloadLength;
            if( crcOn == true )
            {
                SubgRf.PacketParams.Params.Gfsk.CrcLength = RADIO_CRC_2_BYTES_CCIT;
            }
            else
            {
                SubgRf.PacketParams.Params.Gfsk.CrcLength = RADIO_CRC_OFF;
            }
            SubgRf.PacketParams.Params.Gfsk.DcFree = RADIO_DC_FREEWHITENING;

            RadioStandby( );
            RadioSetModem( ( SubgRf.ModulationParams.PacketType == PACKET_TYPE_GFSK ) ? MODEM_FSK : MODEM_LORA );
            SUBGRF_SetModulationParams( &SubgRf.ModulationParams );
            SUBGRF_SetPacketParams( &SubgRf.PacketParams );
            SUBGRF_SetSyncWord( ( uint8_t[] ){ 0xC1, 0x94, 0xC1, 0x00, 0x00, 0x00, 0x00, 0x00 } );
            SUBGRF_SetWhiteningSeed( 0x01FF );

            SubgRf.RxTimeout = ( uint32_t )( symbTimeout * ( ( 1.0 / ( double )datarate ) * 8.0 ) * 1000 );
            break;

        case MODEM_LORA:
            SUBGRF_SetStopRxTimerOnPreambleDetect( false );
            SUBGRF_SetLoRaSymbNumTimeout( symbTimeout );
            SubgRf.ModulationParams.PacketType = PACKET_TYPE_LORA;
            SubgRf.ModulationParams.Params.LoRa.SpreadingFactor = ( RadioLoRaSpreadingFactors_t )datarate;
            SubgRf.ModulationParams.Params.LoRa.Bandwidth = Bandwidths[bandwidth];
            SubgRf.ModulationParams.Params.LoRa.CodingRate = ( RadioLoRaCodingRates_t )coderate;

            if( ( ( bandwidth == 0 ) && ( ( datarate == 11 ) || ( datarate == 12 ) ) ) ||
            ( ( bandwidth == 1 ) && ( datarate == 12 ) ) )
            {
                SubgRf.ModulationParams.Params.LoRa.LowDatarateOptimize = 0x01;
            }
            else
            {
                SubgRf.ModulationParams.Params.LoRa.LowDatarateOptimize = 0x00;
            }

            SubgRf.PacketParams.PacketType = PACKET_TYPE_LORA;
            
            if( ( SubgRf.ModulationParams.Params.LoRa.SpreadingFactor == LORA_SF5 ) ||
                ( SubgRf.ModulationParams.Params.LoRa.SpreadingFactor == LORA_SF6 ) )
            {
                if( preambleLen < 12 )
                {
                    SubgRf.PacketParams.Params.LoRa.PreambleLength = 12;
                }
                else
                {
                    SubgRf.PacketParams.Params.LoRa.PreambleLength = preambleLen;
                }
            }
            else
            {
                SubgRf.PacketParams.Params.LoRa.PreambleLength = preambleLen;
            }

            SubgRf.PacketParams.Params.LoRa.HeaderType = ( RadioLoRaPacketLengthsMode_t )fixLen;

            SubgRf.PacketParams.Params.LoRa.PayloadLength = MaxPayloadLength;
            SubgRf.PacketParams.Params.LoRa.CrcMode = ( RadioLoRaCrcModes_t )crcOn;
            SubgRf.PacketParams.Params.LoRa.InvertIQ = ( RadioLoRaIQModes_t )iqInverted;

            RadioSetModem( ( SubgRf.ModulationParams.PacketType == PACKET_TYPE_GFSK ) ? MODEM_FSK : MODEM_LORA );
            SUBGRF_SetModulationParams( &SubgRf.ModulationParams );
            SUBGRF_SetPacketParams( &SubgRf.PacketParams );

            // WORKAROUND - Optimizing the Inverted IQ Operation, see DS_SX1261-2_V1.2 datasheet chapter 15.4
            if( SubgRf.PacketParams.Params.LoRa.InvertIQ == LORA_IQ_INVERTED )
            {
                // RegIqPolaritySetup = @address 0x0736
                SUBGRF_WriteRegister( 0x0736, SUBGRF_ReadRegister( 0x0736 ) & ~( 1 << 2 ) );
            }
            else
            {
                // RegIqPolaritySetup @address 0x0736
                SUBGRF_WriteRegister( 0x0736, SUBGRF_ReadRegister( 0x0736 ) | ( 1 << 2 ) );
            }
            // WORKAROUND END

            // Timeout Max, Timeout handled directly in SetRx function
            SubgRf.RxTimeout = 0xFFFF;

            break;
    default:
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
        case MODEM_SIGFOX_TX:
          
            RadioSetModem(MODEM_SIGFOX_TX);
            SubgRf.ModulationParams.PacketType = PACKET_TYPE_BPSK;
            SubgRf.ModulationParams.Params.Bpsk.BitRate           = datarate;
            SubgRf.ModulationParams.Params.Bpsk.ModulationShaping = MOD_SHAPING_DBPSK;
            SUBGRF_SetModulationParams( &SubgRf.ModulationParams );

            //SubgRf.PacketParams.Params.Bpsk.PayloadLength         = frameSize + 1; // for added start and stop bit
           // SubgRf.PacketParams.Params.Bpsk.
            //SUBGRF_SetPacketParams( &SubgRf.PacketParams );
          break;
        case MODEM_FSK:
            SubgRf.ModulationParams.PacketType = PACKET_TYPE_GFSK;
            SubgRf.ModulationParams.Params.Gfsk.BitRate = datarate;

            SubgRf.ModulationParams.Params.Gfsk.ModulationShaping = MOD_SHAPING_G_BT_1;
            SubgRf.ModulationParams.Params.Gfsk.Bandwidth = RadioGetFskBandwidthRegValue( bandwidth );
            SubgRf.ModulationParams.Params.Gfsk.Fdev = fdev;

            SubgRf.PacketParams.PacketType = PACKET_TYPE_GFSK;
            SubgRf.PacketParams.Params.Gfsk.PreambleLength = ( preambleLen << 3 ); // convert byte into bit
            SubgRf.PacketParams.Params.Gfsk.PreambleMinDetect = RADIO_PREAMBLE_DETECTOR_08_BITS;
            SubgRf.PacketParams.Params.Gfsk.SyncWordLength = 3 << 3 ; // convert byte into bit
            SubgRf.PacketParams.Params.Gfsk.AddrComp = RADIO_ADDRESSCOMP_FILT_OFF;
            SubgRf.PacketParams.Params.Gfsk.HeaderType = ( fixLen == true ) ? RADIO_PACKET_FIXED_LENGTH : RADIO_PACKET_VARIABLE_LENGTH;

            if( crcOn == true )
            {
                SubgRf.PacketParams.Params.Gfsk.CrcLength = RADIO_CRC_2_BYTES_CCIT;
            }
            else
            {
                SubgRf.PacketParams.Params.Gfsk.CrcLength = RADIO_CRC_OFF;
            }
            SubgRf.PacketParams.Params.Gfsk.DcFree = RADIO_DC_FREEWHITENING;

            RadioStandby( );
            RadioSetModem( ( SubgRf.ModulationParams.PacketType == PACKET_TYPE_GFSK ) ? MODEM_FSK : MODEM_LORA );
            SUBGRF_SetModulationParams( &SubgRf.ModulationParams );
            SUBGRF_SetPacketParams( &SubgRf.PacketParams );
            SUBGRF_SetSyncWord( ( uint8_t[] ){ 0xC1, 0x94, 0xC1, 0x00, 0x00, 0x00, 0x00, 0x00 } );
            SUBGRF_SetWhiteningSeed( 0x01FF );
            break;

        case MODEM_LORA:
            SubgRf.ModulationParams.PacketType = PACKET_TYPE_LORA;
            SubgRf.ModulationParams.Params.LoRa.SpreadingFactor = ( RadioLoRaSpreadingFactors_t ) datarate;
            SubgRf.ModulationParams.Params.LoRa.Bandwidth =  Bandwidths[bandwidth];
            SubgRf.ModulationParams.Params.LoRa.CodingRate= ( RadioLoRaCodingRates_t )coderate;

            if( ( ( bandwidth == 0 ) && ( ( datarate == 11 ) || ( datarate == 12 ) ) ) ||
            ( ( bandwidth == 1 ) && ( datarate == 12 ) ) )
            {
                SubgRf.ModulationParams.Params.LoRa.LowDatarateOptimize = 0x01;
            }
            else
            {
                SubgRf.ModulationParams.Params.LoRa.LowDatarateOptimize = 0x00;
            }

            SubgRf.PacketParams.PacketType = PACKET_TYPE_LORA;

            if( ( SubgRf.ModulationParams.Params.LoRa.SpreadingFactor == LORA_SF5 ) ||
                ( SubgRf.ModulationParams.Params.LoRa.SpreadingFactor == LORA_SF6 ) )
            {
                if( preambleLen < 12 )
                {
                    SubgRf.PacketParams.Params.LoRa.PreambleLength = 12;
                }
                else
                {
                    SubgRf.PacketParams.Params.LoRa.PreambleLength = preambleLen;
                }
            }
            else
            {
                SubgRf.PacketParams.Params.LoRa.PreambleLength = preambleLen;
            }

            SubgRf.PacketParams.Params.LoRa.HeaderType = ( RadioLoRaPacketLengthsMode_t )fixLen;
            SubgRf.PacketParams.Params.LoRa.PayloadLength = MaxPayloadLength;
            SubgRf.PacketParams.Params.LoRa.CrcMode = ( RadioLoRaCrcModes_t )crcOn;
            SubgRf.PacketParams.Params.LoRa.InvertIQ = ( RadioLoRaIQModes_t )iqInverted;

            RadioStandby( );
            RadioSetModem( ( SubgRf.ModulationParams.PacketType == PACKET_TYPE_GFSK ) ? MODEM_FSK : MODEM_LORA );
            SUBGRF_SetModulationParams( &SubgRf.ModulationParams );
            SUBGRF_SetPacketParams( &SubgRf.PacketParams );
            break;
		default: break;
    }

    // WORKAROUND - Modulation Quality with 500 kHz LoRa® Bandwidth, see DS_SX1261-2_V1.2 datasheet chapter 15.1
    if( ( modem == MODEM_LORA ) && ( SubgRf.ModulationParams.Params.LoRa.Bandwidth == LORA_BW_500 ) )
    {
        // RegTxModulation = @address 0x0889
        SUBGRF_WriteRegister( 0x0889, SUBGRF_ReadRegister( 0x0889 ) & ~( 1 << 2 ) );
    }
    else
    {
        // RegTxModulation = @address 0x0889
        SUBGRF_WriteRegister( 0x0889, SUBGRF_ReadRegister( 0x0889 ) | ( 1 << 2 ) );
    }
    // WORKAROUND END

    AntSwitchPower = SUBGRF_SetTxPower( power, RADIO_RAMP_40_US );
    SubgRf.TxTimeout = timeout;
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
           airTime = (uint32_t)rint( ( 8 * ( SubgRf.PacketParams.Params.Gfsk.PreambleLength +
                                     ( SubgRf.PacketParams.Params.Gfsk.SyncWordLength >> 3 ) +
                                     ( ( SubgRf.PacketParams.Params.Gfsk.HeaderType == RADIO_PACKET_FIXED_LENGTH ) ? 0.0 : 1.0 ) +
                                     pktLen +
                                     ( ( SubgRf.PacketParams.Params.Gfsk.CrcLength == RADIO_CRC_2_BYTES ) ? 2.0 : 0 ) ) /
                                     SubgRf.ModulationParams.Params.Gfsk.BitRate ) * 1e3 );
        }
        break;
    case MODEM_LORA:
        {
            double ts = RadioLoRaSymbTime[SubgRf.ModulationParams.Params.LoRa.Bandwidth - 4][12 - SubgRf.ModulationParams.Params.LoRa.SpreadingFactor];
            // time of preamble
            double tPreamble = ( SubgRf.PacketParams.Params.LoRa.PreambleLength + 4.25 ) * ts;
            // Symbol length of payload and time
            double tmp = ceil( ( 8 * pktLen - 4 * SubgRf.ModulationParams.Params.LoRa.SpreadingFactor +
                                 28 + 16 * SubgRf.PacketParams.Params.LoRa.CrcMode -
                                 ( ( SubgRf.PacketParams.Params.LoRa.HeaderType == LORA_PACKET_FIXED_LENGTH ) ? 20 : 0 ) ) /
                                 ( double )( 4 * ( SubgRf.ModulationParams.Params.LoRa.SpreadingFactor -
                                 ( ( SubgRf.ModulationParams.Params.LoRa.LowDatarateOptimize > 0 ) ? 2 : 0 ) ) ) ) *
                                 ( ( SubgRf.ModulationParams.Params.LoRa.CodingRate % 4 ) + 4 );
            double nPayload = 8 + ( ( tmp > 0 ) ? tmp : 0 );
            double tPayload = nPayload * ts;
            // Time on air
            double tOnAir = tPreamble + tPayload;
            // return milli seconds
            airTime = (uint32_t) floor( tOnAir + 0.999 );
        }
        break;
	default :break;
    }
	
    return airTime;
}


void RadioSend( uint8_t *buffer, uint8_t size )
{
    /* Radio IRQ is set to DIO1 by default */
    SUBGRF_SetDioIrqParams( IRQ_TX_DONE | IRQ_RX_TX_TIMEOUT,
                            IRQ_TX_DONE | IRQ_RX_TX_TIMEOUT,
                            IRQ_RADIO_NONE,
                            IRQ_RADIO_NONE );
    
//Set DBG pin PB13
#ifdef LET_SUBGHZ_MW_USING_DGB_LINE2_PIN
    DBG_GPIO_SET(DGB_LINE2_PORT, DGB_LINE2_PIN);
#endif

/* Set RF switch */
 SUBGRF_SetSwitch(AntSwitchPower, RFSWITCH_TX);

  switch(SubgRf.modem)
  {
    case MODEM_LORA:
    {
      SubgRf.PacketParams.Params.LoRa.PayloadLength = size;
      SUBGRF_SetPacketParams( &SubgRf.PacketParams );
      SUBGRF_SendPayload( buffer, size, 0 );
      break;
    }
  case MODEM_FSK:
    {
      SubgRf.PacketParams.Params.Gfsk.PayloadLength = size;
      SUBGRF_SetPacketParams( &SubgRf.PacketParams );
      SUBGRF_SendPayload( buffer, size, 0 );
      break;
    }
    
  case MODEM_BPSK:
      SubgRf.PacketParams.PacketType=PACKET_TYPE_BPSK;
      SubgRf.PacketParams.Params.Bpsk.PayloadLength = size;
      SUBGRF_SetPacketParams( &SubgRf.PacketParams );

      SUBGRF_SendPayload( buffer, size, 0 );
    break;
  case MODEM_SIGFOX_TX:
    {
      uint8_t outBuffer[35] = {0};
      /*from bpsk to dbpsk*/
      /*first 1 bit duplicated*/
      payload_integration( outBuffer, buffer, size );
            
      SubgRf.PacketParams.PacketType=PACKET_TYPE_BPSK;
      SubgRf.PacketParams.Params.Bpsk.PayloadLength = size+1;
      SUBGRF_SetPacketParams( &SubgRf.PacketParams );
      
      
      if( SubgRf.ModulationParams.Params.Bpsk.BitRate == 100 )
      {
        RadioWrite( 0x00F1, 0x0F ); // clean start-up LSB
        RadioWrite( 0x00F0, 0x37 ); // clean start-up MSB
        RadioWrite( 0x00F3, 0x70 ); // clean end of frame LSB
        RadioWrite( 0x00F2, 0x1D ); // clean end of frame MSB
      }
      else // 600 bps
      {
        RadioWrite( 0x00F1, 0x2F ); // clean start-up LSB
        RadioWrite( 0x00F0, 0x09 ); // clean start-up MSB
        RadioWrite( 0x00F3, 0xE1 ); // clean end of frame LSB 
        RadioWrite( 0x00F2, 0x04 ); // clean end of frame MSB
      }
      
      uint16_t bitNum = (size*8)+2;
      RadioWrite( 0x00F4, ( bitNum >> 8 ) & 0x00FF );    // limit frame
      RadioWrite( 0x00F5, bitNum & 0x00FF );             // limit frame
      //
      SUBGRF_SendPayload( outBuffer, size+1 , 0xFFFFFF );
      break;
    }
  default:
    break;
  }

    
    TimerSetValue( &TxTimeoutTimer, SubgRf.TxTimeout );
    TimerStart( &TxTimeoutTimer );
}

void RadioSleep( void )
{
    SleepParams_t params = { 0 };

    params.Fields.WarmStart = 1;
    SUBGRF_SetSleep( params );

    RADIO_DELAY_MS( 2 );
}

void RadioStandby( void )
{
    SUBGRF_SetStandby( STDBY_RC );
}

void RadioRx( uint32_t timeout )
{
    
    /* Radio IRQ is set to DIO1 by default */  
    SUBGRF_SetDioIrqParams( IRQ_RADIO_ALL, //IRQ_RX_DONE | IRQ_RX_TX_TIMEOUT,
                            IRQ_RADIO_ALL, //IRQ_RX_DONE | IRQ_RX_TX_TIMEOUT,
                            IRQ_RADIO_NONE,
                            IRQ_RADIO_NONE );

    if( timeout != 0 )
    {
        TimerSetValue( &RxTimeoutTimer, timeout );
        TimerStart( &RxTimeoutTimer );
    }

    // Set DBG pin PB12
#ifdef LET_SUBGHZ_MW_USING_DGB_LINE1_PIN
    DBG_GPIO_SET(DGB_LINE1_PORT, DGB_LINE1_PIN);
#endif    
    /* RF switch configuration */
    SUBGRF_SetSwitch(AntSwitchPower, RFSWITCH_RX );
    if( SubgRf.RxContinuous == true )
    {
        SUBGRF_SetRx( 0xFFFFFF ); // Rx Continuous
    }
    else
    { 
        SUBGRF_SetRx( SubgRf.RxTimeout << 6 );
    }
}

void RadioRxBoosted( uint32_t timeout )
{
    SUBGRF_SetDioIrqParams( IRQ_RADIO_ALL, //IRQ_RX_DONE | IRQ_RX_TX_TIMEOUT,
                            IRQ_RADIO_ALL, //IRQ_RX_DONE | IRQ_RX_TX_TIMEOUT,
                            IRQ_RADIO_NONE,
                            IRQ_RADIO_NONE );

  if( timeout != 0 )
  {
       TimerSetValue( &RxTimeoutTimer, timeout );
       TimerStart( &RxTimeoutTimer );
  }
  
  /* RF switch configuration */
  SUBGRF_SetSwitch(AntSwitchPower, RFSWITCH_RX);
    if( SubgRf.RxContinuous == true )
    {
        SUBGRF_SetRxBoosted( 0xFFFFFF ); // Rx Continuous
    }
    else
    {
        SUBGRF_SetRxBoosted( SubgRf.RxTimeout << 6 );
    }
}

void RadioSetRxDutyCycle( uint32_t rxTime, uint32_t sleepTime )
{
  /* RF switch configuration */
  SUBGRF_SetSwitch(AntSwitchPower, RFSWITCH_RX);
  SUBGRF_SetRxDutyCycle( rxTime, sleepTime );
}

void RadioStartCad( void )
{
    SUBGRF_SetCad( );
}

void RadioTx( uint32_t timeout )
{
    SUBGRF_SetTx( timeout << 6 );
}

void RadioSetTxContinuousWave( uint32_t freq, int8_t power, uint16_t time )
{
    uint8_t antswitchpow;

    SUBGRF_SetRfFrequency( freq );
  
    antswitchpow = SUBGRF_SetTxPower( power, RADIO_RAMP_40_US );

    /* Set RF switch */
    SUBGRF_SetSwitch(antswitchpow, RFSWITCH_TX);

    SUBGRF_SetTxContinuousWave( );

    TimerSetValue( &TxTimeoutTimer, (uint32_t)(time  * 1e3 ));
    TimerStart( &TxTimeoutTimer );
}

int16_t RadioRssi( RadioModems_t modem )
{
    return SUBGRF_GetRssiInst( );
}

void RadioWrite( uint16_t addr, uint8_t data )
{
    SUBGRF_WriteRegister(addr, data );
}

uint8_t RadioRead( uint16_t addr )
{
    return SUBGRF_ReadRegister(addr);
}

void RadioWriteBuffer( uint16_t addr, uint8_t *buffer, uint8_t size )
{
    SUBGRF_WriteRegisters( addr, buffer, size );
}

void RadioReadBuffer( uint16_t addr, uint8_t *buffer, uint8_t size )
{
    SUBGRF_ReadRegisters( addr, buffer, size );
}

//void RadioWriteFifo( uint8_t *buffer, uint8_t size )
//{
//    SUBGRF_WriteBuffer( 0, buffer, size );
//}
//
//void RadioReadFifo( uint8_t *buffer, uint8_t size )
//{
//    SUBGRF_ReadBuffer( 0, buffer, size );
//}

void RadioSetMaxPayloadLength( RadioModems_t modem, uint8_t max )
{
    if( modem == MODEM_LORA )
    {
        SubgRf.PacketParams.Params.LoRa.PayloadLength = MaxPayloadLength = max;
        SUBGRF_SetPacketParams( &SubgRf.PacketParams );
    }
    else
    {
        if( SubgRf.PacketParams.Params.Gfsk.HeaderType == RADIO_PACKET_VARIABLE_LENGTH )
        {
            SubgRf.PacketParams.Params.Gfsk.PayloadLength = MaxPayloadLength = max;
            SUBGRF_SetPacketParams( &SubgRf.PacketParams );
        }
    }
}

void RadioSetPublicNetwork( bool enable )
{
    uint8_t reg;

    SubgRf.publicNetwork= enable;

    if( enable == true )
    {
        // Change LoRa modem SyncWord
      reg = ( LORA_MAC_PUBLIC_SYNCWORD >> 8 ) & 0xFF;
      SUBGRF_WriteRegisters( REG_LR_SYNCWORD, (uint8_t*)&reg, 1 );
      reg = LORA_MAC_PUBLIC_SYNCWORD & 0xFF;
      SUBGRF_WriteRegisters( REG_LR_SYNCWORD + 1, (uint8_t*)&reg, 1 );
    }
    else
    {
        // Change LoRa modem SyncWord
      reg = ( LORA_MAC_PRIVATE_SYNCWORD >> 8 ) & 0xFF;
      SUBGRF_WriteRegisters( REG_LR_SYNCWORD, (uint8_t*)&reg, 1 );
      reg = LORA_MAC_PRIVATE_SYNCWORD & 0xFF;
      SUBGRF_WriteRegisters( REG_LR_SYNCWORD + 1, (uint8_t*)&reg, 1 );
    }
}

uint32_t RadioGetWakeUpTime( void )
{
  return (SUBGRF_GetRadioWakeUpTime() + RADIO_WAKEUP_TIME);
}


void RadioOnTxTimeoutIrq( void* context )
{
  // Reset DBG pin PB13
#ifdef LET_SUBGHZ_MW_USING_DGB_LINE2_PIN
  DBG_GPIO_RST(DGB_LINE2_PORT, DGB_LINE2_PIN);
#endif  
    if( ( RadioEvents != NULL ) && ( RadioEvents->TxTimeout != NULL ) )
    {
        RadioEvents->TxTimeout( );
    }
}

void RadioOnRxTimeoutIrq( void* context )
{
  // Reset DBG pin PB12
#ifdef LET_SUBGHZ_MW_USING_DGB_LINE1_PIN
  DBG_GPIO_RST(DGB_LINE1_PORT, DGB_LINE1_PIN);
#endif  
    if( ( RadioEvents != NULL ) && ( RadioEvents->RxTimeout != NULL ) )
    {
        RadioEvents->RxTimeout( );
    }
}

void RadioOnDioIrq( RadioIrqMasks_t radioIrq )
{
  SubgRf.radioIrq=radioIrq;  
  
  RadioIrqProcess();
  
}


void RadioIrqProcess(void )
{
  PacketStatus_t RadioPktStatus;
  uint8_t size;
    
  switch (SubgRf.radioIrq)
  {
  case IRQ_TX_DONE:
    // Reset DBG pin PB13
#ifdef LET_SUBGHZ_MW_USING_DGB_LINE2_PIN
    DBG_GPIO_RST(DGB_LINE2_PORT, DGB_LINE2_PIN);
#endif          
    TimerStop( &TxTimeoutTimer );
    SUBGRF_SetStandby( STDBY_RC );
    if( ( RadioEvents != NULL ) && ( RadioEvents->TxDone != NULL ) )
    {
      RadioEvents->TxDone( );
    }    
    break;
    
  case IRQ_RX_DONE: 
    // Reset DBG pin PB12
#ifdef LET_SUBGHZ_MW_USING_DGB_LINE1_PIN
    DBG_GPIO_RST(DGB_LINE1_PORT, DGB_LINE1_PIN);
#endif            
    TimerStop( &RxTimeoutTimer );
    if( SubgRf.RxContinuous == false )
    {
      //!< Update operating mode state to a value lower than \ref MODE_STDBY_XOSC
      SUBGRF_SetStandby( STDBY_RC );
      
      // WORKAROUND - Implicit Header Mode Timeout Behavior, see DS_SX1261-2_V1.2 datasheet chapter 15.3
      // RegRtcControl = @address 0x0902
      SUBGRF_WriteRegister( 0x0902, 0x00 );
      // RegEventMask = @address 0x0944
      SUBGRF_WriteRegister( 0x0944, SUBGRF_ReadRegister( 0x0944 ) | ( 1 << 1 ) );
      // WORKAROUND END
    }
    SUBGRF_GetPayload( RadioRxPayload, &size , 255 );
    SUBGRF_GetPacketStatus( &RadioPktStatus );
    if( ( RadioEvents != NULL ) && ( RadioEvents->RxDone != NULL ) )
    {
      switch (RadioPktStatus.packetType)
      {
      case PACKET_TYPE_LORA:
        RadioEvents->RxDone( RadioRxPayload, size, RadioPktStatus.Params.LoRa.RssiPkt, RadioPktStatus.Params.LoRa.SnrPkt );
        break;
      default:
        RadioEvents->RxDone( RadioRxPayload, size, RadioPktStatus.Params.Gfsk.RssiAvg, (int8_t)(RadioPktStatus.Params.Gfsk.FreqError) );
        break;
      } 
    }
    break;
    
  case IRQ_CRC_ERROR:
    
    if( SubgRf.RxContinuous == false )
    {
      //!< Update operating mode state to a value lower than \ref MODE_STDBY_XOSC
      SUBGRF_SetStandby( STDBY_RC );
    }
    if( ( RadioEvents != NULL ) && ( RadioEvents->RxError ) )
    {
      RadioEvents->RxError( );
    }
    break;  
    
    
  case IRQ_CAD_DONE:
    //!< Update operating mode state to a value lower than \ref MODE_STDBY_XOSC
    SUBGRF_SetStandby( STDBY_RC );
    if( ( RadioEvents != NULL ) && ( RadioEvents->CadDone != NULL ) )
    {
      RadioEvents->CadDone( ( ( SubgRf.radioIrq & IRQ_CAD_ACTIVITY_DETECTED ) == IRQ_CAD_ACTIVITY_DETECTED ) );
    }  
    break; 
    
  case IRQ_RX_TX_TIMEOUT:
    if( SUBGRF_GetOperatingMode( ) == MODE_TX )
    {
      // Reset DBG pin PB13
#ifdef LET_SUBGHZ_MW_USING_DGB_LINE2_PIN
      DBG_GPIO_RST(DGB_LINE2_PORT, DGB_LINE2_PIN);
#endif  
      TimerStop( &TxTimeoutTimer );
      //!< Update operating mode state to a value lower than \ref MODE_STDBY_XOSC
      SUBGRF_SetStandby( STDBY_RC );
      if( ( RadioEvents != NULL ) && ( RadioEvents->TxTimeout != NULL ) )
      {
        RadioEvents->TxTimeout( );
      }
    }
    else if( SUBGRF_GetOperatingMode( ) == MODE_RX )
    {
      // Reset DBG pin PB12
#ifdef LET_SUBGHZ_MW_USING_DGB_LINE1_PIN
      DBG_GPIO_RST(DGB_LINE1_PORT, DGB_LINE1_PIN);
#endif            
      TimerStop( &RxTimeoutTimer );
      //!< Update operating mode state to a value lower than \ref MODE_STDBY_XOSC
      SUBGRF_SetStandby( STDBY_RC );
      if( ( RadioEvents != NULL ) && ( RadioEvents->RxTimeout != NULL ) )
      {
        RadioEvents->RxTimeout( );
      }
    }
    break;
    
  case IRQ_PREAMBLE_DETECTED:
    MW_LOG(  "PRE OK\n\r" );
    break;
    
  case IRQ_SYNCWORD_VALID:
    
    MW_LOG(  "SYNC OK\n\r" );
    break;
	
  case IRQ_HEADER_VALID:
    
    MW_LOG(  "HDR OK\n\r" );
    break;
    
  case IRQ_HEADER_ERROR:
    TimerStop( &RxTimeoutTimer );
    if( SubgRf.RxContinuous == false )
    {
      //!< Update operating mode state to a value lower than \ref MODE_STDBY_XOSC
      SUBGRF_SetStandby( STDBY_RC );
    }
    if( ( RadioEvents != NULL ) && ( RadioEvents->RxTimeout != NULL ) )
    {
      RadioEvents->RxTimeout( );
      MW_LOG(  "HDR KO\n\r" );
    }
    break;
    
  default:
    break;
    
  }
}

void RadioTxPrbs( void )
{
    SUBGRF_SetSwitch(AntSwitchPower, RFSWITCH_TX);
    Radio.Write(0x6B8, 0x2d);  // sel mode prbs9 instead of preamble
    SUBGRF_SetTxInfinitePreamble();
    SUBGRF_SetTx(0x0fffff);
}

void RadioTxCw( void )
{
    SUBGRF_SetSwitch(AntSwitchPower, RFSWITCH_TX);
    
    SUBGRF_SetTxContinuousWave();
}

/*!
* @brief D-BPSK to BPSK
*
* @param [in]      inBuffer         buffer with frame to encode
* @param [out]     outBuffer        buffer with frame encoded
* @param [in]      size            size of the payload to encode
*/
static void payload_integration( uint8_t *outBuffer, uint8_t *inBuffer, uint8_t size)
{
  uint8_t prevInt=0;
  uint8_t currBit;
  uint8_t index_bit;
  uint8_t index_byte;
  uint8_t index_bit_out;
  uint8_t index_byte_out;
  int i=0;
  
  for (i=0; i<size; i++)
  {
    /*reverse all inputs*/
    inBuffer[i]=~inBuffer[i];
    /*init outBuffer*/
    outBuffer[i]=0;
  }
  
  for (i=0; i<size*8; i++)
  {
    /*index to take bit in inBuffer*/
    index_bit = 7 - (i%8);
    index_byte = i / 8;
    /*index to place bit in outBuffer is shifted 1 bit rigth*/
    index_bit_out = 7 - ((i+1)%8);
    index_byte_out = (i+1) / 8;
    /*extract current bit from input*/
    currBit = (inBuffer[index_byte] >> index_bit) & 0x01; 
    /*integration*/
    prevInt ^= currBit;
    /* write result integration in output*/
    outBuffer[index_byte_out]|= (prevInt << index_bit_out);
  }
  
  outBuffer[size] =(prevInt<<7) | (prevInt<<6) | (( (!prevInt) & 0x01)<<5) ;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
