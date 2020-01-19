/*
 * Copyright 2018 - 2019 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_common.h"
#include "gpio.h"
#include "timer_manager.h"

#include "led.h"
#if (defined(LED_DIMMING_ENABLEMENT) && (LED_DIMMING_ENABLEMENT > 0U))
#include "pwm.h"
#endif

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define LED_ENTER_CRITICAL() uint32_t regPrimask = DisableGlobalIRQ();
#define LED_EXIT_CRITICAL() EnableGlobalIRQ(regPrimask);

/* LED control type enumeration */
typedef enum _led_control_type
{
    kLED_TurnOffOn = 0x01U, /*!< Turn Off or on*/
    kLED_Flash,             /*!< Flash */
#if (defined(LED_COLOR_WHEEL_ENABLEMENT) && (LED_COLOR_WHEEL_ENABLEMENT > 0U))
    kLED_TricolorCycleFlash, /*!< Tricolor Cycle Flash */
    kLED_CycleFlash,         /*!< Cycle Flash */
#endif                       /* (defined(LED_COLOR_WHEEL_ENABLEMENT) && (LED_COLOR_WHEEL_ENABLEMENT > 0U)) */
    kLED_Dimming,            /*!< Dimming */
} led_control_type_t;

/* LED Dimming state structure when dimming is enabled */
typedef struct _led_dimming
{
    uint8_t increasement;
    uint8_t powerDelta[sizeof(led_config_t) / sizeof(led_pin_config_t)];
} led_dimming_t;

/*! @brief The pin config struct of LED */
typedef struct _led_pin
{
    union
    {
        struct
        {
            uint16_t type : 2U;          /*!< LED type, 1 - RGB, 2 - Monochrome */
            uint16_t dimmingEnable : 1U; /*!< dimming enable, 0 - disable, 1 - enable */
            uint16_t : 13U;
        } config;
        struct
        {
            uint16_t : 3U;
            uint16_t port : 3U; /*!< GPIO Port */
            uint16_t : 1U;
            uint16_t pin : 5U;             /*!< GPIO Pin */
            uint16_t pinStateDefault : 1U; /*!< GPIO Pin voltage when LED is off (0 - low level, 1 - high level)*/
            uint16_t : 3U;
        } gpio;
        struct
        {
            uint16_t : 3U;
            uint16_t instance : 3U; /*!< PWM instance of the pin */
            uint16_t : 1U;
            uint16_t channel : 5U;         /*!< PWM channel of the pin */
            uint16_t pinStateDefault : 1U; /*!< The Pin voltage when LED is off (0 - low level, 1 - high level)*/
            uint16_t : 3U;
        } dimming;
    };
} led_pin_t;

/* LED state structure */
typedef struct _led_state
{
    struct _led_state *next;
    uint8_t gpioHandle[sizeof(led_config_t) / sizeof(led_pin_config_t)][HAL_GPIO_HANDLE_SIZE];
    volatile uint32_t expiryPeriodCount;
#if (defined(LED_DIMMING_ENABLEMENT) && (LED_DIMMING_ENABLEMENT > 0U))
    uint8_t pwmHandle[sizeof(led_config_t) / sizeof(led_pin_config_t)][HAL_PWM_HANDLE_SIZE];
#endif
    uint32_t flashCycle;
    led_color_t settingColor;
    led_color_t currentColor;
    led_color_t nextColor;
    led_pin_t pins[sizeof(led_config_t) / sizeof(led_pin_config_t)];
    uint16_t flashPeriod;
#if (defined(LED_DIMMING_ENABLEMENT) && (LED_DIMMING_ENABLEMENT > 0U))
    led_dimming_t dimming;
#endif
    struct
    {
        uint16_t controlType : 4U;
#if (defined(LED_COLOR_WHEEL_ENABLEMENT) && (LED_COLOR_WHEEL_ENABLEMENT > 0U))
        uint16_t flashCount : 3U;
#endif
        uint16_t : 1U;
        uint16_t flashDuty : 7U;
        uint16_t : 1U;
    };
} led_state_t;

typedef struct _led_list
{
    led_state_t *ledState;
    volatile uint32_t periodCount;
    uint8_t timerHandle[TIMER_HANDLE_SIZE];
} led_list_t;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
static led_list_t s_ledList;

