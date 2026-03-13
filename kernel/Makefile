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

all: kernel.bin

%.o: %.c
	$(CC) $(CFLAGS) -O0 -g -c $< -o $@

kernel.elf: $(OBJS)
	$(LD) -Tlinker.ld -o $@ $(OBJS)

kernel.bin: kernel.elf
	$(OBJCOPY) -O binary $< ../$@

clean:
	rm -rf *.o *.elf

.PHONY: all clean