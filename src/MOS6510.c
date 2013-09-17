/*
 ============================================================================
 Name        : MOS6510.c
 Author      : Sampo Peltonen
 Version     :
 Copyright   : Your copyright notice
 Description : MOS6510 simulation
 ============================================================================
 */

#include "MOS6510.h"
#include "stdlibtools.h"

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
			printf("ERROR: Accumulator address mode should be handled differently.\n");
			exit(1);
		case IMMEDIATE:
			*targetAddress = PC++; // inc counter manually
			break;
		case ZEROPAGE:
			oper1 = readMemoryPC();
			*targetAddress = oper1;
			break;
		case ZEROPAGE_X:
			oper1 = readMemoryPC();
			*targetAddress = oper1+X;
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
			*targetAddress =  memReadWord((oper2 << 8) + oper1);
			#if DEBUG_6510
				printf("INDIRECTION: operand address=%X\n",(oper2 << 8) + oper1);
				printf("INDIRECTION: targetAddress=%X\n",*targetAddress);
			#endif
			break;
		case INDIRECT_X:
			oper1 = readMemoryPC();
			tmpb = X + oper1;
			*targetAddress = memReadWord(tmpb);
			break;
		case INDIRECT_Y:
			oper1 = readMemoryPC();
			tmpw = memReadByte(oper1) + Y;
			if(tmpw>0xff) pageCrossed = 1; //if page crossed
			tmpw += memReadByte(oper1+1) << 8;
			*targetAddress = tmpw;
			break;
		default:
			printf1("Addressing %d mode not implemented.\n", am);
			exit(1);
	}
	return(pageCrossed);
}









void mneILL(int am, int cycles) {
	printf1("Illegal opcode %x.\n", memReadByte(PC-1));
	exit(0);
}

void mneADC(int am, int cycles) {
	#if DEBUG_6510
	printf("ADC ");
    #endif
	word target;
	cyc = cycles + resolveAddressModeTarget(am, &target);
	word t = A + memReadByte(target) + ((P & PFLAG_CARRY) << 8);

	#if DEBUG_6510
	printf1("t=A+M+C=%x, ", t);
	#endif

	if((B8 & A)!=(B8 & t)) setPFlag(PFLAG_OVERFLOW);
	else clearPFlag(PFLAG_OVERFLOW);

	if(B8 == (B8 & A)) setPFlag(PFLAG_NEGATIVE);
	else clearPFlag(PFLAG_NEGATIVE);

	if(t==0) setPFlag(PFLAG_ZERO);
	else clearPFlag(PFLAG_ZERO);

	if((P & PFLAG_DECIMAL)==PFLAG_DECIMAL) {
		printf(" decimal mode not implemented.");
		exit(1);
	}
	else {
		if(t>0xff) {
			setPFlag(PFLAG_CARRY);
			#if DEBUG_6510
			printf("C=1, ");
			#endif
		}
		else {
			clearPFlag(PFLAG_CARRY);
			#if DEBUG_6510
			printf("C=0, ");
			#endif
		}
	}
	A = t & 0xff;
	#if DEBUG_6510
	printf("A=%X ", A);
	#endif

}

void mneAND(int am, int cycles) {
	#if DEBUG_6510
	printf("AND ");
	#endif
	word target;
	cyc = cycles + resolveAddressModeTarget(am, &target);
	A = A & memReadByte(target);
	setZeroAndNegativePFlags(&A);
}

void mneASL(int am, int cycles) {
	#if DEBUG_6510
	printf("ASL ");
	#endif
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
	#if DEBUG_6510
	printf("BCC ");
	#endif
	cyc = cycles;
	sbyte offset = readMemoryPC();
	if((PFLAG_CARRY & P)==0) {
		#if DEBUG_6510
		printf1("jumping with decimal offset %d, ",offset);
		#endif
		cyc++;
		word newLoc = PC + offset;
		if((newLoc >> 8) != (PC >> 8)) cyc++; // is page crossed?
		PC = newLoc;
		#if DEBUG_6510
		printf1("PC new loc=%X",PC);
		#endif
	}
}

void mneBCS(int am, int cycles) {
	#if DEBUG_6510
	printf("BCS ");
	#endif
	cyc = cycles;
	sbyte offset = (sbyte) readMemoryPC();
	if((P & PFLAG_CARRY)== PFLAG_CARRY) {
		cyc++;
		word newLoc = PC + offset;
		if((newLoc >> 8) != (PC >> 8)) cyc++; // is page crossed?
		PC = newLoc;
		#if DEBUG_6510
		printf("jumped to %X (offset %d) ",PC,offset);
		#endif
	}
}

void mneBEQ(int am, int cycles) {
	#if DEBUG_6510
	printf("BEQ ");
	#endif
	cyc = cycles;
	sbyte offset = (sbyte) readMemoryPC();
	if((P & PFLAG_ZERO)== PFLAG_ZERO) {
		cyc++;
		word newLoc = PC + offset;
		if((newLoc >> 8) != (PC >> 8)) cyc++; // is page crossed?
		PC = newLoc;
		#if DEBUG_6510
		printf("jumped to %X (offset %d) ",PC,offset);
		#endif
	}
}

void mneBIT(int am, int cycles) {
	#if DEBUG_6510
	printf("BIT ");
	#endif
	cyc = cycles;
	word target;
	resolveAddressModeTarget(am, &target);
	byte t = A & memReadByte(target);
	(t==0) ? setPFlag(PFLAG_ZERO) : clearPFlag(PFLAG_ZERO);
	((t & B7)==B7) ? setPFlag(PFLAG_OVERFLOW) : clearPFlag(PFLAG_OVERFLOW);
	((t & B8)==B8) ? setPFlag(PFLAG_NEGATIVE) : clearPFlag(PFLAG_NEGATIVE);
	//setZeroAndNegativePFlags(&t);  this is bullshit
}

