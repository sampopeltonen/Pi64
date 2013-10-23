/*
 ============================================================================
 Name        : VIC6569.c
 Author      : Sampo Peltonen
 Licence     : GNU General Public License, version 2.
 Copyright   : Copyright (C) 2013  Sampo Peltonen
 Description : VIC6569 simulation
 ============================================================================
 */

#include "MOS6510.h"
#include "ram.h"
#include "VIC6569.h"
#include "ioregarea.h"
#include "rasterlinetiming.h"
#include "stdlibtools.h"
#include "C64Keyboard.h"

extern uint32_t* graphicsAddress;

extern byte kernalROM[];
extern byte basicROM[];
extern byte characterROM[];

extern void setupScreen(unsigned int, unsigned int, unsigned int);
extern void Wait(unsigned int);
extern void ledOn();
extern uint64_t GetTimeStamp();

unsigned int displayEnabled = 0;

unsigned int outputting;

unsigned int currentRasterLine = 0;
unsigned int currentXCoord = 0;
unsigned int currentRLcycle = 0;   // 63 cycles per line in PAL machine (range 0-62)
unsigned int currentYScroll = 0;
unsigned int currentXScroll = 0;

uint32_t debugTimeMeasure = 0;
uint32_t debugTimeMeasureCount = 0;

byte vicRegisters[0x40];

#define BA_UP 1
#define BA_DOWN 0

#define AEC_RUNNING 1
#define AEC_DOWN 0

#define STATE_DISPLAY 1
#define STATE_IDLE 0

uint32_t colors[16];

int aecRunningCounter = 3;

int borderMainFlip=0;
int borderVertFlip=0;

int FLAG_BA  = BA_UP;
int FLAG_AEC = AEC_RUNNING; // switced automatically by FLAG_BA

int FLAG_STATE = STATE_IDLE;

word VC=0; // video counter 10 bits
word VCBASE=0; // video counter base 10 bits
byte RC=0; // row counter 3 bits
word VML[50]; // video matrix line data
byte VMLI=0; // video matrix line index

byte frame=0;

word interruptRasterLine=0;

word VICbgColor0;
word VICbgColor1;
word VICbgColor2;
word VICbgColor3;

#define X_MIN 80
#define X_MAX 474
#define Y_MIN 25
#define Y_MAX 280

// Valid graphics modes (value from bits ECM BMM MCM)
#define GM_STM 0
#define GM_MTM 1
#define GM_SBM 2
#define GM_MBM 3
#define GM_ETM 4

byte graphicsMode = 0;


/* TODO: move this framebuffer stuff to it's own file */

uint32_t gwidth;
uint16_t* gaddress;

// for some reason emulation almost 10% slower in this function is inlined, why?
void drawCharacterOctet(unsigned int x, unsigned int y, byte data, unsigned int foreColor) {
	if(y>=Y_MIN && y<Y_MAX) {
		x-=X_MIN;
		y=y-Y_MIN+100;

		uint16_t* address = gaddress + (y*gwidth+x);
		byte mask = 0b10000000;
		while(mask>0) {
			*address = (data&mask) ? foreColor : VICbgColor0;
			address += 1;
			mask = (mask >>1);
		}
	}
}

void drawCharacterOctetMTM(unsigned int x, unsigned int y, byte data, unsigned int foreColor) {
        if(y>=Y_MIN && y<Y_MAX) {
                x-=X_MIN;
                y=y-Y_MIN+100;

                uint16_t* address = gaddress + (y*gwidth+x);
                byte mask = 0b11000000;
		int i;
		for(i=4; i>0; i--) {
			switch(data&mask) {
				case 0:
					*(address++) = VICbgColor0;
					*(address++) = VICbgColor0;
					break;
				case 64:
					*(address++) = VICbgColor1;
					*(address++) = VICbgColor1;
					break;
				case 128:
					*(address++) = VICbgColor2;
					*(address++) = VICbgColor2;
					break;
				case 192:
					*(address++) = foreColor;
					*(address++) = foreColor;
					break;
			}
			data = data<<2;
		}
	}
}


void drawCharacterOctetSBM(unsigned int x, unsigned int y, byte data, unsigned int color0, unsigned int color1) {
	if(y>=Y_MIN && y<Y_MAX) {
		x-=X_MIN;
		y=y-Y_MIN+100;

		uint16_t* address = gaddress + (y*gwidth+x);
		byte mask = 0b10000000;
		while(mask>0) {
			*address = (data&mask) ? color1 : color0;
			address += 1;
			mask = (mask >>1);
		}
	}
}


