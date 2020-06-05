/*!
 * \file      LoRaMac.h
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
 *
 * \defgroup  LORAMAC LoRa MAC layer implementation
 *            This module specifies the API implementation of the LoRaMAC layer.
 *            This is a placeholder for a detailed description of the LoRaMac
 *            layer and the supported features.
 * \{
 *
 * \example   classA/B-L072Z-LRWAN1/main.c
 *            LoRaWAN class A application example for the B-L072Z-LRWAN1.
 *
 * \example   classB/B-L072Z-LRWAN1/main.c
 *            LoRaWAN class B application example for the B-L072Z-LRWAN1.
 *
 * \example   classC/B-L072Z-LRWAN1/main.c
 *            LoRaWAN class C application example for the B-L072Z-LRWAN1.
 *
 * \example   classA/NAMote72/main.c
 *            LoRaWAN class A application example for the NAMote72.
 *
 * \example   classB/NAMote72/main.c
 *            LoRaWAN class B application example for the NAMote72.
 *
 * \example   classC/NAMote72/main.c
 *            LoRaWAN class C application example for the NAMote72.
 *
 * \example   classA/NucleoL073/main.c
 *            LoRaWAN class A application example for the NucleoL073.
 *
 * \example   classB/NucleoL073/main.c
 *            LoRaWAN class B application example for the NucleoL073.
 *
 * \example   classC/NucleoL073/main.c
 *            LoRaWAN class C application example for the NucleoL073.
 *
 * \example   classA/NucleoL152/main.c
 *            LoRaWAN class A application example for the NucleoL152.
 *
 * \example   classB/NucleoL152/main.c
 *            LoRaWAN class B application example for the NucleoL152.
 *
 * \example   classC/NucleoL152/main.c
 *            LoRaWAN class C application example for the NucleoL152.
 *
 * \example   classA/NucleoL476/main.c
 *            LoRaWAN class A application example for the NucleoL476.
 *
 * \example   classB/NucleoL476/main.c
 *            LoRaWAN class B application example for the NucleoL476.
 *
 * \example   classC/NucleoL476/main.c
 *            LoRaWAN class C application example for the NucleoL476.
 *
 * \example   classA/SAMR34/main.c
 *            LoRaWAN class A application example for the SAMR34.
 *
 * \example   classB/SAMR34/main.c
 *            LoRaWAN class B application example for the SAMR34.
 *
 * \example   classC/SAMR34/main.c
 *            LoRaWAN class C application example for the SAMR34.
 *
 * \example   classA/SKiM880B/main.c
 *            LoRaWAN class A application example for the SKiM880B.
 *
 * \example   classB/SKiM880B/main.c
 *            LoRaWAN class B application example for the SKiM880B.
 *
 * \example   classC/SKiM880B/main.c
 *            LoRaWAN class C application example for the SKiM880B.
 *
 * \example   classA/SKiM881AXL/main.c
 *            LoRaWAN class A application example for the SKiM881AXL.
 *
 * \example   classB/SKiM881AXL/main.c
 *            LoRaWAN class B application example for the SKiM881AXL.
 *
 * \example   classC/SKiM881AXL/main.c
 *            LoRaWAN class C application example for the SKiM881AXL.
 *
 * \example   classA/SKiM980A/main.c
 *            LoRaWAN class A application example for the SKiM980A.
 *
 * \example   classB/SKiM980A/main.c
 *            LoRaWAN class B application example for the SKiM980A.
 *
 * \example   classC/SKiM980A/main.c
 *            LoRaWAN class C application example for the SKiM980A.
 *
 */
#ifndef __LORAMAC_H__
#define __LORAMAC_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>
#include "utilities.h"
#include "timer.h"
#include "systime.h"
#include "radio.h"
#include "LoRaMacTypes.h"

/*!
 * Maximum number of times the MAC layer tries to get an acknowledge.
 */
#define MAX_ACK_RETRIES                             8

/*!
 * Frame direction definition for up-link communications
 */
#define UP_LINK                                     0

/*!
 * Frame direction definition for down-link communications
 */
#define DOWN_LINK                                   1

/*!
 * LoRaMac MLME-Confirm queue length
 */
#define LORA_MAC_MLME_CONFIRM_QUEUE_LEN             5

/*!
 * Maximum number of multicast context
 */
#define   LORAMAC_MAX_MC_CTX       4

/*!
 * Start value for multicast keys enumeration
 */
#define LORAMAC_CRYPTO_MULTICAST_KEYS   127

/*!
 * End-Device activation type
 */
typedef enum eActivationType
{
    /*!
     * None
     */
    ACTIVATION_TYPE_NONE = 0,
    /*!
     * Activation By Personalization (ACTIVATION_TYPE_ABP)
     */
    ACTIVATION_TYPE_ABP = 1,
    /*!
     * Over-The-Air Activation (ACTIVATION_TYPE_OTAA)
     */
    ACTIVATION_TYPE_OTAA = 2,
}ActivationType_t;

/*!
 * LoRaMAC channels parameters definition
 */
typedef union uDrRange
{
    /*!
     * Byte-access to the bits
     */
    int8_t Value;
    /*!
     * Structure to store the minimum and the maximum datarate
     */
    struct sFields
    {
        /*!
         * Minimum data rate
         *
         * LoRaWAN Regional Parameters V1.0.2rB
         *
         * The allowed ranges are region specific. Please refer to \ref DR_0 to \ref DR_15 for details.
         */
        int8_t Min : 4;
        /*!
         * Maximum data rate
         *
         * LoRaWAN Regional Parameters V1.0.2rB
         *
         * The allowed ranges are region specific. Please refer to \ref DR_0 to \ref DR_15 for details.
         */
        int8_t Max : 4;
    }Fields;
}DrRange_t;

/*!
 * LoRaMAC channel definition
 */
typedef struct sChannelParams
{
    /*!
     * Frequency in Hz
     */
    uint32_t Frequency;
    /*!
     * Alternative frequency for RX window 1
     */
    uint32_t Rx1Frequency;
    /*!
     * Data rate definition
     */
    DrRange_t DrRange;
    /*!
     * Band index
     */
    uint8_t Band;
}ChannelParams_t;

/*!
 * LoRaMAC receive window channel parameters
 */
typedef struct sRxChannelParams
{
    /*!
     * Frequency in Hz
     */
    uint32_t Frequency;
    /*!
     * Data rate
     *
     * LoRaWAN Regional Parameters V1.0.2rB
     *
     * The allowed ranges are region specific. Please refer to \ref DR_0 to \ref DR_15 for details.
     */
    uint8_t  Datarate;
}RxChannelParams_t;

/*!
 * LoRaMAC receive window enumeration
 */
typedef enum eLoRaMacRxSlot
{
    /*!
     * LoRaMAC receive window 1
     */
    RX_SLOT_WIN_1,
    /*!
     * LoRaMAC receive window 2
     */
    RX_SLOT_WIN_2,
    /*!
     * LoRaMAC receive window 2 for class c - continuous listening
     */
    RX_SLOT_WIN_CLASS_C,
    /*!
     * LoRaMAC class c multicast downlink
     */
    RX_SLOT_WIN_CLASS_C_MULTICAST,
    /*!
     * LoRaMAC class b ping slot window
     */
    RX_SLOT_WIN_CLASS_B_PING_SLOT,
    /*!
     * LoRaMAC class b multicast slot window
     */
    RX_SLOT_WIN_CLASS_B_MULTICAST_SLOT,
    /*!
     * LoRaMAC no active receive window
     */
    RX_SLOT_NONE,
}LoRaMacRxSlot_t;

/*!
 * LoRaMAC structure to hold internal context pointers and its lengths
 */
typedef struct sLoRaMacCtxs
{
    /*!
     * \brief   Pointer to Mac context
     */
    void* MacNvmCtx;
    /*!
     * \brief   Size of Mac context
     */
    size_t MacNvmCtxSize;
    /*!
     * \brief   Pointer to region context
     */
    void* RegionNvmCtx;
    /*!
     * \brief   Size of region context
     */
    size_t RegionNvmCtxSize;
    /*!
     * \brief   Pointer to crypto module context
     */
    void* CryptoNvmCtx;
    /*!
     * \brief   Size of crypto module context
     */
    size_t CryptoNvmCtxSize;
    /*!
     * \brief   Pointer to secure element driver context
     */
    void* SecureElementNvmCtx;
    /*!
     * \brief   Size of secure element driver context
     */
    size_t SecureElementNvmCtxSize;
    /*!
     * \brief   Pointer to MAC commands module context
     */
    void* CommandsNvmCtx;
    /*!
     * \brief   Size of MAC commands module context
     */
    size_t CommandsNvmCtxSize;
    /*!
     * \brief   Pointer to Class B module context
     */
    void* ClassBNvmCtx;
    /*!
     * \brief   Size of MAC Class B module context
     */
    size_t ClassBNvmCtxSize;
    /*!
     * \brief   Pointer to MLME Confirm queue module context
     */
    void* ConfirmQueueNvmCtx;
    /*!
     * \brief   Size of MLME Confirm queue module context
     */
    size_t ConfirmQueueNvmCtxSize;
}LoRaMacCtxs_t;

/*!
 * Global MAC layer parameters
 */
typedef struct sLoRaMacParams
{
    /*!
     * Channels TX power
     */
    int8_t ChannelsTxPower;
    /*!
     * Channels data rate
     */
    int8_t ChannelsDatarate;
    /*!
     * System overall timing error in milliseconds.
     * [-SystemMaxRxError : +SystemMaxRxError]
     * Default: +/-10 ms
     */
    uint32_t SystemMaxRxError;
    /*!
     * Minimum required number of symbols to detect an Rx frame
     * Default: 6 symbols
     */
    uint8_t MinRxSymbols;
    /*!
     * LoRaMac maximum time a reception window stays open
     */
    uint32_t MaxRxWindow;
    /*!
     * Receive delay 1
     */
    uint32_t ReceiveDelay1;
    /*!
     * Receive delay 2
     */
    uint32_t ReceiveDelay2;
    /*!
     * Join accept delay 1
     */
    uint32_t JoinAcceptDelay1;
    /*!
     * Join accept delay 1
     */
    uint32_t JoinAcceptDelay2;
    /*!
     * Number of uplink messages repetitions [1:15] (unconfirmed messages only)
     */
    uint8_t ChannelsNbTrans;
    /*!
     * Datarate offset between uplink and downlink on first window
     */
    uint8_t Rx1DrOffset;
    /*!
     * LoRaMAC 2nd reception window settings
     */
    RxChannelParams_t Rx2Channel;
    /*!
     * LoRaMAC continuous reception window settings
     */
    RxChannelParams_t RxCChannel;
    /*!
     * Uplink dwell time configuration. 0: No limit, 1: 400ms
     */
    uint8_t UplinkDwellTime;
    /*!
     * Downlink dwell time configuration. 0: No limit, 1: 400ms
     */
    uint8_t DownlinkDwellTime;
    /*!
     * Maximum possible EIRP
     */
    float MaxEirp;
    /*!
     * Antenna gain of the node
     */
    float AntennaGain;
}LoRaMacParams_t;