void mneBMI(int am, int cycles) {
	#if DEBUG_6510
	printf("BMI ");
	#endif
	cyc = cycles;
	sbyte offset = (sbyte) readMemoryPC();
	if((P & PFLAG_NEGATIVE)== PFLAG_NEGATIVE) {
		cyc++;
		word newLoc = PC + offset;
		if((newLoc >> 8) != (PC >> 8)) cyc++; // is page crossed?
		PC = newLoc;
		#if DEBUG_6510
		printf("jumped to %X (offset %d) ",PC,offset);
		#endif
	}
}

void mneBNE(int am, int cycles) {
	#if DEBUG_6510
	printf("BNE ");
	#endif
	//if(am!=RELATIVE) { printf("BNE am %d illegal.\n",am); exit(1); }
	cyc = cycles;
	sbyte offset = readMemoryPC();
	if((PFLAG_ZERO & P)==0) {
		#if DEBUG_6510
		printf("P.Z==0: jumping with offset %d, ",offset);
		#endif
		cyc++;
		word newLoc = PC + offset;
		if((newLoc >> 8) != (PC >> 8)) cyc++; // is page crossed?
		PC = newLoc;
		#if DEBUG_6510
		printf("PC new loc=%X",PC);
		#endif
	}
	#if DEBUG_6510
	else {
		printf("P.Z==1. NO jump to loc %X ", (PC+offset));
	}
	#endif
}

void mneBPL(int am, int cycles) {
	#if DEBUG_6510
	printf("BPL ");
	#endif
	cyc = cycles;
	sbyte offset = readMemoryPC();
	if((PFLAG_NEGATIVE & P)==0) {
		#if DEBUG_6510
		printf("jumping with offset %d, ",offset);
		#endif
		cyc++;
		word newLoc = PC + offset;
		if((newLoc >> 8) != (PC >> 8)) cyc++; // is page crossed?
		PC = newLoc;
		#if DEBUG_6510
		printf("PC new loc=%X",PC);
		#endif
	}
}

void mneBRK(int am, int cycles) {
	printf1("BRK %x not implemented.\n",am); 
	printf1("PC=%x",PC);
	exit(1);
}

void mneBVC(int am, int cycles) {
	// the BVC instruction will take 3 cycles no matter what address it is located at.
	// tarkista toi, ei taida pitää paikkaansa
  printf("BVC not implemented.\n"); exit(1);
}

void mneBVS(int am, int cycles) {
  printf("BVS not implemented.\n"); exit(1);
}

void mneCLC(int am, int cycles) {
	#if DEBUG_6510
	printf("CLC ");
	#endif
	cyc = cycles;
	clearPFlag(PFLAG_CARRY);
}

void mneCLD(int am, int cycles) {
	#if DEBUG_6510
	printf("CLD ");
	#endif
	cyc = cycles;
	if(am==IMPLIED)
		clearPFlag(PFLAG_DECIMAL);
	else {
		printf1("CLD addressing mode not implemented: %d\n", am);
		exit(1);
	}
}

void mneCLI(int am, int cycles) {
	#if DEBUG_6510
	printf("CLI ");
	#endif
	cyc = cycles;
	clearPFlag(PFLAG_IRQ);
}

void mneCLV(int am, int cycles) {
  printf("CLV not implemented.\n"); exit(1);
}

void mneCMP(int am, int cycles) {
	#if DEBUG_6510
	printf("CMP ");
	#endif
	word target;
	cyc = cycles + resolveAddressModeTarget(am, &target);
	byte value = memReadByte(target);
	byte tmpb = A - value;
	setZeroAndNegativePFlags(&tmpb);
	if(A>=value) setPFlag(PFLAG_CARRY);
	else clearPFlag(PFLAG_CARRY);
}

void mneCPX(int am, int cycles) {
	#if DEBUG_6510
	printf1("CPX am=%d, ",am);
	#endif
	cyc = cycles;
	word target;
	resolveAddressModeTarget(am, &target);
	#if DEBUG_6510
	printf("vrt X-M %X-%X ",X, memReadByte(target));
	#endif
	byte tmp = X - memReadByte(target);
	setZeroAndNegativePFlags(&tmp);
	if((tmp & B8)==B8) setPFlag(PFLAG_CARRY);
	else clearPFlag(PFLAG_CARRY);
}

void mneCPY(int am, int cycles) {
	#if DEBUG_6510
	printf("CPY am=%d, ",am);
	#endif
	cyc = cycles;
	word target;
	resolveAddressModeTarget(am, &target);
	#if DEBUG_6510
	printf("vrt Y-M %X-%X ",Y, memReadByte(target));
	#endif
	byte tmp = Y - memReadByte(target);
	setZeroAndNegativePFlags(&tmp);
	if((tmp & B8)==B8) setPFlag(PFLAG_CARRY);
	else clearPFlag(PFLAG_CARRY);
}

void mneDEC(int am, int cycles) {
	#if DEBUG_6510
	printf("DEC ");
	#endif
	word target;
	cyc = cycles;
	resolveAddressModeTarget(am, &target);
	byte tmp = memReadByte(target);
	tmp--;
	setZeroAndNegativePFlags(&tmp);
	memWriteByte(target, tmp);
}

void mneDEX(int am, int cycles) {
	#if DEBUG_6510
	printf("DEX ");
	#endif
	cyc = cycles;
	X--;
	setZeroAndNegativePFlags(&X);
}

void mneDEY(int am, int cycles) {
	#if DEBUG_6510
	printf("DEY ");
	#endif
	cyc = cycles;
	Y--;
	setZeroAndNegativePFlags(&Y);
}

void mneEOR(int am, int cycles) {
	#if DEBUG_6510
	printf("EOR ");
	#endif
	word target;
	cyc = cycles + resolveAddressModeTarget(am, &target);
	A = A ^ memReadByte(target);
	setZeroAndNegativePFlags(&A);
}

void mneINC(int am, int cycles) {
	#if DEBUG_6510
	printf("INC ");
	#endif
	cyc = cycles;
	word target;
	resolveAddressModeTarget(am, &target);
	byte tmpb = memReadByte(target)+1;
	memWriteByte(target, tmpb);
	setZeroAndNegativePFlags(&tmpb);
}

