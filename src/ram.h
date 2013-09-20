#ifndef RAM_H_
#define RAM_H_

#include "types.h"

byte memReadByte(word address);
word memReadWord(word address);
void memWriteByte(word address, byte data);


/* pointers to specific memory locations
 * labeled as in http://c64.ch/programming/memorymap.php
 */

/* 6510 On chip Data Direction Register */
extern byte *D6510; // 0 

/* 6510 On chip 8 bit Input/Output Register */
extern byte *R6510; // 1 

/* unused */
//byte *_ = &mem[0x0002]; // 2 

/* Jump Vector: Convert Floating - Integer */
extern byte *ADRAY1; // 3-4 

/* Jump Vector: Convert Integer - Floating */
extern byte *ADRAY2; // 5-6 

/* Search Character */
extern byte *CHARAC; // 7 

/* Flag: Scan for Quote at End of String */
extern byte *ENDCHR; // 8 

/* Screen Column From Last TAB */
extern byte *TRMPOS; // 9 

/* Flag: 0=Load 1=Verify */
extern byte *VERCK; // 10 

/* Input Buffer Pointer / Number of Subscripts */
extern byte *COUNT; // 11 

/* Flag: Default Array DIMension */
extern byte *DIMFLG; // 12 

/* Data Type: $FF=String $00=Numeric */
extern byte *VALTYP; // 13 

/* Data Type: $80=Integer $00=Floating */
extern byte *INTFLG; // 14 

/* Flag: DATA scan / LIST quote / Garbage Coll */
extern byte *GARBFL; // 15 

/* Flag: Subscript Ref / User Function Call */
extern byte *SUBFLG; // 16 

/* Flag: $00=INPUT, $40=GET, $98=READ */
extern byte *INPFLG; // 17 

/* Flag: TAN sign / Comparison Result */
extern byte *TANSGN; // 18 

/* Flag: INPUT Prompt */
//byte *_ = &mem[0x0013]; // 19 

/* Temp: Integer Value */
extern byte *LINNUM; // 23-24 

/* Temp String Stack */
extern byte *TEMPPT; // 22 Pointer: 

/* Last Temp String Address */
extern byte *LASTPT; // 23-24 

/* Stack for Temp Strings */
extern byte *TEMPST; // 25-33 

/* Utility Pointer Area */
extern byte *INDEX; // 34-37 

/* Floating Point Product of Multiply */
extern byte *RESHO; // 38-42 

/* Pointer: Start of BASIC Text */
extern byte *TXTTAB; // 43-4 

/* Pointer: Start of BASIC Variables */
extern byte *VARTAB; // 45-46 

/* Pointer: Start of BASIC Arrays */
extern byte *ARYTAB; // 47-48 

/* Pointer: End of BASIC Arrays (+1) */
extern byte *STREND; // 49-50 

/* Pointer: Bottom of String Storage */
extern byte *FRETOP; // 51-52 

/* Utility String Pointer */
extern byte *FRESPC; // 53-54 

/* Pointer: Highest Address Used by BASIC */
extern byte *MEMSIZ; // 55-56 

/* Current BASIC Line Number */
extern byte *CURLIN; // 57-58 

/* Previous BASIC Line Number */
extern byte *OLDLIN; // 59-60 

/* Pointer: BASIC Statement for CONT */
extern byte *OLDTXT; // 61-62 

/* Current DATA Line Number */
extern byte *DATLIN; // 63-64 

/* Pointer: Current DATA Item Address */
extern byte *DATPTR; // 65-66 

/* Vector: INPUT Routine */
extern byte *INPPTR; // 67-68 

/* Current BASIC Variable Name */
extern byte *VARNAM; // 69-70 

/* Pointer: Current BASIC Variable Data */
extern byte *VARPNT; // 71-72 

/* Pointer: Index Variable for FOR / NEXT */
extern byte *FORPNT; // 73-74 

/* Temp Pointer / Data Area */
/* Floating Point Product of Multiply */
extern byte *RESHO; // 38-42 

/* Pointer: Start of BASIC Text */
extern byte *TXTTAB; // 43-4 

/* Pointer: Start of BASIC Variables */
extern byte *VARTAB; // 45-46 

/* Pointer: Start of BASIC Arrays */
extern byte *ARYTAB; // 47-48 

