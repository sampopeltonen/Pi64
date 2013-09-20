raspi64
=======

This project is about emulating Commodore 64 on cheap card size computer Raspberry Pi without any actual operating system on the machine.

This is a hobby project for me to learn both the ARM assembly and the internals of the legendary home computer C64.

### Current Status

The project is on quite early state and **is not functional C64 emulator**.

Current status is that the default VIC text mode works barely enough that the C64 screen is successfully outputted and C64 kernal boot process runs through and startup messages are visible.

CIA chips are work in progress and don't work. So no keyboard or even blinking cursor as it depends on a CIA timer.

SID chip (sound) is totally out of scope for any time soon.

VIC chip needs much additional work.

Emulation speed is faster than real-time, about 125%.

### Compilation and installation

Cross compilation tools for ARM architecture are required. I have been using [Yagarto](http://sourceforge.net/projects/yagarto/) toolchain on a Mac host. It should also work on Windows. This toolchain provides you with required tools: Assembler, C-compiler, linker and obj-file tool for converting ELF executable to raw executable binary file.

Project is standalone so it doesn't need any additional libraries.

Running the build script (need to make decent Makefile asap)
	
	build.sh
	
should build and link all sources and produce emulator binary

	kernel.img
	
For running the emulator you need an SD card with FAT32 partition. In the root of this filesystem three files are needed for running the emulator on the Raspberry Pi:

	bootcode.bin
	kernel.img
	start.elf
	
bootcode.bin and start.elf are some kind of Raspberry Pi firmware that, to my understanding, govern the early boot process of loading the kernel.img to memory and then jumping to it. They are available [here](https://github.com/raspberrypi/firmware/tree/master/boot).

### Credits

All the C64 emulation code is my own and some of the low level graphics output also, but some the code is from other people.

Bootsrap code, framebuffer initialization, system timer and text output (for debugging purposes) come from Alex Chadwick's [Baking Pi](http://www.cl.cam.ac.uk/projects/raspberrypi/tutorials/os/index.html) tutorial.

Code related to processor l1 cache and MMU initialization originate from [David Welch's Pi examples](https://github.com/dwelch67/raspberrypi).
