/*!
 * \file      LoRaMacTypes.h
 *
 * \brief     LoRa MAC layer internal types definition. Please do not include in application sources.
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
 *              (C)2013 Semtech
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
 * \author    Daniel Jaeckle ( STACKFORCE )
 *
 * \author    Johannes Bruder ( STACKFORCE )
 *
 * addtogroup LORAMAC
 * \{
 *
 */
#ifndef __LORAMAC_TYPES_H__
#define __LORAMAC_TYPES_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>
#include "timer.h"

/*!
 * Start value for unicast keys enumeration
 */
#define LORAMAC_CRYPTO_UNICAST_KEYS     0

/*!
 * Start value for multicast keys enumeration
 */
#define LORAMAC_CRYPTO_MULTICAST_KEYS   127

/*!
 * LoRaWAN devices classes definition
 *
 * LoRaWAN Specification V1.0.2, chapter 2.1
 */
typedef enum DeviceClass_e
{
    /*!
     * LoRaWAN device class A
     *
     * LoRaWAN Specification V1.0.2, chapter 3
     */
    CLASS_A = 0x00,
    /*!
     * LoRaWAN device class B
     *
     * LoRaWAN Specification V1.0.2, chapter 8
     */
    CLASS_B = 0x01,
    /*!
     * LoRaWAN device class C
     *
     * LoRaWAN Specification V1.0.2, chapter 17
     */
    CLASS_C = 0x02,
}DeviceClass_t;

/*!
 * LoRaWAN Frame type enumeration to differ between the possible data up/down frame configurations.
 *
 * Note: The naming is implementation specific since there is no definition
 *       in the LoRaWAN specification included.
 */
typedef enum eFType
{
    /*!
     * Frame type A
     *
     * FOptsLen > 0, Fopt present, FPort > 0, FRMPayload present
     */
    FRAME_TYPE_A,
    /*!
     * Frame type B
     *
     * FOptsLen > 0, Fopt present, FPort not present, FRMPayload not present
     */
    FRAME_TYPE_B,
    /*!
     * Frame type C
     *
     * FOptsLen = 0, Fopt not present, FPort = 0 , FRMPayload containing MAC commands
     */
    FRAME_TYPE_C,
    /*!
     * Frame type D
     *
     * FOptsLen = 0, Fopt not present, FPort > 0 , FRMPayload present
     */
    FRAME_TYPE_D,
}FType_t;

/*!
 * LoRaWAN Frame counter identifier.
 */
typedef enum eFCntIdentifier
{
    /*!
     * Uplink frame counter which is incremented with each uplink.
     */
    FCNT_UP = 0,
    /*!
     * Network downlink frame counter which is incremented with each downlink on FPort 0
     * or when the FPort field is missing.
     */
    N_FCNT_DOWN,
    /*!
     * Application downlink frame counter which is incremented with each downlink
     * on a port different than 0.
     */
    A_FCNT_DOWN,
    /*!
     * In case if the device is connected to a LoRaWAN 1.0 Server,
     * this counter is used for every kind of downlink frame.
     */
    FCNT_DOWN,
    /*!
     * Multicast downlink counter for index 0
     */
    MC_FCNT_DOWN_0,
    /*!
     * Multicast downlink counter for index 1
     */
    MC_FCNT_DOWN_1,
    /*!
     * Multicast downlink counter for index 2
     */
    MC_FCNT_DOWN_2,
    /*!
     * Multicast downlink counter for index 3
     */
    MC_FCNT_DOWN_3,
}FCntIdentifier_t;

/*!
 * LoRaMac Key identifier
 */
