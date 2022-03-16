#ifndef config_h
#define config_h
#include <Adafruit_GFX.h>

#define P_LAT 16
#define P_A 5
#define P_B 4
#define P_C 15
#define P_D 12
#define P_E 0
#define P_OE 2

#define matrix_width 64
#define matrix_height 32

PxMATRIX display(64, 32, P_LAT, P_OE, P_A, P_B, P_C, P_D);

#endif
