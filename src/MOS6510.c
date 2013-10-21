/*
 ============================================================================
 Name        : MOS6510.c
 Author      : Sampo Peltonen
 Licence     : GNU General Public License, version 2.
 Copyright   : Copyright (C) 2013  Sampo Peltonen
 Description : MOS6510 simulation
 ============================================================================
 */

#include "types.h"
#include "MOS6510.h"
#include "stdlibtools.h"
//#include "usbkeyboard.h"
#include "ram.h"

#define DEBUG_6510 0

//http://www.cpu-world.com/Arch/650x.html
#define ILLEGAL 0
#define ACCUMULATOR 1
#define ABSOLUTE 2
#define ABSOLUTE_X 3
#define ABSOLUTE_Y 4
#define IMMEDIATE 5
#define IMPLIED 6
#define INDIRECT 7
#define INDIRECT_X 8
#define INDIRECT_Y 9
#define RELATIVE 10
#define ZEROPAGE 11
#define ZEROPAGE_X 12
#define ZEROPAGE_Y 13

#define PFLAG_CARRY 1
#define PFLAG_ZERO 2
#define PFLAG_IRQ 4
#define PFLAG_DECIMAL 8
#define PFLAG_BREAK 16
#define PFLAG_UNUSED 32
#define PFLAG_OVERFLOW 64
#define PFLAG_NEGATIVE 128


// Internal Registers

byte A;
byte X;
byte Y;
byte P;
word PC;
byte S;

int cyc = 0;

int irqLineUp;
int nmiLineUp;
int interruptDelayed=0;

int yesDump;

typedef struct {
	void (*pt2MnemonicHandler)(int, int);
	int am;
	int cycles;
} mneAM_s;

mneAM_s mneAM[0x100];


inline byte readMemoryPC() {
	return memReadByte(PC++);
}

inline void setPFlag(byte flag) {
	P |= flag;
}
inline void clearPFlag(byte flag) {
	P &= ~flag;
}
inline void togglePFlag(byte flag) {
	P ^= flag;
}

inline void stackPush(byte value) {
	memWriteByte(0x100 + S--, value);
}
inline byte stackPull() {
	return(memReadByte(0x100 + ++S));
}


void setZeroAndNegativePFlags(byte *target) {
	if(*target==0) setPFlag(PFLAG_ZERO);
	else clearPFlag(PFLAG_ZERO);
	if(B8 == (B8 & *target)) setPFlag(PFLAG_NEGATIVE);
	else clearPFlag(PFLAG_NEGATIVE);
}


/* returns 1 if page crossed, 0 otherwise*/
int resolveAddressModeTarget(int am, word *targetAddress) {
	byte oper1;
	byte oper2;
	word tmpw;
	byte tmpb;
	int pageCrossed = 0;
	switch (am) {
		case ACCUMULATOR:
			*targetAddress = A;
			break;
		case IMMEDIATE:
			*targetAddress = PC++;
			break;
		case ZEROPAGE:
			oper1 = readMemoryPC();
			*targetAddress = oper1;
			break;
		case ZEROPAGE_X:
			oper1 = readMemoryPC();
			*targetAddress = (oper1+X) & 0xff;
			break;
		case ZEROPAGE_Y:
			oper1 = readMemoryPC();
                        *targetAddress = (oper1+Y) & 0xff;
			break;
		case ABSOLUTE:
			oper1 = readMemoryPC();
			oper2 = readMemoryPC();
			*targetAddress = (oper2 << 8) + oper1;
			break;
		case ABSOLUTE_X:
			oper1 = readMemoryPC();
			oper2 = readMemoryPC();
			tmpw = oper1 + X;
			if(tmpw>0xff) pageCrossed = 1; //if page crossed
			*targetAddress = (oper2 << 8) + tmpw;
			break;
		case ABSOLUTE_Y:
			oper1 = readMemoryPC();
			oper2 = readMemoryPC();
			tmpw = oper1 + Y;
			if(tmpw>0xff) pageCrossed = 1; //if page crossed
			*targetAddress = (oper2 << 8) + tmpw;
			break;
		case INDIRECT:
			oper1 = readMemoryPC();
			oper2 = readMemoryPC();
			*targetAddress = memReadWordWithBoundaryCross(oper1, oper2);
			break;
		case INDIRECT_X:
			oper1 = readMemoryPC();
			tmpb = X + oper1;
			*targetAddress = memReadWordWithBoundaryCross(tmpb&0xff, tmpb>>8);
			break;
		case INDIRECT_Y:
			oper1 = readMemoryPC();
			tmpw = memReadByte(oper1) + Y;
			if(tmpw>0xff) pageCrossed = 1; //if page crossed
			tmpw += memReadByte((byte)(oper1+1)) << 8;
			*targetAddress = tmpw;
			break;
		default:
			printf1("AM %d not implemented.", am);
			printf1("PC=%x",PC);
			exit(1);
	}
	return(pageCrossed);
}






void mneILL(int am, int cycles) {
	printf1("Illop:%x",memReadByte(PC-1));
	printf1("PC:%x",PC-1);
	exit(1);
}

void mneADC(int am, int cycles) {
	word target;

	cyc = cycles + resolveAddressModeTarget(am, &target);
	byte targetValue = memReadByte(target);
	word tmp = targetValue + ((P & PFLAG_CARRY) ? 1 : 0);
	word t = A + tmp;
	// V flag according to http://www.righto.com/2012/12/the-6502-overflow-flag-explained.html
	((A^t)&(tmp^t)&0x80) ? setPFlag(PFLAG_OVERFLOW) : clearPFlag(PFLAG_OVERFLOW);
	if((t&0xff)==0) setPFlag(PFLAG_ZERO);
	else clearPFlag(PFLAG_ZERO);

	if(P & PFLAG_DECIMAL) {
		// Decimal mode according to http://www.6502.org/tutorials/decimal_mode.html
		word tmpA = A;
		word AL = (tmpA & 0xf) + (targetValue & 0xf) + ((P & PFLAG_CARRY) ? 1 : 0);
		if(AL>=0xa) AL = ((AL + 0x6) & 0xf) + 0x10;
		tmpA = (tmpA & 0xf0) + (targetValue & 0xf0) + AL;
		if(B8 & tmpA) setPFlag(PFLAG_NEGATIVE);
		else clearPFlag(PFLAG_NEGATIVE);
		if(tmpA >= 0xa0) tmpA = tmpA + 0x60;
		A = tmpA & 0xff;
		if(tmpA>=0x100) setPFlag(PFLAG_CARRY);
		else clearPFlag(PFLAG_CARRY);
	}
	else {
		A = t & 0xff;
		if(B8 & A) setPFlag(PFLAG_NEGATIVE);
		else clearPFlag(PFLAG_NEGATIVE);
		if(t>0xff) setPFlag(PFLAG_CARRY);
		else clearPFlag(PFLAG_CARRY);
	}
}

void mneAND(int am, int cycles) {
	word target;
	cyc = cycles + resolveAddressModeTarget(am, &target);
	A = A & memReadByte(target);
	setZeroAndNegativePFlags(&A);
}

void mneASL(int am, int cycles) {
	cyc = cycles;
	word target;
	if(am == ACCUMULATOR) {
		((A & B8)==B8) ? setPFlag(PFLAG_CARRY) : clearPFlag(PFLAG_CARRY);
		A = (A << 1) & 0xfe;
		setZeroAndNegativePFlags(&A);
	}
	else {
		resolveAddressModeTarget(am, &target);
		byte oldValue = memReadByte(target);
		((oldValue & B8)==B8) ? setPFlag(PFLAG_CARRY) : clearPFlag(PFLAG_CARRY);
		byte newValue = (oldValue << 1) & 0xfe;
		memWriteByte(target, newValue);
		setZeroAndNegativePFlags(&newValue);
	}
}

void mneBCC(int am, int cycles) {
	cyc = cycles;
	sbyte offset = readMemoryPC();
	if((PFLAG_CARRY & P)==0) {
		cyc++;
		word newLoc = PC + offset;
		if((newLoc >> 8) != (PC >> 8)) cyc++; // is page crossed?
		PC = newLoc;
	}
}

void mneBCS(int am, int cycles) {
	cyc = cycles;
	sbyte offset = (sbyte) readMemoryPC();
	if((P & PFLAG_CARRY)== PFLAG_CARRY) {
		cyc++;
		word newLoc = PC + offset;
		if((newLoc >> 8) != (PC >> 8)) cyc++; // is page crossed?
		PC = newLoc;
	}
}

void mneBEQ(int am, int cycles) {
	cyc = cycles;
	sbyte offset = (sbyte) readMemoryPC();
	if((P & PFLAG_ZERO)== PFLAG_ZERO) {
		cyc++;
		word newLoc = PC + offset;
		if((newLoc >> 8) != (PC >> 8)) cyc++; // is page crossed?
		PC = newLoc;
	}
}

void mneBIT(int am, int cycles) {
	cyc = cycles;
	word target;
	resolveAddressModeTarget(am, &target);
	byte t = memReadByte(target);
	((t & B7)==B7) ? setPFlag(PFLAG_OVERFLOW) : clearPFlag(PFLAG_OVERFLOW);
	((t & B8)==B8) ? setPFlag(PFLAG_NEGATIVE) : clearPFlag(PFLAG_NEGATIVE);
	((A&t)==0) ? setPFlag(PFLAG_ZERO) : clearPFlag(PFLAG_ZERO);
	//setZeroAndNegativePFlags(&t);  this is not true
}

void mneBMI(int am, int cycles) {
	cyc = cycles;
	sbyte offset = (sbyte) readMemoryPC();
	if((P & PFLAG_NEGATIVE)== PFLAG_NEGATIVE) {
		cyc++;
		word newLoc = PC + offset;
		if((newLoc >> 8) != (PC >> 8)) cyc++; // is page crossed?
		PC = newLoc;
	}
}

