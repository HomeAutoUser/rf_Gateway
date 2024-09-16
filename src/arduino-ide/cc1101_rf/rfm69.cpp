#include "config.h"

#ifdef RFM69
#include "rfm69.h"
#include "hoymiles.h"
#include "functions.h"
#include "macros.h"

/********************* functions *********************/
void ChipInit() { /* Init RFM69 - Set default´s */
#ifdef debug_chip
  Serial.println(F("[DB] RFM69 init starting"));
#endif
  pinModeFast(SS, OUTPUT);
  ChipDeselect();
  SPI.begin();
  SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0)); // SCLK frequency, burst access max. 10 MHz
  ChipFound = Chip_readReg(CHIP_VERSION, READ_BURST);
  if (ChipFound >= 0x21 && ChipFound <= 0x24) {                 // found SX1231/SX1231H
    SX1231_setIdleMode();
    //ChipFound = true;
#ifdef debug_chip
    delay(10000);
    Serial.print(F("[DB] Chip VERSION SX1231           ")); Serial.println(chipVersion, HEX); // VERSION – Chip ID
    Serial.print(F("[DB] Crystal oscillator frequency  ")); Serial.println(fxOsc);
    Serial.print(F("[DB] Frequency synthesizer step    ")); Serial.println(fStep, 8);
    Serial.print(F("[DB] Chip_available_modes          ")); Serial.println(NUMBER_OF_MODES);  // Number of compiled register modes
    Serial.print(F("[DB] Prog_Ident Firmware1          ")); Serial.println(Prog_Ident1, HEX); // 0xDE
    Serial.print(F("[DB] Prog_Ident Firmware2          ")); Serial.println(Prog_Ident2, HEX); // 0x22
    Serial.println(F("[DB] SX1231 read all 112 register before load new settings"));
    SX1231_read_reg_all();  // SX1231 read all 112 register
#endif

    /* wenn Registerwerte geändert wurden beim compilieren */
    uint16_t chk = 0;
    uint16_t chk_comp = 0;
    for (byte i = 0; i < NUMBER_OF_MODES; i++) {
      chk += pgm_read_word(&(Registers[i].PKTLEN));
    }
    EEPROM.get(EEPROM_ADDR_CHK, chk_comp);

    /* normaler Start */
    if (EEPROMread(EEPROM_ADDR_FW1) == Prog_Ident1 && EEPROMread(EEPROM_ADDR_FW2) == Prog_Ident2 && chk == chk_comp) {  // EEPROM OK, chk OK
      EEPROM.get(EEPROM_ADDR_AFC, freqAfc);         /* afc from EEPROM */
      if (freqAfc > 1) {
        freqAfc = 0;
        EEPROM.write(EEPROM_ADDR_AFC, freqAfc);
#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
        EEPROM.commit();
#endif
      }
      EEPROM.get(EEPROM_ADDR_FOFFSET, freqOffset); /* freq offset from EEPROM */
      if (freqOffset > 10000 || freqOffset < -10000) {
        freqOffset = 0;
        EEPROM.put(EEPROM_ADDR_FOFFSET, freqOffset);
#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
        EEPROM.commit();
#endif
      }
#ifdef debug_chip
      Serial.print(F("[DB] freqAfc                       ")); Serial.println(freqAfc);
      Serial.print(F("[DB] freqOffset                    ")); Serial.print((freqOffset / 1000.0), 3); Serial.println(F(" MHz"));
      Serial.print(F("[DB] ReceiveModeNr                 ")); Serial.println(ReceiveModeNr);
#endif
      if (ReceiveModeNr < NUMBER_OF_MODES) {
        if (ReceiveModeNr == 1 && ToggleCnt == 1) { // use config from EEPROM
#ifdef debug_chip
          String ReceiveModeName = getModeName(ReceiveModeNr);
          Serial.println(F("[DB] SX1231 use config from EEPROM"));
          Serial.print(F("[DB] write ReceiveModeNr ")); Serial.print(ReceiveModeNr);
          Serial.print(F(", ")); Serial.println(ReceiveModeName);
#endif
          // configure SX1231 registers from EEPROM
          for (byte i = 1; i < REGISTER_MAX; i++) { // ACHTUNG! Register müssen immer 84 Einträge umfassen!
            uint8_t addr = i;
            if (i >= 80) {
              addr = SX1231_RegAddrTranslate[i - 80];
            }
#ifdef debug_chip
            Serial.print(F("[DB] write mode value 0x"));
            SerialPrintDecToHex(EEPROMread(i)); //
            Serial.print(F(" to SX1231 register 0x"));
            SerialPrintDecToHex(addr);
            Serial.print(F(" count "));
            Serial.println(i);
#endif
            Chip_writeReg(addr, EEPROMread(i));
          }
        } else {
          Interupt_Variant(ReceiveModeNr); // Empfangsvariante & Register einstellen
        }
      } else { // Ende if (ReceiveModeNr < NUMBER_OF_MODES) {
        SX1231_setIdleMode();        // SX1231 start idle mode
      }
    } else { // Ende if (EEPROMread(EEPROM_ADDR_FW1) == Prog_Ident1 && EEPROMread(EEPROM_ADDR_FW2) == Prog_Ident2 && chk == chk_comp)
      /* ERROR EEPROM oder Registeranzahl geändert */
      EEPROMwrite(EEPROM_ADDR_Prot, 0); // set ReceiveModeNr to Chip factory default
      ToggleCnt = 0;
      ReceiveModeNr = 0;
      //ReceiveModePKTLEN = 0;
      /* wenn Registerwerte geändert wurden beim compilieren */
      if (chk != chk_comp) {  // checksum over PKTLEN not OK
        Serial.println(F("ChipInit, reset togglebank and stop receiving (chk != chk registers.PKTLEN)"));
        EEPROM.put(EEPROM_ADDR_CHK, chk);  // reset
#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
        EEPROM.commit();
#endif
      }
      /* EEPROM wurde gelöscht */
      if (EEPROMread(EEPROM_ADDR_FW1) != Prog_Ident1 || EEPROMread(EEPROM_ADDR_FW2) != Prog_Ident2) {  // EEPROM not OK
        Serial.println(F("ChipInit, EEPROM Init to defaults after ERROR"));
        EEPROMwrite(EEPROM_ADDR_FW1, Prog_Ident1);  // reset Prog_Ident1
        EEPROMwrite(EEPROM_ADDR_FW2, Prog_Ident2);  // reset Prog_Ident2
#ifdef debug_chip
        Serial.println(F("[DB] reconfigure SX1231 and write values to EEPROM"));
#endif
#ifdef CODE_ESP
        memcpy_P(&myArraySRAM, &Registers[0], REGISTER_WR); // ESP
#else
        const uint32_t ptr = pgm_read_ptr(&(Registers[0].regVal)); // Arduino, pointer to register values
#endif
        for (byte i = 0; i <= REGISTER_WR; i++) {                 // write register values ​​to EEPROM
          uint8_t addr = i;
          if (i >= 80) {
            addr = SX1231_RegAddrTranslate[i - 80];
          }
#ifdef debug_chip
          Serial.print(F("[DB] write default value 0x"));
#ifdef CODE_ESP
          SerialPrintDecToHex(pgm_read_byte_near(myArraySRAM.regVal + i)); // ESP write value to EEPROM
#else
          SerialPrintDecToHex(pgm_read_byte_near(ptr + i)); // Arduino write value to EEPROM
#endif
          Serial.print(F(" to EEPROM address "));
          Serial.println(addr);
#endif
#ifdef CODE_ESP
          EEPROMwrite(addr, pgm_read_byte_near(myArraySRAM.regVal + i)); // ESP write value to EEPROM
#else
          EEPROMwrite(addr, pgm_read_byte_near(ptr + i)); // Arduino write value to EEPROM
#endif
        }
      } // Ende EEPROM wurde gelöscht
      if (ReceiveModeNr < NUMBER_OF_MODES) {
        Chip_writeRegFor(0); // write all registers to Chip factory default
      }
#ifdef debug_chip
      Serial.println(F("[DB] set cc110x to factory settings"));
      Serial.print(F("[DB] ReceiveModeNr = "));
      Serial.print(ReceiveModeNr); Serial.print(F(", ")); Serial.println(getModeName(ReceiveModeNr));
      Serial.println(F("[DB] RFM69 remains idle, please set receive mode!"));
#endif
    }      // Ende ERROR EEPROM oder Registeranzahl geändert
  } else { /* NO RFM69 found */
    ChipFound = 0;
#ifdef debug_chip
    Serial.println(F("[DB] SX1231 NOT recognized"));
#endif
  }
}

