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
#define UART_BRD        ((UART_CLK / (UART_BAUD_RATE * 16)) - 1)

void uart0_init(void)
{
    GPHCON &= ~(0xf << 4);
    GPHCON |= 0xa0;

    GPHUP &= ~(0x3 << 2);
    GPHUP |= 0x0c;

    ULCON0 = 0x3;
    UCON0 = 0x5;

    UFCON0 = 0x0;
    UMCON0 = 0x0;
    
    UBRDIV0 = UART_BRD;
}

extern void putc(unsigned char c);
int getc(void)
{
    int c;
    while (!(UTRSTAT0 & 0x1));

    c = URXH0;

    if (c == '\r')
        c = '\n';

    if (c == '\b')
    {
        putc('\b');
        putc(' ');
    }

    putc(c);
    return c;
}

char *gets(char *buf)
{
    int temp;
    char *tbuf = buf;

    while ((temp = getc()) != '\n')
    {
        *buf++ = temp;
    }
    *buf = '\0';

    return tbuf;
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
