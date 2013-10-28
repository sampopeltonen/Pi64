
#include "cathoderay.h"


extern uint32_t* graphicsAddress;

uint32_t gwidth;
uint16_t* gaddress;


void cathoderay_init() {
        gwidth = graphicsAddress[0];
        gaddress = (uint16_t*)graphicsAddress[8];
}


void drawCharacterOctet(unsigned int x, unsigned int y, byte data, unsigned int foreColor) {
        if(y>=Y_MIN && y<Y_MAX) {
                x-=X_MIN;
                y=y-Y_MIN+100;

                uint16_t* address = gaddress + (y*gwidth+x);
                byte mask = 0b10000000;
                while(mask>0) {
                        *address = (data&mask) ? foreColor : VICbgColor[0];
                        address += 1;
                        mask = (mask >>1);
                }
        }
}
void drawCharacterOctetETM(unsigned int x, unsigned int y, byte data, unsigned int foreColor, unsigned int bgColor) {
        if(y>=Y_MIN && y<Y_MAX) {
                x-=X_MIN;
                y=y-Y_MIN+100;

                uint16_t* address = gaddress + (y*gwidth+x);
                byte mask = 0b10000000;
                while(mask>0) {
                        *address = (data&mask) ? foreColor : VICbgColor[bgColor];
                        address += 1;
                        mask = (mask >>1);
                }
        }
}

void drawCharacterOctetMTM(unsigned int x, unsigned int y, byte data, unsigned int foreColor) {
        if(y>=Y_MIN && y<Y_MAX) {
                x-=X_MIN;
                y=y-Y_MIN+100;

                uint16_t* address = gaddress + (y*gwidth+x);
                byte mask = 0b11000000;
                int i;
                for(i=4; i>0; i--) {
                        switch(data&mask) {
                                case 0:
                                        *(address++) = VICbgColor[0];
                                        *(address++) = VICbgColor[0];
                                        break;
                                case 64:
                                        *(address++) = VICbgColor[1];
                                        *(address++) = VICbgColor[1];
                                        break;
                                case 128:
                                        *(address++) = VICbgColor[2];
                                        *(address++) = VICbgColor[2];
                                        break;
                                case 192:
                                        *(address++) = foreColor;
                                        *(address++) = foreColor;
                                        break;
                        }
                        data = data<<2;
                }
        }
}


void drawCharacterOctetSBM(unsigned int x, unsigned int y, byte data, unsigned int color0, unsigned int color1) {
        if(y>=Y_MIN && y<Y_MAX) {
                x-=X_MIN;
                y=y-Y_MIN+100;

                uint16_t* address = gaddress + (y*gwidth+x);
                byte mask = 0b10000000;
                while(mask>0) {
                        *address = (data&mask) ? color1 : color0;
                        address += 1;
                        mask = (mask >>1);
                }
        }
}

void drawCharacterOctetMBM(unsigned int x, unsigned int y, byte data, unsigned int color0, unsigned int color1, unsigned int color2) {
        if(y>=Y_MIN && y<Y_MAX) {
                x-=X_MIN;
                y=y-Y_MIN+100;

                uint16_t* address = gaddress + (y*gwidth+x);
                byte mask = 0b11000000;
                int i;
                for(i=4; i>0; i--) {
                        switch(data&mask) {
                                case 0:
                                        *(address++) = VICbgColor[0];
                                        *(address++) = VICbgColor[0];
                                        break;
                                case 64:
                                        *(address++) = color0;
                                        *(address++) = color0;
                                        break;
                                case 128:
                                        *(address++) = color1;
                                        *(address++) = color1;
                                        break;
                                case 192:
                                        *(address++) = color2;
                                        *(address++) = color2;
                                        break;
                        }
                        data = data<<2;
                }
        }
}


inline void drawBorderOctet(unsigned int x, unsigned int y, unsigned int color) {
        if(y>=Y_MIN && y<Y_MAX) {
                x-=X_MIN;
                y=y-Y_MIN+100;

                uint16_t* address = gaddress + (y*gwidth+x);
                *(address++) = color;
                *(address++) = color;
                *(address++) = color;
                *(address++) = color;
                *(address++) = color;
                *(address++) = color;
                *(address++) = color;
                *(address++) = color;
        }
}


