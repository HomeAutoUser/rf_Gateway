#include "config.h"
#include "macros.h"


/* output DEC to HEX with a leading zero to serial without ln */
void SerialPrintDecToHex(uint8_t dec) {
  if (dec < 16) {
    Serial.print('0');
  }
  Serial.print(dec, HEX);
}