void mneBNE(int am, int cycles) {
	cyc = cycles;
	sbyte offset = readMemoryPC();
	if((PFLAG_ZERO & P)==0) {
		cyc++;
		word newLoc = PC + offset;
		if((newLoc >> 8) != (PC >> 8)) cyc++; // is page crossed?
		PC = newLoc;
	}
}

void mneBPL(int am, int cycles) {
	cyc = cycles;
	sbyte offset = readMemoryPC();
	if((PFLAG_NEGATIVE & P)==0) {
		cyc++;
		word newLoc = PC + offset;
		if((newLoc >> 8) != (PC >> 8)) cyc++; // is page crossed?
		PC = newLoc;
	}
}

void mneBRK(int am, int cycles) {
	cyc = cycles;
	PC+=1;
	stackPush(PC >> 8);
	stackPush(PC);
	stackPush(P|PFLAG_BREAK);
	P |= PFLAG_IRQ;
	PC = memReadWord(0xfffe);
}

void mneBVC(int am, int cycles) {
        cyc = cycles;
        sbyte offset = (sbyte) readMemoryPC();
        if((P & PFLAG_OVERFLOW)==0) {
                cyc++;
                word newLoc = PC + offset;
                if((newLoc >> 8) != (PC >> 8)) cyc++; // is page crossed?
                PC = newLoc;
        }
}

void mneBVS(int am, int cycles) {
	cyc = cycles;
	sbyte offset = (sbyte) readMemoryPC();
	if((P & PFLAG_OVERFLOW)== PFLAG_OVERFLOW) {
		cyc++;
		word newLoc = PC + offset;
		if((newLoc >> 8) != (PC >> 8)) cyc++; // is page crossed?
		PC = newLoc;
	}
}

void mneCLC(int am, int cycles) {
	cyc = cycles;
	clearPFlag(PFLAG_CARRY);
}

void mneCLD(int am, int cycles) {
	cyc = cycles;
	clearPFlag(PFLAG_DECIMAL);
}

void mneCLI(int am, int cycles) {
	cyc = cycles;
	clearPFlag(PFLAG_IRQ);
}

void mneCLV(int am, int cycles) {
	cyc = cycles;
	clearPFlag(PFLAG_OVERFLOW);
}

void mneCMP(int am, int cycles) {
	word target;
	cyc = cycles + resolveAddressModeTarget(am, &target);
	byte value = memReadByte(target);
	byte tmpb = A - value;
	setZeroAndNegativePFlags(&tmpb);
	if(A>=value) setPFlag(PFLAG_CARRY);
	else clearPFlag(PFLAG_CARRY);
}

void mneCPX(int am, int cycles) {
	cyc = cycles;
	word target;
	resolveAddressModeTarget(am, &target);
	byte value = memReadByte(target);
	byte tmp = X - value;
	setZeroAndNegativePFlags(&tmp);
        if(X>=value) setPFlag(PFLAG_CARRY);
        else clearPFlag(PFLAG_CARRY);
}

void mneCPY(int am, int cycles) {
	cyc = cycles;
	word target;
	resolveAddressModeTarget(am, &target);
	byte value = memReadByte(target);
        byte tmp = Y - value;
        setZeroAndNegativePFlags(&tmp);
        if(Y>=value) setPFlag(PFLAG_CARRY);
        else clearPFlag(PFLAG_CARRY);
}

void mneDEC(int am, int cycles) {
	word target;
	cyc = cycles;
	resolveAddressModeTarget(am, &target);
	byte tmp = memReadByte(target);
	tmp--;
	setZeroAndNegativePFlags(&tmp);
	memWriteByte(target, tmp);
}

void mneDEX(int am, int cycles) {
	cyc = cycles;
	X--;
	setZeroAndNegativePFlags(&X);
}

void mneDEY(int am, int cycles) {
	cyc = cycles;
	Y--;
	setZeroAndNegativePFlags(&Y);
}

void mneEOR(int am, int cycles) {
	word target;
	cyc = cycles + resolveAddressModeTarget(am, &target);
	A = A ^ memReadByte(target);
	setZeroAndNegativePFlags(&A);
}

void mneINC(int am, int cycles) {
	cyc = cycles;
	word target;
	resolveAddressModeTarget(am, &target);
	byte tmpb = memReadByte(target)+1;
	memWriteByte(target, tmpb);
	setZeroAndNegativePFlags(&tmpb);
}

void mneINX(int am, int cycles) {
	cyc = cycles;
	X++;
	setZeroAndNegativePFlags(&X);
}

void mneINY(int am, int cycles) {
	cyc = cycles;
	Y++;
	setZeroAndNegativePFlags(&Y);
}

void mneJMP(int am, int cycles) {
	// ABSOLUTE or INDIRECT
	cyc = cycles;
	byte oper1 = readMemoryPC();
        byte oper2 = readMemoryPC();
	if(am==ABSOLUTE) {
                PC = (oper2 << 8) + oper1;
	}
	else {
		word address = (oper2 << 8) + oper1;
                if(oper1==0xff) {
			//JMP indirect xxFF bug is implemented here
			PC = memReadByte(address) + (memReadByte(address-0x100+1) << 8);
		}
		else {
			PC =  memReadWord(address);
		}
	}
}

void mneJSR(int am, int cycles) {
	// there's only ABSOLUTE adressing mode
	cyc = cycles;
	word tmpW;
	tmpW = readMemoryPC() + (readMemoryPC() << 8);
	stackPush((PC-1) >> 8);
	stackPush(PC-1);
	PC = tmpW;
}

// http://www.obelisk.demon.co.uk/6502/reference.html#LDA
void mneLDA(int am, int cycles) {
	word target;
	cyc = cycles + resolveAddressModeTarget(am, &target);
	A = memReadByte(target);
	setZeroAndNegativePFlags(&A);
}

void mneLDX(int am, int cycles) {
	word target;
	cyc = cycles + resolveAddressModeTarget(am, &target);
	X = memReadByte(target);
	setZeroAndNegativePFlags(&X);
}

void mneLDY(int am, int cycles) {
	word target;
	cyc = cycles + resolveAddressModeTarget(am, &target);
	Y = memReadByte(target);
	setZeroAndNegativePFlags(&Y);
}

void mneLSR(int am, int cycles) {
	cyc = cycles;
	word target;
	clearPFlag(PFLAG_NEGATIVE);
        if(am==ACCUMULATOR) {
		(A & B1) ? setPFlag(PFLAG_CARRY) : clearPFlag(PFLAG_CARRY);
                A = (A >> 1) & 0x7f;
		(A==0) ? setPFlag(PFLAG_ZERO) : clearPFlag(PFLAG_ZERO);
        }
        else {
		resolveAddressModeTarget(am, &target);
		byte tmp = memReadByte(target);
		(tmp & B1) ? setPFlag(PFLAG_CARRY) : clearPFlag(PFLAG_CARRY);
		tmp = (tmp >> 1) & 0x7f;
		(tmp==0) ? setPFlag(PFLAG_ZERO) : clearPFlag(PFLAG_ZERO);
		memWriteByte(target, tmp);
	}
}

void mneNOP(int am, int cycles) {
  	cyc = cycles;
}

void mneORA(int am, int cycles) {
	word target;
	cyc = cycles + resolveAddressModeTarget(am, &target);
	A = A | memReadByte(target);
	setZeroAndNegativePFlags(&A);
}

void mnePHA(int am, int cycles) {
	cyc = cycles;
	stackPush(A);
}

void mnePHP(int am, int cycles) {
	cyc = cycles;
	//TODO: check if this setting B flag is correct?
	stackPush(P|PFLAG_BREAK|PFLAG_UNUSED);
}

void mnePLA(int am, int cycles) {
	cyc = cycles;
	A = stackPull();
	setZeroAndNegativePFlags(&A);
}

void mnePLP(int am, int cycles) {
	cyc = cycles;
	P = (stackPull()|PFLAG_UNUSED); // the unused flag is supposed to be 1
}

void mneROL(int am, int cycles) {
	cyc = cycles;
	word target;
	byte tmpb;
	int tmpbit = 0;

	if(am==ACCUMULATOR) {
		if((A & B8)==B8) tmpbit = 1;
		A = (A << 1) & 0xfe;
		if((P & PFLAG_CARRY)==PFLAG_CARRY) A = (A | B1);
		setZeroAndNegativePFlags(&A);
	}
	else {
		resolveAddressModeTarget(am, &target);
		tmpb = memReadByte(target);
		if((tmpb & B8)==B8) tmpbit = 1;
		tmpb = (tmpb << 1) & 0xfe;
		if((P & PFLAG_CARRY)==PFLAG_CARRY) tmpb = (tmpb | B1);
		setZeroAndNegativePFlags(&tmpb);
		memWriteByte(target, tmpb);
	}

	if(tmpbit) setPFlag(PFLAG_CARRY);
	else clearPFlag(PFLAG_CARRY);
}

void mneROR(int am, int cycles) {
	cyc = cycles;
	word target;
	byte tmpb;
	int tmpbit = 0;
	if(am==ACCUMULATOR) {
		if(A & B1) tmpbit = 1;
		A = (A >> 1) & 0x7f;
		A = A | ((P & PFLAG_CARRY) ? 0x80 : 0x00);
		tmpbit ? setPFlag(PFLAG_CARRY) : clearPFlag(PFLAG_CARRY);
		setZeroAndNegativePFlags(&A);
	}
	else {
		resolveAddressModeTarget(am, &target);
		tmpb = memReadByte(target);
		
		if(tmpb & B1) tmpbit = 1;
		tmpb = (tmpb >> 1) & 0x7f;
		tmpb = tmpb | ((P & PFLAG_CARRY) ? 0x80 : 0x00);
		tmpbit ? setPFlag(PFLAG_CARRY) : clearPFlag(PFLAG_CARRY);
		setZeroAndNegativePFlags(&tmpb);
		memWriteByte(target, tmpb);
	}
}

