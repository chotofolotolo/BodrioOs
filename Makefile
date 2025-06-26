# Makefile

# --- Herramientas ---
NASM = nasm
GCC = gcc
LD = ld
CAT = cat
RM = rm -f

# --- Flags de Compilación ---
NASMFLAGS = -f bin
CFLAGS = -m32 -ffreestanding -fno-pie -fno-pic -nostdlib -c -Wall -Wextra
LDFLAGS = -m elf_i386 -T link.ld

# --- QEMU ---
QEMU = qemu-system-i386 -fda os-image.bin -no-reboot -no-shutdown


# --- Objetivos ---
all: os-image

bootloader.bin: bootloader.asm
	$(NASM) $(NASMFLAGS) $< -o $@

kernel.o: kernel.c
	$(GCC) $(CFLAGS) $< -o $@

kernel.bin: kernel.o link.ld
	$(LD) $(LDFLAGS) $< -o $@

os-image: bootloader.bin kernel.bin
	$(CAT) $^ > $ os-image.bin

run: os-image.bin
	$(QEMU)

clean:
	$(RM) *.bin *.o os-image.bin

