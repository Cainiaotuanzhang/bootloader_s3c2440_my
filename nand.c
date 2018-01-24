/* NAND FLASH控制器 */
#define NFCONF  (*(volatile unsigned long *)0x4E000000)
#define NFCONT  (*(volatile unsigned long *)0x4E000004)
#define NFCMMD  (*(volatile unsigned long *)0x4E000008)
#define NFADDR  (*(volatile unsigned long *)0x4E00000C)
#define NFDATA  (*(volatile unsigned long *)0x4E000010)
#define NFSTAT  (*(volatile unsigned long *)0x4E000020)

#define NF_CE_L()       do{NFCONT &= ~(1 << 1);}while(0)                /* 打开片选 */
#define NF_CE_H()       do{NFCONT |= (1 << 1);}while(0)                 /* 关闭片选 */

#define NF_WAIT_RB()    do{while(!(NFSTAT & (1 << 0)));}while(0)        /* 等待NANDFLASH不忙 */
#define NF_DETECT_RB()  do{while(!(NFSTAT & (1 << 2)));}while(0)        /* 等待RnB信号变高，即不忙 */

extern void nand_read(unsigned int addr, unsigned char *buf, unsigned int len);

void clear_bss(void)
{
    extern int __bss_start, __bss_end;
    int *p = &__bss_start;

    for ( ; p < &__bss_end; p++)
        *p = 0;
}

void copy_code_to_sdram(unsigned char *addr, unsigned char *dest, unsigned int len)
{
    nand_read((unsigned int)addr, dest, len);
}

void nand_cmd(unsigned char cmd)
{
    volatile int i;
    NFCMMD = cmd;
    for (i = 0; i < 10; i++);
}


void nand_addr(unsigned int addr)
{

    unsigned int col  = addr % 2048;  
    unsigned int page = addr / 2048;
    volatile int i;  
  
    NFADDR = col & 0xff;  
    for (i = 0; i < 10; i++);  
    NFADDR = (col >> 8) & 0x0ff;  
    for (i = 0; i < 10; i++);  
      
    NFADDR  = page & 0xff;  
    for (i = 0; i < 10; i++);  
    NFADDR  = (page >> 8) & 0xff;  
    for (i = 0; i < 10; i++);  
    NFADDR  = (page >> 16) & 0xff;  
    for (i = 0; i < 10; i++); 

}

void nand_read(unsigned int addr, unsigned char *buf, unsigned int len)
{
    int i = 0;
    int col = addr % 2048;

    /* 1.选中，清除RnB信号 */
    NF_CE_L();

    while (i < len)
    {
        /* 2.发出读命令00h */
        nand_cmd(0x00);

        /* 3.发出地址(分5步发出) */
        nand_addr(addr);

        /* 4.发出读命令30h */
        nand_cmd(0x30);

        /* 5.判断状态 */
        NF_WAIT_RB();

        /* 6.读数据 */
        for (; (col < 2048) && (i < len); col++)
        {
            buf[i] = *((unsigned char *)(&NFDATA));
            addr++;
            i++;
        }
        
        col = 0;
    }

    /* 7.取消选中 */
    NF_CE_H();
}

void nand_init(void)
{
#define TACLS 0 
#define TWRPH0  1
#define TWRPH1  0
    /* 设置时序 */
    NFCONF = (TACLS << 12) | (TWRPH0 << 8) | (TWRPH1 << 4);
    /* 使能NAND Flash控制器，初始化ECC, 禁止片选 */
    NFCONT = (1 << 4) | (1 << 1) | (1 << 0);
}

