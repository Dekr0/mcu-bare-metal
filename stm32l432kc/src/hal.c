#include <stdlib.h>

#include "hal.h"

static GPIO_TypeDef *get_gpio_regs(gpio_bank_e gpio_bank);

GPIO_TypeDef *get_gpio_regs(gpio_bank_e gpio_bank) {
    switch (gpio_bank) {
    case GPIO_BANK_A:
        return GPIOA;
    case GPIO_BANK_B:
        return GPIOB;
    case GPIO_BANK_C:
        return GPIOC;
    case GPIO_BANK_H:
        return GPIOH;
    }
    return NULL;
}

/**
 * Available GPIOs whose clocks can be enabled for STM32L432KB and STM32L432KC
 * RM0394 - pg. 218
 * STM32L432KB and STM32L432KC board data sheet - pg. 60
 * */
int8_t gpio_clk_enable(uint16_t gpio_pin) {
    gpio_bank_e bank = get_gpio_pin_bank(gpio_pin);
    switch (bank) {
    case GPIO_BANK_A:
    case GPIO_BANK_B:
    case GPIO_BANK_C:
        RCC->AHB2ENR |= bit(bank);
        return 0;
    case GPIO_BANK_H:
        RCC->AHB2ENR |= 1UL << 7;
        return 0;
    }
    return -1;
}

/**
 * RM0394 - pg. 267
 * */
int8_t gpio_set_mode(uint16_t gpio_pin, gpio_mode_e mode) {
    if (!(mode >= 0b00 && mode <= 0b11))
        return -1;
    GPIO_TypeDef *g = get_gpio_regs(get_gpio_pin_bank(gpio_pin));
    if (g == NULL)
        return -1;
    uint8_t pin_no = get_gpio_pinno(gpio_pin);
    setbits(g->MODER, 3UL << (pin_no * 2), ((uint32_t)mode) << (pin_no * 2));
    return 0;
}

int8_t gpio_set_output_type(uint16_t gpio_pin, gpio_output_type_e type) {
    if (type != 0 && type != 1)
        return -1;
    GPIO_TypeDef *g = get_gpio_regs(get_gpio_pin_bank(gpio_pin));
    if (g == NULL)
        return -1;
    uint8_t pin_no = get_gpio_pinno(gpio_pin);
    setbits(g->OTYPER, 1UL << pin_no, ((uint32_t)type) << pin_no);
    return 0;
}

int8_t gpio_set_output_speed(uint16_t gpio_pin, gpio_output_speed_e speed) {
    if (!(speed >= 0b00 && speed <= 0b11))
        return -1;
    GPIO_TypeDef *g = get_gpio_regs(get_gpio_pin_bank(gpio_pin));
    if (g == NULL)
        return -1;
    uint8_t pin_no = get_gpio_pinno(gpio_pin);
    setbits(g->OSPEEDR, 3UL << (pin_no * 2), ((uint32_t)speed) << (pin_no * 2));
    return 0;
}

int8_t gpio_set_pull(uint16_t gpio_pin, gpio_pupd_e pull) {
    if (!(pull >= 0b00 && pull <= 0b10))
        return -1;
    GPIO_TypeDef *g = get_gpio_regs(get_gpio_pin_bank(gpio_pin));
    if (g == NULL)
        return -1;
    uint8_t pin_no = get_gpio_pinno(gpio_pin);
    setbits(g->PUPDR, 3UL << (pin_no * 2), ((uint32_t)pull) << (pin_no * 2));
    return 0;
}

/**
 * RM0394 - pg. 271 - 273
 * STM32L432KB and STM32L432KC board data sheet - pg. 52 - 58
 * */
