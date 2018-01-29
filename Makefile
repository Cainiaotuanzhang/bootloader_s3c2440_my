CROSS_COMPILE = arm-linux-
CC 	= $(CROSS_COMPILE)gcc
LD 	= $(CROSS_COMPILE)ld
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump


TOP_DIR     := $(PWD)
OBJ_DIR     := $(TOP_DIR)/obj
BIN         := boot.bin
SUB_DIR		:= arch/arm/cpu/arm920t \
			   drivers/nand \
			   drivers/uart \
			   board

CFLAGS 		:= -Wall -O2
CPPFLAGS	:= -nostdinc -nostdlib -fno-builtin
INCLUDE     := -I$(TOP_DIR)/include

export CC TOP_DIR OBJ_DIR BIN_DIR BIN CFLAGS CPPFLAGS INCLUDE

objs += $(OBJ_DIR)/start.o
objs += $(OBJ_DIR)/nand.o
objs += $(OBJ_DIR)/uart.o
objs += $(OBJ_DIR)/boot.o

all: CHECKDIR $(SUB_DIR) $(BIN)

CHECKDIR:
	mkdir -p $(OBJ_DIR)

$(SUB_DIR):ECHO
	$(MAKE) -C $@
ECHO:
	@echo $(SUB_DIR)

$(BIN): $(objs)
	$(LD) -T./board/boot.lds -o boot.elf $^
	$(OBJCOPY) -O binary -S boot.elf $@
	$(OBJDUMP) -D -m arm boot.elf > boot.dis
clean:
	rm -rf $(OBJ_DIR) *.bin *.dis *.elf


