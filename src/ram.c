/*
 ============================================================================
 Name        : ram.c
 Author      : Sampo Peltonen
 Licence     : GNU General Public License, version 2.
 Copyright   : Copyright (C) 2013  Sampo Peltonen
 Description : Commodore 64 simulation
 ============================================================================
 */

//#include <stdlib.h>
//#include <stdio.h>
#include "ram.h"
//#include "rom.h"
#include "ioregarea.h"
//#include "monitor.h"
#include "stdlibtools.h"

#define R6510_FLAG_0 1
#define R6510_FLAG_1 2
#define R6510_FLAG_2 4

extern byte kernalROM[];
extern byte basicROM[];
extern byte characterROM[];

/* whole memory space allocated */
byte mem[0xffff];



byte memReadByte(word address) {

	
	/*if(address>=0x37 && address<=0x38) {
		printf2("BAE: %x read value %x",address,mem[address]);
	}*/


	/*
	if((R6510_FLAG_0 + R6510_FLAG_1) == ((R6510_FLAG_0 + R6510_FLAG_1) & ~*R6510)) {
		monitorReadByte(MONITOR_MEM_BANK_MAIN, address);
		return mem[address]; // ram visible in all areas
	}*/

	if(address >= 0xe000) {
		if(R6510_FLAG_1 == (*R6510 & R6510_FLAG_1)) {
			//monitorReadByte(MONITOR_MEM_BANK_ALTERNATIVE, address);
			//printf1("Reading kernal rom from address %x",address-0xe000);
			return(kernalROM[address-0xe000]);
		}
		else {
			//monitorReadByte(MONITOR_MEM_BANK_MAIN, address);
			return(mem[address]);
		}
	}

	if(address >= 0xa000 && address <=0xbfff) {
		if((R6510_FLAG_0 + R6510_FLAG_1) == ((R6510_FLAG_0 + R6510_FLAG_1) & *R6510)) {
			//monitorReadByte(MONITOR_MEM_BANK_ALTERNATIVE, address);
			return(basicROM[address-0xa000]);
		}
		else {
			//monitorReadByte(MONITOR_MEM_BANK_MAIN, address);
			return(mem[address]);
		}
	}

	if(address >= 0xd000 && address <=0xdfff) {
		if((R6510_FLAG_2) == ((R6510_FLAG_2) & *R6510)) {
			// ioarea is visible
			//printf("reading from i/o area (%x) ioarea(%x) value=%x\n", address, address-0xd000, *ioarea[address-0xd000]);
			//monitorReadByte(MONITOR_MEM_BANK_MAIN, address);
			return(ioAreaReadByte(address-0xd000));
		}
		else {
			//printf("reading from character rom area (%x)\n", address);
			//monitorReadByte(MONITOR_MEM_BANK_ALTERNATIVE, address);
			return(characterROM[address-0xd000]);
		}
	}

	//monitorReadByte(MONITOR_MEM_BANK_MAIN, address);
	return mem[address];
}


word memReadWord(word address) {

	return(memReadByte(address) + (memReadByte(address + 1) << 8));
}

void memWriteByte(word address, byte data) {

	if(address >= 0xd000 && address <=0xdfff) {

		if((R6510_FLAG_2) == ((R6510_FLAG_2) & *R6510)) {
			ioAreaWriteByte(address-0xd000, data);
			//monitorWriteByte(MONITOR_MEM_BANK_MAIN, address);
		}
		else {
			//monitorWriteByte(MONITOR_MEM_BANK_MAIN, address);
			mem[address] = data; // writing the mem underlying under character ROM area
		}
	}
	else {
		//monitorWriteByte(MONITOR_MEM_BANK_MAIN, address);
		mem[address] = data;
	}
	//printf("memWrite adr %x = %x, ", address, mem[address]);
}

/*
// mitä nämä ovat? olen unohtanut.
void vicSetRegisterBits(word address, byte bits) {
	mem[address] = mem[address] | bits;
}
void vicClearRegisterBits(word address, byte bits) {
	mem[address] = mem[address] & ~bits;
}
void vicWriteRegister(word address, byte data) {
	mem[address] = data;
}
*/



/* set memory to default state
 * as described in http://sta.c64.org/cbm64mem.html
 * */
void initRam() {
	*D6510 = 0x2f;
	*R6510 = 0x37;
	//*ADRAY1 = 0xb1;
	//*(ADRAY1 + sizeof(byte)) = 0xaa;
	//*INTFLG = 0x80;
	*MEMSIZ = 0xA0;
	*(MEMSIZ + 1) = 0x00;
	*TXTTAB = 0x08;
	*(TXTTAB  + 1) =0x01;
	initIoRegArea();
}

