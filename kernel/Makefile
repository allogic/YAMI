CC = gcc
LD = ld
OBJCOPY = objcopy

CFLAGS += -m64
CFLAGS += -mno-red-zone
CFLAGS += -nostdlib
CFLAGS += -ffreestanding
CFLAGS += -fno-stack-protector
CFLAGS += -fno-pic
CFLAGS += -Wall
CFLAGS += -Wextra

LFLAGS += -m64
LFLAGS += -nostdlib

SRCS += Kernel.c

OBJS += $(SRCS:.c=.o)

%.o: %.c
	$(CC) $(CFLAGS) -O0 -g -c $< -o $@

Kernel.elf: $(OBJS)
	$(LD) -T Linker.ld -o $@ $(OBJS)

Kernel.bin: Kernel.elf
	$(OBJCOPY) -O binary $< ../$@

Clean:
	rm -rf *.o *.elf

.PHONY: Kernel.elf Kernel.bin Clean