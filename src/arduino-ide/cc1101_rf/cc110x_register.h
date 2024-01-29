#ifndef CC110X_REGISTER_H
#define CC110X_REGISTER_H

#include <Arduino.h>

#ifdef CC110x
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
#endif  // END - #ifdef CC110x
#endif  // END - #ifndef CC110X_REGISTER_H