void mneRTI(int am, int cycles) {
	cyc = cycles;
	P = stackPull();
	PC = stackPull() + (stackPull() << 8);
	//printf1("RTI pulled PC value %x from stack",PC);
}

void mneRTS(int am, int cycles) {
	cyc = cycles;
	PC = stackPull() + (stackPull() << 8) + 1;
}

void mneSBC(int am, int cycles) {
        word target;
        cyc = cycles + resolveAddressModeTarget(am, &target);
	byte targetValue = ~memReadByte(target);
        word tmp = targetValue + ((P & PFLAG_CARRY) ? 1 : 0);
        word t = A + tmp;
        if((t&0xff)==0) setPFlag(PFLAG_ZERO);
        else clearPFlag(PFLAG_ZERO);

        if(P & PFLAG_DECIMAL) {
        	((A^t)&(tmp^t)&0x80) ? setPFlag(PFLAG_OVERFLOW) : clearPFlag(PFLAG_OVERFLOW);
		/*
		problem with these values:
		A=99
		src=88
		temppu = 99-88-1 = 10
		Tests (sbc*) says V should be set 
		*/
		//(((AAA^temppu) & 0x80) && ((AAA^src) & 0x80)) ? setPFlag(PFLAG_OVERFLOW) : clearPFlag(PFLAG_OVERFLOW);
		targetValue = ~targetValue;
                // Decimal mode according to http://www.6502.org/tutorials/decimal_mode.html
		/*
		3a. AL = (A & $0F) - (B & $0F) + C-1
		3b. If AL < 0, then AL = ((AL - $06) & $0F) - $10
		3c. A = (A & $F0) - (B & $F0) + AL
		3d. If A < 0, then A = A - $60
		3e. The accumulator result is the lower 8 bits of A
		*/
                sword tmpA = A;
                sword AL = (tmpA & 0xf) - (targetValue & 0xf) + ((P & PFLAG_CARRY) ? 1 : 0) - 1;
                if(AL<0x0) AL = ((AL - 0x6) & 0xf) - 0x10;
                tmpA = (tmpA & 0xf0) - (targetValue & 0xf0) + AL;
                if(B8 & tmpA) setPFlag(PFLAG_NEGATIVE);
                else clearPFlag(PFLAG_NEGATIVE);
                if(tmpA < 0x0) tmpA = tmpA - 0x60;
                A = tmpA & 0xff;
        }
        else {
        	((A^t)&(tmp^t)&0x80) ? setPFlag(PFLAG_OVERFLOW) : clearPFlag(PFLAG_OVERFLOW);
                A = t & 0xff;
        	if(B8 & A) setPFlag(PFLAG_NEGATIVE);
        	else clearPFlag(PFLAG_NEGATIVE);
        }
        if(t>0xff) setPFlag(PFLAG_CARRY);
        else clearPFlag(PFLAG_CARRY);

}

void mneSEC(int am, int cycles) {
	cyc = cycles;
	setPFlag(PFLAG_CARRY);
}

void mneSED(int am, int cycles) {
	cyc = cycles;
	setPFlag(PFLAG_DECIMAL);
}

void mneSEI(int am, int cycles) {
	cyc = cycles;
	setPFlag(PFLAG_IRQ);
}

void mneSTA(int am, int cycles) {
	word target;
	cyc = cycles;
	resolveAddressModeTarget(am, &target);
	memWriteByte(target, A);
}

void mneSTX(int am, int cycles) {
	cyc = cycles;
	word target;
	resolveAddressModeTarget(am, &target);
	memWriteByte(target, X);
}

void mneSTY(int am, int cycles) {
	cyc = cycles;
	word target;
	resolveAddressModeTarget(am, &target);
	memWriteByte(target, Y);
}

void mneTAX(int am, int cycles) {
	cyc = cycles;
	X = A;
	setZeroAndNegativePFlags(&X);
}

void mneTAY(int am, int cycles) {
	cyc = cycles;
	Y = A;
	setZeroAndNegativePFlags(&Y);
}

void mneTSX(int am, int cycles) {
	cyc = cycles;
	X = S;
	setZeroAndNegativePFlags(&X);
}

void mneTXA(int am, int cycles) {
	cyc = cycles;
	A = X;
	setZeroAndNegativePFlags(&A);
}

void mneTXS(int am, int cycles) {
	cyc = cycles;
	S=X;
}

void mneTYA(int am, int cycles) {
	cyc = cycles;
	A = Y;
	setZeroAndNegativePFlags(&A);
}


/* non-standard and undocumented op-codes */
void mneASR(int am, int cycles) {
	// ASR (aka ALR)     $4B       A <- [(A /\ M) >> 1]       (Immediate)      1/2
	cyc = cycles;
	word target;
	resolveAddressModeTarget(am, &target);
	byte tmpVal = (A & memReadByte(target));
	(tmpVal & B1) ? setPFlag(PFLAG_CARRY) : clearPFlag(PFLAG_CARRY);
	A = (tmpVal >> 1);
	setZeroAndNegativePFlags(&A);
}

void mneANC(int am, int cycles) {
	//ANC      $0B       A <- A /\ M, C=~A7         (Immediate)      1/2
	cyc = cycles;
	word target;
	resolveAddressModeTarget(am, &target);
	A = A & memReadByte(target);
	// I guess there's a error in a doc, C doesn't seem to be ~A7
	(A & B8) ? setPFlag(PFLAG_CARRY) : clearPFlag(PFLAG_CARRY);
	setZeroAndNegativePFlags(&A);
}

void mneANE(int am, int cycles) {
	// ANE       $8B       M <-[(A)\/$EE] /\ (X)/\(M) (Immediate)      2/2^4
	// A = (A | #$EE) & X & #byte
	cyc = cycles;
	word target;
	resolveAddressModeTarget(am, &target);
	A = (A|0xee) & X & memReadByte(target);	
	setZeroAndNegativePFlags(&A);
}

void mneARR(int am, int cycles) {
	// see long description of this PITA opcode in
	// http://www.zimmers.net/anonftp/pub/cbm/documents/chipdata/64doc
	cyc = cycles;
	word target;
	resolveAddressModeTarget(am, &target);
	byte targetVal = memReadByte(target);
	byte maskC = (P & PFLAG_CARRY) ? 0b10000000 : 0;

	if(P & PFLAG_DECIMAL) {
		byte t = A & targetVal;
		byte AH = t >> 4;
		byte AL = t & 0xf;
		A = (t >> 1) | maskC;
		A ? clearPFlag(PFLAG_ZERO) : setPFlag(PFLAG_ZERO);
		maskC ? setPFlag(PFLAG_NEGATIVE) : clearPFlag(PFLAG_NEGATIVE);
		((t ^ A) & 0x40) ? setPFlag(PFLAG_OVERFLOW) : clearPFlag(PFLAG_OVERFLOW);
		if (AL + (AL & 1) > 5)
			A = (A & 0xf0) | ((A + 6) & 0xf);
		if((AH + (AH & 1)) > 5) {
			setPFlag(PFLAG_CARRY);
			A = (A + 0x60) & 0xff;
		}
		else {
			clearPFlag(PFLAG_CARRY);
		}
	}
	else {
		A = ((A & targetVal) >> 1) | maskC;
		setZeroAndNegativePFlags(&A);
		byte aV = A & B7;
		aV ? setPFlag(PFLAG_CARRY) : clearPFlag(PFLAG_CARRY);
		byte bV = (A & B6) << 1;
		(aV ^ bV) ? setPFlag(PFLAG_OVERFLOW) : clearPFlag(PFLAG_OVERFLOW);
	}
}


void mneSLO(int am, int cycles) {
	// aka ASO
	// This opcode ASLs the contents of a memory location and then ORs the result with the accumulator.  
	cyc = cycles;
	word target;
	resolveAddressModeTarget(am, &target);
	byte targetVal = memReadByte(target);
	targetVal & B8 ? setPFlag(PFLAG_CARRY) : clearPFlag(PFLAG_CARRY);
	targetVal = (targetVal << 1);
	memWriteByte(target, targetVal);
	A = A | targetVal;
	setZeroAndNegativePFlags(&A);
}

void mneAXS(int am, int cycles) {
	// aka SAX
	cyc = cycles;
        word target;
        resolveAddressModeTarget(am, &target);
	memWriteByte(target, (A & X));
}

void mneDCP(int am, int cycles) {
	// aka DCM
	cyc = cycles;
	word target;
        resolveAddressModeTarget(am, &target);
        byte targetVal = memReadByte(target);
        targetVal & B8 ? setPFlag(PFLAG_CARRY) : clearPFlag(PFLAG_CARRY);
        targetVal--;
        memWriteByte(target, targetVal);
       	targetVal = A - targetVal;
        setZeroAndNegativePFlags(&targetVal);
        if(A>=targetVal) setPFlag(PFLAG_CARRY);
        else clearPFlag(PFLAG_CARRY); 
}