void Chip_writeRegFor(uint8_t regNr) { // write all registers
#ifdef CODE_ESP
  memcpy_P(&myArraySRAM, &Registers[regNr], REGISTER_WR); // ESP, copy PROGMEM to SRAM
#else
  const uint32_t ptr = pgm_read_ptr(&(Registers[regNr].regVal)); // Arduino, pointer to register values
#endif
  //ReceiveModePKTLEN = pgm_read_word(&(Registers[regNr].PKTLEN));
  //ReceiveModeName = getModeName(regNr);
#ifdef debug_chip
  Serial.print(F("[DB] Chip_writeRegFor register number: ")); Serial.println(regNr);
  Serial.print(F("[DB] Chip_writeRegFor register name:   ")); Serial.println(getModeName(regNr));
  Serial.print(F("[DB] Chip_writeRegFor register count:  ")); Serial.println(REGISTER_WR);
  //Serial.print(F("[DB] Chip_writeRegFor packet length:   ")); Serial.println(ReceiveModePKTLEN);
  char hex[3];
#endif
  for (byte i = 1; i <= REGISTER_WR; i++) {
    if (ReceiveModeNr == 1) { // CC110x User setting
      Chip_writeReg(i, EEPROM.read(i)); /* write value to cc110x */
    } else {
      uint8_t addr = i;
      if (i >= 80) {
        addr = SX1231_RegAddrTranslate[i - 80];
      }
#ifdef CODE_ESP
      Chip_writeReg(addr, pgm_read_byte_near(myArraySRAM.regVal + i)); // ESP write value to SX1231
#else
      Chip_writeReg(addr, pgm_read_byte_near(ptr + i)); // Arduino write value to SX1231
#endif
#ifdef debug_chip
      Serial.print(F("[DB] write register address: "));
      onlyDecToHex2Digit(addr, hex);
      Serial.print(hex);
      Serial.print(F(" value: "));
#ifdef CODE_ESP
      onlyDecToHex2Digit(pgm_read_byte_near(myArraySRAM.regVal + i), hex);
#else
      onlyDecToHex2Digit(pgm_read_byte_near(ptr + i), hex);
#endif
      Serial.print(hex);
      Serial.println("");
#endif
      if (i == 9 && freqOffset != 0) { // Register 0x07 0x08 0x09 - attention to the frequency offset !!!
        byte value[3];
        Chip_setFreq(Chip_readFreq(), value); // add offset to frequency
#ifdef debug_chip
        Serial.print(F("[DB] SX1231 Frequency + offset calculated "));
        Serial.print(((Chip_readFreq() + freqOffset) / 1000), 3);
        Serial.println(F(" MHz and write new value"));
#endif
        for (byte i2 = 0; i2 < 3; i2++) { // write value to frequency register 0x07, 0x08, 0x09
          Chip_writeReg(i2 + 7, value[i2]);  // write new values in SX1231
        }
      }
    }
  }
#if defined (WMBus_S) || defined (WMBus_T) || defined (Inverter_CMT2300A)
  String ReceiveModeName = getModeName(regNr); // name of active mode from array
#endif
#if defined (WMBus_S) || defined (WMBus_T)
  if (ReceiveModeName.startsWith("W")) { // WMBUS
    if (ReceiveModeName.endsWith("S")) { // WMBUS_S
      mbus_init(WMBUS_SMODE); // 1 = WMBUS_SMODE
    }
    if (ReceiveModeName.endsWith("T")) { // WMBUS_T
      mbus_init(WMBUS_TMODE); // 2 = WMBUS_TMODE
    }
  }
#endif
#if defined (Inverter_CMT2300A)
  if (ReceiveModeName.startsWith("H")) { // Hoymiles
    hm_init();
  }
#endif
}

