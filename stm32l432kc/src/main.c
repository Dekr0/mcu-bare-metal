#include <stdbool.h>

#include "hal.h"

#define ELED_GPIOPIN encode_gpiopin(GPIO_BANK_B, 4)
#define ILED_GPIOPIN encode_gpiopin(GPIO_BANK_B, 3)

static inline GPIO_TypeDef  *get_gpio(gpio_bank_e gpio);
static inline int8_t gpio_clock_enable(uint16_t gpio_pin);
static inline int8_t gpio_set_mode(uint16_t gpio_pin, gpio_mode_e mode);
static inline int8_t gpio_write(uint16_t gpio_pin, bool set);

static inline GPIO_TypeDef *get_gpio(gpio_bank_e gpio) {
    switch (gpio) {
    case GPIO_BANK_A:
        return GPIOA;
    case GPIO_BANK_B:
        return GPIOB;
    case GPIO_BANK_C:
        return GPIOC;
    case GPIO_BANK_H:
        return GPIOH;
    }
    return 0;
}

static inline int8_t gpio_clock_enable(uint16_t gpio_pin) {
    gpio_bank_e bank = get_pinbank(gpio_pin);
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

static inline int8_t gpio_set_mode(uint16_t gpio_pin, gpio_mode_e mode) {
    GPIO_TypeDef *g = get_gpio(get_pinbank(gpio_pin));
    if (g == 0)
        return -1;

    int8_t pin_no = get_pinno(gpio_pin);
    if (pin_no < 0 || pin_no > 15)
        return -1;

    setbits(g->MODER, 3UL << (pin_no * 2), ((uint32_t) mode) << (pin_no * 2));

    return 0;
}

static inline int8_t gpio_write(uint16_t gpio_pin, bool set) {
    GPIO_TypeDef *g = get_gpio(get_pinbank(gpio_pin));
    if (g == 0)
        return -1;

    int8_t pin_no = get_pinno(gpio_pin);
    if (pin_no < 0 || pin_no > 15) 
        return -1;

    g->BSRR = bit(pin_no) << (set ? 0 : 16);

    return -1;
}

static inline void systick_init(uint32_t ticks) {
    if ((ticks - 1) > 0xffffff) return; // Systick timer is 24 bit
    SYSTICK->LOAD = ticks - 1;
    SYSTICK->VAL = 0;
    SYSTICK->CTRL = bit(0) | bit(1) | bit(2); // Enable systick
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN; // Enable SYSCFG
}

static volatile uint32_t s_ticks;
void systick_handler(void) {
    s_ticks++;
}

bool timer_expired(uint32_t *deadline, uint32_t period, uint32_t now) {
    // Time wrapped by situations such as overflow? Reset timer
    if (now + period < *deadline)
        *deadline = 0; 
    
    // Initial deadline
    if (*deadline == 0)
        *deadline = now + period; 

    if (*deadline > now || now - *deadline <= period)
        return false; 
    *deadline += period;  // Next expiration time
                                                                                 //
    return true; // Expired, return true
}

int main(void) {
    systick_init(SYS_FREQUENCY / 1000);

    if (gpio_clock_enable(ILED_GPIOPIN) == -1)
        return -1;

    if (gpio_set_mode(ILED_GPIOPIN, GPIO_MODE_OUTPUT) == -1)
        return -1;

    if (gpio_clock_enable(ELED_GPIOPIN) == -1)
        return -1;

    if (gpio_set_mode(ELED_GPIOPIN, GPIO_MODE_OUTPUT) == -1)
        return -1;

    uint32_t timer, period = 50;          // Declare timer and 5ms period
    for (;;) {
        if (timer_expired(&timer, period, s_ticks)) {
            static bool on; // This block is executed
            gpio_write(ILED_GPIOPIN, on); // Every `period` milliseconds
            gpio_write(ELED_GPIOPIN, on);
            on = !on; // Toggle LED state
        }
    }
    return 0;
}

__attribute__((naked, noreturn)) void reset_handler(void) {
    extern long _sdata, _edata, _sidata;
    extern long _sbss, _ebss;

    /** copy .data to SRAM region */
    for (long *dst = &_sdata, *src = &_sidata; dst < &_edata;)
        *dst++ = *src++;

    /** zero out .bss */
    for (long *dst = &_sbss; dst < &_ebss; dst++)
        *dst = 0;

    main();
    for (;;) (void) 0;
}

extern void _estack(void);

__attribute__((section(".isr_vectors"))) void (*const tab[16 + 91])(void) = { 
    _estack, reset_handler, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, systick_handler
};
