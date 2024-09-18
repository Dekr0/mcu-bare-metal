#ifndef __HAL_H__
#define __HAL_H__

#include <stdint.h>
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

#define GPIO_INPUT_MODE 0b00
#define GPIO_OUTPUT_MODE 0b01
#define GPIO_AF_MODE 0b10
#define GPIO_ANALOG_MODE 0b11

#define gpio_pin(gpio_pin_bank, gpio_pin_no) \
    ((((uint16_t) (gpio_pin_bank)) << 8) | (gpio_pin_no))
#define gpio_pin_bank(gpio_pin) ((gpio_pin) >> 8)
#define gpio_pin_no(gpio_pin) ((gpio_pin) & 0b11111111)

#define set_bits(dest, clear_mask, set_mask) \


int8_t gpio_enable_clk(uint16_t gpio_pin);
int8_t gpio_set_af(uint16_t gpio_pin, uint8_t mode);
int8_t gpio_set_mode(uint16_t gpio_pin, uint8_t mode);
int8_t uart_enable_re(USART_TypeDef *usart);
int8_t uart_enable_te(USART_TypeDef *usart);
int8_t uart_enable_ue(USART_TypeDef *usart);

#endif