void drawCharacterOctetMBM(unsigned int x, unsigned int y, byte data, unsigned int color0, unsigned int color1, unsigned int color2) {
	if(y>=Y_MIN && y<Y_MAX) {
		x-=X_MIN;
		y=y-Y_MIN+100;

		uint16_t* address = gaddress + (y*gwidth+x);
                byte mask = 0b11000000;
		int i;
		for(i=4; i>0; i--) {
			switch(data&mask) {
				case 0:
					*(address++) = VICbgColor0;
					*(address++) = VICbgColor0;
					break;
				case 64:
					*(address++) = color0;
					*(address++) = color0;
					break;
				case 128:
					*(address++) = color1;
					*(address++) = color1;
					break;
				case 192:
					*(address++) = color2;
					*(address++) = color2;
					break;
			}
			data = data<<2;
		}
	}
}


inline void drawBorderOctet(unsigned int x, unsigned int y, unsigned int color) {
	if(y>=Y_MIN && y<Y_MAX) {
		x-=X_MIN;
		y=y-Y_MIN+100;

		uint16_t* address = gaddress + (y*gwidth+x);
		*(address++) = color;
		*(address++) = color;
		*(address++) = color;
		*(address++) = color;
		*(address++) = color;
		*(address++) = color;
		*(address++) = color;
		*(address++) = color;
	}
}

/* end framebuffer stuff */






inline void updateRasterRegister() {
	vicRegisters[0x12] = currentRasterLine;
	if(currentRasterLine>0xff) vicRegisters[0x11] = vicRegisters[0x11] | B8;
	else vicRegisters[0x11] = vicRegisters[0x11] & ~B8;
}


/* Returns int 000-111 ie. 0-7 */
/*inline byte getYScroll() {
	return(vicRegisters[0x11] & 7); // kolme alinta bittiä
}*/

inline byte isDENbit() {
	return(vicRegisters[0x11] & B5);
}

/*
Address should be 0-$3fff because vic only has 14 address lines.
Highest 2 bits are provided by CIA2 (pointing to bank 0-3).

AFAIK:
Vic sees a data bus of 12 bits wide, highest 4 bits if this bus always
point to data in the color ram area starting from $d800 in the normal
address space. The address for this color ram access is 10 lowest bits
of the requested address.
*/
inline word vicMemReadCAccess(word address) {

	// TODO: resolve the bank (bits 15-16)
	//word bit15 = 0x4000;
	//word bit16 = 0x8000;

	// TODO: char rom should only visible in banks 0 and 2
	if(address>=0x1000 && address<0x2000) {
		byte ret = characterROM[address-0x1000];
		// TODO: figure out should the color ram also be accessed here?
		return(ret);
	}
	// read normally + highest bits from color ram
	return(memReadByte(address) + (memReadByte((address & 0b1111111111) + 0xd800)<<8 ));
}
inline byte vicMemRead(word address) {

	// TODO: resolve the bank (bits 15-16)
	//word bit15 = 0x4000;
	//word bit16 = 0x8000;

	// TODO: char rom should only visible in banks 0 and 2
	if(address>=0x1000 && address<0x2000) {
		byte ret = characterROM[address-0x1000];
		// TODO: figure out should the color ram also be accessed here?
		return(ret);
	}
	// read normally + highest bits from color ram
	return(memReadByte(address));
}

inline word vicGetCAddress() {
	//	 c-access Addresses
	//	 +----+----+----+----+----+----+----+----+----+----+----+----+----+----+
	//	 | 13 | 12 | 11 | 10 |  9 |  8 |  7 |  6 |  5 |  4 |  3 |  2 |  1 |  0 |
	//	 +----+----+----+----+----+----+----+----+----+----+----+----+----+----+
	//	 |VM13|VM12|VM11|VM10| VC9| VC8| VC7| VC6| VC5| VC4| VC3| VC2| VC1| VC0|
	//	 +----+----+----+----+----+----+----+----+----+----+----+----+----+----+

	// | $d018 |VM13|VM12|VM11|VM10|CB13|CB12|CB11|  - | Memory pointers

	return(((vicRegisters[0x18] & (B5+B6+B7+B8)) << 6) + VC);
}

