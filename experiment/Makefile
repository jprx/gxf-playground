
TOOLCHAIN:=aarch64-elf-
CC:=$(TOOLCHAIN)gcc
OBJCOPY:=$(TOOLCHAIN)objcopy

SRC:=kernel
LINKER:=link.ld

$(SRC).bin: $(SRC).s Makefile $(LINKER)
	$(CC) -x assembler-with-cpp $< -T link.ld -ffreestanding -nostdlib -o $(SRC).o
	$(OBJCOPY) -O binary $(SRC).o $@

.PHONY: clean
clean:
	rm -f kernel.bin kernel.o
