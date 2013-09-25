
extern void PUT32(unsigned int, unsigned int);
extern unsigned int GET32(unsigned int);

extern unsigned int bss_start;
extern unsigned int bss_end;
extern unsigned int data_rom_start;
extern unsigned int data_start;
extern unsigned int data_end;

void prepGlobals() {
	unsigned int ra;
    	//unsigned int rb;
	for(ra=bss_start;ra<bss_end;ra+=4) PUT32(ra,0);
	//for(ra=data_start,rb=data_rom_start;ra<data_end;ra+=4,rb+=4) PUT32(ra,GET32(rb));
}
