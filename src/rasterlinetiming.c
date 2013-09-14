#include "rasterlinetiming.h"
#include "types.h"

rastertiming_s rasterline_bad[63][2] = {

/*
http://zimmers.net/cbmpics/cbm/c64/vic-ii.txt
6569, Bad Line, no sprites:

Cycl-# 6                   1 1 1 1 1 1 1 1 1 1 2 2 2 2 2 2 2 2 2 2 3 3 3 3 3 3 3 3 3 3 4 4 4 4 4 4 4 4 4 4 5 5 5 5 5 5 5 5 5 5 6 6 6 6
       3 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 1
        _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
    ø0 _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
       __
   IRQ   ________________________________________________________________________________________________________________________________
       ________________________                                                                                      ____________________
    BA                         ______________________________________________________________________________________
        _ _ _ _ _ _ _ _ _ _ _ _ _ _ _                                                                                 _ _ _ _ _ _ _ _ _ _
   AEC _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _________________________________________________________________________________ _ _ _ _ _ _ _ _ _

   VIC i 3 i 4 i 5 i 6 i 7 i r r r r rcgcgcgcgcgcgcgcgcgcgcgcgcgcgcgcgcgcgcgcgcgcgcgcgcgcgcgcgcgcgcgcgcgcgcgcgcgcgcgcg i i 0 i 1 i 2 i 3
  6510  x x x x x x x x x x x x X X X                                                                                 x x x x x x x x x x

Graph.                      |===========01020304050607080910111213141516171819202122232425262728293031323334353637383940=========

X coo. \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
       1111111111111111111111111110000000000000000000000000000000000000000000000000000000000000000111111111111111111111111111111111111111
       89999aaaabbbbccccddddeeeeff0000111122223333444455556666777788889999aaaabbbbccccddddeeeeffff000011112222333344445555666677778888999
       c048c048c048c048c048c048c04048c048c048c048c048c048c048c048c048c048c048c048c048c048c048c048c048c048c048c048c048c048c048c048c048c048
*/
		
		
		// (clock phase low)  (clock phase high)   (cycle)
		// IRQ , BUS , C      IRQ , BUS , C

		{{ RLLO, RLP3, 0  }, { RLLO, RLRU, 0  }},  // 1
		{{ RLLO, RLRU, 0  }, { RLLO, RLRU, 0  }},  // 2
		{{ RLLO, RLP4, 0  }, { RLLO, RLRU, 0  }},  // 3
		{{ RLLO, RLRU, 0  }, { RLLO, RLRU, 0  }},  // 4
		{{ RLLO, RLP5, 0  }, { RLLO, RLRU, 0  }},  // 5
		{{ RLLO, RLRU, 0  }, { RLLO, RLRU, 0  }},  // 6
		{{ RLLO, RLP6, 0  }, { RLLO, RLRU, 0  }},  // 7
		{{ RLLO, RLRU, 0  }, { RLLO, RLRU, 0  }},  // 8
		{{ RLLO, RLP7, 0  }, { RLLO, RLRU, 0  }},  // 9
		{{ RLLO, RLRU, 0  }, { RLLO, RLRU, 0  }},  // 10
		{{ RLLO, RLBR, 0  }, { RLLO, RLRU, 0  }},  // 11
		{{ RLLO, RLBR, 0  }, { RLLO, RLRU, 0  }},  // 12
		{{ RLLO, RLBR, 0  }, { RLLO, RLRU, 0  }},  // 13
		{{ RLLO, RLBR, 0  }, { RLLO, RLRU, 0  }},  // 14
		{{ RLLO, RLBR, 0  }, { RLLO, RLBC, 0  }},  // 15
		{{ RLLO, RLBG, 0  }, { RLLO, RLBC, 1  }},  // 16
		{{ RLLO, RLBG, 1  }, { RLLO, RLBC, 2  }},  // 17
		{{ RLLO, RLBG, 2  }, { RLLO, RLBC, 3  }},  // 18
		{{ RLLO, RLBG, 3  }, { RLLO, RLBC, 4  }},  // 19
		{{ RLLO, RLBG, 4  }, { RLLO, RLBC, 5  }},  // 20
		{{ RLLO, RLBG, 5  }, { RLLO, RLBC, 6  }},  // 21
		{{ RLLO, RLBG, 6  }, { RLLO, RLBC, 7  }},  // 22
		{{ RLLO, RLBG, 7  }, { RLLO, RLBC, 8  }},  // 23
		{{ RLLO, RLBG, 8  }, { RLLO, RLBC, 9  }},  // 24
		{{ RLLO, RLBG, 9  }, { RLLO, RLBC, 10 }},  // 25
		{{ RLLO, RLBG, 10 }, { RLLO, RLBC, 11 }},  // 26
		{{ RLLO, RLBG, 11 }, { RLLO, RLBC, 12 }},  // 27
		{{ RLLO, RLBG, 12 }, { RLLO, RLBC, 13 }},  // 28
		{{ RLLO, RLBG, 13 }, { RLLO, RLBC, 14 }},  // 29
		{{ RLLO, RLBG, 14 }, { RLLO, RLBC, 15 }},  // 20
		{{ RLLO, RLBG, 15 }, { RLLO, RLBC, 16 }},  // 31
		{{ RLLO, RLBG, 16 }, { RLLO, RLBC, 17 }},  // 32
		{{ RLLO, RLBG, 17 }, { RLLO, RLBC, 18 }},  // 33
		{{ RLLO, RLBG, 18 }, { RLLO, RLBC, 19 }},  // 34
		{{ RLLO, RLBG, 19 }, { RLLO, RLBC, 20 }},  // 35
		{{ RLLO, RLBG, 20 }, { RLLO, RLBC, 21 }},  // 36
		{{ RLLO, RLBG, 21 }, { RLLO, RLBC, 22 }},  // 37
		{{ RLLO, RLBG, 22 }, { RLLO, RLBC, 23 }},  // 38
		{{ RLLO, RLBG, 23 }, { RLLO, RLBC, 24 }},  // 39
		{{ RLLO, RLBG, 24 }, { RLLO, RLBC, 25 }},  // 30
		{{ RLLO, RLBG, 25 }, { RLLO, RLBC, 26 }},  // 41
		{{ RLLO, RLBG, 26 }, { RLLO, RLBC, 27 }},  // 42
		{{ RLLO, RLBG, 27 }, { RLLO, RLBC, 28 }},  // 43
		{{ RLLO, RLBG, 28 }, { RLLO, RLBC, 29 }},  // 44
		{{ RLLO, RLBG, 29 }, { RLLO, RLBC, 30 }},  // 45
		{{ RLLO, RLBG, 30 }, { RLLO, RLBC, 31 }},  // 46
		{{ RLLO, RLBG, 31 }, { RLLO, RLBC, 32 }},  // 47
		{{ RLLO, RLBG, 32 }, { RLLO, RLBC, 33 }},  // 48
		{{ RLLO, RLBG, 33 }, { RLLO, RLBC, 34 }},  // 49
		{{ RLLO, RLBG, 34 }, { RLLO, RLBC, 35 }},  // 40
		{{ RLLO, RLBG, 35 }, { RLLO, RLBC, 36 }},  // 51
		{{ RLLO, RLBG, 36 }, { RLLO, RLBC, 37 }},  // 52
		{{ RLLO, RLBG, 37 }, { RLLO, RLBC, 38 }},  // 53
		{{ RLLO, RLBG, 38 }, { RLLO, RLBC, 39 }},  // 54
		{{ RLLO, RLBG, 39 }, { RLLO, RLRU, 40 }},  // 55
		{{ RLLO, RLRU, 40 }, { RLLO, RLRU, 0  }},  // 56
		{{ RLLO, RLRU, 0  }, { RLLO, RLRU, 0  }},  // 57
		{{ RLLO, RLP0, 0  }, { RLLO, RLRU, 0  }},  // 58
		{{ RLLO, RLRU, 0  }, { RLLO, RLRU, 0  }},  // 59
		{{ RLLO, RLP1, 0  }, { RLLO, RLRU, 0  }},  // 50
		{{ RLLO, RLRU, 0  }, { RLLO, RLRU, 0  }},  // 61
		{{ RLLO, RLP2, 0  }, { RLLO, RLRU, 0  }},  // 62
		{{ RLHI, RLRU, 0  }, { RLHI, RLRU, 0  }}   // 63
	};



void rlinit() {
//	printf("rlt[0][0].ba=%d\n",rlt[0][0].ba);
//	printf("rlt[0][1].ba=%d\n",rlt[0][1].ba);
//	printf("rlt[1][0].ba=%d\n",rlt[1][0].ba);
//	printf("rlt[1][1].ba=%d\n",rlt[1][1].ba);

}