/*!
 * LoRaMAC data structure for a PingSlotInfoReq \ref MLME_PING_SLOT_INFO
 *
 * LoRaWAN Specification
 */
typedef union uPingSlotInfo
{
    /*!
     * Parameter for byte access
     */
    uint8_t Value;
    /*!
     * Structure containing the parameters for the PingSlotInfoReq
     */
    struct sInfoFields
    {
        /*!
         * Periodicity = 0: ping slot every second
         * Periodicity = 7: ping slot every 128 seconds
         */
        uint8_t Periodicity     : 3;
        /*!
         * RFU
         */
        uint8_t RFU             : 5;
    }Fields;
}PingSlotInfo_t;

/*!
 * LoRaMAC data structure for the \ref MLME_BEACON MLME-Indication
 *
 * LoRaWAN Specification
 */
typedef struct sBeaconInfo
{
    /*!
     * Timestamp in seconds since 00:00:00, Sunday 6th of January 1980
     * (start of the GPS epoch) modulo 2^32
     */
    SysTime_t Time;
    /*!
     * Frequency
     */
    uint32_t Frequency;
    /*!
     * Datarate
     */
    uint8_t Datarate;
    /*!
     * RSSI
     */
    int16_t Rssi;
    /*!
     * SNR
     */
    int8_t Snr;
    /*!
     * Data structure for the gateway specific part. The
     * content of the values may differ for each gateway
     */
    struct sGwSpecific
    {
        /*!
         * Info descriptor - can differ for each gateway
         */
        uint8_t InfoDesc;
        /*!
         * Info - can differ for each gateway
         */
        uint8_t Info[6];
    }GwSpecific;
}BeaconInfo_t;

/*!
 * Enumeration containing the status of the operation of a MAC service
 */
typedef enum eLoRaMacEventInfoStatus
{
    /*!
     * Service performed successfully
     */
    LORAMAC_EVENT_INFO_STATUS_OK = 0,
    /*!
     * An error occurred during the execution of the service
     */
    LORAMAC_EVENT_INFO_STATUS_ERROR,
    /*!
     * A Tx timeout occurred
     */
    LORAMAC_EVENT_INFO_STATUS_TX_TIMEOUT,
    /*!
     * An Rx timeout occurred on receive window 1
     */
    LORAMAC_EVENT_INFO_STATUS_RX1_TIMEOUT,
    /*!
     * An Rx timeout occurred on receive window 2
     */
    LORAMAC_EVENT_INFO_STATUS_RX2_TIMEOUT,
    /*!
     * An Rx error occurred on receive window 1
     */
    LORAMAC_EVENT_INFO_STATUS_RX1_ERROR,
    /*!
     * An Rx error occurred on receive window 2
     */
    LORAMAC_EVENT_INFO_STATUS_RX2_ERROR,
    /*!
     * An error occurred in the join procedure
     */
    LORAMAC_EVENT_INFO_STATUS_JOIN_FAIL,
    /*!
     * A frame with an invalid downlink counter was received. The
     * downlink counter of the frame was equal to the local copy
     * of the downlink counter of the node.
     */
    LORAMAC_EVENT_INFO_STATUS_DOWNLINK_REPEATED,
    /*!
     * The MAC could not retransmit a frame since the MAC decreased the datarate. The
     * payload size is not applicable for the datarate.
     */
    LORAMAC_EVENT_INFO_STATUS_TX_DR_PAYLOAD_SIZE_ERROR,
    /*!
     * The node has lost MAX_FCNT_GAP or more frames.
     */
    LORAMAC_EVENT_INFO_STATUS_DOWNLINK_TOO_MANY_FRAMES_LOSS,
    /*!
     * An address error occurred
     */
    LORAMAC_EVENT_INFO_STATUS_ADDRESS_FAIL,
    /*!
     * Message integrity check failure
     */
    LORAMAC_EVENT_INFO_STATUS_MIC_FAIL,
    /*!
     * ToDo
     */
    LORAMAC_EVENT_INFO_STATUS_MULTICAST_FAIL,
    /*!
     * ToDo
     */
    LORAMAC_EVENT_INFO_STATUS_BEACON_LOCKED,
    /*!
     * ToDo
     */
    LORAMAC_EVENT_INFO_STATUS_BEACON_LOST,
    /*!
     * ToDo
     */
    LORAMAC_EVENT_INFO_STATUS_BEACON_NOT_FOUND,
}LoRaMacEventInfoStatus_t;

/*!
 * LoRaMac tx/rx operation state
 */
typedef union eLoRaMacFlags_t
{
    /*!
     * Byte-access to the bits
     */
    uint8_t Value;
    /*!
     * Structure containing single access to bits
     */
    struct sMacFlagBits
    {
        /*!
         * MCPS-Req pending
         */
        uint8_t McpsReq                 : 1;
        /*!
         * MCPS-Ind pending
         */
        uint8_t McpsInd                 : 1;
        /*!
         * MLME-Req pending
         */
        uint8_t MlmeReq                 : 1;
        /*!
         * MLME-Ind pending
         */
        uint8_t MlmeInd                 : 1;
        /*!
         * MLME-Ind to schedule an uplink pending
         */
        uint8_t MlmeSchedUplinkInd      : 1;
        /*!
         * MAC cycle done
         */
        uint8_t MacDone                 : 1;
    }Bits;
}LoRaMacFlags_t;

/*!
 *
 * \brief   LoRaMAC data services
 *
 * \details The following table list the primitives which are supported by the
 *          specific MAC data service:
 *
 * Name                  | Request | Indication | Response | Confirm
 * --------------------- | :-----: | :--------: | :------: | :-----:
 * \ref MCPS_UNCONFIRMED | YES     | YES        | NO       | YES
 * \ref MCPS_CONFIRMED   | YES     | YES        | NO       | YES
 * \ref MCPS_MULTICAST   | NO      | YES        | NO       | NO
 * \ref MCPS_PROPRIETARY | YES     | YES        | NO       | YES
 *
 * The following table provides links to the function implementations of the
 * related MCPS primitives:
 *
 * Primitive        | Function
 * ---------------- | :---------------------:
 * MCPS-Request     | \ref LoRaMacMlmeRequest
 * MCPS-Confirm     | MacMcpsConfirm in \ref LoRaMacPrimitives_t
 * MCPS-Indication  | MacMcpsIndication in \ref LoRaMacPrimitives_t
 */
typedef enum eMcps
{
    /*!
     * Unconfirmed LoRaMAC frame
     */
    MCPS_UNCONFIRMED,
    /*!
     * Confirmed LoRaMAC frame
     */
    MCPS_CONFIRMED,
    /*!
     * Multicast LoRaMAC frame
     */
    MCPS_MULTICAST,
    /*!
     * Proprietary frame
     */
    MCPS_PROPRIETARY,
}Mcps_t;

/*!
 * Structure which defines return parameters for requests.
 */
typedef struct sRequestReturnParam
{
    /*!
     * This value reports the time in milliseconds which
     * an application must wait before its possible to send
     * the next uplink.
     */
    TimerTime_t DutyCycleWaitTime;
}RequestReturnParam_t;

/*!
 * LoRaMAC MCPS-Request for an unconfirmed frame
 */
typedef struct sMcpsReqUnconfirmed
{
    /*!
     * Frame port field. Must be set if the payload is not empty. Use the
     * application specific frame port values: [1...223]
     *
     * LoRaWAN Specification V1.0.2, chapter 4.3.2
     */
    uint8_t fPort;
    /*!
     * Pointer to the buffer of the frame payload
     */
    void* fBuffer;
    /*!
     * Size of the frame payload
     */
    uint16_t fBufferSize;
    /*!
     * Uplink datarate, if ADR is off
     */
    int8_t Datarate;
}McpsReqUnconfirmed_t;

/*!
 * LoRaMAC MCPS-Request for a confirmed frame
 */
typedef struct sMcpsReqConfirmed
{
    /*!
     * Frame port field. Must be set if the payload is not empty. Use the
     * application specific frame port values: [1...223]
     *
     * LoRaWAN Specification V1.0.2, chapter 4.3.2
     */
    uint8_t fPort;
    /*!
     * Pointer to the buffer of the frame payload
     */
    void* fBuffer;
    /*!
     * Size of the frame payload
     */
    uint16_t fBufferSize;
    /*!
     * Uplink datarate, if ADR is off
     */
    int8_t Datarate;
    /*!
     * Number of trials to transmit the frame, if the LoRaMAC layer did not
     * receive an acknowledgment. The MAC performs a datarate adaptation,
     * according to the LoRaWAN Specification V1.0.2, chapter 18.4, according
     * to the following table:
     *
     * Transmission nb | Data Rate
     * ----------------|-----------
     * 1 (first)       | DR
     * 2               | DR
     * 3               | max(DR-1,0)
     * 4               | max(DR-1,0)
     * 5               | max(DR-2,0)
     * 6               | max(DR-2,0)
     * 7               | max(DR-3,0)
     * 8               | max(DR-3,0)
     *
     * Note, that if NbTrials is set to 1 or 2, the MAC will not decrease
     * the datarate, in case the LoRaMAC layer did not receive an acknowledgment
     */
    uint8_t NbTrials;
}McpsReqConfirmed_t;

/*!
 * LoRaMAC MCPS-Request for a proprietary frame
 */
typedef struct sMcpsReqProprietary
{
    /*!
     * Pointer to the buffer of the frame payload
     */
    void* fBuffer;
    /*!
     * Size of the frame payload
     */
    uint16_t fBufferSize;
    /*!
     * Uplink datarate, if ADR is off
     */
    int8_t Datarate;
}McpsReqProprietary_t;

/*!
 * LoRaMAC MCPS-Request structure
 */
