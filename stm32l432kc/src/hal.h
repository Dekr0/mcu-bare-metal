#ifndef __HAL_H__
#define __HAL_H__

#include <stdint.h>

#include "Include/stm32l432xx.h"

/**
 * SysTick timer register information
 * PM0214 - pg. 246
 * */
typedef struct {
    volatile uint32_t CTRL, LOAD, VAL, CALIB;
} systick_t;

/**
 * SysTick timer register addresses
 * PM0214 - pg. 246
 * */
#define SYSTICK ((systick_t *)0xe000e010)
#define AHB_DIV 1
#define APB1_DIV 1
#define APB2_DIV 1
#define PLL_HSI 16
#define PLL_M 1
#define PLL_N 10
#define PLL_R 2
#define SYS_FREQUENCY 16000000
#define APB2_FREQUENCY (SYS_FREQUENCY / APB2_DIV)
#define APB1_FREQUENCY (SYS_FREQUENCY / APB1_DIV)

/**
 * Available GPIO for STM32L432KB and STM32L432KC
 * STM32L432KB and STM32L432KC board data sheet - pg. 60
 * */
typedef enum : uint8_t {
    GPIO_BANK_A,
    GPIO_BANK_B,
    GPIO_BANK_C,
    GPIO_BANK_H
} gpio_bank_e;

typedef enum : uint8_t {
    GPIO_AF_0,
    GPIO_AF_1,
    GPIO_AF_2,
    GPIO_AF_3,
    GPIO_AF_4,
    GPIO_AF_5,
    GPIO_AF_6,
    GPIO_AF_7,
    GPIO_AF_8,
    GPIO_AF_9,
    GPIO_AF_10,
    GPIO_AF_11,
    GPIO_AF_12,
    GPIO_AF_13,
    GPIO_AF_14,
    GPIO_AF_15
} gpio_af_e;

typedef enum : uint8_t {
    GPIO_MODE_INPUT,
    GPIO_MODE_OUTPUT,
    GPIO_MODE_AF,
    GPIO_MODE_ANALOG
} gpio_mode_e;

typedef enum : uint8_t {
    GPIO_OUTPUT_SPEED_LOW,
    GPIO_OUTPUT_SPEED_MEDIUM,
    GPIO_OUTPUT_SPEED_HIGH,
    GPIO_OUTPUT_SPEED_VERY_HIGH,
} gpio_output_speed_e;

typedef enum : uint8_t {
    GPIO_OUTPUT_TYPE_PULL_UP_PULL_DOWN,
    GPIO_OUTPUT_TYPE_OPEN_DRAIN
} gpio_output_type_e;

typedef enum : uint8_t {
    GPIO_PUPD_NONE,
    GPIO_PUPD_PULL_UP,
    GPIO_PUPD_PULL_DONW
} gpio_pupd_e;

/**
 * Encode the GPIO port and the pin number of that given GPIO port into a
 * 16 bits
 * Bit 15 to 8 represent the GPIO port.
 * Bit 7 to 0 represent the pin number of that given GPIO port
 * */
#define encode_gpiopin(bank, pin_no) (((bank) << 8) | (pin_no))
#define get_gpio_pinno(gpio_pin) (gpio_pin & 255)
#define get_gpio_pin_bank(gpio_pin) (gpio_pin >> 8)

#define ST_LINK_DEBUG USART2
#define BUILTIN_LED encode_gpiopin(GPIO_BANK_B, 3)

/** Utility Macros */
#define bit(x) (1UL << (x))
#define setbits(r, clearmask, setmask)                                         \
    (r) &= ~(clearmask);                                                       \
    (r) |= (setmask);

int8_t gpio_clk_enable(uint16_t gpio_pin);
int8_t gpio_set_mode(uint16_t gpio_pin, gpio_mode_e mode);
int8_t gpio_set_output_type(uint16_t gpio_pin, gpio_output_type_e type);
int8_t gpio_set_output_speed(uint16_t gpio_pin, gpio_output_speed_e speed);
int8_t gpio_set_pull(uint16_t gpio_pin, gpio_pupd_e pull);
int8_t gpio_set_af(uint16_t gpio_pin, gpio_af_e af);
int8_t gpio_toggle(uint16_t gpio_pin);
int8_t gpio_write(uint16_t gpio_pin, uint8_t set);
int8_t gpio_batch_set(uint16_t gpio_pin, gpio_af_e af, gpio_mode_e mode,
                      gpio_pupd_e pull, gpio_output_speed_e speed,
                      gpio_output_type_e type);
int8_t gpio_as_output(uint16_t gpio_pin);
int8_t st_link_debug_init(uint64_t baud);
int8_t systick_init(uint32_t ticks);
void spin(volatile int32_t count);


#endif
