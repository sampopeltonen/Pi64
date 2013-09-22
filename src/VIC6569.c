/*
 ============================================================================
 Name        : VIC6569.c
 Author      : Sampo Peltonen
 Licence     : GNU General Public License, version 2.
 Copyright   : Copyright (C) 2013  Sampo Peltonen
 Description : VIC6569 simulation
 ============================================================================
 */

//#include <stdio.h>
//#include <SDL/SDL.h>
#include "MOS6510.h"
#include "ram.h"
//#include "rom.h"
#include "VIC6569.h"
#include "ioregarea.h"
#include "rasterlinetiming.h"
#include "stdlibtools.h"
//#include "monitor.h"

// pal specific stuff
#define RASTER_LINES 312
#define VISIBLE_LINES 284
#define PIX_PER_LINE 504
#define VISIBLE_PIX_PER_LINE 403

extern byte kernalROM[];
extern byte basicROM[];
extern byte characterROM[];

extern void setupScreen(unsigned int, unsigned int, unsigned int);
extern void Wait(unsigned int);
extern void ledOn();
extern uint64_t GetTimeStamp();
//extern void setForeColour (unsigned int);
//extern void setBgColour (unsigned int);
extern word VICforeColour[];
extern word VICbgColour[];
extern void drawPixelOctet(unsigned int x, unsigned int y, unsigned int octet);

// controlled by DEN bit, inspected during rasterline 0x30
int displayEnabled = 0;

unsigned int  currentRasterLine = 0;
//word currentXCoord = 0;
unsigned int currentRLcycle = 0;   // 63 cycles per line in PAL machine (range 0-62)

uint32_t debugTimeMeasure = 0;
uint32_t debugTimeMeasureCount = 0;


#define BA_UP 1
#define BA_DOWN 0

#define AEC_RUNNING 1
#define AEC_DOWN 0

#define STATE_DISPLAY 1
#define STATE_IDLE 0

uint32_t colors[16];

int aecRunningCounter = 3;

int FLAG_BA  = BA_UP;
int FLAG_AEC = AEC_RUNNING; // switced automatically by FLAG_BA

int FLAG_STATE = STATE_IDLE;

word VC=0; // video counter 10 bits
word VCBASE=0; // video counter base 10 bits
byte RC=0; // row counter 3 bits
word VML[50]; // video matrix line data
byte VMLI=0; // video matrix line index

byte frame=0;

void drawCharacterOctet(unsigned int x, unsigned int y, byte data, unsigned int foreColor) {
	x-=120;
	VICbgColour[0] = colors[(vicRegisters[0x21] & (B1+B2+B3+B4))];
	VICforeColour[0] = foreColor;
	drawPixelOctet(x,y,data);
}


void updateRasterRegister() {
	vicRegisters[0x12] = currentRasterLine;
	if(currentRasterLine>0xff) vicRegisters[0x11] = vicRegisters[0x11] | B8;
	else vicRegisters[0x11] = vicRegisters[0x11] & ~B8;
}


/* Returns int 000-111 ie. 0-7 */
byte getYScroll() {
	return(vicRegisters[0x11] & 7); // kolme alinta bittiä
}

byte isDENbit() {
	return((vicRegisters[0x11] & B4)==B4);
}

// Address should be 0-$3fff because vic only has 14 address lines
// rest 3 bits are provided by CIA2 (pointing to bank 0-3)
byte vicMemReadByte(word address) {
	//printf1("vicmemread addr %x", address);
	if(address>0x3fff) {
		printf1("incorrect vic ram access: %x\n",address);
		exit(1);
	}

	// TODO: resolve the bank somehow (bits 15-16)
	//word bit15 = 0x4000;
	//word bit16 = 0x8000;

	// TODO: char rom should only visible in banks 0 and 2
	if(address>=0x1000 && address<0x2000) {
		byte ret = characterROM[address-0x1000];
		//printf1("ret %x",ret);
		return(ret);
	}

	return(memReadByte(address));
}

word vicGetCAddress() {
	//	 c-access Addresses
	//	 +----+----+----+----+----+----+----+----+----+----+----+----+----+----+
	//	 | 13 | 12 | 11 | 10 |  9 |  8 |  7 |  6 |  5 |  4 |  3 |  2 |  1 |  0 |
	//	 +----+----+----+----+----+----+----+----+----+----+----+----+----+----+
	//	 |VM13|VM12|VM11|VM10| VC9| VC8| VC7| VC6| VC5| VC4| VC3| VC2| VC1| VC0|
	//	 +----+----+----+----+----+----+----+----+----+----+----+----+----+----+

	// | $d018 |VM13|VM12|VM11|VM10|CB13|CB12|CB11|  - | Memory pointers

	//word tmp = ;
	//word caddress = ((vicRegisters[0x18] & (B5+B6+B7+B8)) << 6) + VC;
	//printf("ca=$%x ",caddress);
	//printf("vicRegisters[0x18]=$%x VC=%d c_address=$%x\n", vicRegisters[0x18], VC, caddress);
	return(((vicRegisters[0x18] & (B5+B6+B7+B8)) << 6) + VC);
}

