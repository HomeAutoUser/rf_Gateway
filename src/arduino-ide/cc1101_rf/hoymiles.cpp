#include "config.h"
#ifdef Inverter_CMT2300A
#include "hoymiles.h"

#if defined (WMBus_S) || defined (WMBus_T)
extern RXinfoDescr RXinfo;
//extern TXinfoDescr TXinfo;
#endif

#if !defined(WMBus_S) && !defined(WMBus_T)
uint8_t MBpacket[32]; // Ausgabepuffer - Größe prüfen!
uint8_t MBbytes[64];  // Empfangspuffer - Größe prüfen!
RXinfoDescr RXinfo;
//TXinfoDescr TXinfo;
#endif

void hm_init() {
  memset( &RXinfo, 0, sizeof( RXinfo ));
#ifdef debug_hoymiles
  Serial.println(F("hm_init"));
#endif
}

static uint8_t hm_on(uint8_t force) {
  // already in RX?
#ifdef CC110x
  if (!force && Chip_readReg(CC110x_MARCSTATE, READ_BURST) == MARCSTATE_RX)
    return 0;
#elif RFM69
  if (!force && (Chip_readReg(0x01, READ_BURST) & 0b00011100) >> 2 == 0x04) // 4 = Receiver
    return 0;
#endif
  // init RX here, each time we're idle
  RXinfo.state = 0;
#ifdef CC110x
  CC110x_CmdStrobe(CC110x_SIDLE);
  CC110x_CmdStrobe(CC110x_SNOP);
  CC110x_CmdStrobe(CC110x_SFRX);
#elif RFM69
  Chip_writeReg(0x28, 0x10); // FIFO are cleared when this bit is set.
#endif

  // Initialize RX info variable
  RXinfo.lengthField = 0;           // Length Field in the wireless packet
  RXinfo.length      = 0;           // Total length of bytes to receive packet
  RXinfo.bytesLeft   = 0;           // Bytes left to to be read from the RX FIFO
  RXinfo.pByteIndex  = MBbytes;     // Pointer to current position in the byte array
  RXinfo.format      = INFINITE;    // Infinite or fixed packet mode
  //  RXinfo.mode        = mbus_mode;   // Wireless MBUS radio mode
  //  RXinfo.framemode   = WMBUS_NONE;  // Received frame mode (Distinguish between C- and T-mode)
  //  RXinfo.frametype   = 0;           // Frame A or B in C-mode

#ifdef CC110x
  Chip_writeReg(CC110x_FIFOTHR, 0x42); // Set RX FIFO threshold to 12 bytes
  Chip_writeReg(CC110x_PKTCTRL0, 0x02); // Set infinite length
#elif RFM69
  Chip_writeReg(0x3C, 0x06); // Set RX FIFO threshold to 6 bytes
  Chip_writeReg(0x37, 0x00); // RegPacketConfig1, Unlimited length packet format is selected when bit PacketFormat is set to 0 and PayloadLength is set to 0.
  Chip_writeReg(0x38, 0x00); // RegPayloadLength, Unlimited length packet format is selected when bit PacketFormat is set to 0 and PayloadLength is set to 0.
#endif

  Chip_setReceiveMode();
  RXinfo.state = 1;
#ifdef debug_hoymiles
  Serial.println(F("hm_on "));
#endif
  return 1; // this will indicate we just have re-started RX
}