/* pointers to specific memory locations
 * labeled as in http://c64.ch/programming/memorymap.php
 */

/* 6510 On chip Data Direction Register */
byte *D6510 = &mem[0x0000]; // 0

/* 6510 On chip 8 bit Input/Output Register */
byte *R6510 = &mem[0x0001]; // 1

/* unused */
//byte *_ = &mem[0x0002]; // 2 1

/* Jump Vector: Convert Floating - Integer */
byte *ADRAY1 = &mem[0x0003]; // 3-4 $B1AA

/* Jump Vector: Convert Integer - Floating */
byte *ADRAY2 = &mem[0x0005]; // 5-6

/* Search Character */
byte *CHARAC = &mem[0x0007]; // 7

/* Flag: Scan for Quote at End of String */
byte *ENDCHR = &mem[0x0008]; // 8

/* Screen Column From Last TAB */
byte *TRMPOS = &mem[0x0009]; // 9

/* Flag: 0=Load 1=Verify */
byte *VERCK = &mem[0x000A]; // 10

/* Input Buffer Pointer / Number of Subscripts */
byte *COUNT = &mem[0x000B]; // 11

/* Flag: Default Array DIMension */
byte *DIMFLG = &mem[0x000C]; // 12

/* Data Type: $FF=String $00=Numeric */
byte *VALTYP = &mem[0x000D]; // 13

/* Data Type: $80=Integer $00=Floating */
byte *INTFLG = &mem[0x000E]; // 14

/* Flag: DATA scan / LIST quote / Garbage Coll */
byte *GARBFL = &mem[0x000F]; // 15

/* Flag: Subscript Ref / User Function Call */
byte *SUBFLG = &mem[0x0010]; // 16

/* Flag: $00=INPUT, $40=GET, $98=READ */
byte *INPFLG = &mem[0x0011]; // 17

/* Flag: TAN sign / Comparison Result */
byte *TANSGN = &mem[0x0012]; // 18

/* Flag: INPUT Prompt */
//byte *_ = &mem[0x0013]; // 19

/* Temp: Integer Value */
byte *LINNUM = &mem[0x0014]; // 23-24

/* Temp String Stack */
byte *TEMPPT = &mem[0x0016]; // 22 Pointer:

/* Last Temp String Address */
byte *LASTPT = &mem[0x0017]; // 23-24

/* Stack for Temp Strings */
byte *TEMPST = &mem[0x0019]; // 25-33

/* Utility Pointer Area */
byte *INDEX = &mem[0x0022]; // 34-37

/* Floating Point Product of Multiply */
byte *RESHO = &mem[0x0026]; // 38-42

/* Pointer: Start of BASIC Text */
byte *TXTTAB = &mem[0x002B]; // 43-4

/* Pointer: Start of BASIC Variables */
byte *VARTAB = &mem[0x002D]; // 45-46

/* Pointer: Start of BASIC Arrays */
byte *ARYTAB = &mem[0x002F]; // 47-48

/* Pointer: End of BASIC Arrays (+1) */
byte *STREND = &mem[0x0031]; // 49-50

/* Pointer: Bottom of String Storage */
byte *FRETOP = &mem[0x0033]; // 51-52

/* Utility String Pointer */
byte *FRESPC = &mem[0x0035]; // 53-54

/* Pointer: Highest Address Used by BASIC */
byte *MEMSIZ = &mem[0x0037]; // 55-56

/* Current BASIC Line Number */
byte *CURLIN = &mem[0x0039]; // 57-58

/* Previous BASIC Line Number */
byte *OLDLIN = &mem[0x003B]; // 59-60

/* Pointer: BASIC Statement for CONT */
byte *OLDTXT = &mem[0x003D]; // 61-62

/* Current DATA Line Number */
byte *DATLIN = &mem[0x003F]; // 63-64

/* Pointer: Current DATA Item Address */
byte *DATPTR = &mem[0x0041]; // 65-66

/* Vector: INPUT Routine */
byte *INPPTR = &mem[0x0043]; // 67-68

/* Current BASIC Variable Name */
byte *VARNAM = &mem[0x0045]; // 69-70

/* Pointer: Current BASIC Variable Data */
byte *VARPNT = &mem[0x0047]; // 71-72

