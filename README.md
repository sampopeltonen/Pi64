Pi64
====

This project is about emulating Commodore 64 on a very affordable, card sized computer, Raspberry Pi without any operating system on the machine. So it's running on "bare metal".

![screenshot](https://raw.github.com/sampopeltonen/pi64/master/docs/IMG_1678.jpg)|
-------|
___Emulator starts instantly when the power is switched on.___|

### Current Status

The project is a work in progress and **is not yet fully functional C64 emulator**.

Features so far:

* CPU logic works 99.8% (problem with the V flag in SBC in decimal mode)
* Illegal op-codes work (according to a test suite)
* Op-code cycle timing works (according to a test suite)
* Interrupts work (interrupt sequence might have a small timing problem but I'm not sure how to test that reliably)
* Borders almost fully work (in 38 column mode borders are off by 1 pixel to the right)
* Colors work (background, borders, characters...)
* Screen X and Y scrolling work
* 4/5 of valid VIC graphics modes work (ECM text mode missing)
* VIC rasterline interrupts work
* Most relevant parts of CIA chips work (timers, interrupts, i/o with keyboard)
* Commodore 64 keyboard works through a self made serial-parallel adapter and through the CIA chip implementation
* Emulation speed currently little bit faster than real time

Some of the major things that do not work yet:

* Sprites
* Any way to load software, this is quite a big question mark as how to proceed with (Test suite programs are compiled inside the binary and can be fed into C64 memory by pressing F7)
* joystick connections
* Sound

Possibility to connect real C64 cartridges to the Raspberry Pi would be ideal, but unfortunately it's not possible. To my understanding Raspberry Pi is too slow for the needed serial-parallel connection.

![konehuone](https://raw.github.com/sampopeltonen/pi64/master/docs/C64KeyboardAdapter.jpg) |
----|
___The adapter for connecting a real C64 keyboard to Raspberry Pi's GPIO pins. With the adapter only 4 of the Pi's GPIO pins are used for the connection. I'll add the schema for the adapter here as soon as I have it drawn somehow.___ |


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