void mneINX(int am, int cycles) {
	#if DEBUG_6510
	printf("INX ");
	#endif
	cyc = cycles;
	X++;
	setZeroAndNegativePFlags(&X);
}

void mneINY(int am, int cycles) {
	#if DEBUG_6510
	printf("INY ");
	#endif
	cyc = cycles;
	Y++;
	setZeroAndNegativePFlags(&Y);
}

void mneJMP(int am, int cycles) {
	#if DEBUG_6510
	printf("JMP ");
	#endif
	cyc = cycles;
	word target;
	resolveAddressModeTarget(am, &target);
	// jmp xxFF bug not implemented
	PC = target;
	#if DEBUG_6510
	printf("  to %X ",PC);
	#endif
}

void mneJSR(int am, int cycles) {
	#if DEBUG_6510
	printf("JSR ");
	#endif
	cyc = cycles;
	word tmpW;
	if(am==ABSOLUTE) {
		tmpW = readMemoryPC() + (readMemoryPC() << 8);
		stackPush((PC-1) >> 8);
		stackPush(PC-1);
		PC = tmpW;
		#if DEBUG_6510
		printf("jumped to %X ",PC);
		#endif
	}
	else {
		printf1("JSR addressing mode not implemented: %d\n", am); exit(1);
	}
}

// http://www.obelisk.demon.co.uk/6502/reference.html#LDA
void mneLDA(int am, int cycles) {
	#if DEBUG_6510
	printf("LDA ");
	#endif
	word target;
	cyc = cycles + resolveAddressModeTarget(am, &target);
	A = memReadByte(target);
	#if DEBUG_6510
	printf("A=%X ", A);
	#endif
	setZeroAndNegativePFlags(&A);
}

void mneLDX(int am, int cycles) {
	#if DEBUG_6510
	printf("LDX ");
	#endif
	word target;
	cyc = cycles + resolveAddressModeTarget(am, &target);
	X = memReadByte(target);
	setZeroAndNegativePFlags(&X);
}

void mneLDY(int am, int cycles) {
	#if DEBUG_6510
	printf("LDY ");
	#endif
	word target;
	cyc = cycles + resolveAddressModeTarget(am, &target);
	Y = memReadByte(target);
	setZeroAndNegativePFlags(&Y);
}

void mneLSR(int am, int cycles) {
	#if DEBUG_6510
	printf("LSR ");
	#endif
	cyc = cycles;
	word target;
	resolveAddressModeTarget(am, &target);
	byte tmp = memReadByte(target);
	clearPFlag(PFLAG_NEGATIVE);
	(tmp & B1) ? setPFlag(PFLAG_CARRY) : clearPFlag(PFLAG_CARRY);
	tmp = (tmp >> 1) & 0x7f;
	(tmp==0) ? setPFlag(PFLAG_ZERO) : clearPFlag(PFLAG_ZERO);
}

void mneNOP(int am, int cycles) {
  printf("NOP not implemented.\n"); exit(1);
}

void mneORA(int am, int cycles) {
	#if DEBUG_6510
	printf("ORA ");
	#endif
	word target;
	cyc = cycles + resolveAddressModeTarget(am, &target);
	A = A | memReadByte(target);
	setZeroAndNegativePFlags(&A);
}

void mnePHA(int am, int cycles) {
	#if DEBUG_6510
	printf("PHA ");
	#endif
	cyc = cycles;
	stackPush(A);
}

void mnePHP(int am, int cycles) {
	#if DEBUG_6510
	printf("PHP ");
	#endif
	cyc = cycles;
	stackPush(P);
}

void mnePLA(int am, int cycles) {
	#if DEBUG_6510
	printf("PLA ");
	#endif
	cyc = cycles;
	A = stackPull();
	setZeroAndNegativePFlags(&A);
}

void mnePLP(int am, int cycles) {
	#if DEBUG_6510
	printf("PLP ");
	#endif
	cyc = cycles;
	P = stackPull();
}

void mneROL(int am, int cycles) {
	#if DEBUG_6510
	printf("ROL ");
	#endif
	cyc = cycles;
	word target;
	byte tmpb;
	int tmpbit = 0;

	if(am==ACCUMULATOR) {
		if((A & B8)==B8) tmpbit = 1;
		A = (A << 1) & 0xfe;
		if((P & PFLAG_CARRY)==PFLAG_CARRY) A = (A | B1);
	}
	else {
		resolveAddressModeTarget(am, &target);
		tmpb = memReadByte(target);
		if((tmpb & B8)==B8) tmpbit = 1;
		tmpb = (tmpb << 1) & 0xfe;
		if((P & PFLAG_CARRY)==PFLAG_CARRY) tmpb = (tmpb | B1);
		memWriteByte(target, tmpb);
	}

	if(tmpbit) setPFlag(PFLAG_CARRY);
	else clearPFlag(PFLAG_CARRY);
}

void mneROR(int am, int cycles) {
	#if DEBUG_6510
	printf("ROR ");
	#endif
	cyc = cycles;
	word target;
	byte tmpb;
	int tmpbit = 0;
	if(am==ACCUMULATOR) {
		/*
		Näyttää bugille tässä, korjaa ja testaa
		*/
		if((A & B8)==B8) tmpbit = 1;
		A = (A >> 1) & 0x7f;
		A = A | ((P & PFLAG_CARRY) ? 0x80 : 0x00);
		tmpbit ? setPFlag(PFLAG_CARRY) : clearPFlag(PFLAG_CARRY);
		setZeroAndNegativePFlags(&A);
	}
	else {
		resolveAddressModeTarget(am, &target);
		tmpb = memReadByte(target);
		if((tmpb & B8)==B8) tmpbit = 1;
		tmpb = (tmpb >> 1) & 0x7f;
		tmpb = tmpb | ((P & PFLAG_CARRY) ? 0x80 : 0x00);
		tmpbit ? setPFlag(PFLAG_CARRY) : clearPFlag(PFLAG_CARRY);
		setZeroAndNegativePFlags(&tmpb);
		memWriteByte(target, tmpb);
	}
}