typedef enum eKeyIdentifier
{
    /*!
     * Application root key
     */
    APP_KEY = 0,
    /*!
     * Network root key
     */
    NWK_KEY,
    /*!
     * Join session integrity key
     */
    J_S_INT_KEY,
    /*!
     * Join session encryption key
     */
    J_S_ENC_KEY,
    /*!
     * Forwarding Network session integrity key
     */
    F_NWK_S_INT_KEY,
    /*!
     * Serving Network session integrity key
     */
    S_NWK_S_INT_KEY,
    /*!
     * Network session encryption key
     */
    NWK_S_ENC_KEY,
    /*!
     * Application session key
     */
    APP_S_KEY,
    /*!
     * Multicast root key
     */
    MC_ROOT_KEY,
    /*!
     * Multicast key encryption key
     */
    MC_KE_KEY = LORAMAC_CRYPTO_MULTICAST_KEYS,
    /*!
     * Multicast root key index 0
     */
    MC_KEY_0,
    /*!
     * Multicast Application session key index 0
     */
    MC_APP_S_KEY_0,
    /*!
     * Multicast Network session key index 0
     */
    MC_NWK_S_KEY_0,
    /*!
     * Multicast root key index 1
     */
    MC_KEY_1,
    /*!
     * Multicast Application session key index 1
     */
    MC_APP_S_KEY_1,
    /*!
     * Multicast Network session key index 1
     */
    MC_NWK_S_KEY_1,
    /*!
     * Multicast root key index 2
     */
    MC_KEY_2,
    /*!
     * Multicast Application session key index 2
     */
    MC_APP_S_KEY_2,
    /*!
     * Multicast Network session key index 2
     */
    MC_NWK_S_KEY_2,
    /*!
     * Multicast root key index 3
     */
    MC_KEY_3,
    /*!
     * Multicast Application session key index 3
     */
    MC_APP_S_KEY_3,
    /*!
     * Multicast Network session key index 3
     */
    MC_NWK_S_KEY_3,
    /*!
     * Zero key for slot randomization in class B
     */
    SLOT_RAND_ZERO_KEY,
    /*!
     * No Key
     */
    NO_KEY,
}KeyIdentifier_t;

/*!
 * LoRaMac Crypto address identifier
 */
typedef enum eAddressIdentifier
{
    /*!
     * Multicast Address for index 0
     */
    MULTICAST_0_ADDR = 0,
    /*!
     * Multicast Address for index 1
     */
    MULTICAST_1_ADDR = 1,
    /*!
     * Multicast Address for index 2
     */
    MULTICAST_2_ADDR = 2,
    /*!
     * Multicast Address for index 3
     */
    MULTICAST_3_ADDR = 3,
    /*!
     * Unicast End-device address
     */
    UNICAST_DEV_ADDR = 4,
}AddressIdentifier_t;

/*
 * Multicast Rx window parameters
 */
typedef union uMcRxParams
{
    struct
    {
        /*!
            * Reception frequency of the ping slot windows
            */
        uint32_t Frequency;
        /*!
            * Datarate of the ping slot
            */
        int8_t Datarate;
        /*!
            * This parameter is necessary for class B operation. It defines the
            * periodicity of the multicast downlink slots
            */
        uint16_t Periodicity;
    }ClassB;
    struct
    {
        /*!
        * Reception frequency of the ping slot windows
        */
        uint32_t Frequency;
        /*!
        * Datarate of the ping slot
        */
        int8_t Datarate;
    }ClassC;
}McRxParams_t;

/*!
 * Multicast channel
 */
typedef struct sMcChannelParams
{
    /*!
     * Indicate if the multicast channel is being setup remotely or locally.
     * Indicates which set of keys are to be used. \ref uMcKeys
     */
    bool IsRemotelySetup;
    /*!
     * Multicats channel LoRaWAN class B or C
     */
    DeviceClass_t Class;
    /*!
     * True if the entry is active
     */
    bool IsEnabled;
    /*
     * Address identifier
     */
    AddressIdentifier_t GroupID;
    /*!
     * Address
     */
    uint32_t Address;
    /*!
     * Multicast keys
     */
    union uMcKeys
    {
        /*!
         * Encrypted multicast key - Used when IsRemotelySetup equals `true`.
         * MC_KEY is decrypted and then the session keys ar derived.
         */
        uint8_t *McKeyE;
        /*!
         * Multicast Session keys - Used when IsRemotelySetup equals `false`
         */
        struct
        {
            /*!
             * Multicast application session key
             */
            uint8_t *McAppSKey;
            /*!
             * Multicast network session key
             */
            uint8_t *McNwkSKey;
        }Session;
    }McKeys;
    /*!
     * Minimum multicast frame counter value
     */
    uint32_t FCountMin;
    /*!
     * Maximum multicast frame counter value
     */
    uint32_t FCountMax;
    /*!
     * Multicast reception parameters
     */
    McRxParams_t RxParams;
}McChannelParams_t;

