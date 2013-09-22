/*
 ============================================================================
 Name        : CIA6526.h
 Author      : Sampo Peltonen
 Licence     : GNU General Public License, version 2.
 Copyright   : Copyright (C) 2013  Sampo Peltonen
 Description : CIA6526 simulation
 ============================================================================
 */

#ifndef CIA6526_H_
#define CIA6526_H_

#include "types.h"

#define CIA_PORTA 0
#define CIA_PORTB 1
#define CIA_DDRA 2
#define CIA_DDRB 3
#define CIA_TIMERALO 4
#define CIA_TIMERAHI 5
#define CIA_TIMERBLO 6
#define CIA_TIMERBHI 7
#define CIA_TODTENTHS 8
#define CIA_TODSECS 9
#define CIA_TODMINS 10
#define CIA_TODHOURS 11
#define CIA_SERIAL 12
#define CIA_INTMASK 13
#define CIA_CRA 14
#define CIA_CRB 15


typedef struct {

	int cia_number;
	byte reg[0xf][2];

	byte interruptStatus;
	byte interruptMask;	//only lowest 5 bits are relevant
	
	word timerACurrentValue;
	word timerBCurrentValue;

	word timerALatchValue;
	word timerBLatchValue;

	byte portADataDirReg;
	byte portBDataDirReg;
	
	byte timerAControlReg;	
	byte timerBControlReg;

	void (*irqHandler)();  // function that this CIA calls when irq happens

	/*
	byte portA;			// address 0
	byte portB;			// address 1

	byte ddrA;			// address 2
	byte ddrB;			// address 3

	byte timerAValueLo;	// address 4 read only
	byte timerAValueHi;	// address 5 read only

	byte timerBValueLo;	// address 6 read only
	byte timerBValueHi;	// address 7 read only

	byte todTenths;		// address 8
	byte todSeconds;	// address 9
	byte todMinutes;	// address A
	byte todHours;		// address B  (am/pm style)

	byte timerAControl;	// address E
	byte timerBControl;	// address F

	byte timerALatchLo;	// address 4 write
	byte timerALatchHi;	// address 5 write

	byte timerBLatchLo;	// address 6 write
	byte timerBLatchHi;	// address 7 write
	 */
} cia_state;


cia_state cia1_state;
cia_state cia2_state;


byte cia6526_readByte(cia_state* cia, word address);
void cia6526_writeByte(cia_state* cia, word address, byte data);

void cia6526_cycle(cia_state* cia);

void cia_init();

#endif /*CIA6526_H_*/
