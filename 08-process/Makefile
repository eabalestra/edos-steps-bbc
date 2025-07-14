QEMU=qemu-system-riscv32

# cross compiler prefix

# GNU-Linux
PREFIX = riscv64-linux-gnu-
# for macos
# PREFIX = riscv64-unknown-elf-

CC = $(PREFIX)gcc
OBJDUMP = $(PREFIX)objdump

# Compiler flags for compiling to riscv32 architecture standalone program (no
# runtime and standard library)
CFLAGS = -O -Wall -Wextra -MD -mcmodel=medany -march=rv32imazicsr -mabi=ilp32 \
         -ffreestanding -nostdlib -fno-common -fno-omit-frame-pointer \
		 -fno-stack-protector -fno-pie -no-pie -nostartfiles -fmax-errors=1

# build userland programs and the kernel
# 1. Build user programs
# 2. Compile kernel.
# 3. Extract assembly code from kernel binary using objdump 
kernel: *.s *.c user/*.c user/*.s
	make -C user
	$(CC) $(CFLAGS) -T kernel.ld -Wl,--build-id=none,-Map=kernel.map -o kernel \
	arch.s trap.s *.c
	$(OBJDUMP) -d kernel > kernel.asm

# Start QEMU. Firmware loads kernel binary at 0x80000000 and jump there.
qemu: kernel
	$(QEMU) -machine virt -bios none -nographic  -smp 1 \
	        -kernel kernel

# clean generated files
clean:
	make -C user clean
	rm kernel.map *.asm kernel