void mneINS(int am, int cycles) {
	// aka ISC or ISB
	// This opcode INCs the contents of a memory location and then SBCs the result from the A register.
        cyc = cycles;

        // INC PART
        word target;
        resolveAddressModeTarget(am, &target);
        byte targetValue = memReadByte(target);
        targetValue++;
        memWriteByte(target, targetValue);

	// SBC PART
        targetValue = ~targetValue;
        word tmp = targetValue + ((P & PFLAG_CARRY) ? 1 : 0);
        word t = A + tmp;
        if((t&0xff)==0) setPFlag(PFLAG_ZERO);
        else clearPFlag(PFLAG_ZERO);

        if(P & PFLAG_DECIMAL) {
                ((A^t)&(tmp^t)&0x80) ? setPFlag(PFLAG_OVERFLOW) : clearPFlag(PFLAG_OVERFLOW);
                targetValue = ~targetValue;
                sword tmpA = A;
                sword AL = (tmpA & 0xf) - (targetValue & 0xf) + ((P & PFLAG_CARRY) ? 1 : 0) - 1;
                if(AL<0x0) AL = ((AL - 0x6) & 0xf) - 0x10;
                tmpA = (tmpA & 0xf0) - (targetValue & 0xf0) + AL;
                if(B8 & tmpA) setPFlag(PFLAG_NEGATIVE);
                else clearPFlag(PFLAG_NEGATIVE);
                if(tmpA < 0x0) tmpA = tmpA - 0x60;
                A = tmpA & 0xff;
        }
        else {
                ((A^t)&(tmp^t)&0x80) ? setPFlag(PFLAG_OVERFLOW) : clearPFlag(PFLAG_OVERFLOW);
                A = t & 0xff;
                if(B8 & A) setPFlag(PFLAG_NEGATIVE);
                else clearPFlag(PFLAG_NEGATIVE);
        }
        if(t>0xff) setPFlag(PFLAG_CARRY);
        else clearPFlag(PFLAG_CARRY);
}

void mneLAS(int am, int cycles) {
	word target;
	cyc = cycles + resolveAddressModeTarget(am, &target);
        byte targetVal = memReadByte(target);
	byte tmp = targetVal & S;
	A = tmp;
	X = tmp;
	S = tmp;
        setZeroAndNegativePFlags(&A);
}

void mneLAX(int am, int cycles) {
	cyc = cycles;
	word target;
        cyc += resolveAddressModeTarget(am, &target);
        byte targetVal = memReadByte(target);
	A = targetVal;
	X = targetVal;
        setZeroAndNegativePFlags(&A);
}

void mneLSE(int am, int cycles) {
	// aka SRE
	/* LSE LSRs the contents of a memory location and then EORs the result with the accumulator. */
	cyc = cycles;
        word target;
        resolveAddressModeTarget(am, &target);
        byte tmp = memReadByte(target);
	(tmp & B1) ? setPFlag(PFLAG_CARRY) : clearPFlag(PFLAG_CARRY);
        tmp = (tmp >> 1) & 0x7f;
	memWriteByte(target, tmp);
	A ^=tmp;
        setZeroAndNegativePFlags(&A);
}

void mneLXA(int am, int cycles) {
	// aka OAL
	// only IMMEDIATE mode
	/* This opcode ORs the A register with #$EE, ANDs the result with an immediate 
	value, and then stores the result in both A and X. */
	cyc = cycles;
        word target;
        resolveAddressModeTarget(am, &target);
        byte tmp = memReadByte(target);
	A |= 0xee;
	A &= tmp;
	X = A;
        setZeroAndNegativePFlags(&A);
}

void mneSKW(int am, int cycles) {
	// skips next word, some kind of nop op
	word target;
	cyc = cycles + resolveAddressModeTarget(am, &target);
	memReadByte(target);
}

void mneSKB(int am, int cycles) {
	// skips next byte
	// no varying timing
	cyc = cycles;
	word target;
	resolveAddressModeTarget(am, &target);
	memReadByte(target);
}

void mneRLA(int am, int cycles) {
	cyc = cycles;
        word target;
        resolveAddressModeTarget(am, &target);
        byte tmp = memReadByte(target);
	byte carry = (P & PFLAG_CARRY) ? 1 : 0;
	(tmp & B8) ? setPFlag(PFLAG_CARRY) : clearPFlag(PFLAG_CARRY);
        tmp = (tmp << 1) + carry;
	memWriteByte(target, tmp);
	A &=tmp;
        setZeroAndNegativePFlags(&A);
}
	
void mneRRA(int am, int cycles) {
	/* RRA RORs the contents of a memory location and then ADCs the result with 
	   the accumulator. */
	cyc = cycles;

	// ROR PART
	word target;
	int tmpbit = 0;
	resolveAddressModeTarget(am, &target);
	byte targetValue = memReadByte(target);
	if(targetValue & B1) tmpbit = 1;
	targetValue = (targetValue >> 1) & 0x7f;
	targetValue = targetValue | ((P & PFLAG_CARRY) ? 0x80 : 0x00);
	tmpbit ? setPFlag(PFLAG_CARRY) : clearPFlag(PFLAG_CARRY);
	memWriteByte(target, targetValue);
	
	// ADC PART
	word tmp = targetValue + ((P & PFLAG_CARRY) ? 1 : 0);
	word t = A + tmp;
	((A^t)&(targetValue^t)&0x80) ? setPFlag(PFLAG_OVERFLOW) : clearPFlag(PFLAG_OVERFLOW);
	if((t&0xff)==0) setPFlag(PFLAG_ZERO);
	else clearPFlag(PFLAG_ZERO);

	if(P & PFLAG_DECIMAL) {
		// Decimal mode according to http://www.6502.org/tutorials/decimal_mode.html
		word tmpA = A;
		word AL = (tmpA & 0xf) + (targetValue & 0xf) + ((P & PFLAG_CARRY) ? 1 : 0);
		if(AL>=0xa) AL = ((AL + 0x6) & 0xf) + 0x10;
		tmpA = (tmpA & 0xf0) + (targetValue & 0xf0) + AL;
		if(B8 & tmpA) setPFlag(PFLAG_NEGATIVE);
		else clearPFlag(PFLAG_NEGATIVE);
		if(tmpA >= 0xa0) tmpA = tmpA + 0x60;
		A = tmpA & 0xff;
		if(tmpA>=0x100) setPFlag(PFLAG_CARRY);
		else clearPFlag(PFLAG_CARRY);
	}
	else {
		A = t & 0xff;
		if(B8 & A) setPFlag(PFLAG_NEGATIVE);
		else clearPFlag(PFLAG_NEGATIVE);
		if(t>0xff) setPFlag(PFLAG_CARRY);
		else clearPFlag(PFLAG_CARRY);
	}
}

void mneSBX(int am, int cycles) {
	// aka SAX
	cyc = cycles;
	word target;
	resolveAddressModeTarget(am, &target);
	X = A & X;	
	byte value = memReadByte(target);
        (X>=value) ? setPFlag(PFLAG_CARRY) : clearPFlag(PFLAG_CARRY);
         X -= value;
        setZeroAndNegativePFlags(&X);
}

void mneSHA(int am, int cycles) {
	// aka AXA
	cyc = cycles;
	word target;
	resolveAddressModeTarget(am, &target);
	byte crazyvalue = (target >> 8) + 1;
	memWriteByte(target, A & X & crazyvalue);
}

void mneSHS(int am, int cycles) {
	// aka TAS
        cyc = cycles;
        word target;
        resolveAddressModeTarget(am, &target);
        byte crazyvalue = (target >> 8) + 1;
	S = A & X;
        memWriteByte(target, S & crazyvalue);	
}

void mneSHX(int am, int cycles) {
	// aka XAS
	cyc = cycles;
        word target;
        resolveAddressModeTarget(am, &target);
        byte crazyvalue = (target >> 8) + 1;
        memWriteByte(target, X & crazyvalue);
}

void mneSHY(int am, int cycles) {
	// aka SAY
	cyc = cycles;
        word target;
        resolveAddressModeTarget(am, &target);
        byte crazyvalue = (target >> 8) + 1;
        memWriteByte(target, Y & crazyvalue);
}