/*******************************************************************************
 * Code
 ******************************************************************************/

static led_status_t LED_SetStatus(led_state_t *ledState, led_color_t color, uint32_t threshold)
{
    led_pin_t *ledRgbPin;
    led_status_t status = kStatus_LED_Success;
    led_color_t colorSet;
    uint8_t count;

    ledState->expiryPeriodCount = s_ledList.periodCount + threshold;
    ledRgbPin                   = (led_pin_t *)ledState->pins;

    if ((uint16_t)kLED_TypeRgb == ledRgbPin->config.type)
    {
        count = sizeof(led_config_t) / sizeof(led_pin_config_t);
    }
    else
    {
        count = 1;
    }

    for (uint8_t i = 0; i < count; i++)
    {
        colorSet = ((color >> (i * 8U)) & (0xFFU));
#if (defined(LED_DIMMING_ENABLEMENT) && (LED_DIMMING_ENABLEMENT > 0U))
        if (ledRgbPin[i].config.dimmingEnable)
        {
            HAL_PwmUpdateDutycycle(ledState->pwmHandle[i], ledRgbPin[i].dimming.channel, kHAL_EdgeAlignedPwm,
                                   colorSet * 100 / 255);
        }
        else
#endif
        {
            if (kStatus_HAL_GpioSuccess !=
                HAL_GpioSetOutput(ledState->gpioHandle[i], (colorSet != 0U) ?
                                                               (1U - (uint8_t)ledRgbPin[i].gpio.pinStateDefault) :
                                                               (uint8_t)ledRgbPin[i].gpio.pinStateDefault))
            {
                status = kStatus_LED_Error;
            }
        }
    }

    return status;
}

static void LED_TimerEvent(void *param)
{
    led_state_t *ledState = (led_state_t *)s_ledList.ledState;
    uint32_t threshold    = 0;
    led_color_t color;
#if (defined(LED_DIMMING_ENABLEMENT) && (LED_DIMMING_ENABLEMENT > 0U))
    uint16_t power[sizeof(led_config_t) / sizeof(led_pin_config_t)];
    uint8_t count = sizeof(led_config_t) / sizeof(led_pin_config_t);
#endif

    s_ledList.periodCount += LED_TIMER_INTERVAL;

    while (NULL != ledState)
    {
        if (s_ledList.periodCount >= ledState->expiryPeriodCount)
        {
            switch (ledState->controlType)
            {
                case (uint16_t)kLED_Flash: /*!< Flash */
#if (defined(LED_COLOR_WHEEL_ENABLEMENT) && (LED_COLOR_WHEEL_ENABLEMENT > 0U))
                case (uint16_t)kLED_TricolorCycleFlash: /*!< Tricolor Cycle Flash */
                case (uint16_t)kLED_CycleFlash:         /*!< Cycle Flash */
#endif /* (defined(LED_COLOR_WHEEL_ENABLEMENT) && (LED_COLOR_WHEEL_ENABLEMENT > 0U)) */
                    if (LED_FLASH_CYCLE_FOREVER != ledState->flashCycle)
                    {
                        if ((0U != ledState->flashCycle))
                        {
                            if (((led_color_t)kLED_Black == ledState->currentColor) || (100U == ledState->flashDuty))
                            {
                                ledState->flashCycle--;
                            }
                        }
                    }
                    if (0U != ledState->flashCycle)
                    {
                        if ((100U > ledState->flashDuty))
                        {
                            color                  = ledState->nextColor;
                            ledState->nextColor    = ledState->currentColor;
                            ledState->currentColor = color;
                        }

                        if (((led_color_t)kLED_Black == ledState->currentColor))
                        {
                            threshold = (uint32_t)ledState->flashPeriod * (100U - (uint32_t)ledState->flashDuty) / 100U;
                        }
                        else
                        {
#if (defined(LED_COLOR_WHEEL_ENABLEMENT) && (LED_COLOR_WHEEL_ENABLEMENT > 0U))
                            ledState->flashCount++;
                            if ((uint16_t)kLED_TricolorCycleFlash == ledState->controlType)
                            {
                                ledState->currentColor = (led_color_t)(0xFFUL << ((ledState->flashCount % 3U) * 8U));
                            }
                            else if ((uint16_t)kLED_CycleFlash == ledState->controlType)
                            {
                                color = 0;
                                if (0U == ((ledState->flashCount) & 0x07U))
                                {
                                    (ledState->flashCount)++;
                                }
                                if (0U != ((ledState->flashCount) & 0x04U))
                                {
                                    color = (led_color_t)0xFF0000;
                                }
                                if (0U != ((ledState->flashCount) & 0x02U))
                                {
                                    color |= (led_color_t)0xFF00;
                                }
                                if (0U != ((ledState->flashCount) & 0x01U))
                                {
                                    color |= (led_color_t)0xFF;
                                }
                                ledState->currentColor = color;
                            }
                            else
                            {
                                /*Misra Rule 15.7*/
                            }
#endif /* (defined(LED_COLOR_WHEEL_ENABLEMENT) && (LED_COLOR_WHEEL_ENABLEMENT > 0U)) */
                            threshold = (uint32_t)ledState->flashPeriod * (uint32_t)(ledState->flashDuty) / 100U;
                        }

                        (void)LED_SetStatus(ledState, ledState->currentColor, threshold);
                    }
                    break;
#if (defined(LED_DIMMING_ENABLEMENT) && (LED_DIMMING_ENABLEMENT > 0U))
                case kLED_Dimming: /*!< Dimming */
                    for (int i = 0; i < count; i++)
                    {
                        int value = (ledState->currentColor >> (8 * i)) & 0xFF;
                        if ((ledState->dimming.increasement))
                        {
                            if ((value + ledState->dimming.powerDelta[i]) < 0xFF)
                            {
                                power[i] = value + ledState->dimming.powerDelta[i];
                            }
                            else
                            {
                                power[i] = 0xFF;
                            }
                        }
                        else
                        {
                            if (value > ledState->dimming.powerDelta[i])
                            {
                                power[i] = value - ledState->dimming.powerDelta[i];
                            }
                            else
                            {
                                power[i] = 0;
                            }
                        }

                        if (kLED_TypeMonochrome == ledState->pins[0].config.type)
                        {
                            break;
                        }
                    }
                    ledState->currentColor = LED_MAKE_COLOR(power[0], power[1], power[2]);
                    LED_SetStatus(ledState, ledState->currentColor, ledState->flashPeriod);
                    break;
#endif
                default:
                    /* MISRA Rule 16.4*/
                    break;
            }
        }
        ledState = ledState->next;
    }
}