/* Pointer: End of BASIC Arrays (+1) */
extern byte *STREND; // 49-50 

/* Pointer: Bottom of String Storage */
extern byte *FRETOP; // 51-52 

/* Utility String Pointer */
extern byte *FRESPC; // 53-54 

/* Pointer: Highest Address Used by BASIC */
extern byte *MEMSIZ; // 55-56 

/* Current BASIC Line Number */
extern byte *CURLIN; // 57-58 

/* Previous BASIC Line Number */
extern byte *OLDLIN; // 59-60 

/* Pointer: BASIC Statement for CONT */
extern byte *OLDTXT; // 61-62 

/* Current DATA Line Number */
extern byte *DATLIN; // 63-64 

/* Pointer: Current DATA Item Address */
extern byte *DATPTR; // 65-66 

/* Vector: INPUT Routine */
extern byte *INPPTR; // 67-68 

/* Current BASIC Variable Name */
extern byte *VARNAM; // 69-70 

/* Pointer: Current BASIC Variable Data */
extern byte *VARPNT; // 71-72 

/* Pointer: Index Variable for FOR / NEXT */
extern byte *FORPNT; // 73-74 

/* Temp Pointer / Data Area */
//byte *_; // 75-96 

/* Floating Point Accumulator #1: Exponent */
extern byte *FACEXP; // 97 

/* Floating Accumulator #1: Mantissa */
extern byte *FACHO; // 98-101 

/* Floating Accumulator #1: Sign */
extern byte *FACSGN; // 102 

/* Pointer: Series Evaluation Constant */
extern byte *SGNFLG; // 103 

/* Floating Accumualtor #1: Overflow Digit */
extern byte *BITS; // 104 

/* Floating Point Acc. #2: Exponent */
extern byte *ARGEXP; // 105 

/* Floating Acc. #2: Mantissa */
extern byte *ARGHO; // 106-109 

/* Floating Acc. #2: Sign */
extern byte *ARGSGN; // 110 

/* Sign Comparison Result: Acc. #1 vs #2 */
extern byte *ARISGN; // 111 

/* Flaoting Acc. #1: Low Order (Rounding) */
extern byte *FACOV; // 112 

/* Pointer: Cassette Buffer */
extern byte *FBUFPT; // 113-114 

/* Subroutine: Get Next Byte of BASIC Text */
extern byte *CHRGET; // 115-138 

/* Entry to Get Same Byte of BASIC Text */
extern byte *CHRGOT; // 121 

/* Pointer: Current Byte of BASIC Text */
extern byte *TXTPTR; // 122-123 

/* Floating RND Function Seed Value */
extern byte *RNDX; // 139-143 

/* Kernal I/O Status Word: ST */
extern byte *STATUS; // 144 

/* Flag: STOP Key / RVS Key */
extern byte *STKEY; // 145 

/* Timing Constant for Tape */
extern byte *SVXT; // 146 

/* Flag: 0=Load, 1=Verify */
extern byte *VERCK2; // 147 

/* Flag: Serial Bus - Output Char, Buffered */
extern byte *C3PO; // 148 

/* Buffered Character for Serial Bus */
extern byte *BSOUR; // 149 

/* Cassettte Sync No. */
extern byte *SYNO; // 150 

/* Temp Data Area */
//byte *_; // 151 

/* No. of Open Files / Index to File Table */
extern byte *LDTND; // 152 

/* Default Input Device (0) */
extern byte *DFLTN; // 153 

/* Default Output (CMD) Device (3) */
extern byte *DFLTO; // 154 

/* Tape Character Parity */
extern byte *PRTY; // 155 

/* Flag: Tape Byte Recieved */
extern byte *DPSW; // 156 

/* Flag: $80=Direct Mode, $00=Program */
extern byte *MSGFLG; // 157 

/* Tape Pass 1 Error Log */
extern byte *PTR1; // 158 

/* Tape Pass 2 Error Log */
extern byte *PTR2; // 159 

/* Real Time Jiffy Clock (Approx 1/60 sec) */
extern byte *TIME; // 160-162 

/* Temp Data Area */
//byte *_; // 163-164 

/* Cassette Sync Countdown */
extern byte *CNTDN; // 165 

/* Pointer: Tape I/O Buffer */
extern byte *BUFPNT; // 166 

/* RS-232 Input Bits / Cassette Temp */
extern byte *INBIT; // 167 

