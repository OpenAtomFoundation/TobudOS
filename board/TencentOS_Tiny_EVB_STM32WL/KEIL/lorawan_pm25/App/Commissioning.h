/**
  **********************************************************************************************************************************************************
  * @file    commissioning.h
  * @author  MCD Application Team
  * @brief   End device commissioning parameters
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LORA_COMMISSIONING_H__
#define __LORA_COMMISSIONING_H__

#ifdef __cplusplus
extern "C" {
#endif
/*!
 ******************************************************************************
 ********************************** WARNING ***********************************
 ******************************************************************************
  The crypto-element implementation supports both 1.0.x and 1.1.x LoRaWAN
  versions of the specification.
  Thus it has been decided to use the 1.1.x keys and EUI name definitions.
  The below table shows the names equivalence between versions:
               +-------------------+-------------------------+
               |       1.0.x       |          1.1.x          |
               +===================+=========================+
               | LORAWAN_DEVICE_EUI| LORAWAN_DEVICE_EUI      |
               +-------------------+-------------------------+
               | LORAWAN_APP_EUI   | LORAWAN_JOIN_EUI        |
               +-------------------+-------------------------+
               | N/A               | LORAWAN_APP_KEY         |
               +-------------------+-------------------------+
               | LORAWAN_APP_KEY   | LORAWAN_NWK_KEY         |
               +-------------------+-------------------------+
               | LORAWAN_NWK_S_KEY | LORAWAN_F_NWK_S_INT_KEY |
               +-------------------+-------------------------+
               | LORAWAN_NWK_S_KEY | LORAWAN_S_NWK_S_INT_KEY |
               +-------------------+-------------------------+
               | LORAWAN_NWK_S_KEY | LORAWAN_NWK_S_ENC_KEY   |
               +-------------------+-------------------------+
               | LORAWAN_APP_S_KEY | LORAWAN_APP_S_KEY       |
               +-------------------+-------------------------+
 ******************************************************************************
 ******************************************************************************
 ******************************************************************************
 */
/*!
 * Indicates if the end-device is to be connected to a private or public network
 */
#define LORAWAN_PUBLIC_NETWORK					true

/*root key*/
#define LORAWAN_APP_KEY							{ 0x12, 0x34, 0x56, 0x78, 0x91, 0x23, 0x45, 0x67, 0x89, 0x12, 0x34, 0x56, 0x78, 0x91, 0x23, 0x45 } 

/*for ABP================================================================================*/
#define STATIC_DEVICE_ADDRESS					1     //if 0 , using a ramdom address

#define LORAWAN_NETWORK_ID						( uint32_t )0

#define LORAWAN_DEVICE_ADDRESS					( uint32_t )0x0100000a
#define LORAWAN_NWK_KEY							{ 0x12, 0x34, 0x56, 0x78, 0x91, 0x23, 0x45, 0x67, 0x89, 0x12, 0x34, 0x56, 0x78, 0x91, 0x23, 0x45 } 


/*for 1.1.x*/
#define LORAWAN_F_NWK_S_INT_KEY					{ 0x12, 0x34, 0x56, 0x78, 0x91, 0x23, 0x45, 0x67, 0x89, 0x12, 0x34, 0x56, 0x78, 0x91, 0x23, 0x45 } 
#define LORAWAN_S_NWK_S_INT_KEY					{ 0x12, 0x34, 0x56, 0x78, 0x91, 0x23, 0x45, 0x67, 0x89, 0x12, 0x34, 0x56, 0x78, 0x91, 0x23, 0x45 } 
#define LORAWAN_NWK_S_ENC_KEY					{ 0x12, 0x34, 0x56, 0x78, 0x91, 0x23, 0x45, 0x67, 0x89, 0x12, 0x34, 0x56, 0x78, 0x91, 0x23, 0x45 } 
#define LORAWAN_APP_S_KEY						{ 0x12, 0x34, 0x56, 0x78, 0x91, 0x23, 0x45, 0x67, 0x89, 0x12, 0x34, 0x56, 0x78, 0x91, 0x23, 0x45 } 
   
/*for OTAA================================================================================*/
#define STATIC_DEVICE_EUI						1	//if 0 , using a ramdom EUI

#define IEEE_OUI								0x01, 0x01, 0x01
#define LORAWAN_DEVICE_EUI						{ 0x8c, 0xf9, 0x57, 0x20, 0x00, 0x00, 0x4d, 0xf8}
#define LORAWAN_JOIN_EUI	/*APP_EUI*/			{ 0x8c, 0xf9, 0x57, 0x20, 0x00, 0x00, 0x4d, 0xf8}




#ifdef __cplusplus
}
#endif

#endif /* __LORA_COMMISSIONING_H__ */
