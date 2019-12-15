/* --COPYRIGHT--,BSD
 * Copyright (c) 2017, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
//*****************************************************************************
//
// saph.h - Driver for the SAPH Module.
//
//*****************************************************************************

#ifndef __MSP430WARE_SAPH_H__
#define __MSP430WARE_SAPH_H__

#include "inc/hw_memmap.h"

#ifdef __MSP430_HAS_SAPH__

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
// The following value specifies how many configurable channels in physcial
// interface.
//
//*****************************************************************************
#define SAPH_PHY_CHANNEL_COUNT                                                2

typedef struct _phy_param {
    //! Is the physical output channel to configure.
    //! \n Valid values are:
    //! - \b SAPH_PHY_CHANNEL_1
    //! - \b SAPH_PHY_CHANNEL_2
    uint16_t channel;
    //! Is the channel output value.
    //! \n Valid values are:
    //! - \b SAPH_PHY_OUTPUT_HIGH 
    //! - \b SAPH_PHY_OUTPUT_LOW [Default]
    uint16_t outputValue;
    //! Is the channel output enable.
    //! \n Valid values are:
    //! - \b SAPH_PHY_OUTPUT_ENABLE 
    //! - \b SAPH_PHY_OUTPUT_DISABLE [Default]
    uint16_t enableOutput;
    //! Is the channel output full pull enable.
    //! \n Valid values are:
    //! - \b SAPH_PHY_FULLPULL_ENABLE 
    //! - \b SAPH_PHY_FULLPULL_DISABLE [Default]
    uint16_t enableFullPull;
    //! Is the channel output termination enable.
    //! \n Valid values are:
    //! - \b SAPH_PHY_TERMINATION_ENABLE 
    //! - \b SAPH_PHY_TERMINATION_DISABLE [Default]
    uint16_t enableTermination;
    //! Is the channel output functional select.
    //! \n Valid values are:
    //! - \b SAPH_PHY_OUTPUT_GENERAL_PURPOSE [Default]
    //! - \b SAPH_PHY_OUTPUT_PULSEGENERATOR_SINGLE_DRIVE
    //! - \b SAPH_PHY_OUTPUT_PULSEGENERATOR_DIFFERENTIAL_DRIVE
    uint16_t outputFunction;
    //! Is the channel pull up trim pattern.
    uint16_t pullUpTrim;
    //! Is the channel pull down trim pattern.
    uint16_t pullDownTrim;
    //! Is the channel termination trim pattern.
    uint16_t terminationTrim;
} SAPH_configPHYParam;

typedef struct _phy_bias {
    //! Selects the channel for excitation bias switch.
    //! \n Valid values are:
    //! - \b SAPH_PHY_BIAS_SWITCH_OPEN [Default]
    //! - \b SAPH_PHY_BIAS_SWITCH_CLOSED 
    uint16_t biasSwitch[SAPH_PHY_CHANNEL_COUNT];
    //! Selects PGA bias.
    //! \n Valid values are:
    //! - \b SAPH_PHY_PGA_BIAS_GROUNDED 
    //! - \b SAPH_PHY_PGA_BIAS_LOW_VALUE
    //! - \b SAPH_PHY_PGA_BIAS_NOMINAL_VALUE
    //! - \b SAPH_PHY_PGA_BIAS_HIGH_VALUE
    uint16_t biasPGA;
    //! Selects excitation bias.
    //! \n Valid values are:
    //! - \b SAPH_PHY_EXCITATION_BIAS_GROUNDED
    //! - \b SAPH_PHY_EXCITATION_BIAS_LOW_VALUE
    //! - \b SAPH_PHY_EXCITATION_BIAS_NOMINAL_VALUE
    //! - \b SAPH_PHY_EXCITATION_BIAS_HIGH_VALUE
    uint16_t biasExcitation;
    //! Enables charge pump during acquisition.
    //! \n Valid values are:
    //! - \b SAPH_PHY_MULTIPLEXER_CHARGEPUMP_ENABLE
    //! - \b SAPH_PHY_MULTIPLEXER_CHARGEPUMP_DISABLE
    uint16_t enableChargePump;
    //! Enables line input leakage compensation.
    //! \n Valid values are:
    //! - \b SAPH_PHY_LEAKAGE_COMPENSATION_ENABLE
    //! - \b SAPH_PHY_LEAKAGE_COMPENSATION_DISABLE
    uint16_t enableLeakageCompensation;
    //! Selects PGA bias switch open or closed.
    //! \n Valid values are:
    //! - \b SAPH_PHY_PGA_BIAS_SWITCH_OPEN
    //! - \b SAPH_PHY_PGA_BIAS_SWITCH_CLOSED
    uint16_t biasSwitchPGA;
    //! Select ASQ bias switch control.
    //! \n Valid values are:
    //! - \b SAPH_PHY_BIAS_SWITCH_CONTROLLED_BY_REGISTER
    //! - \b SAPH_PHY_BIAS_SWITCH_CONTROLLED_BY_ASQ
    uint16_t biasSwitchASQ;
} SAPH_configPHYBiasParam;

typedef struct _ppg_count
{
    //! Selects high impedance input.
    //! \n Valid values are:
    //! - \b SAPH_PPG_HIGH_IMPEDANCE_ON_PAUSE_OUTPUTDRIVE
    //! - \b SAPH_PPG_HIGH_IMPEDANCE_ON_PAUSE_PLEV
    uint16_t highImpedance;
    //! Sets the pause level high or low.
    //! \n Valid values are:
    //! - \b SAPH_PPG_PAUSE_LEVEL_LOW
    //! - \b SAPH_PPG_PAUSE_LEVEL_HIGH
    uint16_t pauseLevel;
    //! Sets the pause polarity high or low.
    //! \n Valid values are:
    //! - \b SAPH_PPG_PAUSE_POLARITY_HIGH
    //! - \b SAPH_PPG_PAUSE_POLARITY_LOW
    uint16_t pausePolarity;
    //! Sets the stop pulse count.
    uint16_t stopPauseCount;
    //! Sets the excitation pulse count.
    uint16_t excitationPulseCount;
} SAPH_configPPGCountParam;

typedef struct _ppg_config
{
    //! Enables PPG prescaler.
    //! \n Valid values are:
    //! - \b SAPH_PPG_PRESCALER_ENABLE
    //! - \b SAPH_PPG_PRESCALER_DISABLE
    uint16_t enablePrescaler;
    //! Selects PPG trigger source.
    //! \n Valid values are:
    //! - \b SAPH_PPG_TRIGGER_SOURCE_SOFTWARE
    //! - \b SAPH_PPG_TRIGGER_SOURCE_ASQ
    //! - \b SAPH_PPG_TRIGGER_SOURCE_TIMER
    uint16_t triggerSource;
    //! Selects PPG channel.
    //! \n Valid values are:
    //! - \b SAPH_PPG_CHANNEL_0
    //! - \b SAPH_PPG_CHANNEL_1
    uint16_t channelSelect;
    //! Selects PPG or ASEQ in charge of PHY port.
    //! \n Valid values are:
    //! - \b SAPH_PPG_PORT_CHARGED_BY_PPG
    //! - \b SAPH_PPG_PORT_CHARGED_BY_ASEQ
    uint16_t portSelect;
} SAPH_configPPGParam;

typedef struct _asq_config
{
    //! Enables ASQ abort on errors if the conversion result is outside of expected
    //! value or an overflow/underflow condition occured.
    //! \n Valid values are:
    //! - \b SAPH_ASQ_ABORT_ON_ERROR_ENABLE
    //! - \b SAPH_ASQ_ABORT_ON_ERROR_DISABLE
    uint16_t abortOnError;
    //! Selects ASQ trigger source.
    //! \n Valid values are:
    //! - \b SAPH_ASQ_TRIGGER_SOURCE_SOFTWARE
    //! - \b SAPH_ASQ_TRIGGER_SOURCE_P_SEQUENCER
    //! - \b SAPH_ASQ_TRIGGER_SOURCE_TIMER
    uint16_t triggerSource;
    //! Selects ASQ channel.
    //! \n Valid values are:
    //! - \b SAPH_ASQ_CHANNEL_0
    //! - \b SAPH_ASQ_CHANNEL_1
    uint16_t channelSelect;
    //! Selects pwn channel or opposite side channel to receive.
    //! \n Valid values are:
    //! - \b SAPH_ASQ_IDENTICAL_CHANNEL
    //! - \b SAPH_ASQ_DIFFERENT_CHANNEL
    uint16_t sideOfChannel;
    //! Selects standby indication.
    //! \n Valid values are:
    //! - \b SAPH_ASQ_STANDBY_INDICATION_POWEROFF
    //! - \b SAPH_ASQ_STANDBY_INDICATION_STANDBY
    uint16_t standByIndication;
    //! Requests OFF in the end of sequence.
    //! \n Valid values are:
    //! - \b SAPH_ASQ_END_OF_SEQUENCE_OFF_DISABLE
    //! - \b SAPH_ASQ_END_OF_SEQUENCE_OFF_ENABLE
    uint16_t endOfSequence;
    //! Selects early receive bias generating source.
    //! \n Valid values are:
    //! - \b SAPH_ASQ_EARLY_RECEIVE_BIAS_BY_TIMEMARK_C
    //! - \b SAPH_ASQ_EARLY_RECEIVE_BIAS_BY_TIMEMARK_A
    uint16_t earlyReceiveBias;
    //! Enables channel toggle.
    //! \n Valid values are:
    //! - \b SAPH_ASQ_TOGGLE_CHANNEL_ENABLE
    //! - \b SAPH_ASQ_TOGGLE_CHANNEL_DISABLE
    uint16_t enableChannelToggle;
} SAPH_configASQParam;

typedef struct _asq_ping_config
{
    //! Sets ASQ ping counter for polarity.
    uint16_t polarity;
    //! Sets ASQ ping counter for pause level.
    uint16_t pauseLevel;
    //! Sets ASQ ping counter for high impedance.
    uint16_t pauseHighImpedance;
} SAPH_configASQPingParam;

typedef struct _mcnf_config
{
    //! Enables/disables low power bias operation mode.
    //! \n Valid values are:
    //! - \b SAPH_LOW_POWER_BIAS_MODE_DISABLE [Default]
    //! - \b SAPH_LOW_POWER_BIAS_MODE_ENABLE
    uint16_t lowPowerBiasMode;
    //! Enables/disables charge pump of the input multiplexer.
    //! \n Valid values are:
    //! - \b SAPH_CHARGE_PUMP_ON_SDHS_ASQ_REQUESTS_ONLY [Default]
    //! - \b SAPH_CHARGE_PUMP_ON_ALWAYS
    uint16_t chargePump;
    //! Sets bias impedance for RxBias and TxBias.
    //! \n Valid values are:
    //! - \b SAPH_MCNF_500_OHMS_RXBIAS_450_OHMS_TXBIAS
    //! - \b SAPH_MCNF_900_OHMS_RXBIAS_850_OHMS_TXBIAS
    //! - \b SAPH_MCNF_1500_OHMS_RXBIAS_1450_OHMS_TXBIAS [Default]
    //! - \b SAPH_MCNF_2950_OHMS_RXBIAS_2900_OHMS_TXBIAS
    uint16_t biasImpedance;
} SAPH_configModeParam;

//*****************************************************************************
//
// The following are values that can be passed to the mask parameter for
// functions: SAPH_clearInterrupt(), SAPH_enableInterrupt(), and 
// SAPH_disableInterrupt() as well as returned by the 
// SAPH_getInterruptStatus() function.
//
//*****************************************************************************
#define SAPH_DATA_ERROR_INTERRUPT                                       DATAERR
#define SAPH_TIMEMARK_F_TIMEOUT_INTERRUPT                                 TMFTO
#define SAPH_ACQUISITION_SEQUENCER_DONE_INTERRUPT                         SEQDN
#define SAPH_PING_TRANSMIT_DONE_INTERRUPT                                 PNGDN

//*****************************************************************************
//
// The following are values that can be passed to the config parameter for
// functions: SAPH_configurePHY().
//
//*****************************************************************************
#define SAPH_PHY_CHANNEL_1                                                    0
#define SAPH_PHY_CHANNEL_2                                                    1

//*****************************************************************************
//
// The following are values that can be passed to the config parameter for
// functions: SAPH_configurePHY().
//
//*****************************************************************************
#define SAPH_PHY_OUTPUT_HIGH                                           CH0OUT_1
#define SAPH_PHY_OUTPUT_LOW                                            CH0OUT_0

//*****************************************************************************
//
// The following are values that can be passed to the config parameter for
// functions: SAPH_configurePHY().
//
//*****************************************************************************
#define SAPH_PHY_OUTPUT_ENABLE                                          CH0OE_1
#define SAPH_PHY_OUTPUT_DISABLE                                         CH0OE_0

//*****************************************************************************
//
// The following are values that can be passed to the config parameter for
// functions: SAPH_configurePHY().
//
//*****************************************************************************
#define SAPH_PHY_FULLPULL_ENABLE                                        CH0FP_1
#define SAPH_PHY_FULLPULL_DISABLE                                       CH0FP_0

//*****************************************************************************
//
// The following are values that can be passed to the config parameter for
// functions: SAPH_configurePHY().
//
//*****************************************************************************
#define SAPH_PHY_TERMINATION_ENABLE                                   CH0TERM_1
#define SAPH_PHY_TERMINATION_DISABLE                                  CH0TERM_0

//*****************************************************************************
//
// The following are values that can be passed to the config parameter for
// functions: SAPH_configurePHY().
//
//*****************************************************************************
#define SAPH_PHY_OUTPUT_GENERAL_PURPOSE                               PCH0SEL_0
#define SAPH_PHY_OUTPUT_PULSEGENERATOR_SINGLE_DRIVE                   PCH0SEL_1
#define SAPH_PHY_OUTPUT_PULSEGENERATOR_DIFFERENTIAL_DRIVE             PCH0SEL_2

//*****************************************************************************
//
// The following are values that can be passed to the enableDummyLoad parameter
// for functions: SAPH_configurePHYMultiplexer().
//
//*****************************************************************************
#define SAPH_PHY_DUMMYLOAD_ENABLE                                       DUMEN_1
#define SAPH_PHY_DUMMYLOAD_DISABLE                                      DUMEN_0

//*****************************************************************************
//
// The following are values that can be passed to the sourceControl parameter
// for functions: SAPH_configurePHYMultiplexer().
//
//*****************************************************************************
#define SAPH_PHY_SOURCE_CONTROLLED_BY_REGISTER                         MUXCTL_0
#define SAPH_PHY_SOURCE_CONTROLLED_BY_ASQ                              MUXCTL_1

//*****************************************************************************
//
// The following are values that can be passed to the inputChannel parameter
// for functions: SAPH_configurePHYMultiplexer().
//
//*****************************************************************************
#define SAPH_PHY_INPUT_CHANNEL_0                                  MUXSEL__CH0IN
#define SAPH_PHY_INPUT_CHANNEL_1                                  MUXSEL__CH1IN

//*****************************************************************************
//
// The following are values that can be passed to the config parameter for
// functions: SAPH_configurePHYBias().
//
//*****************************************************************************
#define SAPH_PHY_BIAS_SWITCH_OPEN                                     CH0EBSW_0
#define SAPH_PHY_BIAS_SWITCH_CLOSED                                   CH0EBSW_1

//*****************************************************************************
//
// The following are values that can be passed to the config parameter for
// functions: SAPH_configurePHYBias().
//
//*****************************************************************************
#define SAPH_PHY_PGA_BIAS_GROUNDED                                    PGABIAS_0
#define SAPH_PHY_PGA_BIAS_LOW_VALUE                                   PGABIAS_1
#define SAPH_PHY_PGA_BIAS_NOMINAL_VALUE                               PGABIAS_2
#define SAPH_PHY_PGA_BIAS_HIGH_VALUE                                  PGABIAS_3

//*****************************************************************************
//
// The following are values that can be passed to the config parameter for
// functions: SAPH_configurePHYBias().
//
//*****************************************************************************
#define SAPH_PHY_EXCITATION_BIAS_GROUNDED                             EXCBIAS_0
#define SAPH_PHY_EXCITATION_BIAS_LOW_VALUE                            EXCBIAS_1
#define SAPH_PHY_EXCITATION_BIAS_NOMINAL_VALUE                        EXCBIAS_2
#define SAPH_PHY_EXCITATION_BIAS_HIGH_VALUE                           EXCBIAS_3

//*****************************************************************************
//
// The following are values that can be passed to the config parameter for
// functions: SAPH_configurePHYBias().
//
//*****************************************************************************
#define SAPH_PHY_MULTIPLEXER_CHARGEPUMP_ENABLE                           CPDA_1
#define SAPH_PHY_MULTIPLEXER_CHARGEPUMP_DISABLE                          CPDA_0

//*****************************************************************************
//
// The following are values that can be passed to the config parameter for
// functions: SAPH_configurePHYBias().
//
//*****************************************************************************
#define SAPH_PHY_LEAKAGE_COMPENSATION_ENABLE                             LILC_1
#define SAPH_PHY_LEAKAGE_COMPENSATION_DISABLE                            LILC_0

//*****************************************************************************
//
// The following are values that can be passed to the config parameter for
// functions: SAPH_configurePHYBias().
//
//*****************************************************************************
#define SAPH_PHY_PGA_BIAS_SWITCH_OPEN                                  PGABSW_0
#define SAPH_PHY_PGA_BIAS_SWITCH_CLOSED                                PGABSW_1

//*****************************************************************************
//
// The following are values that can be passed to the config parameter for
// functions: SAPH_configurePHYBias().
//
//*****************************************************************************
#define SAPH_PHY_BIAS_SWITCH_CONTROLLED_BY_REGISTER                    ASQBSC_0
#define SAPH_PHY_BIAS_SWITCH_CONTROLLED_BY_ASQ                         ASQBSC_1

//*****************************************************************************
//
// The following are values that can be passed to the config parameter for
// functions: SAPH_configurePPGCount().
//
//*****************************************************************************
#define SAPH_PPG_HIGH_IMPEDANCE_ON_PAUSE_HIGH                            PHIZ_1
#define SAPH_PPG_HIGH_IMPEDANCE_ON_PAUSE_LOW                             PHIZ_0

//*****************************************************************************
//
// The following are values that can be passed to the config parameter for
// functions: SAPH_configurePPGCount().
//
//*****************************************************************************
#define SAPH_PPG_PAUSE_LEVEL_HIGH                                        PLEV_1
#define SAPH_PPG_PAUSE_LEVEL_LOW                                         PLEV_0

//*****************************************************************************
//
// The following are values that can be passed to the config parameter for
// functions: SAPH_configurePPGCount().
//
//*****************************************************************************
#define SAPH_PPG_PAUSE_POLARITY_HIGH                                     PPOL_0
#define SAPH_PPG_PAUSE_POLARITY_LOW                                      PPOL_1

//*****************************************************************************
//
// The following are values that can be passed to the config parameter for
// functions: SAPH_configurePPG().
//
//*****************************************************************************
#define SAPH_PPG_PRESCALER_ENABLE                                       PSCEN_1
#define SAPH_PPG_PRESCALER_DISABLE                                      PSCEN_0

//*****************************************************************************
//
// The following are values that can be passed to the config parameter for
// functions: SAPH_configurePPG().
//
//*****************************************************************************
#define SAPH_PPG_TRIGGER_SOURCE_SOFTWARE                                TRSEL_0
#define SAPH_PPG_TRIGGER_SOURCE_ASQ                                     TRSEL_1
#define SAPH_PPG_TRIGGER_SOURCE_TIMER                                   TRSEL_2

//*****************************************************************************
//
// The following are values that can be passed to the config parameter for
// functions: SAPH_configurePPG().
//
//*****************************************************************************
#define SAPH_PPG_CHANNEL_0                                           PPGCHSEL_0
#define SAPH_PPG_CHANNEL_1                                           PPGCHSEL_1

//*****************************************************************************
//
// The following are values that can be passed to the config parameter for
// functions: SAPH_configurePPG().
//
//*****************************************************************************
#define SAPH_PPG_PORT_CHARGED_BY_PPG                                    PGSEL_0
#define SAPH_PPG_PORT_CHARGED_BY_ASEQ                                   PGSEL_1

//*****************************************************************************
//
// The following are values that can be passed to the config parameter for
// functions: SAPH_configureASQ().
//
//*****************************************************************************
#define SAPH_ASQ_ABORT_ON_ERROR_ENABLE                                 ERABRT_1
#define SAPH_ASQ_ABORT_ON_ERROR_DISABLE                                ERABRT_0

//*****************************************************************************
//
// The following are values that can be passed to the config parameter for
// functions: SAPH_configureASQ().
//
//*****************************************************************************
#define SAPH_ASQ_TRIGGER_SOURCE_SOFTWARE                        TRIGSEL__SWTRIG
#define SAPH_ASQ_TRIGGER_SOURCE_P_SEQUENCER                        TRIGSEL__PSQ
#define SAPH_ASQ_TRIGGER_SOURCE_TIMER                            TRIGSEL__TIMER

//*****************************************************************************
//
// The following are values that can be passed to the config parameter for
// functions: SAPH_configureASQ().
//
//*****************************************************************************
#define SAPH_ASQ_CHANNEL_0                                           ASQCHSEL_0
#define SAPH_ASQ_CHANNEL_1                                           ASQCHSEL_1

//*****************************************************************************
//
// The following are values that can be passed to the config parameter for
// functions: SAPH_configureASQ().
//
//*****************************************************************************
#define SAPH_ASQ_DIFFERENT_CHANNEL                                      CHOWN_0
#define SAPH_ASQ_IDENTICAL_CHANNEL                                      CHOWN_1

//*****************************************************************************
//
// The following are values that can be passed to the config parameter for
// functions: SAPH_configureASQ().
//
//*****************************************************************************
#define SAPH_ASQ_STANDBY_INDICATION_POWEROFF                      STDBY__PWROFF
#define SAPH_ASQ_STANDBY_INDICATION_STANDBY                        STDBY__STDBY

//*****************************************************************************
//
// The following are values that can be passed to the config parameter for
// functions: SAPH_configureASQ().
//
//*****************************************************************************
#define SAPH_ASQ_END_OF_SEQUENCE_OFF_DISABLE                            ESOFF_0
#define SAPH_ASQ_END_OF_SEQUENCE_OFF_ENABLE                             ESOFF_1

//*****************************************************************************
//
// The following are values that can be passed to the config parameter for
// functions: SAPH_configureASQ().
//
//*****************************************************************************
#define SAPH_ASQ_EARLY_RECEIVE_BIAS_BY_TIMEMARK_C                     EARLYRB_0
#define SAPH_ASQ_EARLY_RECEIVE_BIAS_BY_TIMEMARK_A                     EARLYRB_1

//*****************************************************************************
//
// The following are values that can be passed to the config parameter for
// functions: SAPH_configureASQ().
//
//*****************************************************************************
#define SAPH_ASQ_TOGGLE_CHANNEL_ENABLE                                  CHTOG_1
#define SAPH_ASQ_TOGGLE_CHANNEL_DISABLE                                 CHTOG_0

//*****************************************************************************
//
// The following are values that can be passed to the config parameter for
// functions: SAPH_configureMode().
//
//*****************************************************************************
#define SAPH_LOW_POWER_BIAS_MODE_DISABLE                                LPBE_0
#define SAPH_LOW_POWER_BIAS_MODE_ENABLE                                 LPBE_1

//*****************************************************************************
//
// The following are values that can be passed to the config parameter for
// functions: SAPH_configureMode().
//
//*****************************************************************************
#define SAPH_CHARGE_PUMP_ON_SDHS_ASQ_REQUESTS_ONLY                      CPEO_0
#define SAPH_CHARGE_PUMP_ON_ALWAYS                                      CPEO_1

//*****************************************************************************
//
// The following are values that can be passed to the config parameter for
// functions: SAPH_configureMode().
//
//*****************************************************************************
#define SAPH_MCNF_500_OHMS_RXBIAS_450_OHMS_TXBIAS                       BIMP_0
#define SAPH_MCNF_900_OHMS_RXBIAS_850_OHMS_TXBIAS                       BIMP_1
#define SAPH_MCNF_1500_OHMS_RXBIAS_1450_OHMS_TXBIAS                     BIMP_2
#define SAPH_MCNF_2950_OHMS_RXBIAS_2900_OHMS_TXBIAS                     BIMP_3

//*****************************************************************************
//
// Prototypes for the APIs.
//
//*****************************************************************************

//*****************************************************************************
//
//! \brief Unlocks certain SAPH registers.
//!
//! Unlock SAPH registers with offset address of 0xF or higher. IIDX, MIS, RIS,
//! IMSC, ICR, ISR, DESCLO, and DESCHI registers are not affected and the
//! registers are not locked. Utilize this function if those registers need to
//! be modified.
//! \n\b NOTE: Most of the SAPH APIs will not work unless SAPH is unlocked.
//!
//! \param baseAddress is the base address of the SAPH module.
//!
//! \return None
//
//*****************************************************************************
extern void SAPH_unlock(uint16_t baseAddress);

//*****************************************************************************
//
//! \brief Locks certain SAPH registers.
//!
//! Locks SAPH registers with offset address of 0xF or higher. IIDX, MIS, RIS,
//! IMSC, ICR, ISR, DESCLO, and DESCHI registers are not affected and the
//! registers are not locked. Utilize this function after SAPH_unlock()
//! after necessary registers have been modified.
//! SAPH registers, with the exception of the list in paragraph above, are
//! locked by default.
//!
//! \param baseAddress is the base address of the SAPH module.
//!
//! \return None
//
//*****************************************************************************
extern void SAPH_lock(uint16_t baseAddress);

//*****************************************************************************
//
//! \brief Clears SAPH interrupts.
//!
//! The SAPH interrupt source is cleared, so that it no longer asserts. If the
//! interrupt is not enabled, the corresponding bit in the RIS register bit will
//! be cleared. If the interrupt is enabled, the corresponding bit both MIS and 
//! RIS registers will be both cleared.
//!
//! \param baseAddress is the base address of the SAPH module.
//! \param mask is the logical OR of any of the following:
//!        - \b SAPH_DATA_ERROR_INTERRUPT - SAPH data error interrupt
//!        - \b SAPH_TIMEMARK_F_TIMEOUT_INTERRUPT - SAPH TMF time out interrupt
//!        - \b SAPH_ACQUISITION_SEQUENCER_DONE_INTERRUPT - SAPH acquistion sequencer done interrupt
//!        - \b SAPH_PING_TRANSMIT_DONE_INTERRUPT - SAPH ping transmit done interrupt
//!
//! \return None
//
//*****************************************************************************
extern void SAPH_clearInterrupt(uint16_t baseAddress, uint8_t mask);

//*****************************************************************************
//
//! \brief Enables SAPH interrupts.
//!
//! Enables the indicated SAPH interrupt sources.  Only the sources that are
//! enabled can be reflected to the processor interrupt; disabled sources have
//! no effect on the processor. <b>Does not clear interrupt flags.</b>
//!
//! \param baseAddress is the base address of the SAPH module.
//! \param mask is the logical OR of any of the following:
//!        - \b SAPH_DATA_ERROR_INTERRUPT - SAPH data error interrupt
//!        - \b SAPH_TIMEMARK_F_TIMEOUT_INTERRUPT - SAPH TMF time out interrupt
//!        - \b SAPH_ACQUISITION_SEQUENCER_DONE_INTERRUPT - SAPH acquistion sequencer done interrupt
//!        - \b SAPH_PING_TRANSMIT_DONE_INTERRUPT - SAPH ping transmit done interrupt
//!
//! \return None
//
//*****************************************************************************
extern void SAPH_enableInterrupt(uint16_t baseAddress, uint8_t mask);

//*****************************************************************************
//
//! \brief Disables selected SAPH interrupt sources.
//!
//! Disables the indicated SAPH interrupt sources. Only the sources that are
//! enabled can be reflected to the processor interrupt; disabled sources have
//! no effect on the processor.
//!
//! \param baseAddress is the base address of the SAPH module.
//! \param mask is the logical OR of any of the following:
//!        - \b SAPH_DATA_ERROR_INTERRUPT - SAPH data error interrupt
//!        - \b SAPH_TIMEMARK_F_TIMEOUT_INTERRUPT - SAPH TMF time out interrupt
//!        - \b SAPH_ACQUISITION_SEQUENCER_DONE_INTERRUPT - SAPH acquistion sequencer done interrupt
//!        - \b SAPH_PING_TRANSMIT_DONE_INTERRUPT - SAPH ping transmit done interrupt
//!
//! \return None
//
//*****************************************************************************
extern void SAPH_disableInterrupt(uint16_t baseAddress, uint8_t mask);

//*****************************************************************************
//
//! \brief Gets the current SAPH interrupt status.
//!
//! This returns the interrupt status for the SAPH module based on which flag
//! is passed.
//!
//! \param baseAddress is the base address of the SAPH module.
//! \param mask is the logical OR of any of the following:
//!        - \b SAPH_DATA_ERROR_INTERRUPT - SAPH data error interrupt
//!        - \b SAPH_TIMEMARK_F_TIMEOUT_INTERRUPT - SAPH TMF time out interrupt
//!        - \b SAPH_ACQUISITION_SEQUENCER_DONE_INTERRUPT - SAPH acquistion sequencer done interrupt
//!        - \b SAPH_PING_TRANSMIT_DONE_INTERRUPT - SAPH ping transmit done interrupt
//!
//! \return Logical OR of any of the following:
//!        - \b SAPH_DATA_ERROR_INTERRUPT - SAPH data error interrupt
//!        - \b SAPH_TIMEMARK_F_TIMEOUT_INTERRUPT - SAPH TMF time out interrupt
//!        - \b SAPH_ACQUISITION_SEQUENCER_DONE_INTERRUPT - SAPH acquistion sequencer done interrupt
//!        - \b SAPH_PING_TRANSMIT_DONE_INTERRUPT - SAPH ping transmit done interrupt
//
//*****************************************************************************
extern uint8_t SAPH_getInterruptStatus(uint16_t baseAddress, uint8_t mask);

//*****************************************************************************
//
//! \brief Configures physical interface output and trim values.
//!
//! This configures SAPH physical interface:
//! - channel output 
//! - channel related trim values
//!
//! \param baseAddress is the base address of the SAPH module.
//! \param config is the pointer to the struct for physical interface configuration.
//!
//! \return None
//!
//*****************************************************************************
extern void SAPH_configurePHY(uint16_t baseAddress, SAPH_configPHYParam *config);

//*****************************************************************************
//
//! \brief Configures physical interface multiplexer.
//!
//! This configures SAPH physical interface mutliplexer for dummy load, channel
//! multiplexer source control and input channel selection.
//!
//! \param baseAddress is the base address of the SAPH module.
//! \param enableDummyLoad indicates whether to enable/disable dummy load on non-
//!        selected multiplexer inputs.
//!        Valid values are:
//!        - \b SAPH_PHY_DUMMYLOAD_ENABLE
//!        - \b SAPH_PHY_DUMMYLOAD_DISABLE [Default]
//! \param sourceControl selects which controls the multiplexer.
//!        Valid values are:
//!        - \b SAPH_PHY_SOURCE_CONTROLLED_BY_REGISTER [Default]
//!        - \b SAPH_PHY_SOURCE_CONTROLLED_BY_ASQ
//! \param inputChannel selects input channel.
//!        Valid values are:
//!        - \b SAPH_PHY_INPUT_CHANNEL_0 [Default]
//!        - \b SAPH_PHY_INPUT_CHANNEL_1
//!
//! \return None
//!
//*****************************************************************************
extern void SAPH_configurePHYMultiplexer(uint16_t baseAddress,
        uint16_t enableDummyLoad, uint16_t sourceControl, uint16_t inputChannel);

//*****************************************************************************
//
//! \brief Configures physical interface bias.
//!
//! This configures SAPH physical interface bias parameters:
//! - excitation bias switch
//! - PGA bias
//! - excitation bias
//! - charge pump
//! - line input leakage
//! - compensation
//! - ASQ bias
//!
//! \param baseAddress is the base address of the SAPH module.
//! \param config is the pointer to the struct for physical interface bias configuration.
//!
//! \return None
//!
//*****************************************************************************
extern void SAPH_configurePHYBias(uint16_t baseAddress,
        SAPH_configPHYBiasParam *config);

//*****************************************************************************
//
//! \brief Configures pulse generator count.
//!
//! This configures SAPH pulse generator count:
//! - high impedance
//! - pause level
//! - pause polarity
//! - stop pulse
//! - excitation pulse
//!
//! \param baseAddress is the base address of the SAPH module.
//! \param config is the pointer to the struct for pulse generator count configuration.
//!
//! \return None
//!
//*****************************************************************************
extern void SAPH_configurePPGCount(uint16_t baseAddress,
        SAPH_configPPGCountParam *config);

//*****************************************************************************
//
//! \brief Sets pulse generator low period.
//!
//! The low phase of period of generator pulses defines the length of low phase
//! of the pulses in units of high speed clocks. The minimum count is two regardless
//! of the value set in this register.
//!
//! \param baseAddress is the base address of the SAPH module.
//! \param period is the low period value to set.
//!
//! \return None
//!
//*****************************************************************************
extern void SAPH_setPPGLowPeriod(uint16_t baseAddress, uint16_t period);

//*****************************************************************************
//
//! \brief Sets pulse generator high period.
//!
//! The high phase of period of generator pulses defines the length of high phase
//! of the pulses in units of high speed clocks. The minimum count is two regardless
//! of the value set in this register.
//!
//! \param baseAddress is the base address of the SAPH module.
//! \param period is the high period value to set.
//!
//! \return None
//!
//*****************************************************************************
extern void SAPH_setPPGHighPeriod(uint16_t baseAddress, uint16_t period);

//*****************************************************************************
//
//! \brief Configures pulse generator.
//!
//! This configures SAPH pulse generator:
//! - trigger source
//! - prescaler enable
//! - channel select
//! - control select
//!
//! \param baseAddress is the base address of the SAPH module.
//! \param config is the pointer to the struct for pulse generator configuration.
//!
//! \return None
//!
//*****************************************************************************
extern void SAPH_configurePPG(uint16_t baseAddress, SAPH_configPPGParam *config);

//*****************************************************************************
//
//! \brief Triggers pulse generator.
//!
//! This function triggers the pulse generator if SAPH_PPG_TRIGGER_SOURCE_SOFTWARE
//! is selected as the trigger sourcce.
//!
//! \param baseAddress is the base address of the SAPH module.
//!
//! \return None
//!
//*****************************************************************************
extern void SAPH_triggerPPG(uint16_t baseAddress);

//*****************************************************************************
//
//! \brief Stops pulse generator.
//!
//! \param baseAddress is the base address of the SAPH module.
//!
//! \return None
//!
//*****************************************************************************
extern void SAPH_stopPPG(uint16_t baseAddress);

//*****************************************************************************
//
//! \brief Configures acquisition sequencer.
//!
//! This configures SAPH acquisition sequencer:
//! - trigger source
//! - abort-on-error control
//! - channel select
//! - side of channel to receive
//! - standby indication
//! - OFF at end-of-sequence
//! - early receive bias
//! - channel toggle
//!
//! \param baseAddress is the base address of the SAPH module.
//! \param config is the pointer to the struct for acquisition sequencer configuration.
//!
//! \return None
//!
//*****************************************************************************
extern void SAPH_configureASQ(uint16_t baseAddress, SAPH_configASQParam *config);

//*****************************************************************************
//
//! \brief Triggers acquisition sequencer.
//!
//! This function triggers the acquisition sequencer if SAPH_ASQ_TRIGGER_SOURCE_SOFTWARE
//! is selected as the trigger sourcce.
//!
//! \param baseAddress is the base address of the SAPH module.
//!
//! \return None
//!
//*****************************************************************************
extern void SAPH_triggerASQ(uint16_t baseAddress);

//*****************************************************************************
//
//! \brief Stops acquisition sequencer.
//!
//! \param baseAddress is the base address of the SAPH module.
//!
//! \return None
//!
//*****************************************************************************
extern void SAPH_stopASQ(uint16_t baseAddress);

//*****************************************************************************
//
//! \brief Gets ping counter of acquisition sequencer.
//!
//! This function returns ping counter which represents the number of acquistion
//! turns regardless if up/down direction is changed. The acquistion stops if
//! this count decrements from zero.
//!
//! \param baseAddress is the base address of the SAPH module.
//!
//! \return None
//!
//*****************************************************************************
extern uint8_t SAPH_getASQPingCounter(uint16_t baseAddress);

//*****************************************************************************
//
//! \brief Configures acquisition sequencer ping parameters.
//!
//! This configures SAPH acquisition sequencer ping:
//! - output polarity
//! - pause level
//! - pause impedance
//!
//! \param baseAddress is the base address of the SAPH module.
//! \param config is the pointer to the struct for acquisition sequencer ping 
//!        configuration.
//!
//! \return None
//!
//*****************************************************************************
extern void SAPH_configureASQPing(uint16_t baseAddress,
        SAPH_configASQPingParam *config);

//*****************************************************************************
//
//! \brief Configures acquisition sequencer time base parameters.
//!
//! This configures SAPH acquisition sequencer time base parameters:
//! - power state
//! - prescaler start value
//!
//! \param baseAddress is the base address of the SAPH module.
//! \param prescalerStartValue is the ASQ prescaler start value.
//!
//! \return None
//!
//*****************************************************************************
extern void SAPH_configureASQTimeBase(uint16_t baseAddress,
        uint16_t prescalerStartValue);

//*****************************************************************************
//
//! \brief Starts acquisition timer.
//!
//! \param baseAddress is the base address of the SAPH module.
//!
//! \return None
//!
//*****************************************************************************
extern void SAPH_startASQTimer(uint16_t baseAddress);

//*****************************************************************************
//
//! \brief Stops acquisition timer.
//!
//! \param baseAddress is the base address of the SAPH module.
//!
//! \return None
//!
//*****************************************************************************
extern void SAPH_stopASQTimer(uint16_t baseAddress);

//*****************************************************************************
//
//! \brief Clears acquisition timer.
//!
//! \param baseAddress is the base address of the SAPH module.
//!
//! \return None
//!
//*****************************************************************************
extern void SAPH_clearASQTimer(uint16_t baseAddress);

//*****************************************************************************
//
//! \brief Configures certain mode configuration parameters.
//!
//! This configures SAPH:
//! - bias generator impedance
//! - charge pump enable timing
//! - low power bias mode
//! \n\b NOTE: This will modify all elements of SAPH mode configuration register. If there are elements not specified in the config parameter, the register values will be cleared. There are other SAPH functions to modify these features individually.
//!
//! \param baseAddress is the base address of the SAPH module.
//! \param config is the pointer to the struct for mode configuration.
//!
//! \return None
//!
//*****************************************************************************
extern void SAPH_configureMode(uint16_t baseAddress,
        SAPH_configModeParam *config);

//*****************************************************************************
//
//! \brief Enables low power bias operation mode.
//!
//! In this mode, the SAPH acquisition sequencer (ASQ) uses the CHxEBSW and PGABSW as auxiliary values to achieve faster channel setting on reactive input loads. The ASQ has full controls over the bias switch multiplexer.
//! \n\b NOTE: Avoid changing operation mode unless PSQ is in OFF state. Otherwise, it can cause corrupt measurement results and irregular triggers of sub modules by ASQ.
//!
//! \param baseAddress is the base address of the SAPH module.
//!
//! \return None
//!
//*****************************************************************************
extern void SAPH_enableLowPowerBiasMode(uint16_t baseAddress);

//*****************************************************************************
//
//! \brief Disables low power bias operation mode.
//!
//! Enters manual bias mode and regular ASQ bias mode. In this configuration, the user controls by the ASQBSW has full control over TxBias and RxBias switches.
//! \n\b NOTE: Avoid changing operation mode unless PSQ is in OFF state. Otherwise, it can cause corrupt measurement results and irregular triggers of sub modules by ASQ.
//!
//! \param baseAddress is the base address of the SAPH module.
//!
//! \return None
//!
//*****************************************************************************
extern void SAPH_disableLowPowerBiasMode(uint16_t baseAddress);

//*****************************************************************************
//
//! \brief Keeps charge pump of the input multiplexer always on.
//!
//! The charge pump of the input multiplexer is on regardless of SDHS and ASQ requests. This setting will override other USS charge pump settings elsewhere.
//!
//! \param baseAddress is the base address of the SAPH module.
//!
//! \return None
//!
//*****************************************************************************
extern void SAPH_enableChargePumpAlways(uint16_t baseAddress);

//*****************************************************************************
//
//! \brief Charge pump of input multiplexer is only turned on by data acquisition.
//!
//! The charge pump of the input multiplexer is turned on by SDHS and ASQ related requests only. This will also require the charge pump to be enabled explicitly through other methods [controlled by CPDA bit or SAPH_configurePHYBias()] in the SAPH module.
//!
//! \param baseAddress is the base address of the SAPH module.
//!
//! \return None
//!
//*****************************************************************************
extern void SAPH_enableChargePumpOnAcquisitionOnly(uint16_t baseAddress);

//*****************************************************************************
//
//! \brief Sets impedance of the buffers for RxBias and TxBias.
//!
//! While for resistive loads the lowest impedance shows the fastest settling; this is not the case for reactive loads.
//!
//! \param baseAddress is the base address of the SAPH module.
//! \param biasImpedance sets bias impedance for RxBias and TxBias.
//!        Valid values are:
//!        - \b SAPH_MCNF_500_OHMS_RXBIAS_450_OHMS_TXBIAS
//!        - \b SAPH_MCNF_900_OHMS_RXBIAS_850_OHMS_TXBIAS
//!        - \b SAPH_MCNF_1500_OHMS_RXBIAS_1450_OHMS_TXBIAS [Default]
//!        - \b SAPH_MCNF_2950_OHMS_RXBIAS_2900_OHMS_TXBIAS
//!
//! \return None
//!
//*****************************************************************************
extern void SAPH_setBiasGeneratorImpedance(uint16_t baseAddress, uint16_t biasImpedance);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif
#endif // __MSP430WARE_SAPH_H__