/* RS-232 Input Bit Count / Cassette Temp */
extern byte *BITCI; // 168 

/* RS-232 Flag: Check for Start Bit */
extern byte *RINONE; // 169 

/* RS-232 Input Byte Buffer / Cassette Temp */
extern byte *RIDATA; // 170 

/* Rs-232 Input Parity / Cassette Short Cnt */
extern byte *RIPRTY; // 171 

/* 172-173 Pointer: Tape Buffer / Screen Scrolling */
extern byte *SAL; // 

/* 174-175 Tape End Addresses / End of Program */
extern byte *EAL; // 

/* 176-177 Tape Timing Constants */
extern byte *CMPO; // 

/* 178-179 Pointer: Start of Tape Buffer */
extern byte *TAPE1; // 

/* RS-232 One Bit Count / Cassette Temp */
extern byte *BITTS; // 180 

/* RS-232 Next Bit to Send / Tape EOT Flag */
extern byte *NXTBIT; // 181 

/* RS-232 Out Byte Buffer */
extern byte *RODATA; // 182 

/* Length of Current File Name */
extern byte *FNLEN; // 183 

/* Current Logical File Number */
extern byte *LA; // 184 

/* Current Secondary Address */
extern byte *SA; // 185 

/* Current Device Number */
extern byte *FA; // 186 

/* Pointer: Curertn File Name */
extern byte *FNADR; // 187-188 

/* RS-232 Out Parity / Cassette Temp */
extern byte *ROPRTY; // 189 

/* Cassette Read / Write Block Count */
extern byte *FSBLK; // 190 

/* Serial Word Buffer */
extern byte *MYCH; // 191 

/* Tape Motor Interlock */
extern byte *CAS1; // 192 

/* I/O Start Address */
extern byte *STAL; // 193-194 

/* Tape Load Temps */
extern byte *MEMUSS; // 195-196 

/* Curretn Key Pressed: CHR$(n) 0=No Key */
extern byte *LSTX; // 197 

/* No. of Chars in Keyboard Buffer (Queue) */
extern byte *NDX; // 198 

/* Flag: Print Reverse Chars 1=Yes, 0=No */
extern byte *RVS; // 199 

/* Pointer: End of Logical Line for INPUT */
extern byte *INDX; // 200 

/* Cursor X-Y Position at Start of INPUT */
extern byte *LXSP; // 201-202 

/* Flag: Print Shifted Chars */
extern byte *SFDX; // 203 

/* Cursor Blink Enable: 0=Flash Cursor */
extern byte *BLNSW; // 2-4 

/* Timer: Countdown to Toggle Cursor */
extern byte *BLNCT; // 205 

/* Character Under Cursor */
extern byte *GDBLN; // 206 

/* Flag: Last Cursor Blink On / Off */
extern byte *BLNON; // 207 

/* Flag: INPUT or GET from Keyboard */
extern byte *CRSW; // 208 

/* Pointer: Current Screen Line Address */
extern byte *PNT; // 209-210 

/* Cursor Column on Current Line */
extern byte *PNTR; // 211 

/* Flag: Editor in Quote Mode, $00=NO */
extern byte *QTSW; // 212 

/* Physical Screen Line Length */
extern byte *LNMX; // 213 

/* Current Cursor Physical Line Number */
extern byte *TBLX; // 214 

/* Temp Data Area */
//byte *_; // 215 

/* Flag: Insert Mode >0 = # INSTs */
extern byte *INSRT; // 216 

/* Screen Line Link Table / Editor Temps */
extern byte *LDTB1; // 217-242 

/* Pointer: Current Screen Colour RAM Location */
extern byte *USER; // 243-244 

/* Vector: Keyboard Decode Table */
extern byte *KEYTAB; // 245-246 

/* RS-232 Input Buffer Pointer */
extern byte *RIBUF; // 247-248 

/* RS-232 Output Buffer Pointer */
extern byte *ROBUF; // 249-250 

/* Free Zero Page Space for User Programs */
extern byte *FREKZP; // 251-254 

/* BASIC Temp Data Area */
extern byte *BASZPT; // 255 

/* Micro Processor System Stack Area */
//byte *_; // 256-511 

/* Floating to String Work Area */
//byte *_; // 256-266 

/* Tape Input error Log */
extern byte *BAD; // 256-318 

