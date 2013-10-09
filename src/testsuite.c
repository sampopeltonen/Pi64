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
bccr	OK
bcsr	OK
beqr	OK
bita	OK
bitz	OK
bmir	OK
bner	OK
bplr	OK
branchwrap	OK
brkn	OK
bvcr	OK
bvsr	OK
clcn	OK
cldn	OK
clin	OK
clvn	OK
cmpa	OK
cmpax	OK
cmpay	OK
cmpb	OK
cmpix	OK
cmpiy	OK
cmpz	OK
cmpzx	OK
cpuport		fail 60
cputiming
cpxa	OK
cpxb	OK
cpxz	OK
cpya	OK
cpyb	OK
cpyz	OK
dcma	OK
dcmax	OK
dcmay	OK
dcmix	OK
dcmiy	OK
dcmz	OK
dcmzx	OK
deca	OK
decax	OK
decz	OK
deczx	OK
dexn	OK
deyn	OK
eora	OK
eorax	OK
eoray	OK
eorb	OK
eorix	OK
eoriy	OK
eorz	OK
eorzx	OK
inca	OK
incax	OK
incz	OK
inczx	OK
insa	fail ins uses SBC logic, test SBC first 93
insax
insay
insix
insiy
insz
inszx
inxn	OK
inyn	OK
jmpi	OK
jmpw	OK
jsrw	OK
lasay	OK
laxa	OK
laxay	OK
laxix	OK
laxiy	OK
laxz	OK
laxzy	OK
ldaa	OK
ldaax	OK
ldaay	OK
ldab	OK
ldaix	OK
ldaiy	OK
ldaz	OK
ldazx	OK
ldxa	OK
ldxay	OK
ldxb	OK
ldxz	OK
ldxzy	OK
ldya	OK
ldyax	OK
ldyb	OK
ldyz	OK
ldyzx	OK
lsea	OK
lseax	OK
lseay	OK
lseix	OK
lseiy	OK
lsez	OK
lsezx	OK
lsra	OK
lsrax	OK
lsrn	OK
lsrz	OK
lsrzx	OK
lxab	OK
mmu	fail 143
mmufetch fail 
nopa	OK
nopax	OK
nopb	OK
nopn	OK
nopz	OK
nopzx	OK
oraa	OK
oraax	OK
oraay	OK
orab	OK
oraix	OK
oraiy	OK
oraz	OK
orazx	OK
phan	OK
phpn	OK
plan	OK
plpn	OK
rlaa	OK
rlaax	OK
rlaay	OK
rlaix	OK
rlaiy	OK
rlaz	OK
rlazx	OK
rola	OK
rolax	OK
roln	OK
rolz	OK
rolzx	OK
rora	OK
rorax	OK
rorn	OK
rorz	OK
rorzx	OK
rraa	OK
rraax	OK
rraay	OK
rraix	OK
rraiy	OK
rraz	OK
rrazx	OK
rtin	OK
rtsn	OK
sbca	fail 189
sbcax
sbcay
sbcb
sbcb(eb)
sbcix
sbciy
sbcz
sbczx
sbxb
secn
sedn
sein
shaay
shaiy
shsay
shxay
shyax
staa
staax
staay
staix
staiy
staz
stazx
stxa
stxz
stxzy
stya
styz
styzx
taxn
tayn
trap1
trap10
trap11
trap12
trap13
trap14
trap15
trap16
trap17
trap2
trap3
trap4
trap5
trap6
trap7
trap8
trap9
tsxn
txan
txsn
tyan
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
extern byte test_bcsr[];
extern byte test_beqr[];
extern byte test_bita[];
extern byte test_bitz[];
extern byte test_bmir[];
extern byte test_bner[];
extern byte test_bplr[];
extern byte test_branchwrap[];
extern byte test_brkn[];
extern byte test_bvcr[];
extern byte test_bvsr[];
extern byte test_clcn[];
extern byte test_cldn[];
extern byte test_clin[];
extern byte test_clvn[];
extern byte test_cmpa[];
extern byte test_cmpax[];
extern byte test_cmpay[];
extern byte test_cmpb[];
extern byte test_cmpix[];
extern byte test_cmpiy[];
extern byte test_cmpz[];
extern byte test_cmpzx[];
extern byte test_cpuport[];
extern byte test_cputiming[];
extern byte test_cpxa[];
extern byte test_cpxb[];
extern byte test_cpxz[];
extern byte test_cpya[];
extern byte test_cpyb[];
extern byte test_cpyz[];
extern byte test_dcma[];
extern byte test_dcmax[];
extern byte test_dcmay[];
extern byte test_dcmix[];
extern byte test_dcmiy[];
extern byte test_dcmz[];
extern byte test_dcmzx[];
extern byte test_deca[];
extern byte test_decax[];
extern byte test_decz[];
extern byte test_deczx[];
extern byte test_dexn[];
extern byte test_deyn[];
extern byte test_eora[];
extern byte test_eorax[];
extern byte test_eoray[];
extern byte test_eorb[];
extern byte test_eorix[];
extern byte test_eoriy[];
extern byte test_eorz[];
extern byte test_eorzx[];
extern byte test_inca[];
extern byte test_incax[];
extern byte test_incz[];
extern byte test_inczx[];
extern byte test_insa[];
extern byte test_insax[];
extern byte test_insay[];
extern byte test_insix[];
extern byte test_insiy[];
extern byte test_insz[];
extern byte test_inszx[];
extern byte test_inxn[];
extern byte test_inyn[];
extern byte test_jmpi[];
extern byte test_jmpw[];
extern byte test_jsrw[];
extern byte test_lasay[];
extern byte test_laxa[];
extern byte test_laxay[];
extern byte test_laxix[];
extern byte test_laxiy[];
extern byte test_laxz[];
extern byte test_laxzy[];
extern byte test_ldaa[];
extern byte test_ldaax[];
extern byte test_ldaay[];
extern byte test_ldab[];
extern byte test_ldaix[];
extern byte test_ldaiy[];
extern byte test_ldaz[];
extern byte test_ldazx[];
extern byte test_ldxa[];
extern byte test_ldxay[];
extern byte test_ldxb[];
extern byte test_ldxz[];
extern byte test_ldxzy[];
extern byte test_ldya[];
extern byte test_ldyax[];
extern byte test_ldyb[];
extern byte test_ldyz[];
extern byte test_ldyzx[];
extern byte test_lsea[];
extern byte test_lseax[];
extern byte test_lseay[];
extern byte test_lseix[];
extern byte test_lseiy[];
extern byte test_lsez[];
extern byte test_lsezx[];
extern byte test_lsra[];
extern byte test_lsrax[];
extern byte test_lsrn[];
extern byte test_lsrz[];
extern byte test_lsrzx[];
extern byte test_lxab[];
extern byte test_mmu[];
extern byte test_mmufetch[];
extern byte test_nopa[];
extern byte test_nopax[];
extern byte test_nopb[];
extern byte test_nopn[];
extern byte test_nopz[];
extern byte test_nopzx[];
extern byte test_oraa[];
extern byte test_oraax[];
extern byte test_oraay[];
extern byte test_orab[];
extern byte test_oraix[];
extern byte test_oraiy[];
extern byte test_oraz[];
extern byte test_orazx[];
extern byte test_phan[];
extern byte test_phpn[];
extern byte test_plan[];
extern byte test_plpn[];
extern byte test_rlaa[];
extern byte test_rlaax[];
extern byte test_rlaay[];
extern byte test_rlaix[];
extern byte test_rlaiy[];
extern byte test_rlaz[];
extern byte test_rlazx[];
extern byte test_rola[];
extern byte test_rolax[];
extern byte test_roln[];
extern byte test_rolz[];
extern byte test_rolzx[];
extern byte test_rora[];
extern byte test_rorax[];
extern byte test_rorn[];
extern byte test_rorz[];
extern byte test_rorzx[];
extern byte test_rraa[];
extern byte test_rraax[];
extern byte test_rraay[];
extern byte test_rraix[];
extern byte test_rraiy[];
extern byte test_rraz[];
extern byte test_rrazx[];
extern byte test_rtin[];
extern byte test_rtsn[];
extern byte test_sbca[];
extern byte test_sbcax[];
extern byte test_sbcay[];
extern byte test_sbcb[];
extern byte test_sbcbeb[];
extern byte test_sbcix[];
extern byte test_sbciy[];
extern byte test_sbcz[];
extern byte test_sbczx[];
extern byte test_sbxb[];
extern byte test_secn[];
extern byte test_sedn[];
extern byte test_sein[];
extern byte test_shaay[];
extern byte test_shaiy[];
extern byte test_shsay[];
extern byte test_shxay[];
extern byte test_shyax[];
extern byte test_staa[];
extern byte test_staax[];
extern byte test_staay[];
extern byte test_staix[];
extern byte test_staiy[];
extern byte test_staz[];
extern byte test_stazx[];
extern byte test_stxa[];
extern byte test_stxz[];
extern byte test_stxzy[];
extern byte test_stya[];
extern byte test_styz[];
extern byte test_styzx[];
extern byte test_taxn[];
extern byte test_tayn[];
extern byte test_trap1[];
extern byte test_trap10[];
extern byte test_trap11[];
extern byte test_trap12[];
extern byte test_trap13[];
extern byte test_trap14[];
extern byte test_trap15[];
extern byte test_trap16[];
extern byte test_trap17[];
extern byte test_trap2[];
extern byte test_trap3[];
extern byte test_trap4[];
extern byte test_trap5[];
extern byte test_trap6[];
extern byte test_trap7[];
extern byte test_trap8[];
extern byte test_trap9[];
extern byte test_tsxn[];
extern byte test_txan[];
extern byte test_txsn[];
extern byte test_tyan[];

