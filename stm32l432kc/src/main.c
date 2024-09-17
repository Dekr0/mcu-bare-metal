#include <stdbool.h>

#include "hal.h"

static volatile uint64_t s_ticks;
void systick_handler(void) { s_ticks++; }

bool timer_expired(uint32_t *deadline, uint32_t period, uint32_t now) {
    // Time wrapped by situations such as overflow? Reset timer
    if (now + period < *deadline)
        *deadline = 0;

    // Initial deadline
    if (*deadline == 0)
        *deadline = now + period;

    if (*deadline > now || now - *deadline <= period)
        return false;
    *deadline += period; // Next expiration time
                         //
    return true;         // Expired, return true
}

int main(void) {
    if (systick_init(SYS_FREQUENCY / 1000) == -1)
        return -1;

    if (gpio_as_output(BUILTIN_LED) == -1)
        return -1;

    if (st_link_debug_init(9600) == -1)
        return -1;

    uint32_t timer, period = 50;
    for (;;) {
        if (timer_expired(&timer, period, s_ticks)) {
            gpio_toggle(BUILTIN_LED);
            ST_LINK_DEBUG->TDR = 97;
            while ((ST_LINK_DEBUG->ISR & bit(7)) == 0)
                spin(1);
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
    for (;;)
        (void)0;
}

extern void _estack(void);

__attribute__((section(".isr_vectors"))) void (*const tab[16 + 91])(void) = {
    _estack, reset_handler,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0,       systick_handler};
