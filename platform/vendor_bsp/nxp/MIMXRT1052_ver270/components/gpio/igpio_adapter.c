/*
 * Copyright 2018 - 2019 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_device_registers.h"
#include "fsl_gpio.h"
#include "gpio.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief The pin config struct of gpio adapter. */
typedef struct _hal_gpio_pin
{
    uint16_t port : 3U;
    uint16_t pin : 5U;
    uint16_t direction : 1U;
    uint16_t trigger : 3U;
    uint16_t reserved : 4U;
} hal_gpio_pin_t;

typedef struct _hal_gpio_state
{
    struct _hal_gpio_state *next;
    hal_gpio_callback_t callback;
    void *callbackParam;
    hal_gpio_pin_t pin;
} hal_gpio_state_t;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

static void HAL_GpioInterruptHandle(uint8_t port, uint32_t intFlag);

/*******************************************************************************
 * Variables
 ******************************************************************************/

static hal_gpio_state_t *s_GpioHead;

/*******************************************************************************
 * Code
 ******************************************************************************/

static void HAL_GpioInterruptCallback(uint8_t port)
{
    GPIO_Type *gpioList[] = GPIO_BASE_PTRS;
    uint32_t intFlag;

    intFlag = GPIO_PortGetInterruptFlags(gpioList[port]);
    GPIO_PortClearInterruptFlags(gpioList[port], intFlag);

    HAL_GpioInterruptHandle(port, intFlag);
}