int initialized = 0;
int testNumber=189;

#define TESTCOUNT 244
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
		//printf2("%x:%x",memaddr,test1[progaddr]);
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
	tests[i++] = test_bcsr;
	tests[i++] = test_beqr;
	tests[i++] = test_bita;
	tests[i++] = test_bitz;
	tests[i++] = test_bmir;
	tests[i++] = test_bner;
	tests[i++] = test_bplr;
	tests[i++] = test_branchwrap;
	tests[i++] = test_brkn;
	tests[i++] = test_bvcr;
	tests[i++] = test_bvsr;
	tests[i++] = test_clcn;
	tests[i++] = test_cldn;
	tests[i++] = test_clin;
	tests[i++] = test_clvn;
	tests[i++] = test_cmpa;
	tests[i++] = test_cmpax;
	tests[i++] = test_cmpay;
	tests[i++] = test_cmpb;
	tests[i++] = test_cmpix;
	tests[i++] = test_cmpiy;
	tests[i++] = test_cmpz;
	tests[i++] = test_cmpzx;
	tests[i++] = test_cpuport;
	tests[i++] = test_cputiming;
	tests[i++] = test_cpxa;
	tests[i++] = test_cpxb;
	tests[i++] = test_cpxz;
	tests[i++] = test_cpya;
	tests[i++] = test_cpyb;
	tests[i++] = test_cpyz;
	tests[i++] = test_dcma;
	tests[i++] = test_dcmax;
	tests[i++] = test_dcmay;
	tests[i++] = test_dcmix;
	tests[i++] = test_dcmiy;
	tests[i++] = test_dcmz;
	tests[i++] = test_dcmzx;
	tests[i++] = test_deca;
	tests[i++] = test_decax;
	tests[i++] = test_decz;
	tests[i++] = test_deczx;
	tests[i++] = test_dexn;
	tests[i++] = test_deyn;
	tests[i++] = test_eora;
	tests[i++] = test_eorax;
	tests[i++] = test_eoray;
	tests[i++] = test_eorb;
	tests[i++] = test_eorix;
	tests[i++] = test_eoriy;
	tests[i++] = test_eorz;
	tests[i++] = test_eorzx;
	tests[i++] = test_inca;
	tests[i++] = test_incax;
	tests[i++] = test_incz;
	tests[i++] = test_inczx;
	tests[i++] = test_insa;
	tests[i++] = test_insax;
	tests[i++] = test_insay;
	tests[i++] = test_insix;
	tests[i++] = test_insiy;
	tests[i++] = test_insz;
	tests[i++] = test_inszx;
	tests[i++] = test_inxn;
	tests[i++] = test_inyn;
	tests[i++] = test_jmpi;
	tests[i++] = test_jmpw;
	tests[i++] = test_jsrw;
	tests[i++] = test_lasay;
	tests[i++] = test_laxa;
	tests[i++] = test_laxay;
	tests[i++] = test_laxix;
	tests[i++] = test_laxiy;
	tests[i++] = test_laxz;
	tests[i++] = test_laxzy;
	tests[i++] = test_ldaa;
	tests[i++] = test_ldaax;
	tests[i++] = test_ldaay;
	tests[i++] = test_ldab;
	tests[i++] = test_ldaix;
	tests[i++] = test_ldaiy;
	tests[i++] = test_ldaz;
	tests[i++] = test_ldazx;
	tests[i++] = test_ldxa;
	tests[i++] = test_ldxay;
	tests[i++] = test_ldxb;
	tests[i++] = test_ldxz;
	tests[i++] = test_ldxzy;
	tests[i++] = test_ldya;
	tests[i++] = test_ldyax;
	tests[i++] = test_ldyb;
	tests[i++] = test_ldyz;
	tests[i++] = test_ldyzx;
	tests[i++] = test_lsea;
	tests[i++] = test_lseax;
	tests[i++] = test_lseay;
	tests[i++] = test_lseix;
	tests[i++] = test_lseiy;
	tests[i++] = test_lsez;
	tests[i++] = test_lsezx;
	tests[i++] = test_lsra;
	tests[i++] = test_lsrax;
	tests[i++] = test_lsrn;
	tests[i++] = test_lsrz;
	tests[i++] = test_lsrzx;
	tests[i++] = test_lxab;
	tests[i++] = test_mmu;
	tests[i++] = test_mmufetch;
	tests[i++] = test_nopa;
	tests[i++] = test_nopax;
	tests[i++] = test_nopb;
	tests[i++] = test_nopn;
	tests[i++] = test_nopz;
	tests[i++] = test_nopzx;
	tests[i++] = test_oraa;
	tests[i++] = test_oraax;
	tests[i++] = test_oraay;
	tests[i++] = test_orab;
	tests[i++] = test_oraix;
	tests[i++] = test_oraiy;
	tests[i++] = test_oraz;
	tests[i++] = test_orazx;
	tests[i++] = test_phan;
	tests[i++] = test_phpn;
	tests[i++] = test_plan;
	tests[i++] = test_plpn;
	tests[i++] = test_rlaa;
	tests[i++] = test_rlaax;
	tests[i++] = test_rlaay;
	tests[i++] = test_rlaix;
	tests[i++] = test_rlaiy;
	tests[i++] = test_rlaz;
	tests[i++] = test_rlazx;
	tests[i++] = test_rola;
	tests[i++] = test_rolax;
	tests[i++] = test_roln;
	tests[i++] = test_rolz;
	tests[i++] = test_rolzx;
	tests[i++] = test_rora;
	tests[i++] = test_rorax;
	tests[i++] = test_rorn;
	tests[i++] = test_rorz;
	tests[i++] = test_rorzx;
	tests[i++] = test_rraa;
	tests[i++] = test_rraax;
	tests[i++] = test_rraay;
	tests[i++] = test_rraix;
	tests[i++] = test_rraiy;
	tests[i++] = test_rraz;
	tests[i++] = test_rrazx;
	tests[i++] = test_rtin;
	tests[i++] = test_rtsn;
	tests[i++] = test_sbca;
	tests[i++] = test_sbcax;
	tests[i++] = test_sbcay;
	tests[i++] = test_sbcb;
	tests[i++] = test_sbcbeb;
	tests[i++] = test_sbcix;
	tests[i++] = test_sbciy;
	tests[i++] = test_sbcz;
	tests[i++] = test_sbczx;
	tests[i++] = test_sbxb;
	tests[i++] = test_secn;
	tests[i++] = test_sedn;
	tests[i++] = test_sein;
	tests[i++] = test_shaay;
	tests[i++] = test_shaiy;
	tests[i++] = test_shsay;
	tests[i++] = test_shxay;
	tests[i++] = test_shyax;
	tests[i++] = test_staa;
	tests[i++] = test_staax;
	tests[i++] = test_staay;
	tests[i++] = test_staix;
	tests[i++] = test_staiy;
	tests[i++] = test_staz;
	tests[i++] = test_stazx;
	tests[i++] = test_stxa;
	tests[i++] = test_stxz;
	tests[i++] = test_stxzy;
	tests[i++] = test_stya;
	tests[i++] = test_styz;
	tests[i++] = test_styzx;
	tests[i++] = test_taxn;
	tests[i++] = test_tayn;
	tests[i++] = test_trap1;
	tests[i++] = test_trap10;
	tests[i++] = test_trap11;
	tests[i++] = test_trap12;
	tests[i++] = test_trap13;
	tests[i++] = test_trap14;
	tests[i++] = test_trap15;
	tests[i++] = test_trap16;
	tests[i++] = test_trap17;
	tests[i++] = test_trap2;
	tests[i++] = test_trap3;
	tests[i++] = test_trap4;
	tests[i++] = test_trap5;
	tests[i++] = test_trap6;
	tests[i++] = test_trap7;
	tests[i++] = test_trap8;
	tests[i++] = test_trap9;
	tests[i++] = test_tsxn;
	tests[i++] = test_txan;
	tests[i++] = test_txsn;
	tests[i++] = test_tyan;
	
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
