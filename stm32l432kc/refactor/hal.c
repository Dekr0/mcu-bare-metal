#include "hal.h"

static GPIO_TypeDef *get_gpio_regs(const uint8_t gpio_bank);

static GPIO_TypeDef *get_gpio_regs(const uint8_t gpio_bank) {
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

int8_t gpio_enable_clk(const uint16_t gpio_pin) {
    const uint8_t gpio_pin_bank = gpio_pin_bank(gpio_pin);
    switch (gpio_pin_bank) {
    case GPIO_BANK_A:
        RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
        return 0;
    case GPIO_BANK_B:
        RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
        return 0;
    case GPIO_BANK_C:
        RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
        return 0;
    case GPIO_BANK_H:
        RCC->AHB2ENR |= RCC_AHB2ENR_GPIOHEN;
        return 0;
    }
    return -1;
}

int8_t gpio_set_af(const uint16_t gpio_pin, const uint8_t af) {
    if (af > 0b1111)
        return -1;
    GPIO_TypeDef *gpio = get_gpio_regs(gpio_pin_bank(gpio_pin));
    if (gpio == NULL)
        return -1;
    const uint8_t pin_no = gpio_pin_no(gpio_pin);
    if (pin_no > 0b1111)
        return -1;
    const uint8_t shift_count = gpio_afr_shift_count(pin_no);
    set_bits(gpio->AFR[pin_no >> 3], gpio_afr_mask << shift_count, ((uint32_t) af) << shift_count);
    return 0;
}

int8_t gpio_set_mode(const uint16_t gpio_pin, const uint8_t mode) {
    if (mode > 0b11)
        return -1;
    GPIO_TypeDef *gpio = get_gpio_regs(gpio_pin_bank(gpio_pin));
    if (gpio == NULL)
        return -1;
    const uint8_t pin_no = gpio_pin_no(gpio_pin);
    if (pin_no > 0b1111)
        return -1;
    const uint8_t shift_count = gpio_mode_shift_count(pin_no);
    set_bits(gpio->MODER, gpio_mode_mask << shift_count, ((uint32_t) mode) << shift_count);
    return 0;
}

int8_t gpio_toggle(const uint16_t gpio_pin) {
    GPIO_TypeDef *g = get_gpio_regs(gpio_pin_bank(gpio_pin));
    if (g == NULL)
        return -1;
    uint32_t mask = bit_mask(gpio_pin_no(gpio_pin));
    g->BSRR = mask << (g->ODR & mask ? 16 : 0);
    return 0;
}

int8_t gpio_write(const uint16_t gpio_pin, const uint8_t en) {
    if (en != 0b0 && en != 0b1)
        return -1;
    GPIO_TypeDef *g = get_gpio_regs(gpio_pin_bank(gpio_pin));
    if (g == NULL)
        return -1;
    g->BSRR = bit_mask(gpio_pin_no(gpio_pin)) << (en ? 0 : 16);
    return 0;
}

int8_t uart_init(USART_TypeDef *usart, uint8_t en_rx, uint8_t en_tx, uint32_t baud) {
    uint16_t tx = 0, rx = 0;
    uint8_t tx_af = 0, rx_af = 0;
    if (usart == USART1) {
        tx = gpio_pin(GPIO_BANK_A, 9), rx = gpio_pin(GPIO_BANK_A, 10);
        tx_af = GPIO_AF7, rx_af = GPIO_AF7;
        RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    } else if (usart == USART2) {
        tx = gpio_pin(GPIO_BANK_A, 2), rx = gpio_pin(GPIO_BANK_A, 15);
        tx_af = GPIO_AF7, rx_af = GPIO_AF3;
        RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;
    } else {
        return -1;
    }

    if (gpio_enable_clk(tx) == -1 || gpio_enable_clk(rx) == -1)
        return -1;
    if (gpio_set_mode(tx, GPIO_AF_MODE) == -1)
        return -1;
    if (gpio_set_mode(rx, GPIO_AF_MODE) == -1)
        return -1;
    if (gpio_set_af(tx, tx_af) == -1 || gpio_set_af(rx, rx_af) == -1)
        return -1;

    usart->CR1 = 0;
    usart->CR1 |= USART_CR1_UE;
    if (en_rx)
        usart->CR1 |= USART_CR1_RE;
    if (en_tx)
        usart->CR1 |= USART_CR1_TE;
    usart->BRR = DEFAULT_SYS_FREQUENCY / baud;
    return 0;
}