typedef struct sMcpsReq
{
    /*!
     * MCPS-Request type
     */
    Mcps_t Type;

    /*!
     * MCPS-Request parameters
     */
    union uMcpsParam
    {
        /*!
         * MCPS-Request parameters for an unconfirmed frame
         */
        McpsReqUnconfirmed_t Unconfirmed;
        /*!
         * MCPS-Request parameters for a confirmed frame
         */
        McpsReqConfirmed_t Confirmed;
        /*!
         * MCPS-Request parameters for a proprietary frame
         */
        McpsReqProprietary_t Proprietary;
    }Req;

    /*!
     * MCPS-Request return parameters
     */
    RequestReturnParam_t ReqReturn;
}McpsReq_t;

/*!
 * LoRaMAC MCPS-Confirm
 */
typedef struct sMcpsConfirm
{
    /*!
     * Holds the previously performed MCPS-Request
     */
    Mcps_t McpsRequest;
    /*!
     * Status of the operation
     */
    LoRaMacEventInfoStatus_t Status;
    /*!
     * Uplink datarate
     */
    uint8_t Datarate;
    /*!
     * Transmission power
     */
    int8_t TxPower;
    /*!
     * Set if an acknowledgement was received
     */
    bool AckReceived;
    /*!
     * Provides the number of retransmissions
     */
    uint8_t NbRetries;
    /*!
     * The transmission time on air of the frame
     */
    TimerTime_t TxTimeOnAir;
    /*!
     * The uplink counter value related to the frame
     */
    uint32_t UpLinkCounter;
    /*!
     * The uplink channel related to the frame
     */
    uint32_t Channel;
}McpsConfirm_t;

/*!
 * LoRaMAC MCPS-Indication primitive
 */
typedef struct sMcpsIndication
{
    /*!
     * MCPS-Indication type
     */
    Mcps_t McpsIndication;
    /*!
     * Status of the operation
     */
    LoRaMacEventInfoStatus_t Status;
    /*!
     * Multicast
     */
    uint8_t Multicast;
    /*!
     * Application port
     */
    uint8_t Port;
    /*!
     * Downlink datarate
     */
    uint8_t RxDatarate;
    /*!
     * Frame pending status
     */
    uint8_t FramePending;
    /*!
     * Pointer to the received data stream
     */
    uint8_t* Buffer;
    /*!
     * Size of the received data stream
     */
    uint8_t BufferSize;
    /*!
     * Indicates, if data is available
     */
    bool RxData;
    /*!
     * Rssi of the received packet
     */
    int16_t Rssi;
    /*!
     * Snr of the received packet
     */
    int8_t Snr;
    /*!
     * Receive window
     */
    LoRaMacRxSlot_t RxSlot;
    /*!
     * Set if an acknowledgement was received
     */
    bool AckReceived;
    /*!
     * The downlink counter value for the received frame
     */
    uint32_t DownLinkCounter;
    /*!
     * The device address of the frame
     */
    uint32_t DevAddress;
    /*!
     * Set if a DeviceTimeAns MAC command was received.
     */
    bool DeviceTimeAnsReceived;
}McpsIndication_t;

/*!
 * \brief LoRaMAC management services
 *
 * \details The following table list the primitives which are supported by the
 *          specific MAC management service:
 *
 * Name                         | Request | Indication | Response | Confirm
 * ---------------------------- | :-----: | :--------: | :------: | :-----:
 * \ref MLME_JOIN               | YES     | NO         | NO       | YES
 * \ref MLME_LINK_CHECK         | YES     | NO         | NO       | YES
 * \ref MLME_TXCW               | YES     | NO         | NO       | YES
 * \ref MLME_SCHEDULE_UPLINK    | NO      | YES        | NO       | NO
 * \ref MLME_DERIVE_MC_KE_KEY   | YES     | NO         | NO       | YES
 * \ref MLME_DERIVE_MC_KEY_PAIR | YES     | NO         | NO       | YES
 *
 * The following table provides links to the function implementations of the
 * related MLME primitives.
 *
 * Primitive        | Function
 * ---------------- | :---------------------:
 * MLME-Request     | \ref LoRaMacMlmeRequest
 * MLME-Confirm     | MacMlmeConfirm in \ref LoRaMacPrimitives_t
 * MLME-Indication  | MacMlmeIndication in \ref LoRaMacPrimitives_t
 */
typedef enum eMlme
{
    /*!
     * An unknown MLME service
     */
    MLME_UNKNOWN,
    /*!
     * Initiates the Over-the-Air activation
     *
     * LoRaWAN Specification V1.0.2, chapter 6.2
     */
    MLME_JOIN,
    /*!
     * Initiates sending a ReJoin-request type 0
     *
     * LoRaWAN Specification V1.1.0, chapter 6.2.4.1
     */
    MLME_REJOIN_0,
    /*!
     * Initiates sending a ReJoin-request type 1
     *
     * LoRaWAN Specification V1.1.0, chapter 6.2.4.2
     */
    MLME_REJOIN_1,
    /*!
     * LinkCheckReq - Connectivity validation
     *
     * LoRaWAN Specification V1.0.2, chapter 5, table 4
     */
    MLME_LINK_CHECK,
    /*!
     * Sets Tx continuous wave mode
     *
     * LoRaWAN end-device certification
     */
    MLME_TXCW,
    /*!
     * Sets Tx continuous wave mode (new LoRa-Alliance CC definition)
     *
     * LoRaWAN end-device certification
     */
    MLME_TXCW_1,
    /*!
     * Indicates that the application shall perform an uplink as
     * soon as possible.
     */
    MLME_SCHEDULE_UPLINK,
    /*!
     * Derives the McKEKey from the AppKey or NwkKey.
     */
    MLME_DERIVE_MC_KE_KEY,
    /*!
     * Derives a Multicast group key pair ( McAppSKey, McNwkSKey ) from McKey
     */
    MLME_DERIVE_MC_KEY_PAIR,
    /*!
     * Initiates a DeviceTimeReq
     *
     * LoRaWAN end-device certification
     */
    MLME_DEVICE_TIME,
    /*!
     * The MAC uses this MLME primitive to indicate a beacon reception
     * status.
     *
     * LoRaWAN end-device certification
     */
    MLME_BEACON,
    /*!
     * Initiate a beacon acquisition. The MAC will search for a beacon.
     * It will search for XX_BEACON_INTERVAL milliseconds.
     *
     * LoRaWAN end-device certification
     */
    MLME_BEACON_ACQUISITION,
    /*!
     * Initiates a PingSlotInfoReq
     *
     * LoRaWAN end-device certification
     */
    MLME_PING_SLOT_INFO,
    /*!
     * Initiates a BeaconTimingReq
     *
     * LoRaWAN end-device certification
     */
    MLME_BEACON_TIMING,
    /*!
     * Primitive which indicates that the beacon has been lost
     *
     * \remark The upper layer is required to switch the device class to ClassA
     *
     * LoRaWAN end-device certification
     */
    MLME_BEACON_LOST,
}Mlme_t;

/*!
 * LoRaMAC MLME-Request for the join service
 */
typedef struct sMlmeReqJoin
{
    /*!
     * Datarate used for join request.
     */
    uint8_t Datarate;
}MlmeReqJoin_t;

/*!
 * LoRaMAC MLME-Request for Tx continuous wave mode
 */
typedef struct sMlmeReqTxCw
{
    /*!
     * Time in seconds while the radio is kept in continuous wave mode
     */
    uint16_t Timeout;
    /*!
     * RF frequency to set (Only used with new way)
     */
    uint32_t Frequency;
    /*!
     * RF output power to set (Only used with new way)
     */
    uint8_t Power;
}MlmeReqTxCw_t;

/*!
 * LoRaMAC MLME-Request for the ping slot info service
 */
typedef struct sMlmeReqPingSlotInfo
{
    PingSlotInfo_t PingSlot;
}MlmeReqPingSlotInfo_t;

/*!
 * LoRaMAC MLME-Request to derive the McKEKey from the AppKey or NwkKey
 */
typedef struct sMlmeReqDeriveMcKEKey
{
    /*!
     *  Key identifier of the root key to use to perform the derivation ( NwkKey or AppKey )
     */
    KeyIdentifier_t KeyID;
    /*!
     * Nonce value ( nonce <= 15)
     */
    uint16_t Nonce;
    /*!
     * DevEUI Value
     */
    uint8_t* DevEUI;
}MlmeReqDeriveMcKEKey_t;

/*!
 * LoRaMAC MLME-Request to derive a Multicast group key pair ( McAppSKey, McNwkSKey ) from McKey
 */
typedef struct sMlmeReqDeriveMcSessionKeyPair
{
    /*!
     *  Address identifier to select the multicast group
     */
    AddressIdentifier_t GroupID;
}MlmeReqDeriveMcSessionKeyPair_t;

/*!
 * LoRaMAC MLME-Request structure
 */
typedef struct sMlmeReq
{
    /*!
     * MLME-Request type
     */
    Mlme_t Type;

    /*!
     * MLME-Request parameters
     */
    union uMlmeParam
    {
        /*!
         * MLME-Request parameters for a join request
         */
        MlmeReqJoin_t Join;
        /*!
         * MLME-Request parameters for Tx continuous mode request
         */
        MlmeReqTxCw_t TxCw;
        /*!
         * MLME-Request parameters for a ping slot info request
         */
        MlmeReqPingSlotInfo_t PingSlotInfo;
        /*!
         * MLME-Request to derive the McKEKey from the AppKey or NwkKey
         */
        MlmeReqDeriveMcKEKey_t DeriveMcKEKey;
        /*!
         * MLME-Request to derive a Multicast group key pair ( McAppSKey, McNwkSKey ) from McKey
         */
        MlmeReqDeriveMcSessionKeyPair_t DeriveMcSessionKeyPair;
    }Req;

    /*!
     * MLME-Request return parameters
     */
    RequestReturnParam_t ReqReturn;
}MlmeReq_t;

/*!
 * LoRaMAC MLME-Confirm primitive
 */
typedef struct sMlmeConfirm
{
    /*!
     * Holds the previously performed MLME-Request
     */
    Mlme_t MlmeRequest;
    /*!
     * Status of the operation
     */
    LoRaMacEventInfoStatus_t Status;
    /*!
     * The transmission time on air of the frame
     */
    TimerTime_t TxTimeOnAir;
    /*!
     * Demodulation margin. Contains the link margin [dB] of the last
     * successfully received LinkCheckReq
     */
    uint8_t DemodMargin;
    /*!
     * Number of gateways which received the last LinkCheckReq
     */
    uint8_t NbGateways;
    /*!
     * Provides the number of retransmissions
     */
    uint8_t NbRetries;
    /*!
     * The delay which we have received through the
     * BeaconTimingAns
     */
    TimerTime_t BeaconTimingDelay;
    /*!
     * The channel of the next beacon
     */
    uint8_t BeaconTimingChannel;
}MlmeConfirm_t;