void mneRTI(int am, int cycles) {
  printf("RTI not implemented.\n"); exit(1);
}

void mneRTS(int am, int cycles) {
	#if DEBUG_6510
	printf("RTS ");
	#endif
	cyc = cycles;
	PC = stackPull() + (stackPull() << 8) + 1;
	#if DEBUG_6510
	printf("PC new loc=%X ",PC);
	#endif
}

void mneSBC(int am, int cycles) {
	#if DEBUG_6510
	printf("SBC ");
	#endif
	word target;
	cyc=cycles + resolveAddressModeTarget(am, &target);
	sword tmp;
	if((P & PFLAG_DECIMAL) == PFLAG_DECIMAL) {
		printf("decimal mode not implemented.");
		tmp = 0; // silence compiler warning
		exit(1);
	}
	else {
		#if DEBUG_6510
		printf("value %x ", memReadByte(target));
		#endif
		tmp = A - memReadByte(target) - ((P & PFLAG_CARRY) ? 0 : 1);
		((tmp > 0x7f) || (tmp < -0x80)) ? setPFlag(PFLAG_OVERFLOW) : clearPFlag(PFLAG_OVERFLOW);
	}
	(tmp>=0) ? setPFlag(PFLAG_CARRY) : clearPFlag(PFLAG_CARRY);
	(tmp & B8) ? setPFlag(PFLAG_NEGATIVE) : clearPFlag(PFLAG_NEGATIVE);
	(tmp==0) ? setPFlag(PFLAG_ZERO) : clearPFlag(PFLAG_ZERO);
}

void mneSEC(int am, int cycles) {
	#if DEBUG_6510
	printf("SEC ");
	#endif
	cyc = cycles;
	setPFlag(PFLAG_CARRY);
}

void mneSED(int am, int cycles) {
  printf("SED not implemented.\n"); exit(1);
}

void mneSEI(int am, int cycles) {
	#if DEBUG_6510
	printf("SEI ");
	#endif
	cyc = cycles;

	if(am==IMPLIED) {
		setPFlag(PFLAG_IRQ);
	}
	else {
		printf1("SEI addressing mode not implemented: %d\n", am);
		exit(1);
	}
}

void mneSTA(int am, int cycles) {
	#if DEBUG_6510
	printf("STA ");
	#endif
	word target;
	cyc = cycles;
	resolveAddressModeTarget(am, &target);
	memWriteByte(target, A);
}

void mneSTX(int am, int cycles) {
	#if DEBUG_6510
	printf("STX ");
	#endif
	cyc = cycles;
	word target;
	resolveAddressModeTarget(am, &target);
	memWriteByte(target, X);
}

void mneSTY(int am, int cycles) {
	#if DEBUG_6510
	printf("STY ");
	#endif
	cyc = cycles;
	word target;
	resolveAddressModeTarget(am, &target);
	memWriteByte(target, Y);
}

void mneTAX(int am, int cycles) {
	#if DEBUG_6510
	printf("TAX ");
	#endif
	cyc = cycles;
	X = A;
	setZeroAndNegativePFlags(&X);
}

void mneTAY(int am, int cycles) {
	#if DEBUG_6510
	printf("TAY ");
	#endif
	cyc = cycles;
	Y = A;
	setZeroAndNegativePFlags(&Y);
}

void mneTSX(int am, int cycles) {
  	printf("TSX not implemented.\n");
	exit(1);
}

void mneTXA(int am, int cycles) {
	#if DEBUG_6510
	printf("TXA ");
	#endif
	cyc = cycles;
	A = X;
	setZeroAndNegativePFlags(&A);
}

void mneTXS(int am, int cycles) {
	#if DEBUG_6510
	printf("TXS ");
	#endif
	cyc = cycles;
	if(am==IMPLIED) {
		S=X;
	}
	else {
		printf1("TXS addressing mode not implemented: %d\n", am);
		exit(1);
	}
}

void mneTYA(int am, int cycles) {
	#if DEBUG_6510
	printf("TYA ");
	#endif
	cyc = cycles;
	A = Y;
	setZeroAndNegativePFlags(&A);
}





