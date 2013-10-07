#include "testsuite.h"

#include "types.h"
#include "ram.h"
#include "stdlibtools.h"
#include "MOS6510.h"

/*
adca	OK
adcax	OK
adcay	OK
adcb	OK
adcix	OK
adciy	OK
adcz	OK
adczx	OK
alrb	OK
ancb
*/

extern byte test_adca[];
extern byte test_adcax[];
extern byte test_adcay[];
extern byte test_adcb[];
extern byte test_adcix[];
extern byte test_adciy[];
extern byte test_adcz[];
extern byte test_adczx[];
extern byte test_alrb[];
extern byte test_ancb[];

int initialized = 0;
int testNumber=8;

#define TESTCOUNT 10
void* tests[TESTCOUNT];


void loadTest(int number) {
	byte* test1 = tests[number];
	byte* test2 = tests[number+1];
	
	word memaddr = test1[0] + (test1[1]<<8); // first 2 bytes are load address
	printf1("Test #%d",number);
	word progaddr;	
	unsigned int programlength = test2-test1;
	memWriteByte(memaddr++, 0x10);
	memWriteByte(memaddr++, 0x8);
	for(progaddr=4; progaddr<programlength; progaddr++) {
		memWriteByte(memaddr, test1[progaddr]);
		memaddr++;
	}
}

void init() {
	tests[0] = test_adca;
	tests[1] = test_adcax;
	tests[2] = test_adcay;
	tests[3] = test_adcb;
	tests[4] = test_adcix;
	tests[5] = test_adciy;
	tests[6] = test_adcz;
	tests[7] = test_adczx;
	tests[8] = test_alrb;
	tests[9] = test_ancb;
	initialized=1;
}

void testsuite_loadNext() {
	if(initialized==0) init();
	if(testNumber<TESTCOUNT)
		loadTest(testNumber++);
	//memWriteByte(0x200, 18);
	//memWriteByte(0x201, 82);
}

void testsuite_printStatus() {
	printProcessorStatus();	
}
