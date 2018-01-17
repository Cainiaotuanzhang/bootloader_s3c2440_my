
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

_undefined_instruction:  .word undefined_instruction
_software_interrupt: .word software_interrupt
_prefetch_abort: .word prefetch_abort
_data_abort: .word data_abort
_not_used:   .word not_used
_irq:    .word irq
_fiq:    .word fiq

reset:
/* 0. disable IRQ, disable FIR, set SVC32*/
    mrs r0, cpsr
    bic r0, r0, #0x1f
    orr r0, r0, #0xd3
    msr cpsr, r0

/* 1. disable watch dog */
    ldr r0, =0x53000000 
    mov r1, #0
    str r1, [r0]

/* 2. disable all interrupts */
	ldr r0, =0x4a000008
	mov r1, #0xffffffff
	str r1, [r0]

    ldr r0, =0x4a00001c
    ldr r1, =0x7ff
    str r1, [r0]

/* 3. set clock, set CP15 */
    ldr r0, =0x4c000014
    mov r1, #5   /* FCLK:HCLK:PCLK = 1:4:8 */
    str r1, [r0]

    /* 如果HDIVN非0，CPU的总线模式应该从"fast bus mode"变为"asynchronous bus mode" */
    mrc p15, 0, r1, c1, c0, 0   /* 读出控制寄存器 */
    orr r1, r1, #0xc0000000     /* 设置为"asynchronous bus mode" */
    mcr p15, 0, r1, c1, c0, 0   /* 写入控制寄存器 */

    /* FCLK is 405MHz */
    ldr r0, =0x4c000004
    ldr r1, =(0x7f << 12)
    add r1, r1, #0x21
    str r1, [r0]

/* 4. clean instruction and data buffer */
    mov r0, #0
	mcr p15, 0, r0, c7, c7, 0	/* flush v3/v4 cache */
	mcr p15, 0, r0, c8, c7, 0	/* flush v4 TLB */

/* 5. disable MMU */
    mrc p15, 0, r0, c1, c0, 0
	bic r0, r0, #0x00002300
	bic r0, r0, #0x00000087
	orr r0, r0, #0x00000002
	orr r0, r0, #0x00001000
	mcr p15, 0, r0, c1, c0, 0
	
/* 6. init SDRAM */
    ldr r0, =0x48000000
    adr r1, sdram_config        /* sdram_config的当前地址 */
    add r3, r0, #(13*4)

1:
    ldr r2, [r1], #4
    str r2, [r0], #4
    cmp r0, r3
    bne 1b

/* 7. set stack */
	ldr sp, =0x1000

/* 8. init nand, copy code to memory, clear bss */
    bl clear_bss
    bl nand_init
    bl copy_code_to_sdram
    
/* 9. bl main */
    ldr pc, =main

sdram_config:
    .long 0x22000000
    .long 0x00000700
    .long 0x00000700
    .long 0x00000700
    .long 0x00000700
    .long 0x00000700
    .long 0x00000700
    .long 0x00018001
    .long 0x00018001
    .long 0x008c04f5
    .long 0x000000b1
    .long 0x00000030
    .long 0x00000030

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


