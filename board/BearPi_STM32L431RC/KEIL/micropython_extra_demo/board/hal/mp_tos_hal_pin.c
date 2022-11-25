#include "tos_k.h"
#include "mp_tos_hal_pin.h"
#include "gpio.h"

#define ST_EXTI_VECTOR_SIZE  16

#define ST_PORT_NUM(port)   ((uint8_t)((port) >> 4) & 0x0F)
#define ST_PIN_NUM(port)    ((uint8_t)((port) & 0x0F))    

__STATIC__ hal_pin_cb_t hal_pin_irq_handler[ST_EXTI_VECTOR_SIZE];
__STATIC__ void *hal_pin_irq_handler_arg[ST_EXTI_VECTOR_SIZE];
__STATIC__ GPIO_TypeDef *hal_pin_irq_gpio[ST_EXTI_VECTOR_SIZE];

__STATIC__ const uint8_t nvic_irq_channel[ST_EXTI_VECTOR_SIZE] = {
    EXTI0_IRQn,     EXTI1_IRQn,     EXTI2_IRQn,     EXTI3_IRQn,
    EXTI4_IRQn,     EXTI9_5_IRQn,   EXTI9_5_IRQn,   EXTI9_5_IRQn,
    EXTI9_5_IRQn,   EXTI9_5_IRQn,   EXTI15_10_IRQn, EXTI15_10_IRQn,
    EXTI15_10_IRQn, EXTI15_10_IRQn, EXTI15_10_IRQn, EXTI15_10_IRQn,
};

__STATIC__ int mask2no(uint32_t mask) {
    for (int i = 0; i < 32; i++) {
        if ((1U << i) == mask) {
            return i;
        }
    }
    return -1;
}

__API__ int tos_hal_pin_init(hal_pin_t *pin, hal_pin_port_t port, hal_pin_mode_t mode)
{
    GPIO_TypeDef *gpiox;
    uint32_t gpio_pin;
    GPIO_InitTypeDef gpio_init;

    if (!pin) {
        return -1;
    }

    gpiox = (GPIO_TypeDef *)(GPIOA_BASE + ST_PORT_NUM(port) * 0x0400UL);
    gpio_pin = 1UL << ST_PIN_NUM(port);

    gpio_init.Pin = gpio_pin;
    gpio_init.Speed = GPIO_SPEED_FREQ_HIGH;
    switch (mode) {
    case HAL_PIN_MODE_OUTPUT:
        gpio_init.Mode = GPIO_MODE_OUTPUT_PP;
        gpio_init.Pull = GPIO_NOPULL;
        break;
    case HAL_PIN_MODE_OUTPUT_OD:
        gpio_init.Mode = GPIO_MODE_OUTPUT_OD;
        gpio_init.Pull = GPIO_NOPULL;
        break;
    case HAL_PIN_MODE_INPUT:
        gpio_init.Mode = GPIO_MODE_INPUT;
        gpio_init.Pull = GPIO_NOPULL;
        break;
    case HAL_PIN_MODE_INPUT_PULLUP:
        gpio_init.Mode = GPIO_MODE_INPUT;
        gpio_init.Pull = GPIO_PULLUP;
        break;
    case HAL_PIN_MODE_INPUT_PULLDOWN:
        gpio_init.Mode = GPIO_MODE_INPUT;
        gpio_init.Pull = GPIO_PULLDOWN;
        break;
    default:
        return -1;
    }
    HAL_GPIO_Init((GPIO_TypeDef *)gpiox, &gpio_init);

    pin->port = port;
    pin->private_gpio = gpiox;
    pin->private_pin  = gpio_pin;
    pin->mode = mode;

    return 0;
}

__API__ int tos_hal_pin_irq(hal_pin_t *pin, hal_pin_irq_t irq)
{
    GPIO_TypeDef *gpiox;
    uint32_t gpio_pin;
    uint32_t gpio_pin_num;
    GPIO_InitTypeDef gpio_init;
    TOS_CPU_CPSR_ALLOC();

    if (!pin) {
        return -1;
    }

    if (!pin->private_gpio || !pin->private_pin) {
        return -1;
    }

    gpiox = (GPIO_TypeDef *)pin->private_gpio;
    gpio_pin = (uint32_t)pin->private_pin;
    gpio_pin_num = mask2no(gpio_pin);
    if (gpio_pin_num >= ST_EXTI_VECTOR_SIZE) {
        return -1;
    }
    
    TOS_CPU_INT_DISABLE();
    if (hal_pin_irq_gpio[gpio_pin_num] != NULL && hal_pin_irq_gpio[gpio_pin_num] != gpiox) {
        TOS_CPU_INT_ENABLE();
        return -1;
    }

    if (irq == HAL_PIN_IRQ_NONE) {
        hal_pin_irq_gpio[gpio_pin_num] = NULL;
        hal_pin_irq_handler[gpio_pin_num] = NULL;
        hal_pin_irq_handler_arg[gpio_pin_num] = NULL;
        TOS_CPU_INT_ENABLE();
        HAL_NVIC_DisableIRQ(nvic_irq_channel[gpio_pin_num]);
    } else {
        hal_pin_irq_gpio[gpio_pin_num] = gpiox;
        TOS_CPU_INT_ENABLE();
        gpio_init.Pin = gpio_pin;
        gpio_init.Speed = GPIO_SPEED_FREQ_HIGH;
        gpio_init.Mode = GPIO_MODE_IT_RISING_FALLING;
        switch (irq) {
        case HAL_PIN_IRQ_RISING:
            gpio_init.Mode = GPIO_MODE_IT_RISING;
            break;
        case HAL_PIN_IRQ_FALLING:
            gpio_init.Mode = GPIO_MODE_IT_FALLING;
            break;
        case HAL_PIN_IRQ_RISING_FALLING:
            gpio_init.Mode = GPIO_MODE_IT_RISING_FALLING;
            break;
        }
        switch (pin->mode) {
            case HAL_PIN_MODE_INPUT_PULLUP:
                gpio_init.Pull = GPIO_PULLUP;
                break;
            case HAL_PIN_MODE_INPUT_PULLDOWN:
                gpio_init.Pull = GPIO_PULLDOWN;
                break;
            default:
                gpio_init.Pull = GPIO_NOPULL;
                break;
        }
        HAL_GPIO_Init(gpiox, &gpio_init);
        HAL_NVIC_SetPriority(nvic_irq_channel[gpio_pin_num], 2, 0);
        HAL_NVIC_EnableIRQ(nvic_irq_channel[gpio_pin_num]);

        pin->irq = irq;
    }

    return 0;
}