/*!
 * Multicast context
 */
typedef struct sMulticastCtx
{
    /*!
     * Multicast channel parameters
     */
    McChannelParams_t ChannelParams;
    /*!
     * Downlink counter
     */
    uint32_t* DownLinkCounter;
    /*
     * Following parameters are only used for ClassB multicast channels
     */
    /*!
     * Number of multicast slots. The variable can be
     * calculated as follows:
     * PingNb = 128 / ( 1 << periodicity ), where
     * 0 <= periodicity <= 7
     */
    uint8_t PingNb;
    /*!
     * Period of the multicast slots. The variable can be
     * calculated as follows:
     * PingPeriod = 4096 / PingNb
     */
    uint16_t PingPeriod;
    /*!
     * Ping offset of the multicast channel for Class B
     */
    uint16_t PingOffset;
}MulticastCtx_t;

/*!
 * LoRaMac join-request / rejoin type identifier
 */
typedef enum eJoinReqIdentifier
{
    /*!
     * Rejoin type 0
     */
    REJOIN_REQ_0 = 0x00,
    /*!
     * Rejoin type 1
     */
    REJOIN_REQ_1 = 0x01,
    /*!
     * Rejoin type 2
     */
    REJOIN_REQ_2 = 0x02,
    /*!
     * Join-request
     */
    JOIN_REQ = 0xFF,
}JoinReqIdentifier_t;

/*!
 * LoRaMAC mote MAC commands
 *
 * LoRaWAN Specification V1.1.0, chapter 5, table 4
 */
typedef enum eLoRaMacMoteCmd
{
    /*!
     * LinkCheckReq
     */
    MOTE_MAC_LINK_CHECK_REQ          = 0x02,
    /*!
     * LinkADRAns
     */
    MOTE_MAC_LINK_ADR_ANS            = 0x03,
    /*!
     * DutyCycleAns
     */
    MOTE_MAC_DUTY_CYCLE_ANS          = 0x04,
    /*!
     * RXParamSetupAns
     */
    MOTE_MAC_RX_PARAM_SETUP_ANS      = 0x05,
    /*!
     * DevStatusAns
     */
    MOTE_MAC_DEV_STATUS_ANS          = 0x06,
    /*!
     * NewChannelAns
     */
    MOTE_MAC_NEW_CHANNEL_ANS         = 0x07,
    /*!
     * RXTimingSetupAns
     */
    MOTE_MAC_RX_TIMING_SETUP_ANS     = 0x08,
    /*!
     * TXParamSetupAns
     */
    MOTE_MAC_TX_PARAM_SETUP_ANS      = 0x09,
    /*!
     * DlChannelAns
     */
    MOTE_MAC_DL_CHANNEL_ANS          = 0x0A,
    /*!
     * DeviceTimeReq
     */
    MOTE_MAC_DEVICE_TIME_REQ         = 0x0D,
    /*!
     * PingSlotInfoReq
     */
    MOTE_MAC_PING_SLOT_INFO_REQ      = 0x10,
    /*!
     * PingSlotFreqAns
     */
    MOTE_MAC_PING_SLOT_FREQ_ANS      = 0x11,
    /*!
     * BeaconTimingReq
     */
    MOTE_MAC_BEACON_TIMING_REQ       = 0x12,
    /*!
     * BeaconFreqAns
     */
    MOTE_MAC_BEACON_FREQ_ANS         = 0x13,
}LoRaMacMoteCmd_t;

/*!
 * LoRaMAC server MAC commands
 *
 * LoRaWAN Specification V1.1.0 chapter 5, table 4
 */
