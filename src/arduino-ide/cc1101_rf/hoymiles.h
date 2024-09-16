/*
  TX packet:
  4D485A4801FD7348C8DA3400B8017972E5CB972E5CB005C6AE2E34B801A8EE0D97734340
          01FD7348C8DA34
*/
#ifndef HOYMILES_H
#define HOYMILES_H

#ifdef Inverter_CMT2300A

#ifdef CC110x
#include "cc110x.h"
#elif RFM69
#include "rfm69.h"
#endif

#if defined (WMBus_S) || defined (WMBus_T)
#include "mbus.h"
extern uint8_t MBpacket[291];
extern uint8_t MBbytes[584];
#endif

#if !defined(WMBus_S) && !defined(WMBus_T)
#include "functions.h"

typedef struct RXinfoDescr {
  uint8_t  lengthField;         // The L-field in the WMBUS packet
  uint16_t length;              // Total number of bytes to to be read from the RX FIFO
  uint16_t bytesLeft;           // Bytes left to to be read from the RX FIFO
  uint8_t *pByteIndex;          // Pointer to current position in the byte array
  uint8_t format;               // Infinite or fixed packet mode
  // uint8_t mode;                 // S-mode or T-mode
  // uint8_t framemode;            // C-mode or T-mode frame
  // uint8_t frametype;            // Frame type A or B when in C-mode
  uint8_t state;
} RXinfoDescr;

#define INFINITE   0
#define TRUE       1

extern int8_t freqErr;                              // CC110x automatic Frequency Synthesizer Control
extern uint8_t rssi;

extern void msgOutput_MN(uint8_t * data, uint16_t lenData, uint8_t wmbusFrameTypeA, uint8_t lqi, uint8_t rssi, int8_t freqErr);
#endif // END #if !defined(WMBus_S) && !defined(WMBus_T)

#define CRC8_INIT   0x00
#define CRC8_POLY   0x01

const uint8_t HM_FEC[16] = {0, 5, 7, 2, 3, 6, 4, 1, 6, 3, 1, 4, 5, 0, 2, 7};

void hm_init();
void hm_task();
//void decode4out7(uint8_t *encodedData, uint8_t *decodedData, uint8_t lastByte);
uint8_t decode4out7(uint8_t *encodedData, uint8_t *decodedData, uint8_t lastByte);
uint8_t crc8(const uint8_t *buf, const uint8_t len);

#endif
#endif
