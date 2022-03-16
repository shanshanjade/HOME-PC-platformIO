#ifndef _CONFIG_H
#define _CONFIG_H
#include <Arduino.h>
#include <PxMatrix.h>

#define P_LAT 16
#define P_A 5
#define P_B 4
#define P_C 15
#define P_D 12
#define P_E 0
#define P_OE 2

#define MATRIX_WIDTH 64
#define MATRIX_HEIGHT 32

PxMATRIX display(64, 32, P_LAT, P_OE, P_A, P_B, P_C, P_D);
String serial_read;

#endif