/* System INPUT Buffer */
extern byte *BUF; // 512-600 

/* KERNAL Table: Active Logical File No's */
extern byte *LAT; // 601-610 

/* KERNAL Table: Device No. for Each File */
extern byte *FAT; // 611-620 

/* KERNAL Table: Second Address Each File */
extern byte *SAT; // 621-630 

/* Keyboard Buffer Queue (FIFO) */
extern byte *KEYD; // 631-640 

/* Pointer: Bottom of Memory for OS */
extern byte *MEMSTR; // 641-642 

/* Pointer: Top of Memory of OS */
extern byte *MEMSIZ2; // 643-644 

/* Flag: Kernal Variable for IEEE Timeout */
extern byte *TIMOUT; // 645 

/* Current Character Colour Code */
extern byte *COLOR; // 646 

/* Background Colour Under Cursor */
extern byte *GDCOL; // 647 

/* Top of Screen Memory (Page) */
extern byte *HIBASE; // 648 

/* Size of Keyboard Buffer */
extern byte *XMAX; // 649 

/* Flag: REPEAT Key Used, $80=Repeat */
extern byte *RPTFLG; // 650 

/* Repeat Speed Counter */
extern byte *KOUNT; // 651 

/* Repeat Delay Counter */
extern byte *DELAY; // 652 

/* Flag: Keyboard SHIFT Key / CTRL Key / C Key */
extern byte *SHFLAG; // 653 

/* Last Keybaord Shift Pattern */
extern byte *LSTSHF; // 654 

/* Vector: Keyboard Table Setup */
extern byte *KEYLOG; // 655-656 

/* Flag: $00=Disable SHIFT Keys, $80=Enable SHIFT Keys */
extern byte *MODE; // 657 

/* Flag: Auto Scroll Down, 0=ON */
extern byte *AUTODN; // 658 

/* RS-232: 6551 Control Register Image */
extern byte *M51CTR; // 659 

/* RS-232: 6551 Command Register Image */
extern byte *M51CDR; // 660 

/* RS-232 Non Standard BPS (Time/2-100) USA */
extern byte *M51AJB; // 661-662 

/* RS-232: 6551 Status Register Image */
extern byte *RSSTAT; // 663 

/* RS-232 Number of Bits Left to Send */
extern byte *BITNUM; // 664 

/* RS-232 Baud Rate: Full Bit Time (micro seconds) */
extern byte *BAUDOF; // 665-666 

/* RS-232 Index to End of Input Buffer */
extern byte *RIDBE; // 667 

/* RS-232 Start of Input Buffer (Page) */
extern byte *RIDBS; // 668 

/* RS-232 Start of Output Buffer (Page) */
extern byte *RODBS; // 669 

/* RS-232 Index to End of Output Buffer */
extern byte *RODBE; // 670 

/* Holds IRQ Vector During Tape I/O */
extern byte *IRQTMP; // 671-672 

/* RS-232 Enables */
extern byte *ENABL; // 673 

/* TOD Sense During Cassette I/O */
//byte *_; // 674 

/* Temp Storage for Cassette Read */
//byte *_; // 675 

/* Temp D1IRQ Indicator for Cassette Read */
//byte *_; // 676 

/* Temp for Line Index */
//byte *_; // 677 

/* PAL / NTSC Flag, 0=NTSC, 1=PAL */
//byte *_; // 678 

/* Unused */
//byte *_; // 679-767 

/* Vector: Print BASIC Error Message */
extern byte *IERROR; // 768-769 

/* Vector: BASIC Warm Start */
extern byte *IMAIN; // 770-771 

/* Vector: Tokenize BASIC Text */
extern byte *ICRNCH; // 772-773 

/* Vector: BASIC Text LIST */
extern byte *IQPLOP; // 774-775 

/* Vector: BASIC Character Dispatch */
extern byte *IGONE; // 776-777 

/* Vector: BASIC Token Evaluation */
extern byte *IEVAL; // 778-779 

/* Storage for 6502 A Register */
extern byte *SAREG; // 780 

/* Storage for 6502 X Register */
extern byte *SXREG; // 781 

/* Storage for 6502 Y Register */
extern byte *SYREG; // 782 

/* Storage for 6502 SP Register */
extern byte *SPREG; // 783 

