/*!
 * \file      LoRaMacAdr.c
 *
 * \brief     LoRa MAC ADR implementation
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

#include "region/Region.h"
#include "LoRaMacAdr.h"

static bool CalcNextV10X( CalcNextAdrParams_t* adrNext, int8_t* drOut, int8_t* txPowOut, uint32_t* adrAckCounter )
{
    bool adrAckReq = false;
    int8_t datarate = adrNext->Datarate;
    int8_t txPower = adrNext->TxPower;
    int8_t minTxDatarate;
    GetPhyParams_t getPhy;
    PhyParam_t phyParam;

    // Report back the adr ack counter
    *adrAckCounter = adrNext->AdrAckCounter;

    if( adrNext->AdrEnabled == true )
    {
        // Query minimum TX Datarate
        getPhy.Attribute = PHY_MIN_TX_DR;
        getPhy.UplinkDwellTime = adrNext->UplinkDwellTime;
        phyParam = RegionGetPhyParam( adrNext->Region, &getPhy );
        minTxDatarate = phyParam.Value;
        datarate = MAX( datarate, minTxDatarate );

        if( datarate == minTxDatarate )
        {
            *adrAckCounter = 0;
            adrAckReq = false;
        }
        else
        {
            if( adrNext->AdrAckCounter >=  adrNext->AdrAckLimit )
            {
                adrAckReq = true;
            }
            else
            {
                adrAckReq = false;
            }
            if( adrNext->AdrAckCounter >= ( adrNext->AdrAckLimit + adrNext->AdrAckDelay ) )
            {
                // Set TX Power to maximum
                getPhy.Attribute = PHY_MAX_TX_POWER;
                phyParam = RegionGetPhyParam( adrNext->Region, &getPhy );
                txPower = phyParam.Value;

                if( ( adrNext->AdrAckCounter % adrNext->AdrAckDelay ) == 1 )
                {
                    // Decrease the datarate
                    getPhy.Attribute = PHY_NEXT_LOWER_TX_DR;
                    getPhy.Datarate = datarate;
                    getPhy.UplinkDwellTime = adrNext->UplinkDwellTime;
                    phyParam = RegionGetPhyParam( adrNext->Region, &getPhy );
                    datarate = phyParam.Value;

                    if( datarate == minTxDatarate )
                    {
                        // We must set adrAckReq to false as soon as we reach the lowest datarate
                        adrAckReq = false;
                        if( adrNext->UpdateChanMask == true )
                        {
                            InitDefaultsParams_t params;
                            params.Type = INIT_TYPE_RESTORE_DEFAULT_CHANNELS;
                            RegionInitDefaults( adrNext->Region, &params );
                        }
                    }
                }
            }
        }
    }

    *drOut = datarate;
    *txPowOut = txPower;
    return adrAckReq;
}

/*!
 * \brief Calculates the next datarate to set, when ADR is on or off.
 *
 * \param [IN] adrNext Pointer to the function parameters.
 *
 * \param [OUT] drOut The calculated datarate for the next TX.
 *
 * \param [OUT] txPowOut The TX power for the next TX.
 *
 * \param [OUT] adrAckCounter The calculated ADR acknowledgement counter.
 *
 * \retval Returns true, if an ADR request should be performed.
 */
bool LoRaMacAdrCalcNext( CalcNextAdrParams_t* adrNext, int8_t* drOut, int8_t* txPowOut, uint32_t* adrAckCounter )
{
    if( adrNext->Version.Fields.Minor == 0 )
    {
        return CalcNextV10X( adrNext, drOut, txPowOut, adrAckCounter );
    }
    return false;
}
