AS = nasm
CC = gcc
LD = ld
#OBJCOPY = objcopy

CFLAGS += -m64
CFLAGS += -mno-red-zone
CFLAGS += -nostdlib
CFLAGS += -nodefaultlibs
CFLAGS += -ffreestanding
CFLAGS += -fno-stack-protector
CFLAGS += -fno-pic
CFLAGS += -ffunction-sections
CFLAGS += -Wall
CFLAGS += -Wextra

LFLAGS += -m64
LFLAGS += -nostdlib

ASM_SRCS += Startup.s

C_SRCS += Kernel.c
C_SRCS += FrameBuffer.c
C_SRCS += Terminal.c

OBJS = $(ASM_SRCS:.s=.o) $(C_SRCS:.c=.o)

Kernel: Kernel.elf

%.o: %.s
	$(AS) -f elf64 $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -O0 -g -c $< -o $@

Kernel.elf: $(OBJS)
	$(LD) -T Linker.ld -o ../$@ $(OBJS)

#Kernel.bin: Kernel.elf
#	$(OBJCOPY) -O binary $< ../$@

Clean:
	rm -rf *.o *.elf

.PHONY: Kernel Clean