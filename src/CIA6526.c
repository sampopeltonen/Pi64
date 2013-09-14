
//#include <stdio.h>
#include "types.h"
#include "CIA6526.h"
//#include "MOS6510.h"


int decreaseTimer(byte hiByte, byte loByte, const byte hiByteLatch, const byte loByteLatch) {
	if(loByte!=0) loByte--;
	else {
		loByte=loByteLatch;
		if(hiByte!=0) hiByte--;
		else {
			hiByte=hiByteLatch;
			// A timer overflow! zero!
			//printf("overflow\n");
			return 1;
		}
	}
	return 0;
}

void doTimerCycle(cia_state* cia, int timerLoInd, int crInd) {
	// is timer running?
	if((cia->reg[crInd][0] & B1) == B1) {
		if(cia->reg[timerLoInd][0]!=0) cia->reg[timerLoInd][0]--;
		else {
			if(cia->reg[timerLoInd+1][0]!=0) {
				cia->reg[timerLoInd+1][0]--;
				cia->reg[timerLoInd][0]=0xff;
			}
			else {  // hi and lo are 0

				// TODO: generate interrupt only if corresponding CR says so (CR bit 1)
				if((cia->reg[crInd][0] & B1) == B1)
					//mos6510_HWInterrupt();

				// runmode is CRA bit 4 (1=one shot, 0=continuous)
				if((cia->reg[crInd][0] & B4) == B4) {
					// clear CRA running bit 1
					cia->reg[crInd][0] &= ~B1;
				}

				cia->reg[timerLoInd][0] = cia->reg[timerLoInd][1];		// take value from latch
				cia->reg[timerLoInd+1][0] = cia->reg[timerLoInd+1][1];	// take value from latch

			}

		}
	}
}



void cia6526_cycle(cia_state* cia) {

//	word ciaTimerA = cia->timerAValueLo + (cia->timerAValueHi << 8);
//	word ciaTimerB = cia->timerBValueLo + (cia->timerBValueHi << 8);
//	printf("cia timers A = %d   B = %d\n", ciaTimerA, ciaTimerB);


	doTimerCycle(cia, CIA_TIMERALO, CIA_CRA);
	doTimerCycle(cia, CIA_TIMERBLO, CIA_CRB);


	// TODO: update TOD



}


byte cia6526_readByte(cia_state *cia, word address) {

	//printf("Reading cia %d address 0x%x\n", cia->cia_number, address);

	if(address>0xf) {
		//printf("reading from invalid CIA %d address %x. Exiting\n", cia->cia_number, address);
		//exit(1);
	}
	// this is just some debug stuff to get it as far as "READY"
//	if(address>=0x0 && cia->cia_number==2) {
//		return(0xc0);
//	}
	return(cia->reg[address][0]);


//	return *(byte *)(cia + (address * sizeof(byte))); // kastataan pointteriksi byteen ja sit sen arvo...
}

/**
 * address should be 0 - F
 */
void cia6526_writeByte(cia_state* cia, word address, byte data) {
	if(address>0xf) {
		//printf("write to invalid CIA %d address %x. Exiting\n", cia->cia_number, address);
		//exit(1);
	}
	int regCol;
	// if writing to timer A or B addresses data should go to timer latches (second register column is for that.
	if(address>=CIA_TIMERALO && address<=CIA_TIMERBHI) {
		regCol = 1;
		//printf("suti\n");
		//byte *b = (void *)(cia + (address * sizeof(byte)) + 10 * sizeof(byte));
		//*b = data;
	}
	else {
		regCol = 0;
		//byte *b = (void *)(cia + (address * sizeof(byte)));
		//*b = data;
	}
	cia->reg[address][regCol] = data;
}



