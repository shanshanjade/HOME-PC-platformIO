#ifndef _CONFIG_H
#define _CONFIG_H
#include <Arduino.h>
#include <PxMatrix.h>

#define P_LAT 22
#define P_A 19
#define P_B 23
#define P_C 18
#define P_D 5
#define P_E 15
#define P_OE 16

#define MATRIX_WIDTH 64
#define MATRIX_HEIGHT 32

PxMATRIX display(64, 32, P_LAT, P_OE, P_A, P_B, P_C, P_D);
String serial_read;

#endif
