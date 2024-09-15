- Firmware code usually resides in flash memory. Check where flesh memory from 
data sheet.
- Vector table also resides in flash memory. 
- However, on System reset, the vector table is fixed at address `0x00000000`. 
Privileged software can write to the vector table start address to a different 
memory location.

## Sample `main.c`

- This minimal firmware specifies boot function `reset_handler()` that initially does 
nothing (falls into infinite loop).
- This minimal firmware also specifies a vector table that contains 16 std. entries 
and 91 STM32 entries.
- The exception are first two entries, which play a key role in the MCU boot 
process. 
- Those two first values are: an initial stack pointer, and an address of the 
boot function to execute (a firmware entry point).
- Firmware should be composed in a way that the 2nd 32 bit value in the flash 
memory should contain an address of the boot function.
- When MCU boots, it'll read that address from flash memory, and jump to the 
boot function.

### Startup code

```C
__attribute__((naked, noreturn)) void _reset(void) {
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
```
- This function should copy `.data` section in the compiled and linking result 
into RAM (not flash memory). It also has to zero out the whole `.bss` section. 
- Before this, it need to knows where stack starts, and where `.data` and `.bss` 
section start. 
    - Wait, why it needs to know where stack starts?
- How does it obtain the above information? This information are defined in 
the `SECTIONS` block in ELF. Check `./linking.md` for more information.
- `_sbss`, `_ebss`, `_sdata`, `_edata` and `_sidata` are symbols that are 
defined in ELF.

#### GCC Attributes

- `naked` - This tells GCC not to generate standard function's prologue and epilogue 
      - What's function prologue and epilogue? They are assembly code at the 
      beginning / end of a function.
      - Function prologue prepares the stack and registers for use within the 
      function.
      - Function epilogue restores the stack and register to the state they were 
      in before the function was called.
 
- `noreturn` - This tells GCC that this function does not return.

## Stack Pointer Value in Vector Table

```C
extern void _estack(void);
```
- This is the initial stack pointer value in vector table.
- ELF will define what the value is.
- Since it resides in the vector table (a C array), it need to take the form of 
a function. When referencing `_estack` in vector table, it essentially a pointer.

## Vector Table

```C
__attribute__((section(".isr_vectors"))) void (*const tab[16 + 91])(void) = { 
    _estack, reset_handler, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, systick_handler
};
```
- The vector table is represented as a C array of function pointer.
- The vector table contains 16 standard handlers and 91 STM32-specific handlers.
- Each such function in this C array is an IRQ handler
- `section(".isr_vector")` is correlative to `.isr_vector` section in `SECTIONS` 
block in the ELF.
    - This tell the linker to put this section right at the beginning of the 
    firmware generated.
    - It's in the beginning of flash memory, and right before `.text` (firmware 
    code).
- The elements in this C array fit the description mentioned above about the 
first two entries in the vector table.