word vicGetGAddress(byte rootAddress) {
	//	g-access Addresses
	//	 +----+----+----+----+----+----+----+----+----+----+----+----+----+----+
	//	 | 13 | 12 | 11 | 10 |  9 |  8 |  7 |  6 |  5 |  4 |  3 |  2 |  1 |  0 |
	//	 +----+----+----+----+----+----+----+----+----+----+----+----+----+----+
	//	 |CB13|CB12|CB11| D7 | D6 | D5 | D4 | D3 | D2 | D1 | D0 | RC2| RC1| RC0|
	//	 +----+----+----+----+----+----+----+----+----+----+----+----+----+----+
	word gaddress = ((vicRegisters[0x18] & (B2+B3+B4)) << 10) + (((word)rootAddress) << 3) + RC;
	//printf("vicRegisters[0x18]=$%x rootAddress=%x RC=%d g_address=$%x\n", vicRegisters[0x18], rootAddress, RC, gaddress);
	return(gaddress);
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



	if(currentRLcycle==62) { // beginning of new line, more precis. 0x18c
		currentRLcycle=0;
		currentRasterLine++;
		if(currentRasterLine>=312) {
			currentRasterLine = 0;
			VCBASE = 0;
			//RC = 0; // TODO: oma lisäys
			//printf(" *VCBASE=%d ",VCBASE);
			/* 
			if( SDL_Flip( tvscreen ) == -1 ) {
				printf("Failed to flip screen.");
				exit(1);
			}
			*/
		}

		// lets always disable the display in the beginning of the line 0x30
		if(currentRasterLine==0x30) {
			if(frame++==50) {
				frame=0;
				//printf2("50 frms avg = %d/%d us/cycle", debugTimeMeasure, debugTimeMeasureCount);
				debugTimeMeasure=0;
				debugTimeMeasureCount=0;
			}
			displayEnabled = 0;
		}
		// TODO: interrupt here
		updateRasterRegister();
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
		if((currentRasterLine & 7)==getYScroll() && currentRasterLine>=0x30 && currentRasterLine<=0xf7 && displayEnabled) {
			badLineCondition = 1;

			//The transition from idle to display state occurs as soon as there is a Bad
			//Line Condition (see section 3.5.).
			FLAG_STATE = STATE_DISPLAY;
		}


		//	In the first phase of cycle 14 of each line, VC is loaded from VCBASE
		//	(VCBASE->VC) and VMLI is cleared. If there is a Bad Line Condition in
		//	this phase, RC is also reset to zero.
		if(currentRLcycle==14) {
			VC = VCBASE;
			//printf(" VC=%d ",VC);
			VMLI = 0;
			if(badLineCondition) RC = 0;
		}

		//	In the first phase of cycle 58, the VIC checks if RC=7. If so, the video
		//	   logic goes to idle state and VCBASE is loaded from VC (VC->VCBASE). If
		//	   the video logic is in display state afterwards (this is always the case
		//	   if there is a Bad Line Condition), RC is incremented.
		if(currentRLcycle==58) {
			if(RC==7) {
				//The transition from display to idle
				//state occurs in cycle 58 of a line if the RC (see next section) contains
				//the value 7 and there is no Bad Line Condition.
				if(!badLineCondition) FLAG_STATE = STATE_IDLE;

				//printf("VCBASE=%d VC=%d\n", VCBASE,VC);
				VCBASE = VC;
				//printf(" VCBASE=%d ",VCBASE);
				RC=0;
			}
			else RC++;
		}


	// G-ACCESS
	if(rasterline_bad[currentRLcycle][0].busUsage==RLBG) {
		if(FLAG_STATE==STATE_DISPLAY) {
			// TODO: get real foreColor
			drawCharacterOctet(currentRLcycle*8, currentRasterLine, vicMemReadByte(vicGetGAddress(VML[VMLI])), colors[14]);

			// VC and VMLI are incremented after each g-access in display state.
			VC++;
			VMLI++;
		}
		else {
			drawCharacterOctet(currentRLcycle*8, currentRasterLine, 170, colors[14]);
		}
	}



	if(displayEnabled && currentRasterLine>=0x30 && currentRasterLine<=0xf7) {

		// C-ACCESS
		if(rasterline_bad[currentRLcycle][1].busUsage==RLBC && badLineCondition) {
			//putpixel(screen, currentRLcycle*8+4, currentRasterLine, colors[5]);
			word videomatrixaddress = vicGetCAddress();
			//printf1("C addr = %x",videomatrixaddress);
			byte cdata = vicMemReadByte(videomatrixaddress); // address to font data address
			VML[VMLI] = cdata;
		}
	}

	currentRLcycle++;

}


word drawRGB24toRGB565(byte r, byte g, byte b) {
  return ((r / 8) << 11) | ((g / 4) << 5) | (b / 8);
}

void vic6569_init() {
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
	init_stdlibtools();
	printf("raspi64 started");

	
	vic6569_init();
	//monitor_init();
	mos6510_init();
	//printf("initialization done");
	//printf("start main loop");
	mainLoop();

	//SDL_Quit();
	return 1;
}
