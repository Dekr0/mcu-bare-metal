#include <stdint.h>
#include <stm32l432xx.h>

void spin(volatile int32_t count) {
    while (count-- > 0)
        asm("nop");
}

int main() {
    /** Enable clock for GPIOA */
    RCC->AHB2ENR &= ~(0b1 << RCC_AHB2ENR_GPIOAEN_Pos);
    RCC->AHB2ENR |= 0b1 << RCC_AHB2ENR_GPIOAEN_Pos;

    /** Set PA2 (TX) to AF */
    GPIOA->MODER &= ~(0b11 << GPIO_MODER_MODE2_Pos);
    GPIOA->MODER |= 0b10 << GPIO_MODER_MODE2_Pos;
    /** Set PA15 (RX) to AF */
    GPIOA->MODER &= ~(0b11 << GPIO_MODER_MODE15_Pos);
    GPIOA->MODER |= 0b10 << GPIO_MODER_MODE15_Pos;

    /** Set PA2 (TX) AF to AF7 */
    GPIOA->AFR[0] &= ~(0b1111 << GPIO_AFRL_AFSEL2_Pos);
    GPIOA->AFR[0] |= 0b0111 << GPIO_AFRL_AFSEL2_Pos;

    /** Set PA15 (RX) AF to AF3 */
    GPIOA->AFR[1] &= ~(0b1111 << GPIO_AFRH_AFSEL15_Pos);
    GPIOA->AFR[1] |= 0b0011 << GPIO_AFRH_AFSEL15_Pos;

    /** Enable clock for USART2 */
    RCC->APB1ENR1 &= ~(0b1 << RCC_APB1ENR1_USART2EN_Pos);
    RCC->APB1ENR1 |= 0b1 << RCC_APB1ENR1_USART2EN_Pos;

    USART2->CR1 = 0; // Clear
    USART2->CR1 |= 0b1 << USART_CR1_UE_Pos; // Enable UE
    USART2->CR1 |= 0b1 << USART_CR1_RE_Pos; // Enable RE
    USART2->CR1 |= 0b1 << USART_CR1_TE_Pos; // Enable TE
    USART2->BRR = 4000000 / 9600;

    for (;;) {
        spin(0xffff);
        USART2->TDR = 'w';
        while ((USART2->ISR & USART_ISR_TC) == 0)
            spin(5);
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