/*!
 * LoRaMAC MLME-Indication primitive
 */
typedef struct sMlmeIndication
{
    /*!
     * MLME-Indication type
     */
    Mlme_t MlmeIndication;
    /*!
     * Status of the operation
     */
    LoRaMacEventInfoStatus_t Status;
    /*!
     * Beacon information. Only valid for \ref MLME_BEACON,
     * status \ref LORAMAC_EVENT_INFO_STATUS_BEACON_LOCKED
     */
    BeaconInfo_t BeaconInfo;
}MlmeIndication_t;

/*!
 * LoRa Mac Information Base (MIB)
 *
 * The following table lists the MIB parameters and the related attributes:
 *
 * Attribute                                     | Get | Set
 * ----------------------------------------------| :-: | :-:
 * \ref MIB_DEVICE_CLASS                         | YES | YES
 * \ref MIB_NETWORK_ACTIVATION                   | YES | YES
 * \ref MIB_DEV_EUI                              | YES | YES
 * \ref MIB_JOIN_EUI                             | YES | YES
 * \ref MIB_SE_PIN                               | YES | YES
 * \ref MIB_ADR                                  | YES | YES
 * \ref MIB_NET_ID                               | YES | YES
 * \ref MIB_DEV_ADDR                             | YES | YES
 * \ref MIB_APP_KEY                              | NO  | YES
 * \ref MIB_NWK_KEY                              | NO  | YES
 * \ref MIB_J_S_INT_KEY                          | NO  | YES
 * \ref MIB_J_S_ENC_KEY                          | NO  | YES
 * \ref MIB_F_NWK_S_INT_KEY                      | NO  | YES
 * \ref MIB_S_NWK_S_INT_KEY                      | NO  | YES
 * \ref MIB_NWK_S_ENC_KEY                        | NO  | YES
 * \ref MIB_APP_S_KEY                            | NO  | YES
 * \ref MIB_MC_KE_KEY                            | NO  | YES
 * \ref MIB_MC_KEY_0                             | NO  | YES
 * \ref MIB_MC_APP_S_KEY_0                       | NO  | YES
 * \ref MIB_MC_NWK_S_KEY_0                       | NO  | YES
 * \ref MIB_MC_KEY_1                             | NO  | YES
 * \ref MIB_MC_APP_S_KEY_1                       | NO  | YES
 * \ref MIB_MC_NWK_S_KEY_1                       | NO  | YES
 * \ref MIB_MC_KEY_2                             | NO  | YES
 * \ref MIB_MC_APP_S_KEY_2                       | NO  | YES
 * \ref MIB_MC_NWK_S_KEY_2                       | NO  | YES
 * \ref MIB_MC_KEY_3                             | NO  | YES
 * \ref MIB_MC_APP_S_KEY_3                       | NO  | YES
 * \ref MIB_MC_NWK_S_KEY_3                       | NO  | YES
 * \ref MIB_PUBLIC_NETWORK                       | YES | YES
 * \ref MIB_CHANNELS                             | YES | NO
 * \ref MIB_RX2_CHANNEL                          | YES | YES
 * \ref MIB_RX2_DFAULT_CHANNEL                   | YES | YES
 * \ref MIB_RXC_CHANNEL                          | YES | YES
 * \ref MIB_RXC_DFAULT_CHANNEL                   | YES | YES
 * \ref MIB_CHANNELS_MASK                        | YES | YES
 * \ref MIB_CHANNELS_DEFAULT_MASK                | YES | YES
 * \ref MIB_CHANNELS_NB_TRANS                    | YES | YES
 * \ref MIB_MAX_RX_WINDOW_DURATION               | YES | YES
 * \ref MIB_RECEIVE_DELAY_1                      | YES | YES
 * \ref MIB_RECEIVE_DELAY_2                      | YES | YES
 * \ref MIB_JOIN_ACCEPT_DELAY_1                  | YES | YES
 * \ref MIB_JOIN_ACCEPT_DELAY_2                  | YES | YES
 * \ref MIB_CHANNELS_DATARATE                    | YES | YES
 * \ref MIB_CHANNELS_DEFAULT_DATARATE            | YES | YES
 * \ref MIB_CHANNELS_TX_POWER                    | YES | YES
 * \ref MIB_CHANNELS_DEFAULT_TX_POWER            | YES | YES
 * \ref MIB_SYSTEM_MAX_RX_ERROR                  | YES | YES
 * \ref MIB_MIN_RX_SYMBOLS                       | YES | YES
 * \ref MIB_BEACON_INTERVAL                      | YES | YES
 * \ref MIB_BEACON_RESERVED                      | YES | YES
 * \ref MIB_BEACON_GUARD                         | YES | YES
 * \ref MIB_BEACON_WINDOW                        | YES | YES
 * \ref MIB_BEACON_WINDOW_SLOTS                  | YES | YES
 * \ref MIB_PING_SLOT_WINDOW                     | YES | YES
 * \ref MIB_BEACON_SYMBOL_TO_DEFAULT             | YES | YES
 * \ref MIB_BEACON_SYMBOL_TO_EXPANSION_MAX       | YES | YES
 * \ref MIB_PING_SLOT_SYMBOL_TO_EXPANSION_MAX    | YES | YES
 * \ref MIB_BEACON_SYMBOL_TO_EXPANSION_FACTOR    | YES | YES
 * \ref MIB_PING_SLOT_SYMBOL_TO_EXPANSION_FACTOR | YES | YES
 * \ref MIB_MAX_BEACON_LESS_PERIOD               | YES | YES
 * \ref MIB_ANTENNA_GAIN                         | YES | YES
 * \ref MIB_DEFAULT_ANTENNA_GAIN                 | YES | YES
 * \ref MIB_NVM_CTXS                             | YES | YES
 * \ref MIB_ABP_LORAWAN_VERSION                  | NO  | YES
 * \ref MIB_LORAWAN_VERSION                      | YES | NO
 *
 * The following table provides links to the function implementations of the
 * related MIB primitives:
 *
 * Primitive        | Function
 * ---------------- | :---------------------:
 * MIB-Set          | \ref LoRaMacMibSetRequestConfirm
 * MIB-Get          | \ref LoRaMacMibGetRequestConfirm
 */