led_status_t LED_Init(led_handle_t ledHandle, led_config_t *ledConfig)
{
    led_state_t *ledState;
    led_pin_config_t *ledRgbConfigPin;
    hal_gpio_pin_config_t controlPin;
    uint32_t count;
    uint32_t regPrimask;
    int i;
#if (defined(LED_DIMMING_ENABLEMENT) && (LED_DIMMING_ENABLEMENT > 0U))
    uint8_t rgbFlag        = 0;
    uint8_t rgbDimmingFlag = 0;
#endif

    assert(ledHandle && ledConfig);
    assert(LED_HANDLE_SIZE >= sizeof(led_state_t));

    if (kLED_TypeRgb == ledConfig->type)
    {
        count = sizeof(led_config_t) / sizeof(led_pin_config_t);
    }
    else
    {
        count = 1;
    }

    ledState = (led_state_t *)ledHandle;

    (void)memset(ledHandle, 0, sizeof(led_state_t));

    regPrimask = DisableGlobalIRQ();
    if (NULL == s_ledList.ledState)
    {
        do
        {
            if (kStatus_TimerSuccess != TM_Open((timer_handle_t)s_ledList.timerHandle))
            {
                break;
            }
            if (kStatus_TimerSuccess != TM_InstallCallback(s_ledList.timerHandle, LED_TimerEvent, &s_ledList))
            {
                break;
            }
            if (kStatus_TimerSuccess != TM_Start(s_ledList.timerHandle, kTimerModeIntervalTimer, LED_TIMER_INTERVAL))
            {
                break;
            }
            s_ledList.ledState = ledState;
        } while (false);
    }
    else
    {
        ledState->next     = s_ledList.ledState;
        s_ledList.ledState = ledState;
    }
    EnableGlobalIRQ(regPrimask);

    if (NULL == s_ledList.ledState)
    {
        return kStatus_LED_Error;
    }

    ledState->settingColor = (led_color_t)kLED_White;

    ledRgbConfigPin = (led_pin_config_t *)(void *)(&ledConfig->ledRgb);

    controlPin.direction = kHAL_GpioDirectionOut;
#if (defined(LED_DIMMING_ENABLEMENT) && (LED_DIMMING_ENABLEMENT > 0U))
    for (i = 0; i < (int)count; i++)
    {
        if (ledRgbConfigPin[i].dimmingEnable)
        {
            rgbDimmingFlag = 1;
        }
        else
        {
            rgbFlag = 1;
        }
    }
    if (rgbDimmingFlag && rgbFlag)
    {
        return kStatus_LED_Error;
    }
#endif
    for (i = 0; i < (int)count; i++)
    {
        ledState->pins[i].config.type = (uint16_t)ledConfig->type;
#if (defined(LED_DIMMING_ENABLEMENT) && (LED_DIMMING_ENABLEMENT > 0U))
        if (ledRgbConfigPin[i].dimmingEnable)
        {
            hal_pwm_setup_config_t setupConfig;

            ledState->pins[i].config.dimmingEnable    = ledRgbConfigPin[i].dimmingEnable;
            ledState->pins[i].dimming.instance        = ledRgbConfigPin[i].dimming.instance;
            ledState->pins[i].dimming.channel         = ledRgbConfigPin[i].dimming.channel;
            ledState->pins[i].dimming.pinStateDefault = ledRgbConfigPin[i].dimming.pinStateDefault;
            HAL_PwmInit((hal_pwm_handle_t)ledState->pwmHandle[i], ledRgbConfigPin[i].dimming.instance,
                        ledRgbConfigPin[i].dimming.sourceClock);
            setupConfig.dutyCyclePercent = 0;
            setupConfig.level      = ledRgbConfigPin[i].dimming.pinStateDefault ? kHAL_PwmLowTrue : kHAL_PwmHighTrue;
            setupConfig.mode       = kHAL_EdgeAlignedPwm;
            setupConfig.pwmFreq_Hz = 1000;
            HAL_PwmSetupPwm(ledState->pwmHandle[i], ledRgbConfigPin[i].dimming.channel, &setupConfig);
        }
        else
#endif
        {
            ledState->pins[i].gpio.port            = ledRgbConfigPin[i].gpio.port;
            ledState->pins[i].gpio.pin             = ledRgbConfigPin[i].gpio.pin;
            ledState->pins[i].gpio.pinStateDefault = ledRgbConfigPin[i].gpio.pinStateDefault;
            controlPin.port                        = ledRgbConfigPin[i].gpio.port;
            controlPin.pin                         = ledRgbConfigPin[i].gpio.pin;
            controlPin.level                       = ledRgbConfigPin[i].gpio.pinStateDefault;
            if (kStatus_HAL_GpioSuccess != HAL_GpioInit((hal_gpio_handle_t)ledState->gpioHandle[i], &controlPin))
            {
                return kStatus_LED_Error;
            }
        }
    }

    return LED_TurnOnOff(ledState, 0);
}