/* Pointer: Index Variable for FOR / NEXT */
byte *FORPNT = &mem[0x0049]; // 73-74

/* Temp Pointer / Data Area */
//byte *_ = &mem[0x004B]; // 75-96

/* Floating Point Accumulator #1: Exponent */
byte *FACEXP = &mem[0x0061]; // 97

/* Floating Accumulator #1: Mantissa */
byte *FACHO = &mem[0x0062]; // 98-101

/* Floating Accumulator #1: Sign */
byte *FACSGN = &mem[0x0066]; // 102

/* Pointer: Series Evaluation Constant */
byte *SGNFLG = &mem[0x0067]; // 103

/* Floating Accumualtor #1: Overflow Digit */
byte *BITS = &mem[0x0068]; // 104

/* Floating Point Acc. #2: Exponent */
byte *ARGEXP = &mem[0x0069]; // 105

/* Floating Acc. #2: Mantissa */
byte *ARGHO = &mem[0x006A]; // 106-109

/* Floating Acc. #2: Sign */
byte *ARGSGN = &mem[0x006E]; // 110

/* Sign Comparison Result: Acc. #1 vs #2 */
byte *ARISGN = &mem[0x006F]; // 111

/* Flaoting Acc. #1: Low Order (Rounding) */
byte *FACOV = &mem[0x0070]; // 112

/* Pointer: Cassette Buffer */
byte *FBUFPT = &mem[0x0071]; // 113-114

/* Subroutine: Get Next Byte of BASIC Text */
byte *CHRGET = &mem[0x0073]; // 115-138

/* Entry to Get Same Byte of BASIC Text */
byte *CHRGOT = &mem[0x0079]; // 121

/* Pointer: Current Byte of BASIC Text */
byte *TXTPTR = &mem[0x007A]; // 122-123

/* Floating RND Function Seed Value */
byte *RNDX = &mem[0x008B]; // 139-143

/* Kernal I/O Status Word: ST */
byte *STATUS = &mem[0x0090]; // 144

/* Flag: STOP Key / RVS Key */
byte *STKEY = &mem[0x0091]; // 145

/* Timing Constant for Tape */
byte *SVXT = &mem[0x0092]; // 146

/* Flag: 0=Load, 1=Verify */
byte *VERCK2 = &mem[0x0093]; // 147

/* Flag: Serial Bus - Output Char, Buffered */
byte *C3PO = &mem[0x0094]; // 148

/* Buffered Character for Serial Bus */
byte *BSOUR = &mem[0x0095]; // 149

/* Cassettte Sync No. */
byte *SYNO = &mem[0x0096]; // 150

/* Temp Data Area */
//byte *_ = &mem[0x0097]; // 151

/* No. of Open Files / Index to File Table */
byte *LDTND = &mem[0x0098]; // 152

/* Default Input Device (0) */
byte *DFLTN = &mem[0x0099]; // 153

/* Default Output (CMD) Device (3) */
byte *DFLTO = &mem[0x009A]; // 154

/* Tape Character Parity */
byte *PRTY = &mem[0x009B]; // 155

/* Flag: Tape Byte Recieved */
byte *DPSW = &mem[0x009C]; // 156

/* Flag: $80=Direct Mode, $00=Program */
byte *MSGFLG = &mem[0x009D]; // 157

/* Tape Pass 1 Error Log */
byte *PTR1 = &mem[0x009E]; // 158

/* Tape Pass 2 Error Log */
byte *PTR2 = &mem[0x009F]; // 159

/* Real Time Jiffy Clock (Approx 1/60 sec) */
byte *TIME = &mem[0x00A0]; // 160-162

/* Temp Data Area */
//byte *_ = &mem[0x00A3]; // 163-164

/* Cassette Sync Countdown */
byte *CNTDN = &mem[0x00A5]; // 165

/* Pointer: Tape I/O Buffer */
byte *BUFPNT = &mem[0x00A6]; // 166

/* RS-232 Input Bits / Cassette Temp */
byte *INBIT = &mem[0x00A7]; // 167

/* RS-232 Input Bit Count / Cassette Temp */
byte *BITCI = &mem[0x00A8]; // 168

/* RS-232 Flag: Check for Start Bit */
byte *RINONE = &mem[0x00A9]; // 169

/* RS-232 Input Byte Buffer / Cassette Temp */
byte *RIDATA = &mem[0x00AA]; // 170

/* Rs-232 Input Parity / Cassette Short Cnt */
byte *RIPRTY = &mem[0x00AB]; // 171