typedef enum eMib
{
    /*!
     * LoRaWAN device class
     *
     * LoRaWAN Specification V1.0.2
     */
    MIB_DEVICE_CLASS,
    /*!
     * LoRaWAN Network End-Device Activation
     *
     * LoRaWAN Specification V1.0.2
     */
    MIB_NETWORK_ACTIVATION,
    /*!
     * LoRaWAN device EUI
     *
     * LoRaWAN Specification V1.0.2
     */
    MIB_DEV_EUI,
    /*!
     * LoRaWAN join EUI
     *
     * LoRaWAN Specification V1.0.2
     */
    MIB_JOIN_EUI,
    /*!
     * Secure-element pin
     */
    MIB_SE_PIN,
    /*!
     * Adaptive data rate
     *
     * LoRaWAN Specification V1.0.2, chapter 4.3.1.1
     *
     * [true: ADR enabled, false: ADR disabled]
     */
    MIB_ADR,
    /*!
     * Network identifier
     *
     * LoRaWAN Specification V1.0.2, chapter 6.1.1
     */
    MIB_NET_ID,
    /*!
     * End-device address
     *
     * LoRaWAN Specification V1.0.2, chapter 6.1.1
     */
    MIB_DEV_ADDR,
    /*!
     * Application root key
     *
     * LoRaWAN Specification V1.1.0, chapter 6.1.1.3
     */
    MIB_APP_KEY,
    /*!
     * Network root key
     *
     * LoRaWAN Specification V1.1.0, chapter 6.1.1.3
     */
    MIB_NWK_KEY,
    /*!
     * Join session integrity key
     *
     * LoRaWAN Specification V1.1.0, chapter 6.1.1.4
     */
    MIB_J_S_INT_KEY,
    /*!
     * Join session encryption key
     *
     * LoRaWAN Specification V1.1.0, chapter 6.1.1.4
     */
    MIB_J_S_ENC_KEY,
    /*!
     * Forwarding Network session integrity key
     *
     * LoRaWAN Specification V1.1.0, chapter 6.1.2.2
     */
    MIB_F_NWK_S_INT_KEY,
    /*!
     * Serving Network session integrity key
     *
     * LoRaWAN Specification V1.1.0, chapter 6.1.2.3
     */
    MIB_S_NWK_S_INT_KEY,
    /*!
     * Network session encryption key
     *
     * LoRaWAN Specification V1.1.0, chapter 6.1.2.4
     */
    MIB_NWK_S_ENC_KEY,
    /*!
     * Application session key
     *
     * LoRaWAN Specification V1.1.0, chapter 6.1.1.3
     */
    MIB_APP_S_KEY,
    /*!
     * Multicast key encryption key
     *
     * LoRaWAN - Secure element specification v1
     */
    MIB_MC_KE_KEY,
    /*!
     * Multicast root key index 0
     *
     * LoRaWAN - Secure element specification v1
     */
    MIB_MC_KEY_0,
    /*!
     * Multicast Application session key index 0
     *
     * LoRaWAN - Secure element specification v1
     */
    MIB_MC_APP_S_KEY_0,
    /*!
     * Multicast Network session key index 0
     *
     * LoRaWAN - Secure element specification v1
     */
    MIB_MC_NWK_S_KEY_0,
    /*!
     * Multicast root key index 1
     *
     * LoRaWAN - Secure element specification v1
     */
    MIB_MC_KEY_1,
    /*!
     * Multicast Application session key index 1
     *
     * LoRaWAN - Secure element specification v1
     */
    MIB_MC_APP_S_KEY_1,
    /*!
     * Multicast Network session key index 1
     *
     * LoRaWAN - Secure element specification v1
     */
    MIB_MC_NWK_S_KEY_1,
    /*!
     * Multicast root key index 2
     *
     * LoRaWAN - Secure element specification v1
     */
    MIB_MC_KEY_2,
    /*!
     * Multicast Application session key index 2
     *
     * LoRaWAN - Secure element specification v1
     */
    MIB_MC_APP_S_KEY_2,
    /*!
     * Multicast Network session key index 2
     *
     * LoRaWAN - Secure element specification v1
     */
    MIB_MC_NWK_S_KEY_2,
    /*!
     * Multicast root key index 3
     *
     * LoRaWAN - Secure element specification v1
     */
    MIB_MC_KEY_3,
    /*!
     * Multicast Application session key index 3
     *
     * LoRaWAN - Secure element specification v1
     */
    MIB_MC_APP_S_KEY_3,
    /*!
     * Multicast Network session key index 3
     *
     * LoRaWAN - Secure element specification v1
     */
    MIB_MC_NWK_S_KEY_3,
    /*!
     * Set the network type to public or private
     *
     * LoRaWAN Regional Parameters V1.0.2rB
     *
     * [true: public network, false: private network]
     */
    MIB_PUBLIC_NETWORK,
    /*!
     * Communication channels. A get request will return a
     * pointer which references the first entry of the channel list. The
     * list is of size LORA_MAX_NB_CHANNELS
     *
     * LoRaWAN Regional Parameters V1.0.2rB
     */
    MIB_CHANNELS,
    /*!
     * Set receive window 2 channel
     *
     * LoRaWAN Specification V1.0.2, chapter 3.3.1
     */
    MIB_RX2_CHANNEL,
    /*!
     * Set receive window 2 channel
     *
     * LoRaWAN Specification V1.0.2, chapter 3.3.2
     */
    MIB_RX2_DEFAULT_CHANNEL,
    /*!
     * Set receive window C channel
     *
     * LoRaWAN Specification V1.0.2, chapter 3.3.1
     */
    MIB_RXC_CHANNEL,
    /*!
     * Set receive window C channel
     *
     * LoRaWAN Specification V1.0.2, chapter 3.3.2
     */
    MIB_RXC_DEFAULT_CHANNEL,
    /*!
     * LoRaWAN channels mask
     *
     * LoRaWAN Regional Parameters V1.0.2rB
     */
    MIB_CHANNELS_MASK,
    /*!
     * LoRaWAN default channels mask
     *
     * LoRaWAN Regional Parameters V1.0.2rB
     */
    MIB_CHANNELS_DEFAULT_MASK,
    /*!
     * Set the number of repetitions on a channel
     *
     * LoRaWAN Specification V1.0.2, chapter 5.2
     */
    MIB_CHANNELS_NB_TRANS,
    /*!
     * Maximum receive window duration in [ms]
     *
     * LoRaWAN Specification V1.0.2, chapter 3.3.3
     */
    MIB_MAX_RX_WINDOW_DURATION,
    /*!
     * Receive delay 1 in [ms]
     *
     * LoRaWAN Regional Parameters V1.0.2rB
     */
    MIB_RECEIVE_DELAY_1,
    /*!
     * Receive delay 2 in [ms]
     *
     * LoRaWAN Regional Parameters V1.0.2rB
     */
    MIB_RECEIVE_DELAY_2,
    /*!
     * Join accept delay 1 in [ms]
     *
     * LoRaWAN Regional Parameters V1.0.2rB
     */
    MIB_JOIN_ACCEPT_DELAY_1,
    /*!
     * Join accept delay 2 in [ms]
     *
     * LoRaWAN Regional Parameters V1.0.2rB
     */
    MIB_JOIN_ACCEPT_DELAY_2,
    /*!
     * Default Data rate of a channel
     *
     * LoRaWAN Regional Parameters V1.0.2rB
     *
     * The allowed ranges are region specific. Please refer to \ref DR_0 to \ref DR_15 for details.
     */
    MIB_CHANNELS_DEFAULT_DATARATE,
    /*!
     * Data rate of a channel
     *
     * LoRaWAN Regional Parameters V1.0.2rB
     *
     * The allowed ranges are region specific. Please refer to \ref DR_0 to \ref DR_15 for details.
     */
    MIB_CHANNELS_DATARATE,
    /*!
     * Transmission power of a channel
     *
     * LoRaWAN Regional Parameters V1.0.2rB
     *
     * The allowed ranges are region specific. Please refer to \ref TX_POWER_0 to \ref TX_POWER_15 for details.
     */
    MIB_CHANNELS_TX_POWER,
    /*!
     * Transmission power of a channel
     *
     * LoRaWAN Regional Parameters V1.0.2rB
     *
     * The allowed ranges are region specific. Please refer to \ref TX_POWER_0 to \ref TX_POWER_15 for details.
     */
    MIB_CHANNELS_DEFAULT_TX_POWER,
    /*!
     * System overall timing error in milliseconds.
     * [-SystemMaxRxError : +SystemMaxRxError]
     * Default: +/-10 ms
     */
    MIB_SYSTEM_MAX_RX_ERROR,
    /*!
     * Minimum required number of symbols to detect an Rx frame
     * Default: 6 symbols
     */
    MIB_MIN_RX_SYMBOLS,
    /*!
     * Antenna gain of the node. Default value is region specific.
     * The antenna gain is used to calculate the TX power of the node.
     * The formula is:
     * radioTxPower = ( int8_t )floor( maxEirp - antennaGain )
     *
     * \remark The antenna gain value is referenced to the isotropic antenna.
     *         The value is in dBi.
     *         MIB_ANTENNA_GAIN[dBi] = measuredAntennaGain[dBd] + 2.15
     */
    MIB_ANTENNA_GAIN,
    /*!
     * Default antenna gain of the node. Default value is region specific.
     * The antenna gain is used to calculate the TX power of the node.
     * The formula is:
     * radioTxPower = ( int8_t )floor( maxEirp - antennaGain )
     *
     * \remark The antenna gain value is referenced to the isotropic antenna.
     *         The value is in dBi.
     *         MIB_DEFAULT_ANTENNA_GAIN[dBi] = measuredAntennaGain[dBd] + 2.15
     */
    MIB_DEFAULT_ANTENNA_GAIN,
    /*!
     * Structure holding pointers to internal contexts and its size
     */
    MIB_NVM_CTXS,
    /*!
     * LoRaWAN MAC layer operating version when activated by ABP.
     */
    MIB_ABP_LORAWAN_VERSION,
    /*!
     * LoRaWAN MAC and regional parameter version.
     */
    MIB_LORAWAN_VERSION,
    /*!
     * Beacon interval in ms
     */
    MIB_BEACON_INTERVAL,
    /*!
     * Beacon reserved time in ms
     */
    MIB_BEACON_RESERVED,
    /*!
     * Beacon guard time in ms
     */
    MIB_BEACON_GUARD,
    /*!
     * Beacon window time in ms
     */
    MIB_BEACON_WINDOW,
    /*!
     * Beacon window time in number of slots
     */
    MIB_BEACON_WINDOW_SLOTS,
    /*!
     * Ping slot length time in ms
     */
    MIB_PING_SLOT_WINDOW,
    /*!
     * Default symbol timeout for beacons and ping slot windows
     */
    MIB_BEACON_SYMBOL_TO_DEFAULT,
    /*!
     * Maximum symbol timeout for beacons
     */
    MIB_BEACON_SYMBOL_TO_EXPANSION_MAX,
    /*!
     * Maximum symbol timeout for ping slots
     */
    MIB_PING_SLOT_SYMBOL_TO_EXPANSION_MAX,
    /*!
     * Symbol expansion value for beacon windows in case of beacon
     * loss in symbols
     */
    MIB_BEACON_SYMBOL_TO_EXPANSION_FACTOR,
    /*!
     * Symbol expansion value for ping slot windows in case of beacon
     * loss in symbols
     */
    MIB_PING_SLOT_SYMBOL_TO_EXPANSION_FACTOR,
    /*!
     * Maximum allowed beacon less time in ms
     */
    MIB_MAX_BEACON_LESS_PERIOD,
    /*!
     * Ping slot data rate
     *
     * LoRaWAN Regional Parameters V1.0.2rB
     *
     * The allowed ranges are region specific. Please refer to \ref DR_0 to \ref DR_15 for details.
     */
     MIB_PING_SLOT_DATARATE,
}Mib_t;

/*!
 * LoRaMAC MIB parameters
 */
