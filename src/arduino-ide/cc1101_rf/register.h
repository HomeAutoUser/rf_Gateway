#pragma once
#include <Arduino.h>

/*
    This file provides us with important information about the functional prototypes available to us,
    provides us with symbolic constants and keeps a list of all available macros.
*/

struct Data {
  const uint8_t* reg_val;
  byte length;
  const char* name;
  byte PKTLEN;
};

extern String ReceiveModeName;
extern float Freq_offset;
extern struct Data Registers[];
extern uint8_t RegistersMaxCnt;
extern uint8_t ToggleArray[];