/* 172-173 Pointer: Tape Buffer / Screen Scrolling */
byte *SAL = &mem[0x00AC]; //

/* 174-175 Tape End Addresses / End of Program */
byte *EAL = &mem[0x00AE]; //

/* 176-177 Tape Timing Constants */
byte *CMPO = &mem[0x00B0]; //

/* 178-179 Pointer: Start of Tape Buffer */
byte *TAPE1 = &mem[0x00B2]; //

/* RS-232 One Bit Count / Cassette Temp */
byte *BITTS = &mem[0x00B4]; // 180

/* RS-232 Next Bit to Send / Tape EOT Flag */
byte *NXTBIT = &mem[0x00B5]; // 181

/* RS-232 Out Byte Buffer */
byte *RODATA = &mem[0x00B6]; // 182

/* Length of Current File Name */
byte *FNLEN = &mem[0x00B7]; // 183

/* Current Logical File Number */
byte *LA = &mem[0x00B8]; // 184

/* Current Secondary Address */
byte *SA = &mem[0x00B9]; // 185

/* Current Device Number */
byte *FA = &mem[0x00B1]; // 186

/* Pointer: Curertn File Name */
byte *FNADR = &mem[0x00BB]; // 187-188

/* RS-232 Out Parity / Cassette Temp */
byte *ROPRTY = &mem[0x00BD]; // 189

/* Cassette Read / Write Block Count */
byte *FSBLK = &mem[0x00BE]; // 190

/* Serial Word Buffer */
byte *MYCH = &mem[0x00BF]; // 191

/* Tape Motor Interlock */
byte *CAS1 = &mem[0x00C0]; // 192

/* I/O Start Address */
byte *STAL = &mem[0x00C1]; // 193-194

/* Tape Load Temps */
byte *MEMUSS = &mem[0x00C3]; // 195-196

/* Curretn Key Pressed: CHR$(n) 0=No Key */
byte *LSTX = &mem[0x00C5]; // 197

/* No. of Chars in Keyboard Buffer (Queue) */
byte *NDX = &mem[0x00C6]; // 198

/* Flag: Print Reverse Chars 1=Yes, 0=No */
byte *RVS = &mem[0x00C7]; // 199

/* Pointer: End of Logical Line for INPUT */
byte *INDX = &mem[0x00C8]; // 200

/* Cursor X-Y Position at Start of INPUT */
byte *LXSP = &mem[0x00C9]; // 201-202

/* Flag: Print Shifted Chars */
byte *SFDX = &mem[0x00CB]; // 203

/* Cursor Blink Enable: 0=Flash Cursor */
byte *BLNSW = &mem[0x00CC]; // 2-4

/* Timer: Countdown to Toggle Cursor */
byte *BLNCT = &mem[0x00CD]; // 205

/* Character Under Cursor */
byte *GDBLN = &mem[0x00CE]; // 206

/* Flag: Last Cursor Blink On / Off */
byte *BLNON = &mem[0x00CF]; // 207

/* Flag: INPUT or GET from Keyboard */
byte *CRSW = &mem[0x00D0]; // 208

/* Pointer: Current Screen Line Address */
byte *PNT = &mem[0x00D1]; // 209-210

/* Cursor Column on Current Line */
byte *PNTR = &mem[0x00D3]; // 211

/* Flag: Editor in Quote Mode, $00=NO */
byte *QTSW = &mem[0x00D4]; // 212

/* Physical Screen Line Length */
byte *LNMX = &mem[0x00D5]; // 213

/* Current Cursor Physical Line Number */
byte *TBLX = &mem[0x00D6]; // 214

/* Temp Data Area */
//byte *_ = &mem[0x00D7]; // 215

/* Flag: Insert Mode >0 = # INSTs */
byte *INSRT = &mem[0x00D8]; // 216

/* Screen Line Link Table / Editor Temps */
byte *LDTB1 = &mem[0x00D9]; // 217-242

/* Pointer: Current Screen Colour RAM Location */
byte *USER = &mem[0x00F3]; // 243-244

/* Vector: Keyboard Decode Table */
byte *KEYTAB = &mem[0x00F5]; // 245-246

/* RS-232 Input Buffer Pointer */
byte *RIBUF = &mem[0x00F7]; // 247-248

/* RS-232 Output Buffer Pointer */
byte *ROBUF = &mem[0x00F9]; // 249-250

/* Free Zero Page Space for User Programs */
byte *FREKZP = &mem[0x00FB]; // 251-254