typedef union uMibParam
{
    /*!
     * LoRaWAN device class
     *
     * Related MIB type: \ref MIB_DEVICE_CLASS
     */
    DeviceClass_t Class;
    /*!
     * LoRaWAN Network End-Device Activation ( ACTIVATION_TYPE_NONE, ACTIVATION_TYPE_ABP or OTTA )
     *
     * Related MIB type: \ref MIB_NETWORK_ACTIVATION
     */
    ActivationType_t NetworkActivation;
    /*!
     * LoRaWAN device EUI
     *
     * Related MIB type: \ref MIB_DEV_EUI
     */
    uint8_t* DevEui;
    /*!
     * LoRaWAN Join server EUI
     *
     * Related MIB type: \ref MIB_JOIN_EUI
     */
    uint8_t* JoinEui;
    /*!
     * Secure-element pin
     *
     * Related MIB type: \ref MIB_SE_PIN
     */
    uint8_t* SePin;
    /*!
     * Activation state of ADR
     *
     * Related MIB type: \ref MIB_ADR
     */
    bool AdrEnable;
    /*!
     * Network identifier
     *
     * Related MIB type: \ref MIB_NET_ID
     */
    uint32_t NetID;
    /*!
     * End-device address
     *
     * Related MIB type: \ref MIB_DEV_ADDR
     */
    uint32_t DevAddr;
    /*!
     * Application root key
     *
     * Related MIB type: \ref MIB_APP_KEY
     */
    uint8_t* AppKey;
    /*!
     * Network root key
     *
     * Related MIB type: \ref MIB_NWK_KEY
     */
    uint8_t* NwkKey;
    /*!
     * Join session integrity key
     *
     * Related MIB type: \ref MIB_J_S_INT_KEY
     */
    uint8_t* JSIntKey;
    /*!
     * Join session encryption key
     *
     * Related MIB type: \ref MIB_J_S_ENC_KEY
     */
    uint8_t* JSEncKey;
    /*!
     * Forwarding Network session integrity key
     *
     * Related MIB type: \ref MIB_F_NWK_S_INT_KEY
     */
    uint8_t* FNwkSIntKey;
    /*!
     * Serving Network session integrity key
     *
     * Related MIB type: \ref MIB_S_NWK_S_INT_KEY
     */
    uint8_t* SNwkSIntKey;
    /*!
     * Network session encryption key
     *
     * Related MIB type: \ref MIB_NWK_S_ENC_KEY
     */
    uint8_t* NwkSEncKey;
    /*!
     * Application session key
     *
     * Related MIB type: \ref MIB_APP_S_KEY
     */
    uint8_t* AppSKey;
    /*!
     * Multicast key encryption key
     *
     * Related MIB type: \ref MIB_MC_KE_KEY
     */
    uint8_t* McKEKey;
    /*!
     * Multicast root key index 0
     *
     * Related MIB type: \ref MIB_MC_KEY_0
     */
    uint8_t* McKey0;
    /*!
     * Multicast Application session key index 0
     *
     * Related MIB type: \ref MIB_MC_APP_S_KEY_0
     */
    uint8_t* McAppSKey0;
    /*!
     * Multicast Network session key index 0
     *
     * Related MIB type: \ref MIB_MC_NWK_S_KEY_0
     */
    uint8_t* McNwkSKey0;
    /*!
     * Multicast root key index 0
     *
     * Related MIB type: \ref MIB_MC_KEY_0
     */
    uint8_t* McKey1;
    /*!
     * Multicast Application session key index 1
     *
     * Related MIB type: \ref MIB_MC_APP_S_KEY_1
     */
    uint8_t* McAppSKey1;
    /*!
     * Multicast Network session key index 1
     *
     * Related MIB type: \ref MIB_MC_NWK_S_KEY_1
     */
    uint8_t* McNwkSKey1;
    /*!
     * Multicast root key index 2
     *
     * Related MIB type: \ref MIB_MC_KEY_2
     */
    uint8_t* McKey2;
    /*!
     * Multicast Application session key index 2
     *
     * Related MIB type: \ref MIB_MC_APP_S_KEY_2
     */
    uint8_t* McAppSKey2;
    /*!
     * Multicast Network session key index 2
     *
     * Related MIB type: \ref MIB_MC_NWK_S_KEY_2
     */
    uint8_t* McNwkSKey2;
    /*!
     * Multicast root key index 2
     *
     * Related MIB type: \ref MIB_MC_KEY_2
     */
    uint8_t* McKey3;
    /*!
     * Multicast Application session key index 2
     *
     * Related MIB type: \ref MIB_MC_APP_S_KEY_2
     */
    uint8_t* McAppSKey3;
    /*!
     * Multicast Network session key index 2
     *
     * Related MIB type: \ref MIB_MC_NWK_S_KEY_2
     */
    uint8_t* McNwkSKey3;
    /*!
     * Enable or disable a public network
     *
     * Related MIB type: \ref MIB_PUBLIC_NETWORK
     */
    bool EnablePublicNetwork;
    /*!
     * LoRaWAN Channel
     *
     * Related MIB type: \ref MIB_CHANNELS
     */
    ChannelParams_t* ChannelList;
    /*!
     * Channel for the receive window 2
     *
     * Related MIB type: \ref MIB_RX2_CHANNEL
     */
    RxChannelParams_t Rx2Channel;
    /*!
     * Channel for the receive window 2
     *
     * Related MIB type: \ref MIB_RX2_DEFAULT_CHANNEL
     */
    RxChannelParams_t Rx2DefaultChannel;
    /*!
     * Channel for the receive window C
     *
     * Related MIB type: \ref MIB_RXC_CHANNEL
     */
    RxChannelParams_t RxCChannel;
    /*!
     * Channel for the receive window C
     *
     * Related MIB type: \ref MIB_RXC_DEFAULT_CHANNEL
     */
    RxChannelParams_t RxCDefaultChannel;
    /*!
     * Channel mask
     *
     * Related MIB type: \ref MIB_CHANNELS_MASK
     */
    uint16_t* ChannelsMask;
    /*!
     * Default channel mask
     *
     * Related MIB type: \ref MIB_CHANNELS_DEFAULT_MASK
     */
    uint16_t* ChannelsDefaultMask;
    /*!
     * Number of frame repetitions
     *
     * Related MIB type: \ref MIB_CHANNELS_NB_TRANS
     */
    uint8_t ChannelsNbTrans;
    /*!
     * Maximum receive window duration
     *
     * Related MIB type: \ref MIB_MAX_RX_WINDOW_DURATION
     */
    uint32_t MaxRxWindow;
    /*!
     * Receive delay 1
     *
     * Related MIB type: \ref MIB_RECEIVE_DELAY_1
     */
    uint32_t ReceiveDelay1;
    /*!
     * Receive delay 2
     *
     * Related MIB type: \ref MIB_RECEIVE_DELAY_2
     */
    uint32_t ReceiveDelay2;
    /*!
     * Join accept delay 1
     *
     * Related MIB type: \ref MIB_JOIN_ACCEPT_DELAY_1
     */
    uint32_t JoinAcceptDelay1;
    /*!
     * Join accept delay 2
     *
     * Related MIB type: \ref MIB_JOIN_ACCEPT_DELAY_2
     */
    uint32_t JoinAcceptDelay2;
    /*!
     * Channels data rate
     *
     * Related MIB type: \ref MIB_CHANNELS_DEFAULT_DATARATE
     */
    int8_t ChannelsDefaultDatarate;
    /*!
     * Channels data rate
     *
     * Related MIB type: \ref MIB_CHANNELS_DATARATE
     */
    int8_t ChannelsDatarate;
    /*!
     * Channels TX power
     *
     * Related MIB type: \ref MIB_CHANNELS_DEFAULT_TX_POWER
     */
    int8_t ChannelsDefaultTxPower;
    /*!
     * Channels TX power
     *
     * Related MIB type: \ref MIB_CHANNELS_TX_POWER
     */
    int8_t ChannelsTxPower;
    /*!
     * Multicast channels
     *
     * Related MIB type: \ref MIB_MULTICAST_CHANNEL
     */
    McChannelParams_t MulticastChannel;
    /*!
     * System overall timing error in milliseconds.
     *
     * Related MIB type: \ref MIB_SYSTEM_MAX_RX_ERROR
     */
    uint32_t SystemMaxRxError;
    /*!
     * Minimum required number of symbols to detect an Rx frame
     *
     * Related MIB type: \ref MIB_MIN_RX_SYMBOLS
     */
    uint8_t MinRxSymbols;
    /*!
     * Antenna gain
     *
     * Related MIB type: \ref MIB_ANTENNA_GAIN
     */
    float AntennaGain;
    /*!
     * Default antenna gain
     *
     * Related MIB type: \ref MIB_DEFAULT_ANTENNA_GAIN
     */
    float DefaultAntennaGain;
    /*!
     * Structure holding pointers to internal non-volatile contexts and its lengths.
     *
     * Related MIB type: \ref MIB_NVM_CTXS
     */
    LoRaMacCtxs_t* Contexts;
    /*
     * LoRaWAN MAC layer operating version when activated by ABP.
     *
     * Related MIB type: \ref MIB_ABP_LORAWAN_VERSION
     */
    Version_t AbpLrWanVersion;
    /*
     * LoRaWAN MAC regional parameter version.
     *
     * Related MIB type: \ref MIB_LORAWAN_VERSION
     */
    struct sLrWanVersion
    {
        Version_t LoRaWan;
        Version_t LoRaWanRegion;
    }LrWanVersion;
    /*!
     * Beacon interval in ms
     *
     * Related MIB type: \ref MIB_BEACON_INTERVAL
     */
    uint32_t BeaconInterval;
    /*!
     * Beacon reserved time in ms
     *
     * Related MIB type: \ref MIB_BEACON_RESERVED
     */
    uint32_t BeaconReserved;
    /*!
     * Beacon guard time in ms
     *
     * Related MIB type: \ref MIB_BEACON_GUARD
     */
    uint32_t BeaconGuard;
    /*!
     * Beacon window time in ms
     *
     * Related MIB type: \ref MIB_BEACON_WINDOW
     */
    uint32_t BeaconWindow;
    /*!
     * Beacon window time in number of slots
     *
     * Related MIB type: \ref MIB_BEACON_WINDOW_SLOTS
     */
    uint32_t BeaconWindowSlots;
    /*!
     * Ping slot length time in ms
     *
     * Related MIB type: \ref MIB_PING_SLOT_WINDOW
     */
    uint32_t PingSlotWindow;
    /*!
     * Default symbol timeout for beacons and ping slot windows
     *
     * Related MIB type: \ref MIB_BEACON_SYMBOL_TO_DEFAULT
     */
    uint32_t BeaconSymbolToDefault;
    /*!
     * Maximum symbol timeout for beacons
     *
     * Related MIB type: \ref MIB_BEACON_SYMBOL_TO_EXPANSION_MAX
     */
    uint32_t BeaconSymbolToExpansionMax;
    /*!
     * Maximum symbol timeout for ping slots
     *
     * Related MIB type: \ref MIB_PING_SLOT_SYMBOL_TO_EXPANSION_MAX
     */
    uint32_t PingSlotSymbolToExpansionMax;
    /*!
     * Symbol expansion value for beacon windows in case of beacon
     * loss in symbols
     *
     * Related MIB type: \ref MIB_BEACON_SYMBOL_TO_EXPANSION_FACTOR
     */
    uint32_t BeaconSymbolToExpansionFactor;
    /*!
     * Symbol expansion value for ping slot windows in case of beacon
     * loss in symbols
     *
     * Related MIB type: \ref MIB_PING_SLOT_SYMBOL_TO_EXPANSION_FACTOR
     */
    uint32_t PingSlotSymbolToExpansionFactor;
    /*!
     * Maximum allowed beacon less time in ms
     *
     * Related MIB type: \ref MIB_MAX_BEACON_LESS_PERIOD
     */
    uint32_t MaxBeaconLessPeriod;
    /*!
     * Ping slots data rate
     *
     * Related MIB type: \ref MIB_PING_SLOT_DATARATE
     */
    int8_t PingSlotDatarate;
}MibParam_t;

/*!
 * LoRaMAC MIB-RequestConfirm structure
 */
typedef struct eMibRequestConfirm
{
    /*!
     * MIB-Request type
     */
    Mib_t Type;

    /*!
     * MLME-RequestConfirm parameters
     */
    MibParam_t Param;
}MibRequestConfirm_t;

/*!
 * LoRaMAC tx information
 */
typedef struct sLoRaMacTxInfo
{
    /*!
     * Size of the application data payload which can be transmitted.
     */
    uint8_t MaxPossibleApplicationDataSize;
    /*!
     * The current maximum possible payload size without MAC commands
     * which is dependent on the current datarate.
     */
    uint8_t CurrentPossiblePayloadSize;
}LoRaMacTxInfo_t;

/*!
 * LoRaMAC Status
 */