/* USR Function Jump Instr (4C) */
extern byte *USRPOK; // 784 

/* USR Address Low Byte / High Byte */
extern byte *USRADD; // 785-786 

/* Unused */
//byte *_; // 787 

/* Vector: Hardware IRQ Interrupt */
extern byte *CINV; // 788-789 

/* Vector: BRK Instruction Interrupt */
extern byte *CBINV; // 790-791 

/* Vector: Non Maskable Interrupt */
extern byte *NMINV; // 792-793 

/* KERNAL OPEN Routine Vector */
extern byte *IOPEN; // 794-795 

/* KERNAL CLOSE Routine Vector */
extern byte *ICLOSE; // 796-797 

/* KERNAL CHKIN Routine Vector */
extern byte *ICHKIN; // 798-799 

/* KERNAL CHKOUT Routine Vector */
extern byte *ICKOUT; // 800-801 

/* KERNAL CLRCHN Routine Vector */
extern byte *ICLRCH; // 802-803 

/* KERNAL CHRIN Routine Vector */
extern byte *IBASIN; // 804-805 

/* KERNAL CHROUT Routine Vector */
extern byte *IBSOUT; // 806-807 

/* KERNAL STOP Routine Vector */
extern byte *ISTOP; // 808-809 

/* KERNAL GETIN Routine Vector */
extern byte *IGETIN; // 810-811 

/* KERNAL CLALL Routine Vector */
extern byte *ICLALL; // 812-813 

/* User Defined Vector */
extern byte *USRCMN; // 814-815 

/* KERNAL LOAD Routine Vector */
extern byte *ILOAD; // 816-817 

/* KERNAL SAVE Rountine Vector */
extern byte *ISAVE; // 818-819 

/* Unused */
//byte *_; // 820-827 

/* Tape I/O Buffer */
extern byte *TBUFFR; // 828-1019 

/* Unused */
//byte *_; // 1020-1023 

/* 1024 Byte Screen Memory Area */
extern byte *VICSCN; // 1024-2047 

/* Video Matrix: 25 Lines * 40 Columns */
//byte *_; // 1024-2023 

/* Sprite Data Area */
//byte *_; // 2040-2047 

/* Normal BASIC Program Space */
//byte *_; // 2048-40959 

/* VSP Cartridge Rom - 8192 Bytes */
//byte *_; // 32768-40959 

/* BASIC ROM - 8192 Bytes 8K RAM */
//byte *_; // 40960-49151 

/* RAM - 4096 Bytes */
//byte *_; // 49151-53247 

/* IP / OP Devices & Colour RAM or char gen ROM or RAM */
//byte *_; // 53248-57343 

/* KERNAL ROM - 8192 Bytes 8k RAM */
//byte *_; // 57344-65535 

//byte *_; // 75-96 

/* Floating Point Accumulator #1: Exponent */
extern byte *FACEXP; // 97 

/* Floating Accumulator #1: Mantissa */
extern byte *FACHO; // 98-101 

/* Floating Accumulator #1: Sign */
extern byte *FACSGN; // 102 

/* Pointer: Series Evaluation Constant */
extern byte *SGNFLG; // 103 

/* Floating Accumualtor #1: Overflow Digit */
extern byte *BITS; // 104 

/* Floating Point Acc. #2: Exponent */
extern byte *ARGEXP; // 105 

/* Floating Acc. #2: Mantissa */
extern byte *ARGHO; // 106-109 

/* Floating Acc. #2: Sign */
extern byte *ARGSGN; // 110 

/* Sign Comparison Result: Acc. #1 vs #2 */
extern byte *ARISGN; // 111 

/* Flaoting Acc. #1: Low Order (Rounding) */
extern byte *FACOV; // 112 

/* Pointer: Cassette Buffer */
extern byte *FBUFPT; // 113-114 

/* Subroutine: Get Next Byte of BASIC Text */
extern byte *CHRGET; // 115-138 

/* Entry to Get Same Byte of BASIC Text */
extern byte *CHRGOT; // 121 

/* Pointer: Current Byte of BASIC Text */
extern byte *TXTPTR; // 122-123 

/* Floating RND Function Seed Value */
extern byte *RNDX; // 139-143 

/* Kernal I/O Status Word: ST */
extern byte *STATUS; // 144 

/* Flag: STOP Key / RVS Key */
extern byte *STKEY; // 145 