uint8_t Chip_readReg(uint8_t addr, uint8_t regType) {   // SX1231 read register (address) (regType for compatibility with the CC110x)
  regType += 0; // without this becomes warning: unused parameter 'regType'
  ChipSelect();
  SPI.transfer(addr & 0x7F);
  uint8_t result = SPI.transfer(0);
  ChipDeselect();
  return result;
}

void Chip_writeReg(uint8_t regAddr, uint8_t value) {
#ifdef debug_chip
  Serial.print(F("[DB] Chip_writeReg, write value 0x")); SerialPrintDecToHex(value);
  Serial.print(F(" to SX1231 register 0x")); SerialPrintDecToHex(regAddr); Serial.println("");
#endif
  ChipSelect();                                 // Select RFM69
  SPI.transfer(regAddr | 0x80);                 // Send register address
  SPI.transfer(value);                          // Send value
  ChipDeselect();                               // Deselect RFM69
  /* The Frf setting is split across 3 bytes. A change in the center frequency will only be taken into account when the least significant byte FrfLsb in RegFrfLsb is written. */
  if (regAddr == 0x07 || regAddr == 0x08) {     // RegFrfMsb || RegFrfMid
    uint8_t RegFrfLsb = Chip_readReg(0x09, 0);  // RegFrfLsb
    ChipSelect();                               // Select RFM69
    SPI.transfer(0x09 | 0x80);                  // Send RegFrfLsb register address
    SPI.transfer(RegFrfLsb);                    // Send value
    ChipDeselect();                             // Deselect RFM69
  }
}

