/* NAND FLASH控制器 */
#define NFCONF  (*(volatile unsigned long *)0x4E000000)
#define NFCONT  (*(volatile unsigned long *)0x4E000004)
#define NFCMMD  (*(volatile unsigned long *)0x4E000008)
#define NFADDR  (*(volatile unsigned long *)0x4E00000C)
#define NFDATA  (*(volatile unsigned long *)0x4E000010)
#define NFSTAT  (*(volatile unsigned long *)0x4E000020)

/* GPIO */
#define GPHCON  (*(volatile unsigned long *)0x56000070)
#define GPHUP   (*(volatile unsigned long *)0x56000078)

/* UART REGISTERS*/
#define ULCON0      (*(volatile unsigned long *)0x50000000)
#define UCON0       (*(volatile unsigned long *)0x50000004)
#define UFCON0      (*(volatile unsigned long *)0x50000008)
#define UMCON0      (*(volatile unsigned long *)0x5000000C)
#define UTRSTAT0    (*(volatile unsigned long *)0x50000010)
#define UTXH0       (*(volatile unsigned long *)0x50000020)
#define URXH0       (*(volatile unsigned long *)0x50000024)
#define UBRDIV0     (*(volatile unsigned long *)0x50000028)

#define TXD0READY   (1 << 2)

#define PCLK            (50000000)
#define UART_CLK        PCLK
#define UART_BAUD_RATE  (115200)
#define UART_BRD        ((UART_CLK / (UART_BAUD_RATE *16)) - 1) 

void copy_code_to_sdram(unsigned char *src, unsigned char *dest, unsigned int len)
{
    int i = 0;

    while (i < len)
    {
        dest[i] = src[i];
        i++;
    }
}

void clear_bss(void)
{
    extern int __bss_start, __bss_end;
    int *p = &__bss_start;

    for ( ; p < &__bss_end; p++)
        *p = 0;
}

void nand_init(void)
{
#define TACLS 0 
#define TWRPH0  1
#define TWRPH1  0
    /* 设置时序 */
    NFCONF = (TACLS << 12) | (TWRPH0 << 8) | (TWRPH1 << 4);
    /* 是能NAND Flash控制器，初始化ECC, 禁止片选 */
    NFCONT = (1 << 4) | (1 << 1) | (1 << 0);
}

void nand_select(void)
{
    NFCONT &= ~(1 << 1);
}

void nand_deselect(void)
{
    NFCONT |= (1 << 1);
}

void nand_cmd(unsigned char cmd)
{
    volatile int i;
    NFCMMD = cmd;
    for (i = 0; i < 10; i++);
}

void nand_addr(unsigned int addr)
{
    unsigned int col = addr % 2048;
    unsigned int page = addr / 2048;
    volatile int i;

    NFADDR = col & 0xff;
    for (i = 0; i < 10; i++);
    NFADDR = (col >> 8) & 0xff;
    for (i = 0; i < 10; i++);

    NFADDR = page & 0xff;
    for (i = 0; i < 10; i++);
    NFADDR = (page >> 8) & 0xff;
    for (i = 0; i < 10; i++);
    NFADDR = (page >> 16) & 0xff;
    for (i = 0; i < 10; i++);
}

void nand_wait_ready(void)
{
    while (!(NFSTAT & 1));
}

unsigned char nand_data(void)
{
    return NFDATA;
}

void nand_read(unsigned int addr, unsigned char *buf, unsigned int len)
{
    int col = addr % 2048;
    int i = 0;

    /* 1.选中 */
    nand_select();

    while (i < len)
    {
        /* 2.发出读命令00h */
        nand_cmd(0x00);

        /* 3.发出地址(分5步发出) */
        nand_addr(addr);

        /* 4.发出读命令30h */
        nand_cmd(0x30);

        /* 5.判断状态 */
        nand_wait_ready();

        /* 6.读数据 */
        for ( ; (col < 2048) && (i < len); col++)
        {
            buf[i] = nand_data();
            i++;
            addr++;
        }

        col = 0;
    }

    /* 7.取消选中 */
    nand_deselect();
}

void uart0_init(void)
{
    GPHCON |= 0xa0;
    GPHUP = 0x0c;

    ULCON0 = 0x03;
    UCON0 = 0x05;
    UFCON0 = 0x00;
    UMCON0 = 0x00;
    UBRDIV0 = UART_BRD;
}

void putc(unsigned char c)
{
    while (!(UTRSTAT0 & TXD0READY));

    UTXH0 = c;
}

void puts(char *str)
{
    int i = 0;

    while (str[i])
    {
        putc(str[i]);
        i++;
    }
}

void puthex(unsigned int val)
{
    int i, j;

    puts("0x");

    for (i = 0; i < 8; i++)
    {
        j = (val >> ((7 - i) * 4)) & 0xf;
        if ((j >= 0) && (j <= 9))
            putc('0' + j);
        else
            putc('A' + j - 0xa);
    }
}


