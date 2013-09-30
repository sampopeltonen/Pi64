#include "autotestkeyboard.h"
#include "stdlibtools.h"

byte selectedColumn;

byte keyDown=1;

byte keyCols[256];
byte keyRows[256];

int runIndex = 0;
int finished = 0;

int shifted = 0;

//const char text[] = "abcdefghijklm\\nopqrst\\uvxyz 1234567890";
//const char text[] = "print\\2\\pi64 rules ok\\2\\  \n\n" "20 i=5 \n\n" "run\n";
/*
const char* text = 	"print 1+3\n"
			"10 i=64\n"
			"20 print i\n"
			"run\n";
*/

const char* text =
"10 i=0\n"                                  
"20 print\\2\\pi64 rules ok\\12\\\n"             
"30 i=i+1\n"                                
"40 ifi\\,\\10thengoto20\n"                     
"run\n";

void autoKeybSelectColumn(byte column) {
	selectedColumn = column;
}

byte autoKeybReadRow() {
	if(text[runIndex]=='\\') {
		shifted=(shifted==0 ? 1 : 0);
		runIndex++;
	}
	
	if(keyDown>20 && !finished && shifted && (selectedColumn==0 || selectedColumn==keyCols['\\'])) {
		return keyRows['\\'];
	}

	if(keyDown++==0) runIndex++;

	if(keyDown>40) keyDown=0;

	if(keyDown>20 && !finished && (selectedColumn==0 || selectedColumn==keyCols[text[runIndex]])) {
		if(text[runIndex]==0)
			finished=1;
		else {
			return keyRows[text[runIndex]];
		}
	}

	return 0xff;	
}

void autoKeybInit() {
	keyCols['\n'] = 0b11111110;

	keyCols['*'] = 0b10111111;
	keyCols[';'] = 0b10111111;
	keyCols['/'] = 0b10111111;
	keyCols['='] = 0b10111111;

	keyCols['+'] = 0b11011111;
	keyCols['p'] = 0b11011111;
	keyCols['l'] = 0b11011111;
	keyCols[','] = 0b11011111;
	keyCols['.'] = 0b11011111;
	keyCols[':'] = 0b11011111;
	keyCols['@'] = 0b11011111;
	keyCols['-'] = 0b11011111;

	keyCols['9'] = 0b11101111;
	keyCols['i'] = 0b11101111;
	keyCols['j'] = 0b11101111;
	keyCols['n'] = 0b11101111;
	keyCols['m'] = 0b11101111;
	keyCols['k'] = 0b11101111;
	keyCols['o'] = 0b11101111;
	keyCols['0'] = 0b11101111;

	keyCols['7'] = 0b11110111;
	keyCols['y'] = 0b11110111;
	keyCols['g'] = 0b11110111;
	keyCols['v'] = 0b11110111;
	keyCols['b'] = 0b11110111;
	keyCols['h'] = 0b11110111;
	keyCols['u'] = 0b11110111;
	keyCols['8'] = 0b11110111;

	keyCols['5'] = 0b11111011;
	keyCols['r'] = 0b11111011;
	keyCols['d'] = 0b11111011;
	keyCols['x'] = 0b11111011;
	keyCols['c'] = 0b11111011;
	keyCols['f'] = 0b11111011;
	keyCols['t'] = 0b11111011;
	keyCols['6'] = 0b11111011;

	keyCols['3'] = 0b11111101;
	keyCols['w'] = 0b11111101;
	keyCols['a'] = 0b11111101;
	keyCols['z'] = 0b11111101;
	keyCols['s'] = 0b11111101;
	keyCols['e'] = 0b11111101;
	keyCols['4'] = 0b11111101;
	keyCols['\\'] = 0b11111101;   // let this be left shift

	keyCols['1'] = 0b01111111;
	keyCols[' '] = 0b01111111;
	keyCols['q'] = 0b01111111;
	keyCols['2'] = 0b01111111;


	keyRows['2'] = 0b11110111;
	keyRows['4'] = 0b11110111;
	keyRows['6'] = 0b11110111;
	keyRows['8'] = 0b11110111;
	keyRows['0'] = 0b11110111;
	keyRows['-'] = 0b11110111;

	keyRows['q'] = 0b10111111;
	keyRows['e'] = 0b10111111;
	keyRows['t'] = 0b10111111;
	keyRows['u'] = 0b10111111;
	keyRows['o'] = 0b10111111;
	keyRows['@'] = 0b10111111;

	keyRows['s'] = 0b11011111;
	keyRows['f'] = 0b11011111;
	keyRows['h'] = 0b11011111;
	keyRows['k'] = 0b11011111;
	keyRows[':'] = 0b11011111;
	keyRows['='] = 0b11011111;

	keyRows[' '] = 0b11101111;
	keyRows['z'] = 0b11101111;
	keyRows['c'] = 0b11101111;
	keyRows['b'] = 0b11101111;
	keyRows['m'] = 0b11101111;
	keyRows['.'] = 0b11101111;

	keyRows['\\'] = 0b01111111; // let this be left shift	
	keyRows['x'] = 0b01111111;
	keyRows['v'] = 0b01111111;
	keyRows['n'] = 0b01111111;
	keyRows[','] = 0b01111111;
	keyRows['/'] = 0b01111111;

	keyRows['a'] = 0b11111011;
	keyRows['d'] = 0b11111011;
	keyRows['g'] = 0b11111011;
	keyRows['j'] = 0b11111011;
	keyRows['l'] = 0b11111011;
	keyRows[';'] = 0b11111011;

	keyRows['w'] = 0b11111101;
	keyRows['r'] = 0b11111101;
	keyRows['y'] = 0b11111101;
	keyRows['i'] = 0b11111101;
	keyRows['p'] = 0b11111101;
	keyRows['*'] = 0b11111101;
	keyRows['\n'] = 0b11111101;

	keyRows['1'] = 0b11111110;
	keyRows['3'] = 0b11111110;
	keyRows['5'] = 0b11111110;
	keyRows['7'] = 0b11111110;
	keyRows['9'] = 0b11111110;
	keyRows['+'] = 0b11111110;
}
