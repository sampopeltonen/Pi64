Pi64
====

This project is about emulating Commodore 64 on a low-cost, card size computer, Raspberry Pi without any operating system on the machine. So it's running, as they say, on "bare metal".

![screenshot](https://raw.github.com/sampopeltonen/pi64/master/ScreenShot.png)

Nice thing with running on bare metal is that the program starts instantly when you switch the power on.

### Current Status

The project is a work in progress and **is not yet a fully functional C64 emulator**.

The current status is that the machine boots up properly and is able to run at least simple basic programs. The default VIC text mode somewhat works, but it's still without borders.

I have designed a small adapter for connecting a real C64 keyboard to Raspberry Pi's GPIO pins. With the adapter only 4 of the Pi's GPIO pins are used for the connection. I'll add the schematics for the adapter as soon as I have them drawn somehow.

![screenshot](https://raw.github.com/sampopeltonen/pi64/master/docs/konehuone.png)

CIA chips are partly functional: timers somewhat work, interrupts based on timers are generated (and handled by the processsor) and input/output with the keyboard (through the adapter) works.

SID chip (sound) is out of the scope for any time soon.

VIC chip needs much additional work.

Emulation speed is faster than real-time, about 110%. Needs further optimizing.

### Compilation and installation

Cross compilation tools for ARM architecture are required. I have been using [Yagarto](http://sourceforge.net/projects/yagarto/) toolchain on a Mac host. It should also work on Windows. This toolchain provides you with required tools: Assembler, C-compiler, linker and obj-file tool for converting ELF executable to raw executable binary file.

Project is standalone so it doesn't need any additional libraries.

Running the build script (there's no Makefile yet)
	
	build.sh
	
should build and link all sources and produce emulator binary

	kernel.img
	
For running the emulator you need regular SD card that works with Pi. In the root of this filesystem three files are needed:

	bootcode.bin
	kernel.img
	start.elf
	
bootcode.bin and start.elf are some kind of Raspberry Pi firmware that, to my understanding, govern the early boot process and then loading the kernel.img to memory and then jumping to it. The latest versions are available [here](https://github.com/raspberrypi/firmware/tree/master/boot) or the ones I'm using (I hope the latest) can be found in the PiFirmware folder.

### Credits

All the C64 emulation code is my own and some of the low level graphics output also, but some the code is from other people.

Bootstrap code, framebuffer initialization and output, GPIO read/write, system timer and text output (for debugging purposes) come from Alex Chadwick's [Baking Pi](http://www.cl.cam.ac.uk/projects/raspberrypi/tutorials/os/index.html) tutorial.

Code related to processor l1 cache and MMU initialization originate from [David Welch's Pi examples](https://github.com/dwelch67/raspberrypi).