void SX1231_setTransmitMode() { // start transmit mode
  uint8_t RegOpMode = Chip_readReg(0x01, READ_BURST); // Operating modes of the transceiver
  uint8_t Mode = (RegOpMode & 0b00011100) >> 2;       // Transceiver’s operating modes
  Mode = SX1231_setOperatingMode(3, Mode, RegOpMode); // 3 = Transmitter
#ifdef debug_chip
  Serial.println(F("[DB] SX1231 Set RegOpMode to 0b011 = Transmitter mode (TX)"));
#endif
}

void Chip_setReceiveMode() {    // start receive mode
  uint8_t RegOpMode = Chip_readReg(0x01, READ_BURST); // Operating modes of the transceiver
  uint8_t Mode = (RegOpMode & 0b00011100) >> 2;       // Transceiver’s operating modes
  Mode = SX1231_setOperatingMode(4, Mode, RegOpMode); // 4 = Receiver
#ifdef debug_chip
  Serial.println(F("[DB] SX1231 Set RegOpMode to 0b100 = Receiver mode (RX)"));
#endif
}

void SX1231_setIdleMode() {     // set idle mode
  uint8_t RegOpMode = Chip_readReg(0x01, READ_BURST); // Operating modes of the transceiver
  uint8_t Mode = (RegOpMode & 0b00011100) >> 2;       // Transceiver’s operating modes
  Mode = SX1231_setOperatingMode(1, Mode, RegOpMode); // 1 = Standby mode (STDBY)
#ifdef debug_chip
  Serial.println(F("[DB] SX1231 Set RegOpMode to 0b001 = Standby mode (STDBY)"));
#endif
}

uint8_t SX1231_setOperatingMode(uint8_t ModeNew, uint8_t Mode, uint8_t RegOpMode) {
  if (ModeNew != Mode) {
    RegOpMode = (RegOpMode & 0b11100011) | (ModeNew << 2);
    Chip_writeReg(0x01, RegOpMode);                           // SX1231 write register (High-Byte = address, low-Byte = value)
    uint8_t wait = 0;
    while ((Chip_readReg(0x27, READ_BURST) & 0x80) != 0x80) { // ModeReady - Set when the operation mode requested in Mode, is ready, cleared when changing operating mode.
      delay(1);
      wait++;
      if (wait > 250) {
        break;
      }
    }
    Mode = ModeNew;
#ifdef debug_chip
    Serial.print(F("[DB] new Operating mode: ")); Serial.println(Mode);
    RegOpMode = Chip_readReg(0x01, READ_BURST);               // Operating modes of the transceiver
    Serial.print(F("[DB] SX1231 RegOpMode is 0b")); Serial.println(RegOpMode, BIN);
#endif
  }
  return Mode;
}

