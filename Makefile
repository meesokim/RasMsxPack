ARMGNU ?= arm-none-eabi

AOPS = --warn --fatal-warnings 
COPS = -Wall -Werror -O2 -nostdlib -nostartfiles -ffreestanding -fomit-frame-pointer 

gcc: rpmv0.hex rpmv0.bin

all: gcc clang

clean:
	rm -f *.o *.bin *.hex *.elf *.list *.img *.bc *.clang.opt.s

vectors.o : vectors.s
	$(ARMGNU)-as $(AOPS) vectors.s -o vectors.o

rpmv0.o : rpmv0.c
	$(ARMGNU)-gcc $(COPS) -c rpmv0.c -o rpmv0.o

rpmv0.elf : memmap vectors.o rpmv0.o 
	$(ARMGNU)-ld vectors.o rpmv0.o -T memmap -o rpmv0.elf
	$(ARMGNU)-objdump -D rpmv0.elf > rpmv0.list

rpmv0.bin : rpmv0.elf
	$(ARMGNU)-objcopy rpmv0.elf -O binary kernel.img

rpmv0.hex : rpmv0.elf
	$(ARMGNU)-objcopy rpmv0.elf -O ihex rpmv0.hex


LOPS = -Wall -m32 -emit-llvm
LLCOPS = -march=arm -mcpu=arm1176jzf-s
LLCOPS0 = -march=arm 
LLCOPS1 = -march=arm -mcpu=arm1176jzf-s
#COPS = -Wall  -O2 -nostdlib -nostartfiles -ffreestanding
#OOPS = -std-compile-opts
OOPS = -std-link-opts

clang : rpmv0.clang.hex rpmv0.clang.bin


rpmv0.clang.bc : rpmv0.c
	clang $(LOPS) -c rpmv0.c -o rpmv0.clang.bc

rpmv0.clang.opt.elf : memmap vectors.o rpmv0.clang.bc
	opt $(OOPS) rpmv0.clang.bc -o rpmv0.clang.opt.bc
	llc $(LLCOPS) rpmv0.clang.opt.bc -o rpmv0.clang.opt.s
	$(ARMGNU)-as rpmv0.clang.opt.s -o rpmv0.clang.opt.o
	$(ARMGNU)-ld -o rpmv0.clang.opt.elf -T memmap vectors.o rpmv0.clang.opt.o
	$(ARMGNU)-objdump -D rpmv0.clang.opt.elf > rpmv0.clang.opt.list

rpmv0.clang.hex : rpmv0.clang.opt.elf
	$(ARMGNU)-objcopy rpmv0.clang.opt.elf rpmv0.clang.hex -O ihex

rpmv0.clang.bin : rpmv0.clang.opt.elf
	$(ARMGNU)-objcopy rpmv0.clang.opt.elf -O binary kernel.img 


