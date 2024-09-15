## Brief Primer on Linking

- Linking is the last stage in compiling a program.
- It takes a number of compiled files an merges them into a single program, 
filling in addresses so that everything is in the right place.

### VMA / LMA 
- Every output section that can be loaded / allocated has two address, that is 
VMA and LMA.
- VMA stands for *virtual memory address*. The address the section will have 
when the output file is run.
- LMA stands for *loaded memory address*. The address at which section will be 
loaded.
- Most of the time, VMA and LMA will be the same. For example, the program 
compiled with `gcc` on Linux, the VMA and LMA will be the same since the loaded 
address is same as virtual memory address.
- However, in embedded system, this situation might change.

### Experiment

- After compiling a program, one can inspect the binary `.elf` file using command 
`objdump -D *.elf`.
- This will show all the sections and their loaded memory addresses (LMA).
- The sections are loaded to the device and device starts executing.
- It's important to ensure that device does is copy the section from LMA to VMA.
- Linker will not handle this part of responsibility.
- In order to do the above step, `.data` section will usually have two labels. 
Typically is `_sdata` and `_edata`.
- These two labels will enable programmers to implement an assembly routine that 
will move the entire section.


## Explanation of sample `link.ld`

- This sample is based off STM32L432KC. The following parts in the `.ld` file 
will be varied from MCU
    - Flash memory's origin and length 
    - SRAM memory's origin and length
    - 
- The declaration and definition of `.ld` file will also varied based on what 
the firmware tries to achieve.

### Work through

```ld
ENTRY(_reset);
```
- This above tells the linker the value of "entry point" attribute in the 
generated ELF header.
- The value corresponds to `_reset()` function in the minimal firmware `main.c`.

```ld
MEMORY {
    flash(rx): ORIGIN = 0x08000000, LENGTH = 256k
    sram(rwx): ORIGIN = 0x20000000, LENGTH = 48k
}
```
- This tells the linker there are two memory regions in the address space, address 
and sizes.
- The origin and length of flash memory varies by MCU.
- For STM32L432KC, it has a flash memory of 256 KB and a SRAM of 64 KB. 
    - The 256 KB of flash memory is embedded flash memory available for 
    storing programs and data.
    - The 64 KB of SRAM is embedded SRAM. This SRAM is split into two blocks.
        - 48 KB mapped at address 0x2000 0000 (SRAM1)
        - 16 KB located at address 0x1000 0000 (SRAM2)
- Notice that, length can be smaller than the specification if you want to 
constraint the amount of memory usage.

```ld
_estack = ORIGIN(sram) + LENGTH(sram)
```
- This tell a linker to create a symbol `estack` with value at the end of the 
SRAM region. This symbol contains initial stack value.

```ld
SECTIONS {
  .vectors  : { KEEP(*(.vectors)) }   > flash
  .text     : { *(.text*) }           > flash
  .rodata   : { *(.rodata*) }         > flash

  .data : {
    _sdata = .;   /* .data section start */
    *(.first_data)
    *(.data SORT(.data.*))
    _edata = .;  /* .data section end */
  } > sram AT > flash
  _sidata = LOADADDR(.data);

  .bss : {
    _sbss = .;              /* .bss section start */
    *(.bss SORT(.bss.*) COMMON)
    _ebss = .;              /* .bss section end */
  } > sram

  . = ALIGN(8);
  _end = .;     /* for cmsis_gcc.h  */
}
```
- The definition and declaration in `SECTIONS` block need to be in ordered. 
All section will be layout in the order this block specify.

```ld
  .vectors  : { KEEP(*(.vectors)) }   > flash
  .text     : { *(.text*) }           > flash
  .rodata   : { *(.rodata*) }         > flash
```
- The above tells the linker to the following sections in flash memory in order:
    1. vectors table,
    2. `.text` section (firmware code), 
    3. read only data `.rodata` section.

```ld
  .data : {
    _sdata = .;   /* .data section start */
    *(.first_data)
    *(.data SORT(.data.*))
    _edata = .;  /* .data section end */
  } > sram AT > flash
  _sidata = LOADADDR(.data);
```
- `_sdata` and `_edata` specifies the start and the end of `.data`.
- `_sdata` and `_edata` are also symbols for copying data section to SRAM in the
firmware code. In this case, it will be done `_reset()` boot function.
- `AT` specifies the load address for a section, which is different from its 
run time address.
    - The `.data` section will be placed in `sram` at run time (specified by 
    `> sram`), i.e., VMA of the `.data` section.
    - However, it will be stored in `flash` in the ELF (specify by `AT > flash`), 
    i.e., LMA of the `.data` section.
- This does what exactly we want, copy `.data` section specified in ELF into 
different memory region.
-  `SORT` (alias to `SORT_BY_NAME`) is used to sort input sections. In this case,
only applied to `.data.*` sections, not to the main `.data` section.
- In summary, the above do the following:
    1. Place all files `.first_data` section in the output `.data` section
    2. Then all files `.data` section in the output `.data` section
        - Specifically, sort the contents inside of each `.data` section

```ld
  .bss : {
    _sbss = .;              /* .bss section start */
    *(.bss SORT(.bss.*) COMMON)
    _ebss = .;              /* .bss section end */
  } > sram
```
- Similar to `.data` section, `_sbss` and `_ebss` are both symbols specifics the 
start and the end of `.bss` section.
- They are used for zero out the whole `.bss` section by `_reset()` boot function.

## Example output of ELF dump

- `main.o` without linker script and other procedure for copying `.data` section 
and zeroing out `.bss` section.
```
main.o:     file format elf32-littlearm

Sections:
Idx Name          Size      VMA       LMA       File off  Algn
  0 .text         00000002  00000000  00000000  00000034  2**1
                  CONTENTS, ALLOC, LOAD, READONLY, CODE
                  # Firmware code, a _reset() function, 2 bytes long.

  1 .data         00000000  00000000  00000000  00000036  2**0
                  CONTENTS, ALLOC, LOAD, DATA
                  # This should reside in RAM. _reset() function should copy the 
                  contents of the .data section to RAM.

  2 .bss          00000000  00000000  00000000  00000036  2**0
                  ALLOC
  3 .vectors      000001ac  00000000  00000000  00000038  2**2
                  CONTENTS, ALLOC, LOAD, RELOC, READONLY, DATA
  4 .comment      0000001f  00000000  00000000  000001e4  2**0
                  CONTENTS, READONLY
  5 .ARM.attributes 0000002e  00000000  00000000  00000203  2**0
                  CONTENTS, READONLY
```
- `main.o` after linking with linker script and additional procedures for copying 
`.data` section and zeroing out `.bss` section
```text
firmware.elf:     file format elf32-littlearm

Sections:
Idx Name          Size      VMA       LMA       File off  Algn
0 .vectors      000001ac  08000000  08000000  00001000  2**2
CONTENTS, ALLOC, LOAD, READONLY, DATA
1 .text         00000050  080001ac  080001ac  000011ac  2**2
CONTENTS, ALLOC, LOAD, READONLY, CODE
2 .comment      0000001e  00000000  00000000  000011fc  2**0
CONTENTS, READONLY
3 .ARM.attributes 0000002e  00000000  00000000  0000121a  2**0
CONTENTS, READONLY
```
