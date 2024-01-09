#pragma once

#ifdef RFM69
/*
    This file provides us with important information about the functional prototypes available to us,
    provides us with symbolic constants and keeps a list of all available macros.
*/

#include <Arduino.h>
#include <digitalWriteFast.h>           // https://github.com/ArminJo/digitalWriteFast
#include <EEPROM.h>
#include <SPI.h>


void ChipInit();

extern boolean ChipFound;

#endif