led_status_t LED_Deinit(led_handle_t ledHandle)
{
    led_state_t *ledState;
    led_state_t *ledStatePre;
    uint32_t regPrimask;
#if (defined(LED_DIMMING_ENABLEMENT) && (LED_DIMMING_ENABLEMENT > 0U))
    led_pin_t *ledRgbPin;
#endif

    assert(ledHandle);

    ledState = (led_state_t *)ledHandle;

    regPrimask  = DisableGlobalIRQ();
    ledStatePre = s_ledList.ledState;
    if (ledStatePre != ledState)
    {
        while ((NULL != ledStatePre) && (ledStatePre->next != ledState))
        {
            ledStatePre = ledStatePre->next;
        }
        if (NULL != ledStatePre)
        {
            ledStatePre->next = ledState->next;
        }
    }
    else
    {
        s_ledList.ledState = ledState->next;
    }

    if (NULL == s_ledList.ledState)
    {
        (void)TM_Close(s_ledList.timerHandle);
    }
    EnableGlobalIRQ(regPrimask);

#if (defined(LED_DIMMING_ENABLEMENT) && (LED_DIMMING_ENABLEMENT > 0U))
    ledRgbPin = (led_pin_t *)(ledState->pins);
#endif
    for (uint32_t i = 0; i < (sizeof(led_config_t) / sizeof(led_pin_config_t)); i++)
    {
#if (defined(LED_DIMMING_ENABLEMENT) && (LED_DIMMING_ENABLEMENT > 0U))
        if (ledRgbPin[i].config.dimmingEnable)
        {
            HAL_PwmDeinit(ledState->pwmHandle[i]);
        }
        else
#endif
        {
            (void)HAL_GpioDeinit(ledState->gpioHandle[i]);
        }
        if (((uint16_t)kLED_TypeRgb != ledState->pins[i].config.type))
        {
            break;
        }
    }

    return kStatus_LED_Success;
}

