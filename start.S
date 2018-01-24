
#define S3C2440_MPLL_405MHz     ((0x7f << 12) | (0x2 << 4) | (0x1))

.text
.global _start
_start:
    b reset
    ldr pc, _undefined_instruction  @ 0x00
    ldr pc, _software_interrupt     @ 0x04
    ldr pc, _prefetch_abort         @ 0x08
    ldr pc, _data_abort             @ 0x10
    ldr pc, _not_used               @ 0x14
    ldr pc, _irq                    @ 0x18
    ldr pc, _fiq                    @ 0x1C


reset:
    /* 1. 关看门狗 */
    mov r0, #0x53000000 
    mov r1, #0
    str r1, [r0]

    /* 2. 设置时钟 */
    ldr r0, =0x4c000014
    mov r1, #0x05   /* FCLK:HCLK:PCLK = 1:4:8 */
    str r1, [r0]

    /* 如果HDIVN非0，CPU的总线模式应该从"fast bus mode"变为"asynchronous bus mode" */
    mrc p15, 0, r1, c1, c0, 0   /* 读出控制寄存器 */
    orr r1, r1, #0xc0000000     /* 设置为"asynchronous bus mode" */
    mcr p15, 0, r1, c1, c0, 0   /* 写入控制寄存器 */

    /* FCLK is 405MHz */
    ldr r0, =0x4c000004
    ldr r1, =S3C2440_MPLL_405MHz
    str r1, [r0]

    /* 3. 初始化SDRAM */
    ldr r0, =0x48000000  
    adr r1, sdram_config     /* sdram_config的当前地址 */  
    add r3, r0, #(13*4)  
1:  
    ldr r2, [r1], #4  
    str r2, [r0], #4  
    cmp r0, r3  
    bne 1b  

    /* 4. 重定位 */
    ldr sp, =0x34000000
    bl nand_init
    
    mov r0, #0  
    ldr r1, =_start  
    ldr r2, =__bss_start  
    sub r2, r2, r1
    
    bl copy_code_to_sdram
    bl clear_bss

    ldr lr, =halt
    ldr pc, =main
halt:
    b halt

sdram_config:
    .long   0x22011110      @ BWSCON
    .long   0x00000700      @ BANKCON0
    .long   0x00000700      @ BANKCON1
    .long   0x00000700      @ BANKCON2
    .long   0x00000700      @ BANKCON3  
    .long   0x00000700      @ BANKCON4
    .long   0x00000700      @ BANKCON5
    .long   0x00018005      @ BANKCON6
    .long   0x00018005      @ BANKCON7
    .long   0x008C07A3      @ REFRESH
    .long   0x000000B1      @ BANKSIZE
    .long   0x00000030      @ MRSRB6
    .long   0x00000030      @ MRSRB7

_undefined_instruction:  .word undefined_instruction
_software_interrupt: .word software_interrupt
_prefetch_abort: .word prefetch_abort
_data_abort: .word data_abort
_not_used:   .word not_used
_irq:    .word irq
_fiq:    .word fiq


undefined_instruction:
@ not set

software_interrupt:
@ not set

prefetch_abort:
@ not set

data_abort:
@ not set

not_used:
@ not set

irq:
@ not set

fiq:
@ not set