int8_t gpio_set_af(uint16_t gpio_pin, gpio_af_e af) {
    if (!(af >= 0b0000 && af <= 0b1111))
        return -1;
    GPIO_TypeDef *g = get_gpio_regs(get_gpio_pin_bank(gpio_pin));
    if (g == NULL)
        return -1;
    uint8_t pin_no = get_gpio_pinno(gpio_pin);
    setbits(g->AFR[pin_no >> 3], 15UL << ((pin_no & 0b111) * 4),
            ((uint32_t)af) << ((pin_no & 0b111) * 4));
    return 0;
}

/**
 * RM0394 - pg. 269 - 270
 * */
int8_t gpio_toggle(uint16_t gpio_pin) {
    GPIO_TypeDef *g = get_gpio_regs(get_gpio_pin_bank(gpio_pin));
    if (g == NULL)
        return -1;
    uint32_t mask = bit(get_gpio_pinno(gpio_pin));
    g->BSRR = mask << (g->ODR & mask ? 16 : 0);
    return 0;
}

/**
 * RM0394 - pg. 269 - 270
 * */
int8_t gpio_write(uint16_t gpio_pin, uint8_t set) {
    if (set != 0 && set != 1)
        return -1;
    GPIO_TypeDef *g = get_gpio_regs(get_gpio_pin_bank(gpio_pin));
    if (g == NULL)
        return -1;
    g->BSRR = bit(get_gpio_pinno(gpio_pin)) << (set ? 0 : 16);
    return 0;
}

int8_t gpio_batch_set(uint16_t gpio_pin, gpio_af_e af, gpio_mode_e mode,
                      gpio_pupd_e pull, gpio_output_speed_e speed,
                      gpio_output_type_e type) {
    if (gpio_clk_enable(gpio_pin) == -1)
        return -1;
    if (gpio_set_af(gpio_pin, af) == -1)
        return -1;
    if (gpio_set_mode(gpio_pin, mode) == -1)
        return -1;
    if (gpio_set_pull(gpio_pin, pull) == -1)
        return -1;
    if (gpio_set_output_speed(gpio_pin, speed) == -1)
        return -1;
    if (gpio_set_output_type(gpio_pin, type) == -1)
        return -1;
    return 0;
}

/**
 * PM0214 - pg. 247 - 250
 * */
int8_t systick_init(uint32_t ticks) {
    if ((ticks - 1) > 0xffffff)
        return -1; // Systick timer is 24 bit
    SYSTICK->LOAD = ticks - 1;
    SYSTICK->VAL = 0;
    SYSTICK->CTRL = bit(0) | bit(1) | bit(2); // Enable systick
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;     // Enable SYSCFG
    return 0;
}

void spin(volatile int32_t count) {
    while (count-- > 0)
        asm("nop");
}

int8_t gpio_as_output(uint16_t gpio_pin) {
    if (gpio_clk_enable(gpio_pin) == -1)
        return -1;
    if (gpio_set_mode(gpio_pin, GPIO_MODE_OUTPUT) == -1)
        return -1;
    return 0;
}

int8_t st_link_debug_init(uint64_t baud) {
    RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;

    const uint16_t rx = encode_gpiopin(GPIO_BANK_A, 15);
    const uint16_t tx = encode_gpiopin(GPIO_BANK_A, 2);

    if (gpio_batch_set(rx, GPIO_AF_3, GPIO_MODE_AF, GPIO_PUPD_NONE,
                       GPIO_OUTPUT_SPEED_HIGH,
                       GPIO_OUTPUT_TYPE_PULL_UP_PULL_DOWN) == -1)
        return -1;
    if (gpio_batch_set(tx, GPIO_AF_7, GPIO_MODE_AF, GPIO_PUPD_NONE,
                       GPIO_OUTPUT_SPEED_HIGH,
                       GPIO_OUTPUT_TYPE_PULL_UP_PULL_DOWN) == -1)
        return -1;

    ST_LINK_DEBUG->CR1 = 0;
    ST_LINK_DEBUG->BRR = APB1_FREQUENCY / baud;
    ST_LINK_DEBUG->CR1 |= bit(0) | bit(2) | bit(3);

    return 0;
}
