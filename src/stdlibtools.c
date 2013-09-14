
#include "stdlibtools.h"

#define STR_MAX_LEN 100

extern void DrawString (const void* string, unsigned int, unsigned int, unsigned int);
extern unsigned int FormatString(const char* format, unsigned int length, char* dest, ...);
extern void setForeColour (unsigned int);
extern void DrawLine (unsigned int, unsigned int, unsigned int, unsigned int);
extern void hang();

char stringBuf[STR_MAX_LEN];
unsigned int vpos = 10;

unsigned int slen(const char* str) {
        int i=0;
        while(i<STR_MAX_LEN) {
                if(str[i++]==0) return i;
        }
        return i-1;
}

void outputBuffer(unsigned int len) {
	setForeColour(0x0000); 
	int i=0;
	while(i<16) {
		DrawLine(600, vpos+i, 900, vpos+i);
		i++;
	}
	setForeColour(0x8888);
	DrawString(stringBuf, len, 600, vpos);
	vpos+=16;
	if(vpos>650) vpos=10;
}

// TODO: stdlib replacement stuff
int printf(const char * string) {
        unsigned int len = FormatString(string, slen(string), stringBuf);
	outputBuffer(len);
}

int printf1(const char * string, int a1) {
        unsigned int len = FormatString(string, slen(string), stringBuf, a1);
	outputBuffer(len);
}
int printf2(const char * string, int a1, int a2) {
        unsigned int len = FormatString(string, slen(string), stringBuf, a1, a2);
	outputBuffer(len);
}

int exit(int exitCode) {
        hang();
}
int puts(int c) {
        //nothign
}


