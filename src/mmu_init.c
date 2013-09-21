/*
 * The code in this file is taken from David Welch's twain example.
 * 
 * (except mmu mapping init for frame buffer memory location).
 */

#include "stdlibtools.h"

extern void PUT32 ( unsigned int, unsigned int );
extern unsigned int GET32 ( unsigned int );
extern void start_mmu ( unsigned int, unsigned int );

#define MMUTABLEBASE 0x00200000
#define MMUTABLESIZE (0x8000)
#define COARSE_TABLE_WORDS (1<<((19-12)+1))
#define TOP_LEVEL_WORDS (1<<((31-20)+1))


unsigned int nextfree;

unsigned int next_coarse_offset ( unsigned int x )
{
    unsigned int mask;

    mask=(~0)<<(10-2);
    mask=~mask;
    while(x&mask) x++; //lazy brute force
    return(x);
}

unsigned int add_one ( unsigned int add, unsigned int flags )
{
    unsigned int ra;
    unsigned int rb;
    unsigned int rc;

    //bits 31:20 index into the top level table
    ra=add>>20;
    rc=MMUTABLEBASE+(ra<<2);
    rb=GET32(rc);
    if(rb)
    {
        printf1("Address %x already allocated",add);
        printf1("rc=%x", rc);
        printf1("rb=%x", rb);
        return(1);
    }
    add=ra<<20;

    rb=next_coarse_offset(nextfree);
    rc=rb+COARSE_TABLE_WORDS;
    if(rc>=MMUTABLESIZE)
    {
        printf("Not enough room");
        return(1);
    }
    nextfree=rc;
    //use course page table pointer on top level table
    PUT32(MMUTABLEBASE+(ra<<2),(MMUTABLEBASE+(rb<<2))|0x00000001);
    //fill in the course page table. with small entries
    for(ra=0;ra<COARSE_TABLE_WORDS;ra++)
    {
        PUT32(MMUTABLEBASE+(rb<<2)+(ra<<2),(add+(ra<<12))|0x00000032|flags);
    }
    return(0);
}

void init_mmu(void) {
        //printf("Starting MMU...");            
        for(nextfree=0;nextfree<TOP_LEVEL_WORDS;nextfree++) PUT32(MMUTABLEBASE+(nextfree<<2),0);
        //ram used by the stack and the program
        if(add_one(0x00000000,0x0000|8|4)) exit(1);
        if(add_one(0x00100000,0x0000|8|4)) exit(1);
        if(add_one(0x00200000,0x0000|8|4)) exit(1);
        //Memory mapped I/O used by the uart, etc, not cached
        if(add_one(0x20000000,0x0000)) exit(1);
        if(add_one(0x20100000,0x0000)) exit(1);
        if(add_one(0x20200000,0x0000)) exit(1);

        // framebuffer area seems to be here, not cached
        if(add_one(0x48000000,0x0000)) exit(1);
        if(add_one(0x48100000,0x0000)) exit(1);
        if(add_one(0x48200000,0x0000)) exit(1);

        // #ifdef DCACHE
        start_mmu(MMUTABLEBASE,0x00800005);
        //printf("MMU started");
}