/* Timing Constant for Tape */
extern byte *SVXT; // 146 

/* Flag: 0=Load, 1=Verify */
extern byte *VERCK2; // 147 

/* Flag: Serial Bus - Output Char, Buffered */
extern byte *C3PO; // 148 

/* Buffered Character for Serial Bus */
extern byte *BSOUR; // 149 

/* Cassettte Sync No. */
extern byte *SYNO; // 150 

/* Temp Data Area */
//byte *_; // 151 

/* No. of Open Files / Index to File Table */
extern byte *LDTND; // 152 

/* Default Input Device (0) */
extern byte *DFLTN; // 153 

/* Default Output (CMD) Device (3) */
extern byte *DFLTO; // 154 

/* Tape Character Parity */
extern byte *PRTY; // 155 

/* Flag: Tape Byte Recieved */
extern byte *DPSW; // 156 

/* Flag: $80=Direct Mode, $00=Program */
extern byte *MSGFLG; // 157 

/* Tape Pass 1 Error Log */
extern byte *PTR1; // 158 

/* Tape Pass 2 Error Log */
extern byte *PTR2; // 159 

/* Real Time Jiffy Clock (Approx 1/60 sec) */
extern byte *TIME; // 160-162 

/* Temp Data Area */
//byte *_; // 163-164 

/* Cassette Sync Countdown */
extern byte *CNTDN; // 165 

/* Pointer: Tape I/O Buffer */
extern byte *BUFPNT; // 166 

/* RS-232 Input Bits / Cassette Temp */
extern byte *INBIT; // 167 

/* RS-232 Input Bit Count / Cassette Temp */
extern byte *BITCI; // 168 

/* RS-232 Flag: Check for Start Bit */
extern byte *RINONE; // 169 

/* RS-232 Input Byte Buffer / Cassette Temp */
extern byte *RIDATA; // 170 

/* Rs-232 Input Parity / Cassette Short Cnt */
extern byte *RIPRTY; // 171 

/* 172-173 Pointer: Tape Buffer / Screen Scrolling */
extern byte *SAL; // 

/* 174-175 Tape End Addresses / End of Program */
extern byte *EAL; // 

/* 176-177 Tape Timing Constants */
extern byte *CMPO; // 

/* 178-179 Pointer: Start of Tape Buffer */
extern byte *TAPE1; // 

/* RS-232 One Bit Count / Cassette Temp */
extern byte *BITTS; // 180 

/* RS-232 Next Bit to Send / Tape EOT Flag */
extern byte *NXTBIT; // 181 

/* RS-232 Out Byte Buffer */
extern byte *RODATA; // 182 

/* Length of Current File Name */
extern byte *FNLEN; // 183 

/* Current Logical File Number */
extern byte *LA; // 184 

/* Current Secondary Address */
extern byte *SA; // 185 

/* Current Device Number */
extern byte *FA; // 186 

/* Pointer: Curertn File Name */
extern byte *FNADR; // 187-188 

/* RS-232 Out Parity / Cassette Temp */
extern byte *ROPRTY; // 189 

/* Cassette Read / Write Block Count */
extern byte *FSBLK; // 190 

/* Serial Word Buffer */
extern byte *MYCH; // 191 

/* Tape Motor Interlock */
extern byte *CAS1; // 192 

/* I/O Start Address */
extern byte *STAL; // 193-194 

/* Tape Load Temps */
extern byte *MEMUSS; // 195-196 

/* Curretn Key Pressed: CHR$(n) 0=No Key */
extern byte *LSTX; // 197 

/* No. of Chars in Keyboard Buffer (Queue) */
extern byte *NDX; // 198 

/* Flag: Print Reverse Chars 1=Yes, 0=No */
extern byte *RVS; // 199 

/* Pointer: End of Logical Line for INPUT */
extern byte *INDX; // 200 

/* Cursor X-Y Position at Start of INPUT */
extern byte *LXSP; // 201-202 

/* Flag: Print Shifted Chars */
extern byte *SFDX; // 203 

/* Cursor Blink Enable: 0=Flash Cursor */
extern byte *BLNSW; // 2-4 

/* Timer: Countdown to Toggle Cursor */
extern byte *BLNCT; // 205 

/* Character Under Cursor */
extern byte *GDBLN; // 206 