__API__ int tos_hal_pin_irq_callback(hal_pin_t *pin, hal_pin_cb_t callback, void *cb_arg)
{
    GPIO_TypeDef *gpiox;
    uint32_t gpio_pin;
    uint32_t gpio_pin_num;
    TOS_CPU_CPSR_ALLOC();

    if (!pin) {
        return -1;
    }

    if (!pin->private_gpio || !pin->private_pin) {
        return -1;
    }

    gpiox = (GPIO_TypeDef *)pin->private_gpio;
    gpio_pin = (uint32_t)pin->private_pin;
    gpio_pin_num = mask2no(gpio_pin);
    if (gpio_pin_num >= ST_EXTI_VECTOR_SIZE) {
        return -1;
    }

    TOS_CPU_INT_DISABLE();
    if (hal_pin_irq_gpio[gpio_pin_num] != NULL && hal_pin_irq_gpio[gpio_pin_num] != gpiox) {
        TOS_CPU_INT_ENABLE();
        return -1;
    }
    hal_pin_irq_handler[gpio_pin_num] = gpiox;
    hal_pin_irq_handler[gpio_pin_num] = callback;
    hal_pin_irq_handler_arg[gpio_pin_num] = cb_arg;
    TOS_CPU_INT_ENABLE();

    return 0;
}

__API__ int tos_hal_pin_write(hal_pin_t *pin, int state)
{
    GPIO_TypeDef *gpiox;
    uint32_t gpio_pin;

    if (!pin) {
        return -1;
    }

    if (!pin->private_gpio || !pin->private_pin) {
        return -1;
    }

    gpiox = (GPIO_TypeDef *)pin->private_gpio;
    gpio_pin = (uint32_t)pin->private_pin;

    HAL_GPIO_WritePin(gpiox, gpio_pin, (state == 0) ? GPIO_PIN_RESET : GPIO_PIN_SET);

    return 0;
}

__API__ int tos_hal_pin_read(hal_pin_t *pin)
{
    GPIO_TypeDef *gpiox;
    uint32_t gpio_pin;

    if (!pin) {
        return -1;
    }

    if (!pin->private_gpio || !pin->private_pin) {
        return -1;
    }

    gpiox = (GPIO_TypeDef *)pin->private_gpio;
    gpio_pin = (uint32_t)pin->private_pin;

    return (HAL_GPIO_ReadPin(gpiox, gpio_pin) == GPIO_PIN_RESET) ? 0 : 1;
}

__API__ int tos_hal_pin_deinit(hal_pin_t *pin)
{
    GPIO_TypeDef *gpiox;
    uint32_t gpio_pin;
    uint32_t gpio_pin_num;
    TOS_CPU_CPSR_ALLOC();

    if (!pin) {
        return -1;
    }

    if (!pin->private_gpio || !pin->private_pin) {
        return -1;
    }

    gpiox = (GPIO_TypeDef *)pin->private_gpio;
    gpio_pin = (uint32_t)pin->private_pin;
    gpio_pin_num = mask2no(gpio_pin);

    TOS_CPU_INT_DISABLE();
    if (hal_pin_irq_gpio[gpio_pin_num] == gpiox) {
        hal_pin_irq_gpio[gpio_pin_num] = NULL;
        hal_pin_irq_handler[gpio_pin_num] = NULL;
        hal_pin_irq_handler_arg[gpio_pin_num] = NULL;
    }
    TOS_CPU_INT_ENABLE();
    HAL_GPIO_DeInit(gpiox, gpio_pin);

    return 0;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    uint32_t gpio_pin_num = mask2no(GPIO_Pin);
    hal_pin_cb_t callback;
    uint32_t cb_arg;
    if (gpio_pin_num < ST_EXTI_VECTOR_SIZE) {

        callback = hal_pin_irq_handler[gpio_pin_num];
        cb_arg = hal_pin_irq_handler_arg[gpio_pin_num];
        if (callback) {
            callback(cb_arg);
        }
    }
}