typedef enum eLoRaMacStatus
{
    /*!
     * Service started successfully
     */
    LORAMAC_STATUS_OK,
    /*!
     * Service not started - LoRaMAC is busy
     */
    LORAMAC_STATUS_BUSY,
    /*!
     * Service unknown
     */
    LORAMAC_STATUS_SERVICE_UNKNOWN,
    /*!
     * Service not started - invalid parameter
     */
    LORAMAC_STATUS_PARAMETER_INVALID,
    /*!
     * Service not started - invalid frequency
     */
    LORAMAC_STATUS_FREQUENCY_INVALID,
    /*!
     * Service not started - invalid datarate
     */
    LORAMAC_STATUS_DATARATE_INVALID,
    /*!
     * Service not started - invalid frequency and datarate
     */
    LORAMAC_STATUS_FREQ_AND_DR_INVALID,
    /*!
     * Service not started - the device is not in a LoRaWAN
     */
    LORAMAC_STATUS_NO_NETWORK_JOINED,
    /*!
     * Service not started - payload length error
     */
    LORAMAC_STATUS_LENGTH_ERROR,
    /*!
     * Service not started - the specified region is not supported
     * or not activated with preprocessor definitions.
     */
    LORAMAC_STATUS_REGION_NOT_SUPPORTED,
    /*!
     * The application data was not transmitted
     * because prioritized pending MAC commands had to be sent.
     */
    LORAMAC_STATUS_SKIPPED_APP_DATA,
    /*!
     * An MCPS or MLME request can return this status. In this case,
     * the MAC cannot send the frame, as the duty cycle limits all
     * available bands. When a request returns this value, the
     * variable "DutyCycleWaitTime" in "ReqReturn" of the input
     * parameters contains the remaining time to wait. If the
     * value is constant and does not change, the expected time
     * on air for this frame is exceeding the maximum permitted
     * time according to the duty cycle time period, defined
     * in Region.h, DUTY_CYCLE_TIME_PERIOD. By default this time
     * is 1 hour, and a band with 1% duty cycle is then allowed
     * to use an air time of 36 seconds.
     */
    LORAMAC_STATUS_DUTYCYCLE_RESTRICTED,
    /*!
     *
     */
    LORAMAC_STATUS_NO_CHANNEL_FOUND,
    /*!
     *
     */
    LORAMAC_STATUS_NO_FREE_CHANNEL_FOUND,
     /*!
      * ToDo
      */
    LORAMAC_STATUS_BUSY_BEACON_RESERVED_TIME,
     /*!
      * ToDo
      */
    LORAMAC_STATUS_BUSY_PING_SLOT_WINDOW_TIME,
     /*!
      * ToDo
      */
    LORAMAC_STATUS_BUSY_UPLINK_COLLISION,
    /*!
     * An error in the cryptographic module is occurred
     */
    LORAMAC_STATUS_CRYPTO_ERROR,
    /*!
     * An error in the frame counter handler module is occurred
     */
    LORAMAC_STATUS_FCNT_HANDLER_ERROR,
    /*!
     * An error in the MAC command module is occurred
     */
    LORAMAC_STATUS_MAC_COMMAD_ERROR,
    /*!
     * An error in the Class B module is occurred
     */
    LORAMAC_STATUS_CLASS_B_ERROR,
    /*!
     * An error in the Confirm Queue module is occurred
     */
    LORAMAC_STATUS_CONFIRM_QUEUE_ERROR,
    /*!
     * The multicast group doesn't exist
     */
    LORAMAC_STATUS_MC_GROUP_UNDEFINED,
    /*!
     * Undefined error occurred
     */
    LORAMAC_STATUS_ERROR
}LoRaMacStatus_t;

/*!
 * LoRaMAC region enumeration
 */
typedef enum eLoRaMacRegion_t
{
    /*!
     * AS band on 923MHz
     */
    LORAMAC_REGION_AS923,
    /*!
     * Australian band on 915MHz
     */
    LORAMAC_REGION_AU915,
    /*!
     * Chinese band on 470MHz
     */
    LORAMAC_REGION_CN470,
    /*!
     * Chinese band on 779MHz
     */
    LORAMAC_REGION_CN779,
    /*!
     * European band on 433MHz
     */
    LORAMAC_REGION_EU433,
    /*!
     * European band on 868MHz
     */
    LORAMAC_REGION_EU868,
    /*!
     * South korean band on 920MHz
     */
    LORAMAC_REGION_KR920,
    /*!
     * India band on 865MHz
     */
    LORAMAC_REGION_IN865,
    /*!
     * North american band on 915MHz
     */
    LORAMAC_REGION_US915,
    /*!
     * Russia band on 864MHz
     */
    LORAMAC_REGION_RU864,
}LoRaMacRegion_t;

/*!
 * Enumeration of modules which have a context
 */
typedef enum LoRaMacNvmCtxModule_e
{
    /*!
     * Context for the MAC
     */
    LORAMAC_NVMCTXMODULE_MAC,
    /*!
     * Context for the regions
     */
    LORAMAC_NVMCTXMODULE_REGION,
    /*!
     * Context for the crypto module
     */
    LORAMAC_NVMCTXMODULE_CRYPTO,
    /*!
     * Context for the secure element
     */
    LORAMAC_NVMCTXMODULE_SECURE_ELEMENT,
    /*!
     * Context for the command queue
     */
    LORAMAC_NVMCTXMODULE_COMMANDS,
    /*!
     * Context for class b
     */
    LORAMAC_NVMCTXMODULE_CLASS_B,
    /*!
     * Context for the confirm queue
     */
    LORAMAC_NVMCTXMODULE_CONFIRM_QUEUE,
}LoRaMacNvmCtxModule_t;


/*!
 * LoRaMAC events structure
 * Used to notify upper layers of MAC events
 */
typedef struct sLoRaMacPrimitives
{
    /*!
     * \brief   MCPS-Confirm primitive
     *
     * \param   [OUT] MCPS-Confirm parameters
     */
    void ( *MacMcpsConfirm )( McpsConfirm_t* McpsConfirm );
    /*!
     * \brief   MCPS-Indication primitive
     *
     * \param   [OUT] MCPS-Indication parameters
     */
    void ( *MacMcpsIndication )( McpsIndication_t* McpsIndication );
    /*!
     * \brief   MLME-Confirm primitive
     *
     * \param   [OUT] MLME-Confirm parameters
     */
    void ( *MacMlmeConfirm )( MlmeConfirm_t* MlmeConfirm );
    /*!
     * \brief   MLME-Indication primitive
     *
     * \param   [OUT] MLME-Indication parameters
     */
    void ( *MacMlmeIndication )( MlmeIndication_t* MlmeIndication );
}LoRaMacPrimitives_t;

/*!
 * LoRaMAC callback structure
 */
typedef struct sLoRaMacCallback
{
    /*!
     * \brief   Measures the battery level
     *
     * \retval  Battery level [0: node is connected to an external
     *          power source, 1..254: battery level, where 1 is the minimum
     *          and 254 is the maximum value, 255: the node was not able
     *          to measure the battery level]
     */
    uint8_t ( *GetBatteryLevel )( void );
    /*!
     * \brief   Measures the temperature level
     *
     * \retval  Temperature level
     */
    float ( *GetTemperatureLevel )( void );
    /*!
     * \brief   Will be called when an attribute has changed in one of the context.
     *
     * \param   Context that changed
     */
    void ( *NvmContextChange )( LoRaMacNvmCtxModule_t module );
    /*!
     *\brief    Will be called each time a Radio IRQ is handled by the MAC
     *          layer.
     *
     *\warning  Runs in a IRQ context. Should only change variables state.
     */
    void ( *MacProcessNotify )( void );
}LoRaMacCallback_t;


/*!
 * LoRaMAC Max EIRP (dBm) table
 */
static const uint8_t LoRaMacMaxEirpTable[] = { 8, 10, 12, 13, 14, 16, 18, 20, 21, 24, 26, 27, 29, 30, 33, 36 };

/*!
 * \brief   LoRaMAC layer initialization
 *
 * \details In addition to the initialization of the LoRaMAC layer, this
 *          function initializes the callback primitives of the MCPS and
 *          MLME services. Every data field of \ref LoRaMacPrimitives_t must be
 *          set to a valid callback function.
 *
 * \param   [IN] primitives - Pointer to a structure defining the LoRaMAC
 *                            event functions. Refer to \ref LoRaMacPrimitives_t.
 *
 * \param   [IN] callbacks  - Pointer to a structure defining the LoRaMAC
 *                            callback functions. Refer to \ref LoRaMacCallback_t.
 *
 * \param   [IN] region     - The region to start.
 *
 * \retval  LoRaMacStatus_t Status of the operation. Possible returns are:
 *          returns are:
 *          \ref LORAMAC_STATUS_OK,
 *          \ref LORAMAC_STATUS_PARAMETER_INVALID,
 *          \ref LORAMAC_STATUS_REGION_NOT_SUPPORTED.
 */
LoRaMacStatus_t LoRaMacInitialization( LoRaMacPrimitives_t* primitives, LoRaMacCallback_t* callbacks, LoRaMacRegion_t region );

/*!
 * \brief   Starts LoRaMAC layer
 *
 * \retval  LoRaMacStatus_t Status of the operation. Possible returns are:
 *          returns are:
 *          \ref LORAMAC_STATUS_OK,
 */
LoRaMacStatus_t LoRaMacStart( void );

/*!
 * \brief   Stops LoRaMAC layer
 *
 * \retval  LoRaMacStatus_t Status of the operation. Possible returns are:
 *          returns are:
 *          \ref LORAMAC_STATUS_OK,
 */
LoRaMacStatus_t LoRaMacStop( void );

/*!
 * \brief Returns a value indicating if the MAC layer is busy or not.
 *
 * \retval isBusy Mac layer is busy.
 */
bool LoRaMacIsBusy( void );

/*!
 * Processes the LoRaMac events.
 *
 * \remark This function must be called in the main loop.
 */
void LoRaMacProcess( void );

/*!
 * \brief   Queries the LoRaMAC if it is possible to send the next frame with
 *          a given application data payload size. The LoRaMAC takes scheduled
 *          MAC commands into account and reports, when the frame can be send or not.
 *
 * \param   [IN] size - Size of application data payload to be send next
 *
 * \param   [OUT] txInfo - The structure \ref LoRaMacTxInfo_t contains
 *                         information about the actual maximum payload possible
 *                         ( according to the configured datarate or the next
 *                         datarate according to ADR ), and the maximum frame
 *                         size, taking the scheduled MAC commands into account.
 *
 * \retval  LoRaMacStatus_t Status of the operation. When the parameters are
 *          not valid, the function returns \ref LORAMAC_STATUS_PARAMETER_INVALID.
 *          In case of a length error caused by the application data payload in combination
 *          with the MAC commands, the function returns \ref LORAMAC_STATUS_LENGTH_ERROR.
 *          In this case its recommended to send a frame without application data to flush
 *          the MAC commands. Otherwise the LoRaMAC will prioritize the MAC commands and
 *          if needed it will skip the application data. Please note that if MAC commands do
 *          not fit at all into the payload size on the related datarate, the LoRaMAC will
 *          automatically clip the MAC commands.
 *          In case the query is valid, and the LoRaMAC is able to send the frame,
 *          the function returns \ref LORAMAC_STATUS_OK.
 */
