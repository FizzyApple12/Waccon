#ifndef _CUSTOM_SERIAL_
#define _CUSTOM_SERIAL_

#include "pico.h"

void setup();

void update();

void write(uint8_t data);

void sendRemaining();

uint8_t read();

#endif