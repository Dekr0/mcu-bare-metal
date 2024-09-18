#include <stdbool.h>

#include "hal.h"

void spin(volatile int32_t count) {
    while (count-- > 0)
        asm("nop");
}

int main() {
    uint16_t extern_led = gpio_pin(GPIO_BANK_B, 4);

    if (gpio_enable_clk(BUILTIN_LED) == -1)
        return -1;
    if (gpio_set_mode(BUILTIN_LED, GPIO_OUTPUT_MODE) == -1)
        return -1;
    if (gpio_set_mode(extern_led, GPIO_OUTPUT_MODE) == -1)
        return -1;

    if (uart_init(USART_DEBUG, 0, 1, 115200) == -1)
        return -1;
    for (;;) {
        spin(0xfffff);
        USART_DEBUG->TDR = 'A';
        while ((USART_DEBUG->ISR & USART_ISR_TC) == 0)
            spin(5);
        gpio_toggle(extern_led);
        gpio_toggle(BUILTIN_LED);
    }
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
    for (;;)
        (void)0;
}

extern void _estack(void);

__attribute__((section(".isr_vectors"))) void (*const tab[16 + 91])(void) = {
    _estack, reset_handler,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