inline word vicGetGAddress(byte rootAddress) {
	//	g-access Addresses
	//	 +----+----+----+----+----+----+----+----+----+----+----+----+----+----+
	//	 | 13 | 12 | 11 | 10 |  9 |  8 |  7 |  6 |  5 |  4 |  3 |  2 |  1 |  0 |
	//	 +----+----+----+----+----+----+----+----+----+----+----+----+----+----+
	//	 |CB13|CB12|CB11| D7 | D6 | D5 | D4 | D3 | D2 | D1 | D0 | RC2| RC1| RC0|
	//	 +----+----+----+----+----+----+----+----+----+----+----+----+----+----+
	word gaddress = ((vicRegisters[0x18] & (B2+B3+B4)) << 10) + (((word)rootAddress) << 3) + RC;
	return(gaddress);
}

inline word vicGetGAddressSBM() {
	//	g-access Addresses
	//	+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
	//	| 13 | 12 | 11 | 10 |  9 |  8 |  7 |  6 |  5 |  4 |  3 |  2 |  1 |  0 |
	//	+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
	//	|CB13| VC9| VC8| VC7| VC6| VC5| VC4| VC3| VC2| VC1| VC0| RC2| RC1| RC0|
	//	+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
	word gaddress = ((vicRegisters[0x18] & (B4)) << 10) + (VC << 3) + RC;
	return(gaddress);
}

inline int isYOnVerticalTopComparisonValueAndDenSet() {
	if(currentRasterLine==0x33 && (vicRegisters[0x11] & (B5+B4))==B5+B4) return 1;
	if(currentRasterLine==0x37 && (vicRegisters[0x11] & (B5+B4))==B5) return 1;
	return 0;
}

inline int isYOnVerticalBottomComparisonValue() {
	if(currentRasterLine==(0xfa+1) && (vicRegisters[0x11]&B4)) return 1;
	if(currentRasterLine==(0xf6+1) && (vicRegisters[0x11]&B4)==0) return 1;
	return 0;
}


