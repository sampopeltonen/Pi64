
#include "stdlibtools.h"

#define STR_MAX_LEN 100

extern void DrawString (const void* string, unsigned int, unsigned int, unsigned int);
extern unsigned int FormatString(const char* format, unsigned int length, char* dest, ...);
extern void setForeColour (unsigned int);
extern void DrawLine (unsigned int, unsigned int, unsigned int, unsigned int);
extern void ledOn();
extern void hang();

char stringBuf[STR_MAX_LEN];
unsigned int vpos;

void init_stdlibtools() {
	vpos = 10;
}


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
		DrawLine(320, vpos+i, 1023, vpos+i);
		i++;
	}
	setForeColour(0x8888);
	DrawString(stringBuf, len, 320, vpos);
	vpos+=16;
	//if(vpos>460) vpos=10;
	if(vpos>750) vpos=10;
}

// TODO: stdlib replacement stuff
void printf(const char * string) {
        unsigned int len = FormatString(string, slen(string), stringBuf);
	outputBuffer(len);
}

void printf1(const char * string, int a1) {
        unsigned int len = FormatString(string, slen(string), stringBuf, a1);
	outputBuffer(len);
}
void printf2(const char * string, int a1, int a2) {
        unsigned int len = FormatString(string, slen(string), stringBuf, a1, a2);
	outputBuffer(len);
}
void printf3(const char * string, int a1, int a2, int a3) {
        unsigned int len = FormatString(string, slen(string), stringBuf, a1, a2, a3);
	outputBuffer(len);
}

void LogPrint(char* message, unsigned int messageLength) {
        unsigned int len = FormatString(message, messageLength, stringBuf);
	outputBuffer(len);
	//printf2("%s", (int)message, messageLength);
        //printf1("csud msg length=%d",messageLength);
}


void exit(int exitCode) {
	if(exitCode>0) ledOn();
	printf1("Exit with code %d. Halting.",exitCode);
        hang();
}
void puts(int c) {
        //nothign
}