void initMnemonicArray() {
	byte i;
	i=0x00; mneAM[i].pt2MnemonicHandler = &mneBRK; mneAM[i].cycles = 7; mneAM[i].am = IMPLIED;
	i=0x01; mneAM[i].pt2MnemonicHandler = &mneORA; mneAM[i].cycles = 6; mneAM[i].am = INDIRECT_X;
	i=0x02; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x03; mneAM[i].pt2MnemonicHandler = &mneSLO; mneAM[i].cycles = 8; mneAM[i].am = INDIRECT_X;
	i=0x04; mneAM[i].pt2MnemonicHandler = &mneSKB; mneAM[i].cycles = 3; mneAM[i].am = ZEROPAGE;
	i=0x05; mneAM[i].pt2MnemonicHandler = &mneORA; mneAM[i].cycles = 3; mneAM[i].am = ZEROPAGE;
	i=0x06; mneAM[i].pt2MnemonicHandler = &mneASL; mneAM[i].cycles = 5; mneAM[i].am = ZEROPAGE;
	i=0x07; mneAM[i].pt2MnemonicHandler = &mneSLO; mneAM[i].cycles = 5; mneAM[i].am = ZEROPAGE;
	i=0x08; mneAM[i].pt2MnemonicHandler = &mnePHP; mneAM[i].cycles = 3; mneAM[i].am = IMPLIED;
	i=0x09; mneAM[i].pt2MnemonicHandler = &mneORA; mneAM[i].cycles = 2; mneAM[i].am = IMMEDIATE;
	i=0x0a; mneAM[i].pt2MnemonicHandler = &mneASL; mneAM[i].cycles = 2; mneAM[i].am = ACCUMULATOR;
	i=0x0b; mneAM[i].pt2MnemonicHandler = &mneANC; mneAM[i].cycles = 2; mneAM[i].am = IMMEDIATE;
	i=0x0c; mneAM[i].pt2MnemonicHandler = &mneSKW; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE; // somewhere abs_x, but should be abs
	i=0x0d; mneAM[i].pt2MnemonicHandler = &mneORA; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE;
	i=0x0e; mneAM[i].pt2MnemonicHandler = &mneASL; mneAM[i].cycles = 6; mneAM[i].am = ABSOLUTE;
	i=0x0f; mneAM[i].pt2MnemonicHandler = &mneSLO; mneAM[i].cycles = 6; mneAM[i].am = ABSOLUTE;
	i=0x10; mneAM[i].pt2MnemonicHandler = &mneBPL; mneAM[i].cycles = 2; mneAM[i].am = RELATIVE;
	i=0x11; mneAM[i].pt2MnemonicHandler = &mneORA; mneAM[i].cycles = 5; mneAM[i].am = INDIRECT_Y;
	i=0x12; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x13; mneAM[i].pt2MnemonicHandler = &mneSLO; mneAM[i].cycles = 8; mneAM[i].am = INDIRECT_Y;
	i=0x14; mneAM[i].pt2MnemonicHandler = &mneSKB; mneAM[i].cycles = 4; mneAM[i].am = ZEROPAGE_X;
	i=0x15; mneAM[i].pt2MnemonicHandler = &mneORA; mneAM[i].cycles = 4; mneAM[i].am = ZEROPAGE_X;
	i=0x16; mneAM[i].pt2MnemonicHandler = &mneASL; mneAM[i].cycles = 6; mneAM[i].am = ZEROPAGE_X;
	i=0x17; mneAM[i].pt2MnemonicHandler = &mneSLO; mneAM[i].cycles = 6; mneAM[i].am = ZEROPAGE_X;
	i=0x18; mneAM[i].pt2MnemonicHandler = &mneCLC; mneAM[i].cycles = 2; mneAM[i].am = IMPLIED;
	i=0x19; mneAM[i].pt2MnemonicHandler = &mneORA; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE_Y;
	i=0x1a; mneAM[i].pt2MnemonicHandler = &mneNOP; mneAM[i].cycles = 2; mneAM[i].am = IMPLIED;
	i=0x1b; mneAM[i].pt2MnemonicHandler = &mneSLO; mneAM[i].cycles = 7; mneAM[i].am = ABSOLUTE_Y;
	i=0x1c; mneAM[i].pt2MnemonicHandler = &mneSKW; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE_X;
	i=0x1d; mneAM[i].pt2MnemonicHandler = &mneORA; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE_X;
	i=0x1e; mneAM[i].pt2MnemonicHandler = &mneASL; mneAM[i].cycles = 7; mneAM[i].am = ABSOLUTE_X;
	i=0x1f; mneAM[i].pt2MnemonicHandler = &mneSLO; mneAM[i].cycles = 7; mneAM[i].am = ABSOLUTE_X;
	i=0x20; mneAM[i].pt2MnemonicHandler = &mneJSR; mneAM[i].cycles = 6; mneAM[i].am = ABSOLUTE;
	i=0x21; mneAM[i].pt2MnemonicHandler = &mneAND; mneAM[i].cycles = 6; mneAM[i].am = INDIRECT_X;
	i=0x22; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x23; mneAM[i].pt2MnemonicHandler = &mneRLA; mneAM[i].cycles = 8; mneAM[i].am = INDIRECT_X;
	i=0x24; mneAM[i].pt2MnemonicHandler = &mneBIT; mneAM[i].cycles = 3; mneAM[i].am = ZEROPAGE;
	i=0x25; mneAM[i].pt2MnemonicHandler = &mneAND; mneAM[i].cycles = 3; mneAM[i].am = ZEROPAGE;
	i=0x26; mneAM[i].pt2MnemonicHandler = &mneROL; mneAM[i].cycles = 5; mneAM[i].am = ZEROPAGE;
	i=0x27; mneAM[i].pt2MnemonicHandler = &mneRLA; mneAM[i].cycles = 5; mneAM[i].am = ZEROPAGE;
	i=0x28; mneAM[i].pt2MnemonicHandler = &mnePLP; mneAM[i].cycles = 4; mneAM[i].am = IMPLIED;
	i=0x29; mneAM[i].pt2MnemonicHandler = &mneAND; mneAM[i].cycles = 2; mneAM[i].am = IMMEDIATE;
	i=0x2a; mneAM[i].pt2MnemonicHandler = &mneROL; mneAM[i].cycles = 2; mneAM[i].am = ACCUMULATOR;
	i=0x2b; mneAM[i].pt2MnemonicHandler = &mneANC; mneAM[i].cycles = 2; mneAM[i].am = IMMEDIATE;
	i=0x2c; mneAM[i].pt2MnemonicHandler = &mneBIT; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE;
	i=0x2d; mneAM[i].pt2MnemonicHandler = &mneAND; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE;
	i=0x2e; mneAM[i].pt2MnemonicHandler = &mneROL; mneAM[i].cycles = 6; mneAM[i].am = ABSOLUTE;
	i=0x2f; mneAM[i].pt2MnemonicHandler = &mneRLA; mneAM[i].cycles = 6; mneAM[i].am = ABSOLUTE;
	i=0x30; mneAM[i].pt2MnemonicHandler = &mneBMI; mneAM[i].cycles = 2; mneAM[i].am = RELATIVE;
	i=0x31; mneAM[i].pt2MnemonicHandler = &mneAND; mneAM[i].cycles = 5; mneAM[i].am = INDIRECT_Y;
	i=0x32; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x33; mneAM[i].pt2MnemonicHandler = &mneRLA; mneAM[i].cycles = 8; mneAM[i].am = INDIRECT_Y;
	i=0x34; mneAM[i].pt2MnemonicHandler = &mneSKB; mneAM[i].cycles = 4; mneAM[i].am = ZEROPAGE_X;
	i=0x35; mneAM[i].pt2MnemonicHandler = &mneAND; mneAM[i].cycles = 4; mneAM[i].am = ZEROPAGE_X;
	i=0x36; mneAM[i].pt2MnemonicHandler = &mneROL; mneAM[i].cycles = 6; mneAM[i].am = ZEROPAGE_X;
	i=0x37; mneAM[i].pt2MnemonicHandler = &mneRLA; mneAM[i].cycles = 6; mneAM[i].am = ZEROPAGE_X;
	i=0x38; mneAM[i].pt2MnemonicHandler = &mneSEC; mneAM[i].cycles = 2; mneAM[i].am = IMPLIED;
	i=0x39; mneAM[i].pt2MnemonicHandler = &mneAND; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE_Y;
	i=0x3a; mneAM[i].pt2MnemonicHandler = &mneNOP; mneAM[i].cycles = 2; mneAM[i].am = IMPLIED;
	i=0x3b; mneAM[i].pt2MnemonicHandler = &mneRLA; mneAM[i].cycles = 7; mneAM[i].am = ABSOLUTE_Y;
	i=0x3c; mneAM[i].pt2MnemonicHandler = &mneSKW; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE_X;
	i=0x3d; mneAM[i].pt2MnemonicHandler = &mneAND; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE_X;
	i=0x3e; mneAM[i].pt2MnemonicHandler = &mneROL; mneAM[i].cycles = 7; mneAM[i].am = ABSOLUTE_X;
	i=0x3f; mneAM[i].pt2MnemonicHandler = &mneRLA; mneAM[i].cycles = 7; mneAM[i].am = ABSOLUTE_X;
	i=0x40; mneAM[i].pt2MnemonicHandler = &mneRTI; mneAM[i].cycles = 6; mneAM[i].am = IMPLIED;
	i=0x41; mneAM[i].pt2MnemonicHandler = &mneEOR; mneAM[i].cycles = 6; mneAM[i].am = INDIRECT_X;
	i=0x42; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x43; mneAM[i].pt2MnemonicHandler = &mneLSE; mneAM[i].cycles = 8; mneAM[i].am = INDIRECT_X;
	i=0x44; mneAM[i].pt2MnemonicHandler = &mneSKB; mneAM[i].cycles = 3; mneAM[i].am = ZEROPAGE;
	i=0x45; mneAM[i].pt2MnemonicHandler = &mneEOR; mneAM[i].cycles = 3; mneAM[i].am = ZEROPAGE;
	i=0x46; mneAM[i].pt2MnemonicHandler = &mneLSR; mneAM[i].cycles = 5; mneAM[i].am = ZEROPAGE;
	i=0x47; mneAM[i].pt2MnemonicHandler = &mneLSE; mneAM[i].cycles = 5; mneAM[i].am = ZEROPAGE;
	i=0x48; mneAM[i].pt2MnemonicHandler = &mnePHA; mneAM[i].cycles = 3; mneAM[i].am = IMPLIED;
	i=0x49; mneAM[i].pt2MnemonicHandler = &mneEOR; mneAM[i].cycles = 2; mneAM[i].am = IMMEDIATE;
	i=0x4a; mneAM[i].pt2MnemonicHandler = &mneLSR; mneAM[i].cycles = 2; mneAM[i].am = ACCUMULATOR;
	i=0x4b; mneAM[i].pt2MnemonicHandler = &mneASR; mneAM[i].cycles = 2; mneAM[i].am = IMMEDIATE;	// ASR aka ALR
	i=0x4c; mneAM[i].pt2MnemonicHandler = &mneJMP; mneAM[i].cycles = 3; mneAM[i].am = ABSOLUTE;
	i=0x4d; mneAM[i].pt2MnemonicHandler = &mneEOR; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE;
	i=0x4e; mneAM[i].pt2MnemonicHandler = &mneLSR; mneAM[i].cycles = 6; mneAM[i].am = ABSOLUTE;
	i=0x4f; mneAM[i].pt2MnemonicHandler = &mneLSE; mneAM[i].cycles = 6; mneAM[i].am = ABSOLUTE;
	i=0x50; mneAM[i].pt2MnemonicHandler = &mneBVC; mneAM[i].cycles = 2; mneAM[i].am = RELATIVE;
	i=0x51; mneAM[i].pt2MnemonicHandler = &mneEOR; mneAM[i].cycles = 5; mneAM[i].am = INDIRECT_Y;
	i=0x52; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x53; mneAM[i].pt2MnemonicHandler = &mneLSE; mneAM[i].cycles = 8; mneAM[i].am = INDIRECT_Y;
	i=0x54; mneAM[i].pt2MnemonicHandler = &mneSKB; mneAM[i].cycles = 4; mneAM[i].am = ZEROPAGE_X;
	i=0x55; mneAM[i].pt2MnemonicHandler = &mneEOR; mneAM[i].cycles = 4; mneAM[i].am = ZEROPAGE_X;
	i=0x56; mneAM[i].pt2MnemonicHandler = &mneLSR; mneAM[i].cycles = 6; mneAM[i].am = ZEROPAGE_X;
	i=0x57; mneAM[i].pt2MnemonicHandler = &mneLSE; mneAM[i].cycles = 6; mneAM[i].am = ZEROPAGE_X;
	i=0x58; mneAM[i].pt2MnemonicHandler = &mneCLI; mneAM[i].cycles = 2; mneAM[i].am = IMPLIED;
	i=0x59; mneAM[i].pt2MnemonicHandler = &mneEOR; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE_Y;
	i=0x5a; mneAM[i].pt2MnemonicHandler = &mneNOP; mneAM[i].cycles = 2; mneAM[i].am = IMPLIED;
	i=0x5b; mneAM[i].pt2MnemonicHandler = &mneLSE; mneAM[i].cycles = 7; mneAM[i].am = ABSOLUTE_Y;
	i=0x5c; mneAM[i].pt2MnemonicHandler = &mneSKW; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE_X;
	i=0x5d; mneAM[i].pt2MnemonicHandler = &mneEOR; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE_X;
	i=0x5e; mneAM[i].pt2MnemonicHandler = &mneLSR; mneAM[i].cycles = 7; mneAM[i].am = ABSOLUTE_X;
	i=0x5f; mneAM[i].pt2MnemonicHandler = &mneLSE; mneAM[i].cycles = 7; mneAM[i].am = ABSOLUTE_X;
	i=0x60; mneAM[i].pt2MnemonicHandler = &mneRTS; mneAM[i].cycles = 6; mneAM[i].am = IMPLIED;
	i=0x61; mneAM[i].pt2MnemonicHandler = &mneADC; mneAM[i].cycles = 6; mneAM[i].am = INDIRECT_X;
	i=0x62; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x63; mneAM[i].pt2MnemonicHandler = &mneRRA; mneAM[i].cycles = 8; mneAM[i].am = INDIRECT_X;
	i=0x64; mneAM[i].pt2MnemonicHandler = &mneSKB; mneAM[i].cycles = 3; mneAM[i].am = ZEROPAGE;
	i=0x65; mneAM[i].pt2MnemonicHandler = &mneADC; mneAM[i].cycles = 3; mneAM[i].am = ZEROPAGE;
	i=0x66; mneAM[i].pt2MnemonicHandler = &mneROR; mneAM[i].cycles = 5; mneAM[i].am = ZEROPAGE;
	i=0x67; mneAM[i].pt2MnemonicHandler = &mneRRA; mneAM[i].cycles = 5; mneAM[i].am = ZEROPAGE;
	i=0x68; mneAM[i].pt2MnemonicHandler = &mnePLA; mneAM[i].cycles = 4; mneAM[i].am = IMPLIED;
	i=0x69; mneAM[i].pt2MnemonicHandler = &mneADC; mneAM[i].cycles = 2; mneAM[i].am = IMMEDIATE;
	i=0x6a; mneAM[i].pt2MnemonicHandler = &mneROR; mneAM[i].cycles = 2; mneAM[i].am = ACCUMULATOR;
	i=0x6b; mneAM[i].pt2MnemonicHandler = &mneARR; mneAM[i].cycles = 2; mneAM[i].am = IMMEDIATE;
	i=0x6c; mneAM[i].pt2MnemonicHandler = &mneJMP; mneAM[i].cycles = 5; mneAM[i].am = INDIRECT;
	i=0x6d; mneAM[i].pt2MnemonicHandler = &mneADC; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE;
	i=0x6e; mneAM[i].pt2MnemonicHandler = &mneROR; mneAM[i].cycles = 6; mneAM[i].am = ABSOLUTE;
	i=0x6f; mneAM[i].pt2MnemonicHandler = &mneRRA; mneAM[i].cycles = 6; mneAM[i].am = ABSOLUTE;
	i=0x70; mneAM[i].pt2MnemonicHandler = &mneBVS; mneAM[i].cycles = 2; mneAM[i].am = RELATIVE;
	i=0x71; mneAM[i].pt2MnemonicHandler = &mneADC; mneAM[i].cycles = 5; mneAM[i].am = INDIRECT_Y;
	i=0x72; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x73; mneAM[i].pt2MnemonicHandler = &mneRRA; mneAM[i].cycles = 8; mneAM[i].am = INDIRECT_Y;
	i=0x74; mneAM[i].pt2MnemonicHandler = &mneSKB; mneAM[i].cycles = 4; mneAM[i].am = ZEROPAGE_X;
	i=0x75; mneAM[i].pt2MnemonicHandler = &mneADC; mneAM[i].cycles = 4; mneAM[i].am = ZEROPAGE_X;
	i=0x76; mneAM[i].pt2MnemonicHandler = &mneROR; mneAM[i].cycles = 6; mneAM[i].am = ZEROPAGE_X;
	i=0x77; mneAM[i].pt2MnemonicHandler = &mneRRA; mneAM[i].cycles = 6; mneAM[i].am = ZEROPAGE_X;
	i=0x78; mneAM[i].pt2MnemonicHandler = &mneSEI; mneAM[i].cycles = 2; mneAM[i].am = IMPLIED;
	i=0x79; mneAM[i].pt2MnemonicHandler = &mneADC; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE_Y;
	i=0x7a; mneAM[i].pt2MnemonicHandler = &mneNOP; mneAM[i].cycles = 2; mneAM[i].am = IMPLIED;
	i=0x7b; mneAM[i].pt2MnemonicHandler = &mneRRA; mneAM[i].cycles = 7; mneAM[i].am = ABSOLUTE_Y;
	i=0x7c; mneAM[i].pt2MnemonicHandler = &mneSKW; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE_X;
	i=0x7d; mneAM[i].pt2MnemonicHandler = &mneADC; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE_X;
	i=0x7e; mneAM[i].pt2MnemonicHandler = &mneROR; mneAM[i].cycles = 7; mneAM[i].am = ABSOLUTE_X;
	i=0x7f; mneAM[i].pt2MnemonicHandler = &mneRRA; mneAM[i].cycles = 7; mneAM[i].am = ABSOLUTE_X;
	i=0x80; mneAM[i].pt2MnemonicHandler = &mneSKB; mneAM[i].cycles = 2; mneAM[i].am = IMMEDIATE;
	i=0x81; mneAM[i].pt2MnemonicHandler = &mneSTA; mneAM[i].cycles = 6; mneAM[i].am = INDIRECT_X;
	i=0x82; mneAM[i].pt2MnemonicHandler = &mneSKB; mneAM[i].cycles = 2; mneAM[i].am = IMMEDIATE;
	i=0x83; mneAM[i].pt2MnemonicHandler = &mneAXS; mneAM[i].cycles = 6; mneAM[i].am = INDIRECT_X;
	i=0x84; mneAM[i].pt2MnemonicHandler = &mneSTY; mneAM[i].cycles = 3; mneAM[i].am = ZEROPAGE;
	i=0x85; mneAM[i].pt2MnemonicHandler = &mneSTA; mneAM[i].cycles = 3; mneAM[i].am = ZEROPAGE;
	i=0x86; mneAM[i].pt2MnemonicHandler = &mneSTX; mneAM[i].cycles = 3; mneAM[i].am = ZEROPAGE;
	i=0x87; mneAM[i].pt2MnemonicHandler = &mneAXS; mneAM[i].cycles = 3; mneAM[i].am = ZEROPAGE;
	i=0x88; mneAM[i].pt2MnemonicHandler = &mneDEY; mneAM[i].cycles = 2; mneAM[i].am = IMPLIED;
	i=0x89; mneAM[i].pt2MnemonicHandler = &mneSKB; mneAM[i].cycles = 2; mneAM[i].am = IMMEDIATE;
	i=0x8a; mneAM[i].pt2MnemonicHandler = &mneTXA; mneAM[i].cycles = 2; mneAM[i].am = IMPLIED;
	i=0x8b; mneAM[i].pt2MnemonicHandler = &mneANE; mneAM[i].cycles = 2; mneAM[i].am = IMMEDIATE;
	i=0x8c; mneAM[i].pt2MnemonicHandler = &mneSTY; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE;
	i=0x8d; mneAM[i].pt2MnemonicHandler = &mneSTA; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE;
	i=0x8e; mneAM[i].pt2MnemonicHandler = &mneSTX; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE;
	i=0x8f; mneAM[i].pt2MnemonicHandler = &mneAXS; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE;
	i=0x90; mneAM[i].pt2MnemonicHandler = &mneBCC; mneAM[i].cycles = 2; mneAM[i].am = RELATIVE;
	i=0x91; mneAM[i].pt2MnemonicHandler = &mneSTA; mneAM[i].cycles = 6; mneAM[i].am = INDIRECT_Y;
	i=0x92; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x93; mneAM[i].pt2MnemonicHandler = &mneSHA; mneAM[i].cycles = 6; mneAM[i].am = INDIRECT_Y;
	i=0x94; mneAM[i].pt2MnemonicHandler = &mneSTY; mneAM[i].cycles = 4; mneAM[i].am = ZEROPAGE_X;
	i=0x95; mneAM[i].pt2MnemonicHandler = &mneSTA; mneAM[i].cycles = 4; mneAM[i].am = ZEROPAGE_X;
	i=0x96; mneAM[i].pt2MnemonicHandler = &mneSTX; mneAM[i].cycles = 4; mneAM[i].am = ZEROPAGE_Y;
	i=0x97; mneAM[i].pt2MnemonicHandler = &mneAXS; mneAM[i].cycles = 4; mneAM[i].am = ZEROPAGE_Y;
	i=0x98; mneAM[i].pt2MnemonicHandler = &mneTYA; mneAM[i].cycles = 2; mneAM[i].am = IMPLIED;
	i=0x99; mneAM[i].pt2MnemonicHandler = &mneSTA; mneAM[i].cycles = 5; mneAM[i].am = ABSOLUTE_Y;
	i=0x9a; mneAM[i].pt2MnemonicHandler = &mneTXS; mneAM[i].cycles = 2; mneAM[i].am = IMPLIED;
	i=0x9b; mneAM[i].pt2MnemonicHandler = &mneSHS; mneAM[i].cycles = 5; mneAM[i].am = ABSOLUTE_Y;
	i=0x9c; mneAM[i].pt2MnemonicHandler = &mneSHY; mneAM[i].cycles = 5; mneAM[i].am = ABSOLUTE_X;
	i=0x9d; mneAM[i].pt2MnemonicHandler = &mneSTA; mneAM[i].cycles = 5; mneAM[i].am = ABSOLUTE_X;
	i=0x9e; mneAM[i].pt2MnemonicHandler = &mneSHX; mneAM[i].cycles = 5; mneAM[i].am = ABSOLUTE_Y;
	i=0x9f; mneAM[i].pt2MnemonicHandler = &mneSHA; mneAM[i].cycles = 5; mneAM[i].am = ABSOLUTE_Y;
	i=0xa0; mneAM[i].pt2MnemonicHandler = &mneLDY; mneAM[i].cycles = 2; mneAM[i].am = IMMEDIATE;
	i=0xa1; mneAM[i].pt2MnemonicHandler = &mneLDA; mneAM[i].cycles = 6; mneAM[i].am = INDIRECT_X;
	i=0xa2; mneAM[i].pt2MnemonicHandler = &mneLDX; mneAM[i].cycles = 2; mneAM[i].am = IMMEDIATE;
	i=0xa3; mneAM[i].pt2MnemonicHandler = &mneLAX; mneAM[i].cycles = 6; mneAM[i].am = INDIRECT_X;
	i=0xa4; mneAM[i].pt2MnemonicHandler = &mneLDY; mneAM[i].cycles = 3; mneAM[i].am = ZEROPAGE;
	i=0xa5; mneAM[i].pt2MnemonicHandler = &mneLDA; mneAM[i].cycles = 3; mneAM[i].am = ZEROPAGE;
	i=0xa6; mneAM[i].pt2MnemonicHandler = &mneLDX; mneAM[i].cycles = 3; mneAM[i].am = ZEROPAGE;
	i=0xa7; mneAM[i].pt2MnemonicHandler = &mneLAX; mneAM[i].cycles = 3; mneAM[i].am = ZEROPAGE;
	i=0xa8; mneAM[i].pt2MnemonicHandler = &mneTAY; mneAM[i].cycles = 2; mneAM[i].am = IMPLIED;
	i=0xa9; mneAM[i].pt2MnemonicHandler = &mneLDA; mneAM[i].cycles = 2; mneAM[i].am = IMMEDIATE;
	i=0xaa; mneAM[i].pt2MnemonicHandler = &mneTAX; mneAM[i].cycles = 2; mneAM[i].am = IMPLIED;
	i=0xab; mneAM[i].pt2MnemonicHandler = &mneLXA; mneAM[i].cycles = 2; mneAM[i].am = IMMEDIATE;
	i=0xac; mneAM[i].pt2MnemonicHandler = &mneLDY; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE;
	i=0xad; mneAM[i].pt2MnemonicHandler = &mneLDA; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE;
	i=0xae; mneAM[i].pt2MnemonicHandler = &mneLDX; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE;
	i=0xaf; mneAM[i].pt2MnemonicHandler = &mneLAX; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE;
	i=0xb0; mneAM[i].pt2MnemonicHandler = &mneBCS; mneAM[i].cycles = 2; mneAM[i].am = RELATIVE;
	i=0xb1; mneAM[i].pt2MnemonicHandler = &mneLDA; mneAM[i].cycles = 5; mneAM[i].am = INDIRECT_Y;
	i=0xb2; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0xb3; mneAM[i].pt2MnemonicHandler = &mneLAX; mneAM[i].cycles = 5; mneAM[i].am = INDIRECT_Y;
	i=0xb4; mneAM[i].pt2MnemonicHandler = &mneLDY; mneAM[i].cycles = 4; mneAM[i].am = ZEROPAGE_X;
	i=0xb5; mneAM[i].pt2MnemonicHandler = &mneLDA; mneAM[i].cycles = 4; mneAM[i].am = ZEROPAGE_X;
	i=0xb6; mneAM[i].pt2MnemonicHandler = &mneLDX; mneAM[i].cycles = 4; mneAM[i].am = ZEROPAGE_Y;
	i=0xb7; mneAM[i].pt2MnemonicHandler = &mneLAX; mneAM[i].cycles = 4; mneAM[i].am = ZEROPAGE_Y;
	i=0xb8; mneAM[i].pt2MnemonicHandler = &mneCLV; mneAM[i].cycles = 2; mneAM[i].am = IMPLIED;
	i=0xb9; mneAM[i].pt2MnemonicHandler = &mneLDA; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE_Y;
	i=0xba; mneAM[i].pt2MnemonicHandler = &mneTSX; mneAM[i].cycles = 2; mneAM[i].am = IMPLIED;
	i=0xbb; mneAM[i].pt2MnemonicHandler = &mneLAS; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE_Y;
	i=0xbc; mneAM[i].pt2MnemonicHandler = &mneLDY; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE_X;
	i=0xbd; mneAM[i].pt2MnemonicHandler = &mneLDA; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE_X;
	i=0xbe; mneAM[i].pt2MnemonicHandler = &mneLDX; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE_Y;
	i=0xbf; mneAM[i].pt2MnemonicHandler = &mneLAX; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE_Y;
	i=0xc0; mneAM[i].pt2MnemonicHandler = &mneCPY; mneAM[i].cycles = 2; mneAM[i].am = IMMEDIATE;
	i=0xc1; mneAM[i].pt2MnemonicHandler = &mneCMP; mneAM[i].cycles = 6; mneAM[i].am = INDIRECT_X;
	i=0xc2; mneAM[i].pt2MnemonicHandler = &mneSKB; mneAM[i].cycles = 2; mneAM[i].am = IMMEDIATE;
	i=0xc3; mneAM[i].pt2MnemonicHandler = &mneDCP; mneAM[i].cycles = 8; mneAM[i].am = INDIRECT_X;
	i=0xc4; mneAM[i].pt2MnemonicHandler = &mneCPY; mneAM[i].cycles = 3; mneAM[i].am = ZEROPAGE;
	i=0xc5; mneAM[i].pt2MnemonicHandler = &mneCMP; mneAM[i].cycles = 3; mneAM[i].am = ZEROPAGE;
	i=0xc6; mneAM[i].pt2MnemonicHandler = &mneDEC; mneAM[i].cycles = 5; mneAM[i].am = ZEROPAGE;
	i=0xc7; mneAM[i].pt2MnemonicHandler = &mneDCP; mneAM[i].cycles = 5; mneAM[i].am = ZEROPAGE;
	i=0xc8; mneAM[i].pt2MnemonicHandler = &mneINY; mneAM[i].cycles = 2; mneAM[i].am = IMPLIED;
	i=0xc9; mneAM[i].pt2MnemonicHandler = &mneCMP; mneAM[i].cycles = 2; mneAM[i].am = IMMEDIATE;
	i=0xca; mneAM[i].pt2MnemonicHandler = &mneDEX; mneAM[i].cycles = 2; mneAM[i].am = IMPLIED;
	i=0xcb; mneAM[i].pt2MnemonicHandler = &mneSBX; mneAM[i].cycles = 2; mneAM[i].am = IMMEDIATE;
	i=0xcc; mneAM[i].pt2MnemonicHandler = &mneCPY; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE;
	i=0xcd; mneAM[i].pt2MnemonicHandler = &mneCMP; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE;
	i=0xce; mneAM[i].pt2MnemonicHandler = &mneDEC; mneAM[i].cycles = 6; mneAM[i].am = ABSOLUTE;
	i=0xcf; mneAM[i].pt2MnemonicHandler = &mneDCP; mneAM[i].cycles = 6; mneAM[i].am = ABSOLUTE;
	i=0xd0; mneAM[i].pt2MnemonicHandler = &mneBNE; mneAM[i].cycles = 2; mneAM[i].am = RELATIVE;
	i=0xd1; mneAM[i].pt2MnemonicHandler = &mneCMP; mneAM[i].cycles = 5; mneAM[i].am = INDIRECT_Y;
	i=0xd2; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0xd3; mneAM[i].pt2MnemonicHandler = &mneDCP; mneAM[i].cycles = 8; mneAM[i].am = INDIRECT_Y;
	i=0xd4; mneAM[i].pt2MnemonicHandler = &mneSKB; mneAM[i].cycles = 4; mneAM[i].am = ZEROPAGE_X;
	i=0xd5; mneAM[i].pt2MnemonicHandler = &mneCMP; mneAM[i].cycles = 4; mneAM[i].am = ZEROPAGE_X;
	i=0xd6; mneAM[i].pt2MnemonicHandler = &mneDEC; mneAM[i].cycles = 6; mneAM[i].am = ZEROPAGE_X;
	i=0xd7; mneAM[i].pt2MnemonicHandler = &mneDCP; mneAM[i].cycles = 6; mneAM[i].am = ZEROPAGE_X;
	i=0xd8; mneAM[i].pt2MnemonicHandler = &mneCLD; mneAM[i].cycles = 2; mneAM[i].am = IMPLIED;
	i=0xd9; mneAM[i].pt2MnemonicHandler = &mneCMP; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE_Y;
	i=0xda; mneAM[i].pt2MnemonicHandler = &mneNOP; mneAM[i].cycles = 2; mneAM[i].am = IMPLIED;
	i=0xdb; mneAM[i].pt2MnemonicHandler = &mneDCP; mneAM[i].cycles = 7; mneAM[i].am = ABSOLUTE_Y;
	i=0xdc; mneAM[i].pt2MnemonicHandler = &mneSKW; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE_X;
	i=0xdd; mneAM[i].pt2MnemonicHandler = &mneCMP; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE_X;
	i=0xde; mneAM[i].pt2MnemonicHandler = &mneDEC; mneAM[i].cycles = 7; mneAM[i].am = ABSOLUTE_X;
	i=0xdf; mneAM[i].pt2MnemonicHandler = &mneDCP; mneAM[i].cycles = 7; mneAM[i].am = ABSOLUTE_X;
	i=0xe0; mneAM[i].pt2MnemonicHandler = &mneCPX; mneAM[i].cycles = 2; mneAM[i].am = IMMEDIATE;
	i=0xe1; mneAM[i].pt2MnemonicHandler = &mneSBC; mneAM[i].cycles = 6; mneAM[i].am = INDIRECT_X;
	i=0xe2; mneAM[i].pt2MnemonicHandler = &mneSKB; mneAM[i].cycles = 2; mneAM[i].am = IMMEDIATE;
	i=0xe3; mneAM[i].pt2MnemonicHandler = &mneINS; mneAM[i].cycles = 8; mneAM[i].am = INDIRECT_X;
	i=0xe4; mneAM[i].pt2MnemonicHandler = &mneCPX; mneAM[i].cycles = 3; mneAM[i].am = ZEROPAGE;
	i=0xe5; mneAM[i].pt2MnemonicHandler = &mneSBC; mneAM[i].cycles = 3; mneAM[i].am = ZEROPAGE;
	i=0xe6; mneAM[i].pt2MnemonicHandler = &mneINC; mneAM[i].cycles = 5; mneAM[i].am = ZEROPAGE;
	i=0xe7; mneAM[i].pt2MnemonicHandler = &mneINS; mneAM[i].cycles = 5; mneAM[i].am = ZEROPAGE;
	i=0xe8; mneAM[i].pt2MnemonicHandler = &mneINX; mneAM[i].cycles = 2; mneAM[i].am = IMPLIED;
	i=0xe9; mneAM[i].pt2MnemonicHandler = &mneSBC; mneAM[i].cycles = 2; mneAM[i].am = IMMEDIATE;
	i=0xea; mneAM[i].pt2MnemonicHandler = &mneNOP; mneAM[i].cycles = 2; mneAM[i].am = IMPLIED;
	i=0xeb; mneAM[i].pt2MnemonicHandler = &mneSBC; mneAM[i].cycles = 2; mneAM[i].am = IMMEDIATE;  // undocumented op
	i=0xec; mneAM[i].pt2MnemonicHandler = &mneCPX; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE;
	i=0xed; mneAM[i].pt2MnemonicHandler = &mneSBC; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE;
	i=0xee; mneAM[i].pt2MnemonicHandler = &mneINC; mneAM[i].cycles = 6; mneAM[i].am = ABSOLUTE;
	i=0xef; mneAM[i].pt2MnemonicHandler = &mneINS; mneAM[i].cycles = 6; mneAM[i].am = ABSOLUTE;
	i=0xf0; mneAM[i].pt2MnemonicHandler = &mneBEQ; mneAM[i].cycles = 2; mneAM[i].am = RELATIVE;
	i=0xf1; mneAM[i].pt2MnemonicHandler = &mneSBC; mneAM[i].cycles = 5; mneAM[i].am = INDIRECT_Y;
	i=0xf2; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0xf3; mneAM[i].pt2MnemonicHandler = &mneINS; mneAM[i].cycles = 8; mneAM[i].am = INDIRECT_Y;
	i=0xf4; mneAM[i].pt2MnemonicHandler = &mneSKB; mneAM[i].cycles = 4; mneAM[i].am = ZEROPAGE_X;
	i=0xf5; mneAM[i].pt2MnemonicHandler = &mneSBC; mneAM[i].cycles = 4; mneAM[i].am = ZEROPAGE_X;
	i=0xf6; mneAM[i].pt2MnemonicHandler = &mneINC; mneAM[i].cycles = 6; mneAM[i].am = ZEROPAGE_X;
	i=0xf7; mneAM[i].pt2MnemonicHandler = &mneINS; mneAM[i].cycles = 6; mneAM[i].am = ZEROPAGE_X;
	i=0xf8; mneAM[i].pt2MnemonicHandler = &mneSED; mneAM[i].cycles = 2; mneAM[i].am = IMPLIED;
	i=0xf9; mneAM[i].pt2MnemonicHandler = &mneSBC; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE_Y;
	i=0xfa; mneAM[i].pt2MnemonicHandler = &mneNOP; mneAM[i].cycles = 2; mneAM[i].am = IMPLIED;
	i=0xfb; mneAM[i].pt2MnemonicHandler = &mneINS; mneAM[i].cycles = 7; mneAM[i].am = ABSOLUTE_Y;
	i=0xfc; mneAM[i].pt2MnemonicHandler = &mneSKW; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE_X;
	i=0xfd; mneAM[i].pt2MnemonicHandler = &mneSBC; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE_X;
	i=0xfe; mneAM[i].pt2MnemonicHandler = &mneINC; mneAM[i].cycles = 7; mneAM[i].am = ABSOLUTE_X;
	i=0xff; mneAM[i].pt2MnemonicHandler = &mneINS; mneAM[i].cycles = 7; mneAM[i].am = ABSOLUTE_X;

}

