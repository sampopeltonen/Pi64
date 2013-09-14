#!/bin/sh

rm obj/*.o

CC_OPTS="-nostdlib -nostartfiles -ffreestanding -mcpu=arm1176jzf-s -mtune=arm1176jzf-s -O2"

arm-none-eabi-as src/bootstrap.s -o obj/bootstrap.o

arm-none-eabi-as src/frameBuffer.s -o obj/frameBuffer.o
arm-none-eabi-as src/mailbox.s -o obj/mailbox.o
arm-none-eabi-as src/drawing.s -o obj/drawing.o
arm-none-eabi-as src/gpio.s -o obj/gpio.o
arm-none-eabi-as src/maths.s -o obj/maths.o
arm-none-eabi-as src/text.s -o obj/text.o
arm-none-eabi-as src/systemTimer.s -o obj/systemTimer.o

# 64 stuff
arm-none-eabi-gcc $CC_OPTS -c src/CIA6526.c -o obj/CIA6526.o
arm-none-eabi-gcc $CC_OPTS -c src/ioregarea.c -o obj/ioregarea.o
arm-none-eabi-gcc $CC_OPTS -c src/ram.c -o obj/ram.o
arm-none-eabi-gcc $CC_OPTS -c src/MOS6510.c -o obj/MOS6510.o
arm-none-eabi-gcc $CC_OPTS -c src/rasterlinetiming.c -o obj/rasterlinetiming.o
arm-none-eabi-gcc $CC_OPTS -c src/VIC6569.c -o obj/VIC6569.o
#arm-none-eabi-gcc $CC_OPTS -c src/graphicstools.c -o obj/graphicstools.o
arm-none-eabi-gcc $CC_OPTS -c src/stdlibtools.c -o obj/stdlibtools.o
arm-none-eabi-as src/roms.s -o obj/roms.o    # C64 rom images
arm-none-eabi-as src/cathoderay.s -o obj/cathoderay.o

# link
arm-none-eabi-ld -T lscript obj/bootstrap.o \
obj/gpio.o obj/mailbox.o obj/frameBuffer.o obj/drawing.o obj/maths.o obj/text.o obj/systemTimer.o \
obj/CIA6526.o \
obj/ioregarea.o \
obj/ram.o \
obj/MOS6510.o \
obj/rasterlinetiming.o \
obj/cathoderay.o \
obj/VIC6569.o \
obj/stdlibtools.o \
obj/roms.o \
-o kernel.elf

arm-none-eabi-objcopy kernel.elf -O binary kernel.img