/* BASIC Temp Data Area */
byte *BASZPT = &mem[0x00FF]; // 255

/* Micro Processor System Stack Area */
//byte *_ = &mem[0x0100]; // 256-511

/* Floating to String Work Area */
//byte *_ = &mem[0x0100]; // 256-266

/* Tape Input error Log */
byte *BAD = &mem[0x0100]; // 256-318

/* System INPUT Buffer */
byte *BUF = &mem[0x0200]; // 512-600

/* KERNAL Table: Active Logical File No's */
byte *LAT = &mem[0x0259]; // 601-610

/* KERNAL Table: Device No. for Each File */
byte *FAT = &mem[0x0263]; // 611-620

/* KERNAL Table: Second Address Each File */
byte *SAT = &mem[0x026D]; // 621-630

/* Keyboard Buffer Queue (FIFO) */
byte *KEYD = &mem[0x0277]; // 631-640

/* Pointer: Bottom of Memory for OS */
byte *MEMSTR = &mem[0x0281]; // 641-642

/* Pointer: Top of Memory of OS */
byte *MEMSIZ2 = &mem[0x0283]; // 643-644

/* Flag: Kernal Variable for IEEE Timeout */
byte *TIMOUT = &mem[0x0285]; // 645

/* Current Character Colour Code */
byte *COLOR = &mem[0x0286]; // 646

/* Background Colour Under Cursor */
byte *GDCOL = &mem[0x0287]; // 647

/* Top of Screen Memory (Page) */
byte *HIBASE = &mem[0x0288]; // 648

/* Size of Keyboard Buffer */
byte *XMAX = &mem[0x0289]; // 649

/* Flag: REPEAT Key Used, $80=Repeat */
byte *RPTFLG = &mem[0x028A]; // 650

/* Repeat Speed Counter */
byte *KOUNT = &mem[0x028B]; // 651

/* Repeat Delay Counter */
byte *DELAY = &mem[0x028C]; // 652

/* Flag: Keyboard SHIFT Key / CTRL Key / C Key */
byte *SHFLAG = &mem[0x028D]; // 653

/* Last Keybaord Shift Pattern */
byte *LSTSHF = &mem[0x028E]; // 654

/* Vector: Keyboard Table Setup */
byte *KEYLOG = &mem[0x028F]; // 655-656

/* Flag: $00=Disable SHIFT Keys, $80=Enable SHIFT Keys */
byte *MODE = &mem[0x0291]; // 657

/* Flag: Auto Scroll Down, 0=ON */
byte *AUTODN = &mem[0x0292]; // 658

/* RS-232: 6551 Control Register Image */
byte *M51CTR = &mem[0x0293]; // 659

/* RS-232: 6551 Command Register Image */
byte *M51CDR = &mem[0x0294]; // 660

/* RS-232 Non Standard BPS (Time/2-100) USA */
byte *M51AJB = &mem[0x0295]; // 661-662

/* RS-232: 6551 Status Register Image */
byte *RSSTAT = &mem[0x0297]; // 663

/* RS-232 Number of Bits Left to Send */
byte *BITNUM = &mem[0x0298]; // 664

/* RS-232 Baud Rate: Full Bit Time (micro seconds) */
byte *BAUDOF = &mem[0x0299]; // 665-666

/* RS-232 Index to End of Input Buffer */
byte *RIDBE = &mem[0x029B]; // 667

/* RS-232 Start of Input Buffer (Page) */
byte *RIDBS = &mem[0x029C]; // 668

/* RS-232 Start of Output Buffer (Page) */
byte *RODBS = &mem[0x029D]; // 669

/* RS-232 Index to End of Output Buffer */
byte *RODBE = &mem[0x029E]; // 670

/* Holds IRQ Vector During Tape I/O */
byte *IRQTMP = &mem[0x029F]; // 671-672

/* RS-232 Enables */
byte *ENABL = &mem[0x02A1]; // 673

/* TOD Sense During Cassette I/O */
//byte *_ = &mem[0x02A2]; // 674

/* Temp Storage for Cassette Read */
//byte *_ = &mem[0x02A3]; // 675

/* Temp D1IRQ Indicator for Cassette Read */
//byte *_ = &mem[0x02A4]; // 676

/* Temp for Line Index */
//byte *_ = &mem[0x02A5]; // 677

/* PAL / NTSC Flag, 0=NTSC, 1=PAL */
//byte *_ = &mem[0x02A6]; // 678

