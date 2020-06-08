# LoRaWAN endpoint stack implementation and example projects

      ______                              _
     / _____)             _              | |
    ( (____  _____ ____ _| |_ _____  ____| |__
     \____ \| ___ |    (_   _) ___ |/ ___)  _ \
     _____) ) ____| | | || |_| ____( (___| | | |
    (______/|_____)_|_|_| \__)_____)\____)_| |_|
        (C)2013-2019 Semtech

     ___ _____ _   ___ _  _____ ___  ___  ___ ___
    / __|_   _/_\ / __| |/ / __/ _ \| _ \/ __| __|
    \__ \ | |/ _ \ (__| ' <| _| (_) |   / (__| _|
    |___/ |_/_/ \_\___|_|\_\_| \___/|_|_\\___|___|
    embedded.connectivity.solutions===============

## Introduction

The aim of this project is to show an example of the endpoint LoRaWAN stack implementation.

This project has 3 active branches in place. The **[master](https://github.com/Lora-net/LoRaMac-node/tree/master)** branch which provides the latest released source code ([v4.4.2](https://github.com/Lora-net/LoRaMac-node/releases/tag/v4.4.2)), the **[develop](https://github.com/Lora-net/LoRaMac-node/tree/develop)** branch which provides the current source code development status to be released next ([Milestone 4.4.3](https://github.com/Lora-net/LoRaMac-node/milestone/4)) and the **[feature/5.0.0](https://github.com/Lora-net/LoRaMac-node/tree/feature/5.0.0)** branch which provides a preview of the current source code development status for [LoRaWAN Specification v1.1](https://lora-alliance.org/resource-hub/lorawantm-specification-v11) specification.([Milestone 5.0.0](https://github.com/Lora-net/LoRaMac-node/milestone/3))

* The [master](https://github.com/Lora-net/LoRaMac-node/tree/master) branch implementation is based on [LoRaWAN Specification v1.0.3](https://lora-alliance.org/resource-hub/lorawantm-specification-v103) and [LoRaWAN Regional Parameters v1.0.3revA](https://www.lora-alliance.org/resource-hub/lorawantm-regional-parameters-v103reva) specifications.  
ClassA, ClassB and ClassC end-device classes are fully implemented.

* The [develop](https://github.com/Lora-net/LoRaMac-node/tree/develop) branch implementation is based on [LoRaWAN Specification v1.0.3](https://lora-alliance.org/resource-hub/lorawantm-specification-v103) and [LoRaWAN Regional Parameters v1.0.3revA](https://lora-alliance.org/resource-hub/lorawan-regional-parameters-v103reva) specifications.  
ClassA, ClassB and ClassC end-device classes are fully implemented.

* The [feature/5.0.0](https://github.com/Lora-net/LoRaMac-node/tree/feature/5.0.0) branch implementation is based on [LoRaWAN Specification v1.1](https://lora-alliance.org/resource-hub/lorawantm-specification-v11) and [LoRaWAN Regional Parameters v1.1rB](https://lora-alliance.org/resource-hub/lorawantm-regional-parameters-v11rb) specifications.  
ClassA, ClassB and ClassC end-device classes are fully implemented.

This project also provides SX1272/73, SX1276/77/78/79 and SX1261/2 radio drivers.

For each currently supported platform example applications are provided.

* **LoRaMac/classA**: ClassA end-device example application.

* **LoRaMac/classB**: ClassB end-device example application.

* **LoRaMac/classC**: ClassC end-device example application.

* **LoRaMac/fuota-test-01**: FUOTA test scenario 01 end-device example application. (Based on provided application common packages)

* **LoRaMac/periodic-uplink-lpp**: ClassA/B/C end-device example application. Periodically uplinks a frame using the Cayenne LPP protocol. (Based on provided application common packages)

* **ping-pong**: Point to point RF link example application.

* **rx-sensi**: Example application useful to measure the radio sensitivity level using an RF generator.

* **tx-cw**: Example application to show how to generate an RF Continuous Wave transmission.

**Note**: *Each LoRaWAN application example (LoRaMac/classX) includes an implementation of the LoRa-Alliance; LoRaWAN certification protocol.*

**Note**: *The LoRaWAN stack API documentation can be found at: http://stackforce.github.io/LoRaMac-doc/*

## Supported platforms

This project currently provides support for the below platforms.  
This project can be ported to other platforms using different MCU than the ones currently supported.  
The [Porting Guide](http://stackforce.github.io/LoRaMac-doc/_p_o_r_t_i_n_g__g_u_i_d_e.html) document provides guide lines on how to port the project to other platforms.

* NAMote72
  * [NAMote72 platform documentation](Doc/NAMote72-platform.md)

* NucleoLxxx - Discovery kit
  * [NucleoLxxx and Discovery kit platforms documentation](Doc/NucleoLxxx-platform.md)

* SKiM880B, SKiM980A, SKiM881AXL
  * [SKiM88xx platforms documentation](Doc/SKiM88xx-platform.md)

* SAML21
  * [SAML21 platform documentation](Doc/SAML21-platform.md)

## Usage

A CMAKE building system is used in order to generate the right set of files to compile and debug the different projects.

Further information can be found in [Development environment](Doc/development-environment.md) document.

## Acknowledgments

The mbed (https://mbed.org/) project was used at the beginning as source of
inspiration.

This program uses the AES algorithm implementation (http://www.gladman.me.uk/) by Brian Gladman.

This program uses the CMAC algorithm implementation
(http://www.cse.chalmers.se/research/group/dcs/masters/contikisec/) by Lander Casado, Philippas Tsigas.

## Changelog

### 2019-07-19, V4.4.2

* General
    1. Release based on "LoRaWAN specification 1.0.3" and "LoRaWAN Regional Parameters v1.0.3revA"
    2. Examples application refactoring plus the addition of the application status display on the serial port. (921600-8-N-1)
    3. Added new refactored application examples
       * Added LoRa-Alliance defined application layer protocols support. Only FUOTA test scenario 01 required features are currently implemented.  
          - "Clock Synchronization" package
          - "Fragmented data block transport" package
          - "Remote multicast setup" package
    4. Added a NVM context management module. (Disabled by default)
    5. Added secure-element support
    6. Refactored timer.c/h, rtc-driver.c/h and added a systime.c/h module.
    7. Functions in ISR context have been moved to main context
    8. Replaced BoardDisableIrq and BoardEnableIrq functions by CRITICAL_SECTION_BEGIN and CRITICAL_SECTION_END respectively.
    9. Added B-L072Z-LRWAN1 platform support.
    10. Added NucleoL476 platform support.
    11. Added IMST new platforms.
    12. Removed SensorNode platform support
    13. Removed MoteII platform support
    14. Removed LoRaMote platform support
    15. Applied SX1272 and SX1276 radios errata note 3.1 to the radio drivers implementation.
    16. Fix printf/scanf functions when GCC is used as compiler.

* LoRaWAN
    1. GitHub reported issues corrections. Please refer to [Release Version 4.4.2](https://github.com/Lora-net/LoRaMac-node/milestone/2)
    2. Heavily refactored the LoRaMac.c/h implementation.
    3. Added RU864 region support.
    4. Removed US915-Hybrid region support
    5. Added ClassB support.
    6. Added a callback to notify the upper layer to call LoRaMacProcess function
    7. Implemented the support for RxC windows (ClassC) required by the application layer protocols.
    8. Changed multicast channels handling according to the application layer protocols.

**LoRaWAN pre-certification results**

Please refer to [Releases pre-certification-results](https://github.com/Lora-net/LoRaMac-node/wiki/releases-pre-certification-results) document for further information.

### 2018-03-07, V4.4.1

* General
    1. Release based on "LoRaWAN specification 1.0.2" and "LoRaWAN Regional Parameters v1.0.2rB"
    2. Added SX126x radio support.
    3. Added NucleoL073 and NucleoL152 board platforms support.
    4. Added Microchip/Atmel SAML21 Xplained Pro hardware platform support.
    5. Added CMAKE build system support
    6. Removed CoIDE projects support
    7. Removed Keil projects support

* LoRaWAN
    1. GitHub reported issues corrections. Please refer to [Release Version 4.4.1](https://github.com/Lora-net/LoRaMac-node/milestone/1)

**LoRaWAN pre-certification results**

Please refer to [Releases pre-certification-results](https://github.com/Lora-net/LoRaMac-node/wiki/releases-pre-certification-results) document for further information.

### 2017-09-08, V4.4.0

* General
    1. First release based on "LoRaWAN specification 1.0.2" and "LoRaWAN Regional Parameters v1.0.2rB"
    2. This version has passed the LoRa-Alliance compliance tests for the regions shown in the below table.
    3. GitHub reported issues corrections.
    4. Updated radio driver to perform the LBT carrier sense continuously for a given period of time.

* LoRaWAN
    1. GitHub reported issues corrections.
    2. Updated implementation to support 1.0.2 specification additions.
    3. Added the support for all "LoRaWAN Regional Parameters v1.0.2rB" document defined regions.

**LoRaWAN pre-certification results**

Please refer to [Releases pre-certification-results](https://github.com/Lora-net/LoRaMac-node/wiki/releases-pre-certification-results) document for further information.

### 2017-04-19, V4.3.2

* General (Last release based on LoRaWAN specification 1.0.1)
    1. This version has passed EU868 and US915 LoRa-Alliance compliance tests.
    2. GitHub reported issues corrections.
    3. Added an algorithm to automatically compute the Rx windows parameters. (Window symbolTimeout and Offset from downlink expected time)
    4. Added a workaround to reset the radio in case a TxTimeout occurs.
    5. Modified FSK modem handling to use the provided symbolTimeout (1 symbol equals 1 byte) when in RxSingle mode.
    6. Added newly defined TxCw(Tx Continuous Wave) certification protocol command.
    7. Added a fix for an overflow issue that could happen with NmeaStringSize variable.
    8. Improved GpioMcuInit function to first configure the output pin state before activating the pin.

* LoRaWAN
    1. GitHub reported issues corrections.
    2. Changed the AdrAckCounter handling as expected by the test houses.
    3. Fix an issue where the node stopped transmitting.
    4. Removed useless LoRaMacPayload buffer.
    5. MAC layer indications handling simplification.
    6. Relocate parameter settings from ResetMacParameters to the initialization.

**LoRaWAN pre-certification results**

Please refer to [Releases pre-certification-results](https://github.com/Lora-net/LoRaMac-node/wiki/releases-pre-certification-results) document for further information.

### 2017-02-27, V4.3.1

* General
    1. This version has passed EU868 and US915 LoRa-Alliance compliance tests.
    2. Update the MAC layer in order to be LoRaWAN version 1.0.1 compliant (Mainly US915 bug fixes)
    3. Removed api-v3 support from the project.
    4. GitHub reported issues corrections.
    5. Updated SensorNode projects according to the new MCU reference STM32L151CBU6. Bigger memories.
    6. Addition of MoteII platform based on the IMST module iM881A (STM32L051C8)
    7. Addition of NAMote72 platform
    8. Correct compliance test protocol command 0x06 behavior
    9. Added TxCw (Tx continuous wave) LoRaWAN compliance protocol command.
    10. Added TxContinuousWave support to the radio drivers.
    11. Updated ST HAL drivers.
        * STM32L1xx_HAL-Driver : V1.2.0
        * STM32L0xx_HAL_Driver : V1.7.0

* LoRaWAN
    1. US band corrections in order to pass the LoRaWAN certification.
    2. GitHub reported issues corrections.
    3. Add region CN470 support.

**LoRaWAN pre-certification results**

Please refer to [Releases pre-certification-results](https://github.com/Lora-net/LoRaMac-node/wiki/releases-pre-certification-results) document for further information.

### 2016-06-22, V4.3.0

* General
    1. This version has passed all LoRa-Alliance compliance tests.
    2. Update the MAC layer in order to be LoRaWAN version 1.0.1 compliant
    3. Applied to all application files the certification protocol change for LoRaWAN 1.0.1 compliance tests.

       **REMARK**: api-v3 application files aren't updated.

    4. Add radio RX_TIMEOUT irq clear into the irq handler.
    5. Removed the end less loop from HAL_UART_ErrorCallback.
    6. Update of the STM32L0 HAL to version 1.6.0
    7. Consolidated the line endings across all project files.
       Windows line endings has been choose for almost every file.

* LoRaWAN
    1. Updated maximum payload size for US band.
    2. Update datarate offset table for US band.
    3. Make MAC commands sticky
    4. Add retransmission back-off
    5. Remove the TxPower limitation for US band on LoRaMacMibSetRequestConfirm function. The power will be limited anyway when the SendFrameOnChannel functions is called.
    6. Issue(#81): Bug fix in function LoRaMacMlmeRequest case MLME_JOIN. Function will return LORAMAC_STATUS_BUSY in case the MAC is in status MAC_TX_DELAYED.
    7. Add debug pin support to LoRaMote platform.
    8. Updated and improved MPL3115 device driver.
    9. Issue(#83): Bug fix in parameter validation
    10. Issue(#84): Fix issue of CalibrateTimer function.
    11. RTC driver major update
    12. Applied pull request #87.
    13. Add a function to verify the RX frequency of window 2 for US band.
    14. Issue(#88): Bug fix in function PrepareFrame where repeated MAC commands were not handled correctly.
    15. Bug fix in OnRadioRxDone. Node now drops frames on port 0 with fOpts > 0.
    16. Bug fix in OnRadioRxDone. Node now receives frames with fOpts > 0 when no payload is present.

**LoRaWAN pre-certification results**

Please refer to [Releases pre-certification-results](https://github.com/Lora-net/LoRaMac-node/wiki/releases-pre-certification-results) document for further information.

### 2016-05-13, V4.2.0

* General
    1. This version has passed all LoRa-Alliance compliance tests.
    2. Update STM32L1xx_HAL_Driver version to 1.5. Update related drivers and implementations accordingly.

       **REMARK**: This change implies that the time base had to be changed from microseconds to milliseconds.

    3. Corrected the frequency check condition for // ERRATA 2.1 - Sensitivity Optimization with a 500 kHz Bandwidth
    4. Optimize radio drivers regarding FSK PER
    5. Resolve issue when calling SX127xInit function more than once
    6. Add a definition for the LoRaWAN device address. Add an IEEE_OUI for the LoRaWAN device EUI.
    7. Add a definition for the default datarate.
    8. Issue(#66) correction of functions SX1276SetOpMode and SX1272SetOpMode.
    9. Issue(#68): Fix for low level RF switch control.
    10. Increase RTC tick frequency for higher resolution.
    11. Update the radio wake up time.

* LoRaWAN
    1. Issue(#56) correction
    2. Update channel mask handling for US915 hybrid mode to support any block in the channel mask.
    3. Issue(#63) correct the maximum payload length in RX mode.
    4. Fix Tx power setting loss for repeated join requests on US band.
    5. Introduce individual MIN and MAX datarates for RX and TX.
    6. Add the possibility to set and get the ChannelsDefaultDatarate.
    7. Optimization of the RX symbol timeout.
    8. Issue(#59): Add the possibility to set the uplink and downlink counter.
    9. Replace definition LORAMAC_DEFAULT_DATARATE by ChannelsDefaultDatarate in LoRaMacChannelAdd.
    10. Issue(#72): Fix of possible array overrun in LoRaMacChannelRemove.
    11. Introduce a new status MAC_RX_ABORT. Reset MAC_TX_RUNNING only in OnMacStateCheckTimerEvent.
    12. Accept MAC information of duplicated, confirmed downlinks.
    13. Issue(#74): Drop frames with a downlink counter difference greater or equal to MAX_FCNT_GAP.

**LoRaWAN pre-certification results**

Please refer to [Releases pre-certification-results](https://github.com/Lora-net/LoRaMac-node/wiki/releases-pre-certification-results) document for further information.

### 2016-03-10, V4.1.0

* General
    1. This version has passed all mandatory LoRa-Alliance compliance tests.

       *One of the optional tests is unsuccessful (FSK downlinks PER on Rx1 and Rx2 windows) and is currently under investigation.*
    2. Removed support for Raisonance Ride7 IDE (Reduces the amount of work to be done at each new release)
    3. Removed the Bleeper-72 and Bleeper-76 platforms support as these are now deprecated.
    4. Application state machine. Relocate setting sleep state and update the duty cycle in compliance test mode.
    5. Bug fix in TimerIrqHandler. Now, it is possible to insert timers in callback.
    6. Changed TimerHwDelayMs function to be re-entrant.
    7. Corrected FSK modem packets bigger than 64 bytes handling (Issue #36)

* LoRaWAN
    1. Rename attribute nbRetries to NbTrials in structure McpsReqConfirmed_t. (Issue #37)
    2. Updated implementation of SetNextChannel. Added enabling default channels in case of join request. (Issue #39)
    3. Add missing documentation about MIB_REPEATER_SUPPORT. (Issue #42).
    4. Add a new LoRaMacState to allow adding channels during TX procedure. (Issue #43)
    5. Relocate the activation of LoRaMacFlags.Bits.McpsInd in OnRadioRxDone.
    6. Add a new function PrepareRxDoneAbort to prepare a break-out of OnRadioRxDone in case of an error
    7. Activate default channels in case all others are disabled. (Issue #39)
    8. Bug fix in setting the default channel in case none is enabled.
    9. SRV_MAC_NEW_CHANNEL_REQ MAC command added a fix to the macIndex variable on US915 band.
    10. Start the MacStateCheckTimer in OnRxDone and related error cases with a short interval to handle events promptly. (Issue #44)
    11. Reset status of NodeAckRequested if we received an ACK or in case of timeout.
    12. Removed additional EU868 channels from the LoRaWAN implementation files. GitHub (Issue #49)
        The creation of these additional channels has been moved to the application example.
    13. Improved and corrected AdrNextDr function.

### 2015-12-18, V4.0.0

* General
    1. STACKFORCE new API integration
    2. Reverse the EUIs arrays in the MAC layer.
    3. LoRaWAN certification protocol implementation
    4. All reported issues and Pull requests have been addressed.

### 2015-10-06, V3.4.1

* General
    1. Bug fixes

* LoRaWAN
    1. Corrected downlink counter roll over management when several downlinks were missed.
    2. Corrected the Radio maximum payload length management. Radio was filtering received frames with a length bigger than the transmitted one.
    3. Applied Pull request #22 solution proposition.

### 2015-10-30, V3.4.0

* General
    1. Changed all applications in order to have preprocessing definitions on top of the files and added relevant comments
    2. Applications LED control is no more done into the timer callback functions but instead on the main while loop.
    3. Added TimerStop function calls to each timer event callback.
    4. Corrected timings comments. Timing values are most of the time us based.
    5. Changed types names for stdint.h names. Helps on code portability
    6. Renamed rand and srand to rand1 and srand1. Helps on code portability
    7. Added some missing variables casts. Helps on code portability
    8. Removed NULL definition from board.h
    9. Added const variable attribute when necessary to functions prototypes
    10. Moved ID1, ID2 and ID3 definition from board.h to board.c, usb-cdc-board.c and usb-dfu-board.c
    11. Removed the definition of RAND_SEED. It has been replaced by a function named BoardGetRandomSeed
    12. Renamed BoardMeasureBatterieLevel to BoardGetBatteryLevel
    13. Added SetMaxPayloadLength API function to SX1272 and SX1276 radio drivers
    14. Changed the name of Radio API Status function to GetStatus
    15. AES/CMAC Changed types names for stdint.h names. Helps on code portability (Issue #20)
    16. Moved __ffs function from utilities.h to spi-board.c. This function is only used there.
    17. Utilities.c removed fputc function redefinition.
    18. Replaced the usage of __IO attribute by volatile.

* LoRaWAN
    1. Added support for the US915 band (Normal mode and hybrid mode. Hybrid mode is a temporary configuration up until servers support it automatically) (Issue #16)
    2. Corrected and simplified the downlink sequence counter management.
    3. Removed the usage of PACKED attribute for data structures.
    4. Renamed LoRaMacEvent_t into LoRaMacCallbacks_t and added a function pointer for getting battery level status
    5. Renamed LoRaMacSetDutyCycleOn into LoRaMacSetTestDutyCycleOn
    6. Renamed LoRaMacSetMicTest into LoRaMacTestSetMic
    7. Increased the PHY buffer size to 250
    8. Removed IsChannelFree check on LoRaMacSetNextChannel function. LoRaWAN is an ALHOA protocol. (Pull request #8)
    9. LoRaMacEventInfo.TxDatarate now returns LoRaWAN datarate (DR0 -> DR7) instead of (SF12 -> DF7)
    10. Corrected channel mask management for EU868 band.
    11. Corrected LoRaMacPrepareFrame behavior function when no applicative payload is present.
    12. LoRaMac-board.h now implements the settings for the PHY layers specified by LoRaWAN 1.0 specification. ( EU433, CN780, EU868, US915 ) (Issue #19)
    13. Added LORAMAC_MIN_RX1_DR_OFFSET and LORAMAC_MAX_RX1_DR_OFFSET definitions to LoRaMac-board.h. Can be different upon used PHY layer
    14. Added the limitation of output power according to the number of enabled channels for US915 band.
    15. Added the limitation of the applicative payload length according to the datarate. Does not yet take in account the MAC commands buffer. (Issue #15)
    16. Corrected MacCommandBufferIndex management. (Issue #18)

### 2015-08-07, v3.3.0

* General
    1. Added the support for LoRaWAN Class C devices.
    2. Implemented the radios errata note workarounds. SX1276 errata 2.3 "Receiver Spurious Reception of a LoRa Signal" is not yet implemented.
    3. Increased FSK SyncWord timeout value in order to listen for longer time if a down link is available or not. Makes FSK downlink more reliable.
    4. Increased the UART USB FIFO buffer size in order to handle bigger chunks of data.

* LoRaWAN
    1. Renamed data rates as per LoRaWAN specification.
    2. Added the support for LoRaWAN Class C devices.
    3. Handling of the MAC commands was done incorrectly the condition to verify the length of the buffer has changed from < to <=.
    4. Added the possibility to change the channel mask and number of repetitions trough SRV_MAC_LINK_ADR_REQ command when ADR is disabled.
    5. Corrected Rx1DrOffset management. In previous version DR1 was missing for all offsets.
    6. Changed confirmed messages function to use default datarate when ADR control is off.
    7. After a Join accept the node falls back to the default datarate. Enables the user to Join a network using a different datarate from its own default one.
    8. Corrected default FSK channel frequency.
    9. Solved a firmware freezing when one of the following situations arrived in OnRxDone callback: bad address, bad MIC, bad frame. (Pull request #10)
    10. Moved the MAC commands processing to the right places. FOpts field before the Payload and Port 0 just after the decryption. (Pull request #9)
    11. Weird conditions to check datarate on cmd mac SRV_MAC_NEW_CHANNEL_REQ (Pull request #7)
    12. Ignore join accept message if already joined (Pull request #6)
    13. Channel index verification should use OR on SRV_MAC_NEW_CHANNEL_REQ command (Pull request #5)
    14. Corrected the CFList management on JoinAccept. The for loop indexes were wrong. (Pull request #4)
    15. Correction of AES key size (Pull request #3)

### 2015-04-30, v3.2.0

* General
    1. Updated LoRaMac implementation according to LoRaWAN R1.0 specification
    2. General cosmetics corrections
    3. Added the support of packed structures when using IAR tool chain
    4. Timers: Added a function to get the time in us.
    5. Timers: Added a typedef for time variables (TimerTime_t)
    6. Radios: Changed the TimeOnAir radio function to return a uint32_t value instead of a double. The value is in us.
    7. Radios: Corrected the 250 kHz bandwidth choice for the FSK modem
    8. GPS: Added a function that returns if the GPS has a fix or not.
    9. GPS: Changed the GetPosition functions to return a latitude and longitude of 0 and altitude of 65535 when no GPS fix.

* LoRaWAN
    1. Removed support for previous LoRaMac/LoRaWAN specifications
    2. Added missing MAC commands and updated others when necessary
        * Corrected the Port 0 MAC commands decryption
        * Changed the way the upper layer is notified. Now it is only  notified
        when all the operations are finished.

           When a ClassA Tx cycle starts a timer is launched to check every second if everything is finished.

        * Added a new parameter to LoRaMacEventFlags structure that indicates on which Rx window the data has been received.
        * Added a new parameter to LoRaMacEventFlags structure that indicates if there is applicative data on the received payload.
        * Corrected ADR MAC command behavior
        * DutyCycle enforcement implementation (EU868 PHY only)

          **REMARK 1** *The regulatory duty cycle enforcement is enabled by default
          which means that for lower data rates the node may not transmit a new
          frame as quickly as requested.
          The formula used to compute the node idle time is*

          *Toff = TimeOnAir / DutyCycle - TxTimeOnAir*

          *Example:*
          *A device just transmitted a 0.5 s long frame on one default channel.
          This channel is in a sub-band allowing 1% duty-cycle. Therefore this
          whole sub-band (868 MHz - 868.6 MHz) will be unavailable for 49.5 s.*

          **REMARK 2** *The duty cycle enforcement can be disabled for test
          purposes by calling the LoRaMacSetDutyCycleOn function with false
          parameter.*
        * Implemented aggregated duty cycle management
        * Added a function to create new channels
        * Implemented the missing features on the JoinAccept MAC command
        * Updated LoRaMacJoinDecrypt function to handle the CFList field.
    3. Due to duty cycle management the applicative API has changed.
    All applications must be updated accordingly.
    4. Added the possibility to chose to use either public or private networks

### 2015-01-30, v3.1.0

* General
    1. Started to add support for CooCox CoIDE Integrated Development Environment.
       Currently only LoRaMote and SensorNode platform projects are available.
    2. Updated GCC compiler linker scripts.
    3. Added the support of different tool chains for the HardFault_Handler function.

    4. Corrected Radio drivers I&Q signals inversion to be possible in Rx and in Tx.
       Added some missing radio state machine initialization.
    5. Changed the RSSI values type from int8_t to int16_t. We can have RSSI values below -128 dBm.
    6. Corrected SNR computation on RxDone interrupt.
    7. Updated radio API to support FHSS and CAD handling.
    8. Corrected in SetRxConfig function the FSK modem preamble register name.
    9. Added an invalid bandwidth to the Bandwidths table in order to avoid an error
       when selecting 250 kHz bandwidth when using FSK modem.

    10. Corrected RTC alarm setup which could be set to an invalid date.
    11. Added another timer in order increment the tick counter without blocking the normal timer count.
    12. Added the possibility to switch between low power timers and normal timers on the fly.
    13. I2C driver corrected the 2 bytes internal address management.
        Corrected buffer read function when more that 1 byte was to be read.
        Added a function to wait for the I2C bus to become IDLE.
    14. Added an I2C EEPROM driver.
    15. Corrected and improved USB Virtual COM Port management files.
        Corrected the USB CDC and USB UART drivers.
    16. Added the possibility to analyze a hard fault interrupt.

* LoRaMac
    1. Corrected RxWindow2 Datarate management.
    2. SrvAckRequested variable was never reset.
    3. Corrected tstIndoor applications for LoRaMac R3.0 support.
    4. LoRaMac added the possibility to configure almost all the MAC parameters.
    5. Corrected the LoRaMacSetNextChannel function.
    6. Corrected the port 0 MAC command decoding.
    7. Changed all structures declarations to be packed.
    8. Corrected the Acknowledgment retries management when only 1 trial is needed.
       Before the device was issuing at least 2 trials.
    9. Corrected server mac new channel req answer.
    10. Added the functions to read the Up and Down Link sequence counters.
    11. Corrected SRV_MAC_RX2_SETUP_REQ frequency handling. Added a 100 multiplication.
    12. Corrected SRV_MAC_NEW_CHANNEL_REQ. Removed the DutyCycle parameter decoding.
    13. Automatically activate the channel once it is created.
    14. Corrected NbRepTimeoutTimer initial value. RxWindow2Delay already contains RxWindow1Delay in it.

### 2014-07-18, v3.0.0

* General
    1. Added to Radio API the possibility to select the modem.
    2. Corrected RSSI reading formulas as well as changed the RSSI and SNR values from double to int8_t type.
    3. Changed radio callbacks events to timeout when it is a timeout event and error when it is a CRC error.
    4. Radio API updated.
    5. Updated ping-pong applications.
    6. Updated tx-cw applications.
    7. Updated LoRaMac applications in order to handle LoRaMac returned functions calls status.
    8. Updated LoRaMac applications to toggle LED2 each time there is an application payload down link.
    9. Updated tstIndoor application to handle correctly more than 6 channels.
    10. Changed the MPL3115 altitude variable from unsigned to signed value.
    11. Replaced the usage of pow(2, n) by defining POW2 functions. Saves ~2 KBytes of code.
    12. Corrected an issue potentially arriving when LOW_POWER_MODE_ENABLE wasn't defined.
        A timer interrupt could be generated while the TimerList could already be emptied.

* LoRaMac
    1. Implemented LoRaMac specification R3.0 changes.

    2. MAC commands implemented
        * LinkCheckReq                       **YES**
        * LinkCheckAns                       **YES**
        * LinkADRReq                         **YES**
        * LinkADRAns                         **YES**
        * DutyCycleReq                       **YES**
        * DutyCycleAns                       **YES**
        * Rx2SetupReq                        **YES**
        * Rx2SetupAns                        **YES**
        * DevStatusReq                       **YES**
        * DevStatusAns                       **YES**
        * JoinReq                            **YES**
        * JoinAccept                         **YES**
        * NewChannelReq                      **YES**
        * NewChannelAns                      **YES**

    3. Features implemented
        * Possibility to shut-down the device **YES**

            Possible by issuing DutyCycleReq MAC command.
        * Duty cycle management enforcement  **NO**
        * Acknowledgments retries            **YES**
        * Unconfirmed messages retries       **YES**

### 2014-07-10, v2.3.RC2

* General
    1. Corrected all radios antenna switch low power mode handling.
    2. SX1276: Corrected antenna switch control.

### 2014-06-06, v2.3.RC1

* General
    1. Added the support for SX1276 radio.
    2. Radio continuous reception mode correction.
    3. Radio driver RxDone callback function API has changed ( size parameter is no more a pointer).
           Previous function prototype:

            void    ( *RxDone )( uint8_t *payload, uint16_t *size, double rssi, double snr, uint8_t rawSnr );

       New function prototype:

            void    ( *RxDone )( uint8_t *payload, uint16_t size, double rssi, double snr, uint8_t rawSnr );

    4. Added Bleeper-76 and SensorNode platforms support.
    5. Added to the radio drivers a function that generates a random value from
       RSSI readings.
    6. Added a project to transmit a continuous wave and a project to measure the
       the radio sensitivity.
    7. Added a bootloader project for the LoRaMote and SensorNode platforms.
    8. The LoRaMac application for Bleeper platforms now sends the Selector and LED status plus the sensors values.
        * The application payload for the Bleeper platforms is as follows:

            LoRaMac port 1:

                 { 0xX0/0xX1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
                  ----------  ----------  ----------  ----------  ----
                       |           |           |           |        |
                 SELECTOR/LED  PRESSURE   TEMPERATURE  ALTITUDE  BATTERY
                 MSB nibble = SELECTOR               (barometric)
                 LSB bit    = LED
    9. Redefined rand() and srand() standard C functions. These functions are
       redefined in order to get the same behavior across different compiler
       tool chains implementations.
    10. GPS driver improvements. Made independent of the board platform.
    11. Simplified the RTC management.
    12. Added a function to the timer driver that checks if a timer is already in
       the list or not.
    13. Added the UART Overrun bit exception handling to the UART driver.
    14. Removed dependency of spi-board files to the "__builtin_ffs" function.
       This function is only available on GNU compiler tool suite. Removed --gnu
       compiler option from Keil projects. Added own __ffs function
       implementation to utilities.h file.
    15. Removed obsolete class1 devices support.
    16. Known bugs correction.

* LoRaMac
    1. MAC commands implemented
        * LinkCheckReq                       **YES**
        * LinkCheckAns                       **YES**
        * LinkADRReq                         **YES**
        * LinkADRAns                         **YES**
        * DutyCycleReq                       **YES** (LoRaMac specification R2.2.1)
        * DutyCycleAns                       **YES** (LoRaMac specification R2.2.1)
        * Rx2SetupReq                        **YES** (LoRaMac specification R2.2.1)
        * Rx2SetupAns                        **YES** (LoRaMac specification R2.2.1)
        * DevStatusReq                       **YES**
        * DevStatusAns                       **YES**
        * JoinReq                            **YES**
        * JoinAccept                         **YES** (LoRaMac specification R2.2.1)
        * NewChannelReq                      **YES** (LoRaMac specification R2.2.1)
        * NewChannelAns                      **YES** (LoRaMac specification R2.2.1)
    2. Features implemented
        * Possibility to shut-down the device **YES**

          Possible by issuing DutyCycleReq MAC command.
        * Duty cycle management enforcement  **NO**
        * Acknowledgments retries           **WORK IN PROGRESS**

          Not fully debugged. Disabled by default.
        * Unconfirmed messages retries       **WORK IN PROGRESS** (LoRaMac specification R2.2.1)
    3. Implemented LoRaMac specification R2.2.1 changes.
    4. Due to new specification the LoRaMacInitNwkIds LoRaMac API function had
       to be modified.

       Previous function prototype:

            void LoRaMacInitNwkIds( uint32_t devAddr, uint8_t *nwkSKey, uint8_t *appSKey );

       New function prototype:

            void LoRaMacInitNwkIds( uint32_t netID, uint32_t devAddr, uint8_t *nwkSKey, uint8_t *appSKey );
    5. Changed the LoRaMac channels management.
    6. LoRaMac channels definition has been moved to LoRaMac-board.h file
       located in each specific board directory.

### 2014-04-07, v2.2.0

* General
    1. Added IMST SK-iM880A starter kit board support to the project.
        * The application payload for the SK-iM880A platform is as follows:

        LoRaMac port 3:

             { 0x00/0x01, 0x00, 0x00, 0x00 }
              ----------  ----- ----------
                   |        |       |
                  LED     POTI     VDD
    2. Ping-Pong applications have been split per supported board.
    3. Corrected the SX1272 output power management.
       Added a variable to store the current Radio channel.
       Added missing FSK bit definition.
    4. Made fifo functions coding style coherent with the project.
    5. UART driver is now independent of the used MCU

### 2014-03-28, v2.1.0

* General
    1. The timers and RTC management has been rewritten.
    2. Improved the UART and UP501 GPS drivers.
    3. Corrected GPIO pin names management.
    4. Corrected the antenna switch management in the SX1272 driver.
    5. Added to the radio driver the possibility to choose the preamble length
       and rxSingle symbol timeout in reception.
    6. Added Hex coder selector driver for the Bleeper board.
    7. Changed copyright Unicode character to (C) in all source files.

* LoRaMac
    1. MAC commands implemented
        * LinkCheckReq                 **YES**
        * LinkCheckAns                 **YES**
        * LinkADRReq                   **YES**
        * LinkADRAns                   **YES**
        * DevStatusReq                 **YES**
        * DevStatusAns                 **YES**
        * JoinReq                      **YES**
        * JoinAccept                   **YES**
    2. Added acknowledgments retries management.
      Split the LoRaMacSendOnChannel function in LoRaMacPrepareFrame and
      LoRaMacSendFrameOnChannel. LoRaMacSendOnChannel now calls the 2 newly
      defined functions.  
      **WARNING**: By default the acknowledgment retries specific code isn't
      enabled. The current http://iot.semtech.com server version doesn't support
      it.
    3. Corrected issues on JoinRequest and JoinAccept MAC commands.
      Added LORAMAC_EVENT_INFO_STATUS_MAC_ERROR event info status.

### 2014-02-21, v2.0.0

* General
    1. The LoRaMac applications now sends the LED status plus the sensors values.
       For the LoRaMote platform the application also sends the GPS coordinates.
        * The application payload for the Bleeper platform is as follows:

            LoRaMac port 1:

                 { 0x00/0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
                  ----------  ----------  ----------  ----------  ----
                       |           |           |           |        |
                      LED      PRESSURE   TEMPERATURE  ALTITUDE  BATTERY
                                                     (barometric)
        * The application payload for the LoRaMote platform is as follows:

            LoRaMac port 2:

                 { 0x00/0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
                  ----------  ----------  ----------  ----------  ----  ----------------  ----------------  ----------
                       |           |           |           |        |           |                 |              |
                      LED      PRESSURE   TEMPERATURE  ALTITUDE  BATTERY    LATITUDE          LONGITUDE      ALTITUDE
                                                     (barometric)                                              (gps)
    2. Adapted applications to the new MAC layer API.
    3. Added sensors drivers implementation.
    4. Corrected new or known issues.
* LoRaMac
    1. MAC commands implemented
        * LinkCheckReq                 **YES**
        * LinkCheckAns                 **YES**
        * LinkADRReq                   **YES**
        * LinkADRAns                   **YES**
        * DevStatusReq                 **YES**
        * DevStatusAns                 **YES**
        * JoinReq                      **YES (Not tested)**
        * JoinAccept                   **YES (Not tested)**
    2. New MAC layer application API implementation.
* Timers and RTC.
    1. Still some issues. They will be corrected on next revisions of the firmware.

### 2014-01-24, v1.1.0

* LoRaMac
    1. MAC commands implemented
        * LinkCheckReq                 **NO**
        * LinkCheckAns                 **NO**
        * LinkADRReq                   **YES**
        * LinkADRAns                   **YES**
        * DevStatusReq                 **YES**
        * DevStatusAns                 **YES**
    2. Implemented an application LED control
        If the server sends on port 1 an application payload of one byte with
        the following contents:

            0: LED off
            1: LED on
        The node transmits periodically on port 1 the LED status on 1st byte and
        the message "Hello World!!!!" the array looks like:

            { 0, 'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '!', '!', '!', '!' }
* Timers and RTC.
    1. Corrected issues existing in the previous version
    2. Known bugs are:
        * There is an issue when launching an asynchronous Timer. Will be solved
          in a future version

### 2014-01-20, v1.1.RC1

* Added Doc directory. The directory contains:
    1. LoRa MAC specification
    2. Bleeper board schematic
* LoRaMac has been updated according to Release1 of the specification. Main changes are:
    1. MAC API changed.
    2. Frame format.
    3. ClassA first ADR implementation.
    4. MAC commands implemented
        * LinkCheckReq              **NO**
        * LinkCheckAns              **NO**
        * LinkADRReq                **YES**
        * LinkADRAns                **NO**
        * DevStatusReq              **NO**
        * DevStatusAns              **NO**
* Timers and RTC rewriting. Known bugs are:
    1. The Radio wakeup time is taken in account for all timings.
    2. When opening the second reception window the microcontroller sometimes doesn't enter in low power mode.

### 2013-11-28, v1.0.0

* Initial version of the LoRa MAC node firmware implementation.
