/*
 ============================================================================
 Name        : ioregarea.c
 Author      : Sampo Peltonen
 Licence     : GNU General Public License, version 2.
 Copyright   : Copyright (C) 2013  Sampo Peltonen
 Description : Commodore 64 simulation
 ============================================================================
 */

#include "ioregarea.h"
#include "stdlibtools.h"
//#include <stdio.h>
//#include <stdlib.h>
#include "VIC6569.h"
#include "CIA6526.h"
#include "MOS6510.h"

//                                 kyseessä
//                                 tämä alue
// $ffff +--------------+  /$e000 +----------+  +----------+
//       |  Kernal ROM  | /       |  I/O  2  |  |          |
// $e000 +--------------+/  $df00 +----------+  |          |
//       |I/O, Char ROM |         |  I/O  1  |  |          |
// $d000 +--------------+\  $de00 +----------+  |          |
//       |     RAM      | \       |  CIA  2  |  |          |
// $c000 +--------------+  \$dd00 +----------+  |          |
//       |  Basic ROM   |         |  CIA  1  |  |          |
// $a000 +--------------+   $dc00 +----------+  | Char ROM |
//       |              |         |Color RAM |  |          |
//       .     RAM      .         |          |  |          |
//       .              .   $d800 +----------+  |          |
//       |              |         |   SID    |  |          |
// $0002 +--------------+         |registers |  |          |
//       | I/O port DR  |   $d400 +----------+  |          |
// $0001 +--------------+         |   VIC    |  |          |
//       | I/O port DDR |         |registers |  |          |
// $0000 +--------------+   $d000 +----------+  +----------+


// visibility at loc 0xd000 - 0xdfff
byte* ioarea[0x1000];



// unimplemented things should point here
byte unimplementedIOStuff[0x1000];


void initIoRegArea() {
	int i;

	// init unimplemented area
	for(i=0; i<0x1000; i++) {
		ioarea[i] = &unimplementedIOStuff[i];
	}

	cia_init();

	cia1_state.cia_number =1;
	cia1_state.irqHandler = &mos_irq;
	
	cia2_state.cia_number =2;
	cia2_state.irqHandler = &mos_nmi;

}

void cycleIoRegArea() {
	cia6526_cycle(&cia1_state);
	cia6526_cycle(&cia2_state);
}

/*
void checkAddress(word address) {
	if(address>=0x1000) {
		//printf("illegal address access to ioarea, should be 0-0xfff , was 0x%x\n", address);
		//exit(1);
	}
}*/

byte ioAreaReadByte(word address) {
	//checkAddress(address);

	// CIA reads?
	if(address>=0xc00 && address<=0xc0f) {
		//printf("CIA1 read address 0xdc0%x\n", address-0xc00);
		return cia6526_readByte(&cia1_state, address-0xc00);
	}
	if(address>=0xd00 && address<=0xd0f) {
		//printf("CIA2 read address 0xdd0%x\n", address-0xd00);
		return cia6526_readByte(&cia2_state, address-0xd00);
	}
	// VIC-II reads in area 0-3ff?
	if(address<=0x3ff) {
		// vic registers repeat in every 64 bytes in this area
		// so address space seems to be 6 bits
		return vic6569_readByte(address & 0b111111);
	}
	return(*ioarea[address]);
}

void ioAreaWriteByte(word address, byte data) {

	// CIA Writes?
	if(address>=0xc00 && address<=0xc0f) {
		cia6526_writeByte(&cia1_state, address-0xc00, data);
		return;
	}
	if(address>=0xd00 && address<=0xd0f) {
		cia6526_writeByte(&cia2_state, address-0xd00, data);
		return;
	}
	// VIC-II writes in area 0-3ff?
	if(address<=0x3ff) {
		// vic registers repeat in every 64 bytes in this area
		// so address space seems to be 6 bits
		vic6569_writeByte(address & 0b111111, data);
		return;
	}

	*ioarea[address] = data;
}