void initMnemonicArray() {
	byte i;
	i=0x00; mneAM[i].pt2MnemonicHandler = &mneBRK; mneAM[i].cycles = 7; mneAM[i].am = IMPLIED;
	i=0x01; mneAM[i].pt2MnemonicHandler = &mneORA; mneAM[i].cycles = 6; mneAM[i].am = INDIRECT_X;
	i=0x02; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x03; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x04; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x05; mneAM[i].pt2MnemonicHandler = &mneORA; mneAM[i].cycles = 2; mneAM[i].am = ZEROPAGE;
	i=0x06; mneAM[i].pt2MnemonicHandler = &mneASL; mneAM[i].cycles = 5; mneAM[i].am = ZEROPAGE;
	i=0x07; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x08; mneAM[i].pt2MnemonicHandler = &mnePHP; mneAM[i].cycles = 3; mneAM[i].am = IMPLIED;
	i=0x09; mneAM[i].pt2MnemonicHandler = &mneORA; mneAM[i].cycles = 2; mneAM[i].am = IMMEDIATE;
	i=0x0a; mneAM[i].pt2MnemonicHandler = &mneASL; mneAM[i].cycles = 2; mneAM[i].am = ACCUMULATOR;
	i=0x0b; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x0c; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x0d; mneAM[i].pt2MnemonicHandler = &mneORA; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE;
	i=0x0e; mneAM[i].pt2MnemonicHandler = &mneASL; mneAM[i].cycles = 6; mneAM[i].am = ABSOLUTE;
	i=0x0f; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x10; mneAM[i].pt2MnemonicHandler = &mneBPL; mneAM[i].cycles = 2; mneAM[i].am = RELATIVE;
	i=0x11; mneAM[i].pt2MnemonicHandler = &mneORA; mneAM[i].cycles = 5; mneAM[i].am = INDIRECT_Y;
	i=0x12; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x13; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x14; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x15; mneAM[i].pt2MnemonicHandler = &mneORA; mneAM[i].cycles = 3; mneAM[i].am = ZEROPAGE_X;
	i=0x16; mneAM[i].pt2MnemonicHandler = &mneASL; mneAM[i].cycles = 6; mneAM[i].am = ZEROPAGE_X;
	i=0x17; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x18; mneAM[i].pt2MnemonicHandler = &mneCLC; mneAM[i].cycles = 2; mneAM[i].am = IMPLIED;
	i=0x19; mneAM[i].pt2MnemonicHandler = &mneORA; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE_Y;
	i=0x1a; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x1b; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x1c; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x1d; mneAM[i].pt2MnemonicHandler = &mneORA; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE_X;
	i=0x1e; mneAM[i].pt2MnemonicHandler = &mneASL; mneAM[i].cycles = 7; mneAM[i].am = ABSOLUTE_X;
	i=0x1f; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x20; mneAM[i].pt2MnemonicHandler = &mneJSR; mneAM[i].cycles = 6; mneAM[i].am = ABSOLUTE;
	i=0x21; mneAM[i].pt2MnemonicHandler = &mneAND; mneAM[i].cycles = 6; mneAM[i].am = INDIRECT_X;
	i=0x22; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x23; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x24; mneAM[i].pt2MnemonicHandler = &mneBIT; mneAM[i].cycles = 3; mneAM[i].am = ZEROPAGE;
	i=0x25; mneAM[i].pt2MnemonicHandler = &mneAND; mneAM[i].cycles = 2; mneAM[i].am = ZEROPAGE;
	i=0x26; mneAM[i].pt2MnemonicHandler = &mneROL; mneAM[i].cycles = 5; mneAM[i].am = ZEROPAGE;
	i=0x27; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x28; mneAM[i].pt2MnemonicHandler = &mnePLP; mneAM[i].cycles = 4; mneAM[i].am = IMPLIED;
	i=0x29; mneAM[i].pt2MnemonicHandler = &mneAND; mneAM[i].cycles = 2; mneAM[i].am = IMMEDIATE;
	i=0x2a; mneAM[i].pt2MnemonicHandler = &mneROL; mneAM[i].cycles = 2; mneAM[i].am = ACCUMULATOR;
	i=0x2b; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x2c; mneAM[i].pt2MnemonicHandler = &mneBIT; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE;
	i=0x2d; mneAM[i].pt2MnemonicHandler = &mneAND; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE;
	i=0x2e; mneAM[i].pt2MnemonicHandler = &mneROL; mneAM[i].cycles = 6; mneAM[i].am = ABSOLUTE;
	i=0x2f; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x30; mneAM[i].pt2MnemonicHandler = &mneBMI; mneAM[i].cycles = 2; mneAM[i].am = RELATIVE;
	i=0x31; mneAM[i].pt2MnemonicHandler = &mneAND; mneAM[i].cycles = 5; mneAM[i].am = INDIRECT_Y;
	i=0x32; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x33; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x34; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x35; mneAM[i].pt2MnemonicHandler = &mneAND; mneAM[i].cycles = 3; mneAM[i].am = ZEROPAGE_X;
	i=0x36; mneAM[i].pt2MnemonicHandler = &mneROL; mneAM[i].cycles = 6; mneAM[i].am = ZEROPAGE_X;
	i=0x37; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x38; mneAM[i].pt2MnemonicHandler = &mneSEC; mneAM[i].cycles = 2; mneAM[i].am = IMPLIED;
	i=0x39; mneAM[i].pt2MnemonicHandler = &mneAND; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE_Y;
	i=0x3a; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x3b; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x3c; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x3d; mneAM[i].pt2MnemonicHandler = &mneAND; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE_X;
	i=0x3e; mneAM[i].pt2MnemonicHandler = &mneROL; mneAM[i].cycles = 7; mneAM[i].am = ABSOLUTE_X;
	i=0x3f; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x40; mneAM[i].pt2MnemonicHandler = &mneRTI; mneAM[i].cycles = 6; mneAM[i].am = IMPLIED;
	i=0x41; mneAM[i].pt2MnemonicHandler = &mneEOR; mneAM[i].cycles = 6; mneAM[i].am = INDIRECT_X;
	i=0x42; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x43; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x44; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x45; mneAM[i].pt2MnemonicHandler = &mneEOR; mneAM[i].cycles = 3; mneAM[i].am = ZEROPAGE;
	i=0x46; mneAM[i].pt2MnemonicHandler = &mneLSR; mneAM[i].cycles = 5; mneAM[i].am = ZEROPAGE;
	i=0x47; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x48; mneAM[i].pt2MnemonicHandler = &mnePHA; mneAM[i].cycles = 3; mneAM[i].am = IMPLIED;
	i=0x49; mneAM[i].pt2MnemonicHandler = &mneEOR; mneAM[i].cycles = 2; mneAM[i].am = IMMEDIATE;
	i=0x4a; mneAM[i].pt2MnemonicHandler = &mneLSR; mneAM[i].cycles = 2; mneAM[i].am = ACCUMULATOR;
	i=0x4b; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x4c; mneAM[i].pt2MnemonicHandler = &mneJMP; mneAM[i].cycles = 3; mneAM[i].am = ABSOLUTE;
	i=0x4d; mneAM[i].pt2MnemonicHandler = &mneEOR; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE;
	i=0x4e; mneAM[i].pt2MnemonicHandler = &mneLSR; mneAM[i].cycles = 6; mneAM[i].am = ABSOLUTE;
	i=0x4f; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x50; mneAM[i].pt2MnemonicHandler = &mneBVC; mneAM[i].cycles = 2; mneAM[i].am = RELATIVE;
	i=0x51; mneAM[i].pt2MnemonicHandler = &mneEOR; mneAM[i].cycles = 5; mneAM[i].am = INDIRECT_Y;
	i=0x52; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x53; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x54; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x55; mneAM[i].pt2MnemonicHandler = &mneEOR; mneAM[i].cycles = 4; mneAM[i].am = ZEROPAGE_X;
	i=0x56; mneAM[i].pt2MnemonicHandler = &mneLSR; mneAM[i].cycles = 6; mneAM[i].am = ZEROPAGE_X;
	i=0x57; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x58; mneAM[i].pt2MnemonicHandler = &mneCLI; mneAM[i].cycles = 2; mneAM[i].am = IMPLIED;
	i=0x59; mneAM[i].pt2MnemonicHandler = &mneEOR; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE_Y;
	i=0x5a; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x5b; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x5c; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x5d; mneAM[i].pt2MnemonicHandler = &mneEOR; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE_X;
	i=0x5e; mneAM[i].pt2MnemonicHandler = &mneLSR; mneAM[i].cycles = 7; mneAM[i].am = ABSOLUTE_X;
	i=0x5f; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x60; mneAM[i].pt2MnemonicHandler = &mneRTS; mneAM[i].cycles = 6; mneAM[i].am = IMPLIED;
	i=0x61; mneAM[i].pt2MnemonicHandler = &mneADC; mneAM[i].cycles = 6; mneAM[i].am = INDIRECT_X;
	i=0x62; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x63; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x64; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x65; mneAM[i].pt2MnemonicHandler = &mneADC; mneAM[i].cycles = 3; mneAM[i].am = ZEROPAGE;
	i=0x66; mneAM[i].pt2MnemonicHandler = &mneROR; mneAM[i].cycles = 5; mneAM[i].am = ZEROPAGE;
	i=0x67; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x68; mneAM[i].pt2MnemonicHandler = &mnePLA; mneAM[i].cycles = 4; mneAM[i].am = IMPLIED;
	i=0x69; mneAM[i].pt2MnemonicHandler = &mneADC; mneAM[i].cycles = 2; mneAM[i].am = IMMEDIATE;
	i=0x6a; mneAM[i].pt2MnemonicHandler = &mneROR; mneAM[i].cycles = 2; mneAM[i].am = ACCUMULATOR;
	i=0x6b; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x6c; mneAM[i].pt2MnemonicHandler = &mneJMP; mneAM[i].cycles = 5; mneAM[i].am = INDIRECT;
	i=0x6d; mneAM[i].pt2MnemonicHandler = &mneADC; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE;
	i=0x6e; mneAM[i].pt2MnemonicHandler = &mneROR; mneAM[i].cycles = 6; mneAM[i].am = ABSOLUTE;
	i=0x6f; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x70; mneAM[i].pt2MnemonicHandler = &mneBVS; mneAM[i].cycles = 2; mneAM[i].am = RELATIVE;
	i=0x71; mneAM[i].pt2MnemonicHandler = &mneADC; mneAM[i].cycles = 5; mneAM[i].am = INDIRECT_Y;
	i=0x72; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x73; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x74; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x75; mneAM[i].pt2MnemonicHandler = &mneADC; mneAM[i].cycles = 4; mneAM[i].am = ZEROPAGE_X;
	i=0x76; mneAM[i].pt2MnemonicHandler = &mneROR; mneAM[i].cycles = 6; mneAM[i].am = ZEROPAGE_X;
	i=0x77; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x78; mneAM[i].pt2MnemonicHandler = &mneSEI; mneAM[i].cycles = 2; mneAM[i].am = IMPLIED;
	i=0x79; mneAM[i].pt2MnemonicHandler = &mneADC; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE_Y;
	i=0x7a; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x7b; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x7c; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x7d; mneAM[i].pt2MnemonicHandler = &mneADC; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE_X;
	i=0x7e; mneAM[i].pt2MnemonicHandler = &mneROR; mneAM[i].cycles = 7; mneAM[i].am = ABSOLUTE_X;
	i=0x7f; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x80; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x81; mneAM[i].pt2MnemonicHandler = &mneSTA; mneAM[i].cycles = 6; mneAM[i].am = INDIRECT_X;
	i=0x82; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x83; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x84; mneAM[i].pt2MnemonicHandler = &mneSTY; mneAM[i].cycles = 3; mneAM[i].am = ZEROPAGE;
	i=0x85; mneAM[i].pt2MnemonicHandler = &mneSTA; mneAM[i].cycles = 3; mneAM[i].am = ZEROPAGE;
	i=0x86; mneAM[i].pt2MnemonicHandler = &mneSTX; mneAM[i].cycles = 3; mneAM[i].am = ZEROPAGE;
	i=0x87; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x88; mneAM[i].pt2MnemonicHandler = &mneDEY; mneAM[i].cycles = 2; mneAM[i].am = IMPLIED;
	i=0x89; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x8a; mneAM[i].pt2MnemonicHandler = &mneTXA; mneAM[i].cycles = 2; mneAM[i].am = IMPLIED;
	i=0x8b; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x8c; mneAM[i].pt2MnemonicHandler = &mneSTY; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE;
	i=0x8d; mneAM[i].pt2MnemonicHandler = &mneSTA; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE;
	i=0x8e; mneAM[i].pt2MnemonicHandler = &mneSTX; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE;
	i=0x8f; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x90; mneAM[i].pt2MnemonicHandler = &mneBCC; mneAM[i].cycles = 2; mneAM[i].am = RELATIVE;
	i=0x91; mneAM[i].pt2MnemonicHandler = &mneSTA; mneAM[i].cycles = 6; mneAM[i].am = INDIRECT_Y;
	i=0x92; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x93; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x94; mneAM[i].pt2MnemonicHandler = &mneSTY; mneAM[i].cycles = 4; mneAM[i].am = ZEROPAGE_X;
	i=0x95; mneAM[i].pt2MnemonicHandler = &mneSTA; mneAM[i].cycles = 4; mneAM[i].am = ZEROPAGE_X;
	i=0x96; mneAM[i].pt2MnemonicHandler = &mneSTX; mneAM[i].cycles = 4; mneAM[i].am = ZEROPAGE_Y;
	i=0x97; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x98; mneAM[i].pt2MnemonicHandler = &mneTYA; mneAM[i].cycles = 2; mneAM[i].am = IMPLIED;
	i=0x99; mneAM[i].pt2MnemonicHandler = &mneSTA; mneAM[i].cycles = 5; mneAM[i].am = ABSOLUTE_Y;
	i=0x9a; mneAM[i].pt2MnemonicHandler = &mneTXS; mneAM[i].cycles = 2; mneAM[i].am = IMPLIED;
	i=0x9b; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x9c; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x9d; mneAM[i].pt2MnemonicHandler = &mneSTA; mneAM[i].cycles = 5; mneAM[i].am = ABSOLUTE_X;
	i=0x9e; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0x9f; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0xa0; mneAM[i].pt2MnemonicHandler = &mneLDY; mneAM[i].cycles = 2; mneAM[i].am = IMMEDIATE;
	i=0xa1; mneAM[i].pt2MnemonicHandler = &mneLDA; mneAM[i].cycles = 6; mneAM[i].am = INDIRECT_X;
	i=0xa2; mneAM[i].pt2MnemonicHandler = &mneLDX; mneAM[i].cycles = 2; mneAM[i].am = IMMEDIATE;
	i=0xa3; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0xa4; mneAM[i].pt2MnemonicHandler = &mneLDY; mneAM[i].cycles = 3; mneAM[i].am = ZEROPAGE;
	i=0xa5; mneAM[i].pt2MnemonicHandler = &mneLDA; mneAM[i].cycles = 3; mneAM[i].am = ZEROPAGE;
	i=0xa6; mneAM[i].pt2MnemonicHandler = &mneLDX; mneAM[i].cycles = 3; mneAM[i].am = ZEROPAGE;
	i=0xa7; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0xa8; mneAM[i].pt2MnemonicHandler = &mneTAY; mneAM[i].cycles = 2; mneAM[i].am = IMPLIED;
	i=0xa9; mneAM[i].pt2MnemonicHandler = &mneLDA; mneAM[i].cycles = 2; mneAM[i].am = IMMEDIATE;
	i=0xaa; mneAM[i].pt2MnemonicHandler = &mneTAX; mneAM[i].cycles = 2; mneAM[i].am = IMPLIED;
	i=0xab; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0xac; mneAM[i].pt2MnemonicHandler = &mneLDY; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE;
	i=0xad; mneAM[i].pt2MnemonicHandler = &mneLDA; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE;
	i=0xae; mneAM[i].pt2MnemonicHandler = &mneLDX; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE;
	i=0xaf; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0xb0; mneAM[i].pt2MnemonicHandler = &mneBCS; mneAM[i].cycles = 2; mneAM[i].am = RELATIVE;
	i=0xb1; mneAM[i].pt2MnemonicHandler = &mneLDA; mneAM[i].cycles = 5; mneAM[i].am = INDIRECT_Y;
	i=0xb2; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0xb3; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0xb4; mneAM[i].pt2MnemonicHandler = &mneLDY; mneAM[i].cycles = 4; mneAM[i].am = ZEROPAGE_X;
	i=0xb5; mneAM[i].pt2MnemonicHandler = &mneLDA; mneAM[i].cycles = 4; mneAM[i].am = ZEROPAGE_X;
	i=0xb6; mneAM[i].pt2MnemonicHandler = &mneLDX; mneAM[i].cycles = 4; mneAM[i].am = ZEROPAGE_Y;
	i=0xb7; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0xb8; mneAM[i].pt2MnemonicHandler = &mneCLV; mneAM[i].cycles = 2; mneAM[i].am = IMPLIED;
	i=0xb9; mneAM[i].pt2MnemonicHandler = &mneLDA; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE_Y;
	i=0xba; mneAM[i].pt2MnemonicHandler = &mneTSX; mneAM[i].cycles = 2; mneAM[i].am = IMPLIED;
	i=0xbb; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0xbc; mneAM[i].pt2MnemonicHandler = &mneLDY; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE_X;
	i=0xbd; mneAM[i].pt2MnemonicHandler = &mneLDA; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE_X;
	i=0xbe; mneAM[i].pt2MnemonicHandler = &mneLDX; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE_Y;
	i=0xbf; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0xc0; mneAM[i].pt2MnemonicHandler = &mneCPY; mneAM[i].cycles = 2; mneAM[i].am = IMMEDIATE;
	i=0xc1; mneAM[i].pt2MnemonicHandler = &mneCMP; mneAM[i].cycles = 6; mneAM[i].am = INDIRECT_X;
	i=0xc2; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0xc3; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0xc4; mneAM[i].pt2MnemonicHandler = &mneCPY; mneAM[i].cycles = 3; mneAM[i].am = ZEROPAGE;
	i=0xc5; mneAM[i].pt2MnemonicHandler = &mneCMP; mneAM[i].cycles = 3; mneAM[i].am = ZEROPAGE;
	i=0xc6; mneAM[i].pt2MnemonicHandler = &mneDEC; mneAM[i].cycles = 5; mneAM[i].am = ZEROPAGE;
	i=0xc7; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0xc8; mneAM[i].pt2MnemonicHandler = &mneINY; mneAM[i].cycles = 2; mneAM[i].am = IMPLIED;
	i=0xc9; mneAM[i].pt2MnemonicHandler = &mneCMP; mneAM[i].cycles = 2; mneAM[i].am = IMMEDIATE;
	i=0xca; mneAM[i].pt2MnemonicHandler = &mneDEX; mneAM[i].cycles = 2; mneAM[i].am = IMPLIED;
	i=0xcb; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0xcc; mneAM[i].pt2MnemonicHandler = &mneCPY; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE;
	i=0xcd; mneAM[i].pt2MnemonicHandler = &mneCMP; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE;
	i=0xce; mneAM[i].pt2MnemonicHandler = &mneDEC; mneAM[i].cycles = 6; mneAM[i].am = ABSOLUTE;
	i=0xcf; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0xd0; mneAM[i].pt2MnemonicHandler = &mneBNE; mneAM[i].cycles = 2; mneAM[i].am = RELATIVE;
	i=0xd1; mneAM[i].pt2MnemonicHandler = &mneCMP; mneAM[i].cycles = 5; mneAM[i].am = INDIRECT_Y;
	i=0xd2; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0xd3; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0xd4; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0xd5; mneAM[i].pt2MnemonicHandler = &mneCMP; mneAM[i].cycles = 4; mneAM[i].am = ZEROPAGE_X;
	i=0xd6; mneAM[i].pt2MnemonicHandler = &mneDEC; mneAM[i].cycles = 6; mneAM[i].am = ZEROPAGE_X;
	i=0xd7; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0xd8; mneAM[i].pt2MnemonicHandler = &mneCLD; mneAM[i].cycles = 2; mneAM[i].am = IMPLIED;
	i=0xd9; mneAM[i].pt2MnemonicHandler = &mneCMP; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE_Y;
	i=0xda; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0xdb; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0xdc; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0xdd; mneAM[i].pt2MnemonicHandler = &mneCMP; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE_X;
	i=0xde; mneAM[i].pt2MnemonicHandler = &mneDEC; mneAM[i].cycles = 7; mneAM[i].am = ABSOLUTE_X;
	i=0xdf; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0xe0; mneAM[i].pt2MnemonicHandler = &mneCPX; mneAM[i].cycles = 2; mneAM[i].am = IMMEDIATE;
	i=0xe1; mneAM[i].pt2MnemonicHandler = &mneSBC; mneAM[i].cycles = 6; mneAM[i].am = INDIRECT_X;
	i=0xe2; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0xe3; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0xe4; mneAM[i].pt2MnemonicHandler = &mneCPX; mneAM[i].cycles = 3; mneAM[i].am = ZEROPAGE;
	i=0xe5; mneAM[i].pt2MnemonicHandler = &mneSBC; mneAM[i].cycles = 3; mneAM[i].am = ZEROPAGE;
	i=0xe6; mneAM[i].pt2MnemonicHandler = &mneINC; mneAM[i].cycles = 5; mneAM[i].am = ZEROPAGE;
	i=0xe7; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0xe8; mneAM[i].pt2MnemonicHandler = &mneINX; mneAM[i].cycles = 2; mneAM[i].am = IMPLIED;
	i=0xe9; mneAM[i].pt2MnemonicHandler = &mneSBC; mneAM[i].cycles = 2; mneAM[i].am = IMMEDIATE;
	i=0xea; mneAM[i].pt2MnemonicHandler = &mneNOP; mneAM[i].cycles = 2; mneAM[i].am = IMPLIED;
	i=0xeb; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0xec; mneAM[i].pt2MnemonicHandler = &mneCPX; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE;
	i=0xed; mneAM[i].pt2MnemonicHandler = &mneSBC; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE;
	i=0xee; mneAM[i].pt2MnemonicHandler = &mneINC; mneAM[i].cycles = 6; mneAM[i].am = ABSOLUTE;
	i=0xef; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0xf0; mneAM[i].pt2MnemonicHandler = &mneBEQ; mneAM[i].cycles = 2; mneAM[i].am = RELATIVE;
	i=0xf1; mneAM[i].pt2MnemonicHandler = &mneSBC; mneAM[i].cycles = 5; mneAM[i].am = INDIRECT_Y;
	i=0xf2; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0xf3; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0xf4; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0xf5; mneAM[i].pt2MnemonicHandler = &mneSBC; mneAM[i].cycles = 4; mneAM[i].am = ZEROPAGE_X;
	i=0xf6; mneAM[i].pt2MnemonicHandler = &mneINC; mneAM[i].cycles = 6; mneAM[i].am = ZEROPAGE_X;
	i=0xf7; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0xf8; mneAM[i].pt2MnemonicHandler = &mneSED; mneAM[i].cycles = 2; mneAM[i].am = IMPLIED;
	i=0xf9; mneAM[i].pt2MnemonicHandler = &mneSBC; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE_Y;
	i=0xfa; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0xfb; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0xfc; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;
	i=0xfd; mneAM[i].pt2MnemonicHandler = &mneSBC; mneAM[i].cycles = 4; mneAM[i].am = ABSOLUTE_X;
	i=0xfe; mneAM[i].pt2MnemonicHandler = &mneINC; mneAM[i].cycles = 7; mneAM[i].am = ABSOLUTE_X;
	i=0xff; mneAM[i].pt2MnemonicHandler = &mneILL; mneAM[i].cycles = 0; mneAM[i].am = ILLEGAL;

}

