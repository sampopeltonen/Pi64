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
ancb	OK
anda	OK
andax	OK
anday	OK
andb	OK
andix	OK
andiy	OK
andz	OK
andzx	OK
aneb	OK
arrb	OK
asla	OK
aslax	OK
asln	OK
aslz	OK
aslzx	OK
asoa	OK
asoax	OK
asoay	OK
asoix	OK
asoiy	OK
asoz	OK
asozx	OK
axsa	OK
axsix	OK
axsz	OK
axszy	OK
bccr
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
extern byte test_anda[];
extern byte test_andax[];
extern byte test_anday[];
extern byte test_andb[];
extern byte test_andix[];
extern byte test_andiy[];
extern byte test_andz[];
extern byte test_andzx[];
extern byte test_aneb[];
extern byte test_arrb[];
extern byte test_asla[];
extern byte test_aslax[];
extern byte test_asln[];
extern byte test_aslz[];
extern byte test_aslzx[];
extern byte test_asoa[];
extern byte test_asoax[];
extern byte test_asoay[];
extern byte test_asoix[];
extern byte test_asoiy[];
extern byte test_asoz[];
extern byte test_asozx[];
extern byte test_axsa[];
extern byte test_axsix[];
extern byte test_axsz[];
extern byte test_axszy[];
extern byte test_bccr[];

int initialized = 0;
int testNumber=35; //axszy

#define TESTCOUNT 36
void* tests[TESTCOUNT+5];


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
	int i = 0;
	tests[i++] = test_adca;
	tests[i++] = test_adcax;
	tests[i++] = test_adcay;
	tests[i++] = test_adcb;
	tests[i++] = test_adcix;
	tests[i++] = test_adciy;
	tests[i++] = test_adcz;
	tests[i++] = test_adczx;
	tests[i++] = test_alrb;
	tests[i++] = test_ancb;
	tests[i++] = test_anda;
	tests[i++] = test_andax;
	tests[i++] = test_anday;
	tests[i++] = test_andb;
	tests[i++] = test_andix;
	tests[i++] = test_andiy;
	tests[i++] = test_andz;
	tests[i++] = test_andzx;
	tests[i++] = test_aneb;
	tests[i++] = test_arrb;
	tests[i++] = test_asla;
	tests[i++] = test_aslax;
	tests[i++] = test_asln;
	tests[i++] = test_aslz;
	tests[i++] = test_aslzx;
	tests[i++] = test_asoa;
	tests[i++] = test_asoax;
	tests[i++] = test_asoay;
	tests[i++] = test_asoix;
	tests[i++] = test_asoiy;
	tests[i++] = test_asoz;
	tests[i++] = test_asozx;
	tests[i++] = test_axsa;
	tests[i++] = test_axsix;
	tests[i++] = test_axsz;
	tests[i++] = test_axszy;
	tests[i++] = test_bccr;
	
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
