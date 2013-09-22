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

	// init unimplemented area (whole area at this point)
	for(i=0; i<0x1000; i++) {
		ioarea[i] = &unimplementedIOStuff[i];
	}


	//The VIC registers are repeated each 64 bytes in the area $d000-$d3ff,
	//i.e. register 0 appears on addresses $d000, $d040, $d080 etc.
	int v = 0;
	for(i=0; i<0x400; i++) {
		//printf("ioarea[%x] = &vicRegisters[%x]\n", i, v);
		ioarea[i] = &vicRegisters[v++];
		if(v>=0x40) v=0;
	}

	//The unused addresses $d02f-$d03f give $ff on reading, a write access is ignored
	// TODO: tämä kuuluisi vic osastolle
	for(v=0x2f; v<=0x3f; v++) vicRegisters[v] = 0xff;


	// TODO: there may be other things to init here
	// CIA 1 c00 - c07  (dc00 - dc07)
//	ioarea[0xc00] = &cia1_state.portA;
//	ioarea[0xc01] = &cia1_state.portB;
//	ioarea[0xc02] = &cia1_state.ddrA;
//	ioarea[0xc03] = &cia1_state.ddrB;
	// CIA 2 d00 - d07  (dd00 - dd07)
//	ioarea[0xd00] = &cia2_state.portA;
//	ioarea[0xd01] = &cia2_state.portB;
//	ioarea[0xd02] = &cia2_state.ddrA;
//	ioarea[0xd03] = &cia2_state.ddrB;

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

/*
	// CIA reads?
*/
	if(address>=0xc00 && address<=0xc0f) {
		//printf("CIA1 read address 0xdc0%x\n", address-0xc00);
		return cia6526_readByte(&cia1_state, address-0xc00);
	}
	if(address>=0xd00 && address<=0xd0f) {
		//printf("CIA2 read address 0xdd0%x\n", address-0xd00);
		return cia6526_readByte(&cia2_state, address-0xd00);
	}
	return(*ioarea[address]);
}

void ioAreaWriteByte(word address, byte data) {
	//checkAddress(address);

	// CIA Writes?
	if(address>=0xc00 && address<=0xc0f) {
		//printf2("CIA1 write address 0xdc0%x data %x\n", address-0xc00, data);
		cia6526_writeByte(&cia1_state, address-0xc00, data);
		return;
	}
	
	if(address>=0xd00 && address<=0xd0f) {
		//printf2("CIA2 write address 0xdd0%x data %x\n", address-0xd00, data);
		cia6526_writeByte(&cia2_state, address-0xd00, data);
		return;
	}

	*ioarea[address] = data;
}