LoRaMacStatus_t LoRaMacQueryTxPossible( uint8_t size, LoRaMacTxInfo_t* txInfo );

/*!
 * \brief   LoRaMAC channel add service
 *
 * \details Adds a new channel to the channel list and activates the id in
 *          the channel mask. Please note that this functionality is not available
 *          on all regions. Information about allowed ranges are available at the LoRaWAN Regional Parameters V1.0.2rB
 *
 * \param   [IN] id - Id of the channel.
 *
 * \param   [IN] params - Channel parameters to set.
 *
 * \retval  LoRaMacStatus_t Status of the operation. Possible returns are:
 *          \ref LORAMAC_STATUS_OK,
 *          \ref LORAMAC_STATUS_BUSY,
 *          \ref LORAMAC_STATUS_PARAMETER_INVALID.
 */
LoRaMacStatus_t LoRaMacChannelAdd( uint8_t id, ChannelParams_t params );

/*!
 * \brief   LoRaMAC channel remove service
 *
 * \details Deactivates the id in the channel mask.
 *
 * \param   [IN] id - Id of the channel.
 *
 * \retval  LoRaMacStatus_t Status of the operation. Possible returns are:
 *          \ref LORAMAC_STATUS_OK,
 *          \ref LORAMAC_STATUS_BUSY,
 *          \ref LORAMAC_STATUS_PARAMETER_INVALID.
 */
LoRaMacStatus_t LoRaMacChannelRemove( uint8_t id );

/*!
 * \brief   LoRaMAC multicast channel setup service
 *
 * \details Sets up a multicast channel.
 *
 * \param   [IN] channel - Multicast channel to set.
 *
 * \retval  LoRaMacStatus_t Status of the operation. Possible returns are:
 *          \ref LORAMAC_STATUS_OK,
 *          \ref LORAMAC_STATUS_BUSY,
 *          \ref LORAMAC_STATUS_PARAMETER_INVALID,
 *          \ref LORAMAC_STATUS_MC_GROUP_UNDEFINED.
 */
LoRaMacStatus_t LoRaMacMcChannelSetup( McChannelParams_t *channel );

/*!
 * \brief   LoRaMAC multicast channel removal service
 *
 * \details Removes/Disables a multicast channel.
 *
 * \param   [IN] groupID - Multicast channel ID to be removed/disabled
 *
 * \retval  LoRaMacStatus_t Status of the operation. Possible returns are:
 *          \ref LORAMAC_STATUS_OK,
 *          \ref LORAMAC_STATUS_BUSY,
 *          \ref LORAMAC_STATUS_MC_GROUP_UNDEFINED.
 */
LoRaMacStatus_t LoRaMacMcChannelDelete( AddressIdentifier_t groupID );

/*!
 * \brief   LoRaMAC multicast channel get groupId from MC address.
 *
 * \param   [IN]  mcAddress - Multicast address to be checked
 *
 * \retval  groupID           Multicast channel ID associated to the address.
 *                            Returns 0xFF if the address isn't found.
 */
uint8_t LoRaMacMcChannelGetGroupId( uint32_t mcAddress );

/*!
 * \brief   LoRaMAC multicast channel Rx parameters setup service
 *
 * \details Sets up a multicast channel reception parameters.
 *
 * \param   [IN]  groupID  - Multicast channel ID
 * \param   [IN]  rxParams - Reception parameters
 * \param   [OUT] status   - Status mask [UNDEF_ID | FREQ_ERR | DR_ERR | GROUP_ID]
 *
 * \retval  LoRaMacStatus_t Status of the operation. Possible returns are:
 *          \ref LORAMAC_STATUS_OK,
 *          \ref LORAMAC_STATUS_BUSY,
 *          \ref LORAMAC_STATUS_PARAMETER_INVALID,
 *          \ref LORAMAC_STATUS_MC_GROUP_UNDEFINED.
 */
LoRaMacStatus_t LoRaMacMcChannelSetupRxParams( AddressIdentifier_t groupID, McRxParams_t *rxParams, uint8_t *status );

/*!
 * \brief   LoRaMAC MIB-Get
 *
 * \details The mac information base service to get attributes of the LoRaMac
 *          layer.
 *
 *          The following code-snippet shows how to use the API to get the
 *          parameter AdrEnable, defined by the enumeration type
 *          \ref MIB_ADR.
 * \code
 * MibRequestConfirm_t mibReq;
 * mibReq.Type = MIB_ADR;
 *
 * if( LoRaMacMibGetRequestConfirm( &mibReq ) == LORAMAC_STATUS_OK )
 * {
 *   // LoRaMAC updated the parameter mibParam.AdrEnable
 * }
 * \endcode
 *
 * \param   [IN] mibRequest - MIB-GET-Request to perform. Refer to \ref MibRequestConfirm_t.
 *
 * \retval  LoRaMacStatus_t Status of the operation. Possible returns are:
 *          \ref LORAMAC_STATUS_OK,
 *          \ref LORAMAC_STATUS_SERVICE_UNKNOWN,
 *          \ref LORAMAC_STATUS_PARAMETER_INVALID.
 */
LoRaMacStatus_t LoRaMacMibGetRequestConfirm( MibRequestConfirm_t* mibGet );

/*!
 * \brief   LoRaMAC MIB-Set
 *
 * \details The mac information base service to set attributes of the LoRaMac
 *          layer.
 *
 *          The following code-snippet shows how to use the API to set the
 *          parameter AdrEnable, defined by the enumeration type
 *          \ref MIB_ADR.
 *
 * \code
 * MibRequestConfirm_t mibReq;
 * mibReq.Type = MIB_ADR;
 * mibReq.Param.AdrEnable = true;
 *
 * if( LoRaMacMibGetRequestConfirm( &mibReq ) == LORAMAC_STATUS_OK )
 * {
 *   // LoRaMAC updated the parameter
 * }
 * \endcode
 *
 * \param   [IN] mibRequest - MIB-SET-Request to perform. Refer to \ref MibRequestConfirm_t.
 *
 * \retval  LoRaMacStatus_t Status of the operation. Possible returns are:
 *          \ref LORAMAC_STATUS_OK,
 *          \ref LORAMAC_STATUS_BUSY,
 *          \ref LORAMAC_STATUS_SERVICE_UNKNOWN,
 *          \ref LORAMAC_STATUS_PARAMETER_INVALID.
 */
LoRaMacStatus_t LoRaMacMibSetRequestConfirm( MibRequestConfirm_t* mibSet );

/*!
 * \brief   LoRaMAC MLME-Request
 *
 * \details The Mac layer management entity handles management services. The
 *          following code-snippet shows how to use the API to perform a
 *          network join request. Please note that for a join request, the
 *          DevEUI and the JoinEUI must be set previously via the MIB. Please
 *          also refer to the sample implementations.
 *
 * \code
 *
 * MlmeReq_t mlmeReq;
 * mlmeReq.Type = MLME_JOIN;
 * mlmeReq.Req.Join.Datarate = LORAWAN_DEFAULT_DATARATE;
 *
 * if( LoRaMacMlmeRequest( &mlmeReq ) == LORAMAC_STATUS_OK )
 * {
 *   // Service started successfully. Waiting for the Mlme-Confirm event
 * }
 * \endcode
 *
 * \param   [IN] mlmeRequest - MLME-Request to perform. Refer to \ref MlmeReq_t.
 *
 * \retval  LoRaMacStatus_t Status of the operation. Possible returns are:
 *          \ref LORAMAC_STATUS_OK,
 *          \ref LORAMAC_STATUS_BUSY,
 *          \ref LORAMAC_STATUS_SERVICE_UNKNOWN,
 *          \ref LORAMAC_STATUS_PARAMETER_INVALID,
 *          \ref LORAMAC_STATUS_NO_NETWORK_JOINED,
 *          \ref LORAMAC_STATUS_LENGTH_ERROR,
 */
LoRaMacStatus_t LoRaMacMlmeRequest( MlmeReq_t* mlmeRequest );

/*!
 * \brief   LoRaMAC MCPS-Request
 *
 * \details The Mac Common Part Sublayer handles data services. The following
 *          code-snippet shows how to use the API to send an unconfirmed
 *          LoRaMAC frame.
 *
 * \code
 * uint8_t myBuffer[] = { 1, 2, 3 };
 *
 * McpsReq_t mcpsReq;
 * mcpsReq.Type = MCPS_UNCONFIRMED;
 * mcpsReq.Req.Unconfirmed.fPort = 1;
 * mcpsReq.Req.Unconfirmed.fBuffer = myBuffer;
 * mcpsReq.Req.Unconfirmed.fBufferSize = sizeof( myBuffer );
 *
 * if( LoRaMacMcpsRequest( &mcpsReq ) == LORAMAC_STATUS_OK )
 * {
 *   // Service started successfully. Waiting for the MCPS-Confirm event
 * }
 * \endcode
 *
 * \param   [IN] mcpsRequest - MCPS-Request to perform. Refer to \ref McpsReq_t.
 *
 * \retval  LoRaMacStatus_t Status of the operation. Possible returns are:
 *          \ref LORAMAC_STATUS_OK,
 *          \ref LORAMAC_STATUS_BUSY,
 *          \ref LORAMAC_STATUS_SERVICE_UNKNOWN,
 *          \ref LORAMAC_STATUS_PARAMETER_INVALID,
 *          \ref LORAMAC_STATUS_NO_NETWORK_JOINED,
 *          \ref LORAMAC_STATUS_LENGTH_ERROR,
 */
LoRaMacStatus_t LoRaMacMcpsRequest( McpsReq_t* mcpsRequest );

/*!
 * \brief   LoRaMAC deinitialization
 *
 * \details This function stops the timers, re-initializes MAC & regional parameters to default
 *          and sets radio into sleep state.
 *
 * \retval  LoRaMacStatus_t Status of the operation. Possible returns are:
 *          \ref LORAMAC_STATUS_OK,
 *          \ref LORAMAC_STATUS_BUSY
 */
LoRaMacStatus_t LoRaMacDeInitialization( void );

/*!
 * Automatically add the Region.h file at the end of LoRaMac.h file.
 * This is required because Region.h uses definitions from LoRaMac.h
 */
#include "region/Region.h"

/*! \} defgroup LORAMAC */

#ifdef __cplusplus
}
#endif

#endif // __LORAMAC_H__
