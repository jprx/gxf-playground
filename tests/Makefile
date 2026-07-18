TOOLCHAIN:=aarch64-elf-
CC:=$(TOOLCHAIN)gcc
OBJCOPY:=$(TOOLCHAIN)objcopy

CFLAGS:=-x assembler-with-cpp -ffreestanding -nostdlib

SRCS:=$(wildcard *.s)
OBJS:=$(patsubst %.s,%.o,$(SRCS))
BINS:=$(patsubst %.s,%.bin,$(SRCS))
LINKER:=link.ld

.PHONY: all
all: $(BINS)

%.o: %.s Makefile $(LINKER)
	$(CC) $(CFLAGS) $< -T $(LINKER) -o $@

%.bin: %.o
	$(OBJCOPY) -O binary $< $@

.PHONY: clean
clean:
	$(RM) $(OBJS) $(BINS)