led_status_t LED_TurnOnOff(led_handle_t ledHandle, uint8_t turnOnOff)
{
    led_state_t *ledState;
    led_status_t status;

    assert(ledHandle);

    ledState               = (led_state_t *)ledHandle;
    ledState->controlType  = (uint16_t)kLED_TurnOffOn;
    ledState->currentColor = turnOnOff ? ledState->settingColor : (led_color_t)kLED_Black;
    status                 = LED_SetStatus(ledState, ledState->currentColor, 0);
    return status;
}

led_status_t LED_SetColor(led_handle_t ledHandle, led_color_t ledRgbColor)
{
    led_state_t *ledState;

    assert(ledHandle);

    ledState = (led_state_t *)ledHandle;

    if (((uint16_t)kLED_TypeRgb != ledState->pins[0].config.type))
    {
        return kStatus_LED_Error;
    }

    ledState->settingColor = ledRgbColor;

    return kStatus_LED_Success;
}

led_status_t LED_Flash(led_handle_t ledHandle, led_flash_config_t *ledFlash)
{
    led_state_t *ledState;
    led_status_t status;

    assert(ledHandle);
    assert(ledFlash);
    assert(ledFlash->times);
    assert(ledFlash->duty <= 100);

    ledState = (led_state_t *)ledHandle;

    ledState->flashPeriod = ledFlash->period;
    ledState->flashDuty   = ledFlash->duty;

    ledState->currentColor = ledState->settingColor;
    ledState->flashCycle   = ledFlash->times;
    ledState->nextColor    = (led_color_t)kLED_Black;

#if (defined(LED_COLOR_WHEEL_ENABLEMENT) && (LED_COLOR_WHEEL_ENABLEMENT > 0U))
    if (kLED_FlashOneColor == ledFlash->flashType)
    {
        ledState->controlType = (uint16_t)kLED_Flash;
    }
    else
    {
        if (((uint16_t)kLED_TypeRgb != ledState->pins[0].config.type))
        {
            return kStatus_LED_Error;
        }
        ledState->controlType = (uint16_t)kLED_CycleFlash;
    }
#else
    ledState->controlType = (uint16_t)kLED_Flash;
#endif /* (defined(LED_COLOR_WHEEL_ENABLEMENT) && (LED_COLOR_WHEEL_ENABLEMENT > 0U)) */
    status = LED_SetStatus(ledState, ledState->currentColor,
                           ((uint32_t)ledState->flashPeriod * (uint32_t)ledState->flashDuty) / 100U);
    return status;
}

led_status_t LED_Blip(led_handle_t ledHandle)
{
    led_flash_config_t ledFlash;

    ledFlash.duty      = 50;
    ledFlash.flashType = kLED_FlashOneColor;
    ledFlash.period    = LED_BLIP_INTERVAL;
    ledFlash.times     = 1;
    return LED_Flash(ledHandle, &ledFlash);
}

