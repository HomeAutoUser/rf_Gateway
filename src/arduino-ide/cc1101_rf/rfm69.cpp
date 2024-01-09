#include "config.h"

#ifdef RFM69
#include "rfm69.h"
#include "functions.h"
#include "macros.h"


/*********************
  variables for RFM69
 *********************/
byte ReceiveModeNr = 0;                     // activated protocol in flash
boolean ChipFound = false;                      // against not clearly defined entrances (if flicker)

/***********
  functions
 ***********/
void ChipInit() { /* Init RFM69 - Set default´s */
}

#endif
