/*
 ============================================================================
 Name        : CIA6526.c
 Author      : Sampo Peltonen
 Licence     : GNU General Public License, version 2.
 Copyright   : Copyright (C) 2013  Sampo Peltonen
 Description : CIA6526 simulation
 ============================================================================
 */

#include "types.h"
#include "CIA6526.h"
#include "stdlibtools.h"
#include "autotestkeyboard.h"

typedef struct {
	/* addresses actually only needed for debugging purposes, TODO: remove later */
        void (*write)(cia_state*, byte, word /*address*/);
	byte (*read)(cia_state*, word /*address*/);
} ciaReg_s;

ciaReg_s ciaReg[0x10];



void doTimerCycle(cia_state* cia) {
	if(cia->timerAControlReg & B1) {
		// timer is running and counts system cycles
		cia->timerACurrentValue--;
		if(cia->timerACurrentValue==0xffff) {
			// Underflow
			cia->interruptStatus |= B1;   //timerA underflow occurred 
			
			// generate interrupt?
			if(cia->interruptMask&B1) {
				cia->irqHandler();
			}
			
			//printf1("CIA-%d timerA underflow", cia->cia_number);
			if(cia->timerAControlReg&B4) {
				// timer stops
				cia->timerAControlReg&=0b11111110;
			}
			else {
				// timer reloads
				cia->timerACurrentValue = cia->timerALatchValue;	
			}
		}
	}
	if(cia->timerBControlReg&B1) {
		//TODO: cnt pin counting not implemented
		if(cia->timerBControlReg&B7) {
			// timer counts timerA underflows
			if(cia->interruptStatus & B1) cia->timerBCurrentValue--;
		}
		else {
			// timer counts system cycles
			cia->timerBCurrentValue--;
		}
		if(cia->timerBCurrentValue==0xffff) {
			// Underflow
			cia->interruptStatus |= B2;
			//printf1("CIA-%d timerB underflow", cia->cia_number);
			if(cia->timerBControlReg&B4) {
				// timer stops
				cia->timerBControlReg&=0b11111110;
			}
			else {
				// timer reloads
				cia->timerBCurrentValue = cia->timerALatchValue;	
			}
		}
	}
}

void cia6526_cycle(cia_state* cia) {

	doTimerCycle(cia);

	// TODO: update TOD
}


byte cia6526_readByte(cia_state *cia, word address) {
	return ciaReg[address].read(cia, address);
}

/**
 * address should be 0 - F
 */
void cia6526_writeByte(cia_state* cia, word address, byte data) {
	ciaReg[address].write(cia, data, address);
}

void interruptControlAndStatusWrite(cia_state* cia, byte data, word address) {
	if((data & B8)!=0) {	// setting according the bit mask
		cia->interruptMask |= data;
		//printf3("CIA-%d int.mask set %x -> %x",cia->cia_number,data,cia->interruptMask);
	}
	else {			// clearing according the bit mask
		cia->interruptMask &= ~data;
		//printf3("CIA-%d int.mask clear %x -> %x",cia->cia_number,data,cia->interruptMask);
	}
}

byte interruptControlAndStatusRead(cia_state* cia, word address) {
	return 1;
}


void timerAControlRegWrite(cia_state* cia, byte data, word address) {
	cia->timerAControlReg = data;
	//printf2("CIA-%d timerA ctrl reg write %x",cia->cia_number,data);
}
void timerBControlRegWrite(cia_state* cia, byte data, word address) {
	cia->timerBControlReg = data;
	//printf2("CIA-%d timerB ctrl reg write %x",cia->cia_number,data);
}
void portADataDirRegWrite(cia_state* cia, byte data, word address) {
	cia->portADataDirReg = data;
	//printf2("CIA-%d portA d.dir reg write %x",cia->cia_number,data);
}
void portBDataDirRegWrite(cia_state* cia, byte data, word address) {
	cia->portBDataDirReg = data;
	//printf2("CIA-%d portB d.dir reg write %x",cia->cia_number,data);
}

//byte keybCol;
//byte keyDown;
//byte nokey = 0x0;
void portAWrite(cia_state* cia, byte data, word address) {
	if(cia->cia_number==1) {
		autoKeybSelectColumn(data);
		//keybCol = data;
		//printf2("CIA-%d portA write %x",cia->cia_number,data);
	}
	// testing 
}