typedef enum eLoRaMacSrvCmd
{
    /*!
     * ResetInd
     */
    SRV_MAC_RESET_CONF               = 0x01,
    /*!
     * LinkCheckAns
     */
    SRV_MAC_LINK_CHECK_ANS           = 0x02,
    /*!
     * LinkADRReq
     */
    SRV_MAC_LINK_ADR_REQ             = 0x03,
    /*!
     * DutyCycleReq
     */
    SRV_MAC_DUTY_CYCLE_REQ           = 0x04,
    /*!
     * RXParamSetupReq
     */
    SRV_MAC_RX_PARAM_SETUP_REQ       = 0x05,
    /*!
     * DevStatusReq
     */
    SRV_MAC_DEV_STATUS_REQ           = 0x06,
    /*!
     * NewChannelReq
     */
    SRV_MAC_NEW_CHANNEL_REQ          = 0x07,
    /*!
     * RXTimingSetupReq
     */
    SRV_MAC_RX_TIMING_SETUP_REQ      = 0x08,
    /*!
     * NewChannelReq
     */
    SRV_MAC_TX_PARAM_SETUP_REQ       = 0x09,
    /*!
     * DlChannelReq
     */
    SRV_MAC_DL_CHANNEL_REQ           = 0x0A,
    /*!
     * DeviceTimeAns
     */
    SRV_MAC_DEVICE_TIME_ANS          = 0x0D,
    /*!
     * PingSlotInfoAns
     */
    SRV_MAC_PING_SLOT_INFO_ANS       = 0x10,
    /*!
     * PingSlotChannelReq
     */
    SRV_MAC_PING_SLOT_CHANNEL_REQ    = 0x11,
    /*!
     * BeaconTimingAns
     */
    SRV_MAC_BEACON_TIMING_ANS        = 0x12,
    /*!
     * BeaconFreqReq
     */
    SRV_MAC_BEACON_FREQ_REQ          = 0x13,
}LoRaMacSrvCmd_t;

/*!
 * LoRaMAC band parameters definition
 */
typedef struct sBand
{
    /*!
     * Duty cycle
     */
    uint16_t DCycle;
    /*!
     * Maximum Tx power
     */
    int8_t TxMaxPower;
    /*!
     * The last time the band has been
     * synchronized with the current time
     */
    TimerTime_t LastBandUpdateTime;
    /*!
     * Current time credits which are available. This
     * is a value in ms
     */
    TimerTime_t TimeCredits;
    /*!
     * Maximum time credits which are available. This
     * is a value in ms
     */
    TimerTime_t MaxTimeCredits;
    /*!
     * Set to true when the band is ready for use.
     */
    bool ReadyForTransmission;
}Band_t;

/*!
 * LoRaMAC frame types
 *
 * LoRaWAN Specification V1.0.2, chapter 4.2.1, table 1
 */
typedef enum eLoRaMacFrameType
{
    /*!
     * LoRaMAC join request frame
     */
    FRAME_TYPE_JOIN_REQ              = 0x00,
    /*!
     * LoRaMAC join accept frame
     */
    FRAME_TYPE_JOIN_ACCEPT           = 0x01,
    /*!
     * LoRaMAC unconfirmed up-link frame
     */
    FRAME_TYPE_DATA_UNCONFIRMED_UP   = 0x02,
    /*!
     * LoRaMAC unconfirmed down-link frame
     */
    FRAME_TYPE_DATA_UNCONFIRMED_DOWN = 0x03,
    /*!
     * LoRaMAC confirmed up-link frame
     */
    FRAME_TYPE_DATA_CONFIRMED_UP     = 0x04,
    /*!
     * LoRaMAC confirmed down-link frame
     */
    FRAME_TYPE_DATA_CONFIRMED_DOWN   = 0x05,
    /*!
     * LoRaMAC proprietary frame
     */
    FRAME_TYPE_PROPRIETARY           = 0x07,
}LoRaMacFrameType_t;

/*!
 * LoRaMAC Battery level indicator
 */
typedef enum eLoRaMacBatteryLevel
{
    /*!
     * External power source
     */
    BAT_LEVEL_EXT_SRC                = 0x00,
    /*!
     * Battery level empty
     */
    BAT_LEVEL_EMPTY                  = 0x01,
    /*!
     * Battery level full
     */
    BAT_LEVEL_FULL                   = 0xFE,
    /*!
     * Battery level - no measurement available
     */
    BAT_LEVEL_NO_MEASURE             = 0xFF,
}LoRaMacBatteryLevel_t;

#ifdef __cplusplus
}
#endif

#endif // __LORAMAC_TYPES_H__