void vicCycle() {
	if(FLAG_BA==BA_UP) {
		FLAG_AEC = AEC_RUNNING;
		aecRunningCounter = 3;
	}
	else {
		if(aecRunningCounter>0) aecRunningCounter--;
		else FLAG_AEC = AEC_DOWN;
	}

	currentXCoord += 8; // 8 pixels per cycle
	
	if(currentRLcycle==13) {
		// location where X coordinate starts over, actually it's 12.5 where it's 0
		currentXCoord = 4;
	}

	// open/close side borders (main)
	/*     |   CSEL=0   |   CSEL=1
         ------+------------+-----------
         Left  |  31 ($1f)  |  24 ($18)
         Right | 335 ($14f) | 344 ($158)	*/

	// Border 1. If the X coordinate reaches the right comparison value, the main border flip flop is set.
	if(currentXCoord==332 && (vicRegisters[0x16]&B4)==0) {
		borderMainFlip = 1;
	}
	if(currentXCoord==340 && (vicRegisters[0x16]&B4)) {
		borderMainFlip = 1;
	}


	if(currentRLcycle==0) { // beginning of new line
		//currentRLcycle=0;
		currentRasterLine++;
		if(currentRasterLine>=312) {
			currentRasterLine = 0;
			VCBASE = 0;
			//RC = 0;
		}

		// disable the display in the beginning of the line 0x30
		if(currentRasterLine==0x30) {
			if(frame++==50) {
				frame=0;
				printf2("50 frms avg = %d/%d us/cycle", debugTimeMeasure, debugTimeMeasureCount);
				debugTimeMeasure=0;
				debugTimeMeasureCount=0;
			}
			displayEnabled = 0;
		}

		if(vicRegisters[0x1a]&B1) {
			// rasterline interrupt enabled
			if(interruptRasterLine==currentRasterLine) {
				vicRegisters[0x19] |= B1;  // set rst interrupt status register
			}
			if(vicRegisters[0x19] & B1) {
				// TODO: what to do to bit #8 of the interrupt status register ???
				//printf("vic irq");
				mos_irq();
			}
			
		}

		updateRasterRegister();

		// Border 2. If the Y coordinate reaches the bottom comparison value in cycle 63, the vertical border flip flop is set.
		if(isYOnVerticalBottomComparisonValue()) borderVertFlip = 1;

		// Border 3. If the Y coordinate reaches the top comparison value in cycle 63
		//and the DEN bit in register $d011 is set, the vertical border flip flop is reset.
		if(isYOnVerticalTopComparisonValueAndDenSet()) borderVertFlip = 0;
			
	}
	/*
	Borders cont.
	4. If the X coordinate reaches the left comparison value and the Y
	   coordinate reaches the bottom one, the vertical border flip flop is set.
	5. If the X coordinate reaches the left comparison value and the Y
	   coordinate reaches the top one and the DEN bit in register $d011 is set,
	   the vertical border flip flop is reset.
	6. If the X coordinate reaches the left comparison value and the vertical
	   border flip flop is not set, the main flip flop is reset.
	*/
	if((currentXCoord==20 && (vicRegisters[0x16]&B4))  ||  (currentXCoord==28 && (vicRegisters[0x16]&B4)==0)) {
		// TODO: There's a bug in 38 col mode that borders are off by one pixel to the right.
		// 4.
		if(isYOnVerticalBottomComparisonValue())
			borderVertFlip = 1;
		// 5.
		else if(isYOnVerticalTopComparisonValueAndDenSet())
			borderVertFlip = 0;
		// 6.
		if(!borderVertFlip)
			borderMainFlip=0;
	}

	// is display enabled (DEN bit)
	if(currentRasterLine==0x30 && displayEnabled==0) {
		displayEnabled = isDENbit();
	}


	//		 A Bad Line Condition is given at any arbitrary clock cycle, if at the
	//		 negative edge of ø0 at the beginning of the cycle RASTER >= $30 and RASTER
	//		 <= $f7 and the lower three bits of RASTER are equal to YSCROLL and if the
	//		 DEN bit was set during an arbitrary cycle of raster line $30.
	//
	//		This definition has to be taken literally. You can generate and take away a
	//		Bad Line condition multiple times within an arbitrary raster line in the
	//		range of $30-$f7 by modifying YSCROLL, and thus make every raster line
	//		within the display window completely or partially a Bad Line, or trigger or
	//		suppress all the other functions that are connected with a Bad Line
	//		Condition. If YSCROLL=0, a Bad Line Condition occurs in raster line $30 as
	//		soon as the DEN bit (register $d011, bit 4) is set (for more about the DEN
	//		bit, see section 3.10.).
	byte badLineCondition = 0;
	if((currentRasterLine & 7)==currentYScroll && currentRasterLine>=0x30 && currentRasterLine<=0xf7 && displayEnabled) {
		badLineCondition = 1;

		//The transition from idle to display state occurs as soon as there is a Bad
		//Line Condition (see section 3.5.).
		FLAG_STATE = STATE_DISPLAY;
	}


	//	In the first phase of cycle 14 of each line, VC is loaded from VCBASE
	//	(VCBASE->VC) and VMLI is cleared. If there is a Bad Line Condition in
	//	this phase, RC is also reset to zero.
	if(currentRLcycle==13) {
		VC = VCBASE;
		VMLI = 0;
		if(badLineCondition) RC = 0;
	}

	//	In the first phase of cycle 58, the VIC checks if RC=7. If so, the video
	//	   logic goes to idle state and VCBASE is loaded from VC (VC->VCBASE). If
	//	   the video logic is in display state afterwards (this is always the case
	//	   if there is a Bad Line Condition), RC is incremented.
	if(currentRLcycle==57) {
		if(RC==7) {
			//The transition from display to idle
			//state occurs in cycle 58 of a line if the RC (see next section) contains
			//the value 7 and there is no Bad Line Condition.
			if(!badLineCondition) FLAG_STATE = STATE_IDLE;

			VCBASE = VC;
			RC=0;
		}
		else RC++;
	}


	// G-ACCESS
	if(rasterline_bad[currentRLcycle][0].busUsage==RLBG) {
		if(FLAG_STATE==STATE_DISPLAY) {
			if(!borderVertFlip && currentRLcycle>10 && currentRLcycle<59) {
				switch(graphicsMode) {
					case GM_MTM:
						// Multicolor Text Mode 0/0/1
						if(VML[VMLI] & 0b100000000000) {
							drawCharacterOctetMTM(currentRLcycle*8+currentXScroll,
									currentRasterLine,
									vicMemRead(vicGetGAddress(VML[VMLI])),
									colors[(VML[VMLI]>>8) & 0x7]);
							break;
						}
						// else fall through to STM mode below
					case GM_STM:
						// Standard Text Mode 0/0/0
						drawCharacterOctet(currentRLcycle*8+currentXScroll, 
									currentRasterLine, 
									vicMemRead(vicGetGAddress(VML[VMLI])), 
									colors[(VML[VMLI]>>8) & 0xf]);
						break;
					case GM_SBM:
						// Standard Bitmap Mode 0/1/0
						drawCharacterOctetSBM(currentRLcycle*8+currentXScroll, 
									currentRasterLine, 
									vicMemRead(vicGetGAddressSBM()), 
									colors[(VML[VMLI]) & 0xf],
									colors[(VML[VMLI]>>4) & 0xf]);
						break;
					case GM_MBM:
						// Multicolor Bitmap Mode 0/1/1
						// poke53265,59    (norm 27)
						// poke53270,216   (norm 200)
						drawCharacterOctetMBM(currentRLcycle*8+currentXScroll,
									currentRasterLine,
									vicMemRead(vicGetGAddressSBM()),
									colors[(VML[VMLI]) & 0xf],
									colors[(VML[VMLI]>>4) & 0xf],
									colors[(VML[VMLI]>>8) & 0xf]);
                                                break;
			

				}
			}
			// VC and VMLI are incremented after each g-access in display state.
			VC++;
			VMLI++;
		}
		else if(!borderVertFlip && currentRLcycle>10 && currentRLcycle<59) {
			// fills the gap with bgcolor if yscrolling in 25 row mode (POKE53265,24( or 31))
			// TODO: it seems, that in reality there should be weird black/bgcolor pattern
			drawCharacterOctet(currentRLcycle*8, currentRasterLine, 0x0, colors[vicRegisters[0x20] & 0xf]);
		}
	}
		
	if(borderMainFlip && currentRLcycle>10 && currentRLcycle<59) {
		// TODO: optimize, do not redraw borders if not needed
		drawBorderOctet(currentRLcycle*8, currentRasterLine, colors[vicRegisters[0x20]&0xf]);
	}


	if(displayEnabled && currentRasterLine>=0x30 && currentRasterLine<=0xf7) {
		// C-ACCESS
		if(rasterline_bad[currentRLcycle][1].busUsage==RLBC && badLineCondition) {
			word videomatrixaddress = vicGetCAddress();
			word chardata = vicMemReadCAccess(videomatrixaddress);
			VML[VMLI] = chardata;
		}
	}

	currentRLcycle++;
	if(currentRLcycle==63) currentRLcycle=0;
}


