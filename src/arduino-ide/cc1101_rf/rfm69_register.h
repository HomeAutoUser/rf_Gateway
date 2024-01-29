#ifndef RFM69_REGISTER_H
#define RFM69_REGISTER_H

#ifdef RFM69
#include <Arduino.h>
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
#endif  // END - #ifdef RFM69
#endif  // END - #ifndef RFM69_REGISTER_H
