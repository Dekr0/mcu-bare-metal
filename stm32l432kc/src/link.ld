ENTRY(reset_handler);

/**
 * - STM32L432KB STM32L432KC - pg. 15 to 16, pg. 59 to 60
 * */
MEMORY {
    flash(rx) : ORIGIN = 0x08000000, LENGTH = 256k
    sram(rwx) : ORIGIN = 0x20000000, LENGTH = 64k
}

_estack = ORIGIN(sram) + LENGTH(sram);

SECTIONS {
    .vectors : { KEEP(*(.isr_vectors)) }  > flash
    .text    : { *(.text* .text.*) }      > flash
    .rodata  : { *(.rodata*) }            > flash
    .data    : { _sdata = .; *(.first_data) *(.data SORT(.data.*)) _edata = .; } > sram AT > flash
    _sidata = LOADADDR(.data);
    .bss     : { _sbss = .; *(.bss SORT(.bss.*) COMMON) _ebss = .; } > sram
    . = ALIGN(8);
    _end = .;
}