void hm_task() {
  switch (RXinfo.state) {
    case 0:
      hm_on(TRUE);
      return;
    // RX active, awaiting SYNC
    case 1:
#ifdef CC110x
      if (digitalReadFast(GDO2) == 1) { // PIN_RECEIVE, Asserts when sync word has been sent / received, and de-asserts at the end of the packet.
#elif RFM69
      if (Chip_readReg(0x27, 0) & 0b00000001) { // SyncAddressMatch, Set when Sync and Address (if enabled) are detected. Cleared when leaving Rx or FIFO is emptied.
#endif
#ifdef debug_hoymiles
        Serial.println(F("hmt1 sync"));
#endif
        RXinfo.state = 2;
      }
      break;
    // awaiting pkt len to read
    case 2:
#ifdef CC110x
      if (digitalReadFast(GDO0) == 1) { // PIN_SEND, Associated to the RX FIFO: Asserts when RX FIFO is filled at or above the RX FIFO threshold. De-asserts when RX FIFO is drained below the same threshold.
        Chip_readRXFIFO(RXinfo.pByteIndex, 12, NULL, NULL); // Read the first 12 bytes from FIFO
        if (MBbytes[0] != 0x5A || MBbytes[1] != 0x48) { // check sync word byte 2 and 3
#ifdef debug_hoymiles
          Serial.println(F("hmt2 ERROR SYNC"));
#endif
          RXinfo.state = 0;
          return;
        } else { // sync word OK
          rssi = Chip_readRSSI();
          for (uint8_t b = 0; b < 12; b++) {
            MBbytes[b] = MBbytes[b + 2]; // remove sync word
          }
        }
#elif RFM69
      if (Chip_readReg(0x28, 0) & 0b00100000) { // RegIrqFlags2, FifoLevel - Set when the number of bytes in the FIFO strictly exceeds FifoThreshold, else cleared.
        Chip_readBurstReg(RXinfo.pByteIndex, CHIP_RXFIFO, 7); // Read the first 7 bytes from FIFO
        rssi = Chip_readRSSI();
#endif // END chip
#ifdef debug_hoymiles
        char chHex[3];
        Serial.print(F("hmt2 RX "));
#ifdef CC110x
        for (uint8_t x = 0; x < 10; x++) {
#elif RFM69
        for (uint8_t x = 0; x < 7; x++) {
#endif
          onlyDecToHex2Digit(RXinfo.pByteIndex[x], chHex);
          Serial.print(chHex); Serial.print(' ');
        }
        Serial.println("");
#endif
        // Calculate the total number of bytes to receive
        uint8_t bytesDecoded[4];
        uint8_t len = decode4out7(RXinfo.pByteIndex, bytesDecoded, 7);
        if (len == 0) {
#ifdef debug_hoymiles
          Serial.println(F("hmt2 ERROR FEC"));
#endif
          RXinfo.state = 0;
          return;
        }
        RXinfo.lengthField = bytesDecoded[0];
        RXinfo.length = ((RXinfo.lengthField + 3) * 7 / 4) + 1;
        // check if incoming data will fit into buffer
        if (RXinfo.length > sizeof(MBbytes)) {
#ifdef debug_hoymiles
          Serial.print(F("hmt2 ERROR lenght too big ")); Serial.println(RXinfo.length);
#endif
          RXinfo.state = 0;
          return;
        }
#ifdef CC110x
        RXinfo.pByteIndex += 10;
        RXinfo.bytesLeft = RXinfo.length - 10;
        Chip_writeReg(CHIP_PKTLEN, (uint8_t)(RXinfo.length + 2)); // Length + 2 due to 2 byte sync word
        Chip_writeReg(CC110x_PKTCTRL0, FIXED_PACKET_LENGTH); // 0x08: PKTCTRL0 – Packet Automation Control, 0x00
#elif RFM69
        RXinfo.pByteIndex += 7;
        RXinfo.bytesLeft = RXinfo.length - 7;
#endif


#ifdef debug_hoymiles
        Serial.print(F("hmt2 decode "));
        for (uint8_t x = 0; x < 4; x++) {
          onlyDecToHex2Digit(bytesDecoded[x], chHex);
          Serial.print(chHex); Serial.print(' ');
        }
        Serial.println("");
        Serial.print(F("hmt2 len1 ")); Serial.println(RXinfo.lengthField);
        Serial.print(F("hmt2 len2 ")); Serial.println(RXinfo.length);
        Serial.print(F("hmt2 len3 ")); Serial.println(RXinfo.bytesLeft);
#endif
        RXinfo.state = 3;
      }
      break;
    // awaiting more data to be read
    case 3:
#ifdef CC110x
      if (digitalReadFast(GDO2) == 0) { // PIN_RECEIVE, Asserts when sync word has been sent / received, and de-asserts at the end of the packet.
        Chip_readBurstReg(RXinfo.pByteIndex, CHIP_RXFIFO, RXinfo.bytesLeft); // Read remaining bytes from FIFO
        CC110x_readFreqErr();
        RXinfo.state = 4; // decode!
#ifdef debug_hoymiles
        Serial.println(F("hmt3 END OF PAKET"));
#endif
      }
#elif RFM69 // Ende CC110x
      if (Chip_readReg(0x28, 0) & 0b01000000) { // RegIrqFlags2, FifoNotEmpty - Set when FIFO contains at least one byte, else cleared.
        while (Chip_readReg(0x28, 0) & 0b01000000) { // RegIrqFlags2, FifoNotEmpty - Set when FIFO contains at least one byte, else cleared.
          Chip_readBurstReg(RXinfo.pByteIndex, CHIP_RXFIFO, 1); // Read 1 byte from FIFO
          RXinfo.bytesLeft--;
          RXinfo.pByteIndex ++;
          if (RXinfo.bytesLeft == 0) {
            RXinfo.state = 4;
#ifdef debug_hoymiles
            Serial.println(F("hmt3 END OF PAKET"));
#endif
            break;
          }
        }
      }
#endif
#ifdef debug_hoymiles
      if (RXinfo.state == 4) {
        char chHex[3];
        Serial.print(F("hmt3 RX "));
        for (uint8_t x = 0; x < RXinfo.length; x++) {
          onlyDecToHex2Digit(MBbytes[x], chHex);
          Serial.print(chHex); Serial.print(' ');
        }
        Serial.println("");
      }
#endif
      break;
    // decode
    case 4:
      uint8_t len = decode4out7(MBbytes, MBpacket, RXinfo.length);
      if (len == 0) {
#ifdef debug_hoymiles
        Serial.println(F("hmt4 ERROR FEC"));
#endif
        RXinfo.state = 0;
        return;
      }
      // remove first byte from msg (lengthField)
#ifdef debug_hoymiles
      Serial.print(F("hmt4 msg "));
#endif
      for (uint8_t b = 0; b < RXinfo.lengthField; b++) {
        MBpacket[b] = MBpacket[b + 1];
#ifdef debug_hoymiles
        char chHex[3];
        onlyDecToHex2Digit(MBpacket[b], chHex);
        Serial.print(chHex);
        Serial.print(" ");
#endif
      }
#ifdef debug_hoymiles
      Serial.println("");
#endif
      // check CRC8
      uint8_t crc = crc8(MBpacket, RXinfo.lengthField - 1);
      if (crc != MBpacket[RXinfo.lengthField - 1]) { // Error CRC8
#ifdef debug_hoymiles
        Serial.println(F("hmt4 ERROR CRC8"));
#endif
        RXinfo.state = 0;
        return;
      }
      // Output Message
      msgOutput_MN(MBpacket, RXinfo.lengthField - 1, 0, 0, rssi, freqErr); // MN - Nachricht erstellen und ausgeben
      RXinfo.state = 0;
      return;
  }
}

uint8_t decode4out7(uint8_t *encodedData, uint8_t *decodedData, uint8_t lastByte) {
  uint8_t nib[8] = {};
  uint8_t fec[8] = {};
  uint8_t b = 0;
  uint8_t len = 0;
  uint8_t fecIndex = 0;
  while (b <= lastByte - 7) {
    nib[0] = encodedData[b] & 0xF0;
    fec[0] = (encodedData[b] & 0b00001110) >> 1;
    nib[1] = (encodedData[b] << 3 | encodedData[b + 1] >> 5) & 0x0F;
    fec[1] = (encodedData[b + 1] & 0b00011100) >> 2;
    nib[2] = (encodedData[b + 1] << 6 | encodedData[b + 2] >> 2) & 0xF0;
    fec[2] = (encodedData[b + 2] & 0b00111000) >> 3;
    nib[3] = (encodedData[b + 2] << 1 | encodedData[b + 3] >> 7) & 0x0F;
    fec[3] = (encodedData[b + 3] & 0b01110000) >> 4;
    nib[4] = encodedData[b + 3] << 4;
    fec[4] = encodedData[b + 4] >> 5;
    nib[5] = (encodedData[b + 4] >> 1) & 0x0F;
    fec[5] = (encodedData[b + 4] & 0b00000001) << 2 | encodedData[b + 5] >> 6;
    nib[6] = (encodedData[b + 5] << 2) & 0xF0;
    fec[6] = (encodedData[b + 5] & 0b00000011) << 1 | encodedData[b + 6] >> 7;
    nib[7] = (encodedData[b + 6]  >> 3) & 0x0F;
    fec[7] = (encodedData[b + 6] & 0b00000111);
    for (uint8_t c = 0; c < 8; c++) {
      fecIndex = nib[c];
      if ((c & 0x01) == 0) { // MSB
        fecIndex = fecIndex >> 4;
      }
      if (fec[c] != HM_FEC[fecIndex]) { // ERROR FEC
        return 0;
      }
      if ((c & 0x01) == 0) { // MSB
        decodedData[len] = nib[c] | nib[c + 1]; // Byte übernehmen
        len ++;
      }
    }
    b += 7;
  }
  return len;
}

/*
  |<------------- CRC8 ------------>|
  00   71 60 35 46   80 12 23 04   00 00   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
       ^^^^^^^^^^^   ^^^^^^^^^^^      ^^
       Target Addr   Source Addr      CRC8
*/
uint8_t crc8(const uint8_t *buf, const uint8_t len) {
  uint8_t crc = CRC8_INIT;
  for (uint8_t i = 0; i < len; i++) {
    crc ^= buf[i];
    for (uint8_t b = 0; b < 8; b++) {
      crc = (crc << 1) ^ ((crc & 0x80) ? CRC8_POLY : 0x00);
    }
  }
  return crc;
}

#endif
