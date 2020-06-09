  @verbatim
  ******************************************************************************
  * @file    st_readme.txt 
  * @author  MCD Application Team
  * @brief   This file lists the main modification done by STMicroelectronics on
  *          LoRa for integration with STM32Cube solution.
  *          For more details on LoRa implementation on STM32Cube, please refer
  *          to UM2073 "STM32 LORA Expansion Package for STM32Cube "  
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
 ******************************************************************************
  @endverbatim

### V1.2.4/28-Nov-2019 ###
===============================
   + Implements LoRa Mac from Semtech/StackForce develop branch (31-Oct-2019 commits, version 4.4.2)
   
   + Patterns/basic and Patterns/modem directory structure change
   
   + update files license
   
   + certification obtained with LoRAWAN version v1.0.2 in the following regions:
     - EU868
     - IN865
     - KR920
     - AS923
     - US915

   
### V1.2.3/20-Sept-2019 ###
===============================
   + Implements LoRa Mac from Semtech/StackForce develop branch (17-Jun-2019 commits, 4.4.2 release candidate)
      https://github.com/Lora-net/LoRaMac-node/tree/6231a5d65806142a6f887e6183227b8da9e49238

   + Various bug fixes and enhancements

   + Removed inclusions of radio.h, utilities.h, etc. from loraMac.h API 
   + Lora.c/h and lora-test.c/h moved from Certifications/ to Patterns/Basic

### V1.2.2/17-June-2019 ###
===============================
   +  Trace logging & LoRa Mac version display updates 
   +  Some function prototypes added

### V1.2.1/13-December-2018 ###
===============================
   +  Implements LoRa Mac 4.4.2-rc.5 from Semtech/StackForce develop branch

### V1.2.0/10-July-2018 ###
===============================
   +  Implements LoRa Mac 4.4.2-rc.1 from Semtech/StackForce develop branch

   + Supports Class B
   + Features secure element API
   + Added RU864 region
   + Removed US915-Hybrid
  
### V1.1.5/30-March-2018 ###
===============================
   +  Implements LoRa Mac 4.4.1 from Semtech/StackForce
  
### V1.1.4/08-January-2018 ###
===============================
   + Bug fix in McpsIndication related to downlink Application data buffer

### V1.1.3/20-December-2017 ###
===============================
   +  Implements LoRa Mac from Semtech/StackForce develop branch (10-Oct-2017 commits, 4.4.1 release candidate)
      https://github.com/Lora-net/LoRaMac-node/tree/ae0c9cc5d53ebbc11437dfe9711a1c3a1114dc5f

   + Merge pull request #302 from nestorayuso/patch-3
  
