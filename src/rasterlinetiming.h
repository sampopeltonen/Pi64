#ifndef RASTERLINETIMING_H_
#define RASTERLINETIMING_H_

#include "types.h"

#define RLLO 0
#define RLHI 1

#define RLBR 10    // VIC DRAM refresh
#define RLBC 11    // VIC C-access (video matrix and color RAM access)
#define RLBG 12    // VIC G-access (char generator or bitmap access)

#define RLP0 0    // VIC P-access (sprite data pointer access 0-7)
#define RLP1 1    // VIC P-access (sprite data pointer access 0-7)
#define RLP2 2    // VIC P-access (sprite data pointer access 0-7)
#define RLP3 3    // VIC P-access (sprite data pointer access 0-7)
#define RLP4 4    // VIC P-access (sprite data pointer access 0-7)
#define RLP5 5    // VIC P-access (sprite data pointer access 0-7)
#define RLP6 6    // VIC P-access (sprite data pointer access 0-7)
#define RLP7 7    // VIC P-access (sprite data pointer access 0-7)
#define RLRU 99    // VIC decides the action in runtime (sprite data fetch or vic idle+processor)


typedef struct {
	byte irq;
	byte busUsage;
	byte col;
} rastertiming_s;

rastertiming_s rasterline_bad[63][2];

	
void rlinit();

#endif /*RASTERLINETIMING_H_*/
