#include "testsuite.h"

#include "types.h"
#include "ram.h"
#include "stdlibtools.h"
#include "MOS6510.h"

/*
adca
adcax
adcay
adcb

*/

extern byte test_adca[];
extern byte test_adcax[];
extern byte test_adcay[];
extern byte test_adcb[];


void testsuite_loadNext() {
	word memaddr = test_adca[0] + (test_adca[1]<<8); // first 2 bytes are load address
	printf1("Ld %x",memaddr);
	word progaddr;	
	unsigned int programlength = test_adcax-test_adca;
	memWriteByte(memaddr++, 0x10);
	memWriteByte(memaddr++, 0x8);
	for(progaddr=4; progaddr<programlength; progaddr++) {
		memWriteByte(memaddr, test_adca[progaddr]);
		memaddr++;
	}

}

void testsuite_printStatus() {
	printProcessorStatus();	
}