### V1.1.2/08-September-2017 ###
===============================
   +  Implements LoRa Mac 4.4.0 release from Semtech/StackForce
      https://github.com/Lora-net/LoRaMac-node/tree/f42be67be402a40b3586724800771bfe13fb18e6

   + AckTimeoutRetriesCounter must be reset every time a new request (unconfirmed or confirmed) is performed.
   + Added verification for the power parameter in order to check that it is greater or equal to 0.

   + Enhancement for regions without FSK modulation support.
   + Bug fix for RX window 2 in class c mode.
   + AdrAckReq bit must be set to false as soon as we reach the lowest datarate.
   + MacCommandsBufferIndex must be reset when the mac commands are being sent on port 0.
   + Fixed RegionCommonSetBandTxDone in order to also update band->LastTxDoneTime when performing the Join procedure.
   + Added verification of payload size for Unconfirmed and Confirmed messages depending on Dwell time.

   + Added missing Rx1 timeout handling.
   + Updated all regions to use MAX output power by default.
   + Merge remote-tracking branch 'origin/develop' into develop
   + Bug fix in KR920 - update the maxEIRP calculation for continuous wave
   + Bug fix in IN865 - Update the band of the default channels
   + Bug fix in AS923 - for RX use always the payload limitation of dwell 0
   + Update function RegionCommonChanVerifyDr. Perform an 'AND' operation for security
   + Move the verification of ADR parameters into the common section. Update all regions with the related changes
   + Update comment for function RegionLinkAdrReq
   + Apply variables to data structure LinkAdrReqParams_t
   + Bug fix for KR920
   + Rename data structure LinkAdrParams_t
   + Issue(#238): Apply missing variable in sSetBandTxDoneParams
   + Bug fix in processing MAC commands for case SRV_MAC_TX_PARAM_SETUP_REQ.
   + Isse(#238): Apply missing variable in struct sBand
   + Update the MAC to enable the server to control the Channels Mask and the number of transmissions even ADR is off
   + Issue(#238): Update the backoff procedure for all regions. Move code parts into the common section
   + Update implementation to allow automatic MAC answers on port 0
   + Issue(#253): Delete all preconfigured channels when performing a join request
   + Initialize variable phyParam in functions RegionXXGetPhyParam
   + Remove assert_param from the radio drivers
   + Synchronize function RegionXXTxConfig
   + Update carrier sense functionality for LBT
   + Issue(#259): Update comment for variable HasLoopedThroughMain
   + Issue(#257): Fix typo in OnRxWindow2TimerEvent 
   + Change scientific notation to numeric notation
   + Merge pull request #260 from clmklk/AU915

   + AU915: update Datarate limits according to LoRaWan 1.0.2rB
   + AU915: update Downstream datarate table according to LoRaWan 1.0.2rB

   + Merge pull request #225 from OpenChirp/patch_1
	  
### V1.1.1/01-June-2017 ###
===============================
   +  Implements LoRa Mac from Semtech/StackForce develop branch (30-May-2017 commits, 4.4.0 release candidate)
      https://github.com/Lora-net/LoRaMac-node/tree/e2f35db75c1b449379d3b520c2d4e5922a9f5c81

   + Issue(235): Update functions RegionXXNextChannel.
   + Issue(238): Update initialization value of nextTxDelay.
   + Issue(234): Report back the aggregated time off.
   + Issue(232): Relocate function call to CalculateBackOff.
   + Issue(239): Update band TX power to the maximum.
   + Update LimitTxPower for US915.
   + Bug fix in function RegionCN470Verify.
   + Bug fix in function RegionAU915Verify.
   + Issue(229): Fix issue when receiving frames in second RX2 in Class C.
   + Add a ommand to get the next lower datarate.
   + Group initializations
   + Update regional definitions of KR920.
   + Update regional definitions of EU868.
   + Issue(239): Update regional definitions of AU915.
   + Update regional definitions of AU915.
   + Update regional definitions of AS923.
   + Update TX power computations.

   + Remove duplicated call to ApplyDrOffset in function RegionRxConfig.
   + Relocate the datarate, up- and downlink dwell time into a structure.
   + Change API of RegionGetPhyParam and the related functions.
   + Bug fix in function LoRaMacQueryTxPossible.
   + Apply patch for dwell time and minimum datarate.
   + Change the default datarate to DR_2 for AS923.
   + Take dwell time for ADR calculations and datarate settings into account.
   + Update LoRaMacQueryTxPossible to reset the MAC commands buffer.

   + Issue(#221): Add the dwell time in function ValidatePayloadLength.
   + Increase the transmission and reception timeout for KR920.
   + Bug fix in functions OnRxWindowXTimerEvent.
   + Remove datarate assignment.
   + Setup the downlink and uplink dwell time default value to 1.
   + Add frequency range check for AS923
   + Issue(#221): Bug fix in max payload size calculation.

   + GitHub reported issues corrections.  
   + Changed the AdrAckCounter handling as expected by the test houses.  
   + Fix an issue where the node stopped transmitting.  
   + Removed useless LoRaMacPayload buffer.  
   + MAC layer indications handling simplification.  
   + Relocate parameter settings from ResetMacParameters to the initialization.

### V1.1.0/27-February-2017 ###
===============================
   +  Implements LoRa Mac 4.4.0 from Semtech/StackForce  from the develop branch

### V1.0.3/01-January-2017 ###
===============================
   +  Read date between 2 successive read time to make sure date is ok

### V1.0.2/15-November-2016 ###
===============================
   +  Corrected 1 bug in LoRaMac-board.h: RX_WND_2_CHANNEL for EU is now back at DR_0
   +  Corrected 1 bug in LoRaMac.c for  dataRate adaptation

### V1.0.1/15-September-2016 ###
===============================
   +  Implements LoRa Mac 4.3.0 from Semtech/StackForce 

### V1.0.0/01-July-2016 ###
===============================
   + First R1.0.0 customized version for STM32Cube solution.
   + Comissioning_template.h in /Conf contains all Lora Ids to connect on LoRa network
         It is provided as a template. It must be moved to /Projects/inc/ as Comissioning.h
   + All files in Conf/src are provided as template and must be copied in /Projects/src. 
   + All files in Conf/inc are provided as template and must be copied in /Projects/inc.
		#if 0 and #endif must be removed to enable the template in the user directory
   + Implements LoRa Mac 4.2.0 from Semtech/StackForce
      + \Lora\Mac\LoRaMac.c : replace floating exponent e3 and e6 by int number
      + \Lora\Utilities\delay.c : cast uint32_t
   + Modified intensively timeServer.c 
   + new low layer interfacing Cube HAL (hw_rtc.c, hw_gpio.c and hw_spi.c)
   + added lora.c as an interface layer to ease product integration


 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
 