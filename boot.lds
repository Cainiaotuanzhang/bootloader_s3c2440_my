SECTIONS {
    . = 0x33f80000;
    .text : { start.o *(.text) }
    . = ALIGN(4);
    
    .rodata : { *(.rodata)}
    . = ALIGN(4);
    
    .data : { *(.data) }
    . = ALIGN(4);
    
    __bss_start = .;
    .bss : { *(.bss) }
    __bss_end = .;
    . = ALIGN(4);
}
