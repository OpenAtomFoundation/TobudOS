#ifndef _TOS_HAL_PIN_H_
#define _TOS_HAL_PIN_H_

typedef unsigned char hal_pin_port_t;

typedef enum hal_pin_mode_en {
    HAL_PIN_MODE_OUTPUT = 0,
    HAL_PIN_MODE_OUTPUT_OD,
    HAL_PIN_MODE_INPUT,
    HAL_PIN_MODE_INPUT_PULLUP,
    HAL_PIN_MODE_INPUT_PULLDOWN,
} hal_pin_mode_t;

typedef enum hal_pin_irq_en {
    HAL_PIN_IRQ_NONE            = 0x00,
    HAL_PIN_IRQ_RISING          = 0x01,
    HAL_PIN_IRQ_FALLING         = 0x02,
    HAL_PIN_IRQ_RISING_FALLING  = 0x03,
} hal_pin_irq_t;

typedef struct hal_pin_st {
    hal_pin_port_t port;
    void *private_gpio;
    void *private_pin;
    hal_pin_mode_t mode;
    hal_pin_irq_t irq;
} hal_pin_t;


typedef void (*hal_pin_cb_t)(void *arg);

__API__ int tos_hal_pin_init(hal_pin_t *pin, hal_pin_port_t port, hal_pin_mode_t mode);

__API__ int tos_hal_pin_write(hal_pin_t *pin, int state);

__API__ int tos_hal_pin_read(hal_pin_t *pin);

__API__ int tos_hal_pin_mode(hal_pin_t *pin, hal_pin_mode_t mode);

__API__ int tos_hal_pin_irq(hal_pin_t *pin, hal_pin_irq_t irq);

__API__ int tos_hal_pin_irq_callback(hal_pin_t *pin, hal_pin_cb_t callback, void *cb_arg);


#endif