void SX1231_afc(uint8_t freqAfc) {                      // AfcAutoOn, 0  AFC is performed each time AfcStart is set, 1  AFC is performed each time Rx mode is entered
  uint8_t RegAfcFei = Chip_readReg(0x1E, READ_BURST);   // read register AFC and FEI control and status
  RegAfcFei = (RegAfcFei & 0b11111011) | freqAfc << 2;
  Chip_writeReg(0x1E, RegAfcFei);                       // SX1231 write register (High-Byte = address, low-Byte = value)
}

int Chip_readRSSI() {   /* Read RSSI value from Register */
  uint8_t rssiRaw = Chip_readReg(0x24, READ_BURST); // not converted
#if defined(CODE_ESP) || !defined(SIGNALduino_comp)
  RSSI_dez = rssiRaw / -2;                          // SX1231 RSSI for website
#endif
  int16_t RSSI_raw = 0;
  if (rssiRaw >= 21 && rssiRaw < 149) {
    RSSI_raw = rssiRaw - 148;
  } else {
    RSSI_raw = rssiRaw - 404;
  }
  RSSI_raw *= -1;       // SX1231 RSSI for FHEM

#ifdef SIGNALduino_comp /* for FHEM */
  return RSSI_raw;      // not converted for FHEM
#else
  return RSSI_dez;      // calculated for output on WebSocket_raw and msg
#endif
}

void Chip_readBurstReg(byte *uiBuffer, byte regAddr, byte len) {  // Read values in array (FiFo)
  ChipSelect();
  SPI.transfer(regAddr & 0x7F);           // read address
  for (uint8_t ui = 0; ui < len; ui++) {
    uiBuffer[ui] = SPI.transfer(0x00);
  }
  ChipDeselect();
}

void SX1231_read_reg_all() {        // SX1231 read all 112 register
  Serial.println(F("------------------------------------------------------------"));
  Serial.println(F("SX1231 read all 112 register"));
  int i = 0;
  int j = 0;
  byte b = 0;
  char buf[7];
  Serial.print(F("Addr. "));
  for (i = 0; i < 16; i++) {
    sprintf(buf, "%02X ", i);
    Serial.print(buf);
  }
  Serial.println();
  Serial.println(F("------------------------------------------------------------"));

  ChipSelect();
  SPI.transfer(0x01);               // SX1231 Read Start Address of 112 register
  for (i = 0x00; i <= 0x71; i++) {
    if (j == 0) {
      sprintf(buf, "%04X: ", i);
      Serial.print(buf);
    }
    j++;
    if (i == 0) {                   // Address 0x00 is FIFO address
      Serial.print("   ");
    } else {
      b = SPI.transfer(0x00);       // read 1 byte
      sprintf(buf, "%02X ", b);
      if (j == 16) {
        j = 0;
        Serial.println(buf);
      } else {
        Serial.print(buf);
      }
    }
  }
  ChipDeselect();
  Serial.println();
}

float Chip_readFreq() {
  float Freq;
  Freq = Chip_readReg(0x07, 0) * 256;           // read Value RegFrfMsb - RF Carrier Frequency, Most Significant Bits
  Freq = (Freq + Chip_readReg(0x08, 0)) * 256;  // read Value RegFrfMid - RF Carrier Frequency, Intermediate Bits
  Freq = Freq + Chip_readReg(0x09, 0) ;         // read Value RegFrfLsb - RF Carrier Frequency, Least Significant Bits
  Freq = (Freq * fStep) / 1000 + 0.5;           // round RF Carrier Frequency in kHz
  return Freq;
}