/* Unused */
//byte *_ = &mem[0x02A7]; // 679-767

/* Vector: Print BASIC Error Message */
byte *IERROR = &mem[0x0300]; // 768-769

/* Vector: BASIC Warm Start */
byte *IMAIN = &mem[0x0302]; // 770-771

/* Vector: Tokenize BASIC Text */
byte *ICRNCH = &mem[0x0304]; // 772-773

/* Vector: BASIC Text LIST */
byte *IQPLOP = &mem[0x0306]; // 774-775

/* Vector: BASIC Character Dispatch */
byte *IGONE = &mem[0x0308]; // 776-777

/* Vector: BASIC Token Evaluation */
byte *IEVAL = &mem[0x030A]; // 778-779

/* Storage for 6502 A Register */
byte *SAREG = &mem[0x030C]; // 780

/* Storage for 6502 X Register */
byte *SXREG = &mem[0x030D]; // 781

/* Storage for 6502 Y Register */
byte *SYREG = &mem[0x030E]; // 782

/* Storage for 6502 SP Register */
byte *SPREG = &mem[0x030F]; // 783

/* USR Function Jump Instr (4C) */
byte *USRPOK = &mem[0x0310]; // 784

/* USR Address Low Byte / High Byte */
byte *USRADD = &mem[0x0311]; // 785-786

/* Unused */
//byte *_ = &mem[0x0313]; // 787

/* Vector: Hardware IRQ Interrupt */
byte *CINV = &mem[0x0314]; // 788-789

/* Vector: BRK Instruction Interrupt */
byte *CBINV = &mem[0x0316]; // 790-791

/* Vector: Non Maskable Interrupt */
byte *NMINV = &mem[0x0318]; // 792-793

/* KERNAL OPEN Routine Vector */
byte *IOPEN = &mem[0x031A]; // 794-795

/* KERNAL CLOSE Routine Vector */
byte *ICLOSE = &mem[0x031C]; // 796-797

/* KERNAL CHKIN Routine Vector */
byte *ICHKIN = &mem[0x031E]; // 798-799

/* KERNAL CHKOUT Routine Vector */
byte *ICKOUT = &mem[0x0320]; // 800-801

/* KERNAL CLRCHN Routine Vector */
byte *ICLRCH = &mem[0x0322]; // 802-803

/* KERNAL CHRIN Routine Vector */
byte *IBASIN = &mem[0x0324]; // 804-805

/* KERNAL CHROUT Routine Vector */
byte *IBSOUT = &mem[0x0326]; // 806-807

/* KERNAL STOP Routine Vector */
byte *ISTOP = &mem[0x0328]; // 808-809

/* KERNAL GETIN Routine Vector */
byte *IGETIN = &mem[0x032A]; // 810-811

/* KERNAL CLALL Routine Vector */
byte *ICLALL = &mem[0x032C]; // 812-813

/* User Defined Vector */
byte *USRCMN = &mem[0x032E]; // 814-815

/* KERNAL LOAD Routine Vector */
byte *ILOAD = &mem[0x0330]; // 816-817

/* KERNAL SAVE Rountine Vector */
byte *ISAVE = &mem[0x0332]; // 818-819

/* Unused */
//byte *_ = &mem[0x0334]; // 820-827

/* Tape I/O Buffer */
byte *TBUFFR = &mem[0x033C]; // 828-1019

/* Unused */
//byte *_ = &mem[0x03FC]; // 1020-1023

/* 1024 Byte Screen Memory Area */
byte *VICSCN = &mem[0x0400]; // 1024-2047

/* Video Matrix: 25 Lines * 40 Columns */
//byte *_ = &mem[0x0400]; // 1024-2023

/* Sprite Data Area */
//byte *_ = &mem[0x07F8]; // 2040-2047

/* Normal BASIC Program Space */
//byte *_ = &mem[0x0800]; // 2048-40959

/* VSP Cartridge Rom - 8192 Bytes */
//byte *_ = &mem[0x8000]; // 32768-40959

/* BASIC ROM - 8192 Bytes 8K RAM */
//byte *_ = &mem[0xA000]; // 40960-49151

/* RAM - 4096 Bytes */
//byte *_ = &mem[0xC000]; // 49151-53247

/* IP / OP Devices & Colour RAM or char gen ROM or RAM */
//byte *_ = &mem[0xD000]; // 53248-57343

/* KERNAL ROM - 8192 Bytes 8k RAM */
//byte *_ = &mem[0xE000]; // 57344-65535