void initRegisters() {
	A = 0x0;
	X = 0x0;
	Y = 0x0;
	P = 0x0;
	PC = memReadWord(0xfffc); //load the address from FFFC (kernal rom)
	S = 0x00;
}

void reset() {
	cyc=0;
	initRam();
	initRegisters();
}


void mos6510_cycle() {
	if(cyc--<=0) {
		byte opCode = readMemoryPC();
		//printf2("[%x] op=%x  ",PC-1, opCode);
//		if((PC-1)==0xe430) {
//			printf("hoplaa");
//		}
		mneAM[opCode].pt2MnemonicHandler(mneAM[opCode].am, mneAM[opCode].cycles);
		cyc--;
	}
	#if DEBUG_6510
	else printf(".");
	#endif
}


void mos6510_init() {
	printf("MOS6510 init");
	initMnemonicArray();
	//loadROM();
	reset();
}

void mos6510_HWInterrupt() {
	printf("HW interrupt\n");
}


/*
int main(void) {

	initMnemonicArray();
	loadROM();

	//word loadAddress = loadTestProgram();
	//memWriteByte(0x8000, loadAddress); // 0x8000 cardridge jump point
	//memWriteByte(0x8001, (loadAddress >> 8));

	reset();

	mainLoop();


	return EXIT_SUCCESS;
}*/
