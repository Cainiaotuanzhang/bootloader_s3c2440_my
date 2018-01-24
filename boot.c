#include <setup.h>

extern void uart0_init(void);
extern void nand_read(unsigned int addr, unsigned char *buf, unsigned int len);
extern void puts(char *str);
extern void puthex(unsigned int val);
extern int getc(void);

static struct tag *params;

void setup_start_tag(void)
{
    params = (struct tag *)0x30000100;

    params->hdr.tag = ATAG_CORE;
    params->hdr.size = tag_size(tag_core);

    params->u.core.flags = 0;
    params->u.core.pagesize = 0;
    params->u.core.rootdev = 0;

    params = tag_next(params);
}

void setup_memory_tags(void)
{
    params->hdr.tag = ATAG_MEM;
    params->hdr.size = tag_size(tag_mem32);

    params->u.mem.start = 0x30000000;
    params->u.mem.size = 64*1024*1024;

    params = tag_next(params);
}

int strlen(char *str)
{
    int i = 0;

    while (str[i])
    {
        i++;
    }

    return i;
}

void strcpy(char *dest, const char *src)
{
    while ((*dest++ = *src++) != '\0');
}

void setup_commandline_tag(char *cmdline)
{
    int len = strlen(cmdline) + 1;

    params->hdr.tag = ATAG_CMDLINE;
    params->hdr.size = (sizeof(struct tag_header) + len + 3) >> 2;

    strcpy(params->u.cmdline.cmdline, cmdline);
    params = tag_next(params);
}

void setup_end_tag(void)
{
    params->hdr.size = 0;
    params->hdr.tag = ATAG_NONE;
}

int main( void )
{
    void (*theKernel)(int zero, int arch, unsigned int params);
    //volatile unsigned int *p = (volatile unsigned int *)0x30008000;
    /* 1. 帮内核设置串口：内核启动的开始部分会从串口打印一些log，但是内核一开始还没有初始化串口 */
    uart0_init();
    puts("\r\n---------welcome----------\r\n");
    while(1)
    {
        puts("\n\r[boot @ boot_my]:");
        while (1)
        {
            if (getc() == '\n')
            {
                puts("\rNo commander,  unknow.");
                break;
            }
        }
    }


    /* 2. 串口打印 */

    /* 3. 设置参数 */
    puts("Set boot params\n\r");
    setup_start_tag();
    setup_memory_tags();
    setup_commandline_tag("noinitrd root=/dev/mtdblock3 init=/linuxrc console=ttySAC0,115200");
    setup_end_tag();

    /* 4. 跳转执行 */
    puts("Boot kernel\n\r");
    theKernel = (void (*)(int, int, unsigned int))0x30008000;
    theKernel(0, 362, 0x30000100);

    puts("Error!\n\r");
    return -1;
}