void Chip_setFreq(uint32_t frequency, byte * arr) {   // frequency set & calculation -  function used in Chip_writeRegFor
  uint32_t f;
  f = (frequency + freqOffset) * 1000 / fStep;
  arr[0] = f / 65536;
  arr[1] = (f % 65536) / 256;
  arr[2] = f % 256;
#if defined(debug) || defined(debug_html) || defined(debug_chip)
  Serial.print(F("[DB] web_Freq_Set, input  ")); Serial.println(frequency);
  char chHex[3]; // for hex output
  onlyDecToHex2Digit(arr[0], chHex);
  Serial.print(F("[DB] web_Freq_Set, output "));  Serial.print(chHex);
  onlyDecToHex2Digit(arr[1], chHex);
  Serial.print(' '); Serial.print(chHex);
  onlyDecToHex2Digit(arr[2], chHex);
  Serial.print(' '); Serial.println(chHex);
#endif
}

#if defined (ARDUINO_ARCH_ESP8266) || defined (ARDUINO_ARCH_ESP32)
byte Chip_Bandw_cal(float input) {   /* bandwidth calculation from web */
  int bits = 0;
  int bw = 0;
  uint8_t RegDataModul = Chip_readReg(0x02, 0);               // Data operation mode and Modulation settings
  uint8_t ModulationType = (RegDataModul & 0b00011000) >> 3;  // Modulation scheme (FSK / OOK)
  for (int e = 0; e < 8; e++) {
    for (int m = 0; m < 3; m++) {
      bits = (e) + (m << 3);
      bw = (fxOsc / ((16 + m * 4) * (pow(2, e + 2 + ModulationType))) / 1000);
      if (input >= bw) {
        goto END;
      }
    }
  }
END:
#ifdef debug
  char hex[3];
  onlyDecToHex2Digit(((Chip_readReg(CHIP_RxBw, READ_BURST) & 0b11100000) + bits), hex);
  Serial.print(F("[DB] Chip_Bandw_cal, Setting RegRxBw (0x19) to ")); Serial.println(hex);
#endif
  return ((Chip_readReg(CHIP_RxBw, READ_BURST) & 0b11100000) + bits);
}
#endif

void Chip_Datarate_Set(long datarate, byte * arr) { // calculate register values (RegBitrateMsb 0x03, RegBitrateLsb 0x04) for datarate
  if (datarate < 600) {
    datarate = 600;
  } else if (datarate > 603773) {
    datarate = 603773;
  }
  float fBitrate = fxOsc / datarate;
  uint16_t uBitrate = round(fBitrate);
  arr[0] = uBitrate >> 8;     // RegBitrateMsb
  arr[1] = uBitrate & 0x00FF; // RegBitrateLsb
}

void SX1231_Deviation_Set(float deviation, byte * arr) { // calculate register values (RegFdevMsb 0x05, RegFdevLsb 0x06) for frequency deviation
  deviation *= 1000.0;
  if (deviation < 61) {
    deviation = 61;
  } else if (deviation > 299988) {
    deviation = 299988;
  }
  float fFdev = deviation / fStep;
  uint16_t uFdev = round(fFdev);
  arr[0] = uFdev >> 8;      // RegFdevMsb
  arr[1] = uFdev & 0x00FF;  // RegFdevLsb
}

void Chip_sendFIFO(char *data) {
#ifdef debug_chip
  Serial.println(F("[DB] Chip_sendFIFO"));
#endif
  SX1231_setIdleMode(); // SX1231 start idle mode
  ChipSelect();
  SPI.transfer(0x80);   // FIFO write address
  uint8_t val;
  for (uint8_t i = 0; i < strlen(data); i += 2) {
    val = hex2int(data[i]) * 16;
    val += hex2int(data[i + 1]);
    SPI.transfer(val);  // fill FIFO with data
  }
  ChipDeselect();
  SX1231_setTransmitMode(); // SX1231 start transmit mode
  while ((Chip_readReg(0x28, 0) & 0x08) == 0x00) { // wait for PacketSent in RegIrqFlags2
    delay(1);
  }
  SX1231_setIdleMode();     // SX1231 start idle mode
}

#endif // RFM69