/* Flag: Last Cursor Blink On / Off */
extern byte *BLNON; // 207 

/* Flag: INPUT or GET from Keyboard */
extern byte *CRSW; // 208 

/* Pointer: Current Screen Line Address */
extern byte *PNT; // 209-210 

/* Cursor Column on Current Line */
extern byte *PNTR; // 211 

/* Flag: Editor in Quote Mode, $00=NO */
extern byte *QTSW; // 212 

/* Physical Screen Line Length */
extern byte *LNMX; // 213 

/* Current Cursor Physical Line Number */
extern byte *TBLX; // 214 

/* Temp Data Area */
//byte *_; // 215 

/* Flag: Insert Mode >0 = # INSTs */
extern byte *INSRT; // 216 

/* Screen Line Link Table / Editor Temps */
extern byte *LDTB1; // 217-242 

/* Pointer: Current Screen Colour RAM Location */
extern byte *USER; // 243-244 

/* Vector: Keyboard Decode Table */
extern byte *KEYTAB; // 245-246 

/* RS-232 Input Buffer Pointer */
extern byte *RIBUF; // 247-248 

/* RS-232 Output Buffer Pointer */
extern byte *ROBUF; // 249-250 

/* Free Zero Page Space for User Programs */
extern byte *FREKZP; // 251-254 

/* BASIC Temp Data Area */
extern byte *BASZPT; // 255 

/* Micro Processor System Stack Area */
//byte *_; // 256-511 

/* Floating to String Work Area */
//byte *_; // 256-266 

/* Tape Input error Log */
extern byte *BAD; // 256-318 

/* System INPUT Buffer */
extern byte *BUF; // 512-600 

/* KERNAL Table: Active Logical File No's */
extern byte *LAT; // 601-610 

/* KERNAL Table: Device No. for Each File */
extern byte *FAT; // 611-620 

/* KERNAL Table: Second Address Each File */
extern byte *SAT; // 621-630 

/* Keyboard Buffer Queue (FIFO) */
extern byte *KEYD; // 631-640 

/* Pointer: Bottom of Memory for OS */
extern byte *MEMSTR; // 641-642 

/* Pointer: Top of Memory of OS */
extern byte *MEMSIZ2; // 643-644 

/* Flag: Kernal Variable for IEEE Timeout */
extern byte *TIMOUT; // 645 

/* Current Character Colour Code */
extern byte *COLOR; // 646 

/* Background Colour Under Cursor */
extern byte *GDCOL; // 647 

/* Top of Screen Memory (Page) */
extern byte *HIBASE; // 648 

/* Size of Keyboard Buffer */
extern byte *XMAX; // 649 

/* Flag: REPEAT Key Used, $80=Repeat */
extern byte *RPTFLG; // 650 

/* Repeat Speed Counter */
extern byte *KOUNT; // 651 

/* Repeat Delay Counter */
extern byte *DELAY; // 652 

/* Flag: Keyboard SHIFT Key / CTRL Key / C Key */
extern byte *SHFLAG; // 653 

/* Last Keybaord Shift Pattern */
extern byte *LSTSHF; // 654 

/* Vector: Keyboard Table Setup */
extern byte *KEYLOG; // 655-656 

/* Flag: $00=Disable SHIFT Keys, $80=Enable SHIFT Keys */
extern byte *MODE; // 657 

/* Flag: Auto Scroll Down, 0=ON */
extern byte *AUTODN; // 658 

/* RS-232: 6551 Control Register Image */
extern byte *M51CTR; // 659 

/* RS-232: 6551 Command Register Image */
extern byte *M51CDR; // 660 

/* RS-232 Non Standard BPS (Time/2-100) USA */
extern byte *M51AJB; // 661-662 

/* RS-232: 6551 Status Register Image */
extern byte *RSSTAT; // 663 

/* RS-232 Number of Bits Left to Send */
extern byte *BITNUM; // 664 

/* RS-232 Baud Rate: Full Bit Time (micro seconds) */
extern byte *BAUDOF; // 665-666 

/* RS-232 Index to End of Input Buffer */
extern byte *RIDBE; // 667 

/* RS-232 Start of Input Buffer (Page) */
extern byte *RIDBS; // 668 

/* RS-232 Start of Output Buffer (Page) */
extern byte *RODBS; // 669 

