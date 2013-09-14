#include "testing.h"

#include <stdio.h>
#include <stdlib.h>
#include "ram.h"

byte header[26];
byte loadAddress[2];
byte program[3436];

word loadTestProgram() {
	FILE *file;
	if((file = fopen("test/_START.P00", "r")) == NULL) {
		printf("cannot open test program file\n");
		exit(1);
	}
	fread(&header, 26, 1, file);
	fread(&loadAddress, 2, 1, file);
	fread(&program, 3436, 1, file);
	fclose(file);
	
	printf("%x%x\n", loadAddress[1],loadAddress[0]);
	word addr = (loadAddress[1] << 8);
	addr += loadAddress[0];
	printf("%x\n", addr);
	
	int i;
	for(i=0; i<3436; i++) {
		memWriteByte(addr+i, program[i]);
		//printf("[%x] %x\n", addr+i, program[i]);
	}
	
	return(addr);
}

