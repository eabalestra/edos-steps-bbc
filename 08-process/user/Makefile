# cross compiler prefix

# GNU-Linux
PREFIX = riscv64-linux-gnu-
# for macos
# PREFIX = riscv64-unknown-elf-

CC = $(PREFIX)gcc
OBJDUMP = $(PREFIX)objdump
OBJCOPY = $(PREFIX)objcopy

# Compiler flags for compiling to riscv32 architecture standalone program (no
# runtime and standard library)
CFLAGS= -O -Wall -Wextra -MD -mcmodel=medany -march=rv32imazicsr -mabi=ilp32 \
        -ffreestanding -nostdlib -fno-common -fno-omit-frame-pointer \
		-fno-stack-protector -nostartfiles -static

PROGS = idle init producer consumer

# build the embedded file system (efs)
all: $(PROGS)
	./mkefs.sh $(PROGS) README

# build init
init: init.c edoslib.o usys.o
	$(CC) $(CFLAGS) -Wl,-Tuser.ld,--build-id=none -o init -e start \
	 edoslib.o usys.o init.c
	$(OBJDUMP) -d init > init.asm
	$(OBJCOPY) -S -O binary init init.bin

# build idle
idle: idle.c edoslib.o usys.o
	$(CC) $(CFLAGS) -Wl,-Tuser.ld,--build-id=none -o idle -e start \
	 edoslib.o usys.o idle.c
	$(OBJDUMP) -d idle > idle.asm
	$(OBJCOPY) -S -O binary idle idle.bin

# build producer
producer: producer.c edoslib.o usys.o
	$(CC) $(CFLAGS) -Wl,-Tuser.ld,--build-id=none -o producer -e start \
	 edoslib.o usys.o producer.c
	$(OBJDUMP) -d producer > producer.asm
	$(OBJCOPY) -S -O binary producer producer.bin

# build consumer 
consumer: consumer.c edoslib.o usys.o
	$(CC) $(CFLAGS) -Wl,-Tuser.ld,--build-id=none -o consumer -e start \
	 edoslib.o usys.o consumer.c
	$(OBJDUMP) -d consumer > consumer.asm
	$(OBJCOPY) -S -O binary consumer consumer.bin

# build edos library
edoslib.o: edoslib.c
	$(CC) $(CFLAGS) -c edoslib.c

usys.o: usys.s
	$(CC) $(CFLAGS) -c usys.s

clean:
	rm $(PROGS) *.o *.bin *.d *.asm