/* RS-232 Index to End of Output Buffer */
extern byte *RODBE; // 670 

/* Holds IRQ Vector During Tape I/O */
extern byte *IRQTMP; // 671-672 

/* RS-232 Enables */
extern byte *ENABL; // 673 

/* TOD Sense During Cassette I/O */
//byte *_; // 674 

/* Temp Storage for Cassette Read */
//byte *_; // 675 

/* Temp D1IRQ Indicator for Cassette Read */
//byte *_; // 676 

/* Temp for Line Index */
//byte *_; // 677 

/* PAL / NTSC Flag, 0=NTSC, 1=PAL */
//byte *_; // 678 

/* Unused */
//byte *_; // 679-767 

/* Vector: Print BASIC Error Message */
extern byte *IERROR; // 768-769 

/* Vector: BASIC Warm Start */
extern byte *IMAIN; // 770-771 

/* Vector: Tokenize BASIC Text */
extern byte *ICRNCH; // 772-773 

/* Vector: BASIC Text LIST */
extern byte *IQPLOP; // 774-775 

/* Vector: BASIC Character Dispatch */
extern byte *IGONE; // 776-777 

/* Vector: BASIC Token Evaluation */
extern byte *IEVAL; // 778-779 

/* Storage for 6502 A Register */
extern byte *SAREG; // 780 

/* Storage for 6502 X Register */
extern byte *SXREG; // 781 

/* Storage for 6502 Y Register */
extern byte *SYREG; // 782 

/* Storage for 6502 SP Register */
extern byte *SPREG; // 783 

/* USR Function Jump Instr (4C) */
extern byte *USRPOK; // 784 

/* USR Address Low Byte / High Byte */
extern byte *USRADD; // 785-786 

/* Unused */
//byte *_; // 787 

/* Vector: Hardware IRQ Interrupt */
extern byte *CINV; // 788-789 

/* Vector: BRK Instruction Interrupt */
extern byte *CBINV; // 790-791 

/* Vector: Non Maskable Interrupt */
extern byte *NMINV; // 792-793 

/* KERNAL OPEN Routine Vector */
extern byte *IOPEN; // 794-795 

/* KERNAL CLOSE Routine Vector */
extern byte *ICLOSE; // 796-797 

/* KERNAL CHKIN Routine Vector */
extern byte *ICHKIN; // 798-799 

/* KERNAL CHKOUT Routine Vector */
extern byte *ICKOUT; // 800-801 

/* KERNAL CLRCHN Routine Vector */
extern byte *ICLRCH; // 802-803 

/* KERNAL CHRIN Routine Vector */
extern byte *IBASIN; // 804-805 

/* KERNAL CHROUT Routine Vector */
extern byte *IBSOUT; // 806-807 

/* KERNAL STOP Routine Vector */
extern byte *ISTOP; // 808-809 

/* KERNAL GETIN Routine Vector */
extern byte *IGETIN; // 810-811 

/* KERNAL CLALL Routine Vector */
extern byte *ICLALL; // 812-813 

/* User Defined Vector */
extern byte *USRCMN; // 814-815 

/* KERNAL LOAD Routine Vector */
extern byte *ILOAD; // 816-817 

/* KERNAL SAVE Rountine Vector */
extern byte *ISAVE; // 818-819 

/* Unused */
//byte *_; // 820-827 

/* Tape I/O Buffer */
extern byte *TBUFFR; // 828-1019 

/* Unused */
//byte *_; // 1020-1023 

/* 1024 Byte Screen Memory Area */
extern byte *VICSCN; // 1024-2047 

/* Video Matrix: 25 Lines * 40 Columns */
//byte *_; // 1024-2023 

/* Sprite Data Area */
//byte *_; // 2040-2047 

/* Normal BASIC Program Space */
//byte *_; // 2048-40959 

/* VSP Cartridge Rom - 8192 Bytes */
//byte *_; // 32768-40959 

/* BASIC ROM - 8192 Bytes 8K RAM */
//byte *_; // 40960-49151 

/* RAM - 4096 Bytes */
//byte *_; // 49151-53247 

/* IP / OP Devices & Colour RAM or char gen ROM or RAM */
//byte *_; // 53248-57343 

/* KERNAL ROM - 8192 Bytes 8k RAM */
//byte *_; // 57344-65535 

void initRam();

#endif /*RAM_H_*/