void vic6569_writeByte(byte address, byte data) {
	switch(address) {
		case 0x11:    // screen control register #1
			currentYScroll = data & 0x7;
			interruptRasterLine = (interruptRasterLine & 0xff) + ((data << 1) & 0x100);
			if (data&B6) graphicsMode |= B2; else graphicsMode &= ~B2;  // BMM
			if (data&B7) graphicsMode |= B3; else graphicsMode &= ~B3;  // ECM
			break;
		case 0x12:    // interrupt rasterline 8 lowest bits
			interruptRasterLine = (interruptRasterLine & 0x100) + data;
			return;
		case 0x16:    // screen control register #2
			currentXScroll = data & 0x7;
			if (data&B5) graphicsMode |= B1; else graphicsMode &= ~B1;    //MCM
			break;
		case 0x19:    // interrupt status register acknowledge
			vicRegisters[address] = ~data;
			return;
		case 0x21:
			VICbgColor0 = colors[data & 0xf];
			break;
		case 0x22:
			VICbgColor1 = colors[data & 0xf];
			break;
		case 0x23:
			VICbgColor2 = colors[data & 0xf];
			break;
		case 0x24:
			VICbgColor3 = colors[data & 0xf];
			break;

	}
	vicRegisters[address] = data;
}


byte vic6569_readByte(byte address) {
	//The unused addresses $2f-$3f give $ff on reading, a write access is ignored
	switch(address) {
		case 0x11:   // screen control register #1
			return (vicRegisters[address] & 0x7f) + ((currentRasterLine >> 1) & 0x80);
		case 0x12:   // current rasterline lowest 8 bits
			return currentRasterLine & 0xff;
	}
	return vicRegisters[address];
}




word drawRGB24toRGB565(byte r, byte g, byte b) {
  return ((r / 8) << 11) | ((g / 4) << 5) | (b / 8);
}

