CROSS_COMPILE = arm-linux-
CC 	= $(CROSS_COMPILE)gcc
LD 	= $(CROSS_COMPILE)ld
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump

CFLAGS 		:= -Wall -O2
CPPFLAGS	:= -nostdinc -nostdlib -fno-builtin

objs += start.o
objs += init.o
objs += boot.o


boot.bin:$(objs)
	$(LD) -Tboot.lds -o boot.elf $^
	$(OBJCOPY) -O binary -S boot.elf $@
	$(OBJDUMP) -D -m arm boot.elf > boot.dis
%.o:%.S
	$(CC) $(CPPFLAGS) $(CFLAGS) -Iinclude -o $@ -c $<
%.o:%.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -Iinclude -o $@ -c $<
clean:
	rm -rf *.o *.bin *.elf *.dis


