raspi64
=======

This project is about emulating Commodore 64 on cheap card size computer Raspberry Pi without any actual operating system on the machine.

This is a hobby project for me to learn both the ARM assembly and the legendary home computer C64.

### Current Status

The project is on quite early state.

Current status is that the default VIC text mode somewhat works, the picture succesfully outputted from hdmi connector and C64 kernal boot process runs until available free memory is supposed to be calculated and should be outputted to screen.

CIA chips don't work.

SID chip (sound) is totally out of scope for any time soon.

Emulation is running slower than realtime. Should be much faster when MMU and data cache are succesfully enabled on the processor.

### Compilation and installation

Cross compilation tools for ARM architecture are required. I have been using [Yagarto](http://sourceforge.net/projects/yagarto/) toolchain on a Mac host. It should also work on Windows. This toolchain provides you with required tools: Assembler, C-compiler, linker and obj-file tool for converting ELF executable to raw executable binary file.

Project is standalone so it doesn't need any additional libraries.

Running the build script
	
	build.sh
	
should build and link all sources and produce emulator binary

	kernel.img
	
For running the emulator you need an SD card with FAT32 partition. In the root of this filesystem three files are needed:

	bootcode.bin
	kernel.img
	start.elf
	
bootcode.bin and start.elf are some kind of Raspberry Pi firmware that, to my understanding, govern the early boot process of loading the kernel.img to memory and then jumping to it. They are available [here](https://github.com/raspberrypi/firmware/tree/master/boot).

### Credits

Bootsrap code, framebuffer initialization, system timer and text output (for debugging purposes) come from Alex Chadwick's [Baking Pi](http://www.cl.cam.ac.uk/projects/raspberrypi/tutorials/os/index.html) tutorial.

Some code related to processor cache (and I hope in the future to MMU) originate from [David Welch's great tutorials and examples](https://github.com/dwelch67/raspberrypi).

C64 emulation is my own work and not derived from any existing emulator.
