- This write up is target toward STM32L432KC.

## Abstraction of accessing GPIO

- For STM32L432KC, here's memory address range for each GPIO port (Check "Memory 
Organization" section in the MCU datasheet)
    - `GPIOA` - `0x4800 0000 - 0x4800 03FF`
    - `GPIOB` - `0X4800 0400 - 0X4800 07FF`
    - `GPIOC` - `0x4800 0800 - 0x4800 0BFF`
    - `GPIOH` - `0x4800 1C00 - 0X4800 1FFF`
- For each port (A, B, C, ...) of `GPIO`, there are different 32 bit registers 
available to read and write. Each register correspond a specific function of 
the `GPIO`. (Check "GPIO" section in the MCU datasheet) to see what they do.

### Exercise 1

- Let clear out all out value for `GPIOB` `MODER` Register
```c
(volatile uint32_t *) (0x48000400 + 0) = 0;
```

### Exercise 2

- Set Pin 8 to General Purpose Output mode while retain all the setting for 
other pins.
    1. First zero out bit 17 to bit 16 while retain other bits
    2. Set bit 17 to bit 16 to 0b01
```c
(volatile uint32_t *) (0x48000400 + 0) &= ~(3 << 16);
(volatile uint32_t *) (0x48000400 + 0) |= 1 << 16;
```

### Conclusion

- For a register with `N` bits, to set `M` bits where the most right bit is at 
`K` with `VALUE`, one can do the following:
```c
REGISTER &= ~((2^M - 1) << K) // Clearing
REGISTER &= VALUE << K // Set
```

### Abstraction Code

- Check `manual/main.c` and `manual/hal.h`
- This `GPIO_TypeDef` is there to capture all different registers for each port 
of `GPIO`. Setting each 32 bit register will have different behavior on a given 
port of `GPIO`. 
- This form of abstraction will be used to capture other peripheral memory region 
again such as reset and clock control register, SystemTick timer register, UART, 
etc.
- Why this type of abstraction / interfacing is needed? True, we can read and 
write peripheral register by reading off the data sheet and direct accessing 
certain memory addresses (how all exercises did).
- However, this is pretty cryptic and might be repeated and error prone (An 
abstraction can be error prone if not well tested and it's not simple).

## Reset and Clock Control

- For STM32 MCUs, by default have their peripherals disabled in order to save 
power.
- In order to enable a `GPIO` peripheral, it should be enabled (clocked) via the 
***RCC*** (Reset and Clock Control) unit.
- To control *RCC*, it follow the exact same principle of controlling other 
peripherals in the MCU, which is via read / write a collection 32 bits *registers* 
in a specific peripheral memory region.
- Check "Reset and Control" section in the MCU datasheet 

### Abstraction of RCC accessing 

- Check `manual/main.c` and `manual/hal.h`