static void HAL_GpioInterruptHandle(uint8_t port, uint32_t intFlag)
{
    hal_gpio_state_t *head = s_GpioHead;

    while (head)
    {
        /* Check which triger is ON! */
        if (kHAL_GpioInterruptDisable != head->pin.trigger)
        {
            if ((port == head->pin.port) && (intFlag & (1 << head->pin.pin)))
            {
                if ((NULL != head->callback))
                {
                    head->callback(head->callbackParam);
                }
            }
        }

        head = head->next;
    }
}
/* IRQHandler for GPIO1 */
#if defined(GPIO1) && (FSL_FEATURE_SOC_IGPIO_COUNT > 0U)
void GPIO1_Combined_0_15_IRQHandler(void);
void GPIO1_Combined_0_15_IRQHandler(void)
{
    HAL_GpioInterruptCallback(1);

#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
void GPIO1_Combined_16_31_IRQHandler(void);
void GPIO1_Combined_16_31_IRQHandler(void)
{
    HAL_GpioInterruptCallback(1);

#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif
/* IRQHandler for GPIO2 */
#if defined(GPIO2) && (FSL_FEATURE_SOC_IGPIO_COUNT > 0U)
void GPIO2_Combined_0_15_IRQHandler(void);
void GPIO2_Combined_0_15_IRQHandler(void)
{
    HAL_GpioInterruptCallback(2);

#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
void GPIO2_Combined_16_31_IRQHandler(void);
void GPIO2_Combined_16_31_IRQHandler(void)
{
    HAL_GpioInterruptCallback(2);

#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif
/* IRQHandler for GPIO3 */
#if defined(GPIO3) && (FSL_FEATURE_SOC_IGPIO_COUNT > 0U)
void GPIO3_Combined_0_15_IRQHandler(void);
void GPIO3_Combined_0_15_IRQHandler(void)
{
    HAL_GpioInterruptCallback(3);

#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
void GPIO3_Combined_16_31_IRQHandler(void);
void GPIO3_Combined_16_31_IRQHandler(void)
{
    HAL_GpioInterruptCallback(3);

#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif
/* IRQHandler for GPIO4 */
#if defined(GPIO4) && (FSL_FEATURE_SOC_IGPIO_COUNT > 0U)
void GPIO4_Combined_0_15_IRQHandler(void);
void GPIO4_Combined_0_15_IRQHandler(void)
{
    HAL_GpioInterruptCallback(4);

#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
void GPIO4_Combined_16_31_IRQHandler(void);
void GPIO4_Combined_16_31_IRQHandler(void)
{
    HAL_GpioInterruptCallback(4);

#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif
/* IRQHandler for GPIO5 */
#if defined(GPIO5) && (FSL_FEATURE_SOC_IGPIO_COUNT > 0U)
void GPIO5_Combined_0_15_IRQHandler(void);
void GPIO5_Combined_0_15_IRQHandler(void)
{
    HAL_GpioInterruptCallback(5);

#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
void GPIO5_Combined_16_31_IRQHandler(void);
void GPIO5_Combined_16_31_IRQHandler(void)
{
    HAL_GpioInterruptCallback(5);

#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif
/* IRQHandler for GPIO6 GPIO7 GPIO8 GPIO9 */
#if defined(GPIO6) && defined(GPIO7) && defined(GPIO8) && defined(GPIO9) && (FSL_FEATURE_SOC_IGPIO_COUNT > 8U)
void GPIO6_7_8_9_IRQHandler(void);
void GPIO6_7_8_9_IRQHandler(void)
{
    HAL_GpioInterruptCallback(6);
    HAL_GpioInterruptCallback(7);
    HAL_GpioInterruptCallback(8);
    HAL_GpioInterruptCallback(9);

#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif

static hal_gpio_status_t HAL_GpioConflictSearch(hal_gpio_state_t *head, uint8_t port, uint8_t pin)
{
    while (head)
    {
        if ((head->pin.port == port) && (head->pin.pin == pin))
        {
            return kStatus_HAL_GpioPinConflict;
        }
        head = head->next;
    }
    return kStatus_HAL_GpioSuccess;
}

static hal_gpio_status_t HAL_GpioAddItem(hal_gpio_state_t **head, hal_gpio_state_t *node)
{
    hal_gpio_state_t *p = *head;
    uint32_t regPrimask;

    regPrimask = DisableGlobalIRQ();

    if (NULL == p)
    {
        *head = node;
    }
    else
    {
        while (p->next)
        {
            if (p == node)
            {
                EnableGlobalIRQ(regPrimask);
                return kStatus_HAL_GpioPinConflict;
            }
            p = p->next;
        }

        p->next = node;
    }
    node->next = NULL;
    EnableGlobalIRQ(regPrimask);
    return kStatus_HAL_GpioSuccess;
}

static hal_gpio_status_t HAL_GpioRemoveItem(hal_gpio_state_t **head, hal_gpio_state_t *node)
{
    hal_gpio_state_t *p = *head;
    hal_gpio_state_t *q = NULL;
    uint32_t regPrimask;

    regPrimask = DisableGlobalIRQ();
    while (p)
    {
        if (p == node)
        {
            if (NULL == q)
            {
                *head = p->next;
            }
            else
            {
                q->next = p->next;
            }
            break;
        }
        else
        {
            q = p;
            p = p->next;
        }
    }
    EnableGlobalIRQ(regPrimask);
    return kStatus_HAL_GpioSuccess;
}

hal_gpio_status_t HAL_GpioInit(hal_gpio_handle_t gpioHandle, hal_gpio_pin_config_t *pinConfig)
{
    hal_gpio_state_t *gpioState;
    GPIO_Type *gpioList[]           = GPIO_BASE_PTRS;
    hal_gpio_status_t status        = kStatus_HAL_GpioSuccess;
    gpio_pin_config_t gpioPinconfig = {
        kGPIO_DigitalInput,
        0,
        kGPIO_NoIntmode,
    };

    assert(gpioHandle);
    assert(pinConfig);

    if (HAL_GPIO_HANDLE_SIZE < sizeof(hal_gpio_state_t))
    {
        return kStatus_HAL_GpioError;
    }

    gpioState = (hal_gpio_state_t *)gpioHandle;

    /* Check if the port is vaild */
    if (0 == gpioList[pinConfig->port])
    {
        return kStatus_HAL_GpioError;
    }

    if ((NULL != s_GpioHead) &&
        (kStatus_HAL_GpioSuccess != HAL_GpioConflictSearch(s_GpioHead, pinConfig->port, pinConfig->pin)))
    {
        return kStatus_HAL_GpioPinConflict;
    }

    status = HAL_GpioAddItem(&s_GpioHead, gpioState);
    if (kStatus_HAL_GpioSuccess != status)
    {
        return status;
    }

    gpioState->pin.pin       = pinConfig->pin;
    gpioState->pin.port      = pinConfig->port;
    gpioState->pin.direction = pinConfig->direction;
    gpioState->pin.trigger   = kHAL_GpioInterruptDisable;

    if (kHAL_GpioDirectionOut == pinConfig->direction)
    {
        gpioPinconfig.direction = kGPIO_DigitalOutput;
    }
    else
    {
        gpioPinconfig.direction = kGPIO_DigitalInput;
    }
    gpioPinconfig.outputLogic = pinConfig->level;
    GPIO_PinInit(gpioList[gpioState->pin.port], gpioState->pin.pin, &gpioPinconfig);
    return kStatus_HAL_GpioSuccess;
}

hal_gpio_status_t HAL_GpioDeinit(hal_gpio_handle_t gpioHandle)
{
    hal_gpio_state_t *gpioState;
    GPIO_Type *gpioList[] = GPIO_BASE_PTRS;

    assert(gpioHandle);

    gpioState = (hal_gpio_state_t *)gpioHandle;

    if ((uint16_t)kHAL_GpioDirectionIn == gpioState->pin.direction)
    {
        GPIO_PortDisableInterrupts(gpioList[gpioState->pin.port], (1 << gpioState->pin.pin));
    }

    HAL_GpioRemoveItem(&s_GpioHead, gpioState);
    return kStatus_HAL_GpioSuccess;
}

hal_gpio_status_t HAL_GpioGetInput(hal_gpio_handle_t gpioHandle, uint8_t *pinState)
{
    hal_gpio_state_t *gpioStateHandle;
    GPIO_Type *gpioList[] = GPIO_BASE_PTRS;

    assert(gpioHandle);

    gpioStateHandle = (hal_gpio_state_t *)gpioHandle;

    *pinState = ((GPIO_PinRead(gpioList[gpioStateHandle->pin.port], gpioStateHandle->pin.pin)) ? 1 : 0);
    return kStatus_HAL_GpioSuccess;
}

hal_gpio_status_t HAL_GpioSetOutput(hal_gpio_handle_t gpioHandle, uint8_t pinState)
{
    hal_gpio_state_t *gpioStateHandle;
    GPIO_Type *gpioList[] = GPIO_BASE_PTRS;

    assert(gpioHandle);

    gpioStateHandle = (hal_gpio_state_t *)gpioHandle;

    GPIO_PinWrite(gpioList[gpioStateHandle->pin.port], gpioStateHandle->pin.pin, (pinState) ? 1 : 0);
    return kStatus_HAL_GpioSuccess;
}

hal_gpio_status_t HAL_GpioInstallCallback(hal_gpio_handle_t gpioHandle,
                                          hal_gpio_callback_t callback,
                                          void *callbackParam)
{
    hal_gpio_state_t *gpioStateHandle;

    assert(gpioHandle);

    gpioStateHandle = (hal_gpio_state_t *)gpioHandle;

    gpioStateHandle->callbackParam = callbackParam;
    gpioStateHandle->callback      = callback;

    return kStatus_HAL_GpioSuccess;
}

hal_gpio_status_t HAL_GpioGetTriggerMode(hal_gpio_handle_t gpioHandle, hal_gpio_interrupt_trigger_t *gpioTrigger)
{
    hal_gpio_state_t *gpioStateHandle;

    assert(gpioHandle);

    gpioStateHandle = (hal_gpio_state_t *)gpioHandle;

    if (kHAL_GpioDirectionOut == gpioStateHandle->pin.direction)
    {
        return kStatus_HAL_GpioError;
    }

    *gpioTrigger = (hal_gpio_interrupt_trigger_t)gpioStateHandle->pin.trigger;
    return kStatus_HAL_GpioSuccess;
}

hal_gpio_status_t HAL_GpioSetTriggerMode(hal_gpio_handle_t gpioHandle, hal_gpio_interrupt_trigger_t gpioTrigger)
{
    hal_gpio_state_t *gpioStateHandle;
    GPIO_Type *gpioList[] = GPIO_BASE_PTRS;
    uint32_t regPrimask;
    IRQn_Type gpioLowIRQsList[]  = GPIO_COMBINED_LOW_IRQS;
    IRQn_Type gpioHighIRQsList[] = GPIO_COMBINED_HIGH_IRQS;
    gpio_interrupt_mode_t triggerType;

    assert(gpioHandle);

    gpioStateHandle = (hal_gpio_state_t *)gpioHandle;

    if (kHAL_GpioDirectionOut == gpioStateHandle->pin.direction)
    {
        return kStatus_HAL_GpioError;
    }

    switch (gpioTrigger)
    {
        case kHAL_GpioInterruptLogicZero:
            triggerType = kGPIO_IntLowLevel;
            break;
        case kHAL_GpioInterruptLogicOne:
            triggerType = kGPIO_IntHighLevel;
            break;
        case kHAL_GpioInterruptRisingEdge:
            triggerType = kGPIO_IntRisingEdge;
            break;
        case kHAL_GpioInterruptFallingEdge:
            triggerType = kGPIO_IntFallingEdge;
            break;
        case kHAL_GpioInterruptEitherEdge:
            triggerType = kGPIO_IntRisingOrFallingEdge;
            break;
        default:
            triggerType = kGPIO_NoIntmode;
            break;
    }

    gpioStateHandle->pin.trigger = (uint16_t)gpioTrigger;

    /* Disbale Global Interrupt */
    regPrimask = DisableGlobalIRQ();

    /* initialize gpio interrupt */
    GPIO_PinSetInterruptConfig(gpioList[gpioStateHandle->pin.port], gpioStateHandle->pin.pin, triggerType);

    /* Enable IRQ */
    if (triggerType != kGPIO_NoIntmode)
    {
        GPIO_PortEnableInterrupts(gpioList[gpioStateHandle->pin.port], (1U << gpioStateHandle->pin.pin));
        if (gpioStateHandle->pin.pin <= 15)
        {
            NVIC_SetPriority(gpioLowIRQsList[gpioStateHandle->pin.port], HAL_GPIO_ISR_PRIORITY);
            EnableIRQ(gpioLowIRQsList[gpioStateHandle->pin.port]);
        }
        else
        {
            NVIC_SetPriority(gpioHighIRQsList[gpioStateHandle->pin.port], HAL_GPIO_ISR_PRIORITY);
            EnableIRQ(gpioHighIRQsList[gpioStateHandle->pin.port]);
        }
    }
    else
    {
        GPIO_PortDisableInterrupts(gpioList[gpioStateHandle->pin.port], (1U << gpioStateHandle->pin.pin));
    }

    /* Enable Global Interrupt */
    EnableGlobalIRQ(regPrimask);

    return kStatus_HAL_GpioSuccess;
}

hal_gpio_status_t HAL_GpioWakeUpSetting(hal_gpio_handle_t gpioHandle, uint8_t enable)
{
    hal_gpio_state_t *gpioStateHandle;
    assert(gpioHandle);

    gpioStateHandle = (hal_gpio_state_t *)gpioHandle;

    if (kHAL_GpioDirectionOut == gpioStateHandle->pin.direction)
    {
        return kStatus_HAL_GpioError;
    }

    return kStatus_HAL_GpioSuccess;
}

hal_gpio_status_t HAL_GpioEnterLowpower(hal_gpio_handle_t gpioHandle)
{
    assert(gpioHandle);

    return kStatus_HAL_GpioSuccess;
}

hal_gpio_status_t HAL_GpioExitLowpower(hal_gpio_handle_t gpioHandle)
{
    assert(gpioHandle);

    return kStatus_HAL_GpioSuccess;
}
