#ifndef __HAL_H__
#define __HAL_H__

#include <stdint.h>
#include <stdlib.h>
#include <stm32l432xx.h>

#define GPIO_AF0 0b0000
#define GPIO_AF1 0b0001
#define GPIO_AF2 0b0010
#define GPIO_AF3 0b0011
#define GPIO_AF4 0b0100
#define GPIO_AF5 0b0101
#define GPIO_AF6 0b0110
#define GPIO_AF7 0b0111
#define GPIO_AF8 0b1000
#define GPIO_AF9 0b1001
#define GPIO_AF10 0b1010
#define GPIO_AF11 0b1011
#define GPIO_AF12 0b1100
#define GPIO_AF13 0b1101
#define GPIO_AF14 0b1110
#define GPIO_AF15 0b1111

#define GPIO_BANK_A 0U
#define GPIO_BANK_B 1U
#define GPIO_BANK_C 2U
#define GPIO_BANK_D 3U
#define GPIO_BANK_E 4U
#define GPIO_BANK_H 7U

#define GPIO_INPUT_MODE 0b00
#define GPIO_OUTPUT_MODE 0b01
#define GPIO_AF_MODE 0b10
#define GPIO_ANALOG_MODE 0b11

#define gpio_pin(gpio_pin_bank, gpio_pin_no) \
    ((((uint16_t) (gpio_pin_bank)) << 8U) | (gpio_pin_no))
#define gpio_pin_bank(gpio_pin) ((gpio_pin) >> 8U)
#define gpio_pin_no(gpio_pin) ((gpio_pin) & 0b11111111)

#define gpio_mode_mask 0b11
#define gpio_afr_mask 0b1111

#define gpio_mode_shift_count(gpio_pin_no) ((gpio_pin_no) * 2)
#define gpio_afr_shift_count(gpio_pin_no) (((gpio_pin_no) & 0b0111) * 4)

/**
 * This default system frequency is very important. It affect the correctness of 
 * different components' operation
 */
#define DEFAULT_SYS_FREQUENCY 4000000

#define BUILTIN_LED gpio_pin(GPIO_BANK_B, 3)
#define USART_DEBUG USART2

#define bit_mask(bit_pos) (1UL << (bit_pos))
#define set_bits(dest, clear_mask, set_mask) \
    (dest) = ((dest) & ~(clear_mask)) | (set_mask);
#define bit_at_pos(value, zero_idx) (((value) >> (zero_idx)) & 0b1)

int8_t gpio_enable_clk(const uint16_t gpio_pin);
int8_t gpio_set_af(const uint16_t gpio_pin, const uint8_t af);
int8_t gpio_set_mode(const uint16_t gpio_pin, const uint8_t mode);
int8_t gpio_toggle(const uint16_t gpio_pin);
int8_t gpio_write(const uint16_t gpio_pin, const uint8_t en);
int8_t uart_init(USART_TypeDef *usart, uint8_t en_rx, uint8_t en_tx, uint32_t baud);

#endif
