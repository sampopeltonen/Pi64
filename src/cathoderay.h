
#ifndef CATHODERAY_H_
#define CATHODERAY_H_

#include "types.h"

word VICbgColor[4];

#define X_MIN 80
#define X_MAX 474
#define Y_MIN 25
#define Y_MAX 280

void cathoderay_init();

void drawCharacterOctet(unsigned int x, unsigned int y, byte data, unsigned int foreColor);

void drawCharacterOctetETM(unsigned int x, unsigned int y, byte data, unsigned int foreColor, unsigned int bgColor);

void drawCharacterOctetMTM(unsigned int x, unsigned int y, byte data, unsigned int foreColor);

void drawCharacterOctetSBM(unsigned int x, unsigned int y, byte data, unsigned int color0, unsigned int color1);

void drawCharacterOctetMBM(unsigned int x, unsigned int y, byte data, unsigned int color0, unsigned int color1, unsigned int color2);

inline void drawBorderOctet(unsigned int x, unsigned int y, unsigned int color);

#endif