void vic6569_init() {

        gwidth = graphicsAddress[0];
        gaddress = (uint16_t*)graphicsAddress[8];

	int i;
	for(i=0; i<47; i++) {
		// TODO: what are the real initial values?
		vicRegisters[i] = 0;
	}

	// VIC-II colors as described in http://www.pepto.de/projects/colorvic/
	colors[0]  =  drawRGB24toRGB565(0x00, 0x00, 0x00);
	colors[1]  =  drawRGB24toRGB565(0xFF, 0xFF, 0xFF);
	colors[2]  =  drawRGB24toRGB565(0x68, 0x37, 0x2B);
	colors[3]  =  drawRGB24toRGB565(0x70, 0xA4, 0xB2);
	colors[4]  =  drawRGB24toRGB565(0x6F, 0x3D, 0x86);
	colors[5]  =  drawRGB24toRGB565(0x58, 0x8D, 0x43);
	colors[6]  =  drawRGB24toRGB565(0x35, 0x28, 0x79);
	colors[7]  =  drawRGB24toRGB565(0xB8, 0xC7, 0x6F);
	colors[8]  =  drawRGB24toRGB565(0x6F, 0x4F, 0x25);
	colors[9]  =  drawRGB24toRGB565(0x43, 0x39, 0x00);
	colors[10] =  drawRGB24toRGB565(0x9A, 0x67, 0x59);
	colors[11] =  drawRGB24toRGB565(0x44, 0x44, 0x44);
	colors[12] =  drawRGB24toRGB565(0x6C, 0x6C, 0x6C);
	colors[13] =  drawRGB24toRGB565(0x9A, 0xD2, 0x84);
	colors[14] =  drawRGB24toRGB565(0x6C, 0x5E, 0xB5);
	colors[15] =  drawRGB24toRGB565(0x95, 0x95, 0x95);

	rlinit();
	// set initial values
	vicRegisters[0x11] |= B4; // set DEN bit

	VC=0; // video counter 10 bits
	VCBASE=0; // video counter base 10 bits
	RC=0; // row counter 3 bits
	VMLI=0; // video matrix line index

	debugTimeMeasure =0;
	debugTimeMeasureCount =0;
	displayEnabled = 0;

	currentRasterLine = 0;
	currentRLcycle = 0;	
	aecRunningCounter = 3;

	FLAG_BA  = BA_UP;
	FLAG_AEC = AEC_RUNNING; // switced automatically by FLAG_BA

	FLAG_STATE = STATE_IDLE;

	VC=0; // video counter 10 bits
	VCBASE=0; // video counter base 10 bits
	RC=0; // row counter 3 bits
	VMLI=0; // video matrix line index
	frame=0;

}

void mainLoop() {
	//int delayCounter = 0;
	//int monitorUpdateCounter = 0;
	int run = 1;
	uint64_t t1;
	uint64_t t2;
	while(run) {
		t1 = GetTimeStamp();
		vicCycle();
		//t2 = GetTimeStamp(); printf1("%d",t2-t1);

		//Wait(100000); // 100000 about 1/5 sec
		
		cycleIoRegArea();
		
		if(FLAG_BA==BA_UP) mos6510_cycle();

		t2 = GetTimeStamp();
		debugTimeMeasure += (t2-t1);
		debugTimeMeasureCount++;

		//printf1("MOS %d",t2-t1);

		/*
		if(monitorUpdateCounter++==10000) {
			monitorUpdateCounter=0;
			monitorDrawMemBanks();
		}*/

		/*
		if(delayCounter++==50000) {
			delayCounter=0;
			//SDL_Delay(1);
			//SDL_PollEvent(&keyevent);
			//if(keyevent.type==SDL_KEYDOWN && keyevent.key.keysym.sym==SDLK_ESCAPE) run=0;
		}
		*/

	}

}


int vicmain(void) {
	//setupScreen(1024,768,16);
	setupScreen(640,480,16);
	//setupScreen(320,240,16);
	
	//setupScreen(400,300,16); // close to "real"
	
	init_stdlibtools();
	//printf("init keyboard...");
	//initKeyboard();
	printf("* Pi64 *");
	
	vic6569_init();
	//monitor_init();
	mos6510_init();
	//printf("initialization done");
	//printf("start main loop");

	C64Keyboard_init();

	mainLoop();

	//SDL_Quit();
	return 1;
}