void mos_irq() {
	// irq must occur before the last cycle of the ongoing instruction for it to be processed before next instruction
	irqLineUp=1;
	// TODO: investigate and confirm how this interrupt delaying actually should go
	if(cyc<=2) {
		interruptDelayed=2;
	}
}

void mos_nmi() {
	// TODO: remember NMI writes 0 break flag to stack
	printf("NMI");
	nmiLineUp=1;
	if(cyc<=2) {
		interruptDelayed=2;
	}
}

void initRegisters() {
	A = 0x0;
	X = 0x0;
	Y = 0x0;
	P = 0b00100000;
	PC = memReadWord(0xfffc); //load the address from FFFC (kernal rom)
	S = 0x00;
}

void reset() {
	cyc=0;
	initRam();
	initRegisters();
}


void printProcessorStatus() {
	printf1("PC=%x",PC);
	printf1("A=%x",A);
	printf2("X=%x Y=%x",X,Y);
	printf2("P=%x S=%x",P,S);
}

void mos6510_cycle() {
	if(cyc--<=0) {

		// interrupts are handled
		if(irqLineUp && !interruptDelayed) {
			if(!(P & PFLAG_IRQ)) {
				stackPush(PC >> 8);
				stackPush(PC);
				stackPush(P&~PFLAG_BREAK);
				PC = memReadWord(0xfffe);
				// interrupt sequence takes 7 clock cycles
				cyc = 7;
				irqLineUp=0;
				return;
				//TODO: IRQ and BRK both set the I flag, whereas the NMI does not affect its state.
			}
			irqLineUp=0;
		}
		if(nmiLineUp && !interruptDelayed) {
			stackPush(PC >> 8);
			stackPush(PC);
			stackPush(P&~PFLAG_BREAK);
			PC = memReadWord(0xfffa);
			// interrupt sequence takes 7 clock cycles
			cyc = 7;
			//TODO: IRQ and BRK both set the I flag, whereas the NMI does not affect its state.
			nmiLineUp=0;
		}
	
		////// breakpoint debugging  //////
		
		//if(PC==0xa57c || yesDump>0) {
			//if(yesDump==0) {
				//printf3("$200-> %x %x %x", memReadByte(0x200),memReadByte(0x201),memReadByte(0x202) );
				//printf3("$203-> %x %x %x", memReadByte(0x203),memReadByte(0x204),memReadByte(0x205) );
				//printProcessorStatus();
				//exit(1);
			//}
		//	yesDump++;
		//	printf1("Y=%x",Y);
			/*if(yesDump==0x9af) {
				printf3("$62-> %x %x %x", memReadByte(0x62),memReadByte(0x63),memReadByte(0x64) );
				printProcessorStatus();
				//exit(1);
			}*/
			//if(yesDump>100) yesDump=1;
		//}
		/*
		if(PC==0x897 && (A&PFLAG_DECIMAL)) {
			printf("push PwD");
			printProcessorStatus();
			exit(1);
		}

		if(P&PFLAG_DECIMAL) {
			printf1("D-PC=%x",PC);
			printProcessorStatus();
			exit(1);
		}		
		*/
		/*
		if(A==0 && X==0xb1 && Y==0x6c && S==0xf6) {
			printProcessorStatus();
			exit(1);
		}*/

		byte opCode = readMemoryPC();
		mneAM[opCode].pt2MnemonicHandler(mneAM[opCode].am, mneAM[opCode].cycles);
		cyc--;
	}
	if(interruptDelayed) interruptDelayed--;
}


void mos6510_init() {
	yesDump=0; // just a debugging counter
	cyc=0;
	irqLineUp=0;
	nmiLineUp=0;
	//printf("MOS6510 init");
	initMnemonicArray();
	//loadROM();
	reset();
}