led_status_t LED_Dimming(led_handle_t ledHandle, uint16_t dimmingPeriod, uint8_t increasement)
{
#if (defined(LED_DIMMING_ENABLEMENT) && (LED_DIMMING_ENABLEMENT > 0U))
    led_state_t *ledState;
    led_status_t status;
    int i;
    uint16_t power[sizeof(led_config_t) / sizeof(led_pin_config_t)];
    uint8_t value;
    uint8_t count = sizeof(led_config_t) / sizeof(led_pin_config_t);

    assert(ledHandle);
    assert(dimmingPeriod);

    ledState = (led_state_t *)ledHandle;

    if (!ledState->pins[0].config.dimmingEnable)
    {
        return kStatus_LED_Error;
    }

    LED_ENTER_CRITICAL();

    ledState->controlType          = kLED_Dimming;
    ledState->flashPeriod          = LED_DIMMING_UPDATE_INTERVAL;
    ledState->flashDuty            = 100;
    ledState->dimming.increasement = increasement;

    if ((kLED_TypeRgb == ledState->pins[0].config.type))
    {
        ledState->currentColor = ledState->settingColor;
    }

    for (i = 0; i < count; i++)
    {
        ledState->dimming.powerDelta[i] = 0;
    }

    for (i = 0; i < count; i++)
    {
        value = (ledState->currentColor >> (8 * (i))) & 0xFF;
        if (ledState->dimming.increasement)
        {
            ledState->dimming.powerDelta[i] = (0xFF - value) * LED_DIMMING_UPDATE_INTERVAL / (dimmingPeriod);
            if ((value + ledState->dimming.powerDelta[i]) < 0xFF)
            {
                power[i] = value + ledState->dimming.powerDelta[i];
            }
            else
            {
                power[i] = 0xFF;
            }
        }
        else
        {
            ledState->dimming.powerDelta[i] = (value)*LED_DIMMING_UPDATE_INTERVAL / (dimmingPeriod);
            if (value > ledState->dimming.powerDelta[i])
            {
                power[i] = value - ledState->dimming.powerDelta[i];
            }
            else
            {
                power[i] = 0;
            }
        }

        if (kLED_TypeMonochrome == ledState->pins[0].config.type)
        {
            break;
        }
    }

    ledState->currentColor = LED_MAKE_COLOR(power[0], power[1], power[2]);
    ledState->flashCycle   = LED_FLASH_CYCLE_FOREVER;
    LED_EXIT_CRITICAL();
    status = LED_SetStatus(ledState, ledState->currentColor, ledState->flashPeriod);

    return status;
#else
    return kStatus_LED_Error;
#endif
}

led_status_t LED_EnterLowpower(led_handle_t ledHandle)
{
#if 0
    led_state_t* ledState;
    led_pin_config_t* ledRgbPin;
    led_status_t status;
    int count;

    assert(ledHandle);

    ledState = (led_state_t*)ledHandle;

    ledRgbPin = (led_pin_config_t*)&ledState->config.ledRgb;
    if (kLED_TypeRgb == ledState->config.type)
    {
        count = sizeof(led_config_t) / sizeof(led_pin_config_t);
    }
    else
    {
        count = 1;
    }

    for (int i = 0;i < count; i++)
    {
#if (defined(LED_DIMMING_ENABLEMENT) && (LED_DIMMING_ENABLEMENT > 0U))
        if (ledRgbPin[i].dimmingEnable)
        {
            HAL_PwmEnterLowpower(ledRgbPin[i].dimming.instance, ledRgbPin[i].dimming.channel);
        }
        else
#endif
        {
            HAL_GpioEnterLowpower(ledState->gpioHandle[i]);
        }
    }
#endif
    return kStatus_LED_Success;
}

led_status_t LED_ExitLowpower(led_handle_t ledHandle)
{
#if 0
    led_state_t* ledState;
    led_pin_config_t* ledRgbPin;
    led_status_t status;
    int count;

    assert(ledHandle);

    ledState = (led_state_t*)ledHandle;

    ledRgbPin = (led_pin_config_t*)&ledState->config.ledRgb;
    if (kLED_TypeRgb == ledState->config.type)
    {
        count = sizeof(led_config_t) / sizeof(led_pin_config_t);
    }
    else
    {
        count = 1;
    }

    for (int i = 0;i < count; i++)
    {
#if (defined(LED_DIMMING_ENABLEMENT) && (LED_DIMMING_ENABLEMENT > 0U))
        if (ledRgbPin[i].dimmingEnable)
        {
            HAL_PwmExitLowpower(ledRgbPin[i].dimming.instance, ledRgbPin[i].dimming.channel);
        }
        else
#endif
        {
            HAL_GpioExitLowpower(ledState->gpioHandle[i]);
        }
    }
#endif
    return kStatus_LED_Success;
}