byte portBRead(cia_state* cia, word address) {
	if(/*keyDown++>200 &&*/ cia->cia_number==1) {
		return autoKeybReadRow();	
		/*if(keybCol==0) {
			// kernal is checking if anything is pressed (non-ff input)
			return 1;
		}
		if(keybCol==0b11101111) {
			printf1("portB read %x",0b11101111);
			return(0b11101111); // V,B,N tai M
		}*/
	}
	
	return 0xff;
}

void unimplementedRegisterWrite(cia_state* cia, byte data, word address) {
	//printf3("CIA-%d[%x] un.impl. write %x",cia->cia_number, address, data);
}

byte unimplementedRegisterRead(cia_state* cia, word address) {
	//printf2("CIA-%d[%x] un.impl. read",cia->cia_number, address);
	return 0;
}

byte timerAValueReadLow (cia_state* cia, word address) { return (byte) cia->timerACurrentValue; }
byte timerAValueReadHigh(cia_state* cia, word address) { return (byte) (cia->timerACurrentValue>>8); }
byte timerBValueReadLow (cia_state* cia, word address) { return (byte) cia->timerBCurrentValue; }
byte timerBValueReadHigh(cia_state* cia, word address) { return (byte) (cia->timerBCurrentValue>>8); }

void timerAValueLatchLow (cia_state* cia, byte data, word address) { cia->timerALatchValue = (cia->timerALatchValue&0xff00) + data; }
void timerAValueLatchHigh(cia_state* cia, byte data, word address) { cia->timerALatchValue = (cia->timerALatchValue&0x00ff) + (((word)data)<<8); }
void timerBValueLatchLow (cia_state* cia, byte data, word address) { cia->timerBLatchValue = (cia->timerBLatchValue&0xff00) + data; }
void timerBValueLatchHigh(cia_state* cia, byte data, word address) { cia->timerBLatchValue = (cia->timerBLatchValue&0x00ff) + (((word)data)<<8); }

void cia_init() {
	ciaReg[0x0].write = &portAWrite;
	ciaReg[0x0].read  = &unimplementedRegisterRead;
	ciaReg[0x1].write = &unimplementedRegisterWrite;
	ciaReg[0x1].read  = &portBRead;
	ciaReg[0x2].write = &portADataDirRegWrite;
	ciaReg[0x2].read  = &unimplementedRegisterRead;
	ciaReg[0x3].write = &portBDataDirRegWrite;
	ciaReg[0x3].read  = &unimplementedRegisterRead;
	ciaReg[0x4].write = &timerAValueLatchLow;
	ciaReg[0x4].read  = &timerAValueReadLow;
	ciaReg[0x5].write = &timerAValueLatchHigh;
	ciaReg[0x5].read  = &timerAValueReadHigh;
	ciaReg[0x6].write = &timerBValueLatchLow;
	ciaReg[0x6].read  = &timerBValueReadLow;
	ciaReg[0x7].write = &timerBValueLatchHigh;
	ciaReg[0x7].read  = &timerBValueReadHigh;
	ciaReg[0x8].write = &unimplementedRegisterWrite;
	ciaReg[0x8].read  = &unimplementedRegisterRead;
	ciaReg[0x9].write = &unimplementedRegisterWrite;
	ciaReg[0x9].read  = &unimplementedRegisterRead;
	ciaReg[0xa].write = &unimplementedRegisterWrite;
	ciaReg[0xa].read  = &unimplementedRegisterRead;
	ciaReg[0xb].write = &unimplementedRegisterWrite;
	ciaReg[0xb].read  = &unimplementedRegisterRead;
	ciaReg[0xc].write = &unimplementedRegisterWrite;
	ciaReg[0xc].read  = &unimplementedRegisterRead;
	ciaReg[0xd].write = &interruptControlAndStatusWrite;
	//ciaReg[0xd].read  = &interruptControlAndStatusRead;
	//ciaReg[0xd].write = &unimplementedRegisterWrite;
	ciaReg[0xd].read  = &unimplementedRegisterRead;
	ciaReg[0xe].write = &timerAControlRegWrite;
	ciaReg[0xe].read  = &unimplementedRegisterRead;
	ciaReg[0xf].write = &timerBControlRegWrite;
	ciaReg[0xf].read  = &unimplementedRegisterRead;

	autoKeybInit();
}

