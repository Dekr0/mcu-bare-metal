#ifndef __HAL_H__
#define __HAL_H__

#include <stdint.h>

#include "./Include/stm32l432xx.h"

/**
 * SysTick timer register information
 * PM0214 - pg. 246
 * */
typedef struct {
    volatile uint32_t CTRL, LOAD, VAL, CALIB;
} systick_t;

/**
 * Available GPIO for STM32L432KB and STM32L432KC
 * STM32L432KB and STM32L432KC board data sheet - pg. 60
 * */
typedef enum: uint8_t {
    GPIO_BANK_A,
    GPIO_BANK_B,
    GPIO_BANK_C,
    GPIO_BANK_H
} gpio_bank_e;

typedef enum: uint8_t {
    GPIO_MODE_INPUT,
    GPIO_MODE_OUTPUT,
    GPIO_MODE_AF,
    GPIO_MODE_ANALOG
} gpio_mode_e;

/**
 * SysTick timer register addresses 
 * PM0214 - pg. 246
 * */
#define SYSTICK ((systick_t *) 0xe000e010)
#define SYS_FREQUENCY 16000000

#define bit(x) (1UL << (x))

#define setbits(r, clearmask, setmask) (r) = ((r) & ~(clearmask)) | (setmask)

/**
 * Encode the GPIO port and the pin number of that given GPIO port into a 
 * 16 bits
 * Bit 15 to 8 represent the GPIO port.
 * Bit 7 to 0 represent the pin number of that given GPIO port
 * */
#define encode_gpiopin(bank, pin_no) (((bank) << 8) | (pin_no))
#define get_pinno(gpio_pin) (gpio_pin & 255)
#define get_pinbank(gpio_pin) (gpio_pin >> 8)

#endif
